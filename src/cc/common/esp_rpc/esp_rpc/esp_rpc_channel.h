#ifndef __ESP_RPC_RPC_CHANNEL_H__
#define __ESP_RPC_RPC_CHANNEL_H__

#include <google/protobuf/service.h>
#include "ESP.h"

namespace dstream {
namespace esp_rpc {

class ESPClient;

class RpcChannel : public google::protobuf::RpcChannel {
public:
    RpcChannel(ESPADDRESS server_addr, ESPClient* rpc_client);
    virtual ~RpcChannel();
    virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done);
private:
    ESPADDRESS m_server_addr;
    ESPClient* m_rpc_client; 
};

} // namespace esp_rpc
} // namespace dstream

#endif
