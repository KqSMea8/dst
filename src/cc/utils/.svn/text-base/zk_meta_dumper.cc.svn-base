/** 
* @file     zk_meta_dumper.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include <list>

#include "common/application_tools.h"
#include "common/error.h"
#include "common/id.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pn.pb.h"
#include "common/utils.h"
#include "common/zk_client.h"
#include "metamanager/zk_meta_manager.h"

#include <getopt.h> // NOLINT

using namespace dstream;

#ifdef ERROR_CODE
#undef ERROR_CODE
// #define ERROR_CODE error::Code
#define ERROR_CODE int32_t
#endif

#define ASSERT_RETURN(ret, fmt, ...) \
    if (ret < error::OK) { \
        DSTREAM_ERROR(fmt, ## __VA_ARGS__); \
    return ret; \
    }\

#define ASSERT_ERR_RET(ret, fmt, ...) \
    if (ret < error::OK) { \
        DSTREAM_ERROR(fmt, ## __VA_ARGS__); \
        goto ERR_RET; \
    }\

#define ASSERT_WARN(ret, fmt, ...) \
    if (ret < error::OK) { \
        DSTREAM_WARN(fmt, ## __VA_ARGS__); \
    }\

static const int kMaxBufSize = 1024;

void print_usage(const char* pro) {
    printf("Usage:\n\t%s -h ip:port_list -p root_path"
           " [-a app_id] [-s slim-mode/topology]\n", pro);
}

bool IsEqual(const Application& app, const AppID& app_id) {
    return app.id().id() == app_id.id();
}

void GenTreePrefix(int step, std::string* fmt_prefix) {
    assert(NULL != fmt_prefix);
    fmt_prefix->clear();
    for (int i = 0; i < step; i++) {
        (*fmt_prefix) += "   |";
    }
    (*fmt_prefix) += "---";
}

void DumpProcessor(const Processor& processor, int step) {
    std::string prefix;
    GenTreePrefix(step, &prefix);
    std::string fmt = prefix +
                      "Processor: ID:[%s],Name:[%s],Role:[%s],cpu_limit:[%.4f],"
                      "memory_limit:[%lld],network_limit:[%lld],disk_limit:[%lld]\n";
    std::string role;
    switch (processor.role()) {
    case dstream::EXPORTER:
        role = "EXPORTER";
        break;
    case dstream::IMPORTER:
        role = "IMPORTER";
        break;
    case dstream::WORK_PROCESSOR:
        role = "WORK_PROCESSOR";
        break;
    default:
        role = "";
        break;
    }
    const Resource& res = processor.resource_require();
    printf(fmt.c_str(), NumberToString(processor.id().id()).c_str(), processor.name().c_str(),
           role.c_str(), res.cpu(), res.memory(), res.network(), res.disk());
}

void DumpPE(const Application& app, const ProcessorElement& pe, int step) {
    std::string fmt_prefix;
    std::string fmt;

    // print pe root
    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "%s\n";
    std::string str_id;
    str_id = "AppID:";
    StringAppendNumber(&str_id, pe.app().id());
    str_id += ", ProcessorID:";
    StringAppendNumber(&str_id, pe.processor().id());
    str_id += ", PEID:";
    StringAppendNumber(&str_id, pe.pe_id().id());
    str_id += ", Serial:";
    StringAppendNumber(&str_id, pe.serial());
    printf(fmt.c_str(), str_id.c_str());
    Processor processor;
    application_tools::GetProcessor(app, pe, &processor);
    DumpProcessor(processor, step + 1);

    // print backups
    for (int i = 0; i < pe.backups_size(); i++) {
        GenTreePrefix(step + 1, &fmt_prefix);
        fmt = fmt_prefix + "backups[%d]: backup_pe_id:[%llu], pn_id:[%s]";
        printf(fmt.c_str(), i, pe.backups(i).backup_pe_id().id(),
               pe.backups(i).pn_id().id().c_str());
        uint64 last_assignment_time = 0;
        const BackupPE& bk_pe = pe.backups(i);
        if (bk_pe.has_last_assignment_time()) {
            last_assignment_time = bk_pe.last_assignment_time();
        }
        printf(",last_assignment_time:[%llu]\n", last_assignment_time);
    }
    // print primary
    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "primary id:[%llu]\n";
    printf(fmt.c_str(), pe.primary(0).id());
}

ERROR_CODE TraversePE(const Application& app, const std::string root_path,
                      int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    std::list<ProcessorElement> pe_list;
    ERROR_CODE ret = zk_meta_manager::GetZKChildData(zk, root_path.c_str(), &pe_list);
    ASSERT_RETURN(ret, "get pe list ...FAILED");

    std::list<ProcessorElement>::const_iterator it;
    for (it = pe_list.begin(); it != pe_list.end(); it++) {
        DumpPE(app, *it, step + 1);
    }
    return error::OK;
}

void DumpApp(const Application& app, std::string& root_path, int step, zk_client::ZkClient* zk) {
    std::string fmt_prefix;
    std::string fmt;

    // print app root
    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "%llu\n";
    printf(fmt.c_str(), app.id().id());

    // print app attri
    fmt_prefix.clear();
    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "name: %s\n";
    printf(fmt.c_str(), app.name().c_str());
    fmt = fmt_prefix + "user: %s\n";
    printf(fmt.c_str(), app.user().username().c_str());
    fmt = fmt_prefix + "status: %s\n";
    AppStatus status = app.status();
    std::string str_status;
    if (status == SUBMIT) {
        str_status = "SUBMIT";
    } else if (status == RUN) {
        str_status = "RUN";
    } else {
        str_status = "KILL";
    }
    printf(fmt.c_str(), str_status.c_str());

    fmt = fmt_prefix + "LogPath:%s\n";
    std::string hdfs_log_path;
    if (app.has_log_path() && app.log_path().has_path()) {
        hdfs_log_path = app.log_path().path();
    } else {
        DSTREAM_WARN("can't find log_path for app [%zu]", app.id().id());
    }
    printf(fmt.c_str(), hdfs_log_path.c_str());
    printf("%sSubmitTime:%zu\n", fmt_prefix.c_str(), app.submit_time());

    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "PE List\n";
    printf("%s", fmt.c_str());

    std::string pe_root = root_path + "/";
    StringAppendNumber(&pe_root, app.id().id());
    TraversePE(app, pe_root, step + 1, zk);

    // print subscibles
}

ERROR_CODE TraverseApp(const std::string root_path, const AppID& app_id,
                       int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/App";
    printf("%s\n", prefix.c_str());

    std::list<Application> app_list;
    ret = zk_meta_manager::GetZKChildData(zk, prefix.c_str(), &app_list);
    ASSERT_RETURN(ret, "get app list ...FAILED");
    std::list<Application>::const_iterator it;
    std::list<Application>::const_iterator beg_it = app_list.begin();
    std::list<Application>::const_iterator end_it = app_list.end();
    if (app_id.has_id()) {
        for (it = beg_it; it != end_it; it++) {
            if (IsEqual(*it, app_id)) {
                DumpApp(*it, prefix, step, zk);
                break;
            }
        }
        if (it == end_it) {
            DSTREAM_ERROR("no app(%zu) founded", app_id.id());
            return error::META_INVALID;
        }
    } else {
        for (it = beg_it; it != end_it; it++) {
            DumpApp(*it, prefix, step, zk);
        }
        if (beg_it == end_it) {
            DSTREAM_ERROR("no app(%zu) founded", app_id.id());
            return error::META_INVALID;
        }

    }
    return error::OK;
}

ERROR_CODE TraversePM(const std::string& root_path, int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/PM";
    printf("%s\n", prefix.c_str());

    PM pm;
    ret = zk_meta_manager::GetZKData(zk, prefix, &pm);
    ASSERT_RETURN(ret, "get pm ...FAILED");

    std::string fmt_prefix;
    std::string fmt;
    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "PM: host:[%s] submit_port:[%d] report_port:[%d]\n";
    printf(fmt.c_str(), pm.host().c_str(), pm.submit_port(), pm.report_port());

    return error::OK;
}

ERROR_CODE TraverseBackupPM(const std::string root_path, int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/BackupPM";
    printf("%s\n", prefix.c_str());

    std::list<PM> pm_list;
    ret = zk_meta_manager::GetZKChildData(zk, prefix.c_str(), &pm_list);
    ASSERT_RETURN(ret, "get backup pm ...FAILED");

    std::list<PM>::const_iterator it;
    std::list<PM>::const_iterator beg_it = pm_list.begin();
    std::list<PM>::const_iterator end_it = pm_list.end();

    for (it = beg_it; it != end_it; it++) {
        std::string fmt_prefix;
        std::string fmt;
        GenTreePrefix(step, &fmt_prefix);
        fmt = fmt_prefix + "PM: host:[%s] submit_port:[%d] report_port:[%d]\n";
        printf(fmt.c_str(), it->host().c_str(), it->submit_port(), it->report_port());
    }

    return error::OK;
}

void DumpPN(const PN& pn, int step) {
    std::string fmt_prefix;
    std::string fmt;

    // print pe root
    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "%s\n";
    printf(fmt.c_str(), pn.pn_id().id().c_str());

    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "host:[%s], pn_pm_port:[%d], pn_pe_port:[%d], " +
          "pn_pn_port:[%d], debug_port:[%d], status:[%s]\n";
    printf(fmt.c_str(), pn.host().c_str(), pn.rpc_server_port(), pn.pub_port(),
           pn.sub_port(), pn.debug_port(),
           pn.status() == NORMAL ? "NORMAL" : "DECOSSIMION");
}

ERROR_CODE TraversePN(const std::string& root_path, int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/PN";
    printf("%s\n", prefix.c_str());

    list<PN> pn_list;
    ret = zk_meta_manager::GetZKChildData(zk, prefix, &pn_list);
    ASSERT_RETURN(ret, "get pn ...FAILED");

    std::list<PN>::const_iterator it;
    for (it = pn_list.begin(); it != pn_list.end(); it++) {
        DumpPN(*it, step + 1);
    }

    return error::OK;
}

void DumpPNGroup(const GroupInfo& group, int step) {
    std::string fmt_prefix;
    std::string fmt;

    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "%s\n";
    printf(fmt.c_str(), group.group_name().c_str());

    GenTreePrefix(step + 1, &fmt_prefix);
    /*fmt = fmt_prefix + "type:[%d]\n";
    printf(fmt.c_str(), group.type());*/

    for ( int i = 0; i < group.pn_list_size(); i++ ) {
        fmt = fmt_prefix + "[%s]\n";
        printf(fmt.c_str(), group.pn_list(i).c_str());
    }
}

