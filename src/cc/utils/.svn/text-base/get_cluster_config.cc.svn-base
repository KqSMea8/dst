/** 
* @file     get_cluster_config.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include <stdio.h>
#include <map>
#include <string>
#include "common/config_const.h"
#include "common/logger.h"
#include "common/utils.h"
#include "metamanager/zk_meta_manager.h"

using namespace dstream::config_const;
using namespace dstream;
using namespace dstream::zk_meta_manager;
using namespace std;

#ifdef D
#undef D
#endif

#define D(obj, item)\
    printf("["#item"] = [%s]\n", Convert<std::string>(obj.item()).c_str()) // D means Dump
#define C(item) D(cc, item)  // C means DumpClusterConfig
// DR Means DumpRepeated
#define DR(obj, item) do {\
    for (int i = 0; i != obj.item##_size(); i++) {\
        printf("["#item"] = [%s]\n", Convert<std::string>(obj.item(i)).c_str());\
    }\
} while (0)


#define DumpClusterItem(item)

#define SWITCH(str)  { std::string __s__ = str; if (0) {
#define CASE(val)    } else if (__s__ == val) {
#define DEFAULT      } else {
#define ENDSWITCH    }}

int main(int argc, char** argv) {
    logger::initialize("get_cluster_config");
    if (argc < 2) {
        printf("Usage:get_cluster_config <dstream_cfg_file>"
               " [black_list host_num hdfs_path min_host_percetage wait_pn_time_sec] \n");
        exit(1);
    }
    config::Config config;
    if (config.ReadConfig(argv[1]) != error::OK) {
        DSTREAM_WARN("read config file %s fail\n", argv[1]);
        exit(1);
    }
    ZKMetaManager zk_meta;
    if (!zk_meta.InitMetaManager(&config)) {
        DSTREAM_ERROR("init zk meta manager fail");
        exit(1);
    }

    ClusterConfig cc;
    if (zk_meta.GetClusterConfig(&cc) != error::OK) {
        DSTREAM_ERROR("get cluster config failed");
        exit(-1);
    }

    if (argc == 2) {
        C(hdfs_path);
        C(min_host_percentage);
        C(wait_pn_time_sec);
        return 0;
    }
    std::string fail = "";
    std::string delim = "";
    for (int i = 2; i < argc ; i++) {
        SWITCH(argv[i])
        CASE("hdfs_path") C(hdfs_path);
        CASE("min_host_percentage") C(min_host_percentage);
        CASE("wait_pn_time_sec") C(wait_pn_time_sec);
        DEFAULT        fail += delim + argv[i];
        delim = ",";
        ENDSWITCH
    }
    if (!fail.empty()) {
        printf("%s", ("fail to get item [" + fail + "]\n").c_str());
    }
}
