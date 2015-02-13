/** -*- c++ -*-
 * Copyright (C) 2011 Realtime Team (Baidu, Inc)
 *
 * This file is part of DStream.
 *
 * DStream is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 */

#include "processnode/zk_operator.h"

namespace dstream {
namespace router {

// ----------------ZkOperator-------------
#define WATCH_NODE 1
#define CHECK_ZOOKEEPER_CONNECTION_VALID \
    do { \
        if (!m_connected) { \
            DSTREAM_WARN("not connect to zookeeper[%s] yet", \
                         m_zk_servers_list.c_str());\
            return error::ZK_CONNECTION_LOST; \
        } \
    } while (0)

ZkOperator::~ZkOperator() {
    Disconnect();
}

error::Code ZkOperator::Connect() {
    if (m_connected) {
        DSTREAM_DEBUG("connection to zookeeper[%s] already exists,"
                      "will not reconnect", m_zk_servers_list.c_str());
        return error::OK;
    }

    // set log level
    zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
    // set_level();

    // initialize zookeeper handler
    m_zk_handler = zookeeper_init(m_zk_servers_list.c_str(), WatcherFun,
                                  m_recv_timeout, NULL, this, 0);
    if (NULL == m_zk_handler) {
        DSTREAM_WARN("fail to init client to zookeeper[%s]",
                     m_zk_servers_list.c_str());
        return error::ZK_INIT_ERROR;
    }

    // wait a signal that connection is OK
    m_cond_lock.lock();
    while (!m_connected) {
        m_cond.wait();
    }
    m_cond_lock.unlock();

    DSTREAM_DEBUG("connect to zookeeper[%s] ...OK", m_zk_servers_list.c_str());
    return error::OK;
}

bool ZkOperator::IsConnected() {
    return ZOO_CONNECTED_STATE == zoo_state(m_zk_handler);
}

void ZkOperator::Disconnect() {
    m_connected = false;
    if (m_zk_handler) {
        zookeeper_close(m_zk_handler);
        m_zk_handler = NULL;
    }
}

error::Code ZkOperator::NodeExists(const char* path, int32_t* exist, int32_t* attr) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    if (NULL == path || NULL == exist) {
        DSTREAM_WARN("bad parameters path:[%s]", path);
        return error::BAD_ARGUMENT;
    }

    struct Stat stat;
    int32_t zrc = zoo_exists(m_zk_handler, path, WATCH_NODE, &stat);
    if (ZOK != zrc && ZNONODE != zrc) {
        DSTREAM_WARN("fail to check node existence@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_GET_NODE_FAIL;
    }

    *exist = (zrc == ZNONODE) ? 0 : 1;
    if (NULL != attr) {
        *attr = stat.ephemeralOwner ? zk_client::EPHEMERAL : zk_client::PERSISTENT;
    }
    return error::OK;
}

error::Code ZkOperator::GetNodeData(const char* path, zk_meta_manager::MetaZKData*& node_data) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    int32_t zrc = zoo_get(m_zk_handler, path, WATCH_NODE, node_data->m_buf,
                          &(node_data->m_len), NULL);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to get node data@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zookeeper no node %s", path);
            return error::ZK_NO_NODE;
        } else if (ZBADARGUMENTS == zrc) {
            DSTREAM_WARN("zookeeper bad argument %s", path);
            return error::BAD_ARGUMENT;
        }
        return error::ZK_GET_NODE_FAIL;
    }

    return error::OK;
}

error::Code ZkOperator::SetNodeData(const char* path, zk_meta_manager::MetaZKData* node_data) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    int32_t zrc = zoo_set(m_zk_handler, path, node_data->m_buf, node_data->m_len, -1);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to set node data@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        return error::ZK_SET_NODE_FAIL;
    }

    return error::OK;
}

error::Code ZkOperator::CreatePersistentNode(const char* path, zk_client::ZkNode* node) {
    return CreateNode(path, node, 0);
}

error::Code ZkOperator::DeletePersistentNode(const char* path) {
    return DeleteNode(path);
}

error::Code ZkOperator::CreateEphemeralNode(const char* path,
                                            zk_client::ZkNode* node) {
    DSTREAM_DEBUG("create ephemeraNode %s", path);
    return CreateNode(path, node, ZOO_EPHEMERAL | ZOO_SEQUENCE);
}

error::Code ZkOperator::CreateEphemeralNode(const char* path,
                                            zk_client::ZkNode* node,
                                            int32_t enable_sequence) {
    int32_t flag = (0 == enable_sequence) ?
                    ZOO_EPHEMERAL :
                    ZOO_EPHEMERAL | ZOO_SEQUENCE;
    return CreateNode(path, node, flag);
}

error::Code ZkOperator::DeleteEphemeralNode(const char* path) {
    return DeleteNode(path);
}



