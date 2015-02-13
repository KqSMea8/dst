/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/04
* @filename:    update_app_event.cc
* @author:      zhanggengxin@baidu.com
* @brief:       update app status event
*/

#include "processmaster/update_app_event.h"

#include "metamanager/meta_manager.h"
#include "processmaster/pm_common.h"

namespace dstream {
namespace update_app_event {
using namespace event;
using namespace event_manager;
using namespace meta_manager;
using namespace pm_common;
using namespace scheduler;

UpdateAppStatusEvent::UpdateAppStatusEvent(const Application& app,
                                           EventManager* event_manager) :
    Event(UPDATE_APP_STATUS), m_app(app), m_event_manager(event_manager) {
        m_comp_str += "APPID=";
        StringAppendNumber(&m_comp_str, m_app.id().id());
}

bool UpdateAppStatusEvent::Equal(const event::Event& other) {
    if (event_type() != other.event_type()) {
        return false;
    }
    const UpdateAppStatusEvent& other_event = dynamic_cast<const UpdateAppStatusEvent&>
                                              (other); // NOLINT
    return m_app.id().id() < other_event.m_app.id().id();
}

int32_t UpdateAppStatusEvent::HandleEvent() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("meta is null");
        AddNewEvent();
    }
    if (meta->UpdateApplication(m_app) == error::OK) {
        DSTREAM_INFO("update application [%lu] status [%d]", m_app.id().id(),
                     m_app.status());
    } else {
        DSTREAM_WARN("update application [%lu] status [%d] fail", m_app.id().id(),
                     m_app.status());
        AddNewEvent();
    }
    return error::OK;
}

void UpdateAppStatusEvent::AddNewEvent() {
    if (NULL != m_event_manager) {
        m_event_manager->AddEvent(EventPtr(new UpdateAppStatusEvent(m_app,
                                           m_event_manager)));
    }
}

AddSubtreeEvent::AddSubtreeEvent(const Application& app,
                                 EventManager* event_manager) :
    UpdateAppStatusEvent(app, event_manager) {
    m_event_type = ADD_SUB_TREE;
}

int32_t AddSubtreeEvent::HandleEvent() {
    int32_t res = error::OK;
    if ((res = AddSubTree(m_app, m_event_manager)) != error::OK) {
        if (res == error::ZK_NO_NODE) {
            DSTREAM_INFO("[%s] Application [%ld] is not exist", __FUNCTION__, m_app.id().id());
            return res;
        }
        AddNewEvent();
    } else {
        m_app.set_status(RUN);
        m_event_type = UPDATE_APP_STATUS;
        UpdateAppStatusEvent::HandleEvent();
    }
    return res;
}

void AddSubtreeEvent::AddNewEvent() {
    m_event_manager->AddEvent(EventPtr(new AddSubtreeEvent(m_app, m_event_manager)));
}

DelSubtreeEvent::DelSubtreeEvent(const Application& app,
                                 EventManager* event_manager,
                                 Scheduler* scheduler) :
    UpdateAppStatusEvent(app, event_manager), m_scheduler(scheduler) {
    m_event_type = DEL_SUB_TREE;
}

int32_t DelSubtreeEvent::HandleEvent() {
    int32_t res = error::OK;
    if ((res = DelSubTree(m_app, m_event_manager, m_scheduler)) != error::OK) {
        if (res == error::ZK_NO_NODE) {
            DSTREAM_INFO("[%s] Application [%ld] is not exist", __FUNCTION__, m_app.id().id());
            return res;
        }
        AddNewEvent();
    } else {
        m_app.set_status(RUN);
        m_event_type = UPDATE_APP_STATUS;
        UpdateAppStatusEvent::HandleEvent();
    }
    return res;
}

void DelSubtreeEvent::AddNewEvent() {
    m_event_manager->AddEvent(EventPtr(new DelSubtreeEvent(m_app, m_event_manager,
                                       m_scheduler)));
}

UpdateParaEvent::UpdateParaEvent(const Application& app,
                                 EventManager* event_manager,
                                 Scheduler* scheduler) :
    UpdateAppStatusEvent(app, event_manager), m_scheduler(scheduler) {
    m_event_type = UPDATE_PARA;
}

int32_t UpdateParaEvent::HandleEvent() {
    int32_t ret = error::OK;
    if ((ret = UpdatePara(m_app, m_event_manager, m_scheduler)) != error::OK) {
        if (ret == error::ZK_NO_NODE) {
            DSTREAM_INFO("[%s] Application [%ld] is not exist", __FUNCTION__, m_app.id().id());
            return ret;
        }
        AddNewEvent();
    } else {
        m_app.set_status(RUN);
        m_event_type = UPDATE_APP_STATUS;
        UpdateAppStatusEvent::HandleEvent();
    }
    return ret;
}

void UpdateParaEvent::AddNewEvent() {
    m_event_manager->AddEvent(EventPtr(new UpdateParaEvent(m_app, m_event_manager,
                                       m_scheduler)));
}

} // namespace update_app_event
} // namespace dstream
