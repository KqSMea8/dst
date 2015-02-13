/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include "processnode/pe_wrapper.h"
#include <unistd.h>
#include <boost/lexical_cast.hpp>

#include "common/application_tools.h"
#include "common/hdfs_client.h"
#include "common/rpc_const.h"
#include "common/utils.h"

#include "processnode/config_map.h"
#include "processnode/process_node.h"
#include "processnode/prolog.h"
#include "processnode/router_mgr.h"
#include "processnode/zk_wrapper.h"

namespace dstream {
namespace processnode {

const static std::string kPrologName = "bin/prologue";
const static std::string kEpilogName = "bin/epilogue";

void GCPEWrapper::Run() {
    int32_t ret = error::OK;
    const uint64_t& peid = this->m_peid;

    DSTREAM_INFO("peid(%lu) BEGIN gc thread[%p, %lu]", peid, this, syscall(__NR_gettid));

    PEWrapperManager::PEWrapperManagerPtr pe_wrapper_manager_ptr = PEWrapperManager::GetPEWrapperManager();
    PEWrapper::PEWrapperPtr wrapper = pe_wrapper_manager_ptr->GetPEWrapper(peid);
    if (wrapper) {
        ret = wrapper->DestroySelf();
        if (error::OK > ret) {
            DSTREAM_ERROR("fail to clear PE wrapper (%lu), ret(%d)", peid, ret);
        } else {
            DSTREAM_INFO("clear PE wrapper (%lu) success.", peid);
        }
        // whatever the result is, erase pe wrapper from map
        pe_wrapper_manager_ptr->RemovePEWrapper(peid);
    }

    DSTREAM_INFO("peid(%lu) END gc thread", peid);
}

void CreatePEWrapper::Run() {
    int32_t ret = error::OK;
    const uint64_t& peid = this->m_peid;

    DSTREAM_INFO("peid(%lu) BEGIN create thread[%p, %lu]", peid, this, syscall(__NR_gettid));

    PEWrapperManager::PEWrapperManagerPtr pe_wrapper_manager_ptr = PEWrapperManager::GetPEWrapperManager();
    PEWrapper::PEWrapperPtr wrapper = pe_wrapper_manager_ptr->GetPEWrapper(peid);
    if (wrapper) {
        ret = wrapper->CreateSelf();
        if (error::OK > ret) {
            // create fail, and try to clear env
            wrapper->DestroySelf();
            pe_wrapper_manager_ptr->RemovePEWrapper(peid);
            DSTREAM_ERROR("fail to create PE (%lu), ret(%d)", peid, ret);
        } else {
            DSTREAM_INFO("create PE (%lu) success.", peid);
        }
    }

    DSTREAM_INFO("peid(%lu) END create thread", peid);
}

void UpdatePEWrapper::Run() {
    int32_t ret = error::OK;
    const uint64_t& peid = this->m_peid;

    DSTREAM_INFO("peid(%lu) BEGIN update thread[%p, %lu]", peid, this, syscall(__NR_gettid));

    PEWrapperManager::PEWrapperManagerPtr pe_wrapper_manager_ptr = PEWrapperManager::GetPEWrapperManager();
    PEWrapper::PEWrapperPtr wrapper = pe_wrapper_manager_ptr->GetPEWrapper(peid);
    if (wrapper) {
        ret = wrapper->UpdateSelf();
        if (error::OK != ret) {
            DSTREAM_ERROR("fail to update PE (%lu), ret(%d)", peid, ret);
            if (ret != error::UPDATE_PE_CONFIG_FAIL || ret != error::GEN_LOCAL_PE_ENV_FAIL) {
                wrapper->set_status(PEWrapper::DELETING);
                wrapper->StartGCThread(peid);
                DSTREAM_ERROR("fail to update PE(%lu), gc thread start.", peid);
            }
        } else {
            DSTREAM_INFO("update PE (%lu) success.", peid);
        }
        wrapper->ResetUpdateThread();
    }
    DSTREAM_INFO("peid(%lu) END update thread", peid);
}

static const int kFailIntoSleepIntervalSecs = 2;

PEWrapper::PEWrapper() :
        m_status(INVALID),
        m_serial(0),
        m_last_assign_time(0),
        m_tuple_batch_count(config_const::kPNTuplesBatchNum),
        m_tuple_batch_size(config_const::kPNTuplesBatchSizeByte),
        m_send_queue_size(0),
        m_recv_queue_size(0),
        m_send_watermark(FLT_MIN),
        m_recv_watermark(FLT_MIN),
        m_data_strategy(0),
        m_fetch_timeout_ms(0),
        m_send_timeout_ms(0),
        m_exec_workers(0),
        m_net_workers(0),
        m_disk_workers(0),
        m_small_tran_buf_num(0),
        m_small_tran_buf_size(0),
        m_big_tran_buf_num(0),
        m_buf_low_mark(FLT_MIN),
        m_tp(NULL),
        m_normal_exit(false),
        m_flow_control(),
        m_update_lock(),
        m_update_cond(m_update_lock),
        m_status_lock() {
}

PEWrapper::~PEWrapper() {
}

error::Code PEWrapper::StartCreateThread(uint64_t peid) {
    if (NULL != m_create_thread_ptr) {
        return error::OK;
    }
    m_create_thread_ptr = CreatePEWrapper::CreatePEWrapperPtr(new CreatePEWrapper(peid));
    bool ret = m_create_thread_ptr->Start();
    return ret ? error::OK : error::THREAD_ERROR;
}

error::Code PEWrapper::StartGCThread(uint64_t peid) {
    if (NULL != m_gc_thread_ptr) {
        return error::OK;
    }
    m_gc_thread_ptr = GCPEWrapper::GCPEWrapperPtr(new GCPEWrapper(peid));
    bool ret = m_gc_thread_ptr->Start();
    return ret ? error::OK : error::THREAD_ERROR;
}

error::Code PEWrapper::StartUpdateThread(uint64_t peid) {
    if (NULL != m_update_thread_ptr) {
        DSTREAM_WARN("maybe another update thread is running.");
        return error::OK;
    }
    m_update_thread_ptr = UpdatePEWrapper::UpdatePEWrapperPtr(new UpdatePEWrapper(peid));
    bool ret = m_update_thread_ptr->Start();
    return ret ? error::OK : error::THREAD_ERROR;
}

error::Code PEWrapper::StartTransporter() {
    error::Code ret = error::BAD_MEMORY_ALLOCATION;
    // start kylin transporter
    g_pTPStubMgr->FreeOfStub(m_peid.id());
    ESPSTUB of_stub = g_pTPStubMgr->AllocOfStub(m_peid.id());
    if (of_stub == INVALID_ESPSTUB) {
        DSTREAM_ERROR("fail to allocate outflow stub");
        return error::BIND_STUB_FAIL;
    }
    if (m_processor.role() == IMPORTER) {
        m_tp = new (std::nothrow) transporter::Transporter(m_peid.id(), of_stub);
    } else {
        g_pTPStubMgr->FreeIfStub(m_peid.id());
        ESPSTUB if_stub = g_pTPStubMgr->AllocIfStub(m_peid.id());
        if (if_stub == INVALID_ESPSTUB) {
            DSTREAM_ERROR("fail to allocate inflow stub");
            return error::BIND_STUB_FAIL;
        }
        m_tp = new (std::nothrow) transporter::Transporter(m_peid.id(), of_stub, if_stub);
    }
    // set transporter argument & start
    DSTREAM_INFO("pe(%lu) DataStrategy(%d) QSize(%d) WaterMark(%f) BatchSize(%lu) BatchNum(%lu)",
                 m_peid.id(), m_data_strategy, m_send_queue_size, m_send_watermark,
                 m_tuple_batch_size, m_tuple_batch_count);
    if (NULL == m_tp ||
        error::OK != (ret = m_tp->set_data_strategy(m_data_strategy)) ||
        error::OK != (ret = m_tp->set_queues("tp_queue",
                                            "tp_counter",
                                            (size_t)m_recv_queue_size,
                                            (size_t)m_send_queue_size,
                                            m_recv_watermark,
                                            m_send_watermark)) ||
        error::OK != (ret = m_tp->set_batch_num(m_tuple_batch_count)) ||
        error::OK != (ret = m_tp->set_batch_size(m_tuple_batch_size)) ||
        error::OK != (ret = m_tp->Start())) {
        return ret;
    }
    return error::OK;
}

error::Code PEWrapper::DestroyTransporter() {
    if (m_tp) {
        m_tp->Stop();
        m_tp->Release();
        m_tp = NULL;
    }
    if (m_processor.role() != IMPORTER) {
        g_pTPStubMgr->FreeIfStub(m_peid.id());
    }
    g_pTPStubMgr->FreeOfStub(m_peid.id());
    return error::OK;
}


error::Code PEWrapper::GenLocalPEEnv() {
    error::Code ret = error::OK;
    int sys_err = 0;

    ret = ClearLocalPEEnv(m_work_dir);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to clear pe[%lu]'s env", m_peid.id());
    }

