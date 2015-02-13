/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/04
* @filename:    update_app_event.h
* @author:      zhanggengxin@baidu.com
* @brief:       update app status event
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_UPDATE_APP_EVENT_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_UPDATE_APP_EVENT_H__ // NOLINT

#include "common/proto/application.pb.h"
#include "processmaster/event_manager.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace update_app_event {

/************************************************************************/
/* update app status event                                              */
/************************************************************************/
class UpdateAppStatusEvent : public event::Event {
public:
    UpdateAppStatusEvent(const Application& app, event_manager::EventManager* event_manager);
    int32_t HandleEvent();
    int32_t HandleEventResult(int /*res*/, const void* /*data*/) {
        return 0;
    }
    bool Equal(const event::Event& other);
protected:
    virtual void AddNewEvent();
    Application m_app;
    event_manager::EventManager* m_event_manager;
};

/************************************************************************/
/* add subt tree event                                                  */
/************************************************************************/
class AddSubtreeEvent : public UpdateAppStatusEvent {
public:
    AddSubtreeEvent(const Application& app, event_manager::EventManager* event_manager);
    int32_t HandleEvent();
protected:
    void AddNewEvent();
};

/************************************************************************/
/* delete subtree event                                                  */
/************************************************************************/
class DelSubtreeEvent : public UpdateAppStatusEvent {
public:
    DelSubtreeEvent(const Application& app,
                    event_manager::EventManager* event_manager,
                    scheduler::Scheduler* scheduler);
    int32_t HandleEvent();
protected:
    void AddNewEvent();
private:
    scheduler::Scheduler* m_scheduler;
};

/************************************************************************/
/* update parallelism event                                             */
/************************************************************************/
class UpdateParaEvent : public UpdateAppStatusEvent {
public:
    UpdateParaEvent(const Application& app,
                    event_manager::EventManager* event_manager,
                    scheduler::Scheduler* scheduler);
    int32_t HandleEvent();
protected:
    void AddNewEvent();
private:
    scheduler::Scheduler* m_scheduler;
};

} // namespace update_app_event
} // namespace dstream

#endif // NOLINT
