/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:      2012/05/25
* @filename:    pn_rpc_server.h
* @author:      zhanggengxin
* @brief:       pn rpc server
*/

#ifndef __DSTREAM_CC_PROCESSNODE_PN_RPC_SERVER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PN_RPC_SERVER_H__ // NOLINT

#include <map>
#include <string>
#include "common/dstream_type.h"
#include "common/rpc_const.h"
#include "common/rpc_server.h"
#include "processnode/process_node.h"

namespace dstream {
namespace pn_rpc_server {

class PNPMRPCServer : public rpc_server::RPCServer,
    public rpc_const::CreatePEProtocol,
    public rpc_const::PEOperationProtocol {
public:
    explicit PNPMRPCServer(processnode::ProcessNode* pn) : m_process_node(pn) {
        RegisterProtocol(rpc_const::kCreatePEMethod, (rpc_const::CreatePEProtocol*)this);
        RegisterProtocol(rpc_const::kPEOperationMethod, (rpc_const::PEOperationProtocol*)this);
    }
    std::string CREATEPE(const CreatePEMessage* req, CreatePEResult* ack);
    std::string PEOPERATION(const PEOperationMessage* req, PEOperationResult* result);

private:
    processnode::ProcessNode* m_process_node;
};

class PNDebugRPCServer : public rpc_server::RPCServer,
    public rpc_const::DebugPNProtocol {
public:
    explicit PNDebugRPCServer(processnode::ProcessNode* pn) : m_process_node(pn) {
        RegisterProtocol(rpc_const::kDebugPNMethod, (rpc_const::DebugPNProtocol*)this);
    }
    std::string DEBUGPN(const DebugPNRequest* req, DebugPNACK* ack);

private:
    processnode::ProcessNode* m_process_node;
};

class PNPERPCServer : public rpc_server::RPCServer,
    public rpc_const::PubSubReadyProtocol,
    public rpc_const::PubSubHeartbeatProtocol {
public:
    explicit PNPERPCServer(processnode::ProcessNode* pn) : m_process_node(pn) {
        RegisterProtocol(rpc_const::kPubSubReadyMethod, (rpc_const::PubSubReadyProtocol*)this);
        RegisterProtocol(rpc_const::kPubSubHeartbeatMethod,
                         (rpc_const::PubSubHeartbeatProtocol*)this);
    }

    std::string PUB_SUB_READY(const ReadyRequest* /*req*/, ReadyACK* /*ack*/);
    std::string PUB_SUB_HEARTBEAT(const HeartbeatRequest* req, HeartbeatACK* ack);

private:
    processnode::ProcessNode* m_process_node;
};



} // namespace pn_rpc_server
} // namespace dstream

#endif // NOLINT

