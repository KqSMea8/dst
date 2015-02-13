/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/07
* @filename:    event_manager.h
* @author:      zhanggengxin
* @brief:       event manager
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_EVENT_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_EVENT_MANAGER_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <utility>
#include "common/dstream_type.h"
#include "common/error.h"
#include "processmaster/event.h"

namespace dstream {
namespace event_manager {

using event::Event;
using event::EventType;
typedef auto_ptr::AutoPtr<Event> EventPtr;

/************************************************************************/
/* compare two event used to avoid add duplicate event                  */
/************************************************************************/
class CompareEventPtr {
public:
    bool operator()(const EventPtr& event_ptr1, const EventPtr& event_ptr2);
};

// event manager thread
void* EventManagerWorkThread(void* ptr);
typedef std::set<EventPtr, CompareEventPtr> EventSet;
typedef std::map<EventType, EventSet> EventMap;
typedef EventMap::iterator EventMapIter;
typedef EventMap::const_iterator ConstEventMapIter;
typedef std::pair<EventType, EventSet> EventPair;
typedef EventSet::iterator EventSetIter;
/************************************************************************/
/* EventManager : manage and handle event                               */
/************************************************************************/
class EventManager {
public:
    EventManager();
    virtual ~EventManager();
    /**
     * @brief AddEvent
     * @param [in] event_ptr
     * return true on success, if event already exists return true
     */
    bool AddEvent(const EventPtr& event_ptr);

    /**
     * @brief Add delay execute event
     * @param [in] delay_time_ms : delay execute time in millisecond
     *                             if delay_time_ms < 0 the delay_time_ms will be set to 0
     * @param [in] event_ptr
     * return true on success, if event already exists return true
     */
    bool AddDelayExecEvent(uint64_t delay_time_ms, const EventPtr& event_ptr);

    bool Start();
    void Stop();
    void Suspend();
    void Resume();
    int EventSize();
    int ExecuEventSize();
    int AllEventSize();
    int EventTypeSize();
    bool stop() const {
        return m_stop;
    }
    bool suspend() const {
        return m_suspend;
    }
    bool wait_event();
    void set_working(bool working);

    // signal for sleep : used for test
    void SignalSleep();
    void set_wait_time(int wait_time) {
        m_wait_time = wait_time;
    }
    // clear events, used for test
    void ClearEvents();
    // dump Event, used for httpd
    virtual dstream::error::Code Dump(std::string *output);

protected:
    virtual void HandleEvents(EventMap* events);

private:
    friend void* EventManagerWorkThread(void* ptr);
    void HandleEvents();
    /*
     * @brief wait for events or timeout, used in HandleEvents
     * before call this method lock has already locked
     */
    void WaitForEvents();
    bool m_stop;
    bool m_suspend;
    bool m_wait_event;
    int m_wait_time;
    bool m_wait_resume;
    bool m_working;
    pthread_t m_working_thread;
    MutexLock m_lock;
    auto_ptr::AutoPtr<Condition> m_condition;
    EventMap m_execu_map; // TODO
    EventMap m_all_event; 
    CCond m_sleep_cond;
    // delay execute events map
    std::map<uint64_t, EventMap> m_delay_events; 
    // last wait time, used for signal event_manager if it is waiting
    uint64_t m_next_handle_time;
};

} // namespace event_manager
} // namespace dstream

#endif // NOLINT 
