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

#include <signal.h>
#include <string.h>
#include "common/error.h"
#include "common/logger.h"
#include "common/rpc.h"
#include "common/serializer.h"
#include "common/utils.h"

#define ZSERRNO (zmq_strerror(errno))
#define ZSERRNO2(n) (zmq_strerror(n))

namespace dstream {
namespace rpc {

int Connection::kserverPollTimeoutMillSeconds = 500;

static bool SmartCheckUri(const char* uri) {
    std::string u(uri);
    if (u.find("tcp://") != 0) { // if not tcp protocol we don't check.
        return true;
    }
    u = u.substr(strlen("tcp://"));
    // if all digits and '.', then we think it's a ip.
    bool ip = true;
    int dotcount = 0;
    int i = 0;
    for (i = 0; u[i] && u[i] != ':'; i++) {
        if (isdigit(u[i]) || u[i] == '.') {
            if (u[i] == '.') {
                dotcount++;
            }
            continue;
        }
        ip = false;
    }
    if (u[i] != ':') { // invalid.should follow a port.
        return false;
    }
    if (ip) {
        return dotcount == 3; // check '.' == 3
    }
    // otherwise we think it's a hostname.we don't check hostname.
    return true;
}

// zmq_msg_t operation is very brittle, especially zmq_msg_init_data
// is easy to confuse user and cause memory leak or double free.
// and I just try it add as many comments as I can when I operate zmq_msg_t.

// ------------------------------------------------------------
// Context Implementation
Context* Context::m_instance = NULL;

static void ignore_sigpipe(int signo) {
    if (signo == SIGPIPE) {
        // this may lead to mem alloc ?
        // DSTREAM_DEBUG("sigpipe arrived");
    }
}

void Context::init(int io_threads) {
    if (m_instance == NULL) {
        m_instance = new Context(io_threads);
        // always remember to install SIGPIPE handler.
        if (signal(SIGPIPE, ignore_sigpipe) == SIG_ERR) {
            DSTREAM_FATAL("signal(SIGPIPE) failed(%s)", strerror(errno));
        }
    }
}

Context* Context::instance() {
    if (m_instance == NULL) {
        m_instance = new Context(kDefaultIOThreads);
    }
    return m_instance;
}

Context::Context(int io_threads) {
    m_connection_number = 0;
    m_zmq_context = zmq_init(io_threads);
    if (m_zmq_context == NULL) {
        DSTREAM_FATAL("zmq_init(%d) failed(%s)", io_threads, ZSERRNO);
    }
}

Context::~Context() {
again:
    int ret = zmq_term(m_zmq_context);
    if (ret < 0) {
        if (ret == EINTR) {
            goto again;
        }
        DSTREAM_WARN("zmq_term(%p) failed(%s)", m_zmq_context, ZSERRNO);
    }
}

// ------------------------------------------------------------
// RPC Request and Response Implementation
Request::Request(): m_method("have you set method???") {
}

const std::string& Request::method() const {
    return m_method;
}

void Request::set_method(const std::string& method_fun) {
    m_method = method_fun;
}

Response::Response(): m_return_value("OK") {
}

const std::string& Response::return_value() const {
    return m_return_value;
}

void Response::set_return_value(const std::string& return_value) {
    m_return_value = return_value;
}

// ------------------------------------------------------------
// wrapper of zmq
static int _zmq_recv(void* socket, zmq_msg_t* msg, int flags) {
again:
    int ret = zmq_recv(socket, msg, flags);
    if (ret < 0) {
        if (errno == EINTR) {
            goto again;
        }
        DSTREAM_WARN("zmq_recv(%p,%p,%d) failed(%s)", socket, &msg, flags, ZSERRNO);
    }
    return ret;
}

static int _zmq_send(void* socket, zmq_msg_t* msg, int flags) {
again:
    int ret = zmq_send(socket, msg, flags);
    if (ret < 0) {
        if (errno == EINTR) {
            goto again;
        }
        DSTREAM_WARN("zmq_send(%p,%p,%d) failed(%s)", socket, &msg, flags, ZSERRNO);
    }
    return ret;
}

static int _zmq_poll(zmq_pollitem_t* items, int nitems, int timeout_ms) {
again:
    int ret = zmq_poll(items, nitems, timeout_ms * 1000); // timeout unit is microseconds.
    if (ret < 0) {
        if (errno == EINTR) {
            goto again;
        }
        DSTREAM_WARN("zmq_poll(%p,%d,%dms) failed(%s)", items, nitems, timeout_ms, ZSERRNO);
        goto again;
    }
    return ret;
}

static int _zmq_getsockopt(void* socket, int option_name, void* option_value, size_t* option_len) {
again:
    int ret = zmq_getsockopt(socket, option_name, option_value, option_len);
    if (ret < 0) {
        if (errno == EINTR) {
            goto again;
        }
        DSTREAM_WARN("zmq_getsockopt(%p,%d,%p,%p) faild(%s)", socket, option_name, option_value,
                        option_len, ZSERRNO);
    }
    return ret;
}

// copy the 'data/size' to 'msg'
static int _zmq_put_msg(zmq_msg_t* msg, const void* data, size_t size) {
    zmq_msg_close(msg); // discard old content.
    int ret = zmq_msg_init_size(msg, size);
    if (ret < 0) {
        DSTREAM_WARN("zmq_msg_init_size(%zu) failed(%s)", size, ZSERRNO);
    } else {
        memcpy(zmq_msg_data(msg), data, size);
    }
    return ret;
}

void internal_free(void* data, void* /*hint*/) {
    // just free it.
    free(data);
}

// put ptr of 'data/size' to 'msg',
// and when unneed call the 'internal_free' to deallocate 'data/size'
static int _zmq_put_msg_ptr_internal_free(zmq_msg_t* msg, void* data, size_t size) {
    zmq_msg_close(msg); // discard old content.
    // always OK.
    zmq_msg_init_data(msg, data, size, internal_free, NULL);
    return 0;
}

static void* _zmq_socket(void* context, int type) {
    void* socket = zmq_socket(context, type);
    if (socket == NULL) {
        DSTREAM_WARN("zmq_socket(%p,%d) failed(%s)", context, type, ZSERRNO);
    }
    return socket;
}

static int _zmq_connect(void* socket, const char* uri) {
    if (!SmartCheckUri(uri)) {
        DSTREAM_WARN("uri(%s) invalid", uri);
        return -1;
    }
    int ret = zmq_connect(socket, uri);
    if (ret < 0) {
        DSTREAM_WARN("zmq_connect(%p,%s) failed(%s)", socket, uri, ZSERRNO);
    }
    int linger_time = 0;
    ret = zmq_setsockopt(socket, ZMQ_LINGER, &linger_time, sizeof(linger_time));
    if (ret < 0) {
        DSTREAM_WARN("zmq_setsockopt(%p,%s) failed(%s)", socket, uri, ZSERRNO);
    }
    return ret;
}

static int _zmq_bind(void* socket, const char* uri) {
    if (!SmartCheckUri(uri)) {
        DSTREAM_WARN("uri(%s) invalid", uri);
        return -1;
    }
    int ret = zmq_bind(socket, uri);
    if (ret < 0) {
        DSTREAM_WARN("zmq_bind(%p,%s) failed(%s)", socket, uri, ZSERRNO);
    }
    return ret;
}

static int _zmq_close(void* socket) {
    int ret = zmq_close(socket);
    if (ret < 0) {
        DSTREAM_WARN("zmq_close(%p) faild(%s)", socket, ZSERRNO);
    }
    return ret;
}

// ------------------------------------------------------------
// wrapper of serialization and deserilization
static bool serialize_request(const Request* request, WriteableByteArray* bytes) {
    // put method at first.
    if (!serializer::Serialize(request->method(), bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("serialize method(%s)", request->method().c_str());
    // put object then.
    if (!request->Serialize(bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("serialize request OK");
    return true;
}

static bool deserialize_method(std::string* method, ReadableByteArray* bytes) {
    // fetch method at first.
    if (!serializer::Deserialize(method, bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("deserialize method(%s) OK", method->c_str());
    return true;
}

static bool serialize_response(const Response* response, WriteableByteArray* bytes) {
    // put return value at first.
    if (!serializer::Serialize(response->return_value(), bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("serialize return value(%s)", response->return_value().c_str());
    // put object then.
    if (!response->Serialize(bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("serialize response OK");
    return true;
}

static bool deserialize_response(Response* response, ReadableByteArray* bytes) {
    std::string return_value;
    // fetch return value at first.
    if (!serializer::Deserialize(&return_value, bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("deserialize return value(%s)", return_value.c_str());
    // get object then.
    if (!response->Deserialize(bytes)) {
        return false;
    }
    // DSTREAM_DEBUG("deserialize response OK");
    response->set_return_value(return_value.c_str());
    return true;
}

// ------------------------------------------------------------
bool Connection::GenDefaultResponse(WriteableByteArray* out_bytes) {
    // reset out_bytes
    size_t buf_size = 0, response_num = 0;
    dstream::Byte* buf = const_cast<dstream::Byte*>(out_bytes->Data(&buf_size));
    assert(buf_size >= sizeof(response_num));
    memcpy(buf, &response_num, sizeof(response_num));
    return true;
}

// thread and dispatch function.
int Connection::rpc_dispatch_function(ReadableByteArray* in_bytes,
                                      WriteableByteArray* out_bytes, pid_t t_id) {
    int ret = -1;
    Request* request = NULL;
    Response* response = NULL;
    Service* service = NULL;
    std::string method;

    // maybe there is much more request.
    // support not only one request.
    uint64_t number = 0;
    if (!serializer::Deserialize(&number, in_bytes)) {
        DSTREAM_WARN("t:%d [rpc server] deserialize(%p,%p) failed", t_id, &number, in_bytes);
        goto fail;
    }
    if (!serializer::Serialize(number, out_bytes)) {
        DSTREAM_WARN("t:%d [rpc server] serialize(%zu,%p) failed",
                        t_id, static_cast<size_t>(number), out_bytes);
        goto fail;
    }
    for (uint64_t i = 0; i < number; i++) {
        // deserialize method name
        if (!deserialize_method(&method, in_bytes)) {
            DSTREAM_WARN("t:%d [rpc server] deserialize_method(%p,%p) failed",
                            t_id, &method, in_bytes);
            continue;
        }

        Connection::Dispatcher::const_iterator it = m_dispatcher.find(method);
        if (it == m_dispatcher.end()) {
            DSTREAM_WARN("t:%d [rpc server] method(%s) not found", t_id, method.c_str());
            goto fail;
        }

        DSTREAM_DEBUG("dispatch function with method(%s)", method.c_str());
        service = it->second;

        request = service->AllocateRequest();
        if (NULL == request || !request->Deserialize(in_bytes)) {
            DSTREAM_WARN("t:%d [rpc server] request deserialize(%p) failed", t_id, in_bytes);
            goto fail;
        }

        response = service->AllocateResponse();

        // AtomicInc64(running_services_);
        // DSTREAM_DEBUG("thread:[%u] [rpc server] %d running, service->run(%s)",
        //                t_id, (int)running_services_, method.c_str());
        service->Run(request, response);
        // DSTREAM_DEBUG("thread:[%u] [rpc server] handle %s done", t_id, method.c_str());
        // AtomicDec64(running_services_);

        if (!serialize_response(response, out_bytes)) {
            DSTREAM_WARN("t:%d [rpc server] response serialize(%p,%p) failed",
                            t_id, response, out_bytes);
            goto fail;
        }

        // deallocate request and response.
        if (request) {
            service->DeallocateRequest(request);
            request = NULL;
        }
        if (response) {
            service->DeallocateResponse(response);
            response = NULL;
        }
    }
    ret = 0;
fail:
    // check again.
    if (request) {
        service->DeallocateRequest(request);
        request = NULL;
    }
    if (response) {
        service->DeallocateResponse(response);
        response = NULL;
    }
    return ret;
}

static void* proxy_rpc_thread_function(void* arg) {
    Connection* conn = static_cast<Connection*>(arg);
    void* ret = conn->rpc_thread_function();
    DSTREAM_INFO(" rpc server thread exit.");
    return ret;
}

void* Connection::rpc_thread_function() {
    void* receiver = NULL;
    int ret = 0;
    // int t_id = 0;
    pid_t t_id = CurrentThread::tid();
    zmq_pollitem_t items[1];
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    // lock_.lock();
    // t_id = (int)(t_id_++);
    // lock_.unlock();

    receiver = _zmq_socket(Context::instance()->context(), ZMQ_REP);
    if (receiver == NULL) {
        goto fail;
    }
    if (_zmq_connect(receiver, m_inproc_uri) < 0) {
        goto fail;
    }
    DSTREAM_DEBUG("rpc thread function connect to '%s' OK", m_inproc_uri);

    items[0].socket = receiver;
    items[0].fd = -1;
    items[0].events = ZMQ_POLLIN;

    while (1) {
        // check connection is break.
        if (m_stop) {
            break;
        }
        ret = _zmq_poll(items, 1, Connection::kserverPollTimeoutMillSeconds);
        if (ret < 0) {
            DSTREAM_ERROR("thread:[%u] [rpc server error] zmq poll error!", t_id);
            continue;
        } else if (ret == 0) { // timeout.
            continue;
        }

        if (_zmq_recv(receiver, &msg, 0) < 0) {
            DSTREAM_ERROR("thread:[%u] [rpc server error] zmq recv error!", t_id);
            // goto fail;
            continue;
        }

        // handle message
        ReadableByteArray recv_bytes(static_cast<const dstream::Byte*>(zmq_msg_data(&msg)),
                                     zmq_msg_size(&msg));
        WriteableByteArray send_bytes(true); // user free.
        // DSTREAM_DEBUG("server recv data size:%zu", zmq_msg_size(&msg));
        if (rpc_dispatch_function(&recv_bytes, &send_bytes, t_id) < 0) {
            DSTREAM_ERROR("thread:[%u] [rpc server error] dispatch error!", t_id);
            GenDefaultResponse(&send_bytes);
        }

        // reply.
        ByteSize data_size;
        const dstream::Byte* data = send_bytes.Data(&data_size);
        // DSTREAM_DEBUG("server send data size:%zu", data_size);
        _zmq_put_msg_ptr_internal_free(&msg, const_cast<dstream::Byte*>(data), data_size);
        if (_zmq_send(receiver, &msg, 0) < 0) {
            zmq_msg_close(&msg); // like sendPtrInternalFree.
            DSTREAM_ERROR("rpc thread:[%u] [rpc server error] zmq send error!", t_id);
            // goto fail;
            continue; // modify by lanbijia@baidu.com
        }
        // we have to init right here.
        // otherwise 'recv' or we call 'close'
        // will cause double free. be CAUTIOUS!!!.
        zmq_msg_init(&msg);
    } // while(1)

fail:
    zmq_msg_close(&msg); //
    if (receiver) {
        zmq_close(receiver);
        receiver = NULL;
    }
    return NULL;
}

// ------------------------------------------------------------
// Connection Implementation
Connection::Connection():
    m_zmq_main_socket(NULL), m_zmq_back_socket(NULL),
    m_connection_number(0), m_stop(false), m_working(false), m_t_id(0), m_running_services(0) {
}

Connection::~Connection() {
    Close();
}

int Connection::Serve(const char* serv_uri, int callback_threads) {
    int ret = 0;
    pthread_t tids[callback_threads]; // NOLINT
    int success_thrnum = 0;
    zmq_pollitem_t items[2];
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    int64_t more;
    size_t moresz = sizeof(more);
    m_uri = serv_uri;

    if (callback_threads <= 0) {
        DSTREAM_FATAL("callback_threads(%d)<=0", callback_threads);
    }
    m_connection_number = Context::instance()->IncreaseConnectionNumber();

    // bind the main socket to recv request from client
    m_zmq_main_socket = _zmq_socket(Context::instance()->context(), ZMQ_ROUTER);
    if (m_zmq_main_socket == NULL) {
        goto fail;
    }
    if (_zmq_bind(m_zmq_main_socket, serv_uri) < 0) {
        goto fail;
    }
    DSTREAM_INFO("bind main socket to [%s] OK", serv_uri);

    // bind the backend socket to handle the request
    m_zmq_back_socket = _zmq_socket(Context::instance()->context(), ZMQ_DEALER);
    if (m_zmq_back_socket == NULL) {
        goto fail;
    }
    snprintf(m_inproc_uri, sizeof(m_inproc_uri), "inproc://conn#%d", m_connection_number);
    if (_zmq_bind(m_zmq_back_socket, m_inproc_uri) < 0) {
        goto fail;
    }
    DSTREAM_DEBUG("bind back socket to '%s' OK", m_inproc_uri);

    // allocate threads.
    for (success_thrnum = 0; success_thrnum < callback_threads; success_thrnum++) {
        ret = pthread_create(tids + success_thrnum, NULL, proxy_rpc_thread_function, this);
        if (ret < 0) {
            DSTREAM_WARN("pthread_create(%d) failed(%s)", success_thrnum, ZSERRNO2(ret));
            m_stop = true; // set stop flag, so the thread function can exit.
            goto fail;
        }
    }
    // DSTREAM_DEBUG("allocate threads OK");

    // epoll main socket and back socket. also detect stop flag.
    // works as zmq device.
    items[0].socket = m_zmq_main_socket;
    items[0].fd = -1;
    items[0].events = ZMQ_POLLIN;
    items[1].socket = m_zmq_back_socket;
    items[1].fd = -1;
    items[1].events = ZMQ_POLLIN;
    m_working = true;
    while (1) {
        // check connection is break.
        if (m_stop) {
            break;
        }
        ret = _zmq_poll(items, 2, kserverPollTimeoutMillSeconds);
        if (ret < 0) {
            goto fail;
        } else if (ret == 0) {
            continue;
        }
        // DSTREAM_DEBUG("serve poll triggered");

        if (items[0].revents & ZMQ_POLLIN) {
            while (1) {
                if (_zmq_recv(items[0].socket, &msg, 0) < 0) {
                    goto fail;
                }
                if (_zmq_getsockopt(items[0].socket, ZMQ_RCVMORE, &more, &moresz) < 0) {
                    goto fail;
                }
                if (_zmq_send(items[1].socket, &msg, more ? ZMQ_SNDMORE : 0) < 0) {
                    goto fail;
                }
                if (!more) {
                    break;
                }
            }
        }

        if (items[1].revents & ZMQ_POLLIN) {
            while (1) {
                if (_zmq_recv(items[1].socket, &msg, 0) < 0) {
                    goto fail;
                }
                if (_zmq_getsockopt(items[1].socket, ZMQ_RCVMORE, &more, &moresz) < 0) {
                    goto fail;
                }
                if (_zmq_send(items[0].socket, &msg, more ? ZMQ_SNDMORE : 0) < 0) {
                    goto fail;
                }
                if (!more) {
                    break;
                }
            }
        }
    } // while(1)
fail:
    zmq_msg_close(&msg);
    for (int i = 0; i < success_thrnum; i++) {
        // ignore the return value
        pthread_join(tids[i], NULL);
    }
    Close();
    return -1;
}


int Connection::Connect(const char* conn_uri) {
    {
        MutexLockGuard lock_guard(m_lock);
        if (m_zmq_main_socket != NULL) {
            return 0;
        }
        m_uri = conn_uri;
        m_connection_number = Context::instance()->IncreaseConnectionNumber();
        // bind the main socket to send request
        m_zmq_main_socket = _zmq_socket(Context::instance()->context(), ZMQ_REQ);
        if (m_zmq_main_socket == NULL) {
            goto fail;
        }
        if (_zmq_connect(m_zmq_main_socket, conn_uri) < 0) {
            goto fail;
        }
        return 0;
fail:
        CloseWithoutLock();
    }
    return -1;
}

int Connection::Reconnect() {
    Close();
    return Connect(m_uri.c_str());
}

void Connection::Close() {
    MutexLockGuard lock_guard(m_lock);
    if (m_zmq_main_socket) {
        _zmq_close(m_zmq_main_socket);
        m_zmq_main_socket = NULL;
    }
    if (m_zmq_back_socket) {
        _zmq_close(m_zmq_back_socket);
        m_zmq_back_socket = NULL;
    }
    m_working = false;
}

int Connection::SendPtrInternalFree(void* data, size_t size) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    // always OK. so we can close anyway.
    _zmq_put_msg_ptr_internal_free(&msg, data, size);
    // DSTREAM_DEBUG("client send data size:%zu", size);
    // since user don't know when to free the data.
    // so if we fail right here, we have to help user to free it.
    // otherwise user are in the fucking embrassment.
    if (_zmq_send(m_zmq_main_socket, &msg, 0) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }
    return 0;
}

// ----------------------------------------
// Message Implementation
Connection::Message::Message() {
    zmq_msg_init(&msg_);
}

Connection::Message::~Message() {
    free();
}

void Connection::Message::free() {
    zmq_msg_close(&msg_);
}

void* Connection::Message::data(size_t* size) {
    if (size) {
        *size = zmq_msg_size(&msg_);
    }
    return zmq_msg_data(&msg_);
}

int Connection::RecvMessage(Message* message, int timeout_ms) {
    return RecvInternalMessage(&(message->msg_), timeout_ms);
}

int Connection::RecvInternalMessage(zmq_msg_t* msg, int timeout_ms) {
    zmq_pollitem_t items[1];
    items[0].socket = m_zmq_main_socket;
    items[0].fd = -1;
    items[0].events = ZMQ_POLLIN;

    int ret = _zmq_poll(items, 1, timeout_ms);
    if (ret < 0) {
        return -1;
    } else if (ret == 0) {
        DSTREAM_WARN("timeout(%dms)", timeout_ms);
        return -1;
    }
    if (_zmq_recv(m_zmq_main_socket, msg, 0) < 0) {
        return -1;
    }
    return 0;
}

int Connection::Send(ReadableByteArray* bytes) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    ByteSize size = 0;
    const dstream::Byte* data = bytes->Data(&size);
    // DSTREAM_DEBUG("client send data size:%zu", size);
    if (_zmq_put_msg(&msg, data, size) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }
    if (_zmq_send(m_zmq_main_socket, &msg, 0) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }
    zmq_msg_close(&msg);
    return 0;
}

int Connection::Recv(WriteableByteArray* bytes, int timeout_ms) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    if (RecvInternalMessage(&msg, timeout_ms) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }
    const void* data = zmq_msg_data(&msg);
    size_t size = zmq_msg_size(&msg);
    // DSTREAM_DEBUG("client recv data size:%zu", size);
    dstream::Byte* dst = bytes->Allocate(size);
    if (dst == NULL) {
        DSTREAM_WARN("allocate(%zu) failed", size);
        zmq_msg_close(&msg);
        return -1;
    }
    memcpy(dst, data, size);
    zmq_msg_close(&msg);
    return 0;
}


int Connection::Send(const Request* request) {
    WriteableByteArray send_bytes(true);

    // put object.
    // since there may be a lot of request put together.
    // here we declare that we want send 1 request.
    if (!serializer::Serialize(static_cast<uint64_t>(1), &send_bytes)) {
        DSTREAM_WARN("serialize(1,%p) failed", &send_bytes);
        return -1;
    }
    if (!serialize_request(request, &send_bytes)) {
        DSTREAM_WARN("serialize_request(%p,%p) failed", request, &send_bytes);
        return -1;
    }

    // send once.
    ByteSize size;
    const dstream::Byte* data = send_bytes.Data(&size);
    ReadableByteArray _send_bytes(data, size);
    if (SendPtrInternalFree(const_cast<dstream::Byte*>(data), size) < 0) {
        return -1;
    }
    return 0;
}

int Connection::SendVector(const std::vector< const Request* >& request_vector) {
    WriteableByteArray send_bytes(true);

    // put object.
    // put the number of request at first.
    uint64_t number = request_vector.size();
    if (!serializer::Serialize(number, &send_bytes)) {
        DSTREAM_WARN("serialize(%zu,%p) failed", static_cast<size_t>(number), &send_bytes);
        return -1;
    }
    for (size_t i = 0; i < number; i++) {
        if (!serialize_request(request_vector[i], &send_bytes)) {
            DSTREAM_WARN("serialize_request(%p,%p),index(%zu) failed",
                            request_vector[i], &send_bytes, i);
            return -1;
        }
    }

    // send once.
    ByteSize size;
    const dstream::Byte* data = send_bytes.Data(&size);
    ReadableByteArray _send_bytes(data, size);
    if (SendPtrInternalFree(const_cast<dstream::Byte*>(data), size) < 0) {
        return -1;
    }
    return 0;
}

int Connection::Recv(Response* response, int timeout_ms) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    if (RecvInternalMessage(&msg, timeout_ms) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }

    // fetch object.
    ReadableByteArray _recv_bytes(reinterpret_cast<const dstream::Byte*>(zmq_msg_data(&msg)),
                                  zmq_msg_size(&msg));
    // recv number of response at first.
    uint64_t number = 0;
    if (!serializer::Deserialize(&number, &_recv_bytes)) {
        DSTREAM_WARN("deserialize(%p,%p) failed", &number, &_recv_bytes);
        zmq_msg_close(&msg);
        return -1;
    }
    // DSTREAM_DEBUG("get response number %lu", number);
    // check number
    if (number != 1) {
        DSTREAM_WARN("number!=1, perhaps you should call 'recvVector'");
        zmq_msg_close(&msg);
        return -1;
    }
    if (!deserialize_response(response, &_recv_bytes)) {
        DSTREAM_WARN("deserialize_response(%p,%p) failed", response, &_recv_bytes);
        zmq_msg_close(&msg);
        return -1;
    }

    zmq_msg_close(&msg);
    return 0;
}

int Connection::RecvVector(const std::vector< Response* >& response_vector, int timeout_ms) {
    zmq_msg_t msg;
    zmq_msg_init(&msg);

    if (RecvInternalMessage(&msg, timeout_ms) < 0) {
        zmq_msg_close(&msg);
        return -1;
    }

    // fetch object.
    ReadableByteArray _recv_bytes(reinterpret_cast<const dstream::Byte*>(zmq_msg_data(&msg)),
                                  zmq_msg_size(&msg));
    // parse number of response at first.
    uint64_t number = 0;
    if (!serializer::Deserialize(&number, &_recv_bytes)) {
        DSTREAM_WARN("deserialize(%p,%p) failed", &number, &_recv_bytes);
        return -1;
    }
    for (uint64_t idx = 0; idx < number; idx++) {
        if (!deserialize_response(response_vector[idx], &_recv_bytes)) {
            DSTREAM_WARN("deserialize_response(%p,%p),index(%lu) failed",
                            response_vector[idx], &_recv_bytes, idx);
            zmq_msg_close(&msg);
            return -1;
        }
    }

    zmq_msg_close(&msg);
    return 0;
}

/**
* @brief    connect with no lock guard,
*           so be carefull to use it!!!
*
* @return   0: ok;
*           -1: fail;
* @author   konghui, konghui@baidu.com
* @date   2012-03-05
*/
int Connection::ConnectWithoutLock(const char* conn_free_uri) {
    if (m_zmq_main_socket != NULL) {
        return 0;
    }
    if (m_zmq_main_socket != NULL) {
        return 0;
    }
    m_uri = conn_free_uri;
    m_connection_number = Context::instance()->IncreaseConnectionNumber();
    // bind the main socket to send request
    m_zmq_main_socket = _zmq_socket(Context::instance()->context(), ZMQ_REQ);
    if (m_zmq_main_socket == NULL) {
        goto fail;
    }
    if (_zmq_connect(m_zmq_main_socket, conn_free_uri) < 0) {
        goto fail;
    }
    return 0;
fail:
    CloseWithoutLock();
    return -1;
}

/**
* @brief    close connect with no lock guard,
*           so be carefull to use it!!!
*
* @return   0: ok;
*           -1: fail;
* @author   konghui, konghui@baidu.com
* @date   2012-03-05
*/
void Connection::CloseWithoutLock() {
    if (m_zmq_main_socket) {
        _zmq_close(m_zmq_main_socket);
        m_zmq_main_socket = NULL;
    }
    if (m_zmq_back_socket) {
        _zmq_close(m_zmq_back_socket);
        m_zmq_back_socket = NULL;
    }
    m_working = false;
}
/**
* @brief    reconnect with no lock guard, so be carefull to use it!!!
*
* @return   0: ok;
*           -1: fail;
* @author   konghui, konghui@baidu.com
* @date   2012-03-05
*/
int Connection::ReconnectWithoutLock() {
    // close
    CloseWithoutLock();
    // connect
    return ConnectWithoutLock(m_uri.c_str());
}

int Connection::Call(const Request* request, Response* response, int timeout_ms) {
    MutexLockGuard lock_guard(m_lock);
    if (Send(request) < 0) {
        DSTREAM_ERROR("[%s]: fail to send data", __FUNCTION__);
        ReconnectWithoutLock();
        return -1;
    }
    if (Recv(response, timeout_ms) < 0) {
        DSTREAM_ERROR("[%s]: fail to recv data",
                      __FUNCTION__);
        ReconnectWithoutLock();
        return -1;
    }
    return 0;
}

int Connection::TryCall(const Request* request, Response* response, int timeout_ms) {
    int ret = m_lock.trylock();
    if (0 != ret) {
        return error::TRY_LOCK_FAIL;
    }
    if (Send(request) < 0) {
        DSTREAM_ERROR("[%s]: fail to send data", __FUNCTION__);
        ReconnectWithoutLock();
        m_lock.unlock();
        return error::RPC_SEND_FAIL;
    }
    if (Recv(response, timeout_ms) < 0) {
        DSTREAM_ERROR("[%s]: fail to recv data", __FUNCTION__);
        ReconnectWithoutLock();
        m_lock.unlock();
        return error::RPC_RECV_FAIL;
    }
    m_lock.unlock();
    return error::OK;
}

void Connection::RegisterService(const std::string& method, Service* service) {
    m_dispatcher[method] = service;
}

} // namespace rpc
} // namespace dstream

#undef ZSERRNO
#undef ZSERRNO2
