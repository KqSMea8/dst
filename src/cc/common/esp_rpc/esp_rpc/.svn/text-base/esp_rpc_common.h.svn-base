#ifndef __ESP_RPC_ESP_RPC_COMMON_H__
#define __ESP_RPC_ESP_RPC_COMMON_H__

#include "esp_rpc/esp_rpc.pb.h"

namespace google {
namespace protobuf {

class Message;
class MethodDescriptor;
class Closure;
class RpcController;

} // namespace protobuf
} // namespace google

namespace dstream {
namespace esp_rpc {

enum RPCAction {
    ESP_RPC_ACTION_BEGIN = AA_USER_BEGIN,
    ESP_RPC_CALL_ACTION  = ESP_RPC_ACTION_BEGIN + 1,
};

enum RPCMsg {
    ESP_RPC_BEGIN        = 0,
    ESP_REQUEST_MSG      = ESP_RPC_BEGIN + 1,
    ESP_REQUEST_MSG_ACK  = ESP_RPC_BEGIN + 2,
    ESP_RESPONSE_MSG     = ESP_RPC_BEGIN + 3,
    ESP_RESPONSE_MSG_ACK = ESP_RPC_BEGIN + 4,

    ESP_RPC_END          = ESP_RPC_BEGIN + 100,
};

struct CallActionCtx : public AsyncContext {
    ESPADDRESS    addr;
    RpcMessage*   message;
};

struct CallMethodCtx {
    google::protobuf::MethodDescriptor* method;
    google::protobuf::RpcController*    controller;
    google::protobuf::Message*          request;
    google::protobuf::Message*          response;
    google::protobuf::Closure*          done;
};

struct ServiceCallbackCtx {
    ESPADDRESS                 addr;
    google::protobuf::Message* response;
    int                        id;
};

struct ClientCallbackCtx {
    google::protobuf::Message* response;
    google::protobuf::Closure* done;
};

} // namespace esp_rpc
} // namespace dstream

#endif
