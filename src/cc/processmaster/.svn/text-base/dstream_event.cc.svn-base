/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/10
* @filename:    dstream_event.cc
* @author:      zhanggengxin
* @brief:       dstream event
*/


#include "processmaster/dstream_event.h"

#include <list>
#include <set>
#include <string>
#include <utility>

#include "common/application_tools.h"
#include "common/rpc_const.h"
#include "common/utils.h"
#include "metamanager/meta_manager.h"
#include "processmaster/pm_common.h"
#include "processmaster/pn_client_status.h"

namespace dstream {
namespace dstream_event {
using namespace application_tools;
using namespace pn_client_status;
using namespace meta_manager;
using namespace event;
using namespace event_manager;
using namespace dstream_event_manager;
using namespace pm_common;
using namespace rpc_const;
using namespace proto_rpc;
using namespace scheduler;

typedef std::list<BackupPE*> BackupPeList;
typedef BackupPeList::iterator BackupPeListIter;

DStreamEventManager* DStreamEvent::m_event_manager = NULL;
Scheduler* DStreamEvent::m_scheduler = NULL;

PEState DStreamEvent::HandlePEEvent(DStreamEvent& dstream_event,
                                    const Application& app,
                                    ProcessorElement& pe,
                                    PNID& m_dest_pnid) {
    if (NULL != m_scheduler && !m_scheduler->SchedulerCanWork()) {
        DSTREAM_INFO("[%s] Scheduler cannot work", __FUNCTION__);
        return HANDLE_ERROR;
    }
    MetaManager* meta_ = MetaManager::GetMetaManager();
    AppID app_id = app.id();
    PEID pe_id = pe.pe_id();
    // get assignment pe and no assignment pes
    BackupPeList assignment_backups, no_assign_backups;
    PNIDSet m_pn_idset;
    for (int i = 0; i < pe.backups_size(); ++i) {
        BackupPE* backup_pe = pe.mutable_backups(i);
        if (backup_pe->has_pn_id()) {
            assignment_backups.push_back(backup_pe);
            m_pn_idset.insert(backup_pe->pn_id());
        } else {
            no_assign_backups.push_back(backup_pe);
        }
    }

    // check and select/set primary pe if necessary
    BackupPEID* primary_backup = pe.mutable_primary(0);
    if (!primary_backup->has_id() && assignment_backups.size() > 0) {
        PN pn;
        if (assignment_backups.size() == 1) {
            primary_backup->set_id((*assignment_backups.begin())->backup_pe_id().id());
            m_dest_pnid = *(m_pn_idset.begin());
        } else {
            if (m_scheduler->SelectPrimary(m_pn_idset, &m_dest_pnid)) {
                for (BackupPeListIter iter = assignment_backups.begin();
                     iter != assignment_backups.end();
                     ++iter) {
                    if ((*iter)->pn_id().id() == m_dest_pnid.id()) {
                        primary_backup->set_id((*iter)->backup_pe_id().id());
                        break;
                    }
                }
            }
        }
        if (primary_backup->has_id() && meta_->GetPN(m_dest_pnid, &pn) == error::OK) {
            if (meta_->UpdateProcessElement(app_id, pe) == error::OK) {
                std::string uri = GetUri(pn.host(), pn.rpc_server_port());
                if (dstream_event.PEOperation(uri, app_id, pe_id,
                                              *primary_backup,
                                              0,
                                              PEOperationMessage_OperationType_SETPRIMARY)) {
                    return SELECT_PRIMARY;
                }
            } else {
                DSTREAM_WARN("update meta fail");
            }
        } else {
            DSTREAM_WARN("set primary fail");
        }
        return HANDLE_ERROR;
    }

    // create backup pe which does not assign pn
    if (no_assign_backups.size() > 0) {
        BackupPE* backup_pe = *(no_assign_backups.begin());
        Processor processor;
        if (!GetProcessor(app, pe, &processor)) {
            return HANDLE_ERROR;
        }
        // DSTREAM_DEBUG("app id [%lu] pe id [%lu]", app_id.id(), pe.pe_id().id());
        if (m_scheduler->AllocatePN(ConstructProcessorID(pe.pe_id()),
                                    /*processor.m_resourcerequire(),*/
                                    processor,
                                    pe.pe_id(), m_dest_pnid) == error::OK) {
            PN pn;
            if (meta_->GetPN(m_dest_pnid, &pn) == error::OK) {
                RPCRequest<CreatePEMessage>* msg = new RPCRequest<CreatePEMessage>();
                DStreamEventResHandler<CreatePEResult>* response =
                    new DStreamEventResHandler<CreatePEResult>(&dstream_event);
                if (NULL == msg || NULL == response) {
                    DSTREAM_WARN("create request or response fail");
                    RELEASEPTR(msg);
                    RELEASEPTR(response);
                    return HANDLE_ERROR;
                }
                std::string uri = rpc_const::GetUri(pn.host(), pn.rpc_server_port());
                *(msg->mutable_app_id()) = app_id;
                *(msg->mutable_pe_id()) = pe_id;
                *(msg->mutable_backup_id()) = backup_pe->backup_pe_id();
                *(msg->mutable_processor()) = processor;
                msg->set_last_assign_time(backup_pe->has_last_assignment_time() ?
                                          backup_pe->last_assignment_time() : 0);
                if (m_event_manager->AsyncRPCCall(uri, kCreatePEMethod, msg, response)) {
                    DSTREAM_INFO("Call create pe [%lu] on pn [%s]", pe_id.id(),
                                 pn.pn_id().id().c_str());
                    return CREATE_BACKUP;
                }
                RELEASEPTR(msg);
                RELEASEPTR(response);
            } else {
                DSTREAM_WARN("Get PN information fail");
            }
        } else {
            DSTREAM_WARN("Allocate pn fail");
        }
    }
    return HANDLE_ERROR;
}

bool DStreamEvent::PEOperation(const std::string& host,
                               const AppID& app_id,
                               const PEID& pe_id,
                               const BackupPEID& backup_pe_id,
                               const uint32_t revision,
                               PEOperationMessage_OperationType type) {
    RPCRequest<PEOperationMessage>* request = new RPCRequest<PEOperationMessage>;
    DStreamEventResHandler<PEOperationResult>* handler(new
            DStreamEventResHandler<PEOperationResult>(this));
    if (NULL == request || NULL == handler) {
        RELEASEPTR(request);
        RELEASEPTR(handler);
        DSTREAM_WARN("create request or handler fail");
        return false;
    }
    request->set_type(type);
    *(request->mutable_app_id()) = app_id;
    *(request->mutable_pe_id()) = pe_id;
    *(request->mutable_backup_pe_id()) = backup_pe_id;
    request->set_revision(revision);
    if (!m_event_manager->AsyncRPCCall(host, kPEOperationMethod, request, handler)) {
        RELEASEPTR(request);
        RELEASEPTR(handler);
        DSTREAM_WARN("async rpc call fail");
        return false;
    }
    return true;
}

PEFailEvent::PEFailEvent(const AppID& pefail_app_id,
                         const PEID& pefail_pe_id,
                         const Resource& pefail_resource,
                         const BackupPEID& backup_pe_id,
                         const PNID& pn_id)
    : CreatePEEvent(pefail_app_id, pefail_pe_id, pefail_resource, m_event_type = PE_FAIL) {
    m_backup_pe_id = backup_pe_id;
    m_pn_id = pn_id;
    // snprintf(m_comp_str.c_str(), MAX_COMPSTR_LEN, "%lu_%lu_%lu_%s",
    //          m_app_id.id(), m_pe_id.id(), m_backup_pe_id.id(), m_pn_id.id().c_str());
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&PEID=";
    StringAppendNumber(&m_comp_str, m_pe_id.id());
    m_comp_str += "&BackupPEID=";
    StringAppendNumber(&m_comp_str, m_backup_pe_id.id());
    m_comp_str += "PNID=";
    m_comp_str.append(m_pn_id.id());
}

bool PEFailEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const PEFailEvent& other_event = dynamic_cast<const PEFailEvent&>(other); // NOLINT
    if (this == &other_event) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

bool PEFailEvent::less(const Event& other) {
    const PEFailEvent& other_event = dynamic_cast<const PEFailEvent&>(other); // NOLINT
    if (m_app_id.id() != other_event.m_app_id.id()) {
        return m_app_id.id() < other_event.m_app_id.id();
    }
    if (this == &other_event ||
        (m_app_id.id() == other_event.m_app_id.id() &&
         (m_pe_id.id() == other_event.m_pe_id.id()))) {
        return false;
    }
    if (m_pe_id.id() != other_event.m_pe_id.id()) {
        int assign_num = 0;
        int other_assign_num = 0;
        bool has_primary = true;
        bool other_has_primary = true;
        ProcessorElement pe, other_pe;
        MetaManager* meta = MetaManager::GetMetaManager();
        if (meta->GetProcessorElement(m_app_id, m_pe_id, &pe) != error::OK ||
            meta->GetProcessorElement(other_event.m_app_id,
                                      other_event.m_pe_id, &other_pe) != error::OK) {
            DSTREAM_WARN("get pe fail");
            return false;
        }
        GetPESummary(pe, &assign_num, &has_primary);
        other_event.GetPESummary(other_pe, &other_assign_num, &other_has_primary);
        if (!has_primary || !other_has_primary) {
            return !has_primary;
        }
        return assign_num <= other_assign_num;
    }
    return false;
}

void PEFailEvent::GetPESummary(const ProcessorElement& pe,
                               int* assign_num,
                               bool* has_primary) const {
    for (int i = 0; i < pe.backups_size(); ++i) {
        if (pe.backups(i).has_pn_id()) {
            ++(*assign_num);
        }
    }
    *has_primary = pe.primary(0).has_id();
}

bool PEFailEvent::UpdateMeta() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    ProcessorElement pe;
    error::Code res = error::OK;
    if ((res = meta_->GetProcessorElement(m_app_id, m_pe_id, &pe)) != error::OK) {
        if (res = error::ZK_NO_NODE) {
            return true;
        }
        DSTREAM_WARN("get app [%lu] pe [%lu] fail", m_app_id.id(), m_pe_id.id());
        return false;
    }
    for (int i = 0; i < pe.backups_size(); ++i) {
        BackupPE* backup_pe = pe.mutable_backups(i);
        if (backup_pe->backup_pe_id().id() == m_backup_pe_id.id()) {
            if (backup_pe->pn_id().id() == m_pn_id.id()) {
                backup_pe->clear_pn_id();
                BackupPEID* primary_pe = pe.mutable_primary(0);
                if (primary_pe->id() == m_backup_pe_id.id()) {
                    primary_pe->clear_id();
                }
                backup_pe->set_last_fail_pn(m_pn_id.id());
                if (meta_->UpdateProcessElement(m_app_id, pe) != error::OK) {
                    DSTREAM_WARN("update process element fail");
                    return false;
                }
                // return used resource
                m_scheduler->ReleaseResource(ConstructProcessorID(m_pe_id),
                                            pe.resource(), m_pn_id);
                if (m_scheduler->AddFailPE(m_pe_id, m_pn_id) != error::OK) {
                    DSTREAM_WARN("[%s] Add Fail PE Fail", __FUNCTION__);
                }
            }
            return true;
        }
    }
    return true;
}


PNDecommissionEvent::PNDecommissionEvent(const PNID& pn_id) :
    DStreamEvent(PN_DECOMMISSION), m_pn_id(pn_id) {
    m_comp_str += "PNID=";
    m_comp_str.append(m_pn_id.id());
}

int32_t PNDecommissionEvent::HandleEvent() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    assert(NULL != meta_);
    if (m_pn_id.has_id()) {
        // update pn status
        PN pn;
        PeSet pe_set;
        if (meta_->GetPN(m_pn_id, &pn) == error::OK) {
            if (pn.status() == NORMAL) {
                pn.set_status(DECOSSIMION);
                if (meta_->UpdatePN(pn) != error::OK) {
                    DSTREAM_WARN("update pn error");
                    goto ADD_PN_DEC_EVENT;
                }
            }
        } else {
            DSTREAM_WARN("get pn error");
            goto ADD_PN_DEC_EVENT;
        }
        if (meta_->GetPNProcessElements(m_pn_id, &pe_set) == error::OK) {
            if (pe_set.size() > 0) {
                PeSetIter pe_end = pe_set.end();
                for (PeSetIter pe_iter = pe_set.begin();
                     pe_iter != pe_end;
                     ++pe_iter) {
                    for (int i = 0; i < pe_iter->backups_size(); ++i) {
                        const BackupPE& backup_pe = pe_iter->backups(i);
                        if (cmp_pn_id(m_pn_id, backup_pe.pn_id())) {
                            PNID empty_pn_id;
                            m_event_manager->AddEvent(
                                EventPtr(new MigratePEEvent(pe_iter->app(), pe_iter->pe_id(),
                                    backup_pe.backup_pe_id(), m_pn_id, empty_pn_id)));
                            break;
                        }
                    }
                }
            }
        } else {
            DSTREAM_WARN("get process element error");
            goto ADD_PN_DEC_EVENT;
        }
    } else {
        DSTREAM_WARN("meta manager is not init");
        goto ADD_PN_DEC_EVENT;
    }
    return error::OK;
ADD_PN_DEC_EVENT:
    if (NULL != m_event_manager) {
        m_event_manager->AddEvent(EventPtr(new PNDecommissionEvent(m_pn_id)));
    }
    return error::OK;
}

