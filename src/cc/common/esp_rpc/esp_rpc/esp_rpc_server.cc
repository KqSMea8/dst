#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include "esp_rpc/esp_rpc_server.h"
#include "esp_rpc/esp_rpc.pb.h"

namespace dstream {
namespace esp_rpc {

ESP_BEGIN_MAP_MESSAGE(ESPServer, CESPObject)
  ESP_MAP_MESSAGE(ESP_REQUEST_MSG, OnMsg)
ESP_END_MAP_MESSAGE

ESPServer::~ESPServer() {
    std::map<std::string, google::protobuf::Service*>::iterator it;
    for (it = m_service.begin(); it != m_service.end(); it++) {
        if (NULL != it->second) {
            delete it->second;
            it->second = NULL;
        }
    }
    m_service.clear();
}

int ESPServer::RegisterService(google::protobuf::Service* service) {
    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
    m_service[desc->full_name()] = service;
    return 0;
}

void ESPServer::OnMsg(ESPADDRESS addr, uint64 lParam, BufHandle* pHdl) {
    printf("Receive msg \n");
    int size = pHdl->nDataLen;
    RpcMessage message;
    message.ParseFromArray(pHdl->pBuf, size);

    CallActionCtx* ctx = new CallActionCtx();
    ctx->addr    = addr;
    ctx->message = message.New();
    ctx->message->CopyFrom(message);
    QueueExec(0, ESP_RPC_CALL_ACTION, this, ctx, false);
}

void ESPServer::OnCallAction(CallActionCtx* ctx) {
    printf("in OnCallAction\n");
    ESPADDRESS client_addr = ctx->addr;
    RpcMessage* message    = ctx->message;

    std::map<std::string, google::protobuf::Service*>::const_iterator it = m_service.find(message->service());
    if (it != m_service.end()) {
        google::protobuf::Service* service = it->second;
        const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
        const google::protobuf::MethodDescriptor* method =
            desc->FindMethodByName(message->method());
        if (NULL != method) {
            google::protobuf::Message* request = service->GetRequestPrototype(method).New();
            request->ParseFromString(message->request());
            google::protobuf::Message* response = service->GetResponsePrototype(method).New();
            ServiceCallbackCtx* ctx0 = new ServiceCallbackCtx();
            ctx0->addr     = client_addr;
            ctx0->id       = message->id();
            ctx0->response = response;
            service->CallMethod(method, NULL, request, response,
                google::protobuf::NewCallback(this, &ESPServer::DoneCallback, ctx0));
            delete request;
        }
    } else {
        printf("not suppoted\n");
    }
    delete ctx->message;
    delete ctx;
    return;
}

void ESPServer::DoneCallback(ServiceCallbackCtx* ctx) {
    RpcMessage message;
    message.set_type(RESPONSE);
    message.set_id(ctx->id);
    message.set_response(ctx->response->SerializeAsString());
    BufHandle* pHdl = AllocateHdlCanFail();
    pHdl->nDataLen  = message.ByteSize();
    message.SerializeToArray(pHdl->pBuf, message.ByteSize());
    if (this->PostMsg(ctx->addr, ESP_REQUEST_MSG_ACK, 1, pHdl, NULL) == ESPE_OK) {
      printf("Post msg sucess\n");
    } else {
      printf("Post msg failed\n");
    }
    ChainFreeHdl(pHdl, NULL);
    delete ctx->response;
    delete ctx;
    return;
}

void ESPServer::OnCompletion(AsyncContext* ctx) {
    if (LIKELY(ctx->nAction < AA_USER_BEGIN)) {
        CESPObject::OnCompletion(ctx);
        return;
    }
    switch (ctx->nAction) {
    case ESP_RPC_CALL_ACTION:
        return OnCallAction(static_cast<CallActionCtx*>(ctx));
    }
}

} // namespace esp_rpc
} // namespace dstream
