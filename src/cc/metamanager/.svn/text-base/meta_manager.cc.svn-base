/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief MetaManager for dstream
 */

#include "metamanager/meta_manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cstddef>

#include "common/application_tools.h"
#include "common/config_const.h"
#include "common/logger.h"
#include "metamanager/zk_meta_manager.h"

namespace dstream {
namespace meta_manager {
using namespace application_tools;

const uint64_t kMaxAppNum = (1 << 24);

MetaManager::MetaManager() {
    m_current_max_id.set_id(0);
    m_config = NULL;
}

ERROR_CODE MetaManager::AddApplication(Application* app) {
    MutexLockGuard lock_guard(m_lock);
    ClusterConfig cluster_config;
    ERROR_CODE res = error::OK;
    if ((res = GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("get cluster config fail");
        return res;
    }
    app->set_status(SUBMIT);
    if (!InitAppPath(cluster_config, app)) {
        DSTREAM_WARN("init app path fail");
        return error::ADD_APP_FAIL;
    }
    if ((res = SaveApplication(*app)) != error::OK) {
        DSTREAM_WARN("save application fail");
        return res;
    }
    list<ProcessorElement> pe_list;
    if (!CreateAppPEInstance(*app, cluster_config, m_config, &pe_list)) {
        DSTREAM_WARN("create pe instance fail");
        return error::ADD_PE_FAIL;
    }
    if ((res = AddAppProcessElements(app->id(), pe_list)) != error::OK) {
        DSTREAM_WARN("add app pes fail");
        return res;
    }
    return res;
}

ERROR_CODE MetaManager::InsertProcessElement(const Application& update_app) {
    MutexLockGuard lock_guard(m_lock);
    ERROR_CODE res = error::OK;
    ClusterConfig cluster_config;
    if ((res = GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("get cluster config fail");
        return res;
    }
    list<ProcessorElement> current_pe_list;
    if ((res = GetAppProcessElements(update_app.id(),
                                     &current_pe_list)) != error::OK ) {
        DSTREAM_WARN("get app process element fail");
        return res;
    }
    list<ProcessorElement> new_pe_list;
    if (!CreateAppPEInstance(update_app, cluster_config, m_config, &new_pe_list)) {
        DSTREAM_WARN("create new pe instance fail");
        return error::ADD_PE_FAIL;
    }
    list<ProcessorElement>::iterator current_pe_iter, new_pe_iter;

    for (current_pe_iter = current_pe_list.begin();
         current_pe_iter != current_pe_list.end(); current_pe_iter++) {
        for (new_pe_iter = new_pe_list.begin();
             new_pe_iter != new_pe_list.end();) {
            if ((*current_pe_iter).app().id() == (*new_pe_iter).app().id() &&
                (*current_pe_iter).processor().id() == (*new_pe_iter).processor().id()) {
                new_pe_iter = new_pe_list.erase(new_pe_iter);
            } else {
                new_pe_iter++;
            }
        }
    }
    for (new_pe_iter = new_pe_list.begin();
         new_pe_iter != new_pe_list.end(); new_pe_iter++) {
        DSTREAM_DEBUG("after need add pe id [%lu], process id[%lu]",
                      (*new_pe_iter).pe_id().id(),
                      (*new_pe_iter).processor().id());
    }
    if ( new_pe_list.size() == 0 ) {
        DSTREAM_DEBUG("no new pe to insert");
        return res;
    }
    if ((res = AddAppProcessElements(update_app.id(), new_pe_list)) != error::OK) {
        DSTREAM_WARN("add new pe fail");
        return res;
    }
    return res;
}

ERROR_CODE MetaManager::CheckUser(const User& user) {
    ClusterConfig cluster_config_;
    ERROR_CODE res = error::OK;
    if ((res = GetClusterConfig(&cluster_config_)) != error::OK) {
        return res;
    }
    int user_num = cluster_config_.users_size();
    for (int i = 0; i < user_num; ++i) {
        if (cluster_config_.users(i).username() == user.username()
            && cluster_config_.users(i).password() == user.password()) {
            return res;
        }
    }
    return error::INVALID_USER;
}

ERROR_CODE MetaManager::GetNewAppID(AppID* app_id) {
    MutexLockGuard lock_guard(m_lock);
    if (m_current_max_id.id() >= kMaxAppNum) {
        list<AppID> app_id_list;
        ERROR_CODE res;
        if ((res = GetAppIDList(&app_id_list)) != error::OK) {
            DSTREAM_WARN("Get AppID list fail [%d]", res);
            return res;
        }
        if (app_id_list.size() >= kMaxAppNum) {
            DSTREAM_WARN("there is too many application's in cluster");
            return error::ADD_APP_FAIL;
        }
        set<uint64> id_set;
        list<AppID>::iterator id_end = app_id_list.end();
        for (list<AppID>::iterator id_iter = app_id_list.begin();
             id_iter != id_end; ++id_iter) {
            id_set.insert(id_iter->id());
        }
        set<uint64>::iterator id_set_end = id_set.end();
        for (uint64_t i = 1; i < kMaxAppNum; ++i) {
            if (id_set.find(i) == id_set_end) {
                app_id->set_id(i);
                break;
            }
        }
    } else {
        m_current_max_id.set_id(m_current_max_id.id() + 1);
        app_id->set_id(m_current_max_id.id());
    }
    return error::OK;
}

ERROR_CODE MetaManager::UpdateUser(const User& add_user) {
    ClusterConfig cluster_config;
    ERROR_CODE add_res;
    if ((add_res = GetClusterConfig(&cluster_config)) == error::OK) {
        int user_num = cluster_config.users_size();
        for (int i = 0; i < user_num; ++i) {
            if (cluster_config.users(i).username() == add_user.username()) {
                if (cluster_config.users(i).password() == add_user.password()) {
                    return add_res;
                } else {
                    User* user = cluster_config.mutable_users(i);
                    user->set_password(add_user.password());
                    return UpdateClusterConfig(cluster_config);
                }
            }
        }
    }
    User* user = cluster_config.add_users();
    user->set_username(add_user.username());
    user->set_password(add_user.password());
    return UpdateClusterConfig(cluster_config);
}

bool MetaManager::CheckPM(const PM& pm) {
    bool ret = true;
    if (!pm.has_host()) {
        DSTREAM_WARN("pm does not set host name");
        ret = false;
    }
    if (!pm.has_report_port()) {
        DSTREAM_WARN("pm does not set report port");
        ret = false;
    }
    if (!pm.has_submit_port()) {
        DSTREAM_WARN("pm does not set submit port");
        ret = false;
    }
    return ret;
}

bool MetaManager::CheckPN(const PN& pn) {
    bool ret = true;
    if (!pn.has_host()) {
        DSTREAM_WARN("pn does not set host name");
        ret = false;
    }
    if (!pn.has_pn_id()) {
        DSTREAM_WARN("pn does not set id");
        ret = false;
    }
    if (!pn.has_rpc_server_port()) {
        DSTREAM_WARN("pn does not set rpc server port");
        ret = false;
    }
    if (!pn.has_pub_port()) {
        DSTREAM_WARN("pn does not set pub port");
        ret = false;
    }
    if (!pn.has_debug_port()) {
        DSTREAM_WARN("pn does not set debug port");
        ret = false;
    }
    return ret;
}

ERROR_CODE MetaManager::GetAllProcessElements(list<ProcessorElement>* all_pes) {
    list<Application> app_list;
    ERROR_CODE ret_val;
    if ((ret_val = GetApplicationList(&app_list)) != error::OK) {
        DSTREAM_WARN("Get Application list error");
        return ret_val;
    }
    list<Application>::iterator app_id_end = app_list.end();
    for (list<Application>::iterator app_iter = app_list.begin();
         app_iter != app_id_end; ++app_iter) {
        if (!AppCanScheduler(*app_iter)) {
            continue;
        }
        if ((ret_val = GetAppProcessElements(app_iter->id(), all_pes)) != error::OK) {
            DSTREAM_WARN("Get App %ld process elements fail", app_iter->id().id());
            return ret_val;
        }
    }
    return error::OK;
}

ERROR_CODE MetaManager::GetProcessorPEs(const AppID& app_id,
                                        const ProcessorID& processor_id,
                                        list<ProcessorElement>* pe_list) {
    ERROR_CODE res = error::OK;
    list<ProcessorElement> app_pe_list;
    if ((res = GetAppProcessElements(app_id, &app_pe_list)) != error::OK) {
        DSTREAM_WARN("get processorelements fail");
        return res;
    }
    list<ProcessorElement>::iterator pe_end = app_pe_list.end();
    for (list<ProcessorElement>::iterator pe_iter = app_pe_list.begin();
         pe_iter != pe_end; ++pe_iter) {
        if (pe_iter->processor().id() == processor_id.id()) {
            pe_list->push_back(*pe_iter);
        }
    }
    return res;
}

MetaManager* MetaManager::instance = NULL;

MetaManager* MetaManager::GetMetaManager() {
    return instance;
}

bool MetaManager::InitMetaManagerContext(const Config* config) {
    if (NULL != instance) {
        DSTREAM_INFO("meta manager inited");
        return true;
    }
    if (NULL == config) {
        DSTREAM_WARN("config error");
        return false;
    }
    std::string meta_manager_name;
    if (config->GetValue(config_const::kMetaManagerName, &meta_manager_name) == error::OK) {
        if (meta_manager_name == config_const::kZKMetaManagerName) {
            return (NULL != (instance = new zk_meta_manager::ZKMetaManager) &&
                    instance->InitMetaManager(config));
        }
        /*    if (meta_manager_name == config_const::kMemoryMetaManagerName) {
              return (NULL != (instance = new memory_meta_manager::MemoryMetaManager) &&
                      instance->InitMetaManager(config));
            }*/
    } else {
        return (NULL != (instance = new zk_meta_manager::ZKMetaManager) &&
                instance->InitMetaManager(config));
    }
    return false;
}

void MetaManager::DestoryMetaManager() {
    if (NULL != instance) {
        delete instance;
        instance = NULL;
    }
}

} // namespace meta_manager
} // namespace dstream