    if ((sys_err = MakeDir(m_work_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0) {
        if (sys_err != EEXIST) {
            DSTREAM_ERROR("fail to make dir:[%s] for pe:[%lu], errstr:[%s]",
                          m_work_dir.c_str(), m_peid.id(), strerror(sys_err));
            return error::FILE_OPERATION_ERROR;
        }
        DSTREAM_WARN("fail to make dir:[%s] for pe:[%lu], exist",
                     m_work_dir.c_str(), m_peid.id());
    }

    // get stuff from hdfs
    std::string hdfs_client_dir;
    int32_t hdfs_retry;
    g_pn_cfg_map.GetValue(config_const::kHDFSClientDirName, &hdfs_client_dir);
    g_pn_cfg_map.GetIntValue(config_const::kHDFSRetryName, &hdfs_retry);
    if (application_tools::LowerString(hdfs_client_dir.substr(0, 4)) == "mock") {
        // do nothing
    } else {
        dstream::hdfs_client::HdfsClient hp(hdfs_client_dir, hdfs_retry);
        ret = (error::Code)hp.DownloadFile(m_app_uri, m_work_dir);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to download file:[%s] from hdfs:[%s]",
                         m_work_dir.c_str(), m_app_uri.c_str());
            return ret;
        }
    }

    ChmodRF(m_work_dir.c_str());

