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

#ifndef __DSTREAM_CC_PROCESSNODE_ZK_OPERATOR_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_ZK_OPERATOR_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <string>
#include <vector>

#include "common/zk_client.h"
#include "metamanager/zk_config.h"
#include "metamanager/zk_meta_manager.h"

namespace dstream {
namespace router {

class ZkOperator {
public:
    typedef boost::shared_ptr<ZkOperator> ZkOperatorPtr;

    ZkOperator()
        : m_zk_handler(NULL),
          m_connected(false),
          m_cond(m_cond_lock),
          m_cb_handle(NULL),
          m_cb_context(NULL) {
    }
    ZkOperator(const char* zk_servers_list, const int recv_timeout)
        : m_recv_timeout(recv_timeout),
          m_zk_servers_list(zk_servers_list),
          m_zk_handler(NULL),
          m_connected(false),
          m_cond(m_cond_lock),
          m_cb_handle(NULL),
          m_cb_context(NULL) {
    }
    virtual ~ZkOperator();

    /**
     * @brief  random pick up a server and connect.
     * @return    error::OK: success
     *            other error code(<0): fail
     */
    virtual error::Code Connect();

    /**
     * @brief  check wether connection to zk is valid or not.
     * @return    ture: valid connection
     *            false: invalid connection
     */
    virtual bool IsConnected();

    /**
     * @brief  disconnect to zk
     */
    virtual void Disconnect();

    /**
     * @brief  check whether the node exists.
     * @param[in]    path    the abstract path of the node
     * @param[out]   exists  0 if the node does not exist, else 1
     * @param[out]   attr    the atrribute: PERSISTENT or EPHEMERAL
     * @return   error::OK: success;
     *           other error code(<0): fail
     */
    virtual error::Code NodeExists(const char* path, int32_t* exist, int32_t* attr);

    /**
     * @brief  get the given node data.
     * @param[in]  path  the abstract path of the node
     * @param[in][out]  node_data  the data of the node
     * @return   error::OK: success;
     *           other error code(<0): fail
     */
    virtual error::Code GetNodeData(const char* path, zk_meta_manager::MetaZKData*& node_data);

    /**
     * @brief  set node data.
     * @param[in]  path  the abstract path of the node
     * @param[in]  node_data  the data of the node
     * @return   error::OK: success;
     *           other error code(<0): fail
     */
    virtual error::Code SetNodeData(const char* path, zk_meta_manager::MetaZKData* node_data);

    /**
     * @brief  create persistent node.
     * @param[in]  path  the abstract path of the node
     * @param[in]  node  the data of the node
     *        To set the node.data.buf to be NULL use
     *        node.data.buf as NULL and node.data.len as -1;
     *        node.path which will be filled with the path of the
     *        new node.The path string will always be null-terminated.
     * @return   error::OK: success;
     *           other error code(<0): fail
     */
    virtual error::Code CreatePersistentNode(const char* path, zk_client::ZkNode* node);

    /**
     * @brief  delete persistent node.
     * @notice   will fail when node has child nodes. see also@ref deleteNodeTree
     * @param[in]  path  the abstract path of the node
     * @return    error::OK: success;
     *            other error code(<0): fail
     */
    virtual error::Code DeletePersistentNode(const char* path);

    /**
     * @brief  create ephemeral node.
     *
     * @param[in]  path  The name of the node. Expressed as a file name with
     *             slashes separating ancestors of the node.
     * @param[in][out]  node  the data of the node
     *             To set the node_data.data to be NULL use
     *             node_data.data as NULL and node_data.data_len as -1;
     *             node.path which will be filled with the path of the
     *             new node (this might be different than the supplied path
     *             because of the enable_sequence be set to 1 or ignored).
     *             The path string will always be null-terminated.
     * @param[in]  enable_sequence   the flag that should sequence node be enabled.
     *                      0 disable the sequence feature.
     *                      1 or ignored will enable the sequence feature.
     *
     * @return   error::ok: success;
     *           other error code(<0): fail
     */
    virtual error::Code CreateEphemeralNode(const char* path, zk_client::ZkNode* node);
    virtual error::Code CreateEphemeralNode(const char* path, zk_client::ZkNode* node,
                                            int32_t enable_sequence);

    /**
     * @brief  delete ephemeral node
     * @param[in]  path  the abstract path of the node
     * @return    error::ok: success;
     *            other error code(<0): fail
     */
    virtual error::Code DeleteEphemeralNode(const char* path);

    virtual error::Code GetChildList(const char* path, std::vector<std::string>* child_list);

    typedef int (*WatchEventCallback)(int type, int state, const char* path, void* context);

    /**
     * @brief set callback handle & context
     * @return void
     */
    virtual void SetWatchEventCallBack(WatchEventCallback callback, void* context) {
        m_cb_handle = callback;
        m_cb_context = context;
    }

protected:
    error::Code CreateNode(const char* path, zk_client::ZkNode* node, int flag);
    error::Code DeleteNode(const char* path);
    error::Code WatchNode(const char* path);
    void OnConnect();