bool PNDecommissionEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const PNDecommissionEvent& other_event = dynamic_cast<const PNDecommissionEvent&>(other); // NOLINT
    if (this == &other_event) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

MigratePEEvent::MigratePEEvent(const AppID& app_id,
                               const PEID& pe_id,
                               const BackupPEID& backup_pe_id,
                               const PNID& src_pn,
                               const PNID& dest_pn,
                               MigrateState state) : DStreamEvent(MIGRATE_PE) {
    m_app_id = app_id;
    m_pe_id = pe_id;
    m_backup_pe_id = backup_pe_id;
    m_src_pn = src_pn;
    m_dest_pn = dest_pn;
    m_state = state;
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&PEID=";
    StringAppendNumber(&m_comp_str, m_pe_id.id());
    m_comp_str += "&BackupPEID=";
    StringAppendNumber(&m_comp_str, m_backup_pe_id.id());
    m_comp_str += "&SourcePNID=";
    m_comp_str.append(src_pn.id());
}

int32_t MigratePEEvent::HandleEvent() {
    return error::OK;
}

int32_t MigratePEEvent::HandleEventResult(int /*res*/, const void* /*data*/) {
    return error::OK;
}

void MigratePEEvent::AddNewEvent() {
    MigratePEEvent* add_event = new MigratePEEvent(m_app_id, m_pe_id,
                                                   m_backup_pe_id, m_src_pn,
                                                   m_dest_pn, m_state);
    EventPtr auto_ptr(add_event);
    add_event->m_backup_pe_id = m_backup_pe_id;
    m_event_manager->AddEvent(add_event);
}