    return error::OK;
}

error::Code PEWrapper::ClearLocalPEEnv(const std::string& work_dir) {
    error::Code ret = error::OK;

    if (DoesPathExist(work_dir.c_str())) {
        int sys_ret = RemoveRF((work_dir +  "_bak").c_str());
        DSTREAM_DEBUG("[rm %s] with ret value %d", (work_dir +  "_bak").c_str(), sys_ret);

        sys_ret = MoveDir(work_dir.c_str(), (work_dir + "_bak").c_str());
        DSTREAM_DEBUG("[mv %s %s] with ret value %d", work_dir.c_str(),
                      (work_dir +  "_bak").c_str(), sys_ret);
    }

    DSTREAM_INFO("clear up exec directory(%s) success", work_dir.c_str());

    return ret;
}

string PEWrapper::DumpPEInfo() {
    char str_buf[256];
    memset(str_buf, 0, 256);
    snprintf(str_buf, sizeof(str_buf), " peid(%lu) pe_wrapper(%p)",
            m_peid.id(), this);
    return string(str_buf);
}

string PEWrapper::DumpUpStreamInfo() {
    string res = "";
    res += "------------PE Wrapper up-stream info---------\n";
    res += "------------end up-stream info----------\n\n";
    return res;
}

string PEWrapper::DumpDownStreamInfo() {
    string res = "";

    res += "------------PE Wrapper down-stream info---------\n";
    res += "------------end down-stream info----------\n\n";

    return res;
}

error::Code PEWrapper::StartPEProcess(const std::map<std::string, std::string>& pe_env) {
    error::Code ret = error::OK;

    std::string use_cglimit;
    ProcessManager::ProcessPtr process;
    OnPEExitCallback* exit_cb = NULL;
    OnPEStartCallback* start_cb = NULL;

    if (error::OK != g_pn_cfg_map.GetValue(config_const::kUseCglimitName, &use_cglimit)) {
        DSTREAM_WARN("can not find the config_value of key:[%s]", config_const::kUseCglimitName);
    }
    DSTREAM_DEBUG("StartPEProcess config[%s]= %s",
            config_const::kUseCglimitName, use_cglimit.c_str());

    ProcessBuilder process_builder(use_cglimit == "true");
    process_builder.SetEnvironment(pe_env);
    process_builder.SetResourceLimit(processor());
    process_builder.redirect_output_stream(true);
    process_builder.redirect_error_stream(true);

    string cmdline = processor().exec_cmd();
    exit_cb = new OnPEExitCallback();
    exit_cb->set_peid(peid());

    const int log_max_size = -1; // no limit now
    start_cb = new OnPEStartCallback(log_path(), NumberToString(m_peid.id()), log_max_size,
                                     work_dir().c_str());
    process = process_builder.Start(cmdline, exit_cb, start_cb);

    if (!process) {
        DSTREAM_WARN("[%s] fail to create pe: Name:[%s], ID:[%lu], Role:[%d], cmdline argvs:[%s]",
                __FUNCTION__, m_processor.name().c_str(), m_peid.id(),
                m_processor.role(), cmdline.c_str());
        ret = error::CREATE_PE_ERROR;
    } else {
        DSTREAM_INFO("create process:[%d] pe: Name:[%s], ID:[%lu], Role:[%d]"
                     ", work_dir:[%s], cmdline argvs:[%s]...OK", process->pid(),
                     processor().name().c_str(), peid().id(), processor().role(),
                     work_dir().c_str(), cmdline.c_str());
        set_process(process);
        ret = error::OK;
    }

    return ret;
}

void PEWrapper::StopPEProcess() {
    if (m_process) {
        m_process->Destroy();
    }
    DSTREAM_INFO("[%s] pe(%lu) destroy process success", __FUNCTION__, peid().id());
}

void PEWrapper::RestartPEProcess() {
    set_normal_exit(true);
    m_process->Stop();
    DSTREAM_INFO("try to stoping pe(%lu)", peid().id());
}

error::Code PEWrapper::UpdatePEConfig() {
    error::Code ret = error::OK;

    Application app;
    ProcessorElement pe;

    std::string pn_dir;
    std::string pe_dir;

    // transporter config
    int32_t tmp_int = 0;
    if (error::OK > g_pn_cfg_map.GetIntValue(config_const::kPNRecvQueueSizeName, &tmp_int)) {
        tmp_int = config_const::kPNRecvQueueSize;
    }
    m_recv_queue_size = m_processor.has_recv_queue_size() ?
            m_processor.recv_queue_size() :
            tmp_int;
    if (error::OK > g_pn_cfg_map.GetIntValue(config_const::kPNSendQueueSizeName, &tmp_int)) {
        tmp_int = config_const::kPNSendQueueSize;
    }
    m_send_queue_size = m_processor.has_send_queue_size() ?
            m_processor.send_queue_size() :
            tmp_int;
    std::string tmp_str;
    float tmp_float = 0.0;
    if (error::OK > g_pn_cfg_map.GetValue(config_const::kPNRecvWaterMarkName, &tmp_str)) {
        tmp_float = config_const::kPNRecvWaterMark;
    } else {
        tmp_float = strtof(tmp_str.c_str(), NULL);
    }
    m_recv_watermark  = m_processor.has_recv_watermark() ?
            m_processor.recv_watermark() :
            tmp_float;
    if (error::OK > g_pn_cfg_map.GetValue(config_const::kPNSendWaterMarkName, &tmp_str)) {
        tmp_float = config_const::kPNSendWaterMark;
    } else {
        tmp_float = strtof(tmp_str.c_str(), NULL);
    }
    m_send_watermark  = m_processor.has_send_watermark() ?
            m_processor.send_watermark() :
            tmp_float;

    // get tuple batch count
    m_tuple_batch_count = m_processor.has_tuple_batch_count() ?
            m_processor.tuple_batch_count() :
            m_tuple_batch_count;
    m_tuple_batch_size = m_processor.has_tuple_batch_size() ?
            m_processor.tuple_batch_size() :
            m_tuple_batch_size;

    m_fetch_timeout_ms = m_processor.has_fetch_timeout_ms() ?
            m_processor.fetch_timeout_ms() :
            0; // invalid
    m_send_timeout_ms = m_processor.has_send_timeout_ms() ?
            m_processor.send_timeout_ms() :
            0; // invalid
    m_exec_workers = m_processor.has_exec_workers() ?
            m_processor.exec_workers() :
            0; // invalid
    m_net_workers = m_processor.has_net_workers() ?
            m_processor.net_workers() :
            0; // invalid
    m_disk_workers = m_processor.has_disk_workers() ?
            m_processor.disk_workers() :
            0; // invalid
    m_small_tran_buf_num = m_processor.has_small_tran_buf_num() ?
            m_processor.small_tran_buf_num() :
            0; // invalid
    m_small_tran_buf_size = m_processor.has_small_tran_buf_size() ?
            m_processor.small_tran_buf_size() :
            0; // invalid
    m_big_tran_buf_num = m_processor.has_big_tran_buf_num() ?
            m_processor.big_tran_buf_num() :
            0; // invalid
    m_buf_low_mark = m_processor.has_buf_low_mark() ?
            m_processor.buf_low_mark() :
            FLT_MIN; // invalid

    // set dir
    ret = g_zk->GetApp(m_cpeid.app_id(), &app); // cache
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] fail to get the app associated with pe:[%ld] from zookeeper",
                     __FUNCTION__, m_peid.id());
        goto ERR_RET;
    }

    m_app_uri = app.app_path().path() + "/" + processor().name() + "_v";
    m_app_uri = m_app_uri + boost::lexical_cast<std::string>(revision()) + "/*";
    m_app_name = app.name();

    // set data strategy
    m_data_strategy = app.has_data_strategy() ? app.data_strategy() :
            config_const::kDefaultAppDataStrategy;

    m_log_path = "./log/pe";

    // set work dir: pn_dir/app_processor_Vverion_peid
    g_pn_cfg_map.GetValue(config_const::kPNWorkDirName, &pn_dir);
    pe_dir = pn_dir + "/" + app.name() + "_" + m_processor.name() + "_v";
    StringAppendNumber(&pe_dir, m_processor.cur_revision());
    pe_dir.append("_");
    StringAppendNumber(&pe_dir, m_peid.id());
    m_work_dir = pe_dir;

    // get pe log config
    // TODO(zhenpeng) cpeid can delete
    ret = g_zk->GetPE(m_cpeid.app_id(), m_peid, &pe);
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] fail to get the pe associated with pe:[%ld] from zookeeper",
                     __FUNCTION__, m_peid.id());
        goto ERR_RET;
    }
    m_log_config = pe.log();
    m_serial = pe.serial();

    m_dynamic_proto_dir = m_processor.has_dynamic_proto_dir() ?
            m_processor.dynamic_proto_dir() :
            ".";

    // get flow control infomation by processor name
    ret = g_zk->GetAppFlowControl(m_cpeid.app_id(), m_processor.name(), &m_flow_control);
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] fail to get the flow control associated with app:[%ld] from zookeeper",
                __FUNCTION__, m_cpeid.app_id().id());
        // set default value if can not get value from zk
        m_flow_control.Clear();
    } 

