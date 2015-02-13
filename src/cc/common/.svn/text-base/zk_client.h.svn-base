/**
* @file   zk_client.h
* @brief    zookeeper client wrapper
* @author   konghui, konghui@baidu.com
* @version   1.0.1.0
* @date   2011-11-30
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_COMMON_ZK_CLIENT_H__ // NOLINT
#define __DSTREAM_CC_COMMON_ZK_CLIENT_H__ // NOLINT

#include <zookeeper.h>
#include <list>
#include <map>
#include <string>
#include <vector>
#include "common/blocking_queue.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/mutex.h"

namespace dstream {
namespace zk_client {

enum NodeAttribute {
    PERSISTENT  = 0,
    EPHEMERAL   = 1
};

class Data {
public:
    Data(const Data& d) {
        if (m_buf) {
            // free(buf);
        }
        m_buf = d.m_buf;
        m_len = d.m_len;
    }
    Data() : m_buf(NULL), m_len(-1) {
    }

    char* m_buf;    // the buffer holding data
    int   m_len;    // the size of the data buffer.
                    // It'll be set to the actual data length upon return.
                    // If the data is NULL, length is -1.
};

/**
* @brief    zookeeper node path.
*/
typedef Data ZkNodePath;
/**
* @brief    zookeeper node data.
*/
typedef Data ZkNodeData;

/**
* @brief  zookeeper node
*/
struct ZkNode {
    ZkNodePath  path; /**< node path */
    ZkNodeData  data; /**< node data */
};
ZkNode* NewZkNode();
void DeleteZkNode(ZkNode* node);

/**
* @brief  callback class
* @author   konghui, konghui@baidu.com
* @modify   liuguodong,liuguodong01@baidu.com %s/struct Watcher/class Watcher 
* @date   2011-12-08
*/
class Watcher {
public:
    virtual ~Watcher() {}
    virtual void OnEvent(int type, int state, const char* path) = 0;
    static void Callback(int type, int state, const char* path, void* ctx) {
        DSTREAM_DEBUG("get event: type:[%d], state:[%d], path:[%s]",
                      type, state, path);
        if (NULL == ctx) {
            DSTREAM_WARN("bad parameter, watcher is NULL");
            return;
        }
        Watcher* watcher = reinterpret_cast<Watcher*>(ctx);
        watcher->OnEvent(type, state, path);
    }
};

/**
* @brief    zookeeper client
* @author   konghui, konghui@baidu.com
* @date     2011-11-30
* @notice   1. All the operators of this class are synchronous.
*           2. this class is not thread-safe
*/
class ZkClient {
public:
    static const int32_t  kZkMaxNodePathLen   = 1024;
    /**< max path length of zookeeper node */
    static const int32_t  kZkRecvTimeoutMs = 30000;  /**< receive time out, ms */
    static const int32_t  kZkMaxSingleNodePathLen = 1024;
    /**< max path length of a single zookeeper node */
    static const int32_t  kZkMaxSingleNodeDataLen = 1024 * 1024;
    /**< max data length of a single zookeeper node */
    static const int32_t  kZkMaxPathLevel = 256; /**< max zk node path level */

    typedef std::map<std::string, Watcher*> WatchMap;

public:
    /**
    * @brief  constructor
    *
    * @param[in]  zk_servers_list   zookeeper servers list, comma separated host:ip pairs,
    *                               echo corresponding to a zK server.
    *                               format. "ip:port, ip:port"
    *                               e.g. "127.0.0.1:3000,127.0.0.1:3001,127.0.0.1:3002"
    * @param[in]  recv_timeout_ms  session expire time, default 3000 ms.
    * @param[in]  conn_broken_watcher   callback object for connection broken
    */
    ZkClient()
        : m_recv_timeout_ms(kZkRecvTimeoutMs),
          m_zk_log_level(ZOO_LOG_LEVEL_INFO),
          m_connected(false),
          m_condition(m_condition_lock),
          m_zk_handler(NULL),
          m_conn_broken_watcher(NULL) {
    }

    explicit ZkClient(const char* zk_servers_list)
        : m_recv_timeout_ms(kZkRecvTimeoutMs),
          m_servers_list(zk_servers_list),
          m_zk_log_level(ZOO_LOG_LEVEL_INFO),
          m_connected(false),
          m_condition(m_condition_lock),
          m_zk_handler(NULL),
          m_conn_broken_watcher(NULL) {
    }
    ZkClient(const char* zk_servers_list,
             const int recv_timeout_ms)
        : m_recv_timeout_ms(recv_timeout_ms),
          m_servers_list(zk_servers_list),
          m_zk_log_level(ZOO_LOG_LEVEL_INFO),
          m_connected(false),
          m_condition(m_condition_lock),
          m_zk_handler(NULL),
          m_conn_broken_watcher(NULL) {
    }
    ZkClient(const char* zk_servers_list,
             const int recv_timeout_ms,
             Watcher* conn_broken_watcher)
        : m_recv_timeout_ms(recv_timeout_ms),
          m_servers_list(zk_servers_list),
          m_zk_log_level(ZOO_LOG_LEVEL_INFO),
          m_connected(false),
          m_condition(m_condition_lock),
          m_zk_handler(NULL),
          m_conn_broken_watcher(conn_broken_watcher) {
    }
    ZkClient(const char* zk_servers_list,
             const int recv_timeout_ms,
             Watcher* conn_broken_watcher,
             ZooLogLevel level)
        : m_recv_timeout_ms(recv_timeout_ms),
          m_servers_list(zk_servers_list),
          m_zk_log_level(level),
          m_connected(false),
          m_condition(m_condition_lock),
          m_zk_handler(NULL),
          m_conn_broken_watcher(conn_broken_watcher) {
    }
    virtual ~ZkClient();

