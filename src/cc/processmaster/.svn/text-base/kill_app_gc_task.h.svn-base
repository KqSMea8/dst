/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    kill_app_gc_task.h
* @author:      zhanggengxin@baidu.com
* @brief:       gc kill app task
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_KILL_APP_GC_TASK_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_KILL_APP_GC_TASK_H__ // NOLINT

#include "common/proto/application.pb.h"
#include "processmaster/event_manager.h"
#include "processmaster/gc_task.h"

namespace dstream {
namespace kill_app_gc_task {

/************************************************************************/
/* Garbage collection : collect killed application                       */
/************************************************************************/
class KillAppGCTask : public gc_task::GCTask {
public:
    KillAppGCTask(const AppID& app_id, event_manager::EventManager* event_manager);
    int32_t RunTask(scheduler::Scheduler* scheduler = 0);
private:
    int32_t KillApplication(const Application& app, scheduler::Scheduler* scheduler);
    AppID m_kill_app_id;
    event_manager::EventManager* m_event_manager;
};

} // namespace kill_app_gc_task
} // namespace dstream
#endif // NOLINT
