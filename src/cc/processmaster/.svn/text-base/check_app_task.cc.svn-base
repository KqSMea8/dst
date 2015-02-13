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

#include "processmaster/check_app_task.h"
#include <list>
#include "common/application_tools.h"
#include "metamanager/meta_manager.h"
#include "processmaster/dstream_event.h"
#include "processmaster/event_manager.h"
#include "processmaster/gc.h"
#include "processmaster/kill_app_gc_task.h"
#include "processmaster/pm_common.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace check_app_task {
using namespace application_tools;
using namespace event;
using namespace dstream_event;
using namespace gc_task;
using namespace kill_app_gc_task;
using namespace pm_common;
using namespace pm_gc;
using namespace event_manager;
using namespace scheduler;
using meta_manager::MetaManager;

typedef std::list<ProcessorElement> PeList;
typedef PeList::iterator PeListIter;

bool AppCheckTask::RunCheckTask() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("meta is not init");
        return false;
    }
    std::list<Application> app_list;
    if (meta->GetApplicationList(&app_list) != error::OK) {
        DSTREAM_WARN("get application list fail");
        return false;
    }
    typedef std::list<Application>::iterator AppIter;
    DSTREAM_INFO("check app status, app number is : %zu", app_list.size());
    for (AppIter app_iter = app_list.begin();
         app_iter != app_list.end(); ++app_iter) {
        DSTREAM_INFO("app [%lu] status [%d]", app_iter->id().id(),
                     app_iter->status());
        if (app_iter->status() == RUN) {
            if (!AddApplicationEvent(*app_iter)) {
                DSTREAM_WARN("Add App [%lu] Event fail", app_iter->id().id());
                return false;
            }
            continue;
        }
        if (app_iter->status() == SUBMIT || app_iter->status() == KILL) {
            app_iter->set_status(KILL);
            if (meta->UpdateApplication(*app_iter) != error::OK) {
                DSTREAM_WARN("update application [%lu] status fail", app_iter->id().id());
                return false;
            }
            m_gc->AddGCTask(GcTaskAutoPtr(new KillAppGCTask(app_iter->id(), m_event_manager)));
        }
        if (app_iter->status() == ADDTOPO) {
            if (AddSubTree(*app_iter, m_event_manager, true) != error::OK) {
                DSTREAM_WARN("Add Sub Tree fail");
                return false;
            }
            app_iter->set_status(RUN);
            if (meta->UpdateApplication(*app_iter) == error::OK) {
                /*if (!AddApplicationEvent(*app_iter)) {
                      return false;
                  }*/
            } else {
                DSTREAM_WARN("update app [%lu] fail", app_iter->id().id());
                return false;
            }
        }
        if (app_iter->status() == DELTOPO) {
            if (DelSubTree(*app_iter, m_event_manager, m_scheduler) != error::OK) {
                DSTREAM_WARN("delete sub tree fail");
                return false;
            }
            app_iter->set_status(RUN);
            if (meta->UpdateApplication(*app_iter) == error::OK) {
                if (!AddApplicationEvent(*app_iter)) {
                    return false;
                }
            } else {
                DSTREAM_WARN("update app [%lu] fail", app_iter->id().id());
                return false;
            }
        }
        if (app_iter->status() == UPPARA) {
            if (UpdatePara(*app_iter, m_event_manager, m_scheduler) != error::OK) {
                DSTREAM_WARN("update parallelism fail");
                return false;
            }
            app_iter->set_status(RUN);
            if (meta->UpdateApplication(*app_iter) == error::OK) {
                if (!AddApplicationEvent(*app_iter)) {
                    return false;
                }
            } else {
                DSTREAM_WARN("update app [%lu] fail", app_iter->id().id());
                return false;
            }
        }
        /*if (app_iter->status() == ADD_REQ || app_iter->status() == DEL_REQ || app_iter->status() == UPPARA_REQ) {
          app_iter->set_status(RUN);
          if (meta->UpdateApplication(*app_iter) == error::OK) {
              return AddApplicationEvent(*app_iter);
          } else {
              DSTREAM_WARN("update app [%lu] fail", app_iter->id().id());
              return false;
          }
        }*/
        DSTREAM_INFO("check app [%lu] status success", app_iter->id().id());
    }
    return true;
}

bool AppCheckTask::AddApplicationEvent(const Application& app) {
    return m_event_manager->AddEvent(EventPtr(new AddAppEvent(app.id())));
}

} // namespace check_app_task
} // namespace dstream
