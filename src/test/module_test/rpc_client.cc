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

#include "common/rpc.h"
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"

using namespace dstream;

// ------------------------------------------------------------
int main(int argc, const char** argv) {
    logger::initialize("rpc_client");
    rpc::Context::init();
    rpc::Connection conn;
    std::string addr("tcp://127.0.0.1:19870");
    if (argc > 1) {
        addr = argv[1];
    }
    DSTREAM_INFO("connect %s\n", addr.c_str());
    if (conn.Connect(addr.c_str()) < 0) {
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
    if (conn.Call(&req, &res, 0) < 0) {
        conn.Reconnect();
        //return -1;
    }
    req.set_method("echo");
    req.set_field("hello");
    if (conn.Call(&req, &res, 1000) < 0) {
        return -1;
    }

    // ----------------------------------------
    // vector call.
    std::vector< const rpc::Request* > req_vec;
    std::vector< rpc::Response* > res_vec;
    // add request to req_vec.
    proto_rpc::RPCRequest<EchoRequest>* xreq = new proto_rpc::RPCRequest<EchoRequest>();
    xreq->set_method("echo");
    xreq->set_field("hello");
    req_vec.push_back(xreq);
    xreq = new proto_rpc::RPCRequest<EchoRequest>();
    xreq->set_method("echo");
    xreq->set_field("hello");
    req_vec.push_back(xreq);
    //allocate response to res_vec.
    res_vec.push_back(new proto_rpc::RPCResponse<EchoResponse> ());
    res_vec.push_back(new proto_rpc::RPCResponse<EchoResponse> ());
    if (conn.SendVector(req_vec) < 0) {
        return -1;
    }
    if (conn.RecvVector(res_vec, 1000) < 0) {
        return -1;
    }
    return 0;
}

