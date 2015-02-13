/**
* @file
* @brief
* @author     fangjun, fangjun02@baidu.com
* @date
* @notice
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
/**
* @file   action.cc
* @brief    action mapper
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <getopt.h>
#include <set>

#include "client/action.h"
#include "client/application.h"
#include "client/client_config.h"
#include "client/dstream_client.h"
#include "client/pm_client.h"
#include "common/error.h"
#include "common/logger.h"


namespace dstream {
namespace client {

int64_t g_app_id = -1;
User    g_user;
CommandLineArgs  g_cmd_line_args;
PMClient*        g_pc;

#if defined( __DATE__ ) && defined ( __TIME__ )
static const char g_build_time[] = __DATE__" "__TIME__;
#else
static const char g_build_time[] = "unknown";
#endif

static const char* shortopts = "pd:v";
struct option longopts[] = {
    {"conf", required_argument, NULL, 'c'},
    {"submit-app", no_argument, NULL, 0},
    {"check-topology", no_argument, NULL, 0},
    {"decommission-app", no_argument, NULL, 0},
    {"kill-app", no_argument, NULL, 0},
    {"restart-processor", no_argument, NULL, 0},
    {"migrate-pe", no_argument, NULL, 0},
    {"app", required_argument, NULL, 'a'},
    {"revision", required_argument, NULL, 'r'},
    {"get-user", no_argument, NULL, 0 },
    {"add-user", no_argument, NULL, 0 },
    {"update-user", no_argument, NULL, 0 },
    {"del-user", no_argument, NULL, 0 },
    {"username", required_argument, NULL, 'u' },
    {"password", required_argument, NULL, 'w' },
    {"add-sub-tree", no_argument, NULL, 0 },
    {"del-sub-tree", no_argument, NULL, 0 },
    {"add-importer", no_argument, NULL, 0 },
    {"del-importer", no_argument, NULL, 0 },
    {"update-para", no_argument, NULL, 0 },
    {"flow-control", no_argument, NULL, 0 },
    {"update-processor", no_argument, NULL, 0 },
    {"cmd", required_argument, NULL, 'm' },
    {"pro", required_argument, NULL, 'p'},
    {"version", no_argument, NULL, 'v' }
};

#define ACTION_VECTOR_MAP \
ROWS(UNSUPPORTED,       "unsupported action", DefaultAct) \
ROWS(SUBMIT_APP,        "submit-app",         SubmitAppAct) \
ROWS(CHECK_TOPOLOGY,    "check-topology",     CheckTopologyAct) \
ROWS(DECOMMISSION_APP,  "decommission-app",   DecommissionAppAct)\
ROWS(KILL_APP,          "kill-app",           DecommissionAppAct)\
ROWS(DECOMMISSION_PROCESSOR, "decommission-processor", DecommissionProcessorAct) \
ROWS(RESTART_PROCESSOR, "restart-processor",  RestartProcessorAct) \
ROWS(MIGRATE_PE,        "migrate-pe",         MigratePeAct) \
ROWS(GET_USER,          "get-user",           GetUserAct) \
ROWS(UPDATE_USER,       "update-user",        UpdateUserAct) \
ROWS(ADD_USER,          "add-user",           AddUserAct) \
ROWS(DEL_USER,          "del-user",           DelUserAct) \
ROWS(ADD_SUB_TREE,      "add-sub-tree",       AddSubTreeAct) \
ROWS(DEL_SUB_TREE,      "del-sub-tree",       DelSubTreeAct) \
ROWS(ADD_IMPORTER,      "add-importer",       AddImporterAct) \
ROWS(DEL_IMPORTER,      "del-importer",       DelImporterAct) \
ROWS(UPDATE_PARA,       "update-para",        UpdateParallelismAct) \
ROWS(FLOW_CONTROL,      "flow-control",       FlowControlAct) \
ROWS(UPDATE_PRO,        "update-processor",   UpdateProcessorAct) \

Action::Action(): action_code_(UNSUPPORTED), action_exec_(DefaultAct) {
    /* initialize action map */
    ActionVec act_vec;
