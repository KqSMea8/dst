/** 
* @file     get_zk_info.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include <stdio.h>
#include <iterator>
#include <list>
#include <map>
#include <string>
#include "common/config_const.h"
#include "common/logger.h"
#include "common/utils.h"
#include "common/zk_client.h"
#include "metamanager/zk_meta_manager.h"
/*
using namespace dstream::config_const;
using namespace dstream;
using namespace std;
using namespace dstream::zk_client;
using namespace dstream::zk_meta_manager;
*/
#ifndef CheckErrorReturn

#define CheckErrorReturn(retval, errret, errfmt, ...) \
do {\
    if (retval < 0) {\
        DSTREAM_WARN(errfmt, ##__VA_ARGS__);\
        return errret;\
    }\
} while (0)

#define CheckErrorRetF(retval, errfmt, ...) CheckErrorReturn(retval, false, errfmt, ##__VA_ARGS__)
#define CheckErrorRetN(retval, errfmt, ...) CheckErrorReturn(retval, , errfmt, ##__VA_ARGS__)

#endif // CheckErrorReturn


// Dump item if it is #item
#define DumpNotReturn(Item, Type) \
    if (path.find("/"#Item) != string::npos) {\
        Type i;\
        dstream::zk_meta_manager::GetZKData(zk, path, &i);\
        printf("%s", i.DebugString().c_str());\
    }
#define Dump(Item, Type) \
    if (path.find("/"#Item) != string::npos) {\
        Type i;\
        dstream::zk_meta_manager::GetZKData(zk, path, &i);\
        printf("%s", i.DebugString().c_str());\
        return;\
    }
#define D(Item) Dump(Item, Item)

void DumpLeaf(dstream::zk_client::ZkClient* zk, string path) {
    assert(NULL != zk);
    if (path.empty()) {
        return;
    }
    Dump(PM, dstream::PM);
    Dump(PN, dstream::PN);
    Dump(BackupPM, dstream::PM);
    Dump(App, dstream::ProcessorElement);
    Dump(Config, dstream::ClusterConfig);
    Dump(machines/pm_machines, dstream::PMMachines);
    Dump(machines/pn_machines, dstream::PNMachines);
    if (path.find("/Client")) {
        return;
    }
    printf("Unknow Item");
}
std::string PathJoin(const std::string& dir, const std::string path) {
    if (dir.empty()) {
        return path;
    }
    if (dir[dir.size() - 1] == '/') {
        return dir + path;
    }
    return dir + "/" + path;
}

int max_depth = 0x0fffffff; // INF

void DumpZk(dstream::zk_client::ZkClient *zk, const string&  path, int step) {
    assert(NULL != zk);
    if (step > max_depth) {
        return;
    }
    printf("============================================\n[%s]\n", path.c_str());
    std::list<std::string> clist;
    zk->GetChildList(path.c_str(), &clist);
    if (clist.size() == 0) {
        DumpLeaf(zk, path);
    } else {
        DumpNotReturn(App/, dstream::Application);
        for (list<string>::iterator it = clist.begin(); it != clist.end(); ++it) {
            DumpZk(zk, PathJoin(path , *it), step + 1);
        }
    }
}

int main(int argc, char** argv) {
    dstream::logger::initialize("get_zk_info");
    if (argc < 2) {
        printf("Usage:get_zk_info <zk_addr> [Path] [max_depth] \n");
        exit(1);
    }
    string path;
    if (argc == 2) {
        path = "/";
    } else {
        path = argv[2];
        if (strlen(argv[2]) == 0) {
            path = "/";
        } else if (path.size() != 1 && path[path.size() - 1] == '/') {
            path = path.substr(0, path.size() - 1);
        }
    }
    if (argc == 4) {
        max_depth = dstream::Convert<int>(argv[3]);
    }
    const int kTimeout = 3000;
    dstream::zk_client::ZkClient zk(argv[1], kTimeout);
    zk.Connect();
    int32_t exist, attr;
    CheckErrorReturn(zk.NodeExists(path.c_str(), &exist, &attr), -1, "%s",
                     "ZkClient.NodeExists failed");
    if (!exist) {
        printf("Node not exists\n");
        exit(-1);
    }
    DumpZk(&zk, path, 0);
}
