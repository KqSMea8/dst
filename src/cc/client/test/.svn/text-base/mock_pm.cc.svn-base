/**
* @brief  mock pm for client unit test
* @author fangjun,fangjun02@baidu.com
* @date   2013-01-29
*/
#include "mock_pm.h"
#include "common/application_tools.h"

namespace dstream {
namespace client {

using namespace rpc_server;
using namespace rpc_const;
using namespace application_tools;

/*static void ConstructFakeApp(Application& app, int process_num = 3,
                             int base = 2, uint64_t memory = 1024 * 1024 * 1024,
                             int paralism = 1 ) {
  app.mutable_id()->set_id(256);
  app.set_name("test");
  app.set_descr("used for test");
  app.set_flow_control(FLOW);
  User* app_user = app.mutable_user();
  app_user->set_username("test");
  app_user->set_password("test");
  Topology* topology = app.mutable_topology();
  topology->set_processor_num(process_num);
  char process_name[1000];
  for (int i = 0; i < process_num; ++i) {
    Processor* processor = topology->add_processors();
    sprintf(process_name, "test_process_%d", i);
    processor->set_name(process_name);
    processor->set_descr(process_name);
    processor->mutable_id()->set_id(i);
    if (i < 2) {
      processor->set_role(IMPORTER);
    } else {
      processor->set_role(WORK_PROCESSOR);
    }
    processor->set_exec_cmd("test_bin");
    Resource* resource = processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    processor->set_parallism(base * (i + 1));
    processor->set_backup_num(paralism);
    if (i > 1) {
      Subscribe* sub = processor->mutable_subscribe();
      ProcessorSub* process_sub = sub->add_processor();
      sprintf(process_name, "test_process_%d", i - 1);
      process_sub->set_name(process_name);
      process_sub->set_tags("all");
      process_sub->set_username("test");
      process_sub->set_password("test");
    }
    processor->set_pn_group("default");
  }
  app.set_status(RUN);

}*/

MockPM::MockPM() {
}

MockPM::~MockPM() {
}

bool MockPM::StartPM() {
    if (RegisterProtocol(kLoginMethod, (LoginProtocol*)this) &&
        RegisterProtocol(kApplyAppIDMethod, (ApplyAppIDProtocol*)this) &&
        RegisterProtocol(kSubmitAppMethod, (SubmitAppProtocol*)this) &&
        RegisterProtocol(kNotifyPMMethod, (NotifyPMProtocol*)this) &&
        RegisterProtocol(kKillAppMethod, (KillAppProtocol*)this) &&
        RegisterProtocol(kUpdateProcessorMethod, (UpdateProcessorProtocol*)this) &&
        RegisterProtocol(kUpdateProDoneMethod, (UpdateProDoneProtocol*)this) &&
        RegisterProtocol(kUpdateTopologyMethod, (UpdateTopologyProtocol*)this) &&
        RegisterProtocol(kFlowControlMethod, (FlowControlProtocol*)this) &&
        RegisterProtocol(kUserOpMethod, (UserOpProtocol*)this) &&
        StartServer()) {
        m_pm.set_host(server_ip());
        m_pm.set_submit_port(server_port());
        m_pm.set_report_port(server_port());
        return true;
    }
    DSTREAM_WARN("start pm fail");
    return false;
}

void MockPM::StopPM() {
    StopServer();
}

std::string MockPM::LOGIN(const User* request_user, LoginAck* response) {
    response->mutable_user()->set_version(VERSION);
    if (!(request_user->username() == "client_ut" &&
          request_user->password() == "client_ut")) {
        return rpc_const::kFail;
    }
    if(!request_user->has_version() ||
        strncmp(request_user->version().c_str(), VERSION, config_const::kVersionLen) > 0 ||
        strncmp(request_user->version().c_str(),
               config_const::kPMMinRequiredClientVersion, config_const::kVersionLen) < 0) {
        response->set_code(LoginAck_ErrorCode_VERSION_MISSMATCH);
        return rpc_const::kFail;
    }
    response->mutable_user()->set_session_id(123);
    return rpc_const::kOK;
}

std::string MockPM::ASKFORAPPID(const AppID* app_id, AppID* response_app_id) {
    if (rpc_flag[ASKFORAPPID_FAIL] == -1) {
        return rpc_const::kFail;
    }
    response_app_id->set_id(1);
    return rpc_const::kOK;
}

std::string MockPM::SUBMITAPP(const Application* app, FileConfig* file_response) {
    if (rpc_flag[SUBMITAPP_FAIL] == -1) {
        return rpc_const::kFail;
    }
    file_response->set_path("hdfs://hello");
    return rpc_const::kOK;
}

std::string MockPM::FILEUPLOADDONE(const NotifyPM* notify, NotifyAck* /*response*/) {
    if (rpc_flag[FILEUPLOADDONE_FAIL] == -1) {
        return rpc_const::kFail;
    }
    return rpc_const::kOK;
}

std::string MockPM::KILLAPP(const Application* app, AppID* response) {
    if (rpc_flag[KILLAPP_FAIL] == -1) {
        return rpc_const::kFail;
    }
    if ("client_ut" == app->name() || 1 == app->id().id()) {
        return rpc_const::kOK;
    } else {
        return rpc_const::kFail;
    }
}

std::string MockPM::USEROP(const UserOperation* userop, UserOperationReply* response) {
    if (rpc_flag[USEROP_FAIL] == -1) {
        return rpc_const::kFail;
    }
    switch (userop->type()) {
    case UserOperation::GET:
        response->set_replytype(UserOperationReply::GET);
        User* add = response->add_user();
        add->set_username("client_ut");
        add->set_password("client_ut");
        break;
    default:
        response->set_replytype(UserOperationReply::ADD);
        break;
    }
    return rpc_const::kOK;
}

std::string MockPM::UPDATETOPOLOGY(const UpdateTopology* update, UpdateTopology* res_app) {
    if (rpc_flag[UPDATETOPOLOGY_FAIL] == -1 ||
        rpc_flag[UPDATETOPOLOGY_ACK_FAIL] == -1) {
        return rpc_const::kFail;
    }
    if (!update->has_app()) {
        DSTREAM_WARN("update has no app");
        return kFail;
    }
    if (!update->app().has_id() && !update->app().has_name()) {
        DSTREAM_WARN("update has neigther app id nor app name");
        return kFail;
    }
    if (rpc_flag[ADD_SUB_GET_FAIL] == -1) {
        res_app->set_type(UpdateTopology_Operation_ADD_SUB_OK);
        return kOK;
    }
    Application submit_app;
    ConstructFakeAppClient(&submit_app);
    switch (update->type()) {
    case UpdateTopology_Operation_ADD:
        res_app->set_type(UpdateTopology_Operation_ADD_SUB_OK);
        *(res_app->mutable_app()) = submit_app;
        if (rpc_flag[INVAILD_TOPO_FAIL] == -1) {
            res_app->mutable_app()->mutable_topology()->set_processor_num(256);
            return kOK;
        }
        return kOK;
    case UpdateTopology_Operation_DEL:
        res_app->set_type(UpdateTopology_Operation_DEL_SUB_OK);
        *(res_app->mutable_app()) = submit_app;
        if (rpc_flag[INVAILD_TOPO_FAIL] == -1) {
            res_app->mutable_app()->mutable_topology()->set_processor_num(256);
            return kOK;
        }
        return kOK;

    case UpdateTopology_Operation_UPDATE_PARA:
        res_app->set_type(UpdateTopology_Operation_UPDATE_PARA_OK);
        *(res_app->mutable_app()) = submit_app;
        return kOK;

    case UpdateTopology_Operation_ADD_SUB_OK:
        return kOK;

    case UpdateTopology_Operation_ADD_SUB_FAIL:
        return kOK;

    case UpdateTopology_Operation_DEL_SUB_OK:
        return kOK;

    case UpdateTopology_Operation_DEL_SUB_FAIL:
        return kOK;

    case UpdateTopology_Operation_UPDATE_PARA_OK:
        return kOK;

    case UpdateTopology_Operation_UPDATE_PARA_FAIL:
        return kOK;
    }
    return kOK;
}

std::string MockPM::FLOWCONTROL(const FlowControlRequest* request,
                                FlowControlResponse* response) {
    if (rpc_flag[FLOWCONTROL_FAIL] == -1) {
        return rpc_const::kFail;
    }
    return rpc_const::kOK;
}

std::string MockPM::UPDATEPROCESSOR(const UpdateProRequest* request, UpdateProResponse* response) {
    if (rpc_flag[UPDATE_PRO_FAIL] == -1) {
        return rpc_const::kFail;
    }
    response->set_revision(1);
    return rpc_const::kOK;
}

std::string MockPM::UPDATEPRODONE(const UpdateProDone* request, rpc_const::UpdateProAck* response) {
    if (rpc_flag[UPDATE_PRO_DONE_FAIL] == -1) {
        return rpc_const::kFail;
    }
    return rpc_const::kOK;
}

void MockPM::DumpRPCFail() {
    for (int i = 0; i < rpc_flag_num; i++ ) {
        DSTREAM_WARN("%d rpc fail flag is %d", i, rpc_flag[i]);
    }
}

}//namespace client
}//namespace dstream
