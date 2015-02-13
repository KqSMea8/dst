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

#include "processelement/context.h"
#include <cerrno>
#include <cstdlib>
#include <vector>
#include "com_log.h"
#include "common/application_tools.h"
#include "common/commit_log.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/logger.h"
#include "transporter/transporter_common.h"

namespace dstream {
namespace processelement {
namespace internal {

// ------------------------------------------------------------
// Context Implementation

// common config
const char* Context::kEnvKeyProcessElementId = "dstream.pe.id";
const char* Context::kEnvKeyProcessElementName = "dstream.pe.name";
const char* Context::kEnvKeyPEType = "dstream.pe.type";
const char* Context::kEnvKeyLogLevel = "dstream.pe.logLevel";
const char* Context::kEnvKeyProcessorRole = "dstream.pe.role";
const char* Context::kEnvKeyWorkDirectory = "dstream.pe.workDirectory";
const char* Context::kEnvKeyPnDirectory = "dstream.pe.pnDirectory";
const char* Context::kEnvKeyNodeUri = "dstream.pe.nodeUri";
const char* Context::kEnvKeyESPUri = "dstream.pe.ESPUri";
const char* Context::kEnvKeyProcessorStatus = "dstream.pe.hasStatus";
const char* Context::kEnvKeyDynamicProtoDir = "dstream.pe.dynamicProtoDir";
const char* Context::kEnvKeyHeartbeatPollTimeoutSeconds = "dstream.pe.heartbeatPollTimeoutSeconds";
// config for transporter
const char* Context::kEnvKeySendLogTimeoutMillSeconds = "dstream.pe.sendTimeoutMillSeconds";
const char* Context::kEnvKeyFetchLogTimeoutMillSeconds = "dstream.pe.fetchTimeoutMillSeconds";
const char* Context::kEnvKeyTuplesBatchCount = "dstream.pe.tuplesBatchCount";
const char* Context::kEnvKeyTuplesBatchSize = "dstream.pe.tuplesBatchSize";
const char* Context::kEnvKeyForceForwardTuplesPeriodMillSeconds =
    "dstream.pe.forceForwardTuplesPeriodMillSeconds";
// configs of kylin
const char* Context::kEnvKeyExecWorkers = "dstream.pe.execWorkers";
const char* Context::kEnvKeyNetWorkers = "dstream.pe.netWorkers";
const char* Context::kEnvKeyDiskWorkers = "dstream.pe.diskWorkers";
const char* Context::kEnvKeyBufSmallTranBufNum = "dstream.pe.bufSmallTranBufNum";
const char* Context::kEnvKeyBufSmallTranBufSize = "dstream.pe.bufSmallTranBufSize";
const char* Context::kEnvKeyBufBigTranBufNum = "dstream.pe.bufBigTranBufNum";
const char* Context::kEnvKeyBufLowMark = "dstream.pe.bufLowMark";
const char* Context::kEnvKeyAppName = "dstream.pe.appName";

// config for importer
const char* Context::kEnvKeyEachCommitLogPollTimeoutMillSeconds =
    "dstream.pe.eachCommitLogPollTimeoutMillSeconds";
const char* Context::kCommitLogPrefix = "dstream.pe.commitLog.";
const char* Context::kEnvKeyCommitLogNumber = "number";
const char* Context::kEnvKeyCommitLogType = "type";
const char* Context::kEnvKeyCommitLogBPName = "bigpipe.name";
const char* Context::kEnvKeyCommitLogBPPipelet = "bigpipe.pipeletId";
const char* Context::kEnvKeyCommitLogBPConfPath = "bigpipe.confpath";
const char* Context::kEnvKeyCommitLogBPConfName = "bigpipe.confname";
const char* Context::kEnvKeyCommitLogBPSubPoint = "bigpipe.subpoint";
const char* Context::kEnvKeyCommitLogBPUserName = "bigpipe.username";
const char* Context::kEnvKeyCommitLogBPToken = "bigpipe.token";
const char* Context::kEnvKeyCommitLogLFSName = "localfs.name";
const char* Context::kEnvKeyCommitLogLFSSubPoint = "localfs.subpoint";
const char* Context::kEnvKeyCommitLogScribeBuffLen = "scribe.buffer";
const char* Context::kEnvKeyCommitLogFakeLogLen = "fake.loglen";
const char* Context::kEnvKeySpoutLimit = "dstream.importer.spoutlimit";
const char* Context::kConfKeyImporterQueueSize = "ImporterQueueSize";
const char* Context::kConfKeyCommitLogLFS = "LocalFile";
const char* Context::kConfKeyCommitLogLFSReadMethod = "ReadMethod";
const char* Context::kCommitLogLFSReadMethodLoopRead = "ReadLoop";
const char* Context::kCommitLogLFSReadMethodReadToEnd = "ReadToEnd";

// for pe bigpipe exporter
const char* Context::kEnvKeyExporterType = "dstream.exporter.type";
const char* Context::kEnvKeyExporterPubBigpipePipeName = "exporter.bigpipe.PipeName";
const char* Context::kEnvKeyExporterPubBigpipePubToken = "exporter.bigpipe.PubToken";
const char* Context::kEnvKeyExporterPubBigpipePubUser = "exporter.bigpipe.PubUser";
const char* Context::kEnvKeyExporterPubBigpipePipeletNum = "exporter.bigpipe.PipeletNum";
const char* Context::kEnvKeyExporterPubBigpipeRetryTime = "exporter.bigpipe.retry_time";
const char* Context::kEnvKeyExporterPubBigpipeRetryInterval = "exporter.bigpipe.retry_interval";
const char* Context::kEnvKeyExporterPubBigpipeDefaultRetryTime = "3";
const char* Context::kEnvKeyExporterPubBigpipeDefaultRetryInterval = "50000"; // 50ms

// for importer pe persist progress
const char* Context::kEnvKeySwitchOfProgressPersist = "importer.bp_progress.switch";

// for Context Key
const char* Context::kCtxImporterPipeName = "pipe-name";
const char* Context::kCtxImporterPipeletID = "pipelet-id";
const char* Context::kCtxImporterMessageID = "message-id";
const char* Context::kCtxImporterSequenceID = "sequence-id";
const char* Context::kCtxImporterRecvTime = "recv-time";

const char* Context::kTaskVersion = "dstream.pe.c++.1.0.0";
const char* Context::kDefaultWorkDirectory = ".";
const char* Context::kDefaultNodeUri = "ipc://dstream.pe.loopback";
const char* Context::kDefaultImporterUri = "ipc://dstream.pe.importer";
const char* Context::kLogLevelInfo = "INFO";
const char* Context::kLogLevelDebug = "DEBUG";
const char* Context::kLogLevelWarning = "WARNING";
const int Context::kDefaultLogLevel = log4cplus::DEBUG_LOG_LEVEL;
const char* Context::kCommitLogBPType = "bigpipe";
const char* Context::kDefaultCommitLogBPName = "demo.bigpipe";
const char* Context::kDefaultCommitLogBPConfPath = "./conf";
const char* Context::kDefaultCommitLogBPConfName = "./bigpipe.conf";
const char* Context::kDefaultCommitLogBPToken = "_default_token_";
const char* Context::kCommitLogLFSType = "localfs";
const char* Context::kDefaultCommitLogLFSName = "./localfs";
const char* Context::kCommitLogFakeType = "fake";
const char* Context::kCommitLogScribeType = "scribe";
const char* Context::kWorkDirectoryStartup = "dstream.pe.startup";
const char* Context::kWorkDirectoryCheckpoint = "dstream.pe.checkpoint";
const char* Context::kDefaultDynamicProtoDir = ".";
const char* Context::kRoleAsImporter = "importer";
const char* Context::kDefaultFlowControl = "flow";
const char* Context::kRoleAsExporter = "exporter";
const char* Context::kRoleAsWorker = "worker";
const char* Context::kDefaultHasStatus = "0";

const char* Context::kCheckpointDataFilename = "statustable.data";
const char* Context::kCheckpointDescFilename = "description.txt";
const char* Context::kDefaultProcessElementId = "2012";
const char* Context::kDefaultProcessElementName = "demo.pe";
const char* Context::kStreamingKVTagName = "StreamingKVType";
const char* Context::kPipeDelimiter = "###";


Context::Context()
        : m_name(kDefaultProcessElementName),
        m_as_importer(false),
        m_as_exporter(false),
        m_pe_type(static_cast<uint64_t>(kDefaultPEType)),
        m_log_level(kDefaultLogLevel),
        m_tuples_batch_count(config_const::kPNTuplesBatchNum),
        m_tuples_batch_size(config_const::kPNTuplesBatchSizeByte),
        m_importer_tuple_holding_size(0),
        m_importer_tuple_holding_count(0),
        m_force_forward_tuples_period_ms(kDefaultForceForwardTuplesPeriodMillSeconds),
        m_fetch_log_timeout_ms(kFetchLogTimeoutMillSeconds),
        m_send_log_timeout_ms(kSendLogTimeoutMillSeconds),
        m_last_send_tuples_timestamp(0),
        m_heartbeat_poll_timeout_sec(kHeartbeatPollTimeoutSeconds),
        m_each_commit_log_poll_timeout_ms(kEachCommitLogPollTimeoutMillSeconds),
        m_dynamic_proto_dir(kDefaultDynamicProtoDir),
        m_importer_log_queue(NULL),
        m_importer_log_queue_size(kImporterQueueMaxSize) {
    // set current supported context key
    m_key_set[kCtxImporterPipeName]   = NULL;
    m_key_set[kCtxImporterPipeletID]  = NULL;
    m_key_set[kCtxImporterMessageID]  = NULL;
    m_key_set[kCtxImporterSequenceID] = NULL;
    m_key_set[kCtxImporterRecvTime]   = NULL;
}

Context::~Context() {
    // deallocate commit log.
    for (CLNameEntityMap::iterator it = m_cl_name_entity_map.begin();
         it != m_cl_name_entity_map.end(); ++it) {
        delete it->second;
    }

    if (m_importer_log_queue) {
        delete m_importer_log_queue;
        m_importer_log_queue = NULL;
    }
}

bool Context::SetupLoopBackEnvironment() {
    return true;
}

bool Context::ReadEnvironment() {
#define W(env) DSTREAM_WARN("have you forgot to set '%s'?", env)
#define I(env, fmt, value) DSTREAM_INFO("%s = "fmt, env, value)
    const char* value = NULL;

    // Print Pesdk Version
#ifdef VERSION
    DSTREAM_INFO("dstream.pe.version = %s ", VERSION);
#else
    DSTREAM_WARN("failed to get dstream.pe.version");
#endif

    // --------------------
    value = getenv(kEnvKeyPEType);
    if (value) {
        I(kEnvKeyPEType, "%s", value);
        if (strcmp(value, "PE_TYPE_STREAMING") == 0) {
            m_pe_type = Context::PE_TYPE_STREAMING;
        } else {
            W(kEnvKeyPEType);
            return false;
        }
    }

    // --------------------
    value = getenv(kEnvKeyWorkDirectory);
    if (!value) {
        W(kEnvKeyWorkDirectory);
        return false;
    }
    I(kEnvKeyWorkDirectory, "%s", value);
    m_work_directory = value;

    // --------------------
    value = getenv(kEnvKeyPnDirectory);
    if (!value) {
        W(kEnvKeyPnDirectory);
        return false;
    }
    I(kEnvKeyPnDirectory, "%s", value);
    m_pn_directory = value;

    // --------------------
    value = getenv(kEnvKeyNodeUri);
    if (!value) {
        W(kEnvKeyNodeUri);
        return false;
    }
    I(kEnvKeyNodeUri, "%s", value);
    m_node_uri = value;

    // --------------------
    value = getenv(kEnvKeyESPUri);
    if (!value) {
        W(kEnvKeyESPUri);
        return false;
    }
    I(kEnvKeyESPUri, "%s", value);
    m_pn_ipport = value;

    // --------------------
    value = getenv(kEnvKeyProcessElementId);
    if (!value) {
        W(kEnvKeyProcessElementId);
        return false;
    }
    // pe id may be very large.
    m_id = strtoull(value, NULL, 10);
    // m_id = atoi(value);
    I(kEnvKeyProcessElementId, "%lu", static_cast<uint64_t>(m_id));

    // --------------------
    value = getenv(kEnvKeyHeartbeatPollTimeoutSeconds);
    if (value) {
        int v = atoi(value);
        m_heartbeat_poll_timeout_sec = v;
    }
    I(kEnvKeyHeartbeatPollTimeoutSeconds, "%d", m_heartbeat_poll_timeout_sec);

    // --------------------
    value = getenv(kEnvKeyEachCommitLogPollTimeoutMillSeconds);
    if (value) {
        int v = atoi(value);
        m_each_commit_log_poll_timeout_ms = v;
    }
    I(kEnvKeyEachCommitLogPollTimeoutMillSeconds, "%d", m_each_commit_log_poll_timeout_ms);

    // --------------------
    value = getenv(kEnvKeyLogLevel);
    if (value) {
        if (strcmp(value, kLogLevelInfo) == 0) {
            m_log_level = log4cplus::INFO_LOG_LEVEL;
            I(kEnvKeyLogLevel, "%s", kLogLevelInfo);
        } else if (strcmp(value, kLogLevelDebug) == 0) {
            m_log_level = log4cplus::DEBUG_LOG_LEVEL;
            I(kEnvKeyLogLevel, "%s", kLogLevelDebug);
        } else if (strcmp(value, kLogLevelWarning) == 0) {
            m_log_level = log4cplus::WARN_LOG_LEVEL;
            I(kEnvKeyLogLevel, "%s", kLogLevelWarning);
        } else {
            DSTREAM_WARN("unknown log level:'%s'", value);
            return false;
        }
    } else {
        DSTREAM_INFO("log level=%s", kLogLevelDebug);
    }

    value = getenv(kEnvKeyFetchLogTimeoutMillSeconds);
    if (value) {
        int64_t v = atol(value);
        m_fetch_log_timeout_ms = v;
    }
    I(kEnvKeyFetchLogTimeoutMillSeconds, "%d", m_fetch_log_timeout_ms);

    value = getenv(kEnvKeySendLogTimeoutMillSeconds);
    if (value) {
        int64_t v = atol(value);
        m_send_log_timeout_ms = v;
    }
    I(kEnvKeySendLogTimeoutMillSeconds, "%d", m_send_log_timeout_ms);

    // --------------------
    value = getenv(kEnvKeyProcessElementName);
    if (!value) {
        W(kEnvKeyProcessElementName);
        return false;
    } else {
        I(kEnvKeyProcessElementName, "%s", value);
    }
    m_name = value;

    value = getenv(kEnvKeyProcessorRole);
    if (!value) {
        W(kEnvKeyProcessorRole);
        return false;
    } else {
        I(kEnvKeyProcessorRole, "%s", value);
    }

    if (strcmp(value, kRoleAsImporter) == 0) {
        m_as_importer = true;
    } else if (strcmp(value, kRoleAsExporter) == 0) {
        m_as_exporter = true;
    }

    value = getenv(kEnvKeyProcessorStatus);
    if (value) {
        if (!value || strcmp(value, kDefaultHasStatus) == 0) {
            m_has_status = false;
        } else {
            m_has_status = true;
        }
    }

    value = getenv(kEnvKeyDynamicProtoDir);
    if (value) {
        m_dynamic_proto_dir = value;
    }
    I(kEnvKeyDynamicProtoDir, "%s", m_dynamic_proto_dir.c_str());

    // transporter part
    value = getenv(kEnvKeyTuplesBatchCount);
    if (value) {
        int v = atoi(value);
        m_tuples_batch_count = v;
    }
    I(kEnvKeyTuplesBatchCount, "%d", m_tuples_batch_count);

    value = getenv(kEnvKeyTuplesBatchSize);
    if (value) {
        int v = atoi(value);
        m_tuples_batch_size = v;
    }
    I(kEnvKeyTuplesBatchSize, "%d", m_tuples_batch_size);

    value = getenv(kEnvKeyDynamicProtoDir);
    if (value) {
        m_dynamic_proto_dir = value;
    }
    I(kEnvKeyDynamicProtoDir, "%s", m_dynamic_proto_dir.c_str());

    value = getenv(kEnvKeyExecWorkers);
    if (value) {
        transporter::g_kKylinExecWorkers = atoi(value);
    }
    I(kEnvKeyExecWorkers, "%d", transporter::g_kKylinExecWorkers);

    value = getenv(kEnvKeyNetWorkers);
    if (value) {
        transporter::g_kKylinNetWorkers = atoi(value);
    }
    I(kEnvKeyNetWorkers, "%d", transporter::g_kKylinNetWorkers);

    value = getenv(kEnvKeyDiskWorkers);
    if (value) {
        transporter::g_kKylinDiskWorkers = atoi(value);
    }
    I(kEnvKeyDiskWorkers, "%d", transporter::g_kKylinDiskWorkers);

    value = getenv(kEnvKeyBufSmallTranBufNum);
    if (value) {
        transporter::g_kKylinBufSmallTranBufNum = atoi(value);
    }
    I(kEnvKeyBufSmallTranBufNum, "%d", transporter::g_kKylinBufSmallTranBufNum);

    value = getenv(kEnvKeyBufSmallTranBufSize);
    if (value) {
        transporter::g_kKylinBufSmallTranBufSize = atoi(value);
    }
    I(kEnvKeyBufSmallTranBufSize, "%d", transporter::g_kKylinBufSmallTranBufSize);

    value = getenv(kEnvKeyBufBigTranBufNum);
    if (value) {
        transporter::g_kKylinBufBigTranBufNum = atoi(value);
    }
    I(kEnvKeyBufBigTranBufNum, "%d MB", transporter::g_kKylinBufBigTranBufNum);

    value = getenv(kEnvKeyBufLowMark);
    if (value) {
        transporter::g_kKylinBufLowMark = atof(value);
    }
    I(kEnvKeyBufLowMark, "%f", transporter::g_kKylinBufLowMark);

    // read application name
    value = getenv(kEnvKeyAppName);
    if (value) {
        m_app_name = value;
        I(kEnvKeyAppName, "%s", m_app_name.c_str());
    } else {
        W(kEnvKeyAppName);
        return false;
    }

#define R(k) do {                               \
        key = prefix + k;                       \
        value = getenv(key.c_str());            \
        if (!value) {                           \
            W(key.c_str());                     \
            return false;                       \
        }                                       \
        I(key.c_str(), "%s", value);            \
    } while (0);

