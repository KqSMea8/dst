/**
* @file     zk.cc
* @brief    zookeeper client for dstream client.
* @author   konghui, konghui@baidu.com
* @date     2011-12-23
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <list>
#include "client/client_config.h"
#include "client/dstream_client.h"
#include "client/zk.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/utils.h"
#include "common/zk_client.h"

namespace dstream {
namespace client {

ZK* g_zk = NULL;

void ZKWatcher::OnEvent(int type, int /*state*/, const char* /*path*/) {
    // event deal;
    /*if (ZOO_SESSION_EVENT == type && ZOO_SESSION_EVENT == type) {
      //m_connected = false;
    }*/
}

ZK::~ZK() {
    if (m_connected) {
        UnRegister();
    }
    if (NULL != m_client) {
        delete m_client;
        m_client = NULL;
    }
}
int32_t ZK::Register(User* user) {
    if (user == NULL) {
        DSTREAM_WARN("Bad argument,ZK::Register does not take null pointer value");
        return error::BAD_ARGUMENT;
    }

    int32_t ret = error::OK;
    if (!m_connected) {
        ret = m_client->Connect();
        if (ret < error::OK) {
            DSTREAM_WARN("fail to connect zookeeper");
            return ret;
        }
        m_connected = true;
    }

    /* check and build system env if the system env is not ok */
    int32_t exists = 0;
    ret = m_client->SystemRootExists(g_cfg_map.GetValue(config_const::kClientRootName).c_str(),
                                    &exists);
    if (ret < error::OK) {
        return ret;
    }
    if (!exists) {
        /* create system env */
        ret = m_client->InitSystemRoot(g_cfg_map.GetValue(config_const::kClientRootName).c_str());
        if (ret < error::OK) {
            DSTREAM_WARN("fail to create system env");
            return ret;
        }
        DSTREAM_DEBUG("create system env OK");
    }

    zk_client::ZkNode node;
    node.data.m_buf = reinterpret_cast<char*>(user);
    node.data.m_len = sizeof(*user);
    node.path.m_len = zk_client::ZkClient::kZkMaxSingleNodePathLen;
    m_register_path.resize(node.path.m_len);
    node.path.m_buf = StringAsArray(&m_register_path);
    if (NULL == node.path.m_buf) {
        DSTREAM_WARN("fail to get string space");
        return error::BAD_MEMORY_ALLOCATION;
    }
    std::string client_path = g_cfg_map.GetValue(config_const::kClientRootName);
    client_path.append("/").append(user->hostname());
    user->set_hostname(client_path);
    ret = m_client->CreateEphemeralNode(client_path.c_str(), &node, 0);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to create register node:[%s]", client_path.c_str());
    }
    return error::OK;
}
int32_t ZK::UnRegister() {
    int32_t ret = error::OK;
    if (!m_connected) {
        DSTREAM_WARN("no connection on zookeeper");
        return error::OK;
    }

    ret = m_client->DeleteEphemeralNode(m_register_path.c_str());
    if (ret < error::OK) {
        DSTREAM_WARN("fail to remove register node:[%s]", m_register_path.c_str());
        return ret;
    }
    return error::OK;
}
int32_t ZK::GetPM(uint32_t retry, uint32_t interval_secs, PM* pm) {
    if (pm == NULL) {
        DSTREAM_WARN("Bad argument,ZK::GetPM does not take null value pointer");
        return error::BAD_ARGUMENT;
    }

    int32_t ret = error::OK;
    if (!m_connected) {
        for (uint32_t i = 0; i < retry; i++) {
            ret = m_client->Connect();
            if (ret < error::OK) {
                DSTREAM_WARN("[%d]st fail to connect zookeeper", i);
                sleep(interval_secs);
                continue;
            }
            m_connected = true;
            break;
        }
        if (ret < error::OK) {
            return ret;
        }
    }

    std::string pm_path = g_cfg_map.GetValue(config_const::kPMRootPathName);

    zk_client::ZkNodeData node_data;
    node_data.m_len = zk_client::ZkClient::kZkMaxSingleNodeDataLen;
    node_data.m_buf = static_cast<char*>(malloc(node_data.m_len));
    if (NULL == node_data.m_buf) {
        DSTREAM_WARN("fail to alloc memory");
        return error::BAD_MEMORY_ALLOCATION;
    }
    for (uint32_t i = 0; i < retry; i++) {
        node_data.m_len = zk_client::ZkClient::kZkMaxSingleNodeDataLen;
        ret = m_client->GetNodeData(pm_path.c_str(), &node_data);
        if (ret < error::OK) {
            DSTREAM_WARN("[%d]st fail to get data at node:[%s]@[%s]",
                         i, pm_path.c_str(), m_server_list.c_str());
            sleep(interval_secs);
            continue;
        }
        if (!pm->ParseFromArray(node_data.m_buf, node_data.m_len)) {
            DSTREAM_WARN("fail to deserialize pm obj from [%p]", node_data.m_buf);
            ret = error::PARSE_DATA_FAIL;
            break;
        }
        ret = error::OK;
        break;
    }

    free(node_data.m_buf);
    return ret;
}

