/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/3/2012/03/31
* @filename:    check_app_task
* @author:      zhanggengxin@baidu.com
* @brief:       Check App Staus when pm start up
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_CHECK_APP_TASK_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_CHECK_APP_TASK_H__ // NOLINT

#include "processmaster/gc.h"
#include "processmaster/pm_start_check.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace check_app_task {

class AppCheckTask : public pm_start_check::PMStartCheckTask {
public:
    AppCheckTask(event_manager::EventManager* event_manager,
                 pm_gc::GC* gc, scheduler::Scheduler* scheduler) :
        m_event_manager(event_manager), m_gc(gc), m_scheduler(scheduler) {
        m_name = "AppCheckTask";
    }
    bool RunCheckTask();
private:
    bool AddApplicationEvent(const Application& app);
    event_manager::EventManager* m_event_manager;
    pm_gc::GC* m_gc;
    scheduler::Scheduler* m_scheduler;
};

} // namespace check_app_task
} // namespace dstream

#endif // NOLINT