bool MigratePEEvent::ClearMeta(const BackupPEID& /*backup_pe_id*/) {
    return true;
}

void MigratePEEvent::CreateNewPE() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    if (NULL == meta_ || NULL == m_scheduler) {
        m_state = ERROR;
        return;
    }
    ProcessorElement pe;
    if (meta_->GetProcessorElement(m_app_id, m_pe_id, &pe) != error::OK) {
        m_state = ERROR;
        DSTREAM_WARN("get process element error");
        return;
    }
    int backup_size = pe.backups_size();
    list<PNID> assignment_pns;
    for (int i = 0; i < backup_size; ++i) {
        const BackupPE& backup_pe = pe.backups(i);
        if (backup_pe.backup_pe_id().id() == m_backup_pe_id.id()) {
            if (!backup_pe.has_pn_id() || backup_pe.pn_id().id() != m_src_pn.id()) {
                m_state = ERROR;
                return;
            }
        }
        if (backup_pe.has_pn_id()) {
            if (m_dest_pn.has_id() && backup_pe.pn_id().id() == m_dest_pn.id()) {
                m_state = ERROR;
                return;
            }
            assignment_pns.push_back(backup_pe.pn_id());
        }
    }
    list<PNID> prefers;
    if (m_dest_pn.has_id()) {
        prefers.push_back(m_dest_pn);
    }
}

