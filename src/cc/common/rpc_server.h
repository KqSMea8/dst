/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief rpc server
 */

#ifndef __DSTREAM_CC_COMMON_RPC_SERVER_H__ // NOLINT
#define __DSTREAM_CC_COMMON_RPC_SERVER_H__ // NOLINT

#include <pthread.h>
#include <cstddef>
#include <string>

#include "common/mutex.h"
#include "common/rpc.h"

namespace dstream {
namespace rpc_server {

// RPCServer working Thread
void* RPCServerWorkThread(void* ptr);

/************************************************************************/
/* RPC Server : Server work in a single thread                          */
/************************************************************************/
class RPCServer {
public:
    RPCServer() : m_running(false), m_stop(false), m_connection(NULL), m_server_port(-1),
        m_working_thread(0) {}
    ~RPCServer();
    bool RegisterProtocol(const char* method, rpc::Service* service);
    bool running() const {
        return m_running;
    }
    bool StartServer(int port, std::string host, int call_back_threads = 4);
    bool StartServer(int call_back_threads = 4);
    void StopServer();
    const std::string& server_ip() const {
        return m_server_ip;
    }
    int server_port() {
        return m_server_port;
    }

private:
    friend void* RPCServerWorkThread(void* ptr);
    bool StartService();
    bool m_running;
    bool m_stop;
    rpc::Connection*  m_connection;
    int m_server_port;
    pthread_t m_working_thread;
    std::string m_server_ip;
    int m_callback_threads;
    MutexLock m_lock;
};

} // namespace rpc_server
}   // namespace dstream
#endif //__DSTREAM_CC_COMMON_RPC_SERVER_H__ // NOLINT
