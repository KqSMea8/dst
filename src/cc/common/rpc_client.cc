#include "common/rpc_client.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <cstddef>

namespace dstream {
namespace rpc_client {
using namespace rpc;
using namespace proto_rpc;

AsyncRPCConnection::~AsyncRPCConnection() {
    size_t req_num = m_requests.size();
    for (size_t i = 0; i < req_num; ++i) {
        m_handlers[i]->ReleaseHandler(m_requests[i], NULL);
    }
    ClearRequest();
    if (NULL != m_conn) {
        delete m_conn;
        m_conn = NULL;
    }
}

void AsyncRPCConnection::ClearRequest() {
    m_requests.clear();
    m_handlers.clear();
    m_timeout = 0;
}

AsyncRPCClient::AsyncRPCClient(int thread_num)
    : m_thread_num(thread_num), m_list_lock(NULL), m_add_new_call(false) {
}

bool AsyncRPCClient::Initialize() {
    if (m_thread_num > 0) {
        m_list_lock = new MutexLock;
        return NULL != m_list_lock;
    }
    return true;
}

AsyncRPCClient::~AsyncRPCClient() {
    if (NULL != m_list_lock) {
        m_list_lock->lock();
    }
    if (m_uri_conn_map.size() > 0) {
        UriConnMapIter end = m_uri_conn_map.end();
        for (UriConnMapIter iter = m_uri_conn_map.begin(); iter != end; ++iter) {
            delete iter->second;
        }
    }
}

bool AsyncRPCClient::call(const char* uri, const char* method, Request* request,
                          AsyncRPCHandler* handler, int timeout /* = 5000 */) {
    if (NULL == uri || NULL == method || NULL == request
        || NULL == handler || timeout <= 0) {
        DSTREAM_WARN("rpc call argument error");
        return false;
    }
    request->set_method(method);
    bool res = true;
    if (NULL != m_list_lock) {
        m_list_lock->lock();
    }
    AsyncRPCConnection* conn;
    if (!FindAndAddConnection(uri, &conn)) {
        DSTREAM_WARN("aysnc call %s %s fail,", uri, method);
        res = false;
    } else {
        if (conn->m_timeout < timeout) {
            conn->m_timeout = timeout;
        }
        conn->m_requests.push_back(request);
        conn->m_handlers.push_back(handler);
        m_add_new_call = true;
    }
    if (NULL != m_list_lock) {
        m_list_lock->unlock();
    }
    DSTREAM_DEBUG("rpc call %s method %s", uri, method);
    return res;
}

bool AsyncRPCClient::FindAndAddConnection(const char* uri, AsyncRPCConnection** conn_ptr) {
    AsyncRPCConnection* async_conn = NULL;
    UriConnMapIter find = m_uri_conn_map.find(uri);
    if (find == m_uri_conn_map.end()) {
        async_conn = new AsyncRPCConnection;
        if (NULL == async_conn) {
            DSTREAM_WARN("new async connection fail");
            goto Fail;
        }
        if (NULL == (async_conn->m_conn = new Connection)) {
            DSTREAM_WARN("new connection fail");
            goto Fail;
        }
        if (async_conn->m_conn->Connect(uri) < 0) {
            DSTREAM_WARN("connect to server %s error", uri);
            goto Fail;
        }
        InsertRes insert_res = m_uri_conn_map.insert(UriConnPair(uri, async_conn));
        if (false == insert_res.second) {
            DSTREAM_WARN("insert connection to map fail");
            goto Fail;
        }
    } else {
        async_conn = find->second;
        if (NULL == async_conn) {
            m_uri_conn_map.erase(find);
            return FindAndAddConnection(uri, conn_ptr);
        }
    }
    *conn_ptr = async_conn;
    return true;
Fail:
    if (NULL != async_conn) {
        delete async_conn;
    }
    return false;
}

bool AsyncRPCClient::RemoveConnection(const std::string& uri) {
    UriConnMapIter find = m_uri_conn_map.find(uri);
    if (find != m_uri_conn_map.end()) {
        delete find->second;
        m_uri_conn_map.erase(find);
    }
    return true;
}

void AsyncRPCClient::wait() {
    if (NULL != m_list_lock) {
        m_list_lock->lock();
    }
    if (!m_add_new_call) {
        if (NULL != m_list_lock) {
            m_list_lock->unlock();
        }
        return;
    }
    UriConnMap new_map = m_uri_conn_map;
    m_add_new_call = false;
    if (NULL != m_list_lock) {
        m_list_lock->unlock();
    }
    UriConnMapIter end = new_map.end();
    std::set<std::string> fail_uri;
    std::string send_error = "send request error";
    std::string recv_error = "receive response error";
    for (UriConnMapIter iter = new_map.begin(); iter != end; ++iter) {
        AsyncRPCConnection* conn = iter->second;
        if (conn->m_requests.size() > 0) {
            int res = 0;
            if ((res = conn->m_conn->SendVector(conn->m_requests)) < 0) {
                fail_uri.insert(iter->first);
                DSTREAM_WARN("send request vector to %s fail", iter->first.c_str());
                size_t request_num = conn->m_requests.size();
                for (size_t i = 0; i < request_num; ++i) {
                    conn->m_handlers[i]->AsyncHandle(-1, conn->m_requests[i], NULL, send_error);
                }
                conn->ClearRequest();
            }
        }
    }
    timeval current, now;
    gettimeofday(&current, NULL);
    int current_mill_sec = current.tv_usec;
    end = new_map.end();
    for (UriConnMapIter iter = new_map.begin(); iter != end; ++iter) {
        AsyncRPCConnection* conn = iter->second;
        size_t req_num = conn->m_requests.size();
        if (req_num > 0) {
            std::vector<Response*> response(req_num);
            bool create_response_res = true;
            for (size_t i = 0; i < req_num; ++i) {
                if (!create_response_res) {
                    conn->m_handlers[i]->AsyncHandle(-1, conn->m_requests[i], NULL, recv_error);
                    continue;
                }
                response[i] = conn->m_handlers[i]->CreateResponse();
                if (NULL == response[i]) {
                    create_response_res = false;
                    for (size_t j = 0; j <= i; ++i) {
                        conn->m_handlers[j]->AsyncHandle(-1, conn->m_requests[i], NULL, recv_error);
                        if (j < i) {
                            delete response[j];
                        }
                    }
                }
            }
            if (!create_response_res) {
                DSTREAM_WARN("create response faile");
                fail_uri.insert(iter->first);
                conn->ClearRequest();
                continue;
            }
            int res = 0;
            gettimeofday(&now, NULL);
            int timeout = conn->m_timeout - ((now.tv_sec - current.tv_sec) * 1000
                                             + now.tv_usec / 1000 - current_mill_sec / 1000);
            DSTREAM_DEBUG("receive reponse timeout is %d", timeout);
            if ((res = conn->m_conn->RecvVector(response, timeout > 0 ? timeout : 0)) < 0) {
                DSTREAM_WARN("receive %s response fail", iter->first.c_str());
                fail_uri.insert(iter->first);
            }
            for (size_t i = 0; i < req_num; ++i) {
                conn->m_handlers[i]->AsyncHandle(res, conn->m_requests[i],
                                                 response[i], response[i]->return_value());
            }
            conn->ClearRequest();
        }
    }
    if (fail_uri.size() > 0) {
        std::set<std::string>::iterator uri_end = fail_uri.end();
        for (std::set<std::string>::iterator iter = fail_uri.begin(); iter != uri_end; ++iter) {
            RemoveConnection(*iter);
        }
    }
}

} // namespace rpc_client
} // namespace dstream
