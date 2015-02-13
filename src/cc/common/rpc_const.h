/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief rpc const
 */

#ifndef __DSTREAM_CC_COMMON_RPC_CONST_H__ // NOLINT
#define __DSTREAM_CC_COMMON_RPC_CONST_H__ // NOLINT

#include <string>

#include "common/proto/application.pb.h"
#include "common/proto/cluster_config.pb.h"
#include "common/proto/pm.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "common/proto/pn.pb.h"
#include "common/proto/pub_sub.pb.h"
#include "common/proto_rpc.h"

namespace dstream {
namespace rpc_const {

// ------------------------------------------------------------
// rpc return value error string
extern const char* kOK;
extern const char* kFail;
bool IsReturnValueOK(std::string ret_val);
bool ReturnValueEqa(std::string ret_val, const char* err_str);

extern const char* kDefaultProtocol;
std::string GetUri(const std::string& host, int port);
std::string GetUri(const std::string& protocol, const std::string& host, int port);

// ------------------------------------------------------------
// debugger <-> pn related protocols.
// DEBUGGER->PN rpc const and type define
extern const char* kDebugPNMethod;
typedef proto_rpc::RPCService<DebugPNRequest, DebugPNACK> DebugPNProtocol;
#define DEBUGPN handleRequest

// ------------------------------------------------------------
// pm <-> pn related protocols.
// PM->PN rpc const and type define
extern const char* kCreatePEMethod;
typedef proto_rpc::RPCService<CreatePEMessage, CreatePEResult> CreatePEProtocol;
#define CREATEPE handleRequest

extern const char* kPEOperationMethod;
typedef proto_rpc::RPCService<PEOperationMessage, PEOperationResult> PEOperationProtocol;
#define PEOPERATION handleRequest

// PN->PM rpc const and type define
extern const char* kPNReport;
typedef proto_rpc::RPCService<PNReport, ReportResponse> PNReportProtocol;
#define PNREPORT handleRequest

// ------------------------------------------------------------
// pn <-> pe related protocols.
extern const char* kPubSubReadyMethod;
typedef proto_rpc::RPCService<ReadyRequest, ReadyACK> PubSubReadyProtocol;
#define PUB_SUB_READY handleRequest

extern const char* kPubSubDoCheckpointMethod;
typedef proto_rpc::RPCService<DoCheckpointRequest, DoCheckpointACK> PubSubDoCheckpointProtocol;
#define PUB_SUB_DO_CHECKPOINT handleRequest

extern const char* kPubSubUploadCheckpointMethod;
typedef proto_rpc::RPCService<UploadCheckpointRequest, UploadCheckpointACK>
PubSubUploadCheckpointProtocol;
#define PUB_SUB_UPLOAD_CHECKPOINT handleRequest

extern const char* kPubSubReplayMethod;
typedef proto_rpc::RPCService<ReplayRequest, ReplayACK > PubSubReplayProtocol;
#define PUB_SUB_REPLAY handleRequest

extern const char* kPubSubSubscribeMethod;
typedef proto_rpc::RPCService<SubscribeRequest, SubscribeACK> PubSubSubscribeProtocol;
#define PUB_SUB_SUBSCRIBE handleRequest

extern const char* kPubSubSubscribeImporterMethod;
typedef proto_rpc::RPCService<SubscribeImporterRequest, SubscribeImporterACK>
PubSubSubscribeImporterProtocol;
#define PUB_SUB_SUBSCRIBE_IMPORTER handleRequest


extern const char* kPubSubFetchLogMethod;
typedef proto_rpc::RPCService<FetchLogRequest, FetchLogACK> PubSubFetchLogProtocol;
#define PUB_SUB_FETCH_LOG handleRequest

extern const char* kPubSubForwardLogMethod;
typedef proto_rpc::RPCService<ForwardLogRequest, ForwardLogACK> PubSubForwardLogProtocol;
#define PUB_SUB_FORWARD_LOG handleRequest

extern const char* kPubSubHeartbeatMethod;
typedef proto_rpc::RPCService<HeartbeatRequest, HeartbeatACK> PubSubHeartbeatProtocol;
#define PUB_SUB_HEARTBEAT handleRequest

// ------------------------------------------------------------
// pn <-> pn related protocols.
extern const char* kPubSubSubscribePNMethod;
typedef proto_rpc::RPCService<SubscribeRequest, SubscribeACK> PubSubSubscribePNProtocol;
#define PUB_SUB_SUBSCRIBE_PN handleRequest

extern const char* kPubSubFetchLogPNMethod;
typedef proto_rpc::RPCService<FetchLogRequest, FetchLogACK> PubSubFetchLogPNProtocol;
#define PUB_SUB_FETCH_LOG_PN handleRequest

// ------------------------------------------------------------
// client -> pm related protocols.
// rpc const and type define
// typedef Ack LoginAck;
typedef Ack SubmitAck;
typedef Ack KillAppAck;
typedef Ack NotifyAck;
typedef Ack KillPEAck;
typedef Ack KillPNAck;
typedef Ack UpdateProAck;

extern const char* kLoginMethod;
typedef proto_rpc::RPCService<User, LoginAck>            LoginProtocol;
#define LOGIN handleRequest

extern const char* kApplyAppIDMethod;
typedef proto_rpc::RPCService<AppID, AppID>              ApplyAppIDProtocol;
#define ASKFORAPPID  handleRequest

extern const char* kSubmitAppMethod;
typedef proto_rpc::RPCService<Application, FileConfig>   SubmitAppProtocol;
#define SUBMITAPP handleRequest

extern const char* kUpdateTopologyMethod;
typedef proto_rpc::RPCService<UpdateTopology, UpdateTopology>UpdateTopologyProtocol;
#define UPDATETOPOLOGY handleRequest

extern const char* kAddNewTopoMethod;
typedef proto_rpc::RPCService<Application, NotifyAck>   AddNewTopoProtocol;
#define ADDNEWTOPO handleRequest

extern const char* kNotifyPMMethod;
typedef proto_rpc::RPCService<NotifyPM, NotifyAck>       NotifyPMProtocol;
#define FILEUPLOADDONE handleRequest

extern const char* kKillAppMethod;
typedef proto_rpc::RPCService<Application, AppID>           KillAppProtocol;
#define KILLAPP handleRequest

extern const char* kKillPEMethod;
typedef proto_rpc::RPCService<KillPE, KillPEAck>         KillPEProtocol;
#define KILLPE handleRequest

extern const char* kKillPNMethod;
typedef proto_rpc::RPCService<PNID, KillPNAck>        KillPNProtocol;
#define KILLPN handleRequest

extern const char* kUserOpMethod;
typedef proto_rpc::RPCService<UserOperation, UserOperationReply>  UserOpProtocol;
#define USEROP handleRequest

extern const char* kFlowControlMethod;
typedef proto_rpc::RPCService<FlowControlRequest, FlowControlResponse> FlowControlProtocol;
#define FLOWCONTROL handleRequest

extern const char* kUpdateProcessorMethod;
typedef proto_rpc::RPCService<UpdateProRequest, UpdateProResponse> UpdateProcessorProtocol;
#define UPDATEPROCESSOR handleRequest

extern const char* kUpdateProDoneMethod;
typedef proto_rpc::RPCService<UpdateProDone, UpdateProAck> UpdateProDoneProtocol;
#define UPDATEPRODONE handleRequest

extern const char* kGetMetricMethod;
typedef proto_rpc::RPCService<StatusRequest, StatusResponse> GetMetricProtocol;
#define GETMETRIC handleRequest

// utils for get or set black list info
extern const char* kBlackListMethod;
typedef proto_rpc::RPCService<BlackListOperationRequest, BlackListOperationResponse>  BlackListOperationProtocol;
#define BLACKLISTOPERATION handleRequest

} // namespace rpc_const
} // namespace dstream

#endif // NOLINT
