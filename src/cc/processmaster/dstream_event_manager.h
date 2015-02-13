/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/10
* @filename:    dstream_event_manager.h
* @author:      zhanggengxin
* @brief:       dstream event manager
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_DSTREAM_EVENT_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_DSTREAM_EVENT_MANAGER_H__ // NOLINT

#include <set>
#include <string>
#include "common/auto_ptr.h"
#include "common/config.h"
#include "common/rpc_client.h"
#include "metamanager/meta_manager.h"
#include "processmaster/event_manager.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace dstream_event_manager {

/************************************************************************/
/* handle dstream event                                                 */
/************************************************************************/
class DStreamEventManager : public event_manager::EventManager {
public:
    DStreamEventManager(config::Config* config, auto_ptr::AutoPtr<scheduler::Scheduler> scheduler);
    virtual void HandleEvents(event_manager::EventMap* events);
    bool AsyncRPCCall(const std::string& uri, const char* method,
                      rpc::Request* request, rpc_client::AsyncRPCHandler* handler);
private:
    void MergePEFailEvents(event_manager::EventMap* events);
    void AddRemainEvent(event_manager::EventMapIter event_begin,
                        event_manager::EventMapIter event_end);
    void AddNewEvent(event_manager::EventSet::iterator iter,
                     event_manager::EventSet::iterator end);
    rpc_client::AsyncRPCClient m_asyc_rpc_client;
    config::Config* m_config;
    auto_ptr::AutoPtr<scheduler::Scheduler> m_scheduler;
    int m_rpc_call_number;
    uint32_t m_rpc_call_timeout;
};

} // namespace dstream_event_manager
} // namespace dstream

#endif // NOLINT