#undef  ROWS
#define ROWS(CODE, STR, FUNC) \
            act_vec.text = STR; \
            act_vec.exec = FUNC; \
            action_map_.insert(std::make_pair(CODE, act_vec));
    ACTION_VECTOR_MAP
}

int32_t Action::SetAction(const char* action_str) {
    if (NULL == action_str) {
        DSTREAM_INFO("bad arguments: action string is empty");
        return error::BAD_ARGUMENT;
    }
    ActionMap::iterator it;
    for (it = action_map_.begin(); it != action_map_.end(); it++) {
        if (strcmp(const_cast<char*>(it->second.text), action_str) == 0) {
            action_code_ = it->first;
            action_exec_ = it->second.exec;
            return error::OK;
        }
    }
    DSTREAM_WARN("not supported action");
    return error::CLIENT_NOT_SUPPORTED;
}

int32_t Action::Run() {
    return action_exec_();
}

///////////////////////////////////////////////////////////
// command function
///////////////////////////////////////////////////////////
int32_t CheckCommand() {
    if (g_cmd_line_args.cfg_file.empty()) {
        return error::BAD_ARGUMENT;
    }
    /* check action */
    if (g_cmd_line_args.action.empty()) {
        return error::BAD_ARGUMENT;
    }
    if ((g_cmd_line_args.action == "submit-app" ||
         g_cmd_line_args.action == "check-topology" ||
         g_cmd_line_args.action == "add-sub-tree" ||
         g_cmd_line_args.action == "add-importer" ||
         g_cmd_line_args.action == "del-importer" ||
         g_cmd_line_args.action == "update-para" ||
         g_cmd_line_args.action == "update-processor" ||
         g_cmd_line_args.action == "del-sub-tree") &&
        g_cmd_line_args.app_dir.empty()) {
        return error::BAD_ARGUMENT;
    }
    if ((g_cmd_line_args.action == "decommission-app" ||
         g_cmd_line_args.action == "kill-app") &&
         (g_cmd_line_args.app_id < 0 && g_cmd_line_args.app_name.empty()) ) {
        // DSTREAM_WARN("invalid app id[%lu] or app name[%s]",
        //               g_cmd_line_args.app_id, g_cmd_line_args.app_name.c_str());
        return error::BAD_ARGUMENT;
    }
    if (g_cmd_line_args.action == "update-user" &&
        (g_cmd_line_args.username.empty() || g_cmd_line_args.password.empty())) {
        return error::BAD_ARGUMENT;
    }
    if (g_cmd_line_args.action == "add-user" &&
        (g_cmd_line_args.username.empty() || g_cmd_line_args.password.empty())) {
        return error::BAD_ARGUMENT;
    }
    if (g_cmd_line_args.action == "del-user" &&
        g_cmd_line_args.username.empty()) {
        return error::BAD_ARGUMENT;
    }

    if (g_cmd_line_args.action == "flow-control") {
        if (g_cmd_line_args.app_id < 0 && g_cmd_line_args.app_name.empty()) {
            DSTREAM_WARN("app-id or app-name not set.");
            return error::BAD_ARGUMENT;
        }
        // appid or name OK
        if (g_cmd_line_args.cmd.empty()) {
            DSTREAM_WARN("cmd not set.");
            return error::BAD_ARGUMENT;
        }

        if (g_cmd_line_args.processor.empty()) {
            DSTREAM_INFO("processor not configured, will effect ALL processors.");
            g_cmd_line_args.processor = config_const::kAllProcessors;
        }
        DSTREAM_INFO("cmd: %s, processor: %s",
                g_cmd_line_args.cmd.c_str(), g_cmd_line_args.processor.c_str());
        return error::OK;
    }

    //   // other information to be checked
    return error::OK;
}