error::Code ZkOperator::CreateNode(const char* path, zk_client::ZkNode* node, int flag) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    int32_t zrc = ZOK;
    if (NULL == node) {
        zrc = zoo_create(m_zk_handler, path,
                         NULL, 0,
                         &ZOO_OPEN_ACL_UNSAFE,
                         flag,
                         NULL, 0);
    } else {
        zrc = zoo_create(m_zk_handler, path,
                         node->data.m_buf, node->data.m_len,
                         &ZOO_OPEN_ACL_UNSAFE,
                         flag,
                         node->path.m_buf, node->path.m_len);
    }
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to create node@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return zrc == ZNODEEXISTS ?
               error::ZK_NODE_EXISTS :
               error::ZK_CREATE_NODE_FAIL;
    }

    DSTREAM_DEBUG("create node@[%s] ...OK", (NULL == node) ?
                  path : node->path.m_buf);
    return error::OK;
}

error::Code ZkOperator::DeleteNode(const char* path) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    int32_t zrc = zoo_delete(m_zk_handler, path, -1);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to delete node@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_DELETE_NODE_FAIL;
    }

    DSTREAM_DEBUG("delete node@[%s] ...OK", path);
    return error::OK;
}

error::Code ZkOperator::WatchNode(const char* path) {
    // watch node & use global watch function
    int32_t zrc = zoo_exists(m_zk_handler, path, WATCH_NODE, NULL);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to set watch on node:[%s], ErrCode:[%d], Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_SET_WATCHER_FAIL;
    }
    return error::OK;
}

error::Code ZkOperator::GetChildList(const char* path,
                                     std::vector<std::string>* child_list) {
    CHECK_ZOOKEEPER_CONNECTION_VALID;

    if (NULL == path || NULL == child_list) {
        return error::BAD_ARGUMENT;
    }

    int32_t zrc = 0;
    /* get the children list, do not watch the node */
    String_vector children_strings = {0, NULL};
    zrc = zoo_get_children(m_zk_handler, path, WATCH_NODE, &children_strings);
    if (ZOK != zrc) {
        if (ZNONODE == zrc) {
            // DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        // DSTREAM_WARN("fail to get node children@[%s], ErrCode:[%d] Err:[%s]",
        //   path, zrc, zerror(zrc));
        return error::ZK_GET_CHILDREN_FAIL;
    }
    for (int i = 0; i < children_strings.count; ++i) {
        child_list->push_back(children_strings.data[i]);
    }
    deallocate_String_vector(&children_strings);
    return error::OK;
}

void ZkOperator::OnConnect() {
    // notify Connect function
    // DSTREAM_DEBUG("Connect to zookeeper[%s] ...OK", m_zk_servers_list.c_str());
    m_cond_lock.lock();
    m_connected = true;
    m_cond.notify();
    m_cond_lock.unlock();
}

void ZkOperator::WatcherFun(zhandle_t* /*zk_handler*/, int type, int state,
                            const char* path, void* context) {
    ZkOperator* zk_client = reinterpret_cast<ZkOperator*>(context);
    // DSTREAM_DEBUG("get event:[%d], state:[%d] at path:[%s]",
    //                type, state, path);
    if (NULL == zk_client) {
        DSTREAM_DEBUG("no ZkClient object to receive zookeeper event");
        return;
    }

    // handle session event
    if (ZOO_SESSION_EVENT == type) { /* session event */
        if (ZOO_CONNECTED_STATE == state) {
            /* connection is ok */
            zk_client->OnConnect();
            return;
        }
        if (ZOO_EXPIRED_SESSION_STATE == state) {
            /* session expires, trigger callback */
            DSTREAM_WARN("the session with zookeeper expires");
            // TODO(fangjun): need some strategy
        }
        if (ZOO_CONNECTING_STATE == state) {
            DSTREAM_INFO("connecting to zookeeper");
        }
        if (0 == state) { /* TODO: check this */
            /* the connection with zookeeper servers is temporarily unavailable */
            DSTREAM_WARN("the conn to zk is temporarily unavailable");
        }
        return;
    }

    // handle user callback
    if (zk_client->m_cb_handle) {
        zk_client->m_cb_handle(type, state, path, zk_client->m_cb_context);
    }
    return;
}



// ----------------ZkWrapper-------------
#define CHECK_ZK_CLIENT(zk_operator)            \
    if (!zk_operator->IsConnected()) {             \
        DSTREAM_WARN("zk client is not init");       \
        return error::CONNECT_ZK_FAIL;               \
    }                                              \

ZkWrapper::ZkWrapper() {
}

ZkWrapper::~ZkWrapper() {
}

error::Code ZkWrapper::Init(const config::Config& config, ZkOperator::WatchEventCallback cb_func,
                            void* ctx) {
    if (m_zk_operator && m_zk_operator->IsConnected()) {
        return error::OK;
    }
    // read config
    zk_config::ZKConfig zk_config(&config);
    m_root_path = zk_config.GetSysPath();
    m_app_root_path = zk_config.AppRootPath();
    m_pm_path = zk_config.PMPath();
    m_pn_root_path = zk_config.PNRootPath();
    m_zk_addr = zk_config.GetZKPath();
    m_zk_timeout = zk_config.Timeout();
    if ("" == m_zk_addr) {
        return error::ZK_BAD_ADDRESS;
    }

    m_zk_operator = ZkOperator::ZkOperatorPtr(new (std::nothrow) ZkOperator(m_zk_addr.c_str(),
                                              m_zk_timeout));
    if (m_zk_operator == NULL) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    // set callback function
    m_zk_operator->SetWatchEventCallBack(cb_func, ctx);
    return m_zk_operator->Connect();
}

void ZkWrapper::Destroy() {
    if (m_zk_operator) {
        m_zk_operator->Disconnect();
    }
}

error::Code ZkWrapper::AddZKNode(const std::string& path,
                                 const ::google::protobuf::Message* msg,
                                 bool persist_node,
                                 int enable_sequence /* = 0*/,
                                 std::string* node_path /*= 0*/) {
    zk_meta_manager::MetaZKNode zk_node;
    if (!zk_node.AllocateBuf(zk_client::ZkClient::kZkMaxSingleNodePathLen,
                             msg->ByteSize())) {
        DSTREAM_WARN("allocate buffer fail");
        return error::ALLOC_BUFFER_FAIL;
    }
    if (static_cast<int>(path.length()) > zk_node.path.m_len - 1) {
        DSTREAM_WARN("[%s] Add ZKNode Fail path [%s] length [%zd] is large than [%d]",
                     __FUNCTION__, path.c_str(),
                     path.length(), zk_node.path.m_len);
        return error::SERIAIL_DATA_FAIL;
    }
    if (!strcpy(zk_node.path.m_buf, path.c_str())) {
        DSTREAM_WARN("serialize message fail");
        return error::SERIAIL_DATA_FAIL;
    }
    if (NULL != msg) {
        if (!msg->SerializeToArray(zk_node.data.m_buf, zk_node.data.m_len)) {
            DSTREAM_WARN("serialize message fail");
            return error::SERIAIL_DATA_FAIL;
        }
    } else {
        zk_node.data.m_len = 0;
    }
    if (persist_node) {
        return m_zk_operator->CreatePersistentNode(path.c_str(), &zk_node);
    }
    error::Code res = m_zk_operator->CreateEphemeralNode(path.c_str(), &zk_node,
                      enable_sequence);
    if (res == error::OK && NULL != node_path) {
        *node_path = zk_node.path.m_buf;
    }
    DSTREAM_INFO("[%s] AddZKNode [%s] success", __FUNCTION__, zk_node.path.m_buf);
    return res;
}

std::string ZkWrapper::GetAppPath(const AppID& app_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, sizeof(path), "%s/%ld", m_app_root_path.c_str(), app_id.id());
    return path;
}