bool MigratePEEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const MigratePEEvent other_event = dynamic_cast<const MigratePEEvent&>(other); // NOLINT
    if (this == &other_event) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

CreatePEEvent::CreatePEEvent(const AppID& create_app_id,
                             const PEID& create_pe_id,
                             const Resource& res,
                             EventType create_event_type) :
    DStreamEvent(create_event_type), m_app_id(create_app_id),
    m_pe_id(create_pe_id), m_resource(res) {
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&PEID=";
    StringAppendNumber(&m_comp_str, m_pe_id.id());
}

int32_t CreatePEEvent::HandleEvent() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    assert(NULL != meta_);
    Application app;
    ProcessorElement pe;
    error::Code res = error::OK;

    if ((res = meta_->GetApplication(m_app_id, &app)) != error::OK) {
        if (res != error::ZK_NO_NODE) {
            DSTREAM_WARN("get application [%ld] fail", m_app_id.id());
            AddNewEvent();
        }
        goto EXIT;
    }
    if (!AppCanScheduler(app)) {
        DSTREAM_WARN("App [%lu] cannot scheduler status [%d]", m_app_id.id(), app.status());
        goto EXIT;
    }
    if (meta_->GetProcessorElement(m_app_id, m_pe_id, &pe) != error::OK) {
        DSTREAM_WARN("get app [%lu] pe [%lu] fail", m_app_id.id(), m_pe_id.id());
        goto EXIT;
    }
    if (CheckPENormal(pe, m_app_id, m_pe_id)) {
        goto EXIT;
    }
    if ((m_state = HandlePEEvent(*this, app, pe, m_pn_id)) == HANDLE_ERROR) {
        AddNewEvent();
    }
