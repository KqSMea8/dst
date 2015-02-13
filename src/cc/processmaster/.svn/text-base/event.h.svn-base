/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/07
* @filename:    event.h
* @author:      zhanggengxin
* @brief:       enevt
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_EVENT_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_EVENT_H__ // NOLINT

#include <stdint.h>
#include <string>

namespace dstream {
namespace event {

/************************************************************************/
/* event type, the same type is belong to the same struct/class now
   event type is also corresponding to priority of event                  */
/************************************************************************/
enum EventType {
    PE_FAIL,
    OP_MIGRATE_PE,
    QUERY_PE_EXIST,
    CREATE_PE,
    ADD_APP,
    KILL_PE,
    RESTART_PE,
    PN_DECOMMISSION,
    MIGRATE_PE,
    KILL_APP,

    // update app event
    ADD_SUB_TREE,
    DEL_SUB_TREE,
    UPDATE_PARA,
    UPDATE_PRO,
    UPDATE_APP_STATUS,
};
const std::string EventTypeStr[] = {
"PE_FAIL",
"OP_MIGRATE_PE",
"QUERY_PE_EXIST",
"CREATE_PE",
"ADD_APP",
"KILL_PE",
"RESTART_PE",
"PN_DECOMMOSSION",
"MIGRATE_PE",
"KILL_APP",
"ADD_SUB_TREE",
"DEL_SUB_TREE",
"UPDATE_PARA",
"UPDATE_PRO",
"UPDATE_APP_STATUS"
};

/************************************************************************/
/* the abstract event class : define type and interface                 */
/************************************************************************/
class Event {
public:
    explicit Event(EventType type) : m_event_type(type) {}
    EventType event_type() const {
        return m_event_type;
    }
    std::string comp_str() const {
        return m_comp_str;
    }
    virtual ~Event() {}
    virtual int32_t HandleEvent() = 0;
    virtual int32_t HandleEventResult(int res, const void* data) = 0;
    // compare two event, used to avoid duplicate event
    virtual bool Equal(const Event& other) = 0;
    // compare the priority of the same event
    virtual bool less(const Event& other);
protected:
    EventType m_event_type;
    std::string m_comp_str;
};

} // namespace event
} // namespace dstream

#endif // NOLINT
