/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/7/2012/07/01
* @filename:    gc_task.h
* @author:      zhanggengxin@baidu.com
* @brief:       gc task functions used for pm
*/

#ifndef __DSTREAM_PROCESSMASTER_GC_TASK_H__ // NOLINT
#define __DSTREAM_PROCESSMASTER_GC_TASK_H__ // NOLINT

#include "scheduler/scheduler.h"

namespace dstream {
namespace gc_task {

class GCTask {
public:
    virtual ~GCTask() {}
    virtual int32_t RunTask(scheduler::Scheduler* scheduler = NULL) = 0;
};

typedef auto_ptr::AutoPtr<GCTask> GcTaskAutoPtr;

} // namespace gc_task
} // namespace dstream

#endif // NOLINT 
