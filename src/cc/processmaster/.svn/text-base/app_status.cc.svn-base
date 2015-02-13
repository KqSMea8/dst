/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2013/7/2013/07/01
* @filename:    app_status.cc
* @author:      fangjun02@baidu.com
* @brief:       app status functions used for pm
*/


#include "processmaster/app_status.h"
#include <list>
#include "common/id.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace app_status {
using namespace meta_manager;
using namespace application_tools;

AppStatus* AppStatus::m_app_status_instance = NULL;

AppStatus* AppStatus::GetAppStatus() {
    return m_app_status_instance;
}

int32_t AppStatus::InitAppStatus() {
    if (NULL == m_app_status_instance) {
        m_app_status_instance = new AppStatus();
    } else {
    }
    int32_t ret = error::OK;
    MetaManager* meta = MetaManager::GetMetaManager();
    std::list<Application> app_list;
    std::list<Application>::iterator app_list_iter;
    if ((ret = meta->GetApplicationList(&app_list)) != error::OK) {
        DSTREAM_WARN("fail to get app list");
        return ret;
    }
    for (app_list_iter = app_list.begin();
         app_list_iter != app_list.end();
         app_list_iter++) {
        const Topology& tp = app_list_iter->topology();
        for (uint32_t i = 0; i < tp.processor_num(); i++) {
            ProcessorID pid = tp.processors(i).id();
            int revision = tp.processors(i).cur_revision();
            if (m_app_status_instance->AddProcessor(MAKE_APP_PROCESSOR_ID(app_list_iter->id().id(),
                                                    pid.id()), revision) != error::OK) {
                DSTREAM_WARN("fail to add processor [%s]", tp.processors(i).name().c_str());
            }
        }
    }
    return ret;
}

void AppStatus::DestroyAppStatus() {
    if (NULL != m_app_status_instance) {
        delete m_app_status_instance;
        m_app_status_instance = NULL;
    }
}

void AppStatus::UpdateApplication(const Application& app) {
    MutexLockGuard mutex_guard(m_lock);
    PidRevMap pid_map, pid_map_zk, pid_add, pid_delete;
    PidRevMapIter iter;
    // find all pid of this app
    for (iter = m_pid_rev_map.begin();
         iter != m_pid_rev_map.end();
         iter++) {
        if (app.id().id() == EXTRACT_APP_ID_FROM_APPPRO_ID(iter->first)) {
            pid_map.insert(*iter);
        }
    }
    // all pid of this app on zk
    const Topology& tp = app.topology();
    for (int32_t i = 0; i < tp.processors().size(); i++) {
        ProcessorID pid = tp.processors(i).id();
        pid_map_zk.insert(PidRevPair(MAKE_APP_PROCESSOR_ID(app.id().id(),
                                     pid.id()), tp.processors(i).cur_revision()));
    }

    // diff all processor in this app from pid_set
    GetDifference(pid_map_zk, pid_map, pid_add);
    GetDifference(pid_map, pid_map_zk, pid_delete);

    // for all add pid add processor
    for (iter = pid_add.begin(); iter != pid_add.end(); iter++) {
        m_pid_rev_map.insert(*iter);
    }
    // for all delete pid erase processor
    for (iter = pid_delete.begin(); iter != pid_delete.end(); iter++) {
        m_pid_rev_map.erase(iter->first);
    }
}

void AppStatus::GetDifference(const PidRevMap& large_map,
                              const PidRevMap& less_map, PidRevMap& diff_map) {
    for (PidRevMapConstIter it = large_map.begin(); it != large_map.end(); it++) {
        if (less_map.find(it->first) == less_map.end()) {
            diff_map.insert(*it);
        }
    }
}

int32_t AppStatus::AddProcessor(const uint64_t& pid, const unsigned int& revision) {
    MutexLockGuard mutex_guard(m_lock);
    PidRevMapIter iter = m_pid_rev_map.find(pid);
    UNLIKELY_IF(iter != m_pid_rev_map.end()) {
        return error::ADD_APPSTATUS_FAIL;
    } else {
        std::pair<PidRevMapIter, bool> ret = m_pid_rev_map.insert(PidRevPair(pid, revision));
        UNLIKELY_IF(!ret.second) {
            return error::ADD_APPSTATUS_FAIL;
        }
    }
    return error::OK;
}

int32_t AppStatus::DelProcessor(const uint64_t& pid) {
    MutexLockGuard mutex_guard(m_lock);
    PidRevMapIter iter = m_pid_rev_map.find(pid);
    UNLIKELY_IF(iter == m_pid_rev_map.end()) {
        return error::DEL_APPSTATUS_FAIL;
    } else {
        m_pid_rev_map.erase(pid);
    }
    return error::OK;
}

int32_t AppStatus::UpdateProcessor(const uint64_t& pid, const unsigned int& revision) {
    MutexLockGuard mutex_guard(m_lock);
    PidRevMapIter iter = m_pid_rev_map.find(pid);
    UNLIKELY_IF(iter == m_pid_rev_map.end()) {
        return error::UPD_APPSTATUS_FAIL;
    } else {
        iter->second = revision;
    }
    return error::OK;
}

int32_t AppStatus::GetProcessorRevision(const uint64_t& pid, unsigned int* revision) {
    MutexLockGuard mutex_guard(m_lock);
    PidRevMapIter iter = m_pid_rev_map.find(pid);
    UNLIKELY_IF(iter == m_pid_rev_map.end()) {
        return error::GET_APPSTATUS_FAIL;
    } else {
        *revision = iter->second;
    }
    return error::OK;
}

} // namespace app_status
} // namespace dstream
