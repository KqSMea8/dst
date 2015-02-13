/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief
 *
 */

#include "common/rpc.h"
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"
//#include "gtest/gtest.h"

using namespace dstream;
using namespace std;

#define ECHO_BUF_MAX_SIZE (1024*1024*256)
static char* kContent;

std::string addr("tcp://127.0.0.1:19870");
void print_usage(const char* program) {
    printf("Usage example: %s -t 10 -s 1024 -h tcp://10.237.22.17:19870\n", program);
    printf("'-t' is time out (ms) 0 means no timeout, '-s' is the echo buff size, '-h' is the server addr\n");
    return;
}

// ------------------------------------------------------------
int main(int argc, char** argv) {
    int time_out = 0;
    int echo_str_len = 1024;

    // get options
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h:t:s:")) != -1) {
        switch (opt) {
        case 'h':
            addr = optarg;
            break;
        case 't':
            time_out = atoi(optarg);
            break;
        case 's':
            echo_str_len = atoi(optarg);
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }

    kContent = new char [echo_str_len + 1];
    memset(kContent, 'x', echo_str_len);
    kContent[echo_str_len] = 0;

    logger::initialize("rpc_client");
    rpc::Context::init();
    rpc::Connection conn;

    DSTREAM_INFO("connect %s\n", addr.c_str());
    if (conn.Connect(addr.c_str()) < 0) {
        DSTREAM_WARN("connect failed!\n");
        return -1;
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
    if (conn.Call(&req, &res, time_out) < 0) {
        DSTREAM_WARN("send rpc all failed and reconnect ...\n");
        if (conn.Reconnect() < 0) {
            DSTREAM_WARN("reconnect failed!\n");
            return -1;
        }
    }

    // another try
    req.set_method("echo");
    req.set_field("test_simple_1");

RECALL:
    if (conn.Call(&req, &res, 1000) < 0) {
        DSTREAM_WARN("send rpc all failed and recall!\n");
        sleep(2);
        conn.Reconnect();
        goto RECALL;
    }
    cout << "=======>recieve echo:" << dynamic_cast<proto_rpc::RPCResponse<EchoResponse>*>
         (&res)->field() << endl;

    // ----------------------------------------
    // vector call.
    std::vector< const rpc::Request* > req_vec;
    std::vector< rpc::Response* > res_vec;
    // add request to req_vec.
    proto_rpc::RPCRequest<EchoRequest>* xreq = new proto_rpc::RPCRequest<EchoRequest>();
    xreq->set_method("echo");
    xreq->set_field("test_vector_1_1");
    req_vec.push_back(xreq);
    xreq = new proto_rpc::RPCRequest<EchoRequest>();
    xreq->set_method("echo");
    xreq->set_field("test_vector_1_2");
    req_vec.push_back(xreq);
    //allocate response to res_vec.
    res_vec.push_back(new proto_rpc::RPCResponse<EchoResponse> ());
    res_vec.push_back(new proto_rpc::RPCResponse<EchoResponse> ());
    //res_vec.push_back(new proto_rpc::RPCResponse<EchoResponse> ());
    if (conn.SendVector(req_vec) < 0) {
        return -1;
    }
    if (conn.RecvVector(res_vec, 1000) < 0) {
        return -1;
    } else {
        cout << "========>recieve vector echo, vector size : " << res_vec.size() << endl;
        for (unsigned int i = 0; i < res_vec.size(); i++) {
            //proto_rpc::RPCResponse<EchoResponse>* xres=dynamic_cast<proto_rpc::RPCResponse<EchoResponse>*>(res_vec[i]);
            cout << "=======>recieve echo:" << dynamic_cast<proto_rpc::RPCResponse<EchoResponse>*>
                 (res_vec[i])->field() << endl;
        }
    }

    return 0;
}

