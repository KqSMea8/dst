/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief rpc client
 */

#ifndef __DSTREAM_CC_COMMON_RPC_CLIENT_H__ // NOLINT
#define __DSTREAM_CC_COMMON_RPC_CLIENT_H__ // NOLINT

#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "common/error.h"
#include "common/logger.h"
#include "common/mutex.h"
#include "common/proto_rpc.h"
#include "common/rpc.h"

typedef std::vector<const dstream::rpc::Request*> RequestVec;
typedef RequestVec::iterator RequestVecIter;

namespace dstream {
namespace rpc_client {

class AsyncRPCClient;

/************************************************************************/
/* synchronize rpc call, REQUEST and RESPONSE is protobuffer type       */
/************************************************************************/
template <class REQUEST, class RESPONSE>
int rpc_call(const std::string& uri, const char* method,
             const REQUEST& request, RESPONSE& response,
             std::string* res = NULL, int timeout = 1000) {
    if (0 == method || timeout <= 0) {
        DSTREAM_WARN("rpc_call argument error");
        return -1;
    }
    rpc::Connection conn;
    int result = 0;
    if ((result = conn.Connect(uri.c_str())) < 0) {
        DSTREAM_WARN("connect to server %s fail", uri.c_str());
        return result;
    }
    proto_rpc::RPCRequest<REQUEST> rpc_request;
    rpc_request.CopyFrom(request);
    rpc_request.set_method(method);
    proto_rpc::RPCResponse<RESPONSE> rpc_response;
    result = conn.Call(&rpc_request, &rpc_response, timeout);
    if (result != 0) {
        DSTREAM_WARN("call %s method %s fail", uri.c_str(), method);
    } else {
        if (NULL != res) {
            *res = rpc_response.return_value();
        }
        response.CopyFrom(rpc_response);
        DSTREAM_INFO("call %s method %s success", uri.c_str(), method);
    }
    return result;
}

/************************************************************************/
/* Async RPC Request Hanlder, handle the rpc response                   */
/************************************************************************/
class AsyncRPCHandler {
public:
    virtual ~AsyncRPCHandler() {}
    virtual rpc::Response* CreateResponse() = 0;
    virtual void AsyncHandle(int call_res, const rpc::Request* request,
                             const rpc::Response* response, const std::string& return_val) = 0;
    virtual void ReleaseHandler(const rpc::Request* request, const rpc::Response* response) = 0;
};

/************************************************************************/
/* Asyn RPC Hanlder for protobuffer type                                */
/************************************************************************/
template <class T>
class ProtoAsyncRPCHandler : public AsyncRPCHandler {
public:
    virtual ~ProtoAsyncRPCHandler() {}
    rpc::Response* CreateResponse();
    virtual void AsyncHandle(int call_res, const rpc::Request* request,
                             const rpc::Response* response, const std::string& return_val);
    virtual void HandleResponse(int call_res, const rpc::Request* /*request*/,
                                const T* response, const std::string& return_val) {
        HandleResponse(call_res, response, return_val);
    }
    virtual void HandleResponse(int call_res, const T* response, const std::string& return_val) = 0;
    virtual void ReleaseHandler(const rpc::Request* request, const rpc::Response* response);
};

template <class T>
rpc::Response* ProtoAsyncRPCHandler<T>::CreateResponse() {
    return new proto_rpc::RPCResponse<T>();
}

template <class T>
void ProtoAsyncRPCHandler<T>::AsyncHandle(int call_res, const rpc::Request* request,
                                          const rpc::Response* response,
                                          const std::string& return_val) {
    const proto_rpc::RPCResponse<T>* rpc_response = dynamic_cast<const proto_rpc::RPCResponse<T>* >
                                                    (response); // NOLINT
    HandleResponse(call_res, request, rpc_response, return_val);
    ReleaseHandler(request, response);
}

template <class T>
void ProtoAsyncRPCHandler<T>::ReleaseHandler(const rpc::Request* request,
                                             const rpc::Response* response) {
    // DSTREAM_DEBUG("release handler");
    if (NULL != request) {
        delete request;
    }
    if (NULL != response) {
        delete response;
    }
    delete this;
}

typedef std::vector<AsyncRPCHandler*> HandlerVec;
typedef HandlerVec::iterator HandlerVecIter;
/*****************************************************************************/
/* AsyncRPC Connection : manage all the request and response for a connection*/
/*****************************************************************************/
class AsyncRPCConnection {
public:
    friend class AsyncRPCClient;
    AsyncRPCConnection() : m_conn(NULL), m_timeout(0) {}
    ~AsyncRPCConnection();

private:
    void ClearRequest();
    rpc::Connection* m_conn;
    int m_timeout;
    RequestVec m_requests;
    HandlerVec m_handlers;
};

typedef std::map<std::string, AsyncRPCConnection*> UriConnMap;
typedef UriConnMap::iterator UriConnMapIter;
typedef std::pair<std::string, AsyncRPCConnection*> UriConnPair;
typedef std::pair<UriConnMapIter, bool> InsertRes;

/************************************************************************/
/* Async RPCClient : send many request one time and wait for response   */
/* Due to it's complex usage, it should not used unless for efficiency  */
/************************************************************************/
class AsyncRPCClient {
public:
    explicit AsyncRPCClient(int thread_num = 0);
    ~AsyncRPCClient();

    /************************************************************************/
    /*if AsyncRPCClient works in multi thread model
     *it should be call Initialize first                                    */
    /************************************************************************/
    bool Initialize();

    /************************************************************************/
    /*Async method call                                                     */
    /*the request/handler must be a pointer that can be released by RPCClient*/
    /*if the request/handler is an object or released by user
     *the behavior is undefined and may be lead system to down              */
    /************************************************************************/
    bool call(const char* uri, const char* method, rpc::Request* request,
              AsyncRPCHandler* handler, int timeout = 5000);

    /************************************************************************/
    /*wait for all async rpc request                                        */
    /************************************************************************/
    void wait();

private:
    bool FindAndAddConnection(const char* uri, AsyncRPCConnection** conn_ptr);
    bool RemoveConnection(const std::string& uri);
    int m_thread_num;
    MutexLock* m_list_lock;
    bool m_add_new_call;
    UriConnMap m_uri_conn_map;
};

} // namespace rpc_client
} // namespace dstream

#endif // NOLINT
