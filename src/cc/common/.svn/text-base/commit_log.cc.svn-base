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

#include "bigpipe_async_publisher.h"  //NOLINT
#include "bigpipe_async_subscriber.h" //NOLINT

#include "common/commit_log.h"
#include "common/logger.h"

#define BPSERRNO ("bigpipe error")
#define BPSERRNO2(n) ("bigpipe error no"#n)

namespace dstream {

using namespace bigpipe;
using namespace scribe::thrift;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;
using namespace apache::thrift::concurrency;

// ------------------------------------------------------------
// BigPipeCommitLog Implementation.
BigPipeCommitLog::BigPipeCommitLog(Role role,
                                   const std::string& pipe_name,
                                   const std::string& conf_path,
                                   const std::string& conf_name,
                                   const std::string& user_name,
                                   const std::string& token):
    CommitLog(role), m_pipe_name(pipe_name),
    m_config_path(conf_path), m_config_name(conf_name), m_user(user_name), m_token(token),
    m_init(false), m_pipelet_sendto(0), m_read_buffer(NULL),
    m_pipelet_last_read(0) {
}

int BigPipeCommitLog::Open() {
    if (!m_init) {
        if (m_role == CommitLog::kPublisher) {
            boost::shared_ptr<bigpipe::bigpipe_async_publisher_t>
                ctx(new (std::nothrow) bigpipe::bigpipe_async_publisher_t);
            if (!ctx) {
                DSTREAM_ERROR("allocate bigpipe pub api failed.");
                return error::FAILED_EXPECTATION;
            }

            bigpipe::bigpipe_partitioner_t partitioner;
            int ret = ctx->init2(m_pipe_name.c_str(), m_token.c_str(), &partitioner,
                                 m_config_path.c_str(), m_config_name.c_str(),
                                 NULL, /* range file */
                                 NULL, /* progress */
                                 NULL, /* persist path */
                                 BIGPIPE_API_ASYNC_PUBLISH, /* role*/
                                 m_user.c_str());
            if (ret) {
                DSTREAM_ERROR("bigpipe pub init failed, ret=%d", ret);
                return ret;
            }
            m_pub_ctx = ctx;
            m_init = true;
            return ret;
        } else if (m_role == CommitLog::kSubscriber) {
            boost::shared_ptr<bigpipe::bigpipe_async_subscriber_t>
                ctx(new (std::nothrow) bigpipe::bigpipe_async_subscriber_t);
            if (!ctx) {
                DSTREAM_ERROR("allocate bigpipe sub api failed.");
                return error::FAILED_EXPECTATION;
            }

            int ret = ctx->init_env(m_pipe_name.c_str(),
                                    m_token.c_str(),
                                    m_config_path.c_str(),
                                    m_config_name.c_str(),
                                    NULL, /* range file */
                                    NULL, /* persist path */
                                    BIGPIPE_API_ASYNC_SUBSCRIBE, /* role */
                                    m_user.c_str());
            if (ret) {
                DSTREAM_ERROR("bigpipe sub init failed, ret=%d", ret);
                return ret;
            }

            // sub each pipelet
            for (size_t i = 0; i < m_pipelets.size(); i++) {
RESUB:
                ret = ctx->run_pipelet(m_pipelets[i].pipelet(),
                                       m_pipelets[i].msg_id(),
                                       m_pipelets[i].seq_id());
                switch (ret) {
                case 0:
                    DSTREAM_INFO("bigpipe sub pipelet %u (%ld:%lu) OK.",
                                 m_pipelets[i].pipelet(),
                                 m_pipelets[i].msg_id(),
                                 m_pipelets[i].seq_id());
                    break;

                case bigpipe::BIGPIPE_META_STARTPOINT_ERROR:
                case bigpipe::BIGPIPE_SUBSCRIBE_STARTPOINT_ERR:
                    DSTREAM_WARN("bigpipe sub pipelet(%u:%ld) failed, "
                                    "ret=%d, try to re-sub from %ld.",
                                    m_pipelets[i].pipelet(),
                                    m_pipelets[i].msg_id(),
                                    ret, m_sub_point);
                    m_pipelets[i].set_msg_id(m_sub_point);
                    m_pipelets[i].set_seq_id(1);
                    goto RESUB;
                    break;

                default:
                    DSTREAM_ERROR("bigpipe sub pipelet(%u:%ld) failed, ret=%d",
                                  m_pipelets[i].pipelet(),
                                  m_pipelets[i].msg_id(),
                                  ret);
                    return ret;
                    break;
                }
            }

            m_read_buffer = new(std::nothrow) char[kMaxMessageSize];
            assert(m_read_buffer != NULL);
            m_sub_ctx = ctx;
            m_init = true;
            return ret;

        } else {
            DSTREAM_ERROR("unkown commit log role(%d)", m_role);
            return error::FAILED_EXPECTATION;
        }
    }
    return error::FAILED_EXPECTATION;
}

int BigPipeCommitLog::Close() {
    if (!m_init) {
        return error::OK;
    }

    switch (m_role) {
    case CommitLog::kPublisher:
    {
        m_pub_ctx->flush();
        m_pub_ctx->uninit();
    }
    break;

    case CommitLog::kSubscriber:
    {
        m_sub_ctx->uninit();
        delete [] m_read_buffer;
        m_read_buffer = NULL;
    }
    break;

    default:
        break;
    }

    m_init = false;
    return error::OK;
}

BigPipeCommitLog::~BigPipeCommitLog() {
    if (m_read_buffer) {
        delete [] m_read_buffer;
        m_read_buffer = NULL;
    }
}

int BigPipeCommitLog::Read(LogInfo* log) {
    if (m_role == CommitLog::kPublisher) {
        DSTREAM_WARN("publisher can't do read");
        return error::FAILED_EXPECTATION;
    }

    if (!log) {
        DSTREAM_WARN("read param should not be null.");
        return error::FAILED_EXPECTATION;
    }

    int ret = 0;
    if (!m_sub_ctx) {
        DSTREAM_ERROR("bigpipe subscribe api invalid.");
        return error::FAILED_EXPECTATION;
    }

WAIT_AGAIN:
    bigpipe::bigpipe_epoll_event_queue_t epoll_queue;
    bigpipe::bigpipe_epoll_event_t epoll_node;
    ret = m_sub_ctx->epoll_wait(epoll_queue, 5000000);
    switch (ret) {
    case 0:
    {
        // if only one pipelet available, we read it directly,
        // else we find next available piplet,
        // whose pipelet id is larger than the previous one.
        // We use RoundRobin Algorithm, so all pipelets are treated fairly.
        uint32_t pipelet_id = m_pipelet_last_read;
        int32_t node_idx = -1;
        if (epoll_queue.size() == 1) {
            epoll_queue.get(0, &epoll_node);
        } else {
            // find next pipelet
            for (size_t i = 0; i < epoll_queue.size(); i++) {
                epoll_queue.get(i, &epoll_node);
                if (epoll_node.pipelet_id > pipelet_id) {
                    node_idx =  epoll_node.pipelet_id;
                    break;
                }
            }
            if (node_idx < 0) {
                // not find previous pipelet, use first one
                node_idx = 0;
            }
            epoll_queue.get(node_idx, &epoll_node);
        }

        pipelet_id = epoll_node.pipelet_id;
        m_pipelet_last_read = epoll_node.pipelet_id;
        DSTREAM_DEBUG("%zd pipelet ready, we will read %d",
                      epoll_queue.size(), epoll_node.pipelet_id);

        // read data
        if (epoll_node.event & BIGPIPE_EPOLL_EVENT_RECV) {
            uint32_t out_len = 0;
            ret = m_sub_ctx->receive(static_cast<uint32_t>(pipelet_id),
                reinterpret_cast<char*>(m_read_buffer),
                static_cast<uint32_t>(kMaxMessageSize),
                out_len, log->msg_id,
                log->seq_id, log->is_last);
            if (ret) {
                DSTREAM_ERROR("bigpipe sub recv failed, ret=%d", ret);
                return ret;
            }

            // write to local buffer
            log->pipelet_id = pipelet_id;
            log->byte_array.Write(reinterpret_cast<const dstream::Byte*>(m_read_buffer), out_len);
            log->recv_time = static_cast<uint64_t>(GetTimeUS());
        }

        // handle error
        if (epoll_node.event & BIGPIPE_EPOLL_EVENT_ERR) {
            switch (epoll_node.state) {
                case BIGPIPE_CONSUMER_DEAL_OVERTIME:
                    DSTREAM_WARN("pipelet %d long time no data, will recover with subpoint(-3).",
                        pipelet_id);
                    ret = m_sub_ctx->run_pipelet(pipelet_id, -3, 1);
                    if (ret != 0) {
                        DSTREAM_ERROR("pipelet %d recover failed, will reconnect.", pipelet_id);
                        return error::FAILED_EXPECTATION;
                    }
                    break;

                case BMQ_E_START_POINT_SMALL:
                case BIGPIPE_BUFFER_HAS_NO_SEQ:
                    DSTREAM_WARN("piplet %d subpoint invalid, re-sub with subpoint(%ld,1)",
                            pipelet_id, m_sub_point);
                    ret = m_sub_ctx->run_pipelet(pipelet_id, m_sub_point, 1);
                    if (ret != 0) {
                        DSTREAM_ERROR("pipelet %d re-sub failed, will reconnect.", pipelet_id);
                        return error::FAILED_EXPECTATION;
                    }
                    break;

                default:
                    DSTREAM_ERROR("unknown error %d occured, will reconnect.", epoll_node.state);
                    return error::FAILED_EXPECTATION;
            }

            // if has read data, we just return ok.
            if (log->byte_array.DataSize() != 0) {
                DSTREAM_INFO("bigpipe sub epoll recover OK, pipelet(%d).", pipelet_id);
                return ret;
            }
            // no data read, wait again
            goto WAIT_AGAIN;
        }
    }
    break;

    case BIGPIPE_SUBSCRIBE_EPOLL_OVERTIME:
    {
        DSTREAM_INFO("bigpipe sub epoll timeout.");
        goto WAIT_AGAIN;
    }
    break;

    default:
    {
        DSTREAM_ERROR("bigpipe sub epoll wait failed, ret=%d", ret);
        return ret;
    }
    break;
    }

    return ret;
}

inline uint64_t BigPipeCommitLog::GenerateSleepTime(uint32_t failcnt) {
    uint64_t sleeptime = 100000;
    if (BigPipeCommitLog::kMaxRetrycnt <= failcnt) {
        sleeptime = static_cast<uint64_t>(100000 * pow(2, BigPipeCommitLog::kMaxRetrycnt));
    } else {
        sleeptime = static_cast<uint64_t>(100000 * pow(2, failcnt));
    }
    return sleeptime;
}

int BigPipeCommitLog::Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id) {
    if (m_role == CommitLog::kSubscriber) {
        DSTREAM_WARN("subscriber can't do write");
        return error::FAILED_EXPECTATION;
    }

    ByteSize size = 0;
    uint64_t sleeptime = 0;
    uint32_t failcnt = 0;
    const dstream::Byte* data = bytes->Data(&size);
    if (!m_pub_ctx) {
        DSTREAM_ERROR("invalid bigpipe api context.");
        return error::FAILED_EXPECTATION;
    }

    int ret = 0;
    bigpipe::bigpipe_pub_result_t result;
    do {
        ret = m_pub_ctx->send_to_one(m_pipelet_sendto, reinterpret_cast<const char*>(data),
                        size, NULL, &result);
        if (BMQ_E_QUEUE_FULL == ret) {
            sleeptime = GenerateSleepTime(failcnt);
            bigpipe_api_common_util::select_sleep(sleeptime);
            DSTREAM_WARN("bigpipe queue full, will retry later.failcnt(%u),sleeptime(%lu)ms.",
                failcnt, sleeptime);
            failcnt++;
        }
    } while (BMQ_E_QUEUE_FULL == ret);

    if (log_id) {
        *log_id = result.pipelet_msg_id;
    }
    // not used
    (void)(duplicated);

    return ret;
}

