/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#ifndef __DSTREAM_SRC_CC_COMMIT_LOG_H__ // NOLINT
#define __DSTREAM_SRC_CC_COMMIT_LOG_H__ // NOLINT

#include <stdint.h>

#include <string>
#include <vector>

#include "common/byte_array.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/ringbuffer.h"
#include "common/rwlock.h"
#include "common/utils.h"

// for scribe importer
#include "common/proto/gen-cpp/scribe.h"
#include <boost/shared_ptr.hpp>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/concurrency/PosixThreadFactory.h>
#include <thrift/concurrency/Mutex.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TTransportUtils.h>

// forward declaration
namespace bigpipe {
class bigpipe_async_publisher_t;
class bigpipe_async_subscriber_t;
}

namespace dstream {
// commit log item type, moved from task.cc
struct LogInfo {
    uint64_t pipelet_id;
    int64_t msg_id;
    uint64_t seq_id;
    uint64_t recv_time;
    bool is_last;
    WriteableByteArray byte_array;

    LogInfo(uint64_t pid = 0, uint64_t mid = 0, uint64_t sid = 0, bool last = true) :
        pipelet_id(pid),
        msg_id(mid),
        seq_id(sid),
        recv_time(0),
        is_last(last),
        byte_array(true) // user free!
    {}
};

// ------------------------------------------------------------
// CommitLog Interface
// methods all return 0 means success and -1 means failed.
class CommitLog {
public:
    enum Role {
        kPublisher,
        kSubscriber,
    };

    static const int kLatestSubscribePoint = -1;
    explicit CommitLog(Role role):
        m_sub_point(kLatestSubscribePoint), // -1 means start at the latest sub point
        m_role(role) {}
    virtual ~CommitLog() {}
    virtual int Open() = 0;
    virtual int Close() = 0; // !!!can be called more than once.
    virtual int Read(LogInfo* log) = 0;
    virtual int Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id) = 0;
    void set_subpoint(int64_t sub_point) {
        m_sub_point = sub_point;
    }

protected:
    int64_t m_sub_point;
    Role m_role;
}; // class CommitLog

// ------------------------------------------------------------
// Fake CommitLog.
class FakeCommitLog : public CommitLog {
public:
    static const int kMaxLogSize =  2 * 1024 * 1024;
    static const int kMinLogSize =  16;

    FakeCommitLog(Role role,
                  int  log_size = 1024)
        : CommitLog(role), m_seq_no(1), m_log_size(log_size), m_log_inteval(0) {
        if (m_log_size > kMaxLogSize || m_log_size < kMinLogSize) {
            DSTREAM_WARN("log_size[%d]Bytes is too %s into [%d]Bytes",
                         m_log_size,
                         m_log_size > kMaxLogSize ? "large, shrinks" : " short, expands",
                         m_log_size > kMaxLogSize ? kMaxLogSize : kMinLogSize);
            m_log_size = m_log_size > kMaxLogSize ? kMaxLogSize : kMinLogSize;
        }
        DSTREAM_INFO("start to gen logs with size[%d]Bytes", m_log_size);
        m_read_buffer = new char[m_log_size];
        memset(m_read_buffer, '#', m_log_size);
        m_read_buffer[m_log_size - 1] = '\0';
    }
    virtual ~FakeCommitLog() {
        delete m_read_buffer;
    };
    virtual int Open() {
        return error::OK;
    }
    virtual int Close() {
        return error::OK;
    }
    virtual int Read(LogInfo* log) {
        snprintf(m_read_buffer, m_log_size, "%d:", m_seq_no);
        log->byte_array.Write(reinterpret_cast<const Byte*>(m_read_buffer), m_log_size);
        log->msg_id = m_seq_no++;
        return error::OK;
    }
    virtual int Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id) {
        (void)(bytes);
        (void)(duplicated);
        (void)(log_id);
        return error::OK;
    }

private:
    int m_seq_no;
    int m_log_size;
    int m_log_inteval;
    char* m_read_buffer; /* support max 32k per log */
}; // class BigPipeCommitLog