    /**
    * @brief  random pick up a server and connect.
    *
    * @return    error::OK: success
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-11-30
    */
    virtual error::Code Connect();
    /**
    * @brief  check wether connection to zk is valid or not.
    *
    * @return    ture: valid connection
    *           false: invalid connection
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual bool IsConnected();
    /**
    * @brief  disconnect to zk
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual void Disconnect();

    /**
    * @brief  set zookeeper connection broken watcher.
    *
    * @param[in]  watcher watcher for zookeeper connection broken
    *
    * @return  error::OK: success
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-08
    */
    virtual error::Code SetConnBrokenWatcher(Watcher* watcher);

    /**
    * @brief  check whether the node exists.
    *
    * @param[in]    path    the abstract path of the node
    * @param[out]   exists  0 if the node does not exist, else 1
    * @param[out]   attr    the atrribute: PERSISTENT or EPHEMERAL
    *
    * @return   error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code NodeExists(const char* path, int32_t* exist, int32_t* attr);

    /**
    * @brief  get the given node data.
    *
    * @param[in]  path  the abstract path of the node
    * @param[in][out]  node_data  the data of the node
    *
    * @return   error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code GetNodeData(const char* path, ZkNodeData* node_data);
    /**
    * @brief  get the child list of given node
    *
    * @param[in]  path  the abstract path of the node
    * @param[in][out]  children  the data list of the node's children
    *
    * @return    error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    * @notice   user is responsible to free the node buf.
    */
    virtual error::Code GetNodeChildren(const char* path, std::list<ZkNode*>* children);
    virtual error::Code GetChildList(const char* path, std::list<std::string>* child_list);

    /**
    * @brief  set node data.
    *
    * @param[in]  path  the abstract path of the node
    * @param[in]  node_data  the data of the node
    *
    * @return   error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code SetNodeData(const char* path, const ZkNodeData& node_data);

    /**
    * @brief  init system root node path
    *
    * @param[in]  root_path root node path
    * @param[in]  node_data_list nodes' data.
    *                       echo node data corresponding the node in the path.
    *                       eg: root_path='/dstream/pn'
    *                           node_data_list[0]="data of 'dstream' node"
    *                           node_data_list[1]="data of 'pn' node"
    *
    * @return   error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-15
    */
    virtual error::Code InitSystemRoot(const char* root_path);
    virtual error::Code InitSystemRoot(const char* root_path, std::vector<ZkNode*> node_data_list);
    virtual error::Code SystemRootExists(const char* root_path, int32_t* exists);

    /**
    * @brief  create persistent node.
    *
    * @param[in]  path  the abstract path of the node
    * @param[in]  node  the data of the node
    *             To set the node.data.buf to be NULL use
    *             node.data.buf as NULL and node.data.len as -1;
    *             node.path which will be filled with the path of the
    *             new node.The path string will always be null-terminated.
    *
    * @return   error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code CreatePersistentNode(const char* path, ZkNode* node);
    /**
    * @brief  delete persistent node.
    *
    * @param[in]  path  the abstract path of the node
    *
    * @return    error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    * @notice   will fail when node has child nodes. see also@ref deleteNodeTree
    */
    virtual error::Code DeletePersistentNode(const char* path);
    /**
    * @brief  delete node tree
    *
    * @param[in]  path  the abstract path of the node
    * @param[in]  root_delete  whether delete root path or not
    *
    * @return    error::OK: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code DeleteNodeTree(const char* path, bool root_delete = true);

    /**
    * @brief  create ephemeral node.
    *
    * @param[in]  path  The name of the node. Expressed as a file name with
    *                   slashes separating ancestors of the node.
    * @param[in][out]  node  the data of the node
    *                        To set the node_data.data to be NULL use
    *                        node_data.data as NULL and node_data.data_len as -1;
    *                        node.path which will be filled with the path of the
    *                        new node (this might be different than the supplied path
    *                        because of the enable_sequence be set to 1 or ignored).
    *                        The path string will always be null-terminated.
    * @param[in]  enable_sequence   the flag that should sequence node be enabled.
    *                               0 disable the sequence feature.
    *                               1 or ignored will enable the sequence feature.
    *
    * @return   error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code CreateEphemeralNode(const char* path, ZkNode* node);
    virtual error::Code CreateEphemeralNode(const char* path, ZkNode* node,
                                            int32_t enable_sequence);
    /**
    * @brief  delete ephemeral node
    *
    * @param[in]  path  the abstract path of the node
    *
    * @return    error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    virtual error::Code DeleteEphemeralNode(const char* path);

    /**
    * @brief  Watch a node and notify watcher if any events happens.
    *
    * @param[in]  path  the abstract path of the node
    * @param[in][out]  watcher  the watcher that observes the node's change
    *
    * @return    error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-07
    */
    error::Code WatchNode(const char* path, Watcher* watcher);

