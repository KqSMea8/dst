/**
* @file   action.h
* @brief    action mapper
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_CLIENT_DSTREAM_ACTION_H__ //NOLINT
#define __DSTREAM_CC_CLIENT_DSTREAM_ACTION_H__ //NOLINT

#include <stdint.h>
#include <map>
#include <string>

namespace dstream {
namespace client {

/**
* @brief    client action
* @author   konghui, konghui@baidu.com
* @date   2011-12-16
*/
class Action {
public:
    enum ActionCode {
        UNSUPPORTED,
        SUBMIT_APP,
        CHECK_TOPOLOGY,
        DECOMMISSION_APP,
        KILL_APP,
        DECOMMISSION_PROCESSOR,
        RESTART_PROCESSOR,
        MIGRATE_PE,
        GET_USER,
        UPDATE_USER,
        ADD_USER,
        DEL_USER,
        ADD_SUB_TREE,
        DEL_SUB_TREE,
        ADD_IMPORTER,
        DEL_IMPORTER,
        UPDATE_PARA,
        FLOW_CONTROL,
        UPDATE_PRO,
        ACTION_END
    };

    typedef int32_t (*ActionExec)();

    struct ActionVec {
        const char* text;
        ActionExec  exec;
    };

public:
    Action();
    int32_t SetAction(const char* action_str);
    int32_t Run();

private:
    typedef std::map<ActionCode, ActionVec> ActionMap;
    ActionMap   action_map_;
    ActionCode  action_code_;
    ActionExec  action_exec_;
};

int32_t CheckCommand();
int32_t CommandParse(int, char**);

int32_t DefaultAct();
int32_t CheckTopologyAct();
int32_t SubmitAppAct();
int32_t DecommissionAppAct();
int32_t DecommissionProcessorAct();
int32_t RestartProcessorAct();
int32_t MigratePeAct();
int32_t GetUserAct();
int32_t UpdateUserAct();
int32_t AddUserAct();
int32_t DelUserAct();
int32_t AddSubTreeAct();
int32_t DelSubTreeAct();
int32_t AddImporterAct();
int32_t DelImporterAct();
int32_t UpdateParallelismAct();
int32_t FlowControlAct();
int32_t UpdateProcessorAct();

} // namespace client
} // namespace dstream

#endif // __DSTREAM_CC_CLIENT_DSTREAM_ACTION_H__ //NOLINT