ERROR_CODE TraversePNGroup(const std::string& root_path, int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/PNGroup";
    printf("%s\n", prefix.c_str());

    std::list<GroupInfo> pn_group;
    ret = zk_meta_manager::GetZKChildData(zk, prefix, &pn_group);
    ASSERT_RETURN(ret, "get pn group ..FAILED");

    std::list<GroupInfo>::const_iterator it;
    for (it = pn_group.begin(); it != pn_group.end(); it++) {
        DumpPNGroup(*it, step + 1);
    }

    return error::OK;
}

void DumpSubPoint(const SubPoint& sub_point, int step) {
    std::string fmt_prefix;
    std::string fmt;

    GenTreePrefix(step, &fmt_prefix);
    fmt = fmt_prefix + "%lu\n";
    printf(fmt.c_str(), sub_point.id().id());

    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "msg id:[%ld]\n";
    printf(fmt.c_str(), sub_point.msg_id());

    GenTreePrefix(step + 1, &fmt_prefix);
    fmt = fmt_prefix + "seq id:[%ld]\n";
    printf(fmt.c_str(), sub_point.seq_id());
}

ERROR_CODE TraverseSubPoint(const std::string& root_path, int step, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/SubPoint";
    printf("%s\n", prefix.c_str());

    std::list<SubPoint> sub_point;
    ret = zk_meta_manager::GetZKChildData(zk, prefix, &sub_point);
    ASSERT_RETURN(ret, "get sub point ...FAILED");

    std::list<SubPoint>::const_iterator it;
    for (it = sub_point.begin(); it != sub_point.end(); it++) {
        DumpSubPoint(*it, step + 1);
    }

    return error::OK;
}