    // --------------------
    // importer parts.
    // --------------------
#ifndef COMPILE_WITHOUT_COMMIT_LOG
    if (m_as_importer) {
        // whether to persisit progress
        value = getenv(kEnvKeySwitchOfProgressPersist);
        if (!value) {
            W(kEnvKeySwitchOfProgressPersist);
            // by default turn off this function
            m_switch_of_progress_persist = false;
        } else {
            if (0 == strcmp(value, "true")) {
                m_switch_of_progress_persist = true;
            } else {
                m_switch_of_progress_persist = false;
            }
        }
        I(kEnvKeySwitchOfProgressPersist, "%s",
          m_switch_of_progress_persist ? "true" : "false");

        // pe conf file, we use m_name + "_sub.xml" hardcode file name;
        std::string pe_config_file = m_name + "_sub.xml";
        if (m_switch_of_progress_persist) {
            if (0 != m_sub_progress.Init(m_id, m_app_name, pe_config_file)) {
                DSTREAM_WARN("m_sub_progress init failed, [id=%lu, con_file=%s]",
                             m_id, pe_config_file.c_str());
                return false;
            }
        }

        // read & set import queue size
        config::Config conf;
        if (conf.ReadConfig(pe_config_file) == error::OK) {
            std::string conf_value;
            if (conf.GetValue(kConfKeyImporterQueueSize, &conf_value) == error::OK) {
                m_importer_log_queue_size = strtoul(conf_value.c_str(), NULL, 10);
            }
        }
        m_importer_log_queue = new(std::nothrow) LogQ(m_importer_log_queue_size);
        assert(m_importer_log_queue != NULL);
        DSTREAM_INFO("set importer_log_queue_size: %u", m_importer_log_queue_size);

        std::string key = kCommitLogPrefix;
        key += kEnvKeyCommitLogNumber;
        value = getenv(key.c_str());
        if (!value) {
            W(key.c_str());
            return false;
        }
        I(key.c_str(), "%s", value);
        int number = atoi(value);
        for (int i = 0; i < number; i++) {
            std::string prefix = kCommitLogPrefix;
            StringAppendNumber(&prefix, i);
            prefix += ".";

            R(kEnvKeyCommitLogType);

            if (strcmp(value, kCommitLogBPType) == 0) {
                std::string pipe_name;
                uint64_t pipelet_id;
                std::string conf_path;
                std::string conf_name;
                std::string username;
                std::string token;

                R(kEnvKeyCommitLogBPName);
                pipe_name = value;
                Set(kCtxImporterPipeName, pipe_name); // set context

                // each pipelet seperated by ':'
                R(kEnvKeyCommitLogBPPipelet);
                I(kEnvKeyCommitLogBPPipelet, "%s", value);

                std::vector<uint32_t> pipelet_list;
                std::vector<std::string> pipelet_list_str;
                application_tools::SplitStringEx(std::string(value), ':', &pipelet_list_str);
                // convert to uint32 pipelet id
                for (std::vector<std::string>::iterator pipelet_list_iter =
                        pipelet_list_str.begin();
                    pipelet_list_iter != pipelet_list_str.end();
                    ++pipelet_list_iter) {
                    int32_t pipelet_id = atoi(pipelet_list_iter->c_str());
                    if (pipelet_id < 0) {
                        continue; // skip empty pipelet
                    }
                    pipelet_list.push_back(pipelet_id);
                }

                R(kEnvKeyCommitLogBPConfPath);
                conf_path = value;

                R(kEnvKeyCommitLogBPConfName);
                conf_name = value;

                R(kEnvKeyCommitLogBPUserName);
                username = value;

                R(kEnvKeyCommitLogBPToken);
                token = value;

                DSTREAM_INFO("pipe info: pipe_name(%s), conf(%s/%s), user(%s), pass(%s)",
                             pipe_name.c_str(), conf_path.c_str(), conf_name.c_str(),
                             username.c_str(), token.c_str());

                CommitLog* cl = new BigPipeCommitLog(CommitLog::kSubscriber, pipe_name,
                                                     conf_path, conf_name,
                                                     username, token);
                // load comlog
                com_loadlog(conf_path.c_str(), conf_name.c_str());
                // create pipelet name, = pipe_name + "###" + pipelet_id
                std::string pipelet_name = pipe_name + kPipeDelimiter;
                StringAppendNumber(&pipelet_name, static_cast<ssize_t>(pipelet_id));
                m_cl_name_entity_map[pipelet_name] = cl;
                m_cl_id_entity_map[i] = cl;

                std::vector<SubPoint> sub_point_list; // store pipelet subpoint
                if (m_switch_of_progress_persist && (0 == m_sub_progress.load(pipelet_list))) {
                    // get subpoint from zk
                    // TODO(zhenpeng01@baidu.com) error check
                    m_sub_progress.read(&sub_point_list);
                } else {
                    // get subpoint from env OR use default value
                    int64_t sub_point = kDefaultSubPoint;
                    key = prefix + kEnvKeyCommitLogBPSubPoint;
                    value = getenv(key.c_str());
                    if (value) {
                        sub_point = strtoull(value, NULL, 10);
                    }
                    I(key.c_str(), "%lu", sub_point);
                    std::vector<uint32_t>::iterator iter = pipelet_list.begin();
                    while (iter != pipelet_list.end()) {
                        SubPoint point;
                        point.set_pipelet(*iter);
                        point.set_msg_id(sub_point);
                        point.set_seq_id(1);
                        sub_point_list.push_back(point);
                        ++iter;
                    }
                }
                // add pipelet to subscribe api
                std::vector<SubPoint>::iterator sp = sub_point_list.begin();
                while (sp != sub_point_list.end()) {
                    (reinterpret_cast<BigPipeCommitLog*>(cl))->AddPipelet(*sp);
                    DSTREAM_INFO("add pipelet(%u) sub-point(%ld:%lu)",
                                 sp->pipelet(), sp->msg_id(), sp->seq_id());
                    ++sp;
                }
            } else if (strcmp(value, kCommitLogLFSType) == 0) {
                // local fs.
                std::string file_name;
                bool loop_read = false;
                R(kEnvKeyCommitLogLFSName);
                file_name = value;
                // we know that the conf has been initialized,
                // and the config file has also been read.
                if (conf.ReadConfig(pe_config_file) == error::OK) {
                    config::ConfigNode config_node = conf.GetNode(kConfKeyCommitLogLFS);
                    if (!config_node.IsErrorNode()) {
                        std::string conf_value;
                        if (config_node.GetValue(kConfKeyCommitLogLFSReadMethod, &conf_value)
                                == error::OK) {
                            if (strcmp(conf_value.c_str(), kCommitLogLFSReadMethodLoopRead) == 0) {
                                loop_read = true;
                            }
                        } else { // end if of get value LFSReadMethod
                            DSTREAM_WARN("failed to read value [%s]",
                                         kConfKeyCommitLogLFSReadMethod);
                        }
                    } else { // end if of IsErrorNode
                        DSTREAM_WARN("failed to get node [%s]", kConfKeyCommitLogLFS);
                    }
                }

                key = prefix + kConfKeyCommitLogLFSReadMethod;
                if (loop_read) {
                    I(key.c_str(), "%s", kCommitLogLFSReadMethodLoopRead);
                } else {
                    I(key.c_str(), "%s", kCommitLogLFSReadMethodReadToEnd);
                }

                CommitLog* cl = new LocalFileCommitLog(
                    LocalFileCommitLog::kSubscriber,
                    file_name, loop_read);

                m_cl_name_entity_map[file_name] = cl;
                m_cl_id_entity_map[i] = cl;

                key = prefix + kEnvKeyCommitLogLFSSubPoint;
                value = getenv(key.c_str());
                if (value) {
                    cl->set_subpoint(static_cast<uint64_t>(strtoull(value, NULL, 10)));
                    I(key.c_str(), "%s", value);
                }
            } else if (strcmp(value, kCommitLogFakeType) == 0) {
                std::string pipe_name = "fake";
                value = getenv(kEnvKeyCommitLogFakeLogLen);
                CommitLog* cl = NULL;
                if (NULL != value) {
                    I(kEnvKeyCommitLogFakeLogLen, "%s", value);
                    cl = new FakeCommitLog(FakeCommitLog::kPublisher, atoi(value));
                } else {
                    cl = new FakeCommitLog(FakeCommitLog::kPublisher);
                }
                m_cl_name_entity_map[pipe_name] = cl;
                m_cl_id_entity_map[i] = cl;
            } else if (strcmp(value, kCommitLogScribeType) == 0) {
                m_pe_type = PE_TYPE_SCRIBE_IMPORTER;
                std::string pipe_name = "scribe";
                value = getenv(kEnvKeyCommitLogScribeBuffLen);
                CommitLog* cl = NULL;
                if (NULL != value) {
                    I(kEnvKeyCommitLogFakeLogLen, "%s", value);
                    cl = new ScribeCommitLog(CommitLog::kSubscriber, atoi(value));
                } else {
                    cl = new ScribeCommitLog(CommitLog::kSubscriber,
                            kDefaultCommitLogScribeBuffLen);
                }
                m_cl_name_entity_map[pipe_name] = cl;
                m_cl_id_entity_map[i] = cl;
            }
        }

        m_spout_limit_value = INT32_MAX;
        value = getenv(kEnvKeySpoutLimit);
        if (value) {
            m_spout_limit_value = atoi(value);
        } 
        I(kEnvKeySpoutLimit, "%d", m_spout_limit_value);
        AtomicSetValue(m_spout_limit, m_spout_limit_value);
    } else if (m_as_exporter) {
        value = getenv(kEnvKeyExporterType);

        // not NULL and value == bigpipe_type
        if (value && strcmp(value, config_const::kExporterTypeBigpipe) == 0) {
            // run as bigpipe exporter
            m_pe_type = PE_TYPE_BIGPIPE_EXPORTER;

            std::string pipe_name = getenv(kEnvKeyExporterPubBigpipePipeName);
            std::string pub_token = getenv(kEnvKeyExporterPubBigpipePubToken);
            std::string pipelet_num = getenv(kEnvKeyExporterPubBigpipePipeletNum);
            std::string pub_user = getenv(kEnvKeyExporterPubBigpipePubUser);

            m_pipe_name = pipe_name;
            m_pipelet_num = atoi(pipelet_num.c_str());

            m_publisher = new BigPipeCommitLog(BigPipeCommitLog::kPublisher,
                                               pipe_name,
                                               "./conf",
                                               "bigpipe.conf",
                                               pub_user,
                                               pub_token);
            // start bigpipe publisher
            if (m_publisher->Open() != 0) {
                DSTREAM_ERROR("bigpipe commit log for publisher(%s) open failed",
                              pipe_name.c_str());
                return false;
            }
            DSTREAM_INFO("bigpipe commit log for publisher(%s) is running.", pipe_name.c_str());

            std::string retry_time = getenv(kEnvKeyExporterPubBigpipeRetryTime);
            if (retry_time.empty()) {
                retry_time = kEnvKeyExporterPubBigpipeDefaultRetryTime;
            }
            m_bigpipe_retry_time = atoi(retry_time.c_str());

            std::string retry_interval = getenv(kEnvKeyExporterPubBigpipeRetryInterval);
            if (retry_interval.empty()) {
                retry_interval = kEnvKeyExporterPubBigpipeDefaultRetryInterval;
            }
            m_bigpipe_retry_interval = atoi(retry_interval.c_str());

            DSTREAM_INFO("bigpipe exporter, pipename: %s, token: %s, pipelet_num: %d",
                         pipe_name.c_str(), pub_token.c_str(), m_pipelet_num);
            DSTREAM_INFO("bigpipe exporter, retry time: %d, retry interval: %d",
                         m_bigpipe_retry_time, m_bigpipe_retry_interval);
        }
    }
#endif // COMPILE_WITHOUT_COMMIT_LOG
    return true;

#undef R
#undef W
#undef I
}

void Context::WorkAsImporter() {
    setenv(kEnvKeyProcessorRole, kRoleAsImporter, 1);
    DSTREAM_INFO("WorkAsImporter '%s' = '%s'", kEnvKeyProcessorRole, kRoleAsImporter);
}

void Context::WorkAsExporter() {
    setenv(kEnvKeyProcessorRole, kRoleAsExporter, 1);
    DSTREAM_INFO("WorkAsExporter '%s' = '%s'", kEnvKeyProcessorRole, kRoleAsExporter);
}

void Context::WorkAsWorker() {
    setenv(kEnvKeyProcessorRole, kRoleAsWorker, 1);
    DSTREAM_INFO("WorkAsWorker '%s' = '%s'", kEnvKeyProcessorRole, kRoleAsWorker);
}

CommitLog* Context::findCommitLog(const std::string& cl_name) const {
    CLNameEntityMap::const_iterator it = m_cl_name_entity_map.find(cl_name);
    if (it == m_cl_name_entity_map.end()) {
        return NULL;
    }
    return it->second;
}

CommitLog* Context::findCommitLog(uint64_t cl_id) const {
    CLIdEntityMap::const_iterator it = m_cl_id_entity_map.find(cl_id);
    if (it == m_cl_id_entity_map.end()) {
        return NULL;
    }
    return it->second;
}

int Context::Set(const std::string& key, const std::string& value) {
    return Set(key, value, -1);
}

int Context::Set(const std::string& key, const std::string& value, int index) {
    KeySet::const_iterator it = m_key_set.find(key);
    if (it == m_key_set.end()) {
        DSTREAM_WARN("PE context(%s) not exist, can't set!", key.c_str());
        return -1;
    }
    std::string full_key = key;
    if (-1 != index) {
        full_key += kPipeDelimiter;
        StringAppendNumber(&full_key, static_cast<ssize_t>(index));
    }
    m_kv_context[full_key] = value;
    DSTREAM_DEBUG("PE context(%s) set to (%s)!", full_key.c_str(), value.c_str());

    return 0;
}

int Context::Get(const std::string& key, std::string* value) const {
    return Get(key, value, -1);
}

int Context::Get(const std::string& key, std::string* value, int index) const {
    assert(NULL != value);
    std::string full_key = key;
    if (-1 != index) {
        full_key += kPipeDelimiter;
        StringAppendNumber(&full_key, static_cast<ssize_t>(index));
    }
    KVContextMap::const_iterator it = m_kv_context.find(full_key);
    if (it == m_kv_context.end()) {
        DSTREAM_WARN("PE context(%s) not exist!", full_key.c_str());
        *value = "ERROR";
        return -1;
    }
    *value = it->second;
    return 0;
}

bool Context::ImporterReachHoldingThreshold() {
    return (m_importer_tuple_holding_size >= m_tuples_batch_size) ||
           (m_importer_tuple_holding_count >= m_tuples_batch_count);
}

} // namespace internal
} // namespace processelement
} // namespace dstream