ERR_RET:
    return ret;
}

error::Code PEWrapper::GenProcessEnv(std::map<std::string, std::string>* ev) {
    error::Code ret = error::OK;

    // add by fangjun
    ev->insert(std::make_pair(Context::kEnvKeyDynamicProtoDir, m_dynamic_proto_dir));

    // add for flow control
    if (m_flow_control.qps_limit() < 0) {
        m_flow_control.set_qps_limit(INT32_MAX);
    }
    ev->insert(std::make_pair(Context::kEnvKeySpoutLimit,
            boost::lexical_cast<std::string>(m_flow_control.qps_limit()).c_str()));

    // gen env variable
    std::string prefix = Context::kCommitLogPrefix;
    std::string str_uri;
    std::string str_num;
    ev->insert(std::make_pair(Context::kEnvKeyWorkDirectory, m_work_dir));

    // generate pn dir
    char pn_dir[4096];
    if (getcwd(pn_dir, sizeof(pn_dir)) != NULL) {
        ev->insert(std::make_pair(Context::kEnvKeyPnDirectory, pn_dir));
    } else {
        DSTREAM_WARN("failed to get cwd");
    }

    // set esp uri
    str_uri = "127.0.0.1:";
    StringAppendNumber(&str_uri, transporter::g_esp_port);
    ev->insert(std::make_pair(Context::kEnvKeyESPUri, str_uri));

    g_pn_cfg_map.GetValue(config_const::kPNPEListenUriName, &str_uri);
    ev->insert(std::make_pair(Context::kEnvKeyNodeUri, str_uri));
    str_num.clear();
    StringAppendNumber(&str_num, m_peid.id());
    ev->insert(std::make_pair(Context::kEnvKeyProcessElementId, str_num));
    ev->insert(std::make_pair(Context::kEnvKeyProcessElementName, m_processor.name()));

    // setting transporter
    if (m_tuple_batch_count > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_tuple_batch_count);
        ev->insert(std::make_pair(Context::kEnvKeyTuplesBatchCount, str_num));
    }
    if (m_tuple_batch_size > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_tuple_batch_size);
        ev->insert(std::make_pair(Context::kEnvKeyTuplesBatchSize, str_num));
    }
    if (m_force_forward_tuples_period_ms > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_force_forward_tuples_period_ms);
        ev->insert(std::make_pair(Context::kEnvKeyForceForwardTuplesPeriodMillSeconds, str_num));
    }
    if (m_fetch_timeout_ms > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_fetch_timeout_ms);
        ev->insert(std::make_pair(Context::kEnvKeyFetchLogTimeoutMillSeconds, str_num));
    }
    if (m_send_timeout_ms > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_send_timeout_ms);
        ev->insert(std::make_pair(Context::kEnvKeySendLogTimeoutMillSeconds, str_num));
    }
    if (m_exec_workers > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_exec_workers);
        ev->insert(std::make_pair(Context::kEnvKeyExecWorkers, str_num));
    }
    if (m_net_workers > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_net_workers);
        ev->insert(std::make_pair(Context::kEnvKeyNetWorkers, str_num));
    }
    if (m_disk_workers > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_disk_workers);
        ev->insert(std::make_pair(Context::kEnvKeyDiskWorkers, str_num));
    }
    if (m_small_tran_buf_num > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_small_tran_buf_num);
        ev->insert(std::make_pair(Context::kEnvKeyBufSmallTranBufNum, str_num));
    }
    if (m_small_tran_buf_size > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, m_small_tran_buf_size);
        ev->insert(std::make_pair(Context::kEnvKeyBufSmallTranBufSize, str_num));
    }
    if (m_big_tran_buf_num > 0) {
        StringAppendNumber(&str_num, m_big_tran_buf_num);
        ev->insert(std::make_pair(Context::kEnvKeyBufBigTranBufNum, str_num));
    }
    if (m_buf_low_mark > FLT_MIN) {
        char t[32];
        snprintf(t, sizeof(t), "%f", m_buf_low_mark);
        string tmp_str(t);
        ev->insert(std::make_pair(Context::kEnvKeyBufLowMark, tmp_str));
    }

    // pass appname to pe
    ev->insert(std::make_pair(Context::kEnvKeyAppName, m_app_name));

    const Subscribe& sub = m_processor.subscribe();
    // support for bigpipe subscribe
    int32_t size = sub.bigpipe_size();
    if (size > 0) {
        str_num.clear();
        StringAppendNumber(&str_num, size);
        ev->insert(std::make_pair(prefix + Context::kEnvKeyCommitLogNumber, str_num));
        for (int32_t i = 0; i < size; i++) {
            std::string key = prefix;
            StringAppendNumber(&key, i);
            key += ".";
            // by fern for test easily, this modification may affect data replay feature.
            std::string str_importer_type;
            g_pn_cfg_map.GetValue(config_const::kImporterTypeName, &str_importer_type);
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogType, str_importer_type));
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPName,
                    sub.bigpipe(i).name()));

            // set pipelet for pe
            size_t pipelet_total = sub.bigpipe(i).pipelet_id_size();
            size_t pe_total = m_processor.parallism();
            std::ostringstream oss;
            for (size_t idx = m_serial; idx < pipelet_total; idx += pe_total) {
                oss << sub.bigpipe(i).pipelet_id(idx) << ":";
            }
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPPipelet, oss.str()));
            DSTREAM_INFO("set pipelet list: %s", oss.str().c_str());

            if (sub.bigpipe(i).has_sub_point()) {
                std::string sub_point = NumberToString(sub.bigpipe(i).sub_point());
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPSubPoint, sub_point));
            }
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPConfPath,
                    Context::kDefaultCommitLogBPConfPath));
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPConfName,
                    Context::kDefaultCommitLogBPConfName));
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPUserName,
                    sub.bigpipe(i).username()));
            if (sub.bigpipe(i).has_password()) {
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPToken,
                        sub.bigpipe(i).password()));
            } else {
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogBPToken,
                        Context::kDefaultCommitLogBPToken));
            }
        }
    }

    if (sub.localfile_size() > 0) {
        int32_t size = sub.localfile_size();
        str_num.clear();
        StringAppendNumber(&str_num, size);
        ev->insert(std::make_pair(prefix + Context::kEnvKeyCommitLogNumber, str_num));
        for (int32_t i = 0; i < size; i++) {
            std::string key = prefix;
            StringAppendNumber(&key, i);
            key += ".";
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogType,
                    Context::kCommitLogLFSType));
            if (sub.localfile(i).has_path()) {
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogLFSName,
                        sub.localfile(i).path()));
            } else {
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogLFSName,
                        Context::kDefaultCommitLogLFSName));
            }
            if (sub.localfile(i).has_sub_point()) {
                std::string sub_point = NumberToString(sub.localfile(i).sub_point());
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogLFSSubPoint, sub_point));
            }
        }
    }

    if (sub.scribe_size() > 0) {
        int32_t size = sub.scribe_size();
        str_num.clear();
        StringAppendNumber(&str_num, size);
        ev->insert(std::make_pair(prefix + Context::kEnvKeyCommitLogNumber, str_num));
        for (int32_t i = 0; i < size; i++) {
            std::string key = prefix;
            StringAppendNumber(&key, i);
            key += ".";
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogType,
                    Context::kCommitLogScribeType));
            if (sub.scribe(i).has_buffersize()) {
                std::string buffersize = NumberToString(sub.scribe(i).buffersize());
                ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogScribeBuffLen,
                    buffersize));
            }
        }
    }

    if (sub.fake_size() > 0) {
        int32_t size = 1;
        str_num.clear();
        StringAppendNumber(&str_num, size);
        ev->insert(std::make_pair(prefix + Context::kEnvKeyCommitLogNumber, str_num));
        for (int32_t i = 0; i < size; i++) {
            std::string key = prefix;
            StringAppendNumber(&key, i);
            key += ".";
            ev->insert(std::make_pair(key + Context::kEnvKeyCommitLogType,
                    Context::kCommitLogFakeType));
        }
    }

    if (m_processor.role() == IMPORTER) {
        ev->insert(std::make_pair(Context::kEnvKeyProcessorRole, Context::kRoleAsImporter));
        // set persist progress option
        ev->insert(std::make_pair(Context::kEnvKeySwitchOfProgressPersist,
                m_processor.persist_progress()));
        // set persist progress option
        ev->insert(std::make_pair(Context::kEnvKeySwitchOfProgressPersist,
                m_processor.persist_progress()));

    } else if (m_processor.role() == EXPORTER) {
        ev->insert(std::make_pair(Context::kEnvKeyProcessorRole, Context::kRoleAsExporter));

        // add bigpipe exporter support
        if (m_processor.has_publish()) {
            Publish pub = m_processor.publish();
            // read bigpipe config
            if (pub.type() == config_const::kExporterTypeBigpipe) {
                // set exporter type
                ev->insert(std::make_pair(Context::kEnvKeyExporterType,
                        config_const::kExporterTypeBigpipe));

                // set bigpipe config
                ev->insert(std::make_pair(Context::kEnvKeyExporterPubBigpipePipeName,
                        pub.bigpipe().pipe_name()));
                ev->insert(std::make_pair(Context::kEnvKeyExporterPubBigpipePubToken,
                        pub.bigpipe().pub_token()));
                ev->insert(std::make_pair(Context::kEnvKeyExporterPubBigpipePubUser,
                        pub.bigpipe().pub_user()));
                char tmp_str[32];
                snprintf(tmp_str, sizeof(tmp_str), "%d", pub.bigpipe().pipelet_num());
                ev->insert(std::make_pair(Context::kEnvKeyExporterPubBigpipePipeletNum, tmp_str));

                // set failover config
                snprintf(tmp_str, sizeof(tmp_str), "%d", pub.bigpipe().retry_time());
                ev->insert(std::make_pair(Context::kEnvKeyExporterPubBigpipeRetryTime, tmp_str));

                snprintf(tmp_str, sizeof(tmp_str), "%d", pub.bigpipe().retry_interval());
                ev->insert(std::make_pair(
                        Context::kEnvKeyExporterPubBigpipeRetryInterval, tmp_str));
            }

            // for support more type exporter
        }
    } else {
        ev->insert(std::make_pair(Context::kEnvKeyProcessorRole, Context::kRoleAsWorker));
    }
    ev->insert(std::make_pair(Context::kWorkDirectoryStartup, m_work_dir + "/startup"));

    return ret;
}