EXIT:

    return error::OK;
}

int32_t CreatePEEvent::HandleEventResult(int res, const void* data) {
    std::string result = "success";
    if (res != 0) {
        result = "fail";
    }
    if (m_state == CREATE_BACKUP) {
        DSTREAM_INFO("handle create pe [%lu] on pn [%s] event result [%s]",
                     m_pe_id.id(), m_pn_id.id().c_str(), result.c_str());
    } else {
        DSTREAM_INFO("set primary pe [%lu] on pn [%s] event result [%s]",
                     m_pe_id.id(), m_pn_id.id().c_str(), result.c_str());
    }

    MetaManager* meta_ = MetaManager::GetMetaManager();
    if (res != 0) {
        DSTREAM_WARN("create pe [%lu] on pn [%s] fail", m_pe_id.id(),
                     m_pn_id.id().c_str());
        // return resource
        m_scheduler->ReleaseResource(ConstructProcessorID(m_pe_id), m_resource, m_pn_id);
        m_scheduler->AddFailPE(m_pe_id, m_pn_id);
        const CreatePEResult* create_result = (const CreatePEResult*)(data);
        if (create_result->id().id() == m_pn_id.id()) {
            ProcessorElement pe;
            if (meta_->GetProcessorElement(m_app_id, m_pe_id, &pe) == error::OK) {
                for (int i = 0; i < pe.backups_size(); ++i) {
                    BackupPE* bakup_pe = pe.mutable_backups(i);
                    if (bakup_pe->backup_pe_id().id() == create_result->backup_id().id()) {
                        bakup_pe->set_last_fail_pn(m_pn_id.id());
                        if (meta_->UpdateProcessElement(m_app_id, pe) != error::OK) {
                            DSTREAM_WARN("[%s] Update PE [%lu]", __FUNCTION__, m_pe_id.id());
                        }
                        break;
                    }
                }
            }
        }
    } else {
        if (m_state == CREATE_BACKUP) {
            const CreatePEResult* create_result = (const CreatePEResult*)(data);
            if (create_result->id().id() == m_pn_id.id()) {
                ProcessorElement pe;
                if (meta_->GetProcessorElement(m_app_id, m_pe_id, &pe) == error::OK) {
                    for (int i = 0; i < pe.backups_size(); ++i) {
                        BackupPE* backup_pe = pe.mutable_backups(i);
                        if (backup_pe->backup_pe_id().id() == create_result->backup_id().id()) {
                            *(backup_pe->mutable_pn_id()) = m_pn_id;
                            BackupPEID* primary_backup = pe.mutable_primary(0);
                            primary_backup->set_id(m_pe_id.id());
                            backup_pe->set_last_assignment_time((uint64)GetTimeMS());
                            if (meta_->UpdateProcessElement(m_app_id, pe) != error::OK) {
                                DSTREAM_WARN("[%s] update pe [%lu] fail", __FUNCTION__,
                                             m_pe_id.id());
                                // return resource
                                ProcessorID pid = ConstructProcessorID(m_pe_id);
                                m_scheduler->ReleaseResource(pid, m_resource, m_pn_id);
                            } else {
                                DSTREAM_DEBUG("[%s] update pe [%lu] success", __FUNCTION__,
                                              m_pe_id.id());
                                return error::OK;
                            }
                            break;
                        }
                    }
                } else {
                    DSTREAM_WARN("[%s] Get PE [%lu] fail", __FUNCTION__, m_pe_id.id());
                    // return resource
                    ProcessorID pid = ConstructProcessorID(m_pe_id);
                    m_scheduler->ReleaseResource(pid, m_resource, m_pn_id);
                }
            }
        }
    }
    AddNewEvent();
    return error::OK;
}

