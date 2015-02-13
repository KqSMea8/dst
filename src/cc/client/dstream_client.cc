/**
* @file   dstream_client.cc
* @brief    dstrean client is responsible for app management, include
*           app submission, app reconfiguration, app decommission,
*           PE migration and so on.
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "client/action.h"
#include "client/application.h"
#include "client/client_config.h"
#include "client/dstream_client.h"
#include "client/pm_client.h"
#include "client/zk.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/utils.h"
#include "common/zk_client.h"

namespace dstream {
namespace client {


///////////////////////////////////////////////////////////
// common function
///////////////////////////////////////////////////////////

void PrintUsage(const char* prj) {
    printf("Usage:\n");
    printf("  %s --conf <CFG_FILE> --submit-app -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --kill-app --app <APP>\n", prj);
    printf("  %s --conf <CFG_FILE> --restart-processor --app <APP> --processor <PROCESSOR>\n", prj);
    printf("  %s --conf <CFG_FILE> --migrate-pe --app <APP> --pe <PE>\n", prj);
    printf("  %s --conf <CFG_FILE> --manage-cluster\n", prj); // cluster management
    printf("  %s --conf <CFG_FILE> --get-user\n", prj);
    printf("  %s --conf <CFG_FILE> --update-user "
                "--username <USERNAME> --password <PASSWORD>\n", prj);
    printf("  %s --conf <CFG_FILE> --add-user --username <USERNAME> --password <PASSWORD>\n", prj);
    printf("  %s --conf <CFG_FILE> --del-user --username <USERNAME>\n", prj);
    printf("  %s --conf <CFG_FILE> --add-sub-tree -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --del-sub-tree -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --add-importer -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --del-importer -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --update-para -d <APP_DIR>\n", prj);
    printf("  %s --conf <CFG_FILE> --flow-control --app <APP> --pro <PROCESSOR> --cmd <CMD>\n", prj);
    printf("  %s --conf <CFG_FILE> --update-processor -d <APP_DIR> --revision <REVISION>\n", prj);
    printf("  %s --conf <CFG_FILE> --check-topology -d <APP_DIR>\n", prj);
    printf("  %s --version\n", prj);
    printf("Parameters:\n");
    printf("  --conf :\n");
    return;
}

} // namespace client
} // namespace dstream

using namespace dstream;
using namespace dstream::client;


int main(int argc, char* argv[]) {
    int32_t ret = error::OK;
    dstream::logger::initialize("dclient");
    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

    ret = CommandParse(argc, argv);
    if (ret < error::OK) {
        PrintUsage(argv[0]);
        return kCommandParseFailed;
    }

    /* read config file */
    ret = g_cfg_map.InitCfgMapFromFile(g_cmd_line_args.cfg_file);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to read cluster config from config file");
        return kInitConfigFileFailed;
    }
    /* for debug */
    g_cfg_map.Dump();

    /* init hdfs client */
    g_pc = new (std::nothrow) PMClient();
    g_pc->Init();

    if (g_cmd_line_args.need_login_pm) {
        /* registe on zookeeper */
        g_zk = new ZK(g_cfg_map.GetValue(config_const::kZKPathName));
        ret  = g_zk->Register(&g_user);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to registe client to zookeeper");
            return kRegisterZkFailed;
        }

        /* login into pm */
        ret = g_pc->LoginPM(g_cfg_map.GetValue(config_const::kZKPathName), &g_user);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to login into PM");
            return kLoginPmFailed;
        }
    }

    /* executor jobs */
    Action action;
    ret = action.SetAction(g_cmd_line_args.action.c_str());
    if (ret < error::OK) {
        DSTREAM_WARN("fail to set command");
        return kSetCommandFailed;
    }
    ret = action.Run();
    if (ret < error::OK) {
        DSTREAM_WARN("fail to execute command, Err:[%d], ErrStr:[%s]",
                     ret, error::get_text(ret));
        return kExecuteCommandFailed;
    }

    /* unregiste on zookeeper */
    delete g_zk;
    g_zk = NULL;

    /* release g_pc */
    delete g_pc;
    g_pc = NULL;
    return kOk;
}