std::string ZkWrapper::GetPNPath(const PNID& pn_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, sizeof(path), "%s/%s", m_pn_root_path.c_str(), pn_id.id().c_str());
    return path;
}

std::string ZkWrapper::GetPEPath(const AppID& app_id, const PEID& pe_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, sizeof(path), "%s/%ld/%ld", m_app_root_path.c_str(), app_id.id(), pe_id.id());
    return path;
}

error::Code ZkWrapper::GetPM(PM* pm) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return router::GetZKData(m_zk_operator, m_pm_path, pm);
}

error::Code ZkWrapper::GetPN(const PNID& pn_id, PN* pn) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return router::GetZKData(m_zk_operator, GetPNPath(pn_id), pn);
}

error::Code ZkWrapper::AddPN(const PN& pn) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return AddZKNode(GetPNPath(pn.pn_id()), &pn, false);
}

error::Code ZkWrapper::DeletePN(const PN& pn) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return m_zk_operator->DeleteEphemeralNode(GetPNPath(pn.pn_id()).c_str());
}

error::Code ZkWrapper::GetApplication(const AppID& app_id, Application* app) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return router::GetZKData(m_zk_operator, GetAppPath(app_id).c_str(), app);
}

error::Code ZkWrapper::GetPEList(const AppID& app_id, std::vector<std::string>* peid_list) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return m_zk_operator->GetChildList(GetAppPath(app_id).c_str(), peid_list);
}

error::Code ZkWrapper::GetProcessorElement(const AppID& app_id,
                                           const PEID& pe_id,
                                           ProcessorElement* pe) {
    CHECK_ZK_CLIENT(m_zk_operator);
    return router::GetZKData(m_zk_operator, GetPEPath(app_id, pe_id), pe);
}


} // namespace router
} // namespace dstream