void CreatePEEvent::AddNewEvent() {
    if (NULL != m_event_manager) {
        EventPtr add_event;
        add_event = EventPtr(new (std::nothrow)CreatePEEvent(m_app_id, m_pe_id, m_resource));
        m_event_manager->AddDelayExecEvent(delay_exec_time, add_event);
    }
}

bool CreatePEEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const CreatePEEvent& other_event = dynamic_cast<const CreatePEEvent&>(other); // NOLINT
    if (this == &other_event) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

KillPEEvent::KillPEEvent(const AppID& app_id,
                         const PEID& pe_id,
                         const BackupPEID& backup_pe_id,
                         const PNID& pn_id,
                         bool kill_app) : DStreamEvent(KILL_PE) {
    m_app_id = app_id;
    m_pe_id = pe_id;
    m_backup_pe_id = backup_pe_id;
    m_pn_id = pn_id;
    m_kill_app = kill_app;
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&PEID=";
    StringAppendNumber(&m_comp_str, m_pe_id.id());
    m_comp_str += "&BackupPEID=";
    StringAppendNumber(&m_comp_str, m_backup_pe_id.id());
    m_comp_str += "&PNID=";
    m_comp_str.append(m_pn_id.id());
}

int32_t KillPEEvent::HandleEvent() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    PN pn;

    if (meta_->GetPN(m_pn_id, &pn) == error::OK) {
        // DSTREAM_DEBUG("get pn %s success", m_pn_id.id().c_str());
        PEOperation(rpc_const::GetUri(pn.host(), pn.rpc_server_port()),
                    m_app_id, m_pe_id, m_backup_pe_id,
                    0,
                    PEOperationMessage_OperationType_KILL);
        ProcessorID pid = ConstructProcessorID(m_pe_id);
        m_scheduler->ReleaseResource(pid, m_resource, m_pn_id, !m_kill_app);
    } else {
        DSTREAM_WARN("get pn %s fail", m_pn_id.id().c_str());
    }

    return error::OK;
}

int32_t KillPEEvent::HandleEventResult(int res, const void* /*data*/) {
    // do nothing
    DSTREAM_INFO("kill pe [%lu] on pn [%s] result [%d]", m_backup_pe_id.id(),
                 m_pn_id.id().c_str(), res);
    // erase peinfo in pn client status
    PNClientStatus::GetPNClientStatus()->EraseMap(m_pe_id, m_pn_id);
    return error::OK;
}

bool KillPEEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const KillPEEvent& other_event = dynamic_cast<const KillPEEvent&>(other); // NOLINT
    if (this == &other) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

AddAppEvent::AddAppEvent(const AppID& app_id, bool add_sub /*= false*/) :
    DStreamEvent(KILL_APP), m_app_id(app_id), m_add_sub_tree(add_sub) {
    // snprintf(m_comp_str, MAX_COMPSTR_LEN, "%lu", m_app_id.id());
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
}

