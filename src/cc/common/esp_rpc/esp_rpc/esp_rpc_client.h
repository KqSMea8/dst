#ifndef __ESP_RPC_ESP_RPC_CLIENT_H__
#define __ESP_RPC_ESP_RPC_CLIENT_H__

#include "stdafx.h"
#include "Kylin.h"
#include "ESP.h"
#include "ESPSystem.h"
#include "esp_rpc/esp_rpc_common.h"

namespace dstream {
namespace esp_rpc {

class ESPClient : public CESPObject {
    ESP_DECLARE_OBJECT(ESPClient)
public:
    ESPClient() : m_id(0) {}
    virtual ~ESPClient() {}
    static const uint16 STUB_ID = 13;
    void Init() {
        this->BindStub(ESPClient::STUB_ID);
    }

    int AddTask(ESPADDRESS server_addr,
                CallMethodCtx* ctx);
    int PostRequest(ESPADDRESS server_addr);
    void OnMsgAck(ESPADDRESS addr, uint64 lParam, BufHandle* pHdl);

    void OnCallAction(CallActionCtx* ctx);
    void OnCompletion(AsyncContext* ctx);

private:
    int m_id;
    std::map<int, ClientCallbackCtx*> m_client_cb;
};

} // namespace esp_rpc
} // namespace dstream

#endif
