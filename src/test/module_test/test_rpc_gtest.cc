/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  RPC_Server Startup,check RPC_Client work or not. multi-Sever and multi-Client connection.Useing GTEST
 *
 */
#include <string>

#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"
#include "common/rpc.h"
#include <gtest/gtest.h>


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

static void sig_handler(int signo) {
    if (signo == SIGINT) {
        pconn->Stop();
    }
}
//--------------------------Gtest--------------------------------
int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


/**
 * @brief
**/
class RPCtest_suite : public ::testing::Test
{
protected:
    RPCtest_suite() {};
    virtual ~RPCtest_suite() {};
    virtual void SetUp()
    {
    };
    virtual void TearDown()
    {
    };
};
//---------------------------------------------------------------
void* server(void*) {
    //logger::initialize("rpc_server", logger::Priority::DEBUG);
    rpc::Context::init();
    rpc::Connection conn;
    pconn = &conn;
    signal(SIGINT, sig_handler);
    conn.RegisterService("echo", new EchoService());

    std::string addr("tcp://127.0.0.1:19870");
    //DSTREAM_INFO("serve %s\n", addr.c_str());
    if (conn.Serve(addr.c_str(), 10) < 0) {
        pthread_exit((void*)1);
    }
    pthread_exit((void*)0);
}


void* client(void*)
{
    sleep(2);

    //logger::initialize("rpc_client", logger::Priority::DEBUG);
    rpc::Context::init();
    rpc::Connection conn;
    std::string addr("tcp://127.0.0.1:19870");
    //DSTREAM_INFO("connect %s\n", addr.c_str());
    if (conn.Connect(addr.c_str()) < 0) {
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
    if (conn.Call(&req, &res, 0) < 0) {
        conn.Reconnect();
        //return -1;
    }
    req.set_method("echo");
    req.set_field("hello");
    if (conn.Call(&req, &res, 1000) < 0) {
        pthread_exit((void*)1);
    }
    pthread_exit((void*)0);
}


TEST_F(RPCtest_suite, test_basic)
{
    int client_num = 10;
    pthread_t id_s, id_c[client_num];
    int ret = 0;
    ret = pthread_create(&id_s, NULL, server, NULL);
    ASSERT_EQ(0, ret);
    if (ret != 0)
    {
        printf ("Create sever pthread error!\n");
        ASSERT_TRUE(0);
    }
    for (int i = 0; i < client_num; i++)
    {
        ret = pthread_create(&id_c[i], NULL, client, NULL);
        ASSERT_EQ(0, ret);
        if (ret != 0) {
            printf ("Create client pthread error!\n");
            ASSERT_TRUE(0);
        }
    }
    if (ret != 0)
    {
        printf ("Server pthread error!\n");
        ASSERT_TRUE(0);
    }
    for (int i = 0; i < client_num; i++)
    {
        ret = pthread_join(id_c[i], NULL);
        ASSERT_EQ(0, ret);
        if (ret != 0)
        {
            printf ("Client pthread error!\n");
            ASSERT_TRUE(0);
        }
    }
    pconn->Stop();
    ret = pthread_join(id_s, NULL);
    ASSERT_EQ(0, ret);
    ASSERT_TRUE(1);
}