int32_t AddAppEvent::HandleEvent() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("metamanager is not init");
        m_event_manager->AddEvent(EventPtr(new AddAppEvent(*this)));
        return error::OK;
    }
    Application app;
    int32_t res = error::OK;
    if ((res = meta->GetApplication(m_app_id, &app)) != error::OK) {
        // if application is not exist ignore it
        if (res == error::ZK_NO_NODE) {
            DSTREAM_INFO("[%s] App [%lu] is not exist", __FUNCTION__, m_app_id.id());
            return error::OK;
        }
        DSTREAM_WARN("[%s] Get App [%lu] fail", __FUNCTION__, m_app_id.id());
        m_event_manager->AddEvent(EventPtr(new AddAppEvent(*this)));
        return error::OK;
    }
    if (!AppCanScheduler(app)) {
        DSTREAM_INFO("[%s] App [%lu] status [%d] can not schedule", __FUNCTION__,
                     m_app_id.id(),
                     app.status());
        return error::OK;
    }

    // Add Assignment
    ResReqSet res_req_set;
    ResReqSetIter res_req_find;
    set<PEID, PEIDCompare> unsigned_peid_set;
    PeList pe_list;
    PeListIter pe_it, pe_end;
    PN pn;
    if (meta->GetAppProcessElements(m_app_id, &pe_list) != error::OK) {
        DSTREAM_WARN("[%s] get app [%lu] pe list fail", __FUNCTION__, m_app_id.id());
        m_event_manager->AddEvent(EventPtr(new AddAppEvent(*this)));
        return error::OK;
    }

    for (pe_it = pe_list.begin(), pe_end = pe_list.end();
         pe_it != pe_end; ++pe_it) {
        for (int i = 0; i < pe_it->backups_size(); ++i) {
            const BackupPE& bk_pe = pe_it->backups(i);
            if (bk_pe.has_pn_id() && meta->GetPN(bk_pe.pn_id(), &pn) == error::OK) {
                continue;
            } else {
                if (bk_pe.has_pn_id()) {
                    BackupPE* m_bk_pe = pe_it->mutable_backups(i);
                    m_bk_pe->clear_pn_id();
                    if (meta->UpdateProcessElement(m_app_id, *pe_it) != error::OK) {
                        DSTREAM_WARN("[%s] Update App [%lu] PE [%lu] fail", __FUNCTION__,
                                     m_app_id.id(),
                                     pe_it->pe_id().id());
                        m_event_manager->AddEvent(EventPtr(new AddAppEvent(*this)));
                        return error::OK;
                    }
                }
                /* get the group name of pe */
                Processor pro;
                std::string group_name;
                if (GetProcessor(app, *pe_it, &pro)) {
                    group_name = pro.pn_group();
                }
                ProcessorID pid = ConstructProcessorID(pe_it->pe_id());
                unsigned_peid_set.insert(pe_it->pe_id());
                resource_request res_req(pid, pe_it->resource());
                res_req.m_group_name = group_name;
                if ((res_req_find = res_req_set.find(res_req)) == res_req_set.end()) {
                    res_req.m_pe_set.insert(pe_it->pe_id());
                    res_req_set.insert(res_req);
                } else {
                    const_cast<PeidSet&>(res_req_find->m_pe_set).insert(pe_it->pe_id());
                }
            }
        }
    }

    // ask for resource
    if (m_scheduler->AddResRequest(res_req_set) == error::OK) {
        // Add Create PE Event
        PE_LIST::iterator pe_list_end, pe_list_it;
        set<PEID, PEIDCompare>::iterator pe_set_end = unsigned_peid_set.end();
        for (pe_list_it = pe_list.begin(), pe_list_end = pe_list.end();
             pe_list_it != pe_list_end; ++pe_list_it) {
            if (unsigned_peid_set.find(pe_list_it->pe_id()) != pe_set_end) {
                DSTREAM_DEBUG("[%s] Add app [%lu] create pe [%lu] ", __FUNCTION__,
                              m_app_id.id(),
                              pe_list_it->pe_id().id());
                m_event_manager->AddEvent(EventPtr(new CreatePEEvent(m_app_id,
                                                   pe_list_it->pe_id(),
                                                   pe_list_it->resource())));
            }
        }
    } else {
        DSTREAM_WARN("[%s] Add Resource Request for app [%lu] fail", __FUNCTION__,
                     m_app_id.id());
        m_event_manager->AddDelayExecEvent(delay_exec_time, EventPtr(new AddAppEvent(*this)));
        return error::OK;
    }
    return error::OK;
}

bool AddAppEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const AddAppEvent& other_event = dynamic_cast<const AddAppEvent&>(other); // NOLINT
    return m_comp_str < other_event.comp_str();
}

UpdateProcessorEvent::UpdateProcessorEvent(const AppID& app_id, const std::string processor_name,
                                           uint32_t revision) : DStreamEvent(UPDATE_PRO) {
    m_app_id = app_id;
    m_processor_name = processor_name;
    m_revision = revision;
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&ProcessorName=";
    m_comp_str.append(processor_name);
    m_comp_str += "&Revision=";
    StringAppendNumber(&m_comp_str, revision);
}