ERROR_CODE TraverseClient(const std::string& root_path, int /*step*/, zk_client::ZkClient* zk) {
    assert(NULL != zk);
    std::string prefix;
    prefix = root_path + "/Client";
    printf("%s\n", prefix.c_str());

    return error::OK;
}

/** 
* @brief   dump app information for webapps chart show 
* 
* @param    root_path zk root path 
* @param    app_id    app id to show
* @param    zk        zk client instance
* 
* @return   
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-08-08
*/
ERROR_CODE DumpAppInSlimMode(const std::string& root_path, const AppID& app_id, zk_client::ZkClient* zk) {
    if(!app_id.has_id()) {
        DSTREAM_ERROR("no app id provide");
        return -1;
    }
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/App/";
    StringAppendNumber(&prefix, app_id.id());
    printf("%s\n", prefix.c_str());
    Application app;
    ret = zk_meta_manager::GetZKData(zk, prefix.c_str(), &app);
    ASSERT_RETURN(ret, "get app ...FAILED");
    string app_name = app.name();
    dstream::Topology tp = app.topology();
    for(int32_t i = 0; i < tp.processors_size(); i++) {
        printf("%s,%s,%u:", app_name.c_str(),tp.processors(i).name().c_str(),
                tp.processors(i).cur_revision());
        uint64_t parallism = tp.processors(i).parallism();
        for(uint64_t j = 0; j < parallism; j++) {
            uint64_t pe_id = MAKE_PEID(app.id().id(),tp.processors(i).id().id(),j);
            printf("%lu", pe_id);
            if(parallism >= 1 && j < (parallism -1))
                printf(",");
        }
        printf("\n");
    }
    return error::OK;
}

/** 
 * @brief    dump app topology for webapps 
 * 
 * @param    root_path
 * @param    add_id
 * @param    zk
 * 
 * @return   
 * @author   liuguodong,liuguodong01@baidu.com
 * @date     2013-08-08
 */