void BigPipeCommitLog::SetPartition(int pipelet_id) {
    m_pipelet_sendto = pipelet_id;
}

// ------------------------------------------------------------
// LocalFileCommitLog Implementation
LocalFileCommitLog::LocalFileCommitLog(Role role,
                                       const std::string& file_name):
    CommitLog(role), m_file_name(file_name),
    m_fd(-1), m_offset(0), m_file_len(0), m_loop_read(false) {
}

LocalFileCommitLog::LocalFileCommitLog(Role role,
                                       const std::string& file_name,
                                       bool loop_read):
    CommitLog(role), m_file_name(file_name),
    m_fd(-1), m_offset(0), m_file_len(0), m_loop_read(loop_read) {
}

LocalFileCommitLog::~LocalFileCommitLog() {
    Close();
}

int LocalFileCommitLog::Open() {
    if (m_fd == -1) {
        if (m_role == kPublisher) {
            m_fd = ::open(m_file_name.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (m_fd == -1) {
                DSTREAM_WARN("open(%s,O_WRONLY | O_CREAT | O_APPEND, 0666) failed(%s)",
                                m_file_name.c_str(),
                                strerror(errno));
                return error::FAILED_EXPECTATION;
            }
            struct stat st_buf;
            if (fstat(m_fd, &st_buf) == -1) {
                DSTREAM_WARN("file(%s) fstat(%d) failed(%s)",
                                m_file_name.c_str() , m_fd, strerror(errno));
                return error::FAILED_EXPECTATION;
            }
            m_offset = st_buf.st_size;
            return error::OK;
        } else { // as subscriber.
            m_fd = ::open(m_file_name.c_str(), O_RDONLY, 0666);
            if (m_fd == -1) {
                DSTREAM_WARN("open(%s,O_RDONLY,0666) failed(%s)",
                                m_file_name.c_str(), strerror(errno));
                return error::FAILED_EXPECTATION;
            }

            struct stat st_buf;
            if (fstat(m_fd, &st_buf) == -1) {
                DSTREAM_WARN("file(%s) fstat(%d) failed(%s)",
                                m_file_name.c_str(), m_fd, strerror(errno));
                return error::FAILED_EXPECTATION;
            }
            m_file_len = static_cast<uint64_t>(st_buf.st_size);
            // in loop read mode, we can always read data,unless file is unacessable.
            if (m_loop_read) {
                if (m_offset >= m_file_len) {
                    m_offset = 0;
                }
            } else {
                if (m_offset >= m_file_len) {
                    m_offset = m_file_len;
                }
            }
            // seek here.
            if (lseek(m_fd , m_offset, SEEK_SET) == -1) {
                DSTREAM_WARN("file(%s) lseek(%d,%zu,SEEK_SET) failed(%s)",
                                m_file_name.c_str(), m_fd,
                                static_cast<size_t>(m_offset), strerror(errno));
                return error::FAILED_EXPECTATION;
            }
            DSTREAM_DEBUG("open file(%s) at %zd OK.", m_file_name.c_str(), m_offset);
            return error::OK;
        }
    }
    return error::OK;
}

int LocalFileCommitLog::Close() {
    if (m_fd != -1) {
        ::close(m_fd);
        m_fd = -1;
    }
    return error::OK;
}

int LocalFileCommitLog::Read(LogInfo* log) {
    if (m_role == kPublisher) {
        DSTREAM_WARN("publisher can't do read");
        return error::FAILED_EXPECTATION;
    }

    if ( m_offset >= m_file_len ) {
        if (m_loop_read) {
            m_offset = 0;
            if (lseek(m_fd , m_offset, SEEK_SET) == -1) {
                DSTREAM_WARN("file(%s) lseek(%d,%zu,SEEK_SET) failed(%s)",
                                m_file_name.c_str(), m_fd,
                                static_cast<size_t>(m_offset), strerror(errno));
                return error::FAILED_EXPECTATION;
            }
        } else {
            usleep(1000000);
            DSTREAM_WARN("local file(%s) read to EOF...", m_file_name.c_str());
            return error::FILE_READ_EOF;
        }
    }
    if (log) {
        log->msg_id = m_offset;
    }
    // | uint64_t(size) | content |
    uint64_t msgsz = 0;
    if (::read(m_fd, &msgsz, sizeof(msgsz)) == -1) {
        DSTREAM_WARN("file(%s) read(%d) failed(%s)", m_file_name.c_str() , m_fd, strerror(errno));
        return error::FAILED_EXPECTATION;
    }
    dstream::Byte* data = log->byte_array.Allocate(msgsz);
    if (data == NULL) {
        DSTREAM_WARN("bytes allocate failed");
        return error::FAILED_EXPECTATION;
    }
    if (::read(m_fd, data, msgsz) == -1) {
        DSTREAM_WARN("file(%s) read(%d,%zu) failed(%s)", m_file_name.c_str() , m_fd,
                        static_cast<size_t>(msgsz), strerror(errno));
        return error::FAILED_EXPECTATION;
    }
    // bookmark the offset.
    m_offset += sizeof(msgsz) + msgsz;
    return error::OK;
}

int LocalFileCommitLog::Write(ReadableByteArray* bytes, bool* dup, uint64_t* log_id) {
    if (m_role == kSubscriber) {
        DSTREAM_WARN("subscriber can't do write");
        return error::FAILED_EXPECTATION;
    }
    if (dup) {
        *dup = false;
    }
    if (log_id) {
        *log_id = m_offset;
    }
    // | uint64_t(size) | content |
    ByteSize dsize = 0;
    const dstream::Byte* data = bytes->Data(&dsize);
    // aggregate them. we assume a atomic message.
    uint64_t size = dsize;
    WriteableByteArray out_bytes;
    if (!out_bytes.Write(reinterpret_cast<const dstream::Byte*>(&size), sizeof(size))) {
        DSTREAM_WARN("out_bytes write size failed");
        return error::FAILED_EXPECTATION;
    }
    if (!out_bytes.Write(data, size)) {
        DSTREAM_WARN("out_bytes write content failed");
        return error::FAILED_EXPECTATION;
    }
    // write to fd.
    ByteSize size2 = 0;
    const dstream::Byte* data2 = out_bytes.Data(&size2);
    if (::write(m_fd, data2, size2) == -1) {
        DSTREAM_WARN("file(%s) write(%d) failed(%s)", m_file_name.c_str() , m_fd, strerror(errno));
        return error::FAILED_EXPECTATION;
    }
    // bookmark the offset.
    m_offset += size2;
    return error::OK;
}

static void* proxy_scribe_commit_log_thread(void* arg) {
    ScribeCommitLog* sl = reinterpret_cast<ScribeCommitLog*>(arg);
    sl->Serve();
    return NULL;
}

ScribeCommitLog::ScribeCommitLog(Role role, int buffer_size) :
                CommitLog(role), m_init(0), m_port(8840), m_buffer_size(buffer_size) {
    m_handler          = boost::shared_ptr<ScribeHandler>(new ScribeHandler(m_buffer_size));
    m_notification     = boost::shared_ptr<ServerInitNotification>(new ServerInitNotification);
    m_processor        =
        boost::shared_ptr<apache::thrift::TProcessor>(new scribeProcessor(m_handler));
    m_protocol_factory =
        boost::shared_ptr<TProtocolFactory>(new TBinaryProtocolFactory(0, 0, false, false));
    m_thread_manager   = ThreadManager::newSimpleThreadManager(5);
    m_thread_factory   = boost::shared_ptr<PosixThreadFactory>(new PosixThreadFactory());
    m_thread_manager->threadFactory(m_thread_factory);
    m_thread_manager->start();
}

ScribeCommitLog::~ScribeCommitLog() {
    if (m_thread_manager) {
        m_thread_manager->stop();
    }
}

int ScribeCommitLog::Open() {
    int ret = pthread_create(&m_tid, NULL, proxy_scribe_commit_log_thread, this);
    if (ret != 0) {
        return error::FAILED_EXPECTATION;
    }
    return error::OK;
}

int ScribeCommitLog::Read(LogInfo* log) {
    while (m_handler->IsEmpty()) {
        usleep(1000);
    }
    m_handler->GetBuffer(log);
    return 0;
}

int ScribeCommitLog::Write(ReadableByteArray* bytes, bool* duplicated, uint64_t* log_id) {
    bytes = 0;
    duplicated = false;
    log_id = 0;
    return 0;
}

int ScribeCommitLog::Peek(bool* eof, int /*timeout_ms*/) {
    *eof = m_handler->IsEmpty();
    return 0;
}

int ScribeCommitLog::Close() {
    return 0;
}

int32_t ScribeCommitLog::IsInited() {
    return m_notification->IsInited();
}

void ScribeCommitLog::Serve() {
    DSTREAM_INFO("begin to start server ...");
    while (m_port < kMaxPort) {
        try {
            m_server = boost::shared_ptr<TNonblockingServer>(new
                TNonblockingServer(m_processor, m_protocol_factory, m_port, m_thread_manager));
            m_server->setServerEventHandler(m_notification);
            m_server->serve();
        } catch (const TTransportException& ex) {
            if (ex.getType() == TTransportException::NOT_OPEN) {
                DSTREAM_INFO("Bind port %u fail, try next port ...", m_port);
                m_port++;
                continue;
            } else {
                DSTREAM_INFO("Start ThriftNonblocking server fail, error type:%d", ex.getType());
                break;
            }
        }
    }
    return;
}

void ScribeCommitLog::Stop() {
    DSTREAM_INFO("begin to stop server ...");
    m_server->stop();
}

} // namespace dstream