    /**
     * @brief  watch function.
     * @param[in] zk_handler zookeeper handle
     * @param[in] type event type. This is one of the *_EVENT constants.
     * @param[in] state connection state. The state value will be one of the *_STATE constants.
     * @param[out] path znode path for which the watcher is triggered. NULL if the event
     *             type is ZOO_SESSION_EVENT
     * @param[out] watcherCtx watcher context.
     */
    static void WatcherFun(zhandle_t* zk_handler, int type, int state,
                           const char* path, void* context);

private:
    int32_t     m_recv_timeout; /**< receive time out, seconds */
    std::string m_zk_servers_list; /**< zookeeper servers list */
    zhandle_t*  m_zk_handler; /**< current zk session  handler */
    volatile bool m_connected; /**< flag for connection status */

    MutexLock m_cond_lock; /**< mutex for cond */
    Condition m_cond; /**< condition for connection event notice */

    // watch callback handle
    WatchEventCallback m_cb_handle;
    void* m_cb_context;
};


/************************************************************************/
/* GetZK data of path, T is a protobuffer type                          */
/************************************************************************/
template <class T>
error::Code GetZKData(ZkOperator::ZkOperatorPtr zk_client, const std::string& path, T* obj) {
    assert(NULL != zk_client);
    zk_meta_manager::MetaZKData zk_data;
    UNLIKELY_IF(!zk_data.AllocateBuf()) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    error::Code res = error::OK;
    // add for mock
    zk_meta_manager::MetaZKData* data_ptr = &zk_data;
    if ((res = zk_client->GetNodeData(path.c_str(), data_ptr)) != error::OK) {
        DSTREAM_WARN("get node [%s] fail", path.c_str());
        return res;
    }
    if (!obj->ParseFromArray(data_ptr->m_buf, data_ptr->m_len)) {
        DSTREAM_WARN("parse data error");
        return error::SERIAIL_DATA_FAIL;
    }
    return res;
}

/************************************************************************/
/* set zk data of path, T is protobuffer type                           */
/************************************************************************/
template <class T>
error::Code SetZKData(ZkOperator::ZkOperatorPtr zk_client, const std::string& path, const T& obj) {
    assert(NULL != zk_client);
    zk_meta_manager::MetaZKData zk_data;
    UNLIKELY_IF(!zk_data.AllocateBuf(obj.ByteSize())) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    if (!obj.SerializeToArray(zk_data.m_buf, zk_data.m_len)) {
        DSTREAM_WARN("serialize data fail");
        return error::SERIAIL_DATA_FAIL;
    }
    error::Code res;
    if ((res = zk_client->SetNodeData(path.c_str(), zk_data)) != error::OK) {
        DSTREAM_WARN("update zk data fail");
    }
    return res;
}

class ZkWrapper {
public:
    typedef boost::shared_ptr<router::ZkWrapper> ZkWrapperPtr;

    ZkWrapper();
    virtual ~ZkWrapper();

    /**
     * @brief init work
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code Init(const config::Config& config, ZkOperator::WatchEventCallback cb_func,
                             void* ctx);

    /**
     * @brief clearup work
     */
    virtual void Destroy();

    /**
     * @brief get pm info from zookeeper
     *
     * @param pm[out] output PM info
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code GetPM(PM* pm);

    /**
     * @brief get pn info from zookeeper
     *        do fetch until OK, except getting NO_NODE error
     *
     * @param pn[out] output PN info
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code GetPN(const PNID& pnid, PN* pn);

    /**
     * @brief set PN node at zookeeper
     *
     * @param pn[in] input PN info
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code AddPN(const PN& pn);

    /**
     * @brief clear PN node at zookeeper
     *
     * @param pn[in] input PN info
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code DeletePN(const PN& pn);

    /**
     * @brief Get Application node at zookeeper
     *
     * @param app_id[in] indicate which app
     * @param app[out] output App info
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code GetApplication(const AppID& app_id, Application* app);

    /**
     * @brief Get Application children list at zookeeper
     *
     * @param app_id[in] indicate which app
     * @param peid_list[out] output child peid list
     * @return error::OK success
     *         other     fail
     */
    virtual error::Code GetPEList(const AppID& app_id, std::vector<std::string>* peid_list);

    /**
    * @brief Get ProcessorElement node at zookeeper
    *
    * @param app_id[in] indicate which app
    * @param pe_id[in] indicate which pe
    * @param pe[out] output ProcessorElement info
    * @return error::OK success
    *         other     fail
    */
    virtual error::Code GetProcessorElement(const AppID& app_id, const PEID& pe_id,
                                            ProcessorElement* pe);

protected:
    error::Code AddZKNode(const std::string& path,
                          const ::google::protobuf::Message* msg,
                          bool persist_node,
                          int enable_sequence = 0,
                          std::string* node_path = 0);

    std::string GetPNPath(const PNID& pn_id);
    std::string GetPEPath(const AppID& app_id, const PEID& pe_id);
    std::string GetAppPath(const AppID& app_id);

private:
    // zookeeper operator
    ZkOperator::ZkOperatorPtr m_zk_operator;
    // config item
    std::string m_root_path;
    std::string m_app_root_path;
    std::string m_pm_path;
    std::string m_pn_root_path;
    std::string m_zk_addr;
    int m_zk_timeout;
};

} // namespace router
} // namespace dstream

#endif // NOLINT
