/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/10
* @filename:    dstream_event.h
* @author:      zhanggengxin
* @brief:       dstream event
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_DSTREAM_EVENT_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_DSTREAM_EVENT_H__ // NOLINT

#include <string>
#include "common/dstream_type.h"
#include "common/rpc_const.h"
#include "processmaster/dstream_event_manager.h"
#include "processmaster/event.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace dstream_event {

/************************************************************************/
/* DStreamEventResHandler : handle dstream async event response         */
/************************************************************************/
template <class T>
class DStreamEventResHandler : public rpc_client::ProtoAsyncRPCHandler<T> {
public:
    explicit DStreamEventResHandler(event::Event* event) : m_event_ptr(event) {}
    void HandleResponse(int call_res, const T* response,
                        const std::string& return_val);
    void HandleResponse(int call_res, const rpc::Request* request, const T* response,
                        const std::string& return_val);
private:
    event::Event* m_event_ptr;
};

template <class T>
void DStreamEventResHandler<T>::HandleResponse(int call_res,
                                               const T* response,
                                               const std::string& return_val) {
    if (return_val != rpc_const::kOK) {
        m_event_ptr->HandleEventResult(-1, response);
    } else {
        m_event_ptr->HandleEventResult(call_res, response);
    }
}

template <class T>
void DStreamEventResHandler<T>::HandleResponse(int call_res,
                                               const rpc::Request* request,
                                               const T* response,
                                               const std::string& return_val) {
    rpc_client::ProtoAsyncRPCHandler<T>::HandleResponse(call_res, request, response, return_val);
}
/****************************************************************************************/
/* event handle state : including : handle event error,create backup and select primray */
/****************************************************************************************/
enum PEState {
    CREATE_BACKUP,
    SELECT_PRIMARY,
    HANDLE_ERROR
};
/* used for CreatePEEvent or AddAppEvent
 * delay execute can avoid get zk info repeatly
 */
static const uint64_t delay_exec_time = 500;

/************************************************************************/
/* DStreamEvent: the event in dstream system                            */
/************************************************************************/
class DStreamEvent : public event::Event {
public:
    explicit DStreamEvent(const event::EventType& dstream_event_type)
        : event::Event(dstream_event_type) {}
    static void SetEventManager(dstream_event_manager::DStreamEventManager* event_manager) {
        m_event_manager = event_manager;
    }
    static void SetScheduler(scheduler::Scheduler* scheduler) {
        m_scheduler = scheduler;
    }
    virtual int32_t HandleEventResult(int res, const void* data)  {
        return 0;
    }
protected:
    bool KillPE(const AppID& app_id, const PEID& pe_id,
                const BackupPEID& backup_pe_id, const PNID& pn_id);
    virtual bool PEOperation(const std::string& host, const AppID& app_id,
                             const PEID& pe_id, const BackupPEID& backup_pe_id,
                             const uint32_t revision,
                             PEOperationMessage_OperationType type);
    static PEState HandlePEEvent(DStreamEvent& event, const Application& app,
                                 ProcessorElement& pe, PNID& dest_pn_id);
    static dstream_event_manager::DStreamEventManager* m_event_manager;
    static scheduler::Scheduler* m_scheduler;
};

class CreatePEEvent : public DStreamEvent {
public:
    CreatePEEvent(const AppID& app_id, const PEID& pe_id,
                  const Resource& resource,
                  event::EventType event_type = event::CREATE_PE);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int res, const void* data);
    virtual bool Equal(const Event& other);
    AppID app_id() {
        return m_app_id;
    }
    PEID pe_id() {
        return m_pe_id;
    }
    Resource resource() {
        return m_resource;
    }
protected:
    virtual void AddNewEvent();
    PNID m_pn_id;
    AppID m_app_id;
    PEID m_pe_id;
    PEState m_state;
    Resource m_resource;
};

class PEFailEvent : public CreatePEEvent {
public:
    PEFailEvent(const AppID& app_id, const PEID& pe_id, const Resource& resource,
                const BackupPEID& backup_pe_id, const PNID& pn_id);
    virtual bool Equal(const Event& other);
    virtual bool less(const Event& other);
    bool UpdateMeta();
private:
    void GetPESummary(const ProcessorElement& pe, int* assign_num, bool* has_primary) const;
    BackupPEID m_backup_pe_id;
};

class PNDecommissionEvent : public DStreamEvent {
public:
    explicit PNDecommissionEvent(const PNID& pn_id);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int /*res*/, const void* /*data*/) {
        return 0;
    }
    virtual bool Equal(const Event& other);
private:
    PNID m_pn_id;
};

enum MigrateState {
    INIT,
    ERROR,
};

class MigratePEEvent : public DStreamEvent {
public:
    MigratePEEvent(const AppID& app_id, const PEID& pe_id,
                   const BackupPEID& backup_pe_id, const PNID& src_pn,
                   const PNID& dest_pn, MigrateState state = INIT);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int res, const void* data);
    virtual bool Equal(const Event& other);
private:
    void AddNewEvent();
    bool ClearMeta(const BackupPEID& backup_pe_id);
    void CreateNewPE();
    AppID m_app_id;
    PEID m_pe_id;
    BackupPEID m_backup_pe_id;
    BackupPEID m_add_backup_pe_id;
    PNID m_src_pn;
    PNID m_dest_pn;
    MigrateState m_state;
};

class KillPEEvent : public DStreamEvent {
public:
    KillPEEvent(const AppID& app_id, const PEID& pe_id,
                const BackupPEID& backup_pe_id, const PNID& pn_id,
                bool kill_app = false);
    void set_resource(const Resource& resource) {
        m_resource = resource;
    }
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int res, const void* data);
    virtual bool Equal(const Event& other);
private:
    AppID m_app_id;
    PEID m_pe_id;
    BackupPEID m_backup_pe_id;
    PNID m_pn_id;
    Resource m_resource;
    bool m_kill_app;
};

/************************************************************************/
/* Add application event : the event will get resource first            */
/* After get resource the event will add create pe event                */
/************************************************************************/
class AddAppEvent : public DStreamEvent {
public:
    AddAppEvent(const AppID& app_id, bool add_sub = false);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int /*res*/, const void* /*data*/) {
        return 0;
    }
    virtual bool Equal(const Event& other);
private:
    AppID m_app_id;
    bool m_add_sub_tree;
};

class UpdateProcessorEvent : public DStreamEvent {
public:
    UpdateProcessorEvent(const AppID& app_id, const std::string processor_name, uint32_t revision);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int /*res*/, const void* /*data*/) {
        return error::OK;
    }
    virtual bool Equal(const Event& other);
private:
    AppID m_app_id;
    std::string m_processor_name;
    uint32_t m_revision;
};

class RestartPEEvent : public DStreamEvent {
public:
    RestartPEEvent(const AppID& app_id, const PEID& pe_id,
                   const BackupPEID& backup_pe_id, const PNID& pn_id,
                   const uint32_t revision);
    virtual int32_t HandleEvent();
    virtual int32_t HandleEventResult(int res, const void* data);
    virtual bool Equal(const Event& other);
private:
    AppID m_app_id;
    PEID m_pe_id;
    BackupPEID m_backup_pe_id;
    PNID m_pn_id;
    uint32_t m_revision;
};

} // namespace dstream_event
} // namespace dstream

#endif // NOLINT
