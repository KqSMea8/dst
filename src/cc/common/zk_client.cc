/**
* @file   zk_client.cc
* @brief    zookeeper client wrapper
* @author   konghui, konghui@baidu.com
* @version   1.0.1.0
* @date   2011-11-30
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <utility>
#include "common/application_tools.h"
#include "common/logger.h"
#include "common/zk_client.h"

namespace dstream {
namespace zk_client {

ZkNode* NewZkNode() {
    ZkNode* node = new ZkNode();
    node->data.m_len = ZkClient::kZkMaxSingleNodeDataLen;
    node->data.m_buf = static_cast<char*>(malloc(node->data.m_len));
    if (NULL == node->data.m_buf) {
        delete node;
        return NULL;
    }
    node->path.m_len = ZkClient::kZkMaxSingleNodePathLen;
    node->path.m_buf = static_cast<char*>(malloc(node->path.m_len));
    if (NULL == node->path.m_buf) {
        free(node->data.m_buf);
        delete node;
        return NULL;
    }
    return node;
}

void DeleteZkNode(ZkNode* node) {
    if (NULL != node) {
        free(node->data.m_buf);
        free(node->path.m_buf);
        delete node;
    }
}

ZkClient::~ZkClient() {
    Disconnect();
}

error::Code ZkClient::Connect() {
    if (m_connected) {
        DSTREAM_DEBUG("connection to zookeeper[%s] already exists,"
                      "will not reconnect", m_servers_list.c_str());
        return error::OK;
    }

    /* set log level */
    zoo_set_debug_level(m_zk_log_level);

    /* initialize zookeeper handler */
    m_zk_handler = zookeeper_init(m_servers_list.c_str(), WatcherFun,
                                  m_recv_timeout_ms, NULL, this, 0);
    if (NULL == m_zk_handler) {
        DSTREAM_WARN("fail to init client to zookeeper[%s]",
                     m_servers_list.c_str());
        return error::ZK_INIT_ERROR;
    }

    /* wait a signal that connection is OK */
    m_condition_lock.lock();
    while (ZOO_CONNECTED_STATE != zoo_state(m_zk_handler)) {
        m_condition.wait();
    }
    m_condition_lock.unlock();
    m_connected = true;

    DSTREAM_DEBUG("connect to zookeeper[%s] ...OK", m_servers_list.c_str());
    return error::OK;
}

void ZkClient::Disconnect() {
    m_connected = false;
    if (m_zk_handler) {
        zookeeper_close(m_zk_handler);
        m_zk_handler = NULL;
    }
}

error::Code ZkClient::SetConnBrokenWatcher(Watcher* watcher) {
    m_conn_broken_watcher = watcher;
    return error::OK;
}

error::Code ZkClient::NodeExists(const char* path, int32_t* exist, int32_t* attr) {
    if (NULL == path || NULL == exist) {
        DSTREAM_WARN("bad parameters path:[%s]", path);
        return error::BAD_ARGUMENT;
    }

    struct Stat stat;
    int32_t zrc = zoo_exists(m_zk_handler, path, 0, &stat);
    if (ZOK != zrc && ZNONODE != zrc) {
        DSTREAM_WARN("fail to check node existence@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_GET_NODE_FAIL;
    }

    *exist = (zrc == ZNONODE) ? 0 : 1;
    if (NULL != attr) {
        *attr = stat.ephemeralOwner ? EPHEMERAL : PERSISTENT;
    }
    return error::OK;
}

error::Code ZkClient::GetNodeData(const char* path, ZkNodeData* node_data) {
    int32_t zrc = zoo_get(m_zk_handler, path, 0, node_data->m_buf,
                          &(node_data->m_len), NULL);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to get node data@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zookeeper no node %s", path);
            return error::ZK_NO_NODE;
        }
        return error::ZK_GET_NODE_FAIL;
    }

    return error::OK;
}