int32_t CommandParse(int argc, char* argv[]) {
    int opt;
    int optidx;
    opterr = 0;

    optind = 0;
    while ((opt = getopt_long(argc, argv, shortopts, longopts, &optidx)) != -1) {
        switch (opt) {
        case 'a':
            if (is_digit(optarg)) {
                g_cmd_line_args.app_id = strtoll(optarg, NULL, 10);
            } else if (is_valid_appname(optarg)) {
                g_cmd_line_args.app_name = optarg;
            }
            break;
        case 'c':
            g_cmd_line_args.cfg_file = optarg;
            break;
        case 'd':
            g_cmd_line_args.app_dir = optarg;
            break;
        case 'u':
            g_cmd_line_args.username = optarg;
            break;
        case 'w':
            g_cmd_line_args.password = optarg;
            break;
        case 'm':
            g_cmd_line_args.cmd = optarg;
            break;
        case 'p':
            g_cmd_line_args.processor = optarg;
            break;
        case 'r':
            g_cmd_line_args.revision = strtoll(optarg, NULL, 10);
            break;
        case 'v':
            printf("[%s]: version [%s]  Build: [%s]\n", argv[0], VERSION, g_build_time);
            exit(0);
        case 0:
            g_cmd_line_args.action = longopts[optidx].name;
            break;
        default:
            printf("[%s]: Unkown option\n", argv[0]);
            return error::BAD_ARGUMENT;
        }
    }

    if (g_cmd_line_args.action == "check-topology") {
        g_cmd_line_args.need_login_pm = false;
    }
    return CheckCommand();
}

int32_t DefaultAct() {
    DSTREAM_INFO("Not supported action");
    return error::CLIENT_NOT_SUPPORTED;
}

int32_t CheckTopologyAct() {
    DSTREAM_DEBUG("check topology action trigged...");

    int32_t ret = error::OK;
    Application app;
    ret = BuildApp(g_cmd_line_args.app_dir, &app);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to check app from dir:[%s]",
                     g_cmd_line_args.app_dir.c_str());
        return ret;
    }
    DSTREAM_INFO("check topology action ...OK");
    return ret;
}

int32_t SubmitAppAct() {
    DSTREAM_DEBUG("submit app action trigged...");

    int32_t ret = error::OK;

    /* check if the appname existed*/
    std::string app_name;
    ret = GetAppName(g_cmd_line_args.app_dir, false, false, false, &app_name);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to get app name");
        return ret;
    }
    Application existed_app;
    ret = g_pc->GetAppTopology(app_name, existed_app);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to get app topology");
        return ret;
    }
    if (existed_app.id().id() != static_cast<uint64_t>(-1)) {
        DSTREAM_WARN("the app already existed, submit denied");
        return error::CLIENT_SUBMIT_APP_FAIL;
    }

    Application app;
    ret = BuildApp(g_cmd_line_args.app_dir, &app);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to build app from dir:[%s]",
                     g_cmd_line_args.app_dir.c_str());
        return error::CLIENT_SUBMIT_APP_FAIL;
    }

    /* submit topoloy */
    int32_t pm_retry;
    g_cfg_map.GetIntValue(config_const::kClientToPMRetryName, &pm_retry);
    for (int32_t i = 0; i < pm_retry; i++) {
        ret = g_pc->SubmitApp(&app);
        if (ret == error::OK) {
            DSTREAM_INFO("submit app[%lu] ...OK", g_app_id);
            return error::OK;
        }
        DSTREAM_WARN("fail to submit app to pm, retry_time:[%d]", i);
    }
    DSTREAM_WARN("fail to submit app to pm");
    return ret;
}