    /**
    * @brief  watch the node's children and notify the watcher
    *         when a DELETE_EVENT happens.
    *
    * @param[in]  parent_path  the parent' abstract path of the node
    * @param[in][out]  delete_watcher wather for children's deletion
    *
    * @return   error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-07
    */
    error::Code WatchChildrenDelete(const char* parent_path, Watcher* delete_watcher);

    /**
    * @brief  watch the node's children and notify the watcher
    *         when any event happens.
    *
    * @param  parent_path : the parent's abstract path of the node
    * @param  all_watcher : all_watcher wather for children's events
    *
    * @return   error::OK : success
    *           other error code(<0) : fail
    * @author   fangjun, fangjun02@baidu.com
    * @date   2012-12-26
    */
    error::Code WatchChildrenAll(const char* parent_path, Watcher* all_watcher);

    /**
    * @brief  unwatch node
    *
    * @param[in]  path  the abstract path of the node
    *
    * @return   error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-07
    */
    error::Code UnWatchNode(const char* path);
    /**
    * @brief  unwatch node and its children
    *
    * @param[in]  parent_path  the parent' abstract path of the node
    *
    * @return   error::ok: success;
    *           othe error code(<0): fail
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-07
    */
    error::Code UnWatchChildrenDelete(const char* parent_path);

    /**
    * @brief  unwatch node and its children
    *
    * @param  parent_path the parent' abstract path of the node
    *
    * @return   error::ok: success;
    *           othe error code(<0): fail
    * @author   fangjun, fangjun02@baidu.com
    * @date   2012-12-26
    */
    error::Code UnWatchChildrenAll(const char* parent_path);

    /**
    * @brief    debug functions
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    */
    void DumpStatus();

    /**
    * @brief  handle events
    *
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-01
    * @notice   used intervally, should not be called by application
    */
    error::Code OnEvent(int type, int state, const char* path);

private:
    /**
     * forbid copy constructor and assignment constructor;
     * */
    ZkClient(const ZkClient& another_zk_client);
    ZkClient& operator= (const ZkClient& another_zk_client); // NOLINT

    /**
    * @brief  watch function.
    *
    * @param[in] zk_handler zookeeper handle
    * @param[in] type event type. This is one of the *_EVENT constants.
    * @param[in] state connection state. The state value will be one of the *_STATE constants.
    * @param[out] path znode path for which the watcher is triggered. NULL if the event
    *                 type is ZOO_SESSION_EVENT
    * @param[out] watcherCtx watcher context.
    *
    * @author   konghui, konghui@baidu.com
    * @date   2011-12-04
    */
    static void WatcherFun(zhandle_t* zk_handler, int type, int state,
                           const char* path, void* context);

    void    OnConnect();
    /**
    * @brief  on child event
    *
    * @param  parent_path
    * @param  add : if true, it could be any child event, otherwise it is a child delete event
    *
    * @return
    * @author   fangjun, fangjun02@baidu.com
    * @date   2012-12-27
    */
    error::Code OnChildEvent(const char* parent_path, bool all);
    error::Code CreateNode(const char* path, ZkNode* node, int flag);
    error::Code DeleteNode(const char* path);
    error::Code WatchNode(const char* path);
    error::Code WatchChildren(const char* parent_path,
                              WatchMap* self_delete_watcher_map,
                              Watcher* delete_watcher);

private:
    int32_t                    m_recv_timeout_ms;/**< receive time out, ms */
    std::string                m_servers_list;   /**< zookeeper servers list */
    ZooLogLevel                m_zk_log_level;
    volatile bool              m_connected;      /**< conn flag */
    MutexLock                  m_condition_lock;      /**< mutex for cond */
    Condition                  m_condition;           /**< condition for conn event notice */
    zhandle_t*                 m_zk_handler;         /**< current zk session  handler */

    Watcher*  m_conn_broken_watcher;     /**< conn broken watcher */
    MutexLock m_map_lock;                /**< mutex for map */
    WatchMap  m_node_watcher_map;        /**< watch any events of the nodes in this map */
    WatchMap  m_child_delete_watcher_map;/**< watch child_delete_event of the nodes in this map */
    WatchMap  m_child_all_watcher_map;   /*< watch child add event of the nodes in this map */
    WatchMap  m_self_delete_watcher_map; /**< watch delete_event of the nodes in this map*/
}; // class ZkClient

} // namespace zk_client
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_ZK_CLIENT_H__ NOLINT