error::Code ZkClient::GetChildList(const char* path,
                                   std::list<std::string>* child_list) {
    int32_t zrc = 0;
    /* get the children list, do not watch the node */
    String_vector children_strings = {0, NULL};
    zrc = zoo_get_children(m_zk_handler, path, 0, &children_strings);
    if (ZOK != zrc) {
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        DSTREAM_WARN("fail to get node children@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_GET_CHILDREN_FAIL;
    }
    for (int i = 0; i < children_strings.count; ++i) {
        child_list->push_back(children_strings.data[i]);
    }
    deallocate_String_vector(&children_strings);
    return error::OK;
}

error::Code ZkClient::GetNodeChildren(const char* path,
                                      std::list<ZkNode*>* children) {
    int32_t zrc = 0;
    error::Code ret = error::OK;

    /* get the children list, do not watch the node */
    String_vector children_strings = {0, NULL};
    zrc = zoo_get_children(m_zk_handler, path, 0, &children_strings);
    if (ZOK != zrc) {
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        DSTREAM_WARN("fail to get node children@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_GET_CHILDREN_FAIL;
    }

    // delete old data first
    for (std::list<ZkNode*>::iterator it = children->begin(); it != children->end(); it++) {
        DeleteZkNode(*it);
    }
    children->clear();
    for (int32_t i = 0; i < children_strings.count; i++) {
        ZkNode* node = NewZkNode();
        if (NULL == node) {
            DSTREAM_WARN("fail to alloc memory");
            ret = error::BAD_MEMORY_ALLOCATION;
            goto ERR_RET;
        }
        /* put into list */
        children->push_back(node);

        /* copy paths */
        strncpy(node->path.m_buf, path, kZkMaxSingleNodePathLen);
        strncat(node->path.m_buf, "/", kZkMaxSingleNodePathLen);
        strncat(node->path.m_buf, children_strings.data[i], kZkMaxSingleNodePathLen);
        node->path.m_len = strlen(node->path.m_buf);
        if (node->path.m_len > kZkMaxSingleNodePathLen) {
            DSTREAM_WARN("bad path argument");
            ret = error::BAD_ARGUMENT;
            goto ERR_RET;
        }
        /* get node data */
        zrc = zoo_get(m_zk_handler, node->path.m_buf, 0, node->data.m_buf,
                      &(node->data.m_len), NULL);
        if (ZOK != zrc) {
            DSTREAM_WARN("fail to get child node data@[%s], ErrCode:[%d] Err:[%s]",
                         node->path.m_buf, zrc, zerror(zrc));
            ret = error::ZK_GET_NODE_FAIL;
            goto ERR_RET;
        }
    }

    deallocate_String_vector(&children_strings);
    return error::OK;

ERR_RET:
    /* free node that already alloc */
    deallocate_String_vector(&children_strings);
    for (std::list<ZkNode*>::iterator it = children->begin(); it != children->end(); it++) {
        DeleteZkNode(*it);
    }
    children->clear();
    return ret;
}

error::Code ZkClient::SetNodeData(const char* path, const ZkNodeData& node_data) {
    int32_t zrc = zoo_set(m_zk_handler, path, node_data.m_buf, node_data.m_len, -1);
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

error::Code ZkClient::InitSystemRoot(const char* root_path) {
    if (NULL == root_path) {
        DSTREAM_WARN("bad parameters path:[%s]", root_path);
        return error::BAD_ARGUMENT;
    }

    /* split path */
    std::string path = root_path;
    std::vector<std::string> node_names;
    application_tools::SplitString(path, &node_names);
    if (node_names.empty()) {
        DSTREAM_WARN("bad parameters path:[%s]", root_path);
        return error::BAD_ARGUMENT;
    }

    /* loop create */
    std::string node_path = "";
    for (size_t i = 0; i < node_names.size(); i++) {
        node_path += "/";
        node_path += node_names[i];
        error::Code ret = CreatePersistentNode(node_path.c_str(), NULL);
        if (ret < 0 && ret != error::ZK_NODE_EXISTS) {
            DSTREAM_WARN("fail to create node:[%s]", node_names[i].c_str());
            return ret;
        }
    }

    return error::OK;
}

error::Code ZkClient::InitSystemRoot(const char* root_path,
                                     std::vector<ZkNode*> node_data_list) {
    if (NULL == root_path) {
        DSTREAM_WARN("bad parameters path:[%s]", root_path);
        return error::BAD_ARGUMENT;
    }

    /* split path and check node data */
    std::string path = root_path;
    std::vector<std::string> node_names;
    application_tools::SplitString(path, &node_names);
    if (node_names.empty() || node_names.size() != node_data_list.size()) {
        DSTREAM_WARN("bad parameters path:[%s], node_data_list.size:[%lu]",
                     root_path, node_data_list.size());
        return error::BAD_ARGUMENT;
    }

    /* loop create */
    std::string node_path = "";
    for (size_t i = 0; i < node_names.size(); i++) {
        node_path += "/";
        node_path += node_names[i];
        int32_t ret = CreatePersistentNode(node_path.c_str(), node_data_list[i]);
        if (ret < 0 && ret != error::ZK_NODE_EXISTS) {
            DSTREAM_WARN("fail to create node:[%s]", node_names[i].c_str());
            return error::ZK_CREATE_NODE_FAIL;
        }
    }

    return error::OK;
}

error::Code ZkClient::SystemRootExists(const char* root_path, int32_t* exists) {
    return NodeExists(root_path, exists, NULL);
}

error::Code ZkClient::CreatePersistentNode(const char* path, ZkNode* node) {
    return CreateNode(path, node, 0);
}

error::Code ZkClient::DeletePersistentNode(const char* path) {
    return DeleteNode(path);
}

error::Code ZkClient::DeleteNodeTree(const char* path, bool root_delete) {
    if (NULL == path) {
        DSTREAM_WARN("bad parameters path:[%s]", path);
        return error::BAD_ARGUMENT;
    }

    int32_t zrc;
    error::Code ret = error::OK;
    /* check whether this node attribution: persistent or ephemeral */
    Stat stat;
    ZkNode* node = NewZkNode();
    if (NULL == node) {
        DSTREAM_WARN("fail to alloc memory");
        return error::BAD_MEMORY_ALLOCATION;
    }
    zrc = zoo_get(m_zk_handler, path, 0, node->data.m_buf, &(node->data.m_len), &stat);
    DeleteZkNode(node);
    node = NULL;
    if (ZOK != zrc) {
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        DSTREAM_WARN("fail to get node data@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_GET_NODE_FAIL;
    }
    if (0 != stat.ephemeralOwner) { /* ephemeral node, delete and return */
        return DeleteNode(path);
    }

    /* get children list and delete */
    std::string abs_path = path;
    abs_path += "/";
    String_vector children_strings = {0, NULL};
    zrc = zoo_get_children(m_zk_handler, path, 0, &children_strings);
    if (ZOK != zrc) {
        if (ZNONODE == zrc) {
            DSTREAM_WARN("zk no node %s", path);
            return error::ZK_NO_NODE;
        }
        DSTREAM_WARN("fail to get node children@[%s], ErrCode:[%d], Err:[%s]",
                     path, zrc, zerror(zrc));
        return  error::ZK_GET_CHILDREN_FAIL;
    }
    for (int32_t i = 0; i < children_strings.count; i++) {
        /* recursively delete node */
        ret = DeleteNodeTree((abs_path + children_strings.data[i]).c_str());
        if (ret != error::OK) {
            goto ERR_RET;
        }
    }

    /* delete node */
    if (root_delete) {
        ret = DeleteNode(path);
        if (ret < error::OK) {
            goto ERR_RET;
        }
    }

    deallocate_String_vector(&children_strings);
    return error::OK;

ERR_RET:
    deallocate_String_vector(&children_strings);
    return ret;
}

error::Code ZkClient::CreateEphemeralNode(const char* path,
                                          ZkNode* node) {
    DSTREAM_DEBUG("create ephemeraNode %s", path);
    return CreateNode(path, node, ZOO_EPHEMERAL | ZOO_SEQUENCE);
}

error::Code ZkClient::CreateEphemeralNode(const char* path,
                                          ZkNode* node,
                                          int32_t enable_sequence) {
    int32_t flag = (0 == enable_sequence) ?ZOO_EPHEMERAL : ZOO_EPHEMERAL | ZOO_SEQUENCE;
    return CreateNode(path, node, flag);
}

error::Code ZkClient::DeleteEphemeralNode(const char* path) {
    return DeleteNode(path);
}

error::Code ZkClient::WatchNode(const char* path, Watcher* watcher) {
    if (NULL == path || NULL == watcher) {
        DSTREAM_WARN("bad parameters path:[%s], Watcher:[%s]",
                     path, watcher ? "Not NULL" : "NULL");
        return error::BAD_ARGUMENT;
    }
    error::Code res = error::OK;
    WatchMap::const_iterator it;
    m_map_lock.lock();
    it = m_node_watcher_map.find(path);
    if (it != m_node_watcher_map.end()) {
        DSTREAM_WARN("node[%s] already watched by[%p], will not set watcher[%p] "
                     "on it", path, it->second, watcher);
        m_map_lock.unlock();
        return error::ZK_SET_WATCHER_FAIL;
    }
    m_map_lock.unlock();
    /* watch node and add to watch map */
    if ((res = WatchNode(path)) == error::OK) {
        DSTREAM_DEBUG("set watch on node:[%s] ...OK", path);
        /* add to watch map */
        m_map_lock.lock();
        m_node_watcher_map.insert(std::make_pair(path, watcher));
        m_map_lock.unlock();
    }
    return res;
}

error::Code ZkClient::WatchChildrenDelete(const char* parent_path,
                                          Watcher* delete_watcher) {
    if (NULL == parent_path || NULL == delete_watcher) {
        DSTREAM_WARN("bad parameters parent_path:[%s], Watcher:[%s]",
                     parent_path, delete_watcher ? "Not NULL" : "NULL");
        return error::BAD_ARGUMENT;
    }

    error::Code ret = error::OK;

    WatchMap::const_iterator it;
    m_map_lock.lock();
    it = m_child_delete_watcher_map.find(parent_path);
    if (it != m_child_delete_watcher_map.end()) {
        DSTREAM_WARN("node[%s] already watched by[%p], will not set watcher[%p] "
                     "on it", parent_path, it->second, delete_watcher);
        m_map_lock.unlock();
        return error::ZK_SET_WATCHER_FAIL;
    }
    m_map_lock.unlock();


    /* watch node and add to child_delete watch map */
    if ((ret = WatchNode(parent_path)) != error::OK) {
        return ret;
    }
    DSTREAM_DEBUG("set watcher on node:[%s] ...OK", parent_path);
    /* add to child_delete watch map */
    m_map_lock.lock();
    m_child_delete_watcher_map.insert(std::make_pair(parent_path, delete_watcher));
    m_map_lock.unlock();

    /* watch children */
    ret = WatchChildren(parent_path, &m_self_delete_watcher_map,
                        delete_watcher);
    if (ret != error::OK) {
        m_map_lock.lock();
        WatchMap::iterator iter = m_child_delete_watcher_map.find(parent_path);
        if (iter != m_child_delete_watcher_map.end()) {
            DSTREAM_DEBUG("remove watcher[%p]@[%s] from due to watch children fail",
                          iter->second, iter->first.c_str());
            m_child_delete_watcher_map.erase(iter);
        }
        m_map_lock.unlock();
        return ret;
    }
    DSTREAM_DEBUG("set CHILDREN_DELETE watcher on node:[%s] ...OK", parent_path);
    return ret;
}

error::Code ZkClient::WatchChildrenAll(const char* parent_path,
                                       Watcher* all_watcher) {
    if (NULL == parent_path || NULL == all_watcher) {
        DSTREAM_WARN("bad parameters parent_path:[%s], Watcher:[%s]",
                     parent_path, all_watcher ? "Not NULL" : "NULL");
        return error::BAD_ARGUMENT;
    }

    error::Code ret = error::OK;

    WatchMap::const_iterator it;
    m_map_lock.lock();
    it = m_child_all_watcher_map.find(parent_path);
    if (it != m_child_all_watcher_map.end()) {
        DSTREAM_WARN("node[%s] already watched by[%p], will not set watcher[%p] "
                     "on it", parent_path, it->second, all_watcher);
        m_map_lock.unlock();
        return error::ZK_SET_WATCHER_FAIL;
    }
    m_map_lock.unlock();

    if ((ret = WatchNode(parent_path)) != error::OK) {
        DSTREAM_WARN("fail to watch on node %s", parent_path);
        return ret;
    }
    DSTREAM_DEBUG("set watcher on node:[%s] ...OK", parent_path);
    m_map_lock.lock();
    m_child_all_watcher_map.insert(std::make_pair(parent_path, all_watcher));
    m_map_lock.unlock();

    ret = WatchChildren(parent_path, &m_self_delete_watcher_map,
                        all_watcher);
    if (ret != error::OK) {
        m_map_lock.lock();
        WatchMap::iterator iter = m_child_all_watcher_map.find(parent_path);
        if (iter != m_child_all_watcher_map.end()) {
            DSTREAM_DEBUG("remove watcher[%p]@[%s] from due to watch children fail",
                          iter->second, iter->first.c_str());
            m_child_all_watcher_map.erase(iter);
        }
        m_map_lock.unlock();
        return ret;
    }
    DSTREAM_DEBUG("set CHILDREN_ALL watcher on node:[%s] ...OK", parent_path);
    return error::OK;
}
error::Code ZkClient::UnWatchNode(const char* path) {
    if (NULL == path) {
        DSTREAM_WARN("bad parameters path:[%s]", path);
        return error::BAD_ARGUMENT;
    }

    m_map_lock.lock();
    WatchMap::iterator it = m_node_watcher_map.find(path);
    if (it == m_node_watcher_map.end()) {
        m_map_lock.unlock();
        DSTREAM_DEBUG("fail to unwatch node:[%s], watcher does not exists", path);
        return error::OK;
    }
    m_node_watcher_map.erase(it);
    m_map_lock.unlock();
    DSTREAM_DEBUG("Unwatch node:[%s] ...OK", path);
    return error::OK;
}

error::Code ZkClient::UnWatchChildrenDelete(const char* parent_path) {
    if (NULL == parent_path) {
        DSTREAM_WARN("bad parameters path:[%s]", parent_path);
        return error::BAD_ARGUMENT;
    }

    WatchMap::iterator it;
    /* delete from the child_delete_watcher_map */
    m_map_lock.lock();
    it = m_child_delete_watcher_map.find(parent_path);
    if (it != m_child_delete_watcher_map.end()) {
        DSTREAM_DEBUG("remove watcher[%p]@[%s] from child_delete_watcher_map",
                      it->second, it->first.c_str());
        m_child_delete_watcher_map.erase(it);
    }
    m_map_lock.unlock();

    /* delete from the self_delete_watcher_map */
    size_t path_len = strlen(parent_path);
    m_map_lock.lock();
    for (it  = m_self_delete_watcher_map.begin();
         it != m_self_delete_watcher_map.end(); ) {
        /* find all children */
        if (0 == strncmp(it->first.c_str(), parent_path, path_len) &&
            ('\0' == (it->first)[path_len] || '/' == (it->first)[path_len])) {
            DSTREAM_DEBUG("remove watcher[%p]@node:[%s]", it->second, it->first.c_str());
            m_self_delete_watcher_map.erase(it++);
        } else {
            it++;
        }
    }
    m_map_lock.unlock();

    return error::OK;
}

error::Code ZkClient::UnWatchChildrenAll(const char* parent_path) {
    if (NULL == parent_path) {
        DSTREAM_WARN("bad parameters path:[%s]", parent_path);
        return error::BAD_ARGUMENT;
    }

    WatchMap::iterator it;
    m_map_lock.lock();
    it = m_child_all_watcher_map.find(parent_path);
    if (it != m_child_all_watcher_map.end()) {
        DSTREAM_DEBUG("remove watcher[%p]@[%s] from child_all_watcher_map",
                      it->second, it->first.c_str());
        m_child_all_watcher_map.erase(it);
    }
    m_map_lock.unlock();

    size_t path_len = strlen(parent_path);
    m_map_lock.lock();
    for (it  = m_self_delete_watcher_map.begin();
         it != m_self_delete_watcher_map.end(); ) {
        if (0 == strncmp(it->first.c_str(), parent_path, path_len) &&
            ('\0' == (it->first)[path_len] || '/' == (it->first)[path_len])) {
            DSTREAM_DEBUG("remove watcher[%p]@node:[%s]", it->second, it->first.c_str());
            m_self_delete_watcher_map.erase(it++);
        } else {
            it++;
        }
    }
    m_map_lock.unlock();

    return error::OK;
}

void ZkClient::DumpStatus() {
    /* dump zk status */
    // DSTREAM_DEBUG("Zookeeper State:[%s]", m_zk_handler?
    //                                       state2String(m_zk_handler->state):
    //                                       "not connect yet");

    WatchMap::iterator it;

    /* dump node_watch map */
    m_map_lock.lock();
    DSTREAM_DEBUG("\tDUMP Node_Watch_Tree:(%lu nodes)", m_node_watcher_map.size());
    for (it = m_node_watcher_map.begin(); it != m_node_watcher_map.end(); it++) {
        DSTREAM_DEBUG("\t-----Node Watcher@path[%s]", it->first.c_str());
    }
    m_map_lock.unlock();

    /* dump delete_watch map */
    m_map_lock.lock();
    DSTREAM_DEBUG("\tDUMP Children_Delete_Watch_Tree:(%ld nodes)",
                  m_child_delete_watcher_map.size());
    for (it  = m_child_delete_watcher_map.begin();
         it != m_child_delete_watcher_map.end();
         it++) {
        DSTREAM_DEBUG("\t-----Node Watcher@path[%s]", it->first.c_str());
    }
    m_map_lock.unlock();

    /* dump self_delete_watch map */
    m_map_lock.lock();
    DSTREAM_DEBUG("\tDUMP Self_Delete_Watch_Tree:(%lu nodes)",
                  m_self_delete_watcher_map.size());
    for (it  = m_self_delete_watcher_map.begin();
         it != m_self_delete_watcher_map.end();
         it++) {
        DSTREAM_DEBUG("\t-----Node Watcher@path[%s]", it->first.c_str());
    }
    m_map_lock.unlock();
}

void ZkClient::WatcherFun(zhandle_t* /*zk_handler*/, int type, int state,
                          const char* path, void* context) {
    ZkClient* zk_client = reinterpret_cast<ZkClient*>(context);
    // DSTREAM_DEBUG("get event:[%d], state:[%d] at path:[%s]",
    //               type, state, path);
    if (NULL == zk_client) {
        DSTREAM_DEBUG("no ZkClient object to receive zookeeper event");
        return;
    }

    zk_client->OnEvent(type, state, path);
    return;
}

error::Code ZkClient::OnEvent(int type, int state, const char* path) {
    if (ZOO_SESSION_EVENT == type) { /* session event */
        if (ZOO_CONNECTED_STATE == state) {
            /* connection is ok */
            OnConnect();
            return error::OK;
        }
        if (ZOO_EXPIRED_SESSION_STATE == state) {
            /* session expires, trigger callback */
            DSTREAM_WARN("the session with zookeeper expires");
            struct Watcher* w = m_conn_broken_watcher;
            if (w) {
                (Watcher::Callback)(type, state, path, w);
                return error::OK;
            }
            DSTREAM_DEBUG("conn to zk is broken, no session callback be set");
        }
        if (0 == state) { /* TODO: check this */
            /* the connection with zookeeper servers is temporarily unavailable */
            DSTREAM_WARN("the conn to zk is temporarily unavailable");
        }
        return error::OK;
    }

    if (ZOO_DELETED_EVENT == type) {
        DSTREAM_DEBUG("get DELETE_EVENT on node:[%s] ...", path);
        /* handle delete event */
        WatchMap::iterator it;
        m_map_lock.lock();
        it = m_child_all_watcher_map.find(path);
        if (it != m_child_all_watcher_map.end()) {
            DSTREAM_DEBUG("delete watch on node:[%s] from child_all_watcher map", path);
            m_child_all_watcher_map.erase(it);
        }
        it = m_self_delete_watcher_map.find(path);
        if (it != m_self_delete_watcher_map.end()) {
            Watcher* w = it->second;
            DSTREAM_DEBUG("delete watch on node:[%s] from self_delete_watcher map", path);
            m_self_delete_watcher_map.erase(it);
            m_map_lock.unlock();
            /* execute callback */
            DSTREAM_DEBUG("trigger self_delete_watcher[%p] on node:[%s] ...", w, path);
            (Watcher::Callback)(type, state, path, w);
            return error::OK;
        }
        m_map_lock.unlock();
    }

    if (ZOO_CHANGED_EVENT == type) {
        DSTREAM_DEBUG("get CHANGED_EVENT on node:[%s] ...", path);
        /* rewatch */
        if (WatchNode(path) < error::OK) {
            DSTREAM_WARN("fail to rewath on node:[%s]", path);
        }

        /* handle delete event */
        WatchMap::iterator it;
        m_map_lock.lock();
        it = m_child_all_watcher_map.find(path);
        if (it != m_child_all_watcher_map.end()) {
            Watcher* w = it->second;
            m_map_lock.unlock();
            /* execute callback */
            DSTREAM_DEBUG("trigger child_all_watcher[%p] on node:[%s] ...", w, path);
            (Watcher::Callback)(type, state, path, w);
            return error::OK;
        }
        m_map_lock.unlock();
    }

    /* rewatch */
    if (WatchNode(path) < error::OK) {
        DSTREAM_WARN("fail to rewath on node:[%s]", path);
    }

    /* if the path is in the child_delete_watch map, then execute callback */
    /* Note*
     * If you only care about child delete event, then shouldn't execute callback.
     * because ON_DELETE_EVENT already handle this.
     * If you care about all child event, then execute callback here.
     * But may be you will get two event(one is child event, the other is delete event.
     * So be careful here if you use watch child all */
    if (ZOO_CHILD_EVENT == type) {
        /* handle child event */
        DSTREAM_DEBUG("get CHILD_EVENT on node:[%s] ...", path);
        WatchMap::iterator it;
        error::Code ret;
        m_map_lock.lock();
        it = m_child_all_watcher_map.find(path);
        if (it != m_child_all_watcher_map.end()) {
            m_map_lock.unlock();
            ret = OnChildEvent(path, true);
            Watcher* w = it->second;
            (Watcher::Callback)(type, state, path, w);
            return ret;
        }
        it = m_child_delete_watcher_map.find(path);
        if (it != m_child_delete_watcher_map.end()) {
            m_map_lock.unlock();
            ret = OnChildEvent(path, false);
            return ret;
        }
        m_map_lock.unlock();
    }

    /* if the path is in the node watch map, then execute callback */
    m_map_lock.lock();
    WatchMap::iterator it;
    it = m_node_watcher_map.find(path);
    if (it != m_node_watcher_map.end()) {
        Watcher* w = it->second;
        if (ZOO_DELETED_EVENT == type) {
            m_node_watcher_map.erase(it);
            DSTREAM_DEBUG("delete watcher[%p] on node:[%s] from due to node deleted", w, path);
        }
        m_map_lock.unlock();
        /* execute callback */
        DSTREAM_DEBUG("trigger watcher[%p] on node:[%s] ...", w, path);
        (Watcher::Callback)(type, state, path, w);
        return error::OK;
    }
    m_map_lock.unlock();

    /* rewatch */
    DSTREAM_DEBUG("get ignored event:[%d], state:[%d] at path:[%s]",
                  type, state, path);

    return error::OK;
}

error::Code ZkClient::OnChildEvent(const char* parent_path, bool all) {
    /* get watcher on node */
    m_map_lock.lock();
    WatchMap::iterator it;
    if (all) {
        DSTREAM_DEBUG("got a all child event");
        it = m_child_all_watcher_map.find(parent_path);
        if (it == m_child_all_watcher_map.end()) {
            m_map_lock.unlock();
            DSTREAM_WARN("fail to get wather on node:[%s]", parent_path);
            return error::ZK_GET_WATCHER_FAIL;
        }
    } else {
        DSTREAM_DEBUG("got a child delete event");
        it = m_child_delete_watcher_map.find(parent_path);
        if (it == m_child_delete_watcher_map.end()) {
            m_map_lock.unlock();
            DSTREAM_WARN("fail to get wather on node:[%s]", parent_path);
            return error::ZK_GET_WATCHER_FAIL;
        }
    }
    Watcher* delete_watcher = it->second;
    m_map_lock.unlock();
    if (NULL == delete_watcher) {
        DSTREAM_WARN("fail to get wather on node:[%s]", parent_path);
        return error::ZK_GET_WATCHER_FAIL;
    }

    /* recheck all children and watch */
    int32_t ret = WatchChildren(parent_path,
                                &m_self_delete_watcher_map,
                                delete_watcher);
    if (ret < error::OK) {
        return error::ZK_SET_WATCHER_FAIL;
    }

    DSTREAM_DEBUG("handle children event ...OK");
    return error::OK;
}

void ZkClient::OnConnect() {
    /* notify Connect function */
    // DSTREAM_DEBUG("Connect to zookeeper[%s] ...OK", m_servers_list.c_str());
    m_condition.notify();
}

bool ZkClient::IsConnected() {
    return ZOO_CONNECTED_STATE == zoo_state(m_zk_handler);
}

error::Code ZkClient::CreateNode(const char* path, ZkNode* node, int flag) {
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
        return zrc == ZNODEEXISTS ? error::ZK_NODE_EXISTS : error::ZK_CREATE_NODE_FAIL;
    }

    DSTREAM_DEBUG("create node@[%s] ...OK", (NULL == node) ?
                  path : node->path.m_buf);
    return error::OK;
}

error::Code ZkClient::DeleteNode(const char* path) {
    int32_t zrc = zoo_delete(m_zk_handler, path, -1);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to delete node@[%s], ErrCode:[%d] Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_DELETE_NODE_FAIL;
    }

    DSTREAM_DEBUG("delete node@[%s] ...OK", path);
    return error::OK;
}

error::Code ZkClient::WatchNode(const char* path) {
    int32_t zrc = zoo_wexists(m_zk_handler, path, WatcherFun, this, NULL);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to set watch on node:[%s], ErrCode:[%d], Err:[%s]",
                     path, zrc, zerror(zrc));
        return error::ZK_SET_WATCHER_FAIL;
    }
    return error::OK;
}

