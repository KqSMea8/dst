#ifndef __ESP_RPC_ESP_RPC_SERVER_H__
#define __ESP_RPC_ESP_RPC_SERVER_H__

#include "stdafx.h"
#include "Kylin.h"
#include "ESP.h"
#include "ESPSystem.h"
#include "esp_rpc/esp_rpc_common.h"

namespace google {
namespace protobuf {

class Service;

} // namespace protobuf
} // namespace google

namespace dstream {
namespace esp_rpc {

class ESPServer : public CESPObject {
    ESP_DECLARE_OBJECT(ESPServer)
public:
    ESPServer() {}
    virtual ~ESPServer();
    static const uint16 STUB_ID = 14;
    void Init() {
      this->BindStub(ESPServer::STUB_ID);
    }

public:
    int RegisterService(google::protobuf::Service* service);

    void OnMsg(ESPADDRESS addr, uint64 lParam, BufHandle* pHdl);
    void OnCallAction(CallActionCtx* ctx);
    void OnCompletion(AsyncContext* ctx);

private:
    void DoneCallback(ServiceCallbackCtx* ctx);
    std::map<std::string, google::protobuf::Service*> m_service;  
};

} // namespace esp_rpc
} // namespace dstream

#endif