// ------------------------------------------------------------
// CommitLog implemented in bigpipe Interface.
class BigPipeCommitLog : public CommitLog {
public:
    BigPipeCommitLog(Role role,
                     const std::string& pipe_name,
                     const std::string& conf_path,
                     const std::string& conf_name,
                     const std::string& m_username,
                     const std::string& token);
    virtual ~BigPipeCommitLog();
    virtual int Open();
    virtual int Read(LogInfo* log);
    virtual int Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id);
    virtual int Close();
    virtual void SetPartition(int pipelet_id);
    void AddPipelet(const SubPoint& pipelet_info) {
        m_pipelets.push_back(pipelet_info);
    }
    // generate the sleep time according the failed count
    inline uint64_t GenerateSleepTime(uint32_t failcnt);

private:
    // bigpipe conf
    std::string m_pipe_name;
    std::string m_config_path;
    std::string m_config_name;
    std::string m_user;
    std::string m_token;
    bool m_init; // whether inited.

    // for bigpipe api
    boost::shared_ptr<bigpipe::bigpipe_async_subscriber_t> m_sub_ctx;
    boost::shared_ptr<bigpipe::bigpipe_async_publisher_t> m_pub_ctx;

    // for publisher
    static const int kMaxMessageSize = (2 << 20); // 2MB
    uint32_t m_pipelet_sendto;

    // for subscriber
    std::vector<SubPoint> m_pipelets;
    char* m_read_buffer;
    uint32_t m_pipelet_last_read;
    // set the max try count to calculate the sleep time
    static const uint32_t kMaxRetrycnt = 3;
}; // class BigPipeCommitLog

// ------------------------------------------------------------
// CommitLog implemented in filesystem Interface.
// have to abstract a filesystem interface.
// supports such as POSIX interface and hdfs.
class LocalFileCommitLog : public CommitLog {
public:
    LocalFileCommitLog(Role role, const std::string& file_name);
    LocalFileCommitLog(Role role, const std::string& file_name, bool loop_read);
    virtual ~LocalFileCommitLog();
    virtual int Open();
    virtual int Read(LogInfo* log);
    virtual int Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id);
    virtual int Close();

private:
    std::string m_file_name;
    int m_fd;
    uint64_t m_offset;
    uint64_t m_file_len;
    bool m_loop_read;
}; // class LocalFileCommitLog

class ScribeHandler : public scribe::thrift::scribeIf {
public:
    ScribeHandler(int buffer_size) {
        m_ring_buffer = new (std::nothrow) common::RingBuffer(buffer_size);
        m_ring_buffer->Init();
        m_read_buffer = new (std::nothrow) char[kMaxMessageSize];
        assert(m_read_buffer != NULL);
    }
    ~ScribeHandler() {
        if (NULL != m_ring_buffer) {
            delete m_ring_buffer;
            m_ring_buffer = NULL;
        }
        if (NULL != m_read_buffer) {
            delete []m_read_buffer;
            m_read_buffer = NULL;
        }
    }

    scribe::thrift::ResultCode::type Log(const std::vector<scribe::thrift::LogEntry> & messages) {
        m_lock.acquireWrite();
        std::vector<scribe::thrift::LogEntry>::const_iterator it;
        if (0 == messages.size()) {
            if (m_ring_buffer->ReachWaterMark()) {
                DSTREAM_INFO("reach water mark, return try later");
                m_lock.release();
                return scribe::thrift::ResultCode::TRY_LATER;
            }
        }
        for (it = messages.begin();
             it != messages.end();
             ++it) {
            // discard empty message
            if (it->message.empty()) {
                continue;
            }
            int message_len = it->message.size();
            if (!m_ring_buffer->IsEnough(sizeof(message_len) + message_len)) {
                DSTREAM_INFO("Too many messages, drop here");
                m_lock.release();
                return scribe::thrift::ResultCode::TRY_LATER;
            }
            if (m_ring_buffer->Put((char*)(&message_len), sizeof(message_len)) == error::QUEUE_FULL ||
                m_ring_buffer->Put(it->message.data(), message_len) == error::QUEUE_FULL) {
                DSTREAM_INFO("put message into ringbuffer failed");
                m_lock.release();
                return scribe::thrift::ResultCode::OK;
            }
        }
        m_lock.release();
        return scribe::thrift::ResultCode::OK;
    }

