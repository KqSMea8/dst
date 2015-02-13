#include "logger.h"
#include "pm_pn_rpc.pb.h"
#include "proto_serializer.h"
#include "proto_rpc.h"
#include "rpc_client.h"
#include "rpc_const.h"

using namespace dstream;
using namespace dstream::proto_rpc;
using namespace dstream::rpc;
using namespace dstream::rpc_client;
using namespace dstream::rpc_const;

int main(int argc, char* argv[])
{
    logger::initialize("PM");
    char* default_server = "tcp://127.0.0.1:18900";
    char* server;
    if (argc > 1)
    {
        server = argv[1];
    } else
    {
        server = default_server;
    }

    rpc::Context::init();
    CreatePEMessage request;
    CreatePEResult response;
    if (rpc_call(server, kCreatePEMethod, request, response) < 0) {
        DSTREAM_WARN("call CreatePE method fail");
    } else {
        DSTREAM_INFO("call CreatePE method success");
    }
    PEOperationMessage op_request;
    PEOperationResult op_response;
    if (rpc_call(server, kPEOperationMethod, op_request, op_response) < 0) {
        DSTREAM_INFO("call PEOperation method fail\n");
    } else {
        DSTREAM_WARN("call PEOperation method success");
    }
}
