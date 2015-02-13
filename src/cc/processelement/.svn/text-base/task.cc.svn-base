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

#include "processelement/task.h"
#include <stdint.h>
#include "common/bounded_blocking_queue.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/rpc.h"
#include "common/rpc_const.h"
#include "common/proto/pub_sub.pb.h"
#include "common/proto_rpc.h"
#include "common/utils.h"

namespace dstream {
namespace processelement {

using namespace std;
using namespace dstream::common;

// ------------------------------------------------------------
uint64_t Tuple::m_instance_counter = 0;
Tuple::Tuple()
        : m_dynamic_type(false),
        m_hash_code(m_instance_counter),
        m_tag() {
}

// ------------------------------------------------------------
bool StartKylin() {
    if (!transporter::g_init) {
        transporter::g_kKylinExecWorkers = 1;
        transporter::g_kKylinNetWorkers  = 1;
        transporter::g_kKylinDiskWorkers = 0;
        transporter::g_esp_port          = 0; // do not listen at anyport
        transporter::g_kKylinBufSmallTranBufNum  = 1024 * 32;
        transporter::g_kKylinBufSmallTranBufSize = 1024 * 4;
        transporter::g_kKylinBufBigTranBufNum    = 32; // 2MB
        transporter::TPInitKylin();
    }
    assert(IsKylinRunning());
    assert(transporter::g_kKylinExecWorkers > 0);
    assert(transporter::g_kKylinNetWorkers > 0);
    return true;
}

void StopKylin() {
    transporter::TPStopKylin();
}

// ------------------------------------------------------------
const char* Task::kImporterTupleTag = "dstream.pe.importer.tuple.tag";
int Task::m_instance_counter = 0;
typedef BoundedBlockingQueue< ForwardLogRequest* > Q;
Task* Task::m_instance = NULL;
Task::Task()
        : m_quit(false),
        m_terminating(false),
        m_term_recvived(false),
        m_counter_id(0) {
    // --------------------
    // initialize log.
    logger::initialize(Context::m_name.c_str(), m_log_level);

    if (m_instance_counter == 0) {
        m_instance_counter++;
        m_instance = this;
        // set a new process group
        setpgid(getpid(), getpid());
        SetSignalHandler();

        rpc::Context::init();
    } else {
        DSTREAM_FATAL("can't create Task instance more than once");
    }

    m_buf = reinterpret_cast<Byte*>(malloc(m_buf_len));
    if (NULL == m_buf) {
        DSTREAM_FATAL("fail to alloc %d bytes mem",
                      transporter::kMaxMsgSize);
    }
    DSTREAM_INFO("alloc m_buf=%p, m_buf_len=%d",
                 m_buf, static_cast<int>(m_buf_len));
}

Task::~Task() {
    SAFE_DELETE(m_if_cli);
    SAFE_DELETE(m_of_cli);

    free(m_buf);
    m_buf = NULL;
    m_buf_len = 0;
}

Tuple* Task::Emit(const std::string& tag) {
    Tuple* tuple = LoopBack::AllocateTuple(tag);
    if (!tuple) {
        DSTREAM_WARN("allocateOutTuple '%s' failed", tag.c_str());
        return tuple;
    }
    // save the 'gonna send' tuples.
    m_send_tuples.push_back(tuple);
    return tuple;
}

int Task::StartTransporter() {
    int ret = error::OK;
    if (!m_as_importer) {
        m_if_cli = new (std::nothrow) PEFetchCli(m_id);
        if (NULL == m_if_cli) {
            DSTREAM_WARN("fail to alloc PEFetchCli");
            return error::BAD_MEM_ALLOC;
        }
        ret = error::NOT_RUNNING;
        for (uint64_t i = 0; ; i++) {
            ret = m_if_cli->Init(m_pn_ipport, m_fetch_log_timeout_ms);
            if (ret == error::OK) {
                DSTREAM_INFO("conn to inflow[%lu]@pn[ip=%s, timeout=%dms] ...OK",
                             m_id, m_pn_ipport.c_str(), m_fetch_log_timeout_ms);
                break;
            }
            if (0 == (i % 100)) {
                DSTREAM_WARN("fail to conn to inflow(%lu)@pn: %s",
                             m_id, error::ErrStr(ret));
            }
        }
    }
    m_of_cli = new (std::nothrow) PEForwardCli(m_id);
    if (NULL == m_of_cli) {
        DSTREAM_DEBUG("fail to alloc PEForwardCli");
        return error::BAD_MEM_ALLOC;
    }
    ret = error::NOT_RUNNING;
    for (uint64_t i = 0; ; i++) {
        ret = m_of_cli->Init(m_pn_ipport, m_send_log_timeout_ms);
        if (ret == error::OK) {
            DSTREAM_INFO("conn to outflow[%lu]@pn[ip=%s, timeout=%dms] ...OK",
                         m_id, m_pn_ipport.c_str(), m_send_log_timeout_ms);
            break;
        }
        if (0 == (i % 100)) {
            DSTREAM_WARN("fail to conn to outflow(%lu)@pn: %s",
                         m_id, error::ErrStr(ret));
        }
    }
    return ret;
}

static void* proxy_heartbeat_thread_function(void* arg) {
    Task* task = reinterpret_cast<Task*>(arg);
    return task->HeartbeatThreadFunction();
}

void* Task::HeartbeatThreadFunction() {
    rpc::Connection conn;
    if (conn.Connect(m_node_uri.c_str()) < 0) {
        DSTREAM_WARN("connect '%s' failed", m_node_uri.c_str());
        goto fail;
    }
    while (!m_quit) {
        // to assure each run, we will wait
        // unit is second, I think that's enough
        // for heartbeat routine.
        sleep(m_heartbeat_poll_timeout_sec);

        // rpc call heartbeat.
        proto_rpc::RPCRequest< HeartbeatRequest > req;
        proto_rpc::RPCResponse < HeartbeatACK > res;
        req.set_method(rpc_const::kPubSubHeartbeatMethod);
        req.mutable_peid()->set_id(m_id);
        req.add_metric_name(kRecvTuplesCounter);
        req.add_metric_value(NumberToString(m_recv_tuples_counter));
        req.add_metric_name(kSendTuplesCounter);
        req.add_metric_value(NumberToString(m_send_tuples_counter));
        // add by lanbijia@baidu.com
        // for QPS statistic
        req.add_metric_name(kRecvTimeTuplesCounter);
        req.add_metric_value(NumberToString(m_recv_time_tuples_counter /
                                            m_heartbeat_poll_timeout_sec));
        req.add_metric_name(kSendTimeTuplesCounter);
        req.add_metric_value(NumberToString(m_send_time_tuples_counter /
                                            m_heartbeat_poll_timeout_sec));
        m_recv_time_tuples_counter = 0;
        m_send_time_tuples_counter = 0;
        // set user-defined counters
        {
            MutexLockGuard lock_guard(m_counter_map_lock);
            for (size_t i = 0; i < m_counter_map.size(); i++) {
                req.add_metric_name(m_counter_map[i]->name()->c_str());
                req.add_metric_value(NumberToString(m_counter_map[i]->amount()));
            }
        }

        // update cpu, mem usage counters
        int pid = getpid();
        CGLimitUtils cgutils(pid); 
        CpuStat ustat = cgutils.GetCurCpuUsage();
        uint64_t cpu_usage = static_cast<uint64_t>(100 *
                             cgutils.GetCpuUsage(m_counters.last_cpu_stat, ustat));
        m_counters.cpu_usage_counter->Set(cpu_usage);
        m_counters.last_cpu_stat = ustat;
        req.add_metric_name(kCpuUsageCounter);
        req.add_metric_value(NumberToString(cpu_usage));

        MemStat mstat = cgutils.GetMemUsage();
        m_counters.mem_usage_counter->Set(mstat.group);
        req.add_metric_name(kMemUsageCounter);
        req.add_metric_value(NumberToString(mstat.group));

#ifdef DEBUG_TRACE_PRC
        // TRACE
        struct timeval tv;
        char debug_info_string[256];
        gettimeofday(&tv, NULL);
        snprintf(debug_info_string, sizeof(debug_info_string),
                 "[%lu]%s,", (tv.tv_sec * 1000000 + tv.tv_usec),
                 "HeartbeatRequest/kPubSubHeartbeatMethod");
        req.set_debug_info(debug_info_string);
        DSTREAM_DEBUG("[TRACE] [%s] before conn.call(), kDefaultHeartBeatTimeoutMillSeconds"
                      ":%d, req's debug_info_string:%s",
                      __FUNCTION__, kDefaultHeartBeatTimeoutMillSeconds, debug_info_string);

        DSTREAM_DEBUG("[TRACE] [%s] before conn.call(), conn// addr[%s]ref[%lld]working[%s]",
                      __FUNCTION__, conn.uri(), conn.ref(),
                      conn.working() == true ? "true" : "false");
#endif

        if (conn.Call(&req, &res, kDefaultHeartBeatTimeoutMillSeconds) < 0) {
            DSTREAM_WARN("[%s] call(%s) failed", __FUNCTION__,
                            rpc_const::kPubSubHeartbeatMethod);
            continue;
        }

        
        if (res.has_flow_control()) {
            const FlowControl& flow_control = res.flow_control();
            int32_t flow_limit = flow_control.qps_limit() < 0 ? 
                    INT32_MAX : flow_control.qps_limit();
            if (flow_limit != m_spout_limit_value) {
                DSTREAM_INFO("spout limit has changed from %d to %d",
                        m_spout_limit_value, flow_limit);
                AtomicSetValue(m_spout_limit_value, flow_limit);

                if (m_spout_limit_value <= 0) {
                    stopCallback(); // TODO(somerd@baidu.com: abandon when no user use it)
                    StopCallback();
                    DSTREAM_INFO("run user stop callback OK.");
                } else {
                    StartCallback();
                    startCallback(); // TODO(somerd@baidu.com: abandon when no user use it)
                    DSTREAM_INFO("run user start callback OK.");
                }
            }
        }
#ifdef DEBUG_TRACE_PRC
        // TRACE
        if (res.has_debug_info()) {
            DSTREAM_DEBUG("[TRACE] [%s] after conn.call(), res's debug_info:%s",
                          __FUNCTION__, res.debug_info().c_str());
        }
        DSTREAM_DEBUG("[TRACE] [%s] after conn.call(), kDefaultHeartBeatTimeoutMillSeconds:%d",
                      __FUNCTION__, kDefaultHeartBeatTimeoutMillSeconds);
#endif
        assert(res.return_value() == rpc_const::kOK);
    }
fail:
    conn.Close();
    return NULL;
}

void* Task::LoopbackThreadFunction() {
    if (LoopBack::Start() < error::OK) {
        DSTREAM_WARN("start loopback fail");
        return NULL;
    }
    DSTREAM_DEBUG("start loopback transporter");
    rpc::Connection conn;
    for (int i = 0; i < kLoopBackServiceNumber; i++) {
        conn.RegisterService(m_services[i]->service_name(), m_services[i]);
    }
    conn.Serve(kDefaultNodeUri, 1); // 1 working thread is enough. I guess.
    return NULL;
}

static void* proxy_loopback_thread_function(void* arg) {
    Task* task = reinterpret_cast<Task*>(arg);
    return task->LoopbackThreadFunction();
}

struct TaskCommitLog {
    Task* task;
    std::string commit_name;
};
static void* proxy_importer_thread_function(void* arg) {
    TaskCommitLog* tc = reinterpret_cast<TaskCommitLog*>(arg);
    tc->task->ImporterThreadFunction(tc->commit_name);
    delete reinterpret_cast<TaskCommitLog*>(tc);
    return NULL;
}
static void* proxy_importer_progress_thread_function(void* arg) {
    Task* task = reinterpret_cast<Task*>(arg);
    return task->ImporterProgressThreadFunction();
}

// return true when open success
bool Task::OpenLoop(const std::string& commit_name, CommitLog* commit_log) {
    int ret = 0;
    if ((ret = commit_log->Open()) != 0) {
        commit_log->Close(); // call close because we need re-init subscriber
        DSTREAM_WARN("commit (%s) open failed, will re-open.", commit_name.c_str());
        return false;
    }
    // if scribe importer, register on zk
    DSTREAM_INFO("importer type is %lu", m_pe_type);
    if (m_pe_type == internal::Context::PE_TYPE_SCRIBE_IMPORTER) {
        ScribeCommitLog* scl = reinterpret_cast<ScribeCommitLog*>(commit_log);
        while(!scl->IsInited()) {
            usleep(10000);
        }
        DSTREAM_INFO("register app [%s] port [%d] on zk", m_app_name.c_str(), scl->Port());
        ret = m_scribe_register.Init(m_id, m_app_name, Context::m_name + "_sub.xml", scl->Port());
        if (error::OK != ret) {
            DSTREAM_WARN("scribe register failed, [error=%s]", error::ErrStr(ret));
            return false;
        }
    }
    // open success
    return true;
}

void* Task::ImporterProgressThreadFunction() {
    int32_t ret = 0;
    uint64_t interval = 0;
    bool go_on = true;
    ret = m_sub_progress.GetPollInterval(&interval);
    if (0 != ret) {
        DSTREAM_WARN("get poll interval failed");
        go_on = false;
    }

    while (!m_quit && go_on) {
        usleep(interval * 1000);

        ret = m_sub_progress.save();
        if (error::OK != ret) {
            DSTREAM_WARN("progress save failed, [ret=%d]", ret);

            // reinit
            m_sub_progress.Uninit();
            ret = m_sub_progress.Init(m_id, Context::m_app_name, Context::m_name + "_sub.xml");
            if (error::OK != ret) {
                DSTREAM_WARN("progress reinit failed, [ret=%d]", ret);
            } else {
                DSTREAM_INFO("progress connection reinit OK.");
            }
        }
    }

    return NULL;
}

void* Task::ImporterThreadFunction(const std::string& commit_name) {
    CommitLog* commit_log = findCommitLog(commit_name);
    if (NULL == commit_log) {
        DSTREAM_ERROR("can find cimmit_log (%s)", commit_name.c_str());
        return NULL;
    }

    // open
    int ret = 0;
    bool reopen = true;
    LogQ* q = m_importer_log_queue;
    while (!m_terminating && !m_quit) {
        if (reopen) {
            if (!OpenLoop(commit_name, commit_log)) {
                usleep(100000); // 100ms
                continue;
            } 
            reopen = false;
            DSTREAM_DEBUG("commit (%s) open succeed", commit_name.c_str());
        }

        if (AtomicGetValue(m_spout_limit) <= 0) {
            usleep(100); // 0.1ms
            continue;
        }

        LogInfo log_info;
        if ((ret = commit_log->Read(&log_info)) != 0) {
            DSTREAM_WARN("commit log read failed, will reopen it...");
            commit_log->Close(); // call close because we need re-init subscriber
            reopen = true;
            continue;
        }

        q->Put(log_info);
        AtomicSetValue(m_spout_limit, m_spout_limit - 1);
        // update importer queue counter
        m_counters.importer_queue_counter->Set(q->size());
    }

    DSTREAM_ERROR("thread [importer_thread_function] exit.");
    return NULL;
}

static void* proxy_ping_thread_function(void* arg) {
    Task* task = reinterpret_cast<Task*>(arg);
    return task->PingThreadFunction();
}

void* Task::PingThreadFunction() {
    DSTREAM_INFO("ping start...");
    while (getppid() != 1) {
        sleep(kPingTimeoutSeconds);
        AtomicSetValue(m_spout_limit, m_spout_limit_value);
    }
    DSTREAM_WARN("parent exit.ppid==1");
    AtomicSetValue(m_quit, 1);

    // (dirlt):seems if we set quit flag, we can't cancel other threads.
    // and there are bunch threads we don't possess their tid, so we can't
    // cancel them.
    usleep(10000000);
    _exit(0);
}

int Task::NormalWorkflow() {
    if (m_terminating) {
        DSTREAM_WARN("task is terminating, stop fetch data.");
        return error::PE_TERMINATING;
    }

    // fetch data
    BufHandle* hdl = NULL;
    int batch      = m_tuples_batch_count;
    int size       = m_tuples_batch_size;
    uint64_t ack = m_if_ack;
    int ret = m_if_cli->FetchData(&hdl, &batch, &size, &ack);
    UNLIKELY_IF(error::OK != ret) {
        if (ret == error::DROP_MSG_DUE_ACK_MISS_MATCH && m_if_ack == 0) {
            m_if_ack = ack;
            DSTREAM_INFO("reset client ack: %lu(ack: %lu)", m_if_ack, ack);
        } else {
            DSTREAM_WARN("fetch data fail: m_if_ack=%lu, ack=%lu: %s",
                         m_if_ack, ack, error::ErrStr(ret));
        }
        return ret;
    }

    // deseriel data
    m_recv_tuples.clear();
    ret = DeserialTuples(hdl, batch, size, &m_recv_tuples);
    ChainFreeHdl(hdl, NULL);
    hdl = NULL;
    UNLIKELY_IF(error::OK != ret) {
        DSTREAM_WARN("tuple deserialize failed, num=%d, size=%d: %s",
                     batch, size, error::ErrStr(ret));
        return ret;
    }

    m_if_ack++; // increase ack

    // callback
    onTuples(m_recv_tuples);
    OnTuples(m_recv_tuples);

    // deallocate in_tuples.
    for (size_t i = 0; i < m_recv_tuples.size(); i++) {
        Tuple* tuple = const_cast<Tuple*>(m_recv_tuples[i]);
        if (internal::Context::PE_TYPE_STREAMING == m_pe_type) {
            tuple->set_tag(internal::Context::kStreamingKVTagName);
        }
        DeallocateTuple(tuple);
    }
    m_recv_tuples.clear();
    return error::OK;
}

int Task::ImporterWorkflow() {
    // simple. fetch message from working queue
    // and call 'onSpout'
    LogQ* q = m_importer_log_queue;

HOLDING_RUN:
    // don't let the run() loop blocked, cause subscribe and forward now is async.
    if (q->Empty()) {
        // reset holding counter
        m_importer_tuple_holding_size = 0;
        m_importer_tuple_holding_count = 0;
        return error::IMPORTER_QUEUE_EMPTY;
    }
    DSTREAM_DEBUG("process spout...");

    LogInfo log = q->Take();

    ByteSize size = 0;
    const dstream::Byte* data = log.byte_array.Data(&size);
    ReadableByteArray bytes(data, size);

    AtomicAdd64(&m_recv_tuples_counter, 1); // one message.
    AtomicAdd64(&m_recv_time_tuples_counter, 1);

    // update counter
    m_counters.recv_tuples_counter->Increment();
    m_counters.recv_bytes_counter->Add(size);

    m_importer_tuple_holding_size += size;
    m_importer_tuple_holding_count++;

    // before onSpout, set context
    // stringstream ss;
    // ss<<log.sub_point_;
    char sub_point_str[32];
    snprintf(sub_point_str, sizeof(sub_point_str), "%lu", log.pipelet_id);
    Context::set(kCtxImporterPipeletID, sub_point_str);
    snprintf(sub_point_str, sizeof(sub_point_str), "%lu", log.msg_id);
    Context::set(kCtxImporterMessageID, sub_point_str);
    snprintf(sub_point_str, sizeof(sub_point_str), "%lu", log.seq_id);
    Context::set(kCtxImporterSequenceID, sub_point_str);
    snprintf(sub_point_str, sizeof(sub_point_str), "%lu", log.recv_time);
    Context::set(kCtxImporterRecvTime, sub_point_str);

    OnSpout(&bytes); // for forward compatibility
    onSpout(&bytes); // for forward compatibility abandon

    // do some collearup
    free(const_cast<dstream::Byte*>(data));

    if (!importer_reach_holding_threshold()) {
        DSTREAM_DEBUG("not reach batch send threshold, run again.");
        goto HOLDING_RUN;
    }
    DSTREAM_DEBUG("threshold reached(%ld, %ld) > (%d, %d), forwarding log...",
                  m_importer_tuple_holding_size, m_importer_tuple_holding_count,
                  m_tuples_batch_size, m_tuples_batch_count);

    m_importer_tuple_holding_size = 0;
    m_importer_tuple_holding_count = 0;

    return error::OK;
}

int Task::SyncNode() {
    DSTREAM_INFO("node is ready");
    return 0;
}

int Task::ForwardLogToBigpipe() {
    size_t send_tuples_size = 0;
    {
        MutexLockGuard send_tuple_lock(m_send_tuples_lock);

        send_tuples_size = m_send_tuples.size();
        if (0 == send_tuples_size) {
            return 0;
        }

        BigPipeCommitLog* bigpipe_publisher = dynamic_cast<BigPipeCommitLog*>(m_publisher);
        if (bigpipe_publisher == NULL) {
            DSTREAM_ERROR("bigpipe publisher init failed");
            return -1;
        }

        for (size_t i = 0; i < send_tuples_size; ++i) {
            Tuple* out_tuple = const_cast<Tuple*>(m_send_tuples[i]);
            WriteableByteArray to_serialized_bytes;
            out_tuple->Serialize(&to_serialized_bytes);

            size_t out_bytes_size = 0;
            const Byte* data = to_serialized_bytes.Data(
                                    reinterpret_cast<ByteSize*>(&out_bytes_size));
            ReadableByteArray out_bytes(data, out_bytes_size);

            bigpipe_publisher->SetPartition(out_tuple->hash_code() % m_pipelet_num);
            uint32_t retry = 0;

            while (bigpipe_publisher->Write(&out_bytes, NULL, NULL) < 0 &&
                   retry < m_bigpipe_retry_time) {
                DSTREAM_WARN("bigpipe exporter publish failed, retry time %d", retry);
                usleep(m_bigpipe_retry_interval * 1000); // convert to microsecond

                // reconnect
                bigpipe_publisher->Open();
                retry++;
            }
            DeallocateTuple(out_tuple);
            if (retry == m_bigpipe_retry_time) {
                DSTREAM_ERROR("bigpipe exporter max retry failed.");
                // fatal error, need exit
                return error::BIGPIPE_CONNECTION_FAIL;
            }
            // update send bytes counter
            m_counters.send_bytes_counter->Add(out_bytes_size);
        }
        // clear send queue
        m_send_tuples.clear();
        // update counter
        m_counters.send_tuples_counter->Add(send_tuples_size);
        AtomicAdd64(&m_send_tuples_counter, send_tuples_size);
        AtomicAdd64(&m_send_time_tuples_counter, send_tuples_size);
    }
    return 0;
}

int Task::ForwardLog() {
    // --------------------
    // write out_tuples and deallocte out_tuples.
    // {Tuple:hash_code|tag_len|tag|tuple_len|tuple}{Tuple}{Tuple}...
    int     ret   = error::OK;
    size_t  size  = config_const::kPNTuplesBatchSizeByte;
    int32_t batch = 0;
    int32_t remain = 0;
    CBufWriteStream bws;

    // lock m_send_tuples_lock, copy and erase handled tuples
    m_send_tuples_lock.lock();
    if (0 == m_send_tuples.size()) {
        m_send_tuples_lock.unlock();
        return error::OK;
    }
    ret = SerialTuples(m_send_tuples, &bws, &batch, &size);
    UNLIKELY_IF(ret != error::OK) {
        std::string s;
        DumpTuples(m_send_tuples, &s);
        m_send_tuples.clear();
        m_send_tuples_lock.unlock();
        DSTREAM_WARN("tuple serialize failed: %s", error::ErrStr(ret));
        DSTREAM_WARN("those tuples lost!!!: tuples: %s", s.c_str());
        return ret;
    }
    Tuples send_tuples(m_send_tuples.begin(), m_send_tuples.begin() + batch);
    m_send_tuples.erase(m_send_tuples.begin(), m_send_tuples.begin() + batch);
    remain = static_cast<int32_t>(m_send_tuples.size()) - batch;
    m_send_tuples_lock.unlock();

    UNLIKELY_IF(0 == batch) {
        return error::OK;
    }

    // update counter
    m_counters.send_bytes_counter->Add(size);
    m_counters.send_tuples_counter->Add(batch);
    AtomicAdd64(&m_send_tuples_counter, batch);
    AtomicAdd64(&m_send_time_tuples_counter, batch);
    DSTREAM_DEBUG("tuples batched, tuple number: %u; total: %lu",
                  batch, static_cast<uint64_t>(m_send_tuples_counter));

    uint64_t max_retry = UINT64_MAX;
    BufHandle* hdl = bws.GetBegin();
    for (uint64_t retry = 0; !m_quit && (retry < max_retry); retry++) {
        uint64_t ack = m_of_ack;
        ret = m_of_cli->SendData(hdl, batch, size, &ack);
        if (LIKELY(error::OK == ret)) {
            DSTREAM_DEBUG("%luth: send data ...OK: %s", retry, error::ErrStr(ret));
            m_of_ack++;
            break;
        } else if (error::OK_SOFT_WATERMARK == ret) {
            // queue reaches watermark
            DSTREAM_DEBUG("%luth: send data ...OK: %s", retry, error::ErrStr(ret));
            m_of_ack++;
            break;
        } else if (error::QUEUE_FULL == ret) { // queue is full
            DSTREAM_DEBUG("%luth: pn send queue if full", retry);
            usleep(10);
            m_of_ack++;
            break;
            // pn send queue full
            // we need suspend pe importer thread and peek pn status,
            // then resend these tuples
        } else if (error::DROP_MSG_DUE_ACK_MISS_MATCH == ret) {
            if (m_of_ack + 1 == ack) { // outflow已经收到数据，只是之前的ack丢失
                DSTREAM_DEBUG("%luth: %s: m_of_ack: %lu => %lu", retry,
                              error::ErrStr(ret), m_of_ack, ack);
                m_of_ack++;
                break;
            } else { // 不知为何错乱，丢弃当前状态
                DSTREAM_DEBUG("%luth: %s: m_of_ack: %lu => %lu", retry,
                              error::ErrStr(ret), m_of_ack, ack);
                m_of_ack = ack;
            }
        } else if (error::DATA_CORRUPTED == ret) { // data corrupted
            // jump the data
            DSTREAM_WARN("%luth: fail to send data: %s", retry, error::ErrStr(ret));
            std::string s;
            DumpTuples(send_tuples, &s);
            DSTREAM_WARN("those tuples may lost!!!: tuples: %s", s.c_str());
            m_of_ack++;
            break;
        } else {
            std::string s;
            DumpTuples(send_tuples, &s);
            DSTREAM_WARN("%luth: fail to send tuples due to: %s: tuples:%s",
                         retry, error::ErrStr(ret), s.c_str());
        }
    } // end of retry
    ChainFreeHdl(hdl, NULL);
    hdl = NULL;

    // erase already handled tuples
    for (Tuples::iterator it = send_tuples.begin(); it != send_tuples.end(); it++) {
        Tuple* tuple = const_cast<Tuple*>(*it);
        if (internal::Context::PE_TYPE_STREAMING == m_pe_type) {
            tuple->set_tag(internal::Context::kStreamingKVTagName);
        }
        DeallocateTuple(tuple);
    }

    // check rpc call time
    m_last_send_tuples_timestamp = static_cast<size_t>(GetTimeMS());
    return remain > 0 ? error::OK_REMAIN_DATA : error::OK;
}

void Task::Run(bool use_loopback) {
    m_use_loopback = use_loopback;

    pthread_t loopback_tid;
    pthread_t heartbeat_tid;
    pthread_t ping_tid;
    pthread_t importer_log_tid;
    pthread_t importer_progress_tid;
    int thread_ret = 0;

    // install SIGTERM handler
    SetSignalHandler();

    // --------------------
    // start loopback.
    if (use_loopback) {
        if (!setupLoopBackEnvironment()) {
            DSTREAM_WARN("setupLoopBackEnvironment failed");
            return;
        }
        DSTREAM_INFO("start loopback thread");
        thread_ret = pthread_create(&loopback_tid, NULL, proxy_loopback_thread_function, this);
        if (thread_ret != 0) {
            DSTREAM_WARN("pthread_create failed(%s)", strerror(thread_ret));
            return;
        }
        DSTREAM_INFO("loopback thread start over");
        if (SyncNode() != 0) {
            DSTREAM_WARN("SyncNode failed");
            return;
        }
    }
    // --------------------
    // read environment
    // the reason that put this code behind use_loopback is because
    // use_loopback logic may overwrite environment
    DSTREAM_INFO("readEnvironment...");
    if (!readEnvironment()) {
        DSTREAM_WARN("readEnvironment failed");
        return;
    }
    DynamicProtoType::SetProtoPath(m_dynamic_proto_dir);
    // init global counters after read environment
    // TODO(konghui@baidu.com): a better solution is to
    //      init counters in main(), we need some refactoring
    string counters_path;
    if (!m_pn_directory.empty()) {
        counters_path = m_pn_directory;
    } else {
        counters_path = ".";
    }
    counters_path += "/counter";
    if (!Counters::Init(counters_path.c_str())) {
        DSTREAM_ERROR("failed to init counters at %s", counters_path.c_str());
        return;
    }

    // init task counters
    // TODO(konghui@baidu.com): a better way is to init task counters in init
    if (!m_counters.Init(m_id, m_as_importer)) {
        DSTREAM_ERROR("failed to init task counters");
        return;
    }

    // --------------------
    // after read env, we initialize log again with new parameter.
    // logger::initialize(m_name.c_str(), m_log_level);

    // ---------------------
    // call init to customize
    init();
    Init();

    if (use_loopback) {
        // set node uri back.
        if (m_node_uri != kDefaultNodeUri) {
            DSTREAM_WARN("node_uri!='%s',reset!!!", kDefaultNodeUri);
            m_node_uri = kDefaultNodeUri;
        }
    }

    // init kylin, add by konghui
    if (!use_loopback) {
        if (!StartKylin()) {
            DSTREAM_WARN("fail to start kylin");
            return;
        }
    }

    int ret = StartTransporter();
    if (ret < error::OK) {
        DSTREAM_WARN("fail to start pe transporter:%s", error::ErrStr(ret));
        return;
    }

    // --------------------
    // start heartbeat thread.
    DSTREAM_INFO("start heartbeat thread");
    thread_ret = pthread_create(&heartbeat_tid, NULL, proxy_heartbeat_thread_function, this);
    if (thread_ret != 0) {
        DSTREAM_WARN("pthread_create failed(%s)", strerror(thread_ret));
        return;
    }

    // --------------------
    // start ping thread.
    DSTREAM_INFO("start ping thread");
    thread_ret = pthread_create(&ping_tid, NULL, proxy_ping_thread_function, this);
    if (thread_ret != 0) {
        DSTREAM_WARN("pthread_create failed(%s)", strerror(thread_ret));
        return;
    }

    if (m_as_importer) {
        // --------------------
        // start importer fetch commit log thread.
        for (CLNameEntityMap::iterator it = m_cl_name_entity_map.begin();
             it != m_cl_name_entity_map.end(); ++it) {
            TaskCommitLog* tc = new TaskCommitLog();
            tc->task = this;
            tc->commit_name = it->first;
            thread_ret = pthread_create(&importer_log_tid, NULL,
                                        proxy_importer_thread_function,
                                        static_cast<void*>(tc));
            if (thread_ret != 0) {
                DSTREAM_WARN("pthread_create failed(%s)", strerror(thread_ret));
                return;
            }

            if (m_switch_of_progress_persist) {
                thread_ret = pthread_create(&importer_progress_tid,
                                            NULL,
                                            proxy_importer_progress_thread_function,
                                            this);
                if (0 != thread_ret) {
                    DSTREAM_WARN("pthread_create failed(%s)", strerror(thread_ret));
                    return;
                }
            }
        }
    }

    // -----------------------------
    // fetch log and continue working.
    DSTREAM_INFO("start processing...");
    // uint64_t last = 0;
    uint64_t empty_count = 0;
    while (1) {
        if (m_quit) {
            DSTREAM_INFO("Task::run() exit");
            break;
        }

        if (m_term_recvived) {
            DSTREAM_INFO("task get terminate signal, prepare to quit.");
            m_terminating = true;
        }

        int ret = error::OK;
        if (m_as_importer) {
            ret = ImporterWorkflow();
            if (ret != error::OK) {
                empty_count++;
                if (empty_count % 1000 == 0) {
                    DSTREAM_INFO("importerQ empty, count(%lu)", empty_count);
                }
                usleep(100); // 100us
            }
        } else {
            ret = NormalWorkflow();
        }

        // do user space clean up
        if (ReadyToQuit()) {
            onStop(PEET_STOP_DUE_TO_UPD);
            OnStop(PEET_STOP_DUE_TO_UPD);
        }

        if (m_as_exporter && m_pe_type == PE_TYPE_BIGPIPE_EXPORTER) {
            // handle bigpipe exporter
            int ret_status = ForwardLogToBigpipe();
            if (ret_status != error::OK) {
                DSTREAM_ERROR("forward log to bigpipe failed. error code(%d)",  ret_status);
                // unrecoverable error occured, better to exit
                m_quit = true;
            }
        } else {
            // keep trying to forward log until all tuples ard sent in batch
            do {
                ret = ForwardLog();
                if (LIKELY(error::OK == ret)) {
                    break;
                } else if (error::OK_REMAIN_DATA == ret) {
                    continue;
                } else {
                    DSTREAM_WARN("ForwardLog failed: %s", error::ErrStr(ret));
                    break;
                }
            } while (!m_quit);
        }

        if (ReadyToQuit()) {
            DSTREAM_INFO("clean up done, quiting...");
            break;
        }
    } // while(1)
    DSTREAM_WARN("task.run() over.");

    // ------------------------------
    if (use_loopback) {
        pthread_cancel(loopback_tid);
        pthread_join(loopback_tid, NULL);
    }
    pthread_cancel(heartbeat_tid);
    pthread_join(heartbeat_tid, NULL);
    if (m_as_importer) {
        pthread_cancel(importer_log_tid);
        pthread_join(importer_log_tid, NULL);
    }
    if (m_as_importer && m_switch_of_progress_persist) {
        pthread_cancel(importer_progress_tid);
        pthread_join(importer_progress_tid, NULL);
    }

    // ------------------------------
    DSTREAM_WARN("PE main thread exit.");
    // call close interface
    close();
    Close();
    // kill all descendants (process)
    kill(0, SIGKILL);

    // deinit counters
    // TODO(konghui@baidu.com): do this in main()
    if (!Counters::Deinit()) {
        DSTREAM_WARN("failed to deinit counters");
    }
}

// Register a counter with the given name.
Counter* Task::GetCounter(const std::string& count_name) {
    Counter* counter = NULL;

    {
        MutexLockGuard lock_guard(m_counter_map_lock);
        for (size_t i = 0; i < m_counter_map.size(); i++) {
            if (strcmp(m_counter_map[i]->name()->c_str(), count_name.c_str()) == 0) {
                DSTREAM_WARN("[%s] counter<%s> is already existed!",
                                __FUNCTION__, count_name.c_str());
                counter = m_counter_map[i];
                return counter;
            }
        }
    }

    AtomicAdd64(&m_counter_id, 1);
    // maybe master handles all counters?

    counter = new Counter(m_counter_id, count_name);

    {
        MutexLockGuard lock_guard(m_counter_map_lock);
        m_counter_map[counter->id()] = counter;
    }
    return counter;
}

// Increment the value of the counter with the given amount.
void Task::IncrementCounter(const Counter* counter, uint64_t amount) {
    // accumulate counters
    int count_id = counter->id();
    {
        MutexLockGuard lock_guard(m_counter_map_lock);
        m_counter_map[count_id]->AddAmount(amount);
    }
}

void Task::RegisterTupleTypeInfo(const std::string& tag, TupleTypeInfo* info) {
    return LoopBack::RegisterTupleTypeInfo(tag, info);
}

void Task::Quit() {
    AtomicSetValue(m_quit, 1);
    StopKylin();
}

static void sig_term_handler(int signo) {
    Task::instance()->SetToQuit();
}

void Task::SetSignalHandler() {
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    act.sa_handler = sig_term_handler;
    if (sigaction(SIGTERM, &act, NULL) == -1) {
        DSTREAM_ERROR("install SIGTERM handler error: %s", strerror(errno));
        _exit(error::INSTALL_SIGNAL_HANDLER_FAIL);
    }
    DSTREAM_INFO("install SIGTERM handler OK.");
}

inline void Task::SetToQuit() {
    m_term_recvived = true;
    DSTREAM_INFO("set task terminating flag.");
}

inline bool Task::ReadyToQuit() {
    if (m_as_importer) {
        return m_terminating && m_importer_log_queue->Empty();
    } else {
        return m_terminating;
    }
    return false;
}

void DumpTuples(const std::vector< const Tuple* >& tuples, std::string* s) {
    *s += "[";
    StringAppendNumber(s, tuples.size());
    *s += ":[";
    for (size_t i = 0; i < tuples.size(); i++) {
        const Tuple* tuple = tuples[i];
        if (i) {
            s->append(", ");
        }
        *s += "[";
        char ptr_str[64] = {0};
        snprintf(ptr_str, sizeof(ptr_str), "%p", tuple);
        *s += ptr_str;
        s->append(", hash=");
        StringAppendNumber(s, tuple->hash_code());
        s->append(", tag=").append(tuple->tag());
        *s += "]";
    }
    *s += "]]";
}

} // namespace processelement
} // namespace dstream