int32_t UpdateProcessorEvent::HandleEvent() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("metamanager is not init");
        m_event_manager->AddEvent(EventPtr(new UpdateProcessorEvent(*this)));
        return error::OK;
    }
    Application app;
    int32_t res = error::OK;
    if ((res = meta->GetApplication(m_app_id, &app)) != error::OK) {
        // if application is not exist ignore it
        if (res == error::ZK_NO_NODE) {
            DSTREAM_INFO("[%s] App [%lu] is not exist", __FUNCTION__, m_app_id.id());
            return error::OK;
        }
        DSTREAM_WARN("[%s] Get App [%lu] fail", __FUNCTION__, m_app_id.id());
        m_event_manager->AddEvent(EventPtr(new UpdateProcessorEvent(*this)));
        return error::OK;
    }

    // for every processelement of this processor, restart it
    PeList pe_list;
    PeListIter pe_it, pe_end;
    if (meta->GetAppProcessElements(m_app_id, &pe_list) != error::OK) {
        DSTREAM_WARN("[%s] get app [%lu] pe list fail", __FUNCTION__, m_app_id.id());
        m_event_manager->AddEvent(EventPtr(new UpdateProcessorEvent(*this)));
        return error::OK;
    }

    for (pe_it = pe_list.begin(), pe_end = pe_list.end();
         pe_it != pe_end; ++pe_it) {
        for (int i = 0; i < pe_it->backups_size(); ++i) {
            const BackupPE& bk_pe = pe_it->backups(i);
            if (pe_it->processor_name() == m_processor_name) {
                m_event_manager->AddEvent(
                    EventPtr(new RestartPEEvent(m_app_id,
                                                pe_it->pe_id(),
                                                bk_pe.backup_pe_id(), bk_pe.pn_id(), m_revision)));
            }
        }
    }

    return error::OK;
}

bool UpdateProcessorEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const UpdateProcessorEvent& other_event = dynamic_cast<const UpdateProcessorEvent&>(other); // NOLINT
    return m_comp_str < other_event.comp_str();
}

RestartPEEvent::RestartPEEvent(const AppID& app_id,
                               const PEID& pe_id,
                               const BackupPEID& backup_pe_id,
                               const PNID& pn_id,
                               const uint32_t revision) : DStreamEvent(RESTART_PE) {
    m_app_id = app_id;
    m_pe_id = pe_id;
    m_backup_pe_id = backup_pe_id;
    m_pn_id = pn_id;
    m_revision = revision;
    // snprintf(m_comp_str, MAX_COMPSTR_LEN, "%lu_%lu_%lu_%s",
    //          m_app_id.id(), m_pe_id.id(), m_backup_pe_id.id(), m_pn_id.id().c_str());
    m_comp_str += "APPID=";
    StringAppendNumber(&m_comp_str, m_app_id.id());
    m_comp_str += "&PEID=";
    StringAppendNumber(&m_comp_str, m_pe_id.id());
    m_comp_str += "&BackupPEID=";
    StringAppendNumber(&m_comp_str, m_backup_pe_id.id());
    m_comp_str += "&PNID=";
    m_comp_str.append(m_pn_id.id());
    m_comp_str += "&Revision=";
    StringAppendNumber(&m_comp_str, m_revision);
}

int32_t RestartPEEvent::HandleEvent() {
    MetaManager* meta_ = MetaManager::GetMetaManager();
    PN pn;

    if (meta_->GetPN(m_pn_id, &pn) == error::OK) {
        PEOperation(rpc_const::GetUri(pn.host(), pn.rpc_server_port()),
                    m_app_id, m_pe_id, m_backup_pe_id,
                    m_revision,
                    PEOperationMessage_OperationType_RESTART);
    } else {
        DSTREAM_WARN("get pn %s fail", m_pn_id.id().c_str());
    }
    return error::OK;
}

int32_t RestartPEEvent::HandleEventResult(int res, const void* data) {
    DSTREAM_INFO("restart pe [%lu] on pn [%s] result [%d]", m_backup_pe_id.id(),
                 m_pn_id.id().c_str(), res);
    return error::OK;
}

bool RestartPEEvent::Equal(const Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const RestartPEEvent& other_event = dynamic_cast<const RestartPEEvent&>(other); // NOLINT
    if (this == &other) {
        return false;
    }
    return m_comp_str < other_event.comp_str();
}

} // namespace dstream_event
} // namespace dstream
