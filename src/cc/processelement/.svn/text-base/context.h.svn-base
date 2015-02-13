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

#ifndef __DSTREAM_CC_PROCESSELEMENT_CONTEXT_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_CONTEXT_H__ // NOLINT

#include <map>
#include <string>
#include "common/bounded_blocking_queue.h"
#include "common/commit_log.h"
#include "common/proto/application.pb.h"
#include "processelement/bp_progress.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace processelement {
namespace internal {

// ------------------------------------------------------------
// Context Interface.
class Context {
public:
    enum {
        // PE type
        PE_TYPE_NORMAL = 0,
        PE_TYPE_STREAMING,
        PE_TYPE_BIGPIPE_EXPORTER,
        PE_TYPE_SCRIBE_IMPORTER,
    };
    typedef BoundedBlockingQueue< LogInfo > LogQ;

public:
    Context();
    ~Context();
    // ----------------------------------------
    // maybe process node want to know it.
    // some environment keys.
    static const char* kEnvKeyWorkDirectory;
    static const char* kEnvKeyPnDirectory;
    static const char* kEnvKeyNodeUri;
    static const char* kEnvKeyESPUri;
    static const char* kEnvKeyHeartbeatPollTimeoutSeconds;
    static const char* kEnvKeyEachCommitLogPollTimeoutMillSeconds;
    static const char* kEnvKeyProcessElementId;
    static const char* kEnvKeyLogLevel;
    static const char* kEnvKeyProcessElementName;
    static const char* kEnvKeyProcessorRole;
    static const char* kEnvKeyProcessorStatus;
    static const char* kEnvKeyDynamicProtoDir;
    static const char* kEnvKeyPEType;

    // for transporter
    static const char* kEnvKeyTuplesBatchCount;
    static const char* kEnvKeyTuplesBatchSize;
    static const char* kEnvKeyForceForwardTuplesPeriodMillSeconds;
    static const char* kEnvKeySendLogTimeoutMillSeconds;
    static const char* kEnvKeyFetchLogTimeoutMillSeconds;
    static const char* kEnvKeyExecWorkers;
    static const char* kEnvKeyNetWorkers;
    static const char* kEnvKeyDiskWorkers;
    static const char* kEnvKeyBufSmallTranBufNum;
    static const char* kEnvKeyBufSmallTranBufSize;
    static const char* kEnvKeyBufBigTranBufNum;
    static const char* kEnvKeyBufLowMark;
    static const char* kEnvKeyAppName;

    // for pe importer
    static const char* kCommitLogPrefix;
    static const char* kEnvKeyCommitLogNumber;
    static const char* kEnvKeyCommitLogType;
    static const char* kEnvKeyCommitLogBPName;
    static const char* kEnvKeyCommitLogBPPipelet;
    static const char* kEnvKeyCommitLogBPConfPath;
    static const char* kEnvKeyCommitLogBPConfName;
    static const char* kEnvKeyCommitLogBPUserName;
    static const char* kEnvKeyCommitLogBPToken;
    static const char* kEnvKeyCommitLogLFSName;
    static const char* kEnvKeyCommitLogLFSSubPoint;
    static const char* kEnvKeyCommitLogScribeBuffLen;
    static const char* kEnvKeyCommitLogFakeLogLen;
    static const char* kEnvKeySpoutLimit;
    static const char* kConfKeyImporterQueueSize;
    static const char* kConfKeyCommitLogLFS;
    static const char* kConfKeyCommitLogLFSReadMethod;
    static const char* kCommitLogLFSReadMethodLoopRead;
    static const char* kCommitLogLFSReadMethodReadToEnd;

    // for pe bigpipe exporter
    static const char* kEnvKeyExporterType;
    static const char* kEnvKeyExporterPubBigpipePipeName;
    static const char* kEnvKeyExporterPubBigpipePubToken;
    static const char* kEnvKeyExporterPubBigpipePubUser;
    static const char* kEnvKeyExporterPubBigpipePipeletNum;
    static const char* kEnvKeyExporterPubBigpipeRetryTime;
    static const char* kEnvKeyExporterPubBigpipeRetryInterval;
    static const char* kEnvKeyExporterPubBigpipeDefaultRetryTime;
    static const char* kEnvKeyExporterPubBigpipeDefaultRetryInterval;

