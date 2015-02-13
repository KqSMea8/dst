/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/10
* @filename:    dstream_event_manager.cc
* @author:      zhanggengxin
* @brief:       dstream event manager
*/

#include "processmaster/dstream_event_manager.h"

#include "common/config_const.h"
#include "processmaster/dstream_event.h"

namespace dstream {
namespace dstream_event_manager {
using namespace config;
using namespace event;
using namespace dstream_event;
using namespace event_manager;
using namespace rpc;
using namespace rpc_client;
using namespace auto_ptr;
using namespace scheduler;

class CompareFailPEEvent {
public:
    bool operator()(const EventPtr& event1, const EventPtr& event2) {
        return event1->less(*event2);
    }
};

DStreamEventManager::DStreamEventManager(config::Config* config,
                                         AutoPtr<Scheduler> scheduler)
    : m_config(config), m_scheduler(scheduler), m_rpc_call_timeout(50000) {
    DStreamEvent::SetEventManager(this);
    DStreamEvent::SetScheduler(m_scheduler);
    // read conf
    std::string conf_value;
    if (m_config->GetValue(config_const::kConfRPCTimeout, &conf_value) == error::OK) {
        m_rpc_call_timeout = strtoul(conf_value.c_str(), NULL, 10);
    }
    DSTREAM_INFO("set rpc call timeout: %u", m_rpc_call_timeout);
}

void DStreamEventManager::HandleEvents(EventMap* events) {
    m_scheduler->Update();

    m_rpc_call_number = 0;
    int max_async_rpc_number = config_const::kDefaultMaxAsyncRPCNumber;
    std::string max_rpc;
    if (m_config->GetValue(config_const::kMaxAsyncRPCName, &max_rpc) == error::OK) {
        if ((max_async_rpc_number = atoi(max_rpc.c_str())) <= 0) {
            max_async_rpc_number = config_const::kDefaultMaxAsyncRPCNumber;
        }
    }
    DSTREAM_DEBUG("MAX RPC Number [%d]", max_async_rpc_number);
    MergePEFailEvents(events);
    if (m_rpc_call_number >= max_async_rpc_number) {
        AddRemainEvent(events->begin(), events->end());
        return;
    }
    EventMapIter event_end = events->end();
    for (EventMapIter event_iter = events->begin();
         event_iter != event_end; ++event_iter) {
        EventSet::iterator event_set_end = event_iter->second.end();
        for (EventSet::iterator event_set_iter = event_iter->second.begin();
             event_set_iter != event_set_end;
             ++event_set_iter) {
            (*event_set_iter)->HandleEvent();
            if (m_rpc_call_number >= max_async_rpc_number) {
                m_asyc_rpc_client.wait();
                AddNewEvent(event_set_iter, event_set_end);
                AddRemainEvent(++event_iter, event_iter);
                return;
            }
        }
    }
    m_asyc_rpc_client.wait();
}

bool DStreamEventManager::AsyncRPCCall(const std::string& uri,
                                       const char* method,
                                       Request* request,
                                       AsyncRPCHandler* handler) {
    if (m_asyc_rpc_client.call(uri.c_str(), method, request, handler, m_rpc_call_timeout)) {
        ++m_rpc_call_number;
        return true;
    }
    return false;
}

void DStreamEventManager::MergePEFailEvents(EventMap* events) {
    EventMapIter fail_event_find = events->find(PE_FAIL);
    if (fail_event_find != events->end()) {
        // DSTREAM_DEBUG("pe fail event number [%zu]", fail_event_find->second.size());
        EventMapIter create_event_find = events->find(CREATE_PE);
        bool has_create_event = false;
        if (create_event_find != events->end()) {
            has_create_event = !create_event_find->second.empty();
        }
        typedef std::set<EventPtr, CompareFailPEEvent> FailEventSet;
        typedef FailEventSet::iterator FailEventSetIter;
        FailEventSet fail_event_set;
        EventSet::iterator fail_event_end = fail_event_find->second.end();
        for (EventSet::iterator fail_event_iter = fail_event_find->second.begin();
             fail_event_iter != fail_event_end;
             ++fail_event_iter) {
            PEFailEvent* fail_pe = dynamic_cast<PEFailEvent*>((Event*)*fail_event_iter); // NOLINT
            if (!fail_pe->UpdateMeta()) {
                DSTREAM_WARN("update meta fail");
                AddEvent(*fail_event_iter);
                continue;
            }
            EventPtr tmp_event(new CreatePEEvent(fail_pe->app_id(),
                                                 fail_pe->pe_id(),
                                                 fail_pe->resource()));
            if (!has_create_event ||
                create_event_find->second.find(tmp_event) == create_event_find->second.end()) {
                fail_event_set.insert(*fail_event_iter);
            }
        }
        m_scheduler->Update();
        FailEventSetIter new_fail_event_end = fail_event_set.end();
        for (FailEventSetIter fail_event = fail_event_set.begin();
             fail_event != new_fail_event_end;
             ++fail_event) {
            // DSTREAM_DEBUG("handle fail pe event");
            (*fail_event)->HandleEvent();
        }
        m_asyc_rpc_client.wait();
        events->erase(fail_event_find);
    }
}

void DStreamEventManager::AddRemainEvent(EventMapIter event_begin,
                                         EventMapIter event_end) {
    for (; event_begin != event_end; ++event_begin) {
        AddNewEvent(event_begin->second.begin(), event_begin->second.end());
    }
}

void DStreamEventManager::AddNewEvent(EventSet::iterator iter,
                                      EventSet::iterator end) {
    for (; iter != end; ++iter) {
        AddEvent(*iter);
    }
}

} // namespace dstream_event_manager
} // namespace dstream
