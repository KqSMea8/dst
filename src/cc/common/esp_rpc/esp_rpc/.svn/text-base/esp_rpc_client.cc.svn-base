#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <google/protobuf/stubs/common.h>
#include "esp_rpc/esp_rpc_client.h"
#include "esp_rpc/esp_rpc.pb.h"

namespace dstream {
namespace esp_rpc {

ESP_BEGIN_MAP_MESSAGE(ESPClient, CESPObject)
  ESP_MAP_MESSAGE(ESP_REQUEST_MSG_ACK, OnMsgAck)
ESP_END_MAP_MESSAGE

int ESPClient::AddTask(ESPADDRESS server_addr,
                       CallMethodCtx* call_ctx) {
    // construct msg
    RpcMessage message;
    message.set_type(REQUEST);
    int id = AtomicInc(m_id);
    message.set_id(id);
    message.set_service(call_ctx->method->service()->full_name());
    message.set_method(call_ctx->method->name());
    message.set_request(call_ctx->request->SerializeAsString());

    // put response and done into map
    ClientCallbackCtx* cb_ctx = new ClientCallbackCtx();
    cb_ctx->response = call_ctx->response;
    cb_ctx->done     = call_ctx->done;
    m_client_cb[id] = cb_ctx;

    // rpc call
    CallActionCtx* ctx = new CallActionCtx();
    ctx->addr    = server_addr;
    ctx->message = message.New();
    ctx->message->CopyFrom(message);
    QueueExec(0, ESP_RPC_CALL_ACTION, this, ctx, false);
    return 0;
};

void ESPClient::OnMsgAck(ESPADDRESS addr, uint64 lParam, BufHandle* pHdl) {
    printf("Receive msg ack\n");
    RpcMessage message;
    message.ParseFromArray(pHdl->pBuf, pHdl->nDataLen);

    ClientCallbackCtx* ctx;
    std::map<int, ClientCallbackCtx*>::iterator it = m_client_cb.find(message.id());
    if (it != m_client_cb.end()) {
        ctx = it->second;
        m_client_cb.erase(it);
    }
    if (NULL != ctx) {
        if (NULL != ctx->response) {
            ctx->response->ParseFromString(message.response());
            if (NULL != ctx->done) {
                ctx->done->Run();
            }
            delete ctx->response;
        }
        delete ctx;
    }
}

void ESPClient::OnCallAction(CallActionCtx* ctx) {
    ESPADDRESS server_addr = ctx->addr;
    RpcMessage* msg        = ctx->message;
    BufHandle* pHdl = AllocateHdlCanFail();
    pHdl->nDataLen  = msg->ByteSize();
    msg->SerializeToArray(pHdl->pBuf, msg->ByteSize());
    if (PostMsg(server_addr, ESP_REQUEST_MSG, 1, pHdl, NULL) == ESPE_OK) {
        printf("Post msg sucess\n");
    } else {
        printf("Post msg failed\n");
    }
    delete ctx;
    return;
}

void ESPClient::OnCompletion(AsyncContext* ctx) {
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
