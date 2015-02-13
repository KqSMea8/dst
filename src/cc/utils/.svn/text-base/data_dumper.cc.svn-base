/** 
* @file     data_dumper.cc
* @brief    
* @author   konghui, konghui@baidu.com
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __GNU_LIBRARY__  // for kylin, add by konghui
#define __GNU_LIBRARY__
#endif

#include <getopt.h>

#include <iostream> // NOLINT
#include <sstream>

#include "common/error.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "common/proto/pn.pb.h"
#include "common/proto_rpc.h"
#include "common/rpc_client.h"
#include "common/rpc_const.h"
#include "google/protobuf/text_format.h"
#include "metamanager/zk_meta_manager.h"

const char* kDefaultConnectTimeoutMiniSec = "1000";
#define CheckCall(function, arg1) \
do {\
    dstream::error::Code code = function(arg1);\
    if (code != dstream::error::OK) {\
        DSTREAM_ERROR("in function: "#function": [%s]", get_text(code));\
    }\
} while (0)
#define CheckMemCall(obj, function, ...)\
do {\
    dstream::error::Code code = (obj).function(__VA_ARGS__);\
    if (code != dstream::error::OK) {\
        DSTREAM_ERROR("in function: "#function": [%s]", get_text(code));\
    }\
} while (0)
const char* kZkConfigPath = "../../../conf/pn.cfg.xml";
bool GetIpPort(std::string* host, int* port, const Config& config) {
    assert(NULL != host && NULL != port);
    if (!dstream::meta_manager::MetaManager::InitMetaManagerContext(&config)) {
        DSTREAM_ERROR("InitMetaManagerContext error");
        exit(0);
    }
    DSTREAM_INFO("init MetaManagerContext success.");
    dstream::meta_manager::MetaManager* meta_manager =
                dstream::meta_manager::MetaManager::GetMetaManager();
    if (meta_manager == NULL) {
        DSTREAM_WARN("GetMetaManager failed");
        exit(0);
    }
    dstream::PM pm;
    CheckMemCall(*meta_manager, GetPM, &pm);
    if (!pm.has_submit_port() || !pm.has_host()) {
        DSTREAM_ERROR("can not get PM host or submit_port");
        return false;
    }
    *port = pm.submit_port();
    *host = pm.host();
    dstream::meta_manager::MetaManager::DestoryMetaManager();
    return true;
}

void print_usage(const char* pro) {
    printf("Usage:\n\t%s -c config_file_path [-n \"pnid[ pnid2 pnid3[...]]\"]"
           " [-e \"peid[ peid2 peid3 [...]]\"]"
           " [-p zk_path(it will overwrite the config)]"
           " [-h host:port (it will overwrite the config)]"
           , pro);
}
bool ShowInfo(const std::string& uri , const char* pns, const char* pes) {
    dstream::StatusRequest req;
    dstream::StatusResponse res;
    dstream::PNID* pn_id;
    dstream::PEID* pe_id;
    std::string id;
    if (pns != NULL) {
        std::stringstream pn_stream(pns);
        while (pn_stream >> id) {
            pn_id = req.add_pn_id();
            pn_id->set_id(id);
        }
    }
    if (pes != NULL) {
        std::stringstream pe_stream(pes);
        while (pe_stream >> id) {
            pe_id = req.add_pe_id();
            pe_id->set_id(strtoll(id.c_str(), NULL, 10));
        }
    }
    DSTREAM_INFO("start to get response");
    if (dstream::rpc_client::rpc_call(uri.c_str(),
                                      dstream::rpc_const::kGetMetricMethod, req, res) < 0) {
        DSTREAM_DEBUG("get response fail");
        return false;
    }
    DSTREAM_INFO("get response success");
    std::string print_str;
    if (google::protobuf::TextFormat::PrintToString(res, &print_str)) {
        std::cout << print_str << std::endl;
    } else {
        DSTREAM_WARN("PrintToString failed");
        return false;
    }

    return true;
}

int main(int argc, char** argv) {
    dstream::logger::initialize("metrics_dumper");
    const char* pns = NULL;
    const char* pes = NULL;
    int opt;
    opterr = 0;
    const char* config_path = kZkConfigPath;
    const char* zk_root_path = NULL;
    const char* zk_address = NULL;
    while ((opt = getopt(argc, argv, "n:e:p:h:c:")) != EOF) {
        switch (opt) {
        case 'n':
            pns = optarg;
            break;
        case 'e':
            pes = optarg;
            break;
        case 'c':
            config_path = optarg;
            break;
        case 'p':
            zk_root_path = optarg;
            break;
        case 'h':
            zk_address = optarg;
            break;
        default:
            printf("there is no option - %c\n", opt);
            print_usage(argv[0]);
            exit(-1);
            break;
        }
    }
    if (pns == NULL && pes == NULL) {
        print_usage(argv[0]);
        exit(0);
    }

    std::string host;
    int32_t port;
    Config config;
    CheckMemCall(config, ReadConfig, config_path);

    if (zk_address != NULL) {
        config.SetValue("ZooKeeperPath.Address", zk_address);
    }
    if (zk_root_path != NULL) {
        std::string root_path = zk_root_path;
        CheckMemCall(config, SetValue, "ZooKeeperPath.PM.Root",
                    (root_path + "/PM").c_str());
        CheckMemCall(config, SetValue, "ZooKeeperPath.PN.Root",
                    (root_path + "/PN").c_str());
        CheckMemCall(config, SetValue, "ZooKeeperPath.Config.Root",
                    (root_path + "/Config").c_str());
        CheckMemCall(config, SetValue, "ZooKeeperPath.App.Root",
                    (root_path + "/App").c_str());
        CheckMemCall(config, SetValue, "ZooKeeperPath.Client.Root",
                    (root_path + "/Client").c_str());
    }
    if (!GetIpPort(&host, &port, config)) {
        DSTREAM_WARN("get pm ip port failed");
        return -1;
    }
    DSTREAM_INFO("get pm ip & port success. Host:[%s],Port:[%d]", host.c_str(), port);

    dstream::rpc::Context::init();
    std::string addr = "tcp://" + host + ":" + dstream::NumberToString(port);
    ShowInfo(addr, pns, pes);
}
