/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/06
* @filename:    memory_meta_manager.cc
* @author:      zhanggengxin
* @brief:       memory meta data manager
*/

#include "metamanager/memory_meta_manager.h"
#include <sys/timeb.h>
#include <cstddef>
#include "common/logger.h"

namespace dstream {
namespace memory_meta_manager {

MemoryMetaManager::MemoryMetaManager() {
    m_backup_pm_serial = 0;
}

MemoryMetaManager::~MemoryMetaManager() {
}

ERROR_CODE MemoryMetaManager::AddAppProcessElements(const AppID& app,
    const list<ProcessorElement>& pe_list) {
    DSTREAM_DEBUG("add app pe size %zu", pe_list.size());
    PE_SET pe_set;
    APP_PE_MAP_ITER pe_insert = app_pe_map_.insert(APP_PE_PAIR(app, pe_set)).first;
    for (list<ProcessorElement>::const_iterator pe_iter = pe_list.begin(); pe_iter != pe_list.end();
         ++pe_iter) {
        if (!pe_insert->second.insert(*pe_iter).second) {
            DSTREAM_DEBUG("add pe fail");
        }
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetClusterConfig(ClusterConfig* config) {
    if (m_cluster_config.PtrValid()) {
        *config = *m_cluster_config;
        return error::OK;
    }
    return error::CONFIG_NOT_EXIST;
}

ERROR_CODE MemoryMetaManager::UpdateClusterConfig(const ClusterConfig& config) {
    AutoPtr<ClusterConfig> new_config(new ClusterConfig);
    *new_config = config;
    m_cluster_config = new_config;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::UpdateApplication(const Application& app) {
    MutexLockGuard lock_guard(lock_);
    APP_SET_ITER find;
    find = m_app_set.find(app);
    if (find == m_app_set.end()) {
        DSTREAM_WARN("update app fail, app not exist");
        return error::APP_NOT_EXIST;
    }
    const_cast<Application&>(*find) = app;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::UpdateProcessElement(const AppID& app_id,
                                                   const ProcessorElement& pe) {
    MutexLockGuard lock_guard(lock_);
    PE_SET_ITER pe_find;
    PE_INSERT_RES add_pe_res;
    PE_SET* pe_set;
    APP_PE_MAP_ITER app_pe_find = app_pe_map_.find(app_id);
    if (app_pe_find == app_pe_map_.end()) {
        DSTREAM_WARN("app not exist");
        return error::APP_NOT_EXIST;
    }
    pe_set = &(app_pe_find->second);
    pe_find = pe_set->find(pe);
    if (pe_find == pe_set->end()) {
        add_pe_res = pe_set->insert(pe);
        if (!add_pe_res.second) {
            DSTREAM_WARN("add pe error");
            return error::APP_PE_ERROR;
        }
    } else {
        const_cast<ProcessorElement&>(*pe_find) = pe;
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetApplicationList(list<Application>* app_list) {
    MutexLockGuard lock_guard(lock_);
    APP_SET_ITER end = m_app_set.end();
    for (APP_SET_ITER iter = m_app_set.begin(); iter != end; ++iter) {
        app_list->push_back(*iter);
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetAppIDList(std::list<AppID>* id_list) {
    MutexLockGuard lock_guard(lock_);
    APP_SET_ITER end = m_app_set.end();
    for (APP_SET_ITER iter = m_app_set.begin(); iter != end; ++iter) {
        id_list->push_back(iter->id());
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetApplication(const AppID& app_id, Application* app) {
    MutexLockGuard lock_guard(lock_);
    //  *(app->mutable_id()) = app_id;
    app->set_id(app_id);
    APP_SET_ITER app_find = m_app_set.find(*app);
    if (app_find == m_app_set.end()) {
        DSTREAM_WARN("cannot find app");
        return error::APP_NOT_EXIST;
    }
    *app = *app_find;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetAppProcessElements(const AppID& app_id,
        std::list<ProcessorElement>* pe_list) {
    MutexLockGuard lock_guard(lock_);
    APP_PE_MAP_ITER app_find = app_pe_map_.find(app_id);
    if (app_find == app_pe_map_.end()) {
        DSTREAM_WARN("cannot find app");
        return error::APP_NOT_EXIST;
    } else {
        PE_SET_ITER end = app_find->second.end();
        for (PE_SET_ITER iter = app_find->second.begin(); iter != end; ++iter) {
            pe_list->push_back(*iter);
        }
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetAppFlowControl(const AppID& /*app_id*/,
                                                const std::string& /*processor*/,
                                                FlowControl*) {
    // TODO(fangjun02)
    MutexLockGuard lock_guard(lock_);
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetPNProcessElements(const PNID& pn_id,
        set<ProcessorElement, ComparePE>& pe_list) {
    MutexLockGuard lock_guard(lock_);
    APP_PE_MAP_ITER app_pe_end = app_pe_map_.end();
    for (APP_PE_MAP_ITER app_pe_iter = app_pe_map_.begin();
         app_pe_iter != app_pe_end; ++app_pe_iter) {
        PE_SET_ITER pe_end = app_pe_iter->second.end();
        for (PE_SET_ITER pe_iter = app_pe_iter->second.begin();
             pe_iter != pe_end;
             ++pe_iter) {
            int backup_pe_size = pe_iter->backups_size();
            for (int i = 0; i < backup_pe_size; ++i) {
                const dstream::BackupPE& backup_pe = pe_iter->backups(i);
                if (!backup_pe.has_pn_id() || backup_pe.pn_id().id() == pn_id.id()) {
                    pe_list->insert(*pe_iter);
                }
            }
        }
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetPNProcessElementsMap(PNID_PESET_MAP* pn_pe_map) {
    MutexLockGuard lock_guard(lock_);
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetProcessorElement(const AppID& app_id, const PEID& pe_id,
                                                  ProcessorElement* pe) {
    MutexLockGuard lock_guard(lock_);
    APP_PE_MAP_ITER app_find = app_pe_map_.find(app_id);
    if (app_find == app_pe_map_.end()) {
        DSTREAM_WARN("app not exist");
        return error::APP_NOT_EXIST;
    }
    // *(pe.mutable_pe_id()) = pe_id;
    // *(pe.mutable_app()) = app_id;
    pe->set_pe_id(pe_id);
    pe->set_app(app_id);
    PE_SET_ITER pe_find = app_find->second.find(*pe);
    if (pe_find == app_find->second.end()) {
        DSTREAM_WARN("pe not exist");
        return error::PE_NOT_EXIST;
    }
    *pe = *pe_find;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::AddPM(const PM& pm) {
    MutexLockGuard lock_guard(lock_);
    if (m_pm.PtrValid()) {
        DSTREAM_WARN("pm already exist");
        return error::ZK_NODE_EXISTS;
    }
    m_pm = AutoPtr<PM>(new PM);
    if (!m_pm.PtrValid()) {
        DSTREAM_WARN("create pm fail");
        return error::ADD_PM_FAIL;
    }
    *m_pm = pm;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::AddBackupPM(PM* pm) {
    MutexLockGuard lock_guard(lock_);
    char pm_id[1000];
    snprintf(pm_id, sizeof(pm_id), "%d", m_backup_pm_serial++);
    pm->set_id(pm_id);
    m_backup_pm_list.push_back(*pm);
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetBackupPM(list<PM>* backup_pm_list) {
    *backup_pm_list = m_backup_pm_list;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::DeleteBackupPM(const PM& pm) {
    MutexLockGuard lock_guard(lock_);
    for (list<PM>::iterator it = m_backup_pm_list.begin();
         it != m_backup_pm_list.end(); ++it) {
        if (it->id() == pm.id()) {
            m_backup_pm_list.erase(it);
            return error::OK;
        }
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetPM(PM* pm) {
    MutexLockGuard lock_guard(lock_);
    if (!m_pm.PtrValid()) {
        DSTREAM_WARN("pm not exist");
        return error::PM_NOT_EXIST;
    }
    *pm = *m_pm;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::UpdatePM(const PM& pm) {
    MutexLockGuard lock_guard(lock_);
    AutoPtr<PM> new_pm(new PM);
    if (!new_pm.PtrValid()) {
        DSTREAM_WARN("create new pm fail");
        return error::ADD_PM_FAIL;
    }
    *new_pm = pm;
    m_pm = new_pm;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::DeletePM() {
    MutexLockGuard lock_guard(lock_);
    //  m_pm.Release();
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetPN(const PNID& pn_id, PN* pn) {
    MutexLockGuard lock_guard(lock_);
    // *(pn.mutable_pn_id()) = pn_id;
    pn->set_pn_id(pn_id);
    PN_SET_ITER pn_find = m_pn_set.find(pn);
    if (pn_find == m_pn_set.end()) {
        DSTREAM_WARN("pn not exist");
        return error::PN_NOT_EXIST;
    }
    pn->CopyFrom(*pn_find);
    return error::OK;
}

ERROR_CODE MemoryMetaManager::GetPNList(list<PN>* pn_list) {
    MutexLockGuard lock_guard(lock_);
    PN_SET_ITER end = m_pn_set.end();
    for (PN_SET_ITER iter = m_pn_set.begin(); iter != end; ++iter) {
        pn_list->push_back(*iter);
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::AddPN(const PN& pn) {
    MutexLockGuard lock_guard(lock_);
    PN_SET_ITER pn_find = m_pn_set.find(pn);
    if (pn_find != m_pn_set.end()) {
        DSTREAM_WARN("add pn error, pn exist");
        return error::PN_EXSIT;
    }
    PN_INSERT_RES pn_insert_res = m_pn_set.insert(pn);
    if (!pn_insert_res.second) {
        DSTREAM_WARN("insert pn to set fail");
        return error::PN_ADD_FAIL;
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::UpdatePN(const PN& pn) {
    MutexLockGuard lock_guard(lock_);
    PN_SET_ITER pn_find = m_pn_set.find(pn);
    if (pn_find == m_pn_set.end()) {
        DSTREAM_WARN("upate pn fail, pn not exist");
        return error::PN_NOT_EXIST;
    }
    const_cast<PN&>(*pn_find) = pn;
    return error::OK;
}

ERROR_CODE MemoryMetaManager::DeletePN(const PN& pn) {
    MutexLockGuard lock_guard(lock_);
    PN_SET_ITER pn_find = m_pn_set.find(pn);
    if (pn_find == m_pn_set.end()) {
        DSTREAM_WARN("delete pn fail, pn not exist");
        return error::PN_NOT_EXIST;
    }
    m_pn_set.erase(pn_find);
    return error::OK;
}

ERROR_CODE MemoryMetaManager::SaveApplication(const Application& app) {
    APP_SET_ITER find = m_app_set.find(app);
    if (find != m_app_set.end()) {
        DSTREAM_WARN("There is a application");
        return error::APP_EXIST;
    }
    APP_INSERT_RES add_res = m_app_set.insert(app);
    if (!add_res.second) {
        DSTREAM_WARN("add new App %s fail", app.name().c_str());
        return error::ADD_APP_FAIL;
    }
    return error::OK;
}

ERROR_CODE MemoryMetaManager::DeleteApplication(const AppID& app_id) {
    MutexLockGuard lock_guard(lock_);
    // error::Code rev_val = error::OK;
    APP_SET_ITER find;
    APP_PE_MAP_ITER app_pe_find;
    Application app;
    *(app.mutable_id()) = app_id;
    find = m_app_set.find(app);
    if (find == m_app_set.end()) {
        DSTREAM_WARN("delete app fail, app not exist");
        return error::APP_NOT_EXIST;
    }
    m_app_set.erase(find);
    app_pe_find = m_app_pe_map.find(app_id);
    if (app_pe_find != m_app_pe_map.end()) {
        m_app_pe_map.erase(app_pe_find);
    }
    return error::OK;
}

} // namespace memory_meta_manager
} // namespace dstream