int32_t DecommissionAppAct() {
    int32_t ret = error::OK;
    std::string app_string;
    if (!g_cmd_line_args.app_name.empty()) {
        app_string = g_cmd_line_args.app_name;
        DSTREAM_INFO("kill app[%s] ...", app_string.c_str());
        ret = g_pc->DecommissionApp(g_cmd_line_args.app_name);
    } else {
        StringAppendNumber(&app_string, g_cmd_line_args.app_id);
        DSTREAM_INFO("kill app[%s] ...", app_string.c_str());
        ret = g_pc->DecommissionApp(g_cmd_line_args.app_id);
    }
    if (ret < error::OK) {
        DSTREAM_WARN("fail to kill app:[%s]", app_string.c_str());
        return ret;
    }
    DSTREAM_INFO("kill app[%s] ...OK", app_string.c_str());
    return error::OK;
}

int32_t DecommissionProcessorAct() {
    // DSTREAM_DEBUG("decommission processor action trigged...");
    DSTREAM_INFO("operation:[decommission processor] not supported yet");
    // DSTREAM_INFO("decommission processor action ...OK");
    // return error::OK;
    return error::CLIENT_NOT_SUPPORTED;
}

int32_t RestartProcessorAct() {
    // DSTREAM_DEBUG("restart processor action trigged...");
    // DSTREAM_INFO("restart processor action ...OK");
    // return error::OK;
    return error::CLIENT_NOT_SUPPORTED;
}

int32_t MigratePeAct() {
    // DSTREAM_DEBUG("migrate pe action trigged...");
    DSTREAM_INFO("operation:[migrate pe] not supported yet");
    // DSTREAM_INFO("migrate pe action ...OK");
    // return error::OK;
    return error::CLIENT_NOT_SUPPORTED;
}

int32_t GetUserAct() {
    DSTREAM_DEBUG("get user action trigged...");

    std::set<User, application_tools::CompareUser> userinfo;
    std::set<User, application_tools::CompareUser>::iterator it;

    userinfo = g_pc->GetUser();

    for ( it = userinfo.begin(); it != userinfo.end(); it++ ) {
        printf("%s\n", (*it).username().c_str());
    }

    DSTREAM_INFO("get user action ...OK");
    return error::OK;
}

int32_t UpdateUserAct() {
    DSTREAM_DEBUG("update user action trigged...");

    int32_t ret = g_pc->UpdateUser(g_cmd_line_args.username, g_cmd_line_args.password);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to update user:[%s]", g_cmd_line_args.username.c_str());
        return ret;
    }
    DSTREAM_INFO("update user action ...OK");
    return GetUserAct();
}

int32_t AddUserAct() {
    DSTREAM_DEBUG("add user action trigged...");

    int32_t ret = g_pc->AddUser(g_cmd_line_args.username, g_cmd_line_args.password);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to add user:[%s]", g_cmd_line_args.username.c_str());
        return ret;
    }
    DSTREAM_INFO("add user...OK");
    return GetUserAct();
}
int32_t DelUserAct() {
    DSTREAM_DEBUG("delete user action trigged...");

    int32_t ret = g_pc->DeleteUser(g_cmd_line_args.username);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to delete user:[%s]", g_cmd_line_args.username.c_str());
        return ret;
    }
    DSTREAM_INFO("delete user ...OK");
    return GetUserAct();
}

int32_t AddSubTreeAct() {
    DSTREAM_DEBUG("add sub-tree to topology action trigged...");
    int32_t ret = error::OK;
    /* sub-tree validity check */
    Application sub_tree;
    ret = InitApp(g_cmd_line_args.app_dir, true, true, false, false, &sub_tree);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to init sub_tree");
        return ret;
    }
    ret = CheckTopology(sub_tree, false);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to check sub_tree");
        return ret;
    }

    ret = g_pc->AddSubTree(sub_tree);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to add sub_tree");
        return ret;
    }

    DSTREAM_INFO("add sub-tree to topology ...OK");
    return error::OK;
}

