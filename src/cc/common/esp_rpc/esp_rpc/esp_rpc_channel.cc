#include <google/protobuf/descriptor.h>
#include "esp_rpc/esp_rpc_channel.h"
#include "esp_rpc/esp_rpc_client.h"

namespace dstream {
namespace esp_rpc {

RpcChannel::RpcChannel(ESPADDRESS server_addr,
                       ESPClient* rpc_client) : m_server_addr(server_addr),
                                                m_rpc_client(rpc_client) {
}

RpcChannel::~RpcChannel() {
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done) {
    CallMethodCtx* ctx = new CallMethodCtx();
    ctx->method = const_cast<google::protobuf::MethodDescriptor*>(method);
    ctx->controller = controller;
    ctx->request = const_cast<google::protobuf::Message*>(request);
    ctx->response = response;
    ctx->done = done;
    m_rpc_client->AddTask(m_server_addr, ctx);
    delete ctx;
}

} // namespace esp_rpc
} // namespace dstream