error::Code ZkClient::WatchChildren(const char* parent_path,
                                    WatchMap* self_delete_watcher_map,
                                    Watcher* delete_watcher) {
    error::Code ret = error::OK;
    int32_t zrc = ZOK;
    int32_t i = 0;

    String_vector children_strings = {0, NULL};

    zrc = zoo_wget_children(m_zk_handler, parent_path, WatcherFun,
                            this, &children_strings);
    if (ZOK != zrc) {
        DSTREAM_WARN("fail to get node children@[%s], ErrCode:[%d], Err:[%s]",
                     parent_path, zrc, zerror(zrc));
        ret = error::ZK_GET_CHILDREN_FAIL;
        goto  ERR_RET;
    }

    /* add to watch map and watch */
    for (i = 0; i < children_strings.count; i++) {
        /* watch */
        std::pair<std::string, Watcher*> watcher_pair(parent_path, delete_watcher);
        watcher_pair.first.append("/").append(children_strings.data[i]);
        WatchMap::iterator it;
        m_map_lock.lock();
        it = self_delete_watcher_map->find(watcher_pair.first.c_str());
        if (it != self_delete_watcher_map->end()) {
            DSTREAM_DEBUG("Wacher on node:[%s] already exists, will not rewatch",
                          watcher_pair.first.c_str());
            m_map_lock.unlock();
            continue;
        }
        /* add to watch map */
        self_delete_watcher_map->insert(watcher_pair);

        it = m_child_all_watcher_map.find(watcher_pair.first.c_str());
        if (it != m_child_all_watcher_map.end()) {
            DSTREAM_DEBUG("Wacher on node:[%s] already exists, will not rewatch",
                          watcher_pair.first.c_str());
            m_map_lock.unlock();
            continue;
        }
        m_child_all_watcher_map.insert(watcher_pair);
        DSTREAM_DEBUG("set watch on node:[%s] ...OK", watcher_pair.first.c_str());

        m_map_lock.unlock();

        zrc = WatchNode(watcher_pair.first.c_str());
        if (zrc < error::OK) {
            ret = error::ZK_SET_WATCHER_FAIL;
            goto  ERR_RET;
        }
    }

    DSTREAM_DEBUG("set CHILDREN watcher on node:[%s] ...OK", parent_path);
    deallocate_String_vector(&children_strings);
    return error::OK;

ERR_RET:
    WatchMap::iterator it;
    std::string abs_path;
    if (children_strings.count > 0) {
        m_map_lock.lock();
        for (int32_t j = i; j >= 0; j--) {
            abs_path.clear();
            abs_path.append(parent_path).append("/").append(children_strings.data[j]);
            it = self_delete_watcher_map->find(abs_path);
            if (it != self_delete_watcher_map->end()) {
                DSTREAM_DEBUG("remove watcher[%p]@[%s] from watch_node_map",
                              it->second, it->first.c_str());
                self_delete_watcher_map->erase(it);
            }
        }
        m_map_lock.unlock();
    }
    deallocate_String_vector(&children_strings);

    return ret;
}

} // namespace zk_client
} // namespace dstream
