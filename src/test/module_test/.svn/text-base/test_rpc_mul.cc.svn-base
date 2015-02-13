/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  RPC_Server Startup,check RPC_Client work or not. multi-Sever and multi-Client connection.
 *
 */
#include <string>

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"
#include "common/rpc.h"

using namespace dstream;
using std::string;

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
        pconn->stop();
    }
}
void* server(void*) {
    logger::initialize("rpc_server");
    rpc::Context::init();
    rpc::Connection conn;
    pconn = &conn;
    signal(SIGINT, sig_handler);
    conn.registerService("echo", new EchoService());

    std::string addr("tcp://127.0.0.1:19870");
    DSTREAM_INFO("serve %s\n", addr.c_str());
    if (conn.serve(addr.c_str(), 10) < 0) {
        pthread_exit((void*)1);
    }
    pthread_exit((void*)0);
}


void* client(void*)
{
    logger::initialize("rpc_client");
    rpc::Context::init();
    rpc::Connection conn;
    std::string addr("tcp://127.0.0.1:19870");
    DSTREAM_INFO("connect %s\n", addr.c_str());
    if (conn.connect(addr.c_str()) < 0) {
        pthread_exit((void*)1);
    }

    // ----------------------------------------
    // singular call.
    proto_rpc::RPCRequest<EchoRequest> req;
    proto_rpc::RPCResponse<EchoResponse> res;
    req.set_method("echo");
    req.set_field("hello");
    // set a short period.
    // to test whether the connection is still OK.
    // but it turns out we need to reconnect.
    if (conn.call(&req, &res, 0) < 0) {
        conn.reconnect();
        //return -1;
    }
    req.set_method("echo");
    req.set_field("hello");
    if (conn.call(&req, &res, 1000) < 0) {
        pthread_exit((void*)1);
    }
    pthread_exit((void*)0);
}


int main(int argc, char** argv)
{
    int client_num = 10;
    pthread_t id_s, id_c[client_num];
    int ret = 0;
    ret = pthread_create(&id_s, NULL, server, NULL);
    if (ret != 0)
    {
        printf ("Create sever pthread error!\n");
        exit (1);
    }
    for (int i = 0; i < client_num; i++)
    {
        ret = pthread_create(&id_c[i], NULL, client, NULL);
        if (ret != 0) {
            printf ("Create client pthread error!\n");
            exit (2);
        }
    }
    ret = pthread_join(id_s, NULL);
    if (ret != 0)
    {
        printf ("Server pthread error!\n");
        exit (3);
    }
    for (int i = 0; i < client_num; i++)
    {
        ret = pthread_join(id_c[i], NULL);
        if (ret != 0)
        {
            printf ("Client pthread error!\n");
            exit (4);
        }
    }
    return 0;
}
