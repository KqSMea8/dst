#include "logger.h"
#include "proto_rpc.h"
#include "pm_pn_rpc.pb.h"
#include "rpc_const.h"
#include "rpc_server.h"

using namespace dstream;
using namespace dstream::proto_rpc;
using namespace dstream::rpc;
using namespace dstream::rpc_const;
using namespace dstream::rpc_server;

namespace dstream {
class PNRPCServer : public CreatePEProtocol, public PEOperationProtocol, public RPCServer {
public:
    bool Init();
    std::string CREATEPE(const CreatePEMessage* msg, CreatePEResult* res);
    std::string PEOPERATION(const PEOperationMessage* msg, PEOperationResult* res);
};

bool PNRPCServer::Init()
{
    if (RPCServer::Init())
    {
        RegisterProtocol(kCreatePEMethod, (CreatePEProtocol*)this);
        RegisterProtocol(kPEOperationMethod, (PEOperationProtocol*)this);
        return true;
    }
    return false;
}

std::string PNRPCServer::CREATEPE(const CreatePEMessage* msg, CreatePEResult* res)
{
    DSTREAM_INFO("handle CreatePE request");
    return "ok";
}

std::string PNRPCServer::PEOPERATION(const PEOperationMessage* msg, PEOperationResult* res)
{
    DSTREAM_INFO("handle PEOperation request");
    return "ok";
}
}

int main(int argc, char* argv[])
{
    logger::initialize("pn_server");
    char* default_server = "tcp://127.0.0.1:18900";
    char* server;
    if (argc > 1) {
        server = argv[1];
    } else {
        server = default_server;
    }
    PNRPCServer   pn_server;
    if (pn_server.Init()) {
        if (pn_server.StartServer(server)) {
            DSTREAM_INFO("pn server listen on %s", server);
        } else {
            DSTREAM_WARN("pn server listen on %s fail", server);
        }
    } else {
        DSTREAM_WARN("pn server init fail");
    }
    return 0;
}