error::Code PEWrapper::CreateSelf() {
    // set pe wrapper status
    assert(m_status == CREATING);
    error::Code ret = error::OK;
    const uint64_t& peid = m_peid.id();

    std::map<std::string, std::string> process_env;

#ifdef DSTREAM_PROFILE
    struct timeval  begintv, endtv;
    TIMER_PRINT_BEGIN();

    string profile_val = "CreatePE: creating pe[";
    StringAppendNumber(&profile_val, peid);
    profile_val += "],  start creating time";
    TIMER_PRINT(profile_val.c_str());
#endif

    ret = GenProcessEnv(&process_env);
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] pe(%lu) fail to gen process env from pe wrapper",
                     __FUNCTION__, peid);
        goto ERR_RET_CLEAR_PEENV;
    }

    // download hdfs file
    ret = GenLocalPEEnv();
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] peid:[%lu] gen local env fail", __FUNCTION__, peid);
        goto ERR_RET_CLEAR_PEENV;
    }

    // run prologue for pe
    RunProlog(m_work_dir);

    // start PE process
    ret = StartPEProcess(process_env);
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] peid:[%lu] start pe process fail", __FUNCTION__, peid);
        goto ERR_RET_CLEAR_PEENV;
    }

    DSTREAM_INFO("Create PE[%lu] ...OK", peid);