    // for importer pe persist progress
    static const char* kEnvKeySwitchOfProgressPersist;

    // for Context Key
    static const char* kCtxImporterPipeName;
    static const char* kCtxImporterPipeletID;
    static const char* kCtxImporterMessageID;
    static const char* kCtxImporterSequenceID;
    static const char* kCtxImporterRecvTime;

    // some info or default values.
    static const char* kTaskVersion;
    static const char* kDefaultWorkDirectory;
    static const char* kDefaultNodeUri;
    static const char* kDefaultImporterUri;
    static const char* kLogLevelDebug;
    static const char* kLogLevelInfo;
    static const char* kLogLevelWarning;
    static const int   kDefaultLogLevel;
    static const int   kDefaultForwardPipeId = 0;
    static const char* kCommitLogBPType;
    static const char* kDefaultCommitLogBPName;
    static const char* kDefaultCommitLogBPConfPath;
    static const char* kDefaultCommitLogBPConfName;
    static const char* kDefaultCommitLogBPToken;
    static const char* kEnvKeyCommitLogBPSubPoint;
    static const char* kCommitLogLFSType;
    static const char* kDefaultCommitLogLFSName;
    static const char* kCommitLogFakeType;
    static const char* kCommitLogScribeType;
    static const int   kDefaultCommitLogScribeBuffLen = 134217728;
    static const char* kWorkDirectoryStartup;
    static const char* kWorkDirectoryCheckpoint;
    static const char* kRoleAsImporter;
    static const char* kRoleAsExporter;
    static const char* kRoleAsWorker;
    static const char* kCheckpointDataFilename;
    static const char* kCheckpointDescFilename;
    static const char* kDefaultProcessElementId;
    static const char* kDefaultProcessElementName;
    // interval of heartbeat and report metrics.
    static const int kHeartbeatPollTimeoutSeconds = 5;
    // interval of each commit log poll
    static const int kEachCommitLogPollTimeoutMillSeconds = 10;
    // importer queue maxsize.
    static const int kImporterQueueMaxSize = 1024;
    // fetch log timeout.
    static const int kFetchLogTimeoutMillSeconds = 10000;
    static const int kSendLogTimeoutMillSeconds  = 20000;
    // sub log timeout.
    static const int kSubscribeTimeoutMillSeconds = 3000;
    // interval of check parent alive.
    static const int    kPingTimeoutSeconds = 1;
    static const char*  kDefaultHasStatus;
    static const int    kDefaultForceForwardTuplesPeriodMillSeconds = 1000; // 1s
    static const char*  kDefaultDynamicProtoDir;
    static const char*  kDefaultFlowControl;
    static const int    kDefaultCommitLogPollMillSeconds = 200;         // 200ms.
    static const int    kDefaultHeartBeatTimeoutMillSeconds = 5000;     // 1000ms.
    static const uint64_t kDefaultSubPoint = static_cast<uint64_t>(-1); // sub from latest data
    static const uint64_t kDefaultPEType = static_cast<uint64_t>(Context::PE_TYPE_NORMAL);
    static const char*  kStreamingKVTagName;
    static const char*  kPipeDelimiter;

public:
    /** 
    * @brief    set worker ruler
    */
    static void WorkAsImporter();
    static void WorkAsExporter();
    static void WorkAsWorker();

    /** 
    * @brief    find commit log
    * @param    name commit log name
    * @return   commit log handler
    */
    CommitLog* findCommitLog(const std::string& name) const;
    CommitLog* findCommitLog(uint64_t id) const;

    // kv context interface
    int Set(const std::string& key, const std::string& value);
    int Set(const std::string& key, const std::string& value, int index);
    int Get(const std::string& key, std::string* value) const;
    int Get(const std::string& key, std::string* value, int index) const;

