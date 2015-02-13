/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  remote procedure call
 *
 */

#ifndef __DSTREAM_CC_COMMON_RPC_H__ // NOLINT
#define __DSTREAM_CC_COMMON_RPC_H__ // NOLINT

#include <map>
#include <string>
#include <vector>
#include <zmq.h>
#include "common/byte_array.h"
#include "common/mutex.h"
#include "common/serializer.h"
#include "common/utils.h"

namespace dstream {
namespace rpc {

// ------------------------------------------------------------
// RPC Context Singleton Pattern.
// !!!singleton is thread unsafe.
// app better to call 'init' at first.
class Context {
public:
    static void init(int io_threads = kDefaultIOThreads);
    static Context* instance();
    void* context() const {
        return m_zmq_context;
    }
    int IncreaseConnectionNumber() {
        return m_connection_number++;
    }

private:
    explicit Context(int io_threads);
    ~Context();
    static const int kDefaultIOThreads = 4;
    static Context* m_instance;
    int m_connection_number;
    void* m_zmq_context;
}; // class Context

// ------------------------------------------------------------
// RPC Request and Response Interface.
class Request: public serializer::Serializable {
public:
    const std::string& method() const;
    void set_method(const std::string& method_fun);
    Request();
    // here 'virtual' is important',otherwise there is a mem leak
    virtual ~Request() {}

protected:
    std::string m_method;
};

class Response: public serializer::Serializable {
public:
    const std::string& return_value() const;
    void set_return_value(const std::string& return_value);
    Response();
    // here 'virtual' is important',otherwise there is a mem leak
    virtual ~Response() {}

protected:
    std::string m_return_value;
};

// ------------------------------------------------------------
// RPC Service. service register them.
class Service {
public:
    virtual ~Service() {}
    virtual Request* AllocateRequest() = 0;
    virtual Response* AllocateResponse() = 0;
    // default Deallocate action is just delete it.
    virtual void DeallocateRequest(Request* request) {
        delete request;
    }
    virtual void DeallocateResponse(Response* response) {
        delete response;
    }
    virtual void Run(const Request* request, Response* response) = 0;
    const std::string& service_name() const {
        return m_service_name;
    }
    void set_service_name(const std::string& service_name) {
        m_service_name = service_name;
    }
public:
    std::string m_service_name;
};

// ------------------------------------------------------------
// RPC Connection for client and server to communicate
class RefCount {
public:
    RefCount(): m_ref_count(1) {}
    ~RefCount() {}
    int64_t Acquire() {
        return AtomicInc64(m_ref_count);
    }
    int64_t Release() {
        return AtomicDec64(m_ref_count);
    }
protected:
    volatile int64_t m_ref_count;
}; // class RefCount

class Connection: public RefCount {
public:
    Connection();
    ~Connection();
    int64_t Release() {
        int n = RefCount::Release();
        if (n == 0) {
            delete this;
        }
        return n;
    }
    int Serve(const char* serv_uri, int callback_threads);
    // we don't guarantee the server already started
    // if you try to ensure that, you should guarantee on app level.
    // by call a dummy method or something else.
    int Connect(const char* conn_uri);
    int Reconnect();
    void Close();
    // bytes level operation.
    int Send(ReadableByteArray* bytes);
    int Recv(WriteableByteArray* bytes, int timeout_ms);
    // object level operation.
    int Send(const Request* request);
    int SendVector(const std::vector< const Request* >& request_vector);
    int Recv(Response* response, int timeout_ms);
    int RecvVector(const std::vector< Response* >& response_vector, int timeout_ms);
    int Call(const Request* request, Response* response, int timeout_ms);
    int TryCall(const Request* request, Response* response, int timeout_ms);
    void RegisterService(const std::string& method, Service* service);
    // book the serve and connect uri
    // so if connection is broken, user can connect again.
    // but there is a fancy interface 'reconnect' for that.
    // so in most case user don't need this method.
    const char* Uri() const {
        return m_uri.c_str();
    }
    int64_t Ref() {
        return m_ref_count;
    }
    void Stop() {
        m_stop = true;
    }
    bool working() const {
        return m_working;
    }
    void* rpc_thread_function(); // use internally.
    // ----------------------------------------
    // advanced usage. much more efficient.
    // send 'data/size' with pointer, call 'free' to deallocate it after.
    int SendPtrInternalFree(void* data, size_t size);
    class Message {
    public:
        Message();
        ~Message();
        void* data(size_t* size);
        void free();
        friend class Connection;
    private:
        zmq_msg_t msg_;
    };
    // recv 'Message' and get 'data/size', user can call 'free' to free it manually
    // or let scope call ~Message to free it automatically.
    int RecvMessage(Message* message, int timeout_ms);

private:
    // with no lock functions, add by konghui
    // @date   2012-03-05
    int ConnectWithoutLock(const char* conn_free_uri);
    int ReconnectWithoutLock();
    void CloseWithoutLock();

private:
    typedef std::map< std::string, Service* > Dispatcher;
    int rpc_dispatch_function(ReadableByteArray* in_bytes,
                              WriteableByteArray* out_bytes, pid_t t_id);
    bool GenDefaultResponse(WriteableByteArray* out_bytes);
    // recv message and store it to 'msg'
    int RecvInternalMessage(zmq_msg_t* msg, int timeous_ms);
    void* m_zmq_main_socket;
    void* m_zmq_back_socket;
    Dispatcher m_dispatcher;
    int m_connection_number;
    std::string m_uri;
    char m_inproc_uri[128];
    bool m_stop;
    bool m_working;
    MutexLock m_lock;
    int64_t m_t_id; // for debug
    int64_t m_running_services;

public:
    static int kserverPollTimeoutMillSeconds;
}; // class Connection

} // namespace rpc
} // namespace dstream

#endif // NOLINT
