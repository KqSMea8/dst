/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief rpc server
 */

#include "common/rpc_server.h"

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>

#include "common/rpc_const.h"
#include "common/utils.h"

namespace dstream {
namespace rpc_server {

static const int kTryStartPort = 5000;
static const int kTryEndPort   = 60000;

void* RPCServerWorkThread(void* ptr) {
    RPCServer* server = reinterpret_cast<RPCServer*>(ptr);
    if (!server->StartService()) {
        DSTREAM_WARN("start service fail");
        return NULL;
    }
    DSTREAM_INFO("rpc server thread stop");
    return NULL;
}

RPCServer::~RPCServer() {
    StopServer();
}

bool RPCServer::RegisterProtocol(const char* method, rpc::Service* service) {
    if (NULL == m_connection) {
        rpc::Context::init();
        if (NULL == (m_connection = new rpc::Connection)) {
            DSTREAM_WARN("connection is not init");
            return false;
        }
    }
    m_connection->RegisterService(method, service);
    return true;
}

bool RPCServer::StartServer(int port, std::string host, int call_back_threads) {
    MutexLockGuard lock_guard(m_lock);
    if (m_running) {
        DSTREAM_INFO("rpc server already running");
        return true;
    }
    if (m_stop) {
        m_stop = false;
    }
    if (NULL == m_connection) {
        rpc::Context::init();
        if (NULL == (m_connection = new rpc::Connection)) {
            DSTREAM_WARN("connection is not init");
            return false;
        }
    }
    if (host == "*") {
        if (error::OK != GetLocalhostIP(&m_server_ip)) {
            DSTREAM_WARN("get local ip fail");
            return false;
        }
    } else {
        m_server_ip = host;
    }
    m_server_port = port;
    m_callback_threads = call_back_threads;
    m_running = true;
    if (pthread_create(&m_working_thread, NULL, RPCServerWorkThread, this) != 0) {
        DSTREAM_WARN("create server thread fail");
        return false;
    }
    while (m_running && !m_connection->working()) {
        usleep(10000);
    }
    return m_running;
}

bool RPCServer::StartServer(int call_back_threads) {
    return StartServer(0, "*", call_back_threads);
}

bool RPCServer::StartService() {
    if (m_server_port == 0) {
        for (m_server_port = kTryStartPort; m_server_port <= kTryEndPort; ++m_server_port) {
            m_connection->Serve(rpc_const::GetUri(m_server_ip, m_server_port).c_str(),
            m_callback_threads);
            if (m_stop) {
                DSTREAM_INFO("rpc server stopped");
                return true;
            }
        }
    } else {
        m_connection->Serve(rpc_const::GetUri(m_server_ip, m_server_port).c_str(),
                            m_callback_threads);
        if (m_stop) {
            DSTREAM_INFO("rpc server stopped");
            return true;
        }
    }
    return (m_running = false);
}

void RPCServer::StopServer() {
    DSTREAM_DEBUG("Stop rpc server");
    m_lock.lock();
    m_stop    = true;
    if (m_working_thread > 0 && NULL != m_connection) {
        pthread_t working_thread_tmp = m_working_thread;
        rpc::Connection* connection_tmp = m_connection;
        m_working_thread    = 0;
        DSTREAM_DEBUG("stop connection");
        m_connection->Stop();
        m_connection = NULL;
        m_running = false;
        m_lock.unlock();
        pthread_join(working_thread_tmp, NULL);
        delete connection_tmp;
        DSTREAM_DEBUG("Stop rpc server");
        return;
    }
    if (NULL != m_connection) {
        DSTREAM_DEBUG("stop connection");
        m_connection->Stop();
        delete m_connection;
        m_connection = NULL;
    }
    m_lock.unlock();
    DSTREAM_DEBUG("Stop rpc server");
}

} // namespace rpc_server
} // namespace dstream
