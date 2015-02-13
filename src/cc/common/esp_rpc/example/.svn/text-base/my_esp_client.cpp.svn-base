#include "stdafx.h"
#include "Kylin.h"
#include "ESP.h"
#include "ESPSystem.h"
#include "esp_rpc/esp_rpc_channel.h"
#include "esp_rpc/esp_rpc_client.h"
#include "echo.pb.h"

using namespace dstream;
using namespace dstream::esp_rpc;

void EchoCallback(EchoResponse* response) {
    printf("Call back! with response %s\n", response->response().c_str());
}

int main(int argc, char** argv) {
  if (argc < 3) {
    printf("Usage: %s [host][port]\n", argv[0]);
    return -1;
  }
  CESPSystem esp;
  ESPClient client;
  KLSetLog(stderr, 4, NULL);
  SetTranBuf(1024 * 1024, 64, 4096, 0.9, 0.9);
  esp.Init(5, 2, 0, 0, 100);

  char server_host[64];
  snprintf(server_host, 64, "%s:%s", argv[1], argv[2]);
  printf("get server host : %s\n", server_host);
  ESPNODE serv_esp_node = ESPFindNode(server_host);
  if (serv_esp_node == INVALID_ESPNODE) {
    fprintf(stderr, "parameter is invalid %s:%s\n", argv[1], argv[2]);
    return -1;
  }
  client.Init();
  ESPADDRESS server_addr;
  server_addr = MakeESPADDR(serv_esp_node, 14);

  RpcChannel *channel = new RpcChannel(server_addr, &client);
  EchoService::Stub stub(channel);
  EchoRequest req;
  req.set_request("hello");
  EchoResponse response;
  stub.Echo(NULL, &req, &response,
    NewCallback(EchoCallback, &response));

  // while (1) {
      sleep(2);
  //}
  delete channel;
  return 0;
}