int32_t DelSubTreeAct() {
    DSTREAM_DEBUG("delete sub-tree from topology action trigged...");
    int32_t ret = error::OK;
    /* sub-tree validity check */
    Application sub_tree;
    ret = InitApp(g_cmd_line_args.app_dir, false, true, false, false, &sub_tree);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to init sub_tree");
        return ret;
    }
    ret = CheckTopology(sub_tree, false);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to check sub_tree");
        return ret;
    }

    ret = g_pc->DelSubTree(sub_tree);
    if ( ret < error::OK ) {
        DSTREAM_WARN("fail to del sub_tree");
        return ret;
    }

    DSTREAM_INFO("delete sub-tree from topology ...OK");
    return error::OK;
}

int32_t AddImporterAct() {
    DSTREAM_DEBUG("add importer from topology action trigged...");
    int32_t ret = error::OK;
    /* import validity check */
    Application importer;
    ret = InitApp(g_cmd_line_args.app_dir, true, false, true, false, &importer);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init importer");
        return ret;
    }

    ret = g_pc->AddImporter(importer);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to add importer");
        return ret;
    }

    DSTREAM_INFO("add importer into topology ...OK");
    return error::OK;
}

int32_t DelImporterAct() {
    DSTREAM_DEBUG("del importer from topology action trigged ...");
    int32_t ret = error::OK;
    /* importer validity check */
    Application importer;
    ret = InitApp(g_cmd_line_args.app_dir, false, false, true, false, &importer);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init importer");
        return ret;
    }

    ret = g_pc->DelImporter(importer);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to del importer");
        return ret;
    }

    DSTREAM_INFO("del importer from topology ...OK");
    return ret;
}

int32_t UpdateParallelismAct() {
    DSTREAM_DEBUG("update parallelism action trigged ...");
    int32_t ret = error::OK;
    /* processor validity check */
    Application update;
    ret = InitApp(g_cmd_line_args.app_dir, false, false, false, false, &update);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init update");
        return ret;
    }

    if (update.topology().importer_id_size()) {
        DSTREAM_WARN("you can not update parallelism of importer");
        return error::CLIENT_UPDATE_PARA_FAIL;
    }

    ret = g_pc->UpdateParallelism(update);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to update parallelism");
        return ret;
    }
    DSTREAM_INFO("update parallelism ...OK");
    return ret;
}

int32_t FlowControlAct() {
    DSTREAM_DEBUG("flow control action trigged ...");
    int32_t ret = error::OK;

    std::string cmd_string = g_cmd_line_args.cmd;
    std::string app_string;
    std::string processor  = g_cmd_line_args.processor;

    if (!g_cmd_line_args.app_name.empty()) {
        app_string = g_cmd_line_args.app_name;
        DSTREAM_INFO("flow control app[%s] ...", app_string.c_str());
        ret = g_pc->FlowControlApp(g_cmd_line_args.app_name, processor, cmd_string);
    } else {
        StringAppendNumber(&app_string, g_cmd_line_args.app_id);
        DSTREAM_INFO("flow control app[%s] ...", app_string.c_str());
        ret = g_pc->FlowControlApp(g_cmd_line_args.app_id, processor, cmd_string);
    }

    if (ret < error::OK) {
        DSTREAM_WARN("fail to flow control app:[%s], error:[%s]",
                app_string.c_str(), error::get_text(ret));
        return ret;
    }
    printf("%s\n", g_pc->FlowControlInfo().c_str());
    DSTREAM_INFO("flow control app[%s] ...OK", app_string.c_str());
    return ret;
}

int32_t UpdateProcessorAct() {
    DSTREAM_DEBUG("update processor action trigged ...");
    int32_t ret = error::OK;
    /* processor validity check */
    Application update;
    if (g_cmd_line_args.revision == -1) {
        ret = InitApp(g_cmd_line_args.app_dir, true, false, false, true, &update);
    } else {
        ret = InitApp(g_cmd_line_args.app_dir, false, false, false, true, &update);
    }
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init update");
        return ret;
    }

    ret = g_pc->UpdateProcessor(update);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to update processor");
        return ret;
    }
    DSTREAM_INFO("update processor ...OK");
    return ret;
}

} // namespace client
} // namespace dstream
