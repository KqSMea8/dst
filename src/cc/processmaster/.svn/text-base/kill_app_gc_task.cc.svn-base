/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    kill_app_gc_task.cc
* @author:      zhanggengxin@baidu.com
* @brief:       gc kill app task
*/

#include "processmaster/kill_app_gc_task.h"

#include "metamanager/meta_manager.h"
#include "processmaster/dstream_event.h"
#include "processmaster/pm_common.h"

namespace dstream {
namespace kill_app_gc_task {
using namespace dstream_event;
using namespace application_tools;
using namespace meta_manager;
using namespace event_manager;
using namespace pm_common;
using namespace scheduler;

KillAppGCTask::KillAppGCTask(const AppID& app_id,
                             EventManager* event_manager) :
    m_kill_app_id(app_id), m_event_manager(event_manager) {
}

int32_t KillAppGCTask::RunTask(Scheduler* schduler /*= NULL*/) {
    DSTREAM_INFO("Run KillAppGCTask [%lu] Task", m_kill_app_id.id());
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("meta is not init");
        return error::META_INVALID;
    }
    int res = error::OK;
    Application app;
    if ((res = meta->GetApplication(m_kill_app_id, &app)) != error::OK) {
        if (res == error::ZK_NO_NODE) {
            return error::OK;
        }
        DSTREAM_WARN("Get App [%lu] fail error code is:[%d]", m_kill_app_id.id(), res);
        return res;
    }
    if (app.status() == SUBMIT) {
        app.set_status(KILL);
        if ((res = meta->UpdateApplication(app)) != error::OK) {
            DSTREAM_WARN("Update Application [%lu] fail", m_kill_app_id.id());
            return res;
        }
        return KillApplication(app, schduler);
    }
    if (app.status() == ADDTOPO || app.status() == DELTOPO || app.status() == UPPARA) {
        app.set_status(RUN);
        if ((res = meta->UpdateApplication(app)) == error::OK) {
            m_event_manager->AddEvent(EventPtr(new AddAppEvent(app.id())));
            return error::OK;
        }
        DSTREAM_WARN("Update App [%lu] fail", m_kill_app_id.id());
        return res;
    }
    return KillApplication(app, schduler);
}

int32_t KillAppGCTask::KillApplication(const Application& app, Scheduler* scheduler) {
    if (app.status() == KILL) {
        if (NULL != scheduler) {
            const Topology& topology = app.topology();
            for (int i = 0; i < topology.processors_size(); ++i) {
                scheduler->RemoveResRequest(ConstructProcessorID(app.id(),
                                                                 topology.processors(i).id()));
            }
        }
        return KillApp(m_event_manager, app);
    }
    return error::OK;
}

} // namespace kill_app_gc_task
} // namespace dstream
