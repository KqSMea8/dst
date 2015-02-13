/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    garbage collection
*********************************************************************/

#include "processmaster/gc.h"

#include "common/hdfs_client.h"
#include "metamanager/meta_manager.h"
#include "processmaster/dstream_event.h"
#include "processmaster/pm_common.h"
#include "processmaster/pn_client_status.h"

namespace dstream {
namespace pm_gc {
using namespace application_tools;
using namespace dstream_event;
using namespace pn_client_status;
using namespace pm_common;
using namespace meta_manager;
using namespace gc_task;
using namespace config_const;

typedef std::list<Application> AppList;
typedef AppList::iterator AppListIter;

GC::GC(Config* config, EventManagerAutoPtr event_manager, SchedulerAutoPtr scheduler) :
    m_config(config), m_event_manager(event_manager), m_scheduler(scheduler) {
}

void GC::AddGCTask(const GcTaskAutoPtr& task) {
    if (!m_stop) {
        MutexLockGuard lock_guard(m_working_lock);
        m_gc_task_list.push_back(task);
    }
}

void GC::Run() {
    int sleep_ms = kDefaultGCInterval;
    std::string sleep_time;
    if (m_config->GetValue(kGCIntervalName, &sleep_time) == error::OK
        && (sleep_ms = atoi(sleep_time.c_str())) <= 0) {
        sleep_ms = kDefaultGCInterval;
    }
    m_gc_interval = sleep_ms;
    PN_LIST pn_list;
    PnidPeSetMap pnid_pe_set;
    PE_LIST all_pe;
    PeSet empty_pe_set;
    GcTaskList old_task_list;
    DSTREAM_INFO("[%s] gc is running", __FUNCTION__);
    while (!m_stop) {
        MetaManager* meta = MetaManager::GetMetaManager();
        PNClientStatus* staus = PNClientStatus::GetPNClientStatus();
        if (NULL == meta || NULL == staus) {
            m_sleep_condition.Wait(m_gc_interval);
            continue;
        }
        if (meta->GetPNList(&pn_list) == error::OK && !m_stop &&
            meta->GetAllProcessElements(&all_pe) == error::OK) {
            GetPNPEMap(all_pe, &pnid_pe_set);
            PN_LIST_ITER pn_end = pn_list.end();
            PnidReportMap pn_report_map = staus->pn_reports();
            PnidPeSetMapIter pnid_pe_end = pnid_pe_set.end();
            for (PN_LIST_ITER pn_iter = pn_list.begin();
                 pn_iter != pn_end; ++pn_iter) {
                PnidReportMapIter pn_report_find = pn_report_map.find(pn_iter->pn_id());
                if (pn_report_find != pn_report_map.end()) {
                    PnidPeSetMapIter pe_set_find = pnid_pe_set.find(pn_iter->pn_id());
                    if (pe_set_find != pnid_pe_end) {
                        CheckPEOnPN(pn_report_find->second, pe_set_find->second);
                    } else {
                        PeSet pe_set;
                        CheckPEOnPN(pn_report_find->second, pe_set);
                    }
                } else {
                    // DSTREAM_DEBUG("not find report");
                }
            }
        }
        pn_list.clear();
        pnid_pe_set.clear();
        all_pe.clear();
        m_working_lock.lock();
        old_task_list = m_gc_task_list;
        m_gc_task_list.clear();
        m_working_lock.unlock();
        GcTaskListIter task_end = old_task_list.end();
        for (GcTaskListIter task_it = old_task_list.begin();
             task_it != task_end; ++task_it) {
            if (!m_stop) {
                (*task_it)->RunTask(m_scheduler);
            }
        }
        old_task_list.clear();
        m_sleep_condition.Wait(m_gc_interval);
        m_sleep_condition.Init();
    }
    DSTREAM_INFO("[%s] gc is stopped", __FUNCTION__);
}

void GC::CheckPEOnPN(const PNReport& pn_report,
                     PeSet& pe_set) {
    MetaManager* meta = MetaManager::GetMetaManager();
    assert(NULL != meta);
    ProcessorElement pe;
    PNID pn_id = pn_report.id();
    for (int i = 0; i < pn_report.status_size(); ++i) {
        const BackupPEStatus& backup_pe = pn_report.status(i);
        *(pe.mutable_app()) = backup_pe.app_id();
        *(pe.mutable_pe_id()) = backup_pe.pe_id();
        PeSet::iterator pe_find = pe_set.find(pe);
        if (pe_find != pe_set.end()) {
            CheckBackupPE(*pe_find, backup_pe.backup_pd_id(), pn_id);
        } else {
            ProcessorElement meta_pe;
            error::Code res;
            if ((res = meta->GetProcessorElement(pe.app(),
                                                 pe.pe_id(),
                                                 &meta_pe)) == error::OK) {
                CheckBackupPE(meta_pe, backup_pe.backup_pd_id(), pn_id);
            } else {
                if (res == error::ZK_NO_NODE) {
                    // set resource to empty : because the resource has been substract
                    DSTREAM_WARN("exceptional pn is %s", pn_id.id().c_str());
                    AddKillPEEvent(m_event_manager, backup_pe.app_id(),
                                   backup_pe.pe_id(), Resource(),
                                   backup_pe.backup_pd_id(), pn_report.id());
                } else {
                    DSTREAM_WARN("get process element fail");
                }
            }
        }
    }
}

void GC::CheckBackupPE(const ProcessorElement& pe,
                       const BackupPEID& backup_pe_id,
                       const PNID& pn_id) {
    for (int j = 0; j < pe.backups_size(); ++j) {
        const BackupPE& tmp_pe = pe.backups(j);
        if (tmp_pe.backup_pe_id().id() == backup_pe_id.id()) {
            if (tmp_pe.has_pn_id() && tmp_pe.pn_id().id() != pn_id.id()) {
                // set resource to empty : because the resource has been substract
                AddKillPEEvent(m_event_manager, pe.app(), pe.pe_id(),
                               Resource(), backup_pe_id, pn_id);
            }
            break;
        }
    }
}

} // namespace pm_gc
} // namespace dstream
