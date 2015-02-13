#include "stdafx.h"
#include "Kylin.h"
#include "ESP.h"
#include "ESPSystem.h"
#include "esp_rpc/esp_rpc_server.h"
#include "echo.pb.h"

using namespace dstream;
using namespace dstream::esp_rpc;

class EchoServiceImpl : public EchoService {
public:
    virtual void Echo(::google::protobuf::RpcController* controller,
                      const EchoRequest* request,
                      EchoResponse* response,
                      ::google::protobuf::Closure* done) {
        printf("receive request %s\n", request->request().c_str());
        response->set_response("world");
        if (done) {
            done->Run();
        }
    }
};

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("Usage: %s [port]\n", argv[0]);
    return -1;
  }
  CESPSystem esp;
  ESPServer server;
  KLSetLog(stderr, 4, NULL);
  SetTranBuf(1024 * 1024, 64, 4096, 0.9, 0.9);
  esp.Init(5, 2, 0, 0, 100);
  uint16 port = atoi(argv[1]);
  esp.Bind(port);
  server.Init();
  EchoServiceImpl service;
  server.RegisterService(&service);
  while(1) {
    sleep(1);
  }
  esp.StopNetwork();
  esp.Stop(true);
  return 0;
}
