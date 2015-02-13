/** 
* @file     blacklist_utils.cc
* @brief    tools to clear or get blacklist on pm  
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-07
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#ifndef __GUN_LIBRARY__   //for kylin
#define __GUN_LIBRARY__
#endif
#include <cctype>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "common/config.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/dstream_proto_common.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/proto_rpc.h"
#include "common/rpc_client.h"
#include "common/rpc_const.h"
#include "common/zk_client.h"
#include "metamanager/zk_meta_manager.h"
#include "google/protobuf/text_format.h"
#include <getopt.h>
#define ERR_CODE dstream::error::Code
const std::string kDefaultConnetTimeoutMiniSec = "1000";
const std::string kTypeGet                     = "GET";
const std::string kTypeErase                   = "ERASE";
const std::string kTypeReset                   = "RESET";
void print_usage(const char* self_name) {
    printf("Usage:\n\t%s -c conf_file_path "
           "-f [get/erase/reset] -i peid1[,peid2,peid3...]\t\n"
           "Info: -f reset doesn't need -i option\t\n",self_name);
}

bool GetIpPort(const dstream::config::Config& config, std::string* host, int32_t* port) {
    assert(NULL != host && NULL != port);
    if (!dstream::meta_manager::MetaManager::InitMetaManagerContext(&config)) {
        DSTREAM_ERROR("InitMetaManagerContext error");
        return false;
    }
    DSTREAM_INFO("init MetaManagerContext success.");
    dstream::meta_manager::MetaManager* meta_manager =
                dstream::meta_manager::MetaManager::GetMetaManager();
    if (meta_manager == NULL) {
        DSTREAM_WARN("GetMetaManager failed");
        return false;
    }
    dstream::PM pm;
    if(meta_manager->GetPM(&pm) < dstream::error::OK) {
        DSTREAM_ERROR("get pm uri failed");
        return false;
    }
    if (!pm.has_submit_port() || !pm.has_host()) {
        DSTREAM_ERROR("can not get PM host or submit_port");
        return false;
    }
    *port = pm.submit_port();
    *host = pm.host();
    dstream::meta_manager::MetaManager::DestoryMetaManager();
    return true;
}

bool BlackListOperation(const std::string& uri, const char* pe_ids, const string& op_type) { 
    dstream::BlackListOperationRequest  req;
    dstream::BlackListOperationResponse res;
    if(NULL != pe_ids) {
        std::stringstream peid_to_split(pe_ids);
        std::string peid_str;
        while(std::getline(peid_to_split, peid_str, ',')) {
            req.add_pe_id()->set_id(strtoll(peid_str.c_str(), NULL, 10));  
        }
    }
    if(kTypeErase == op_type) {
        if(NULL == pe_ids) {
            DSTREAM_WARN("erase operation need pe id to be provided");
            return false;
        }
        DSTREAM_INFO("try to erase pe [%s] blacklist in pm", pe_ids);
        req.set_operation(dstream::BlackListOperationRequest::ERASE);
        if(dstream::rpc_client::rpc_call(uri.c_str(),
                                             dstream::rpc_const::kBlackListMethod, req, res) < 0) {
            DSTREAM_WARN("erase black list failed");
            return false;
        } else {
            DSTREAM_INFO("erase black list success");
        }
    } else if (kTypeReset == op_type) {
        DSTREAM_INFO("try to reset blacklist in pm");
        req.set_operation(dstream::BlackListOperationRequest::CLEAR);
        if(dstream::rpc_client::rpc_call(uri.c_str(),
                                             dstream::rpc_const::kBlackListMethod, req, res) < 0) {
            DSTREAM_WARN("reset black list failed");
            return false;
        } else {   
            DSTREAM_INFO("reset black list success");
        }
    } else if (kTypeGet == op_type) {
        if(NULL == pe_ids) {
            DSTREAM_WARN("get operation need pe id to be provided");
            return false;
        }
        DSTREAM_INFO("try to get pe [%s] blacklist in pm", pe_ids);
        req.set_operation(dstream::BlackListOperationRequest::GET);
        if(dstream::rpc_client::rpc_call(uri.c_str(),
                                             dstream::rpc_const::kBlackListMethod, req, res) < 0) {
            DSTREAM_WARN("get black list failed");
            return false;
        } else {
            DSTREAM_INFO("get blacklist success");
        }
        printf("num:%d\n", res.pe_pn_blacklist_size());
        std::string print_str;
        if(google::protobuf::TextFormat::PrintToString(res, &print_str)) {
            printf("%s\n", print_str.c_str());
        }
    } else {
        DSTREAM_WARN("unknown operation type:[%s]", op_type.c_str());
        return false;
    }
    return true;
}

int main(int argc, char** argv) {
    dstream::logger::initialize("blacklist_utils");
    int opt;
    const char* config_path  = NULL;
    const char* op_type      = NULL;
    const char* pe_ids       = NULL;
    while((opt = getopt(argc, argv, "c:f:i:")) != -1) { 
            switch (opt) {
                case 'c': {
                    config_path = optarg;
                    break;
                }
                case 'f': {
                    op_type = optarg;
                    break;
                }
                case 'i': {
                    pe_ids = optarg;
                    break;
                }
                default: {
                    printf("unknown option -%c\n", opt);
                    print_usage(argv[0]);
                    return -1;
                }
            }
    }
    if(NULL == config_path) {
        printf("config_file_path is not set!\n");
        print_usage(argv[0]);
        return -1;
    } 
    if (NULL == op_type) {
        printf("operation type is not set!\n");
        print_usage(argv[0]);
        return -1;
    }
    std::string type(op_type);
    std::transform(type.begin(), type.end(), type.begin(), toupper);
    if(type == kTypeGet || type == kTypeErase) {
        if(NULL == pe_ids) {
           DSTREAM_WARN("%s operation need peid to be provided", type.c_str()); 
           return -1;
        }
    } else if(type != kTypeReset) {
        DSTREAM_WARN("unknown operation type:[%s]", type.c_str());
        return -1;
    }
    std::string host;
    int32_t port;
    dstream::config::Config config;
    dstream::error::Code code = config.ReadConfig(config_path);
    if(code < dstream::error::OK) {
        DSTREAM_ERROR("Read config file error:%s", dstream::error::ErrStr(code));
        return -1;
    }
    if(!GetIpPort(config, &host, &port)) {
        DSTREAM_ERROR("Get pm ip port failed");
        return -1;
    }
    DSTREAM_INFO("get pm ip & port success. Host:[%s],Port:[%d]", host.c_str(), port);
    dstream::rpc::Context::init();
    std::string addr = "tcp://" + host + ":" + dstream::NumberToString(port);
    if(BlackListOperation(addr, pe_ids, type)) {
        DSTREAM_INFO("operation success");
    } else {
        DSTREAM_ERROR("operation failed");
    }
    return 0;
}
