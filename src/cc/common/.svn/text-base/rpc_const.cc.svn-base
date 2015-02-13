#include "common/rpc_const.h"

#include <stdlib.h>

namespace dstream {
namespace rpc_const {
using namespace proto_rpc;

const char* kDefaultProtocol = "tcp";

std::string GetUri(const std::string& host, int port) {
    return GetUri(kDefaultProtocol, host, port);
}

std::string GetUri(const std::string& protocol, const std::string& host, int port) {
    char  uri[400];
    snprintf(uri, sizeof(uri), "%s://%s:%d", protocol.c_str(), host.c_str(), port);
    return uri;
}
// ------------------------------------------------------------
// rpc return value error string
const char* kOK   = "OK";
const char* kFail = "FAILED";
bool IsReturnValueOK(std::string ret_val) {
    return ret_val == std::string(kOK);
}

bool ReturnValueEqa(std::string ret_val, const char* err_str) {
    return ret_val == std::string(err_str);
}

// ------------------------------------------------------------
// debugger <-> pn
const char* kDebugPNMethod = "DebugPN";


// ------------------------------------------------------------
// pm <-> pn
const char* kCreatePEMethod = "CreatePE";
const char* kPEOperationMethod = "PEOperation";
const char* kPNReport = "PNReport";


// ------------------------------------------------------------
// pe <-> pn
const char* kPubSubReadyMethod             = "pub_sub.Ready";
const char* kPubSubDoCheckpointMethod      = "pub_sub.DoCheckpoint";
const char* kPubSubUploadCheckpointMethod  = "pub_sub.UploadCheckpoint";
const char* kPubSubReplayMethod            = "pub_sub.Replay";
const char* kPubSubSubscribeMethod         = "pub_sub.Subscribe";
const char* kPubSubSubscribeImporterMethod = "pub_sub.SubscribeImporter";
const char* kPubSubFetchLogMethod          = "pub_sub.FetchLog";
const char* kPubSubForwardLogMethod        = "pub_sub.ForwardLog";
const char* kPubSubHeartbeatMethod         = "pub_sub.Heartbeat";

// ------------------------------------------------------------
// pn <-> pn
const char* kPubSubSubscribePNMethod = "pub_sub.SubscribePN";
const char* kPubSubFetchLogPNMethod  = "pub_sub.FetchLogPN";

// ------------------------------------------------------------
// client -> pm
const char* kLoginMethod           = "client.Login";
const char* kApplyAppIDMethod      = "client.ApplyID";
const char* kSubmitAppMethod       = "client.SubmitApp";
const char* kUpdateTopologyMethod  = "client.UpdateTopo";
const char* kAddNewTopoMethod      = "client.AddNewTopo";
const char* kNotifyPMMethod        = "client.NotifyPM";
const char* kKillAppMethod         = "client.KillApp";
const char* kKillPEMethod          = "client.KillPE";
const char* kKillPNMethod          = "client.KillPN";
const char* kUserOpMethod          = "client.UserOp";
const char* kFlowControlMethod     = "client.FlowControl";
const char* kUpdateProcessorMethod = "client.UpdateProcessor";
const char* kUpdateProDoneMethod   = "client.UpdateProDone";
const char* kGetMetricMethod       = "GetMetric";
const char* kBlackListMethod    = "utils.BlackListOperate";

} // namespace rpc_const
} // namespace dstream
