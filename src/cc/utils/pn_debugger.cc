/** 
* @file     pn_debugger.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include "common/config.h"
#include "common/config_const.h"
#include "common/logger.h"
#include "common/proto/pub_sub.pb.h"
#include "common/rpc_const.h"
#include "common/utils.h"
#include "common/zk_client.h"

#include "processnode/config_map.h"
#include "processnode/process_node.h"

using namespace std;
using namespace dstream;
using namespace dstream::rpc;
using namespace dstream::processnode;


int main(int argc, char** argv) {
    logger::initialize("pn_debugger");

    if (argc < 3) {
        printf("usages : %s <pn_uri> <debug_cmd> [debug_arg]\n", argv[0]);
        printf("debug_cmd : 'allpe' -- show all PEs on this pn.\n");
        printf("            'pe' #peid -- show #peid PE on this pn.\n");
        printf("            'conn' -- show conn info on this pn.\n");
        printf("            'queue' -- show queue info on this pn.\n");
        printf("            'set_log_level' -- set pn log level.\n");
        printf("            'pe_id_list' -- print peid list with white space.\n");
        printf("            'quit' -- order to quit.\n");
        return 0;
    }

    rpc::Connection conn;
    std::string pn_uri = argv[1];
    DSTREAM_DEBUG("debug pn : [%s]\n", pn_uri.c_str());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_ERROR("connect to pn failed\n");
        return -1;
    }
    DSTREAM_DEBUG("connect to pn ...OK\n");

    proto_rpc::RPCRequest< DebugPNRequest > req;
    proto_rpc::RPCResponse< DebugPNACK > res;
    req.set_method(rpc_const::kDebugPNMethod);
    // set cmd & args
    std::string debug_cmd = argv[2];
    req.set_cmd(debug_cmd);
    if (debug_cmd != "allpe"
        && debug_cmd != "conn"
        && debug_cmd != "queue"
        && argc == 4) {
        std::string debug_args = argv[3];
        req.set_args(debug_args);
    }

    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_ERROR("rpc all to pn failed\n");
        return -1;
    }
    DSTREAM_DEBUG("rpc call pn : cmd[%s]\n", pn_uri.c_str());

    // print debug result
    if (res.has_debug_info()) {
        printf("%s\n", res.debug_info().c_str());
    } else {
        printf("no debug info\n");
    }
    return 0;
}