int32_t ZK::Connection() {
    int32_t ret = error::OK;
    if (m_connected) {
        return error::OK;
    }
    ret = m_client->Connect();
    if (ret < error::OK) {
        DSTREAM_WARN("fail to connect zookeeper");
        return ret;
    }
    m_connected = true;
    /* set watcher */
    ret = m_client->SetConnBrokenWatcher(&m_watcher);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to set watcher on connect broken");
        return ret;
    }
    return error::OK;
}

int32_t ZK::GetApp(const std::string& app_name, uint32_t retry,
                   uint32_t interval_secs, Application *app) {
    if (app == NULL) {
        DSTREAM_WARN("Bad argument, ZK::GetApp does not take null value pointer");
    }
    int32_t ret = error::OK;

    if ( !m_connected ) {
        for ( uint32_t i = 0; i < retry; i++ ) {
            ret = m_client->Connect();
            if (ret < error::OK) {
                DSTREAM_WARN("[%d]st fail to connect zookeeper", i);
                sleep(interval_secs);
                continue;
            }
            m_connected = true;
            break;
        }
        if (ret < error::OK) {
            return ret;
        }
    }
    std::string app_path = g_cfg_map.GetValue(config_const::kAppRootPathName);

    std::list<zk_client::ZkNode*> app_list;
    std::list<zk_client::ZkNode*>::iterator app_list_iter;

    for (uint32_t i = 0; i < retry; i++) {
        ret = m_client->GetNodeChildren(app_path.c_str(), &app_list);
        if (ret < error::OK) {
            DSTREAM_WARN("[%d]st fail to get data at node:[%s]@[%s]",
                         i, app_path.c_str(), m_server_list.c_str());
            sleep(interval_secs);
            continue;
        }
        for ( app_list_iter = app_list.begin(); app_list_iter != app_list.end(); app_list_iter++ ) {
            if ( !app->ParseFromArray((*app_list_iter)->data.m_buf,
                                      (*app_list_iter)->data.m_len)) {
                DSTREAM_DEBUG("fail to parse app");
                return error::ZK_GET_NODE_FAIL;
            } else {
                DSTREAM_DEBUG("get app name %s", app->name().c_str());
                if ( strcmp(app_name.c_str(), app->name().c_str()) == 0 ) {
                    return error::OK;
                }
            }
        }
        app->mutable_id()->set_id(static_cast<uint64_t>(-1));
        return error::OK;
    }
    return ret;
}

int32_t ZK::GetAppID(const std::string& /*app_name*/, int64_t* /*app_id*/) {
    return error::OK;
}

} // namespace client
} // namespace dstream
