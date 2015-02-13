/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <signal.h>
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"
using namespace dstream;

static rpc::Connection* pconn;

class EchoService:
    public proto_rpc::RPCService< EchoRequest, EchoResponse > {
public:
    std::string handleRequest(const EchoRequest* req, EchoResponse* res) {
        res->set_field(req->field());
        return "OK";
    }
};

// ------------------------------------------------------------
static void sig_handler(int signo) {
    if (signo == SIGINT) {
        pconn->Stop();
    }
}

void fn(void* data, void* /*hint*/) {
    free(data);
}

void print_usage(const char* program) {
    printf("Usage example: %s -t 8 -h tcp://10.237.22.17:19870\n", program);
    printf("'-t' is the server threads, '-h' is the server addr\n");
    return;
}

std::string addr("tcp://127.0.0.1:19870");
int main(int argc, char** argv) {
    int server_threads = 8;

    // get options
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h:t:")) != -1) {
        switch (opt) {
        case 'h':
            addr = optarg;
            break;
        case 't':
            server_threads = atoi(optarg);
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }
    printf(" ====> server addr = %s\n", addr.c_str());
    printf(" ====> sercer threads = %d\n", server_threads);

    logger::initialize("rpc_server");
    rpc::Context::init();
    rpc::Connection conn;
    pconn = &conn;
    signal(SIGINT, sig_handler);
    conn.RegisterService("echo", new EchoService());
    DSTREAM_NOTICE("serve %s\n", addr.c_str());
    if (conn.Serve(addr.c_str(), server_threads) < 0) {
        return -1;
    }
    return 0;
}