#ifdef DSTREAM_PROFILE
    profile_val = "CreatePE: creating pe[";
    StringAppendNumber(&profile_val, peid);
    profile_val += "],  profiling the all time of creating takes";
    TIMER_PRINT_END(profile_val.c_str());

    profile_val = "CreatePE: creating pe[";
    StringAppendNumber(&profile_val, peid);
    profile_val += "],  finish creating time";
    TIMER_PRINT(profile_val.c_str());
#endif
    m_status = RUNNING;
    return ret;

ERR_RET_CLEAR_PEENV:
    m_status = INVALID;
    return ret;
}

error::Code PEWrapper::UpdateSelf() {
    if (status() != UPDATING) {
        DSTREAM_WARN("pe(%lu) status(%d) error, updating abord.", m_peid.id(), status());
        return error::OK;
    }

    // backup working dir
    std::string old_work_dir = m_work_dir;
    uint32_t    old_revision = revision();

    // 0. update local configs
    set_revision(m_update_revision);
    error::Code ret = UpdatePEConfig();
    if (ret != error::OK) {
        DSTREAM_WARN("update pe config failed, ret(%d)", ret);
        m_work_dir = old_work_dir;
        set_revision(old_revision);
        return error::UPDATE_PE_CONFIG_FAIL;
    }
    DSTREAM_INFO("update pe(%lu) config done.", m_peid.id());

    // 1. download pe files
    ret = GenLocalPEEnv();
    if (ret != error::OK) {
        DSTREAM_WARN("gen local pe env failed, ret(%d)", ret);
        m_work_dir = old_work_dir;
        set_revision(old_revision);
        return error::GEN_LOCAL_PE_ENV_FAIL;
    }
    DSTREAM_INFO("download hdfs(%s) done.", m_work_dir.c_str());

    // 2. run epilogue for OLD pe
    RunEpilog(old_work_dir);

    // 3. kill old pe process
    // 4. wait for pe exit
    {
        MutexLockGuard locl_guard(m_update_lock);
        RestartPEProcess();
        m_update_cond.timed_wait(5000); // wait for 5 sec
    }
    DSTREAM_INFO("old process exit.");
    StopPEProcess(); // make sure pe exit and delete fd watcher

    // 5. run prologue for NEW pe
    RunProlog(m_work_dir);
    DSTREAM_INFO("run prolog for new process OK.");

    // 4. start new pe process
    std::map<std::string, std::string> process_env;
    ret = GenProcessEnv(&process_env);
    if (ret != error::OK) {
        DSTREAM_WARN("pe(%lu) fail to gen process env from pe wrapper, ret(%d)", m_peid.id(), ret);
        return error::GEN_PROCESS_ENV_FAIL;
    }
    DSTREAM_INFO("generate process envirenments OK.");

    ret = StartPEProcess(process_env);
    if (ret != error::OK) {
        DSTREAM_WARN("peid(%lu) start pe process fail, ret(%d)", m_peid.id(), ret);
        return error::START_PE_FAIL;
    }

    // 5. clear old env
    DSTREAM_INFO("pe(%lu) start to clear old dirs...", m_peid.id());
    ClearLocalPEEnv(old_work_dir);

    m_status = RUNNING;
    DSTREAM_INFO("Update PE(%lu) OK", m_peid.id());
    return ret;
}

