/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    garbage collection
*********************************************************************/

#ifndef __DSTREAM_PROCESSMASTER_GC_H__ // NOLINT
#define __DSTREAM_PROCESSMASTER_GC_H__ // NOLINT

#include <list>
#include <set>
#include <string>
#include "common/application_tools.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "processmaster/event_manager.h"
#include "processmaster/gc_task.h"

namespace dstream {
namespace pm_gc {

// define gc task list type
typedef std::list<gc_task::GcTaskAutoPtr> GcTaskList;
typedef GcTaskList::iterator GcTaskListIter;

/************************************************************************/
/* Garbage collection : gc unassigned task and killed task              */
/************************************************************************/
class GC : public thread::Thread {
public:
    typedef auto_ptr::AutoPtr<event_manager::EventManager> EventManagerAutoPtr;
    typedef auto_ptr::AutoPtr<scheduler::Scheduler> SchedulerAutoPtr;
    // set scheduler default to NULL for not modify test case
    GC(config::Config* config, EventManagerAutoPtr event_manager, SchedulerAutoPtr scheduler);
    void AddGCTask(const gc_task::GcTaskAutoPtr& task);
    void Run();
    void set_gc_interval(int gc_interval) {
        m_gc_interval = gc_interval;
    }
    void SignalGCSleep() {
        m_sleep_condition.Signal();
    }
private:
    void CheckPEOnPN(const PNReport& pn_report, application_tools::PeSet& pe_set);
    void CheckBackupPE(const ProcessorElement& pe,
                       const BackupPEID& backup_pe_id,
                       const PNID& pn_id);
    config::Config* m_config;
    EventManagerAutoPtr m_event_manager;
    SchedulerAutoPtr m_scheduler;
    GcTaskList m_gc_task_list;
    int m_gc_interval;
};

} // namespace pm_gc
} // namespace dstream

#endif // NOLINT