    /** 
    * @brief    update process subpoint
    * @param    point subpoint
    */
    inline int32_t UpdateProgress(const SubPoint& point) { return m_sub_progress.update(point); }

protected:
    /** 
    * @brief    has importer touch the holding threshold
    * @return   yes: true; no: false;
    */
    bool ImporterReachHoldingThreshold();
    /** 
    * @brief    set loopback env
    * @return   OK: true; fail: false;
    */
    bool SetupLoopBackEnvironment();
    /** 
    * @brief    importer env
    * @return   OK: true; fail: false;
    */
    bool ReadEnvironment();

    uint64_t id() const { return m_id; }
    const std::string& name() const { return m_name; }

protected:
    uint64_t        m_id;
    std::string     m_name;
    std::string     m_app_name;
    bool            m_as_importer;
    bool            m_as_exporter;
    uint64_t        m_pe_type; // pe type, e.g. STREAMING_PE_TYPE
    int             m_log_level;
    bool            m_has_status;

    int             m_tuples_batch_count;
    int             m_tuples_batch_size;
    int64_t         m_importer_tuple_holding_size;
    int64_t         m_importer_tuple_holding_count;
    size_t          m_force_forward_tuples_period_ms;
    int             m_fetch_log_timeout_ms;
    int             m_send_log_timeout_ms;
    size_t          m_last_send_tuples_timestamp;

    // default is kCheckpointPollTimeoutSeconds
    int             m_chkp_poll_timeout_sec;
    // default is kHeartBeatPollTimeoutSeconds
    int             m_heartbeat_poll_timeout_sec;
    // default is kEachCommitLogPollTimeoutMillSeconds
    int             m_each_commit_log_poll_timeout_ms;

    int32_t         m_spout_limit;
    int32_t         m_spout_limit_value; // spout limit per second, flow control
    // CommitLog exists in map and vector
    // but just delete map at the end.
    typedef std::map<std::string, CommitLog*> CLNameEntityMap;
    typedef std::map<int, CommitLog*> CLIdEntityMap;
    CLNameEntityMap m_cl_name_entity_map;
    CLIdEntityMap   m_cl_id_entity_map;

    // --------------------
    // fields read from environment.
    std::string     m_dynamic_proto_dir; // dynamic proto dir
    std::string     m_work_directory;
    std::string     m_pn_directory;
    std::string     m_node_uri;
    std::string     m_pn_ipport;
    std::string     m_importer_uri;

    // KV context, can be modified from outside
    typedef std::map<std::string, std::string> KVContextMap;
    typedef std::map<std::string, void* /*keep for extent*/> KeySet;
    KVContextMap    m_kv_context;
    KeySet          m_key_set;

    // bigpipe subscribe progress
    BPProgress      m_sub_progress;

    // scribe importer zk register
    ScribeRegister      m_scribe_register;

    // for importer pe
    LogQ*           m_importer_log_queue;
    uint32_t        m_importer_log_queue_size;

    // for bigpipe exporter
    uint32_t        m_pipelet_num;
    std::string     m_pipe_name;
    uint32_t        m_bigpipe_retry_time;
    uint32_t        m_bigpipe_retry_interval;
    CommitLog*      m_publisher;

    // for bigpipe importer
    bool            m_switch_of_progress_persist;  // default is kSwitchOfProgressPersist

//-----------------------------------------------------------------
// !!!!! Abandoned interfaces
public:
    int32_t update_progress(const SubPoint& point) {
        return UpdateProgress(point);
    }
    inline static void workAsImporter() { return WorkAsImporter(); }
    inline static void workAsExporter() { return WorkAsExporter(); }
    inline static void workAsWorker()   { return WorkAsWorker(); }

    inline int set(std::string key, std::string value) {
        return set(key, value, -1);
    }
    inline int set(std::string key, std::string value, int index) {
        return Set(key, value, index);
    }
    inline int get(std::string key, std::string& value) {
        return get(key, value, -1);
    }
    inline int get(std::string key, std::string& value, int index) {
        return Get(key, &value, index);
    }

protected:
    inline bool importer_reach_holding_threshold() {
        return ImporterReachHoldingThreshold();
    }

    inline bool setupLoopBackEnvironment() {
        return SetupLoopBackEnvironment();
    }
    inline bool readEnvironment() {
        return ReadEnvironment();
    }
}; // class Context

} // namespace internal
} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_CONTEXT_H__ NOLINT