error::Code PEWrapper::DestroySelf() {
    m_status = DELETING;
    error::Code ret = error::OK;
    const uint64_t& peid = m_peid.id();

#ifdef DSTREAM_PROFILE
    struct timeval  begintv, endtv;
    TIMER_PRINT_BEGIN();

    string profile_val = "ClearPE: killing pe[";
    StringAppendNumber(&profile_val, peid.id());
    profile_val += "],  start killing time";
    TIMER_PRINT(profile_val.c_str());
#endif

    DSTREAM_INFO("[%s] pe(%lu): clear pe ...", __FUNCTION__, peid);

    // run epilogue for pe
    RunEpilog(m_work_dir);

    // kill pe process
    // RECHECK!!! first call at RPC KillPE
    StopPEProcess();

    // pe wrapper delete pe enviroment
    ret = ClearLocalPEEnv(m_work_dir);
    if (ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to delete pe env, pe(%lu)", __FUNCTION__, peid);
    }

    ret = DestroyLocalPE();
    if (ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to del local pe(%lu) env",
                      __FUNCTION__, peid);
    }

    if (!normal_exit() && NULL != g_pn_reporter) {
        DSTREAM_DEBUG("exceptional pe[%lu] exit", peid);
        g_pn_reporter->SignalCond();
    }