    virtual void getName(std::string& /*_return*/) {}
    virtual void getVersion(std::string& /*_return*/) {}
    virtual facebook::fb303::fb_status::type getStatus() { return facebook::fb303::fb_status::ALIVE; }
    virtual void getStatusDetails(std::string& /*_return*/) {}
    virtual void getCounters(std::map<std::string, int64_t> & /*_return*/) {}
    virtual int64_t getCounter(const std::string& /*key*/) { return 0; }
    virtual void setOption(const std::string& /*key*/, const std::string& /*value*/) {}
    virtual void getOption(std::string& /*_return*/, const std::string& /*key*/) {}
    virtual void getOptions(std::map<std::string, std::string> & /*_return*/) {}
    virtual void getCpuProfile(std::string& /*_return*/, const int32_t /*profileDurationInSec*/) {}
    virtual int64_t aliveSince() { return 0; }
    virtual void reinitialize() {}
    virtual void shutdown() {}

    bool IsEmpty() {
        m_lock.acquireRead();
        bool flag = m_ring_buffer->IsEmpty();
        m_lock.release();
        return flag;
    }

    void GetBuffer(LogInfo* log) {
        if (!log) {
            return;
        }
        m_lock.acquireWrite();
        int size = 0;
        if (m_ring_buffer->Get((char*)(&size), sizeof(size)) == error::OK) {
            if (m_ring_buffer->Get((char*)(m_read_buffer), size) == error::OK) {
                log->byte_array.Write(reinterpret_cast<const dstream::Byte*>(m_read_buffer), size);
            }
        }
        m_lock.release();
        return;
    }

private:
    static const int kMaxMessageSize = (10 << 20);
    apache::thrift::concurrency::ReadWriteMutex m_lock;
    common::RingBuffer* m_ring_buffer;
    char* m_read_buffer;
};

class ServerInitNotification : public apache::thrift::server::TServerEventHandler {
public:
    ServerInitNotification() : m_init(false) {}
    virtual ~ServerInitNotification() {}

    bool IsInited() {
        m_lock.LockForRead();
        bool init = m_init;
        m_lock.Unlock();
        return init;
    }

    virtual void preServe() {
        m_lock.LockForWrite();
        m_init = true;
        m_lock.Unlock();
    }

private:
    RWLock m_lock;
    bool m_init;
};

class ScribeCommitLog : public CommitLog {
public:
    ScribeCommitLog(Role role, int buffer_size);
    virtual ~ScribeCommitLog();
    virtual int Open();
    virtual int Read(LogInfo *log);
    virtual int Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id);
    virtual int Peek(bool* eof, int timeout_ms);
    virtual int Close();
    int32_t IsInited();
    uint32_t Port() { return m_port; }
    void Serve();
    void Stop();

private:
    static const uint32_t kMaxPort = 9700;
    volatile int32_t m_init;
    uint32_t m_port;
    int m_buffer_size;
    pthread_t m_tid;
    boost::shared_ptr<ScribeHandler> m_handler;
    boost::shared_ptr<ServerInitNotification> m_notification;
    boost::shared_ptr<apache::thrift::TProcessor> m_processor;
    boost::shared_ptr<apache::thrift::protocol::TProtocolFactory> m_protocol_factory;
    boost::shared_ptr<apache::thrift::concurrency::ThreadManager> m_thread_manager;
    boost::shared_ptr<apache::thrift::concurrency::PosixThreadFactory> m_thread_factory;
    boost::shared_ptr<apache::thrift::server::TNonblockingServer> m_server;
};

} // namespace dstream

#endif // __DSTREAM_SRC_CC_COMMIT_LOG_H__ // NOLINT