ERROR_CODE DumpAppTopology(const std::string& root_path, const AppID& app_id,zk_client::ZkClient* zk) {
    if(!app_id.has_id()) {
        DSTREAM_ERROR("no app id provide");
        return -1;
    }
    ERROR_CODE ret;
    std::string prefix;
    prefix = root_path + "/App/";
    StringAppendNumber(&prefix, app_id.id());
    printf("%s\n", prefix.c_str());
    Application app;
    ret = zk_meta_manager::GetZKData(zk, prefix.c_str(), &app);
    ASSERT_RETURN(ret, "get app ...FAILED");
    dstream::Topology tp = app.topology();
    uint32_t node_num = tp.processor_num();
    //generate topology string
    string str = "[";
    for(uint32_t i = 0; i < node_num; i++) {
        for(uint32_t j = 0; j < node_num; j++) {
            if (i == j) {
                continue;
            }
            Subscribe proc_sub = tp.processors(i).subscribe();
            Processor proc_upstream = tp.processors(j);
            if(proc_sub.processor_size() > 0) {
                for(int k = 0; k < proc_sub.processor_size(); k++) {
                    if(proc_sub.processor(k).name() == proc_upstream.name()) {
                        str += "[";
                        StringAppendNumber(&str, j + 1);
                        str += ",";
                        StringAppendNumber(&str, i + 1);
                        str += "],";
                    }
                }
            }
        }
    }
    size_t len = str.length();
    if(len >0 && str[len - 1] == ',') {
        str[len -1] = ']';
    } else {
        str += "]";
    }
    printf("Topology:%s\n", str.c_str());
    printf("seq,appid,processorid,processorname\n");
    for(uint32_t i = 0; i < node_num; i++) {
        printf("%u,%zu,%zu,%s\n", i + 1, app_id.id(),
                tp.processors(i).id().id(), tp.processors(i).name().c_str());
    }
    return error::OK;
}

int main(int argc, char* argv[]) {
    const char* ipport = NULL;
    const char* root_path = NULL;
    AppID    app_id;
    ERROR_CODE  ret;
    const char* kSlimMode = "slim-mode";
    const char* kTopoMode = "topology";
    bool     slim_mode = false;
    bool     topo_dump = false; 
    logger::initialize("ZK_META_DUMPER");
    logger::set_level(Logging::WARN_LOG_LEVEL);

    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h:p:a:s:")) != -1) {
        switch (opt) {
        case 'h':
            ipport = optarg;
            break;
        case 'p':
            root_path = optarg;
            break;
        case 'a':
            app_id.set_id(strtoull(optarg, NULL, 10));
            break;
        case 's': {
            string arg(optarg);
            if(arg == kSlimMode) {
                slim_mode = true;
            } else if (arg == kTopoMode) {
                topo_dump = true;
            } else {
                DSTREAM_WARN("unknown -s %s parameter", optarg);
                return -1;
            }
                break;
        }
        default:
            printf("unkown option -%c: %s\n", opt, optarg);
            print_usage(argv[0]);
            exit(1);
        }
    } // end of while

    if (NULL == ipport || NULL == root_path) {
        print_usage(argv[0]);
        return 1;
    }

    // init zookeeper connection
    zk_client::ZkClient zk(ipport, 30000, NULL, ZOO_LOG_LEVEL_WARN);
    ret = zk.Connect();
    ASSERT_RETURN(ret, "connect to zookeeper[%s] ...FAILED", ipport);

    int step = 0;

    if(slim_mode) {
        DumpAppInSlimMode(root_path, app_id, &zk);       
        return 0;
    } else if(topo_dump) {
        DumpAppTopology(root_path, app_id, &zk);
        return 0;
    } 

    // traverse app list
    ret = TraverseApp(root_path, app_id, step + 1, &zk);
    // ASSERT_WARN(ret, "travers app ...FAILED");

    // traverse pm
    ret = TraversePM(root_path, step + 1, &zk);
    // ASSERT_WARN(ret, "travers PM ...FAILED");

    // traverse backup pm
    ret = TraverseBackupPM(root_path, step + 1, &zk);

    // traverse pn
    ret = TraversePN(root_path, step + 1, &zk);
    // ASSERT_WARN(ret, "travers PN ...FAILED");
    //
    // traverse pn group
    ret = TraversePNGroup(root_path, step + 1, &zk);

    // traverse pm
    ret = TraverseClient(root_path, step + 1, &zk);
    // ASSERT_WARN(ret, "travers Client ...FAILED");

    // traverse sub point
    ret = TraverseSubPoint(root_path, step + 1, &zk);
    // ASSERT_WARN(ret, "travers Sub Point ...FAILED");

    return 0;
}