#ifdef DSTREAM_PROFILE
    profile_val = "ClearPE: killing pe[";
    StringAppendNumber(&profile_val, peid);
    profile_val += "],  profiling the all time of killing takes";
    TIMER_PRINT_END(profile_val.c_str());

    profile_val = "ClearPE: killing pe[";
    StringAppendNumber(&profile_val, peid);
    profile_val += "],  finish killing time";
    TIMER_PRINT(profile_val.c_str());
#endif

    if (error::OK == ret) {
        DSTREAM_INFO("[%s] pe(%lu): Clear PE ...OK! ", __FUNCTION__, peid);
    }
    m_status = ZOMBIE;
    return ret;
}

error::Code PEWrapper::InitLocalPE() {
    const uint64_t& peid = m_peid.id();
    error::Code ret = error::OK;

    // we must update config first
    ret = UpdatePEConfig();
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] fail to update PE config, pe(%lu), errno(%d), err(%s)",
                     __FUNCTION__, peid, ret, ErrStr(ret));
        goto ERR_RETURN;
    }
    // start kylin transporter
    if ((ret = StartTransporter()) < error::OK) {
        DSTREAM_WARN("[%s] fail to start transporter, pe(%lu), errno(%d), err(%s)",
                     __FUNCTION__, peid, ret, ErrStr(ret));
        goto ERR_RETURN;
    }
    // add pe to router manager
    router::RouterMgr* ptr = router::RouterMgr::Instance();
    ret = ptr->AddPE(peid);
    if (ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to add pe(%lu) to app router manager, errno(%d), err(%s)",
                      __FUNCTION__, peid, ret, ErrStr(ret));
        goto ERR_RETURN;
    }
    return ret;
ERR_RETURN:
    m_status = INVALID;
    return ret;
}

error::Code PEWrapper::DestroyLocalPE() {
    error::Code ret = error::OK;
    const uint64_t& peid = m_peid.id();
    // delete pe from router manager
    router::RouterMgr* ptr = router::RouterMgr::Instance();
    ret = ptr->DelPE(peid);
    if (ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to del pe(%lu) to app router manager",
                      __FUNCTION__, peid);
    }
    // stop kylin transporter
    ret = DestroyTransporter();
    if (ret < error::OK) {
        DSTREAM_WARN("[%s] fail to stop transporter, pe(%lu)", __FUNCTION__, peid);
    }
    return ret;
}

void PEWrapper::RunProlog(const std::string& work_dir) {
    error::Code ret = Prolog::RunProlog(m_peid.id(), work_dir, m_last_assign_time, kPrologName);
    if (ret != error::OK) {
        DSTREAM_ERROR("peid(%lu) exec prologue fail, ret=%d", m_peid.id(), ret);
    } else {
        DSTREAM_INFO("peid(%lu) exec prologue OK.", m_peid.id());
    }
}

void PEWrapper::RunEpilog(const std::string& work_dir) {
    error::Code ret = Prolog::RunProlog(m_peid.id(), work_dir, m_last_assign_time, kEpilogName);
    if (ret != error::OK) {
        DSTREAM_ERROR("peid(%lu) exec epilogue fail, ret=%d", m_peid.id(), ret);
    } else {
        DSTREAM_INFO("peid(%lu) exec epilogue OK.", m_peid.id());
    }
}

void PEWrapper::NotifyUpdate() {
    MutexLockGuard lock_guard(m_update_lock);
    m_update_cond.notify();
}

} // namespace processnode
} // namespace dstream
