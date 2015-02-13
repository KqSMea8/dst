/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    pn_client_status.cc
* @author:      zhanggengxin@baidu.com
* @brief:       pn staus
*/

#include "processmaster/pn_client_status.h"
#include "boost/algorithm/string/trim.hpp"
#include "common/error.h"
#include "common/id.h"
#include "common/proto/pm.pb.h"
#include "metamanager/meta_manager.h"
#include "processmaster/app_status.h"
#include "processmaster/dstream_event.h"
#include "processmaster/event_manager.h"
#include "processmaster/kill_app_gc_task.h"
#include "processmaster/pm_common.h"
#include "pb_to_json.h"


namespace dstream {
namespace pn_client_status {
using namespace application_tools;
using namespace dstream_event;
using namespace event_manager;
using namespace meta_manager;
using namespace gc_task;
using namespace kill_app_gc_task;
using namespace pm_common;
using namespace pm_gc;
using namespace scheduler;

PNClientStatus* PNClientStatus::pn_status_instace = NULL;

int32_t PNClientStatus::InitPNStatus(EventManager* event_manager, GC* gc,
                                     Scheduler* scheduler) {
    if (NULL == event_manager || NULL == gc || NULL == scheduler) {
        DSTREAM_WARN("init pn status fail");
        return error::INVALID_POINTER;
    }
    if (NULL == pn_status_instace) {
        pn_status_instace = new PNClientStatus(event_manager, gc, scheduler);
    } else {
        pn_status_instace->ClearAll();
        pn_status_instace->m_event_manager = event_manager;
        pn_status_instace->m_gc = gc;
        pn_status_instace->m_scheduler = scheduler;
    }
    return pn_status_instace->InitMap();
}

PNClientStatus* PNClientStatus::GetPNClientStatus() {
    return pn_status_instace;
}

void PNClientStatus::DestroyPNStatus() {
    if (NULL != pn_status_instace) {
        delete pn_status_instace;
        pn_status_instace = NULL;
    }
}

int32_t PNClientStatus::UpdatePNStatus(const PNReport* report) {
    MutexLockGuard mutext_guard(m_lock);
    int res = error::OK;
    PnidReportMapIter pn_find = m_pn_reports.find(report->id());
    if (pn_find == m_pn_reports.end()) {
        if (!AddPNReport(report)) {
            DSTREAM_WARN("add pn report fail");
            return error::ADD_PN_REPORT_FAIL;
        }
    } else {
        pn_find->second = *report;
        DSTREAM_DEBUG("pn [%s] has [%d] pe", pn_find->second.id().id().c_str(),
                     pn_find->second.status_size());
    }

    // check pe revision
    // TODO(fangjun02) return ?
    app_status::AppStatus* app_status = app_status::AppStatus::GetAppStatus();
    unsigned int zk_revision;
    int backup_pe_size = report->status_size();
    for (int i = 0; i < backup_pe_size; i++) {
        const BackupPEStatus& pe_status = report->status(i);
        unsigned int report_revision = pe_status.revision();
        DSTREAM_DEBUG("pn [%s] report pe [%lu] with revision [%u]", report->id().id().c_str(),
                      pe_status.pe_id().id(),
                      report_revision);
        if (app_status->GetProcessorRevision(EXTRACT_APPPRO_ID(pe_status.pe_id().id()),
                                             &zk_revision) != error::OK) {
            DSTREAM_WARN("check pe [%ld]'s revision failed", pe_status.pe_id().id());
        } else {
            if (report_revision < zk_revision) {
                RestartPEEvent* restart_event = new RestartPEEvent(pe_status.app_id(),
                        pe_status.pe_id(),
                        pe_status.backup_pd_id(),
                        report->id(),
                        zk_revision);
                if (!m_event_manager->AddEvent(EventPtr(restart_event))) {
                    DSTREAM_WARN("add pn [%s] pe [%lu] restart event fail",
                                 report->id().id().c_str(), pe_status.pe_id().id());
                    res = error::ADD_PN_REPORT_FAIL;
                }
            }
        }
    }

    int fail_size = report->fails_size();
    if (fail_size == 0) {
        DSTREAM_DEBUG("pn [%s] report no fail pe", report->id().id().c_str());
        return res;
    }
    DSTREAM_WARN("[%s] pn [%s] report [%d] fail pe", __FUNCTION__,
                 report->id().id().c_str(),
                 fail_size);
    MetaManager* meta = MetaManager::GetMetaManager();
    ProcessorElement pe;
    error::Code meta_res = error::OK;
    for (int i = 0; i < fail_size; ++i) {
        const FailPE& fail_pe = report->fails(i);
        if ((meta_res = meta->GetProcessorElement(fail_pe.app_id(), fail_pe.pe_id(),
                                                  &pe)) != error::OK) {
            if (meta_res == error::ZK_NO_NODE) {
                continue;
            }
            res = error::ADD_PN_REPORT_FAIL;
            DSTREAM_WARN("get app [%lu] pe [%lu] fail", fail_pe.app_id().id(),
                         fail_pe.pe_id().id());
            continue;
        }
        DSTREAM_INFO("pn [%s] report fail pe [%lu]", report->id().id().c_str(),
                     fail_pe.pe_id().id());
        PEFailEvent* fail_event = new PEFailEvent(fail_pe.app_id(), fail_pe.pe_id(),
                                                  pe.resource(), fail_pe.backup_pe_id(),
                                                  report->id());
        if (!m_event_manager->AddEvent(EventPtr(fail_event))) {
            DSTREAM_WARN("add pn [%s] pe [%lu] fail event fail", report->id().id().c_str(),
                         fail_pe.pe_id().id());
            res = error::ADD_PN_REPORT_FAIL;
        }
    }
    return res;
}

bool PNClientStatus::AddPNReport(const PNReport* report) {
    MetaManager* meta = MetaManager::GetMetaManager();
    // check pe list diff
    PeSet pe_set, pn_report_pe_set, fail_pe_set;
    PnidPeSetMapIter pn_pe_iter;
    if ((pn_pe_iter = m_pn_pe_map.find(report->id())) == m_pn_pe_map.end()) {
        DSTREAM_WARN("[%s] Get PN [%s] fail", __FUNCTION__,
                     report->id().id().c_str());
        PnidPeSetMapPair pn_pe_pair(report->id(), pe_set);
        m_pn_pe_map.insert(pn_pe_pair);
    } else {
        pe_set = pn_pe_iter->second;
    }
    //  if (meta->GetPNProcessElements(report->id(), pe_set) != error::OK) {
    //    DSTREAM_WARN("get pn [%s]'s pes fail", report->id().id().c_str());
    //    return false;
    //  }
    //  PN pn;
    //  if (meta->GetPN(report->id(), pn) != error::OK) {
    //    DSTREAM_WARN("[%s] Get PN [%s] fail", __FUNCTION__,
    //                                          report->id().id().c_str());
    //    return false;
    //  }
    ProcessorElement pe;
    for (int i = 0; i < report->status_size(); ++i) {
        const BackupPEStatus& pe_status = report->status(i);
        *(pe.mutable_app()) = pe_status.app_id();
        *(pe.mutable_pe_id()) = pe_status.pe_id();
        pn_report_pe_set.insert(pe);
    }
    for (int i = 0; i < report->fails_size(); ++i) {
        const FailPE& fail_pe = report->fails(i);
        *(pe.mutable_app()) = fail_pe.app_id();
        *(pe.mutable_pe_id()) = fail_pe.pe_id();
        fail_pe_set.insert(pe);
    }
    DSTREAM_DEBUG("pn [%s] report pe number [%zd], fail pe number [%zd], zk pe number [%zd]",
                  report->id().id().c_str(), pn_report_pe_set.size(),
                  fail_pe_set.size(), pe_set.size());
    PeSetIter pe_iter, pe_end, report_end;
    PeSetIter fail_pe_end = fail_pe_set.end();
    report_end = pn_report_pe_set.end();
    list<Resource> used_resource_list;
    for (pe_iter = pe_set.begin(), pe_end = pe_set.end();
         pe_iter != pe_end; ++pe_iter) {
        // we do ont handle fail pe here, gc thread will handle this
        if (fail_pe_set.find(*pe_iter) == fail_pe_end) {
            int backup_pe_size = pe_iter->backups_size();
            for (int i = 0; i < backup_pe_size; ++i) {
                const BackupPE& backup_pe = pe_iter->backups(i);
                DSTREAM_DEBUG("backup pe assignment pn [%s]", backup_pe.pn_id().id().c_str());
                // if a pe has already assigned on a specified pn but this pn do not report it
                // we assume it as a fail event
                if (backup_pe.pn_id().id() == report->id().id() &&
                    pn_report_pe_set.find(*pe_iter) == report_end) {
                    DSTREAM_INFO("PE [%lu] fail on pn:[%s] ",
                                 pe_iter->pe_id().id(),
                                 report->id().id().c_str());
                    m_event_manager->AddEvent(EventPtr(new PEFailEvent(pe_iter->app(),
                                                                       pe_iter->pe_id(),
                                                                       pe_iter->resource(),
                                                                       backup_pe.backup_pe_id(),
                                                                       backup_pe.pn_id())));
                    break;
                // else we calculate the resource already used on this pn
                } else {
                    DSTREAM_DEBUG("[%s] pn [%s] report pe [%lu] backup pe [%lu]",
                                  __FUNCTION__, report->id().id().c_str(),
                                  pe_iter->pe_id().id(),
                                  backup_pe.backup_pe_id().id());
                    int32_t res = error::OK;
                    uint64_t id = app_id(pe_iter->pe_id());
                    AppID pe_app_id;
                    pe_app_id.set_id(id);
                    ProcessorElement report_pe;
                    res = meta->GetProcessorElement(pe_app_id, pe_iter->pe_id(), &report_pe);
                    if (res == error::OK || res != error::ZK_NO_NODE) {
                        used_resource_list.push_back(pe_iter->resource());
                    }
                }
            }
        }
    }
    Resource total_resource;
    if (report->has_resource() && report->resource().has_total_resource()) {
        total_resource = report->resource().total_resource();
    }
    m_scheduler->AddPNResource(report->id(), total_resource, &used_resource_list);
    return m_pn_reports.insert(PnidReportPair(report->id(), *report)).second;
}

int32_t PNClientStatus::RemovePNStatus(const PNID& pn_id) {
    MutexLockGuard mutex_guard(m_lock);
    DSTREAM_DEBUG("remove pn [%s] report", pn_id.id().c_str());
    MetaManager* meta_manager_ = MetaManager::GetMetaManager();
    if (meta_manager_ == NULL) {
        DSTREAM_WARN("metamanager is null");
        return error::INVALID_POINTER;
    }
    PnidReportMapIter pn_find = m_pn_reports.find(pn_id);
    bool res = true;
    set<ProcessorElement, ComparePE> pe_set;
    if (meta_manager_->GetPNProcessElements(pn_id, &pe_set) == error::OK) {
        PeSetIter pe_end = pe_set.end();
        for (PeSetIter pe_it = pe_set.begin(); pe_it != pe_end; ++pe_it) {
            for (int i = 0; i < pe_it->backups_size(); ++i) {
                const BackupPE& bk_pe = pe_it->backups(i);
                if (bk_pe.pn_id().id() == pn_id.id()) {
                    res &= AddPEFailEvent(m_event_manager, *pe_it, bk_pe.backup_pe_id(), pn_id);
                    break;
                }
            }
        }
    } else {
        DSTREAM_WARN("get pn process element fail");
        return error::REMOVE_PN_FAIL;
    }
    if (pn_find != m_pn_reports.end()) {
        m_pn_reports.erase(pn_find);
    } else {
        DSTREAM_INFO("pn %s no report", pn_id.id().c_str());
    }
    m_scheduler->RemovePN(pn_id);
    return res ? error::OK : error::REMOVE_PN_FAIL;
}

int32_t PNClientStatus::AddClient(const User* user, uint64_t session_id) {
    MutexLockGuard lock_guard(m_lock);
    client_info ci;
    ci.user = *user;
    if (m_clients.insert(ClientIdPair(session_id, ci)).second) {
        DSTREAM_DEBUG("Add session [%lu] success", session_id);
        return error::OK;
    }
    return error::INSERT_SET_FAIL;
}

int32_t PNClientStatus::FindClient(uint64_t session_id, client_info* ci) {
    MutexLockGuard lock_guard(m_lock);
    ClientIdMapIter client_find = m_clients.find(session_id);
    if (client_find != m_clients.end()) {
        *ci = client_find->second;
        return error::OK;
    }
    DSTREAM_INFO("session [%lu] cannot find", session_id);
    return error::NO_CLIENT;
}

int32_t PNClientStatus::UpdateClient(uint64_t session_id, const AppID& app_id) {
    MutexLockGuard lock_guard(m_lock);
    ClientIdMapIter client_find = m_clients.find(session_id);
    if (client_find != m_clients.end()) {
        client_find->second.app_id = app_id;
        return error::OK;
    }
    return error::NO_CLIENT;
}

int32_t PNClientStatus::RemoveClient(uint64_t session_id) {
    DSTREAM_DEBUG("Remove Client session [%lu]", session_id);
    MutexLockGuard lock_guard(m_lock);
    ClientIdMapIter client_find = m_clients.find(session_id);
    if (client_find != m_clients.end()) {
        AppID app_id = client_find->second.app_id;
        if (NULL != m_gc && app_id.has_id()) {
            m_gc->AddGCTask(GcTaskAutoPtr(new KillAppGCTask(app_id, m_event_manager)));
        }
        m_clients.erase(client_find);
        return error::OK;
    }
    return error::NO_CLIENT;
}

int32_t PNClientStatus::RemoveClient(const std::string& node) {
    DSTREAM_DEBUG("remove client [%s]", node.c_str());
    MutexLockGuard lock_guard(m_lock);
    ClientIdMapIter client_end = m_clients.end();
    for (ClientIdMapIter client_it = m_clients.begin();
         client_it != client_end; ++client_it) {
        if (node == client_it->second.user.hostname()) {
            if (NULL != m_gc) {
                AppID app_id = client_it->second.app_id;
                DSTREAM_DEBUG("client app id [%lu]", app_id.id());
                if (app_id.has_id()) {
                    m_gc->AddGCTask(GcTaskAutoPtr(new KillAppGCTask(app_id, m_event_manager)));
                }
            }
            DSTREAM_DEBUG("Remove Client session [%lu]", client_it->first);
            m_clients.erase(client_it);
            return error::OK;
        }
    }
    return error::NO_CLIENT;
}

int32_t PNClientStatus::InitMap() {
    MetaManager* meta = MetaManager::GetMetaManager();
    error::Code meta_res;
    if ((meta_res = meta->GetPNProcessElementsMap(&m_pn_pe_map)) != error::OK) {
        DSTREAM_WARN("get pn pe map fail");
        return meta_res;
    }
    return error::OK;
}

void PNClientStatus::EraseMap(const PEID& pe_id, const PNID& pn_id) {
    MutexLockGuard lock_gurad(m_lock);
    PnidPeSetMapIter pn_pe_iter;
    if ((pn_pe_iter = m_pn_pe_map.find(pn_id)) != m_pn_pe_map.end()) {
        for (PeSetIter pe_iter = pn_pe_iter->second.begin();
             pe_iter != pn_pe_iter->second.end();) {
            if (pe_iter->pe_id().id() == pe_id.id()) {
                pn_pe_iter->second.erase(pe_iter++);
            } else {
                pe_iter++;
            }
        }
    }
}

PnidReportMap PNClientStatus::pn_reports() {
    MutexLockGuard lock_gurad(m_lock);
    return m_pn_reports;
}

void PNClientStatus::ClearAll() {
    MutexLockGuard lock_guard(m_lock);
    m_pn_reports.clear();
    m_clients.clear();
}

dstream::error::Code PNClientStatus::Dump(std::string* output) {
    PnidReportMap pn_reports_map = pn_reports();
    PnidReportMapIter pr_iter = pn_reports_map.begin();
    PnidReportMapPB pn_reports_pb;
    for (; pr_iter != pn_reports_map.end(); ++pr_iter) {
       PnidReportKV *pn_report_kv = pn_reports_pb.add_pnid_report_kv();
       pn_report_kv->mutable_pn_id()->CopyFrom(pr_iter->first);
       pn_report_kv->mutable_pn_report()->CopyFrom(pr_iter->second);
    }
    // dump pb to json
    std::string json_str = "{\"pn_reports\":";
    std::string tmp;
    ProtoMessageToJson(pn_reports_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += "}";
    output->assign(json_str);
    return dstream::error::OK;
}

} // namespace pn_client_status
} // namespace dstream
