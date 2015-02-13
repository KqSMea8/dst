/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#include "processnode/process_node.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace dstream {
namespace processnode {

volatile int32_t g_quit = 0;

ProcessNode::ProcessNode()
        : m_config_tuples_batch_count(0),
        m_config_force_forward_tuples_period_ms(0) {
    if (Init()) {
        DSTREAM_DEBUG("ProcessNode init.");
    } else {
        DSTREAM_ERROR("ProcessNode init fail.");
    }
}

ProcessNode::~ProcessNode() {
    Destory();
    DSTREAM_DEBUG("ProcessNode destory.");
}

bool ProcessNode::Init() {
    if (g_pn_cfg_map.GetIntValue(config_const::kPNTuplesBatchCountName,
                &m_config_tuples_batch_count) < error::OK) {
        m_config_tuples_batch_count = config_const::kPNTuplesBatchCount;
    }
    if (g_pn_cfg_map.GetIntValue(config_const::kPNForceForwardTuplesPeriodMillSecondsName,
                &m_config_force_forward_tuples_period_ms) < error::OK) {
        m_config_force_forward_tuples_period_ms =
            config_const::kPNForceForwardTuplesPeriodMillSeconds;
    }

    ProcessManager::DelayCleanUp();

    m_wrapper_mgr_ptr = PEWrapperManager::GetPEWrapperManager();
    return (NULL != m_wrapper_mgr_ptr);
}

void ProcessNode::Destory() {
    // do nothing
}

void ProcessNode::WritePNMeta(const PN& pn) {
#define WRITE_META(metaname) \
    filemeta::WriteMeta(MetaPath, #metaname, Convert<std::string>(pn.metaname()));
    mkdir("Meta", 0777);
    const char* MetaPath = "Meta/PN";
    filemeta::WriteMeta(MetaPath, "pnid", pn.pn_id().id());
    WRITE_META(host);
    WRITE_META(rpc_server_port);
    WRITE_META(pub_port);
    WRITE_META(sub_port);
    WRITE_META(importer_port);
    WRITE_META(debug_port);
    WRITE_META(httpd_port);
#undef WRITE_META
}

string ProcessNode::OnDebugPN(const string& cmd, const string& args) {
    DSTREAM_DEBUG("ProcessNode::onDebugPN(cmd:%s, args:%s)...", cmd.c_str(), args.c_str());
    string ret = "";

    assert(PEWrapperManager::GetPEWrapperManager());

    if (cmd == "allpe") {
        ret += "version : ";
        ret += VERSION;
        ret += "\n";
        ret += "all pe : \n";
        ret += PEWrapperManager::GetPEWrapperManager()->DumpPEWrapperMapInfo();

    } else if (cmd == "pe") {
        uint64_t pe_id = strtoll(args.c_str(), NULL, 10);

        ret = PEWrapperManager::GetPEWrapperManager()->DumpPEUpDownStreamInfo(pe_id);

    } else if (cmd == "conn") {
        string conn_num = "none";
        string conn_info = "";

        ret += "connect manager num : " + conn_num + "\n";
        ret += "connect info : \n" + conn_info + "\n";
        ret += "up pe connection map: \n";

    } else if (cmd == "queue") {

        string queue_num = "";
        string queue_info = "";
        queue_num = "none";
        ret = "send queue manager num : " + queue_num + "\n";
        ret += "queue info : \n" + queue_info + "\n";

    } else if (cmd == "set_log_level") {

        int level = atoi(args.c_str());
        if (level == 0) {
            DSTREAM_WARN("receive a cmd to set log level to %d ", level);
        }
        DSTREAM_INFO("logger::set_level(%d)", level);
        logger::set_level(level);
        ret += "OK";

    } else if (cmd == "pe_id_list") {
        ret = PEWrapperManager::GetPEWrapperManager()->DumpPEIDList();
    } else if (cmd == "quit") {
        AtomicSetValue(g_quit, 1);
        DSTREAM_DEBUG("[QUIT] Set g_quit(%d)", AtomicGetValue(g_quit));
        if (g_pn_reporter) {
            g_pn_reporter->Stop(false);
            DSTREAM_DEBUG("[QUIT] Stop pn reporter(%p)", g_pn_reporter);
        }
    } else {
        ret = "debug cmd error!";
    }
    return ret;
}


bool ProcessNode::OnReady(const PEID& peid) {
    return true;
}

void ProcessNode::OnHeartbeat(const PEID& peid,
        const std::map< std::string, std::string >& metrics,
        FlowControl& flow_control) {
    DSTREAM_DEBUG("ProcessNode::onHeartbeat(%lu)...", peid.id());

    if (!g_pn_reporter) {
        flow_control.Clear();
        return;
    }

    // zhangyan04@baidu.com
    // fill metrics from pe.
    g_pn_reporter->SetMetricInfo(peid.id(), metrics);

    PEWrapper::PEWrapperPtr wrapper =
        PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(peid.id());
    if (NULL == wrapper) {
        DSTREAM_WARN("[%s] get PE wrapper fail, pe(%lu)", __FUNCTION__, peid.id());
        flow_control.Clear();
        return;
    }
    flow_control = wrapper->flow_control();
}

int32_t ProcessNode::OnCreatePE(const AppID& app_id,
        const PEID& peid,
        const Processor& processor,
        const BackupPEID& bpeid,
        const uint64_t last_assign_time) {
    int32_t ret = error::OK;
    uint64_t pe_id = peid.id();

    DSTREAM_DEBUG("[%s] create pe wrapper (%lu)", __FUNCTION__, pe_id);
    assert(PEWrapperManager::GetPEWrapperManager());

    // check whether this pe already exists, just return if does
    ret = PEWrapperManager::GetPEWrapperManager()->CheckAndInsertPEWrapper(
            peid, app_id, processor, bpeid, last_assign_time);
    if (ret < error::OK) {
        DSTREAM_WARN("pe(%lu) wrapper already exist.", pe_id);
    } else {
        PEWrapper::PEWrapperPtr wrapper =
            PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(pe_id);
        assert(wrapper);

        if (wrapper->StartCreateThread(pe_id) == error::OK ) {
            DSTREAM_INFO("pe(%lu) start create thread.", pe_id);
            ret = error::OK;
        } else {
            DSTREAM_WARN("pe(%lu) create thread failed.", pe_id);
            ret = error::CREATE_PE_ERROR;
        }
    }
    return ret;
}

int32_t ProcessNode::OnUpdatePE(const AppID& app_id,
        const PEID& peid,
        uint32_t revision) {
    DSTREAM_INFO("[%s] update pe process (%lu)", __FUNCTION__, peid.id());

    assert(PEWrapperManager::GetPEWrapperManager());
    PEWrapper::PEWrapperPtr wrapper =
            PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(peid.id());
    if (NULL == wrapper) {
        DSTREAM_WARN("[%s] fail to get pe(%lu)'s wrapper", __FUNCTION__, peid.id());
        return error::OK;
    }

    // only RUNNING state can do update
    if (!wrapper->CheckAndSetStatus(PEWrapper::RUNNING, PEWrapper::UPDATING)) {
        DSTREAM_WARN("[%s] pe(%lu) is not in running state, current state(%d). "
                "update will be abort.", __FUNCTION__, peid.id(), wrapper->status());
        return error::OK;
    }
    // check running version, no need to lock
    if (wrapper->revision() == revision) {
        DSTREAM_INFO("[%s] pe(%lu) running version(%u) equal to updating version.",
                     __FUNCTION__, peid.id(), wrapper->revision());
        // rollback status
        wrapper->CheckAndSetStatus(PEWrapper::UPDATING, PEWrapper::RUNNING);
        return error::OK;
    }

    DSTREAM_INFO("start to update pe(%lu), running version(%u), update to version(%u).",
                 peid.id(), wrapper->revision(), revision);
    wrapper->update_revision(revision);
    if (wrapper->StartUpdateThread(peid.id()) != error::OK) {
        // rollback status
        wrapper->CheckAndSetStatus(PEWrapper::UPDATING, PEWrapper::RUNNING);
    }
    return error::OK;
}

int32_t ProcessNode::OnKillPE(const PEID& peid) {
    int32_t ret = error::OK;

    DSTREAM_INFO("[%s] kill pe process (%lu)", __FUNCTION__, peid.id());

    assert(PEWrapperManager::GetPEWrapperManager());
    PEWrapper::PEWrapperPtr wrapper =
        PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(peid.id());
    if (NULL == wrapper) {
        DSTREAM_WARN("[%s] fail to get pe(%lu)'s wrapper", __FUNCTION__, peid.id());
        return error::PEWRAPPER_NOT_FOUND;
    }

    // only RUNNING pe could handle KILL command
    if (!wrapper->CheckAndSetStatus(PEWrapper::RUNNING, PEWrapper::DELETING)) {
        DSTREAM_WARN("fail to set pe(%lu) wrapper status to DELETING, cur status:%d",
                peid.id(), wrapper->status());
        return error::PEWRAPPER_STATUS_ERROR;
    }
    // set normal exit
    wrapper->set_normal_exit(true);
    PEWrapperManager::GetPEWrapperManager()->GCPEWrapper(peid.id());

    return ret;
}

ProcessNode* ProcessNode::m_process_node_instance = NULL;

bool ProcessNode::InitProcessNodeContext() {
    if (NULL != m_process_node_instance) {
        DSTREAM_INFO("process node inited");
        return true;
    }
    return (NULL != (m_process_node_instance = new ProcessNode));
}

void ProcessNode::DestoryProcessNode() {
    if (NULL != m_process_node_instance) {
        delete m_process_node_instance;
        m_process_node_instance = NULL;
    }
}

ProcessNode* ProcessNode::GetProcessNode() {
    return m_process_node_instance;
}

} // namespace processnode
} // namespace dstream

