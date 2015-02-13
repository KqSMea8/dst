/** 
* @file     pn_group_dumper.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <list>
#include "common/application_tools.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/cluster_config.pb.h"
#include "common/utils.h"
#include "common/zk_client.h"
#include "google/protobuf/text_format.h"
#include "metamanager/zk_meta_manager.h"

#include <getopt.h> // NOLINT

#ifdef ERROR_CODE
#undef ERROR_CODE
#define ERROR_CODE int32_t
#endif

void print_usage(const char* pro) {
    printf("Usage:\n\t%s -h ip:port_list -p root_path -f file\n", pro);
}

int main(int argc, char* argv[]) {
    const char* ipport = NULL;
    const char* root_path = NULL;
    const char* file_name = NULL;
    ERROR_CODE  ret;

    dstream::logger::initialize("ZK_META_DUMPER");

    int opt;
    opterr = 0;
    while ((opt = ::getopt(argc, argv, "h:p:f:")) != -1) {
        switch (opt) {
        case 'h':
            ipport = optarg;
            break;
        case 'p':
            root_path = optarg;
            break;
        case 'f':
            file_name = optarg;
            break;
        default:
            printf("unkown option -%c: %s\n", opt, optarg);
            print_usage(argv[0]);
            exit(1);
        }
    } // end of while

    if (NULL == ipport || NULL == root_path || NULL == file_name) {
        print_usage(argv[0]);
        return 1;
    }

    zoo_set_debug_level(ZOO_LOG_LEVEL_INFO);
    // init zookeeper connection
    dstream::zk_client::ZkClient zk(ipport);
    ret = zk.Connect();
    if (ret < dstream::error::OK) {
        DSTREAM_WARN("connect to zookeeper[%s] ...FAILED", ipport);
        return ret;
    }

    dstream::PNGroups pn_group;
    std::list<dstream::GroupInfo> group_list;
    std::list<dstream::GroupInfo>::const_iterator group_list_iter;
    std::string pn_group_path = root_path;
    pn_group_path += "/PNGroup";
    ret = dstream::zk_meta_manager::GetZKChildData(&zk, pn_group_path, &group_list);
    if (ret < dstream::error::OK) {
        DSTREAM_WARN("get pn group [%s] ...FAILED", pn_group_path.c_str());
        return ret;
    }

    for (group_list_iter = group_list.begin();
         group_list_iter != group_list.end();
         group_list_iter++) {
        dstream::GroupInfo* add = pn_group.add_group_list();
        add->CopyFrom(*group_list_iter);
    }

    std::string s;
    ret = google::protobuf::TextFormat::PrintToString(pn_group, &s);
    if ( !ret ) {
        DSTREAM_WARN("parse pn group protobuf ...FAILED");
        return -1;
    }

    if (s.empty()) {
        DSTREAM_WARN("there are no groups on zk right now");
        return -1;
    }

    FILE* file = fopen(file_name, "w");
    if (NULL == file) {
        DSTREAM_ERROR("open file %s fail", file_name);
        return -1;
    }
    fprintf(file, "%s", s.c_str());
    fclose(file);

    return 0;
}
