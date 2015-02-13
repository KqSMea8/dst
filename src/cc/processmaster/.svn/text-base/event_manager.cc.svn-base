/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/07
* @filename:    event_manager.cc
* @author:      zhanggengxin
* @brief:       event manager
*/
#include "boost/algorithm/string/trim.hpp"
#include "common/proto/pm.pb.h"
#include "pb_to_json.h"
#include "processmaster/event_manager.h"

#include <unistd.h>
#include <cstddef>

#include "common/logger.h"

#define DEFAULT_WAIT_TIME 1000

namespace dstream {
namespace event_manager {

bool CompareEventPtr::operator() (const EventPtr& event1_ptr,
                                  const EventPtr& event2_ptr) {
    if (event1_ptr->event_type() != event2_ptr->event_type()) {
        return event1_ptr->event_type() < event2_ptr->event_type();
    }
    return event1_ptr->Equal(*event2_ptr);
}

void* EventManagerWorkThread(void* ptr) {
    DSTREAM_INFO("[%s] Event Manager working Thread start...", __FUNCTION__);
    EventManager* event_ptr = static_cast<EventManager*>(ptr); // NOLINT
    event_ptr->HandleEvents();
    DSTREAM_INFO("[%s] Event Manager working Thread stop...", __FUNCTION__);
    return NULL;
}

EventManager::EventManager()
    : m_stop(false), m_suspend(false), m_wait_event(false), m_wait_time(DEFAULT_WAIT_TIME),
      m_wait_resume(false), m_working(false), m_working_thread(0), m_next_handle_time(0) {
}

EventManager::~EventManager() {
    Stop();
}

//  insert event to event_map, used internally
//  if event already in map return true
//  return false if insert fail
static bool AddEventToMap(EventMap& event_map, const EventPtr& event_ptr) {
    EventMapIter event_find = event_map.find(event_ptr->event_type());
    if (event_find == event_map.end()) {
        EventSet event_set;
        if (!event_set.insert(event_ptr).second ||
            !event_map.insert(EventPair(event_ptr->event_type(), event_set)).second) {
            DSTREAM_WARN("AddTo Event Map fail");
            return false;
        }
    } else {
        EventSet& event_set = event_find->second;
        if (event_set.find(event_ptr) == event_set.end()) {
            if (!event_set.insert(event_ptr).second) {
                DSTREAM_WARN("Insert event into set fail");
                return false;
            }
        } else {
            DSTREAM_INFO("Event already exists, Add Event fail");
            return false;
        }
    }
    return true;
}

static bool EraseEventFromMap(EventMap& event_map, const EventPtr& event_ptr) {
    EventMapIter event_find = event_map.find(event_ptr->event_type());
    if (event_find == event_map.end()) {
        return false;
    } else {
        EventSet& event_set = event_find->second;
        EventSetIter event_set_find = event_set.find(event_ptr);
        if (event_set_find == event_set.end()) {
            return false;
        } else {
            event_set.erase(event_set_find);
            return true;
        }
    }
}


bool EventManager::AddEvent(const EventPtr& event_ptr) {
    return AddDelayExecEvent(0, event_ptr);
}

bool EventManager::AddDelayExecEvent(uint64_t delay_time_ms,
                                     const EventPtr& event_ptr) {
    MutexLockGuard lock_guard(m_lock);
    if (m_stop || !m_working) {
        DSTREAM_WARN("[%s] event manager is not working", __FUNCTION__);
        return false;
    }

    EventMapIter all_find;
    EventSetIter all_set_find;
    all_find = m_all_event.find(event_ptr->event_type());
    if (all_find != m_all_event.end()) {
        all_set_find = all_find->second.find(event_ptr);
        if (all_set_find != all_find->second.end()) {
            DSTREAM_INFO("event already in all events");
            return false;
        }
    }

    // insert event into map
    uint64_t now_time = static_cast<uint64_t>(GetTimeMS());
    uint64_t execute_time = now_time + delay_time_ms;
    std::map<uint64_t, EventMap>::iterator find;
    find = m_delay_events.find(execute_time);
    if (find == m_delay_events.end()) {
        std::pair<uint64_t, EventMap> insert_pair(execute_time, EventMap());
        // note: there will not check if insert successfully
        find = m_delay_events.insert(insert_pair).first;
    }
    if (!AddEventToMap(find->second, event_ptr)) {
        return false;
    }

    // insert event into all event
    if (!AddEventToMap(m_all_event, event_ptr)) {
        DSTREAM_WARN("Insert event into all event map failed");
    }

    // check event manager state, signal it if necessary
    if (m_wait_event && m_condition.PtrValid()) {
        if (m_next_handle_time > execute_time) {
            m_wait_event = false;
            m_sleep_cond.Signal();
        }
    }
    return true;
}

void EventManager::HandleEvents() {
    // first get lock
    m_lock.lock();
    while (m_working && !m_stop) {
        if (m_suspend) {
            m_wait_resume = true;
            m_condition->wait();
            continue;
        }

        // if delay_events is empty wait for events
        if (m_delay_events.empty()) {
            m_wait_time = DEFAULT_WAIT_TIME;
            WaitForEvents();
            continue;
        }
        // get events that can execute
        m_execu_map.clear();
        std::map<uint64_t, EventMap>::iterator event_it, event_end;
        uint64_t now_time = static_cast<uint64_t>(GetTimeMS());
        for (event_it = m_delay_events.begin(); event_it != m_delay_events.end();) {
            if (event_it->first > now_time) {
                m_wait_time = event_it->first - now_time;
                break;
            }
            for (EventMapIter it = event_it->second.begin();
                 it != event_it->second.end();
                 it++) {
                for (EventSetIter set_it = it->second.begin();
                     set_it != it->second.end();
                     set_it++) {
                    AddEventToMap(m_execu_map, *set_it);
                    // EventPtr event = *set_it;
                    if (!EraseEventFromMap(m_all_event, *set_it)) {
                        DSTREAM_WARN("Erase Event [%d] From Map fail", (*set_it)->event_type());
                    }
                }
            }
            m_delay_events.erase(event_it++);
        }
        // if there is no events to handle, wait for new events or timeout
        if (m_execu_map.empty()) {
            WaitForEvents();
            continue;
        }
        m_lock.unlock();
        HandleEvents(&m_execu_map);
        m_execu_map.clear();
        // get lock before while loop
        m_lock.lock();
    }
    m_lock.unlock();
    DSTREAM_INFO("[%s] event manager is stoped", __FUNCTION__);
}

void EventManager::WaitForEvents() {
    m_wait_event = true;
    uint64_t wait_time = m_wait_time;
    m_next_handle_time = static_cast<uint64_t>(GetTimeMS()) + m_wait_time;
    m_lock.unlock();
    if (wait_time > 0) {
        m_sleep_cond.Wait(wait_time);
        m_sleep_cond.Init();
    }
    m_lock.lock();
    if (m_wait_event) {
        m_wait_event = false;
    }
}

bool EventManager::Start() {
    MutexLockGuard lock_guard(m_lock);
    if (m_working) {
        return true;
    }
    if (!m_condition.PtrValid()) {
        if ((m_condition = auto_ptr::AutoPtr<Condition>(new Condition(m_lock))) &&
            !m_condition.PtrValid()) {
            DSTREAM_WARN("new condition fail");
            return false;
        }
    }
    if (pthread_create(&m_working_thread, NULL, EventManagerWorkThread, this) != 0) {
        DSTREAM_WARN("start event manager working thread fail");
        return false;
    }
    DSTREAM_INFO("[%s] event manager start", __FUNCTION__);
    return (m_working = true);
}

void EventManager::Stop() {
    m_lock.lock();
    if (m_working) {
        DSTREAM_INFO("[%s] event manager is stoping...", __FUNCTION__);
        m_working = false;
        if (m_condition.PtrValid() && (m_wait_resume || m_wait_event)) {
            if (m_wait_resume) {
                m_wait_resume = false;
                m_condition->notifyAll();
                DSTREAM_DEBUG("notify working thread..");
            } else {
                if (m_wait_event) {
                    DSTREAM_DEBUG("notify working thread..");
                    m_wait_event = false;
                    m_sleep_cond.Signal();
                }
            }
        }
    }
    if (m_working_thread > 0) {
        pthread_t working_thread_tmp = m_working_thread;
        m_working_thread = 0;
        m_lock.unlock();
        DSTREAM_INFO("[%s] waitting for working thread over", __FUNCTION__);
        pthread_join(working_thread_tmp, NULL);
        return;
    }
    m_lock.unlock();
}

void EventManager::set_working(bool working) {
    MutexLockGuard lock_guard(m_lock);
    m_working = working;
}

int EventManager::EventSize() {
    int size = 0;
    MutexLockGuard lock_guard(m_lock);
    std::map<uint64_t, EventMap>::iterator it, end;
    for (it = m_delay_events.begin(), end = m_delay_events.end();
         it != end; ++it) {
        for (EventMapIter event_it = it->second.begin();
             event_it != it->second.end(); ++event_it) {
            size += event_it->second.size();
        }
    }
    return size;
}

int EventManager::AllEventSize() {
    int size = 0;
    MutexLockGuard lock_guard(m_lock);
    for (EventMapIter all_it = m_all_event.begin();
         all_it != m_all_event.end(); ++all_it) {
        size += all_it->second.size();
    }
    return size;
}

int EventManager::ExecuEventSize() {
    int size = 0;
    MutexLockGuard lock_guard(m_lock);
    for (EventMapIter event_it = m_execu_map.begin();
         event_it != m_execu_map.end(); ++event_it) {
        size += event_it->second.size();
    }
    return size;
}

int EventManager::EventTypeSize() {
    int size = 0;
    MutexLockGuard lock_guard(m_lock);
    std::map<uint64_t, EventMap>::iterator it;
    for (it = m_delay_events.begin(); it != m_delay_events.end(); it++) {
        size += it->second.size();
    }
    return size;
}

void EventManager::Suspend() {
    MutexLockGuard lock_guard(m_lock);
    m_suspend = true;
}

void EventManager::Resume() {
    MutexLockGuard lock_guard(m_lock);
    if (!m_suspend) {
        return;
    }
    m_suspend = false;
    if (m_condition.PtrValid() && m_wait_resume) {
        m_wait_resume = false;
        DSTREAM_INFO("resume event manager");
        m_condition->notifyAll();
    }
}

bool EventManager::wait_event() {
    MutexLockGuard lock_guard(m_lock);
    return m_wait_event;
}

void EventManager::SignalSleep() {
    m_sleep_cond.Signal();
}

void EventManager::HandleEvents(EventMap* events) {
    for (EventMapIter event_it = events->begin();
         event_it != events->end(); event_it++) {
        for (EventSet::iterator it = event_it->second.begin();
             it != event_it->second.end(); it++) {
            (*it)->HandleEvent();
        }
    }
}

void EventManager::ClearEvents() {
    MutexLockGuard lock_guard(m_lock);
    m_delay_events.clear();
}

dstream::error::Code EventManager::Dump(std::string *output) {
    // take snapshot
    m_lock.lock();
    // copy execute_event_map
    EventMapIter em_iter = m_execu_map.begin();
    EventMapPB execute_event_map_pb;
    for (; em_iter != m_execu_map.end(); ++em_iter) {
        EventKV* event_kv;
        event_kv = execute_event_map_pb.add_event_kv();
        event_kv->set_event_type(event::EventTypeStr[em_iter->first]);
        EventSetValue* event_set_value = event_kv->mutable_event_set();
        EventSetIter es_iter = em_iter->second.begin();
        for (; es_iter != em_iter->second.end(); ++es_iter) {
            std::string event = !(*es_iter)->comp_str().empty() ? (*es_iter)->comp_str() : "NULL"; 
            event_set_value->add_event_description(event);
        }
    }
    // copy all_event_map
    EventMapPB all_event_map_pb;
    em_iter = m_all_event.begin();
    for (; em_iter != m_all_event.end(); ++em_iter) {
        EventKV* event_kv;
        event_kv = all_event_map_pb.add_event_kv();
        event_kv->set_event_type(event::EventTypeStr[em_iter->first]);
        EventSetValue* event_set_value = event_kv->mutable_event_set();
        EventSetIter es_iter = em_iter->second.begin();
        for (; es_iter != em_iter->second.end(); ++es_iter) {
            std::string event = !(*es_iter)->comp_str().empty() ? (*es_iter)->comp_str() : "NULL"; 
            event_set_value->add_event_description(event);
        }
    }
    // copy m_delay_events
    DelayEventPB delay_event_map_pb;
    std::map<uint64_t, EventMap>::iterator de_iter = m_delay_events.begin();
    for (; de_iter != m_delay_events.end(); ++de_iter) {
        uint64_t time_stamp = de_iter->first;
        EventMap event_map  = de_iter->second;
        DelayEventKV* delay_event_kv = delay_event_map_pb.add_delay_event_kv();
        delay_event_kv->set_timestamp(time_stamp);
        EventMapPB* event_map_pb = delay_event_kv->mutable_event_map();
        em_iter = event_map.begin();
        for (; em_iter != event_map.end(); ++em_iter) {
            EventKV* event_kv;
            event_kv = event_map_pb->add_event_kv();
            event_kv->set_event_type(event::EventTypeStr[em_iter->first]);
            EventSetValue* event_set_value = event_kv->mutable_event_set();
            EventSetIter es_iter = em_iter->second.begin();
            for (; es_iter != em_iter->second.end(); ++es_iter) {
                std::string event = !(*es_iter)->comp_str().empty() ? (*es_iter)->comp_str() : "NULL"; 
                event_set_value->add_event_description(event);
            }
        }

    }
    // complete copy, unlock the struct.
    m_lock.unlock();

    std::string json_str("");
    std::string tmp;
    json_str += "{\"excute_event_map\":";
    ProtoMessageToJson(execute_event_map_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += ",\"all_event_map\":";
    tmp.clear();
    ProtoMessageToJson(all_event_map_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += ",\"delay_event_map\":";
    tmp.clear();
    ProtoMessageToJson(delay_event_map_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += "}";
    output->assign(json_str);
    return dstream::error::OK;
}

} // namespace event_manager
} // namespace dstream
