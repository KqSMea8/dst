/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:      2012/05/28
* @filename:    pn_rpc_server.cc
* @author:      zhanggengxin
* @brief:       pn rpc server
*/

#include "processnode/pn_rpc_server.h"

namespace dstream {
namespace pn_rpc_server {

#ifdef DEBUG_TRACE_PRC
void TRACE_RPC(Log* logptr, const string& str) {
    struct timeval tv;
    char debug_info_string[256];

    gettimeofday(&tv, NULL);
    snprintf(debug_info_string, sizeof(debug_info_string), "[%lu]%s,",
             (tv.tv_sec * 1000000 + tv.tv_usec), str.c_str());
    string tmp_debug_info(debug_info_string);
    if (logptr->has_debug_info()) {
        tmp_debug_info = logptr->debug_info() + tmp_debug_info;
    }
    logptr->set_debug_info(tmp_debug_info);
}
#endif


std::string PNPMRPCServer::CREATEPE(const CreatePEMessage* req, CreatePEResult* ack) {
    DSTREAM_INFO("receive CREATEPE request, peid %lu", req->pe_id().id());
    int32_t ret = m_process_node->OnCreatePE(req->app_id(), req->pe_id(),
                                             req->processor(), req->backup_id(),
                                             req->last_assign_time());
    ack->mutable_id()->set_id(m_process_node->GetPNMeta().pn_id().id());
    ack->mutable_app_id()->set_id(req->app_id().id());
    ack->mutable_pe_id()->set_id(req->pe_id().id());
    ack->mutable_backup_id()->set_id(req->backup_id().id());
    ack->set_res(ret);
    return (ret == error::OK || ret == error::PE_ALREADY_EXISTS) ?
           rpc_const::kOK : rpc_const::kFail;
}

std::string PNPMRPCServer::PEOPERATION(const PEOperationMessage* req, PEOperationResult* result) {
    int ret = 0;
    if (req->type() == PEOperationMessage_OperationType_KILL) {
        DSTREAM_INFO("receive Kill PE request, peid %lu", req->pe_id().id());
        ret = m_process_node->OnKillPE(req->pe_id());
        result->set_res(ret);

    } else if (req->type() == PEOperationMessage_OperationType_RESTART) {
        DSTREAM_INFO("recevive UPDATE PE request, peid %lu", req->pe_id().id());
        ret = m_process_node->OnUpdatePE(req->app_id(), req->pe_id(), req->revision());
        result->set_res(ret);
    }

    return rpc_const::kOK;
}

std::string PNDebugRPCServer::DEBUGPN(const DebugPNRequest* req, DebugPNACK* ack) {
    string debug_info = m_process_node->OnDebugPN(req->cmd(), req->args());
    ack->set_debug_info(debug_info);
    return rpc_const::kOK;
}


std::string PNPERPCServer::PUB_SUB_READY(const ReadyRequest* req,
                                         ReadyACK* ack) {
    int status = error::OK;

    if (m_process_node->OnReady(req->peid())) {
        status = error::SEND_QUEUE_FULL;
    }
    ack->set_status(status);
    return rpc_const::kOK;
}

std::string PNPERPCServer::PUB_SUB_HEARTBEAT(const HeartbeatRequest* req,
                                             HeartbeatACK* ack) {
#ifdef DEBUG_TRACE_PRC
    // TRACE
    if (req->has_debug_info()) {
        DSTREAM_DEBUG("[TRACE] [%s] heartbeat_req's debug_info:%s",
                      __FUNCTION__, req->debug_info().c_str());
    }
#endif

    assert(req->metric_name().size() == req->metric_value().size());
    std::map< std::string, std::string > metrics;
    for (int i = 0; i < req->metric_name().size(); i++) {
        const std::string& k = req->metric_name(i);
        const std::string& v = req->metric_value(i);
        metrics[k] = v;
    }
    FlowControl flow_control;
    m_process_node->OnHeartbeat(req->peid(), metrics, flow_control);
    ack->mutable_flow_control()->CopyFrom(flow_control);

#ifdef DEBUG_TRACE_PRC
    DSTREAM_DEBUG("[TRACE] [%s] after onHeartbeat", __FUNCTION__);
    // TRACE
    struct timeval tv;
    char debug_info_string[256];
    gettimeofday(&tv, NULL);
    snprintf(debug_info_string, sizeof(debug_info_string), "[%lu]%s,",
             (tv.tv_sec * 1000000 + tv.tv_usec), "HeartbeatService::PUB_SUB_HEARTBEAT");
    ack->set_debug_info(debug_info_string);
    DSTREAM_DEBUG("[TRACE] [%s] heartbeat_res's debug_info:%s",
                  __FUNCTION__, debug_info_string);
#endif

    return rpc_const::kOK;
}

} // namespace pn_rpc_server
} // namespace dstream


