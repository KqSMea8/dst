/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    client operation
*********************************************************************/
#include "common/utils.h"
#include "client_op.h"
#include "processmaster/pn_client_status.h"
#include "processmaster/zk_watcher.h"

namespace dstream {
namespace client_op {
using namespace rpc_const;
using namespace config_const;
using namespace application_tools;
using namespace pn_client_status;
using namespace meta_manager;
using namespace zk_meta_manager;

const char* kBigpipePath = "test-inf-dstream";
const char* kHdfsPath    = "test-inf-dstream";
const char* kUser        = "test";
const char* kNoVersion   = "client.no.version";
const char* kLessEqualVersion = "1-5-1-0";
const char* kGreatEqualVersion = "1-8-0-0";
const char* kRightVersion = VERSION;
//static int client_serial = 0;

void InitMeta(const char* /*meta_manager_name*/,
              ClusterConfig* old_config)
{
    Config config;
    ASSERT_EQ(error::OK, config.ReadConfig(kTestPMConfigFile));
    ASSERT_TRUE(MetaManager::InitMetaManagerContext(&config));
    ClusterConfig cluster_config;
    MetaManager* meta = MetaManager::GetMetaManager();
    ASSERT_TRUE(NULL != meta);
    if (NULL != old_config &&
        meta->GetClusterConfig(&cluster_config) == error::OK) {
        *old_config = cluster_config;
    }
    cluster_config.Clear();
    cluster_config.set_hdfs_path(kHdfsPath);
    User* add_user = cluster_config.add_users();
    add_user->set_password(kUser);
    add_user->set_username(kUser);
    ASSERT_EQ(error::OK, meta->UpdateClusterConfig(cluster_config));
}

std::string GetPMUri() {
    MetaManager* meta = MetaManager::GetMetaManager();
    PM pm;
    for (int i = 0; i < 1000; ++i) {
        meta->GetPM(&pm);
        if (pm.submit_port() == 0) {
            DSTREAM_INFO("get sumit port is not correct");
            usleep(200 * 1000);
            continue;
        }
        break;
    }
    if (pm.has_host() && pm.has_submit_port() && pm.submit_port() > 0) {
        return GetUri(pm.host(), pm.submit_port());
    }
    return "";
}

void LogIn(const char* version , const char* user,
           const char* passwd, std::string& res,
           uint64_t& session_id, ZKMetaManager* zk_meta) {
    Config config;
    ASSERT_EQ(error::OK, config.ReadConfig(kTestPMConfigFile));
    zk_config::ZKConfig zk_config(&config);
    char client_path[2000];
    timeval now;
    gettimeofday(&now, NULL);
    sprintf(client_path, "%s/%zu", zk_config.ClientRootPath().c_str(),
            now.tv_usec);
    User client;
    LoginAck response;
    client.set_username(user);
    client.set_password(passwd);
    client.set_hostname(client_path);
    if (strcmp(version, kNoVersion) != 0) {
        client.set_version(version);
    }
    if (NULL != zk_meta) {
        zk_meta->AddZKNode(client_path, &client, false);
    }
    ASSERT_EQ(0, rpc_call(GetPMUri(), kLoginMethod, client, response, &res, 1000 * 60 * 10));
    session_id = response.user().session_id();
}

void LogIn(std::string* res, uint64_t* session_id, ZKMetaManager* zk_meta) {
    std::string login_res;
    uint64_t s_id;
    LogIn(kRightVersion, kUser, kUser, login_res, s_id, zk_meta);
    if (NULL != res) {
        *res = login_res;
    }
    if (NULL != session_id) {
        *session_id = s_id;
    }
}

void AskForAppID(AppID& app_id, ZKMetaManager* zk_meta) {
    std::string rpc_res;
    uint64_t session_id;
    LogIn(kRightVersion, kUser, kUser, rpc_res, session_id, zk_meta);
    ASSERT_STREQ(kOK, rpc_res.c_str());
    AppID id;
    id.set_session_id(session_id);
    rpc_res = "";
    ASSERT_EQ(0, rpc_call(GetPMUri(), kApplyAppIDMethod, id, app_id, &rpc_res, 1000 * 60 * 10));
    ASSERT_STREQ(kOK, rpc_res.c_str());
}

static void SubmitApp(Application& submit_app, bool success, int process_num,
                      int base , uint64_t memory)
{
    ZKMetaManager* meta = (ZKMetaManager*)MetaManager::GetMetaManager();
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    /*ZkClient* zk_client = meta->zk_client();*/
    AppID replay_app_id;
    client_info ci;
    std::string res;
    FileConfig file_config;
    AskForAppID(replay_app_id, meta);
    ASSERT_EQ(status->FindClient(replay_app_id.session_id(), &ci), error::OK);
    ConstructFakeApp(&submit_app, process_num, base, memory);
    submit_app.set_session_id(replay_app_id.session_id());
    (*submit_app.mutable_id()) = replay_app_id;
    ASSERT_EQ(0, rpc_call(GetPMUri(), kSubmitAppMethod,
                          submit_app, file_config, &res, 1000 * 60 * 10));

    std::cout << kOK << res.c_str() << std::endl;
    ASSERT_STREQ(kOK, res.c_str());
    if (!success) {
        return;
    }
    NotifyPM notify_pm;
    NotifyAck notify_ack;
    notify_pm.set_session_id(replay_app_id.session_id());
    notify_pm.set_status(kOK);
    ASSERT_EQ(0, rpc_call(GetPMUri(), kNotifyPMMethod,
                          notify_pm, notify_ack, &res, 1000 * 60 * 10));
    ASSERT_STREQ(kOK, res.c_str());
    meta->AddScribeAppNode("test", "123");
    meta->AddScribePENode("test/temp", "123");
}

Application SubmitApp(bool success /* = true*/, int process_num /*= 2*/,
                      int base /*= 2*/, uint64_t memory /*= 1024 * 1024 * 1024*/)
{
    Application submit_app;
    SubmitApp(submit_app, success, process_num, base, memory);
    return submit_app;
}

//Add SubTree request
UpdateTopology UpdateTopoRequestTest(const Application& app, UTO operation)
{
    UpdateTopology uto, uto_res;
    uint64_t session_id;
    string res;
    LogIn(kRightVersion, kUser, kUser, res, session_id, (ZKMetaManager*)MetaManager::GetMetaManager());
    Application* uto_app = uto.mutable_app();
    *uto_app = app;
    uto.set_session_id(session_id);
    uto.set_type(operation);
    *(uto_res.mutable_app()) = uto.app();
    rpc_call(GetPMUri(), kUpdateTopologyMethod, uto, uto_res, &res, 1000 * 60 * 10);
    uto_res.set_session_id(session_id);
    return uto_res;
}

UpdateTopology AddSubTreeRequestTest(const Application& add_app)
{
    return UpdateTopoRequestTest(add_app, UpdateTopology_Operation_ADD);
}

void UpdateTopoOverTest(UpdateTopology& ut, UTO op)
{
    ut.set_type(op);
    UpdateTopology res;
    rpc_call(GetPMUri(), kUpdateTopologyMethod, ut, res, NULL, 1000 * 60 * 10);
}

void AddSubTreeTest(const Application& add_app)
{
    UpdateTopology uto = AddSubTreeRequestTest(add_app);
    Application* uto_add_app = uto.mutable_app();
    Topology* topoloy = uto_add_app->mutable_topology();
    Processor* add_processor = topoloy->add_processors();
    ProcessorID* pid = add_processor->mutable_id();
    pid->set_id(topoloy->processor_num());
    add_processor->set_backup_num(1);
    add_processor->set_parallism(8);
    add_processor->set_pn_group("default");
    add_processor->mutable_resource_require()->set_memory(1024 * 1024 * 1024);
    UpdateTopoOverTest(uto, UpdateTopology_Operation_ADD_SUB_OK);
}

//Send Del SubTree request
UpdateTopology DelSubTreeRequestTest(const Application& del_app)
{
    return UpdateTopoRequestTest(del_app, UpdateTopology_Operation_DEL);
}


void DelSubTreeTest(const Application& del_app)
{
    UpdateTopology uto = DelSubTreeRequestTest(del_app);
    Application* uto_add = uto.mutable_app();
    if ( uto_add->mutable_topology()->processors_size() == 0 ) {
        return;
    }
    Processor* processor = uto_add->mutable_topology()->mutable_processors()->ReleaseLast();
    delete processor;
    UpdateTopoOverTest(uto, UpdateTopology_Operation_DEL_SUB_OK);
}

//Send Update Parallelism request
UpdateTopology UpdateParaRequestTest(const Application& update) {
    return UpdateTopoRequestTest(update, UpdateTopology_Operation_UPDATE_PARA);
}

void UpdateParaTest(const Application& update) {
    UpdateTopology uto = UpdateParaRequestTest(update);
    UpdateTopoOverTest(uto, UpdateTopology_Operation_UPDATE_PARA_OK);
}

void AddApp(Application& app)
{
    MetaManager* meta = MetaManager::GetMetaManager();
    ConstructFakeApp(&app, 3, 2);
    if (meta->AddApplication(&app) != error::OK) {
        DSTREAM_WARN("add application fail");
    }
    app.set_status(RUN);
    meta->UpdateApplication(app);
    DSTREAM_DEBUG("add application");
}

void ClearApp()
{
    ZKMetaManager* meta_ = (ZKMetaManager*)MetaManager::GetMetaManager();
    list<Application> app_list;
    PM pm;
    if (meta_->GetApplicationList(&app_list) == error::OK &&
        !app_list.empty()) {
        for (list<Application>::iterator it = app_list.begin(), end = app_list.end();
             it != end; ++it) {
            if (meta_->GetPM(&pm) == error::OK) {
                KillApp(it->id(), meta_);
            }
            meta_->DeleteApplication(it->id());
        }
    }
}

void CheckApp(const AppID& app_id)
{
    MetaManager* meta = MetaManager::GetMetaManager();
    bool pe_normal = true;
    for (int i = 0; i < 1000; ++i) {
        PE_LIST pe_list;
        ASSERT_EQ(error::OK, meta->GetAppProcessElements(app_id, &pe_list));
        PE_LIST::iterator pe_end = pe_list.end();
        pe_normal = true;
        for (PE_LIST::iterator pe_it = pe_list.begin(); pe_it != pe_end; ++pe_it) {
            if (!CheckPENormal(*pe_it, app_id, pe_it->pe_id())) {
                DSTREAM_INFO("pe(%lu) is not normal", pe_it->pe_id().id());
                pe_normal = false;
                break;
            }
        }
        if (!pe_normal) {
            usleep(200 * 1000);
        } else {
            break;
        }
    }
    ASSERT_TRUE(pe_normal);
}

//Add Processor for app, pp is processor's paralism
void AddProcessor(Application& app, int& pp, int& backup_num) {
    pp = 3;
    backup_num = 1;
    Topology* topology = app.mutable_topology();
    Processor* add_processor = topology->add_processors();
    add_processor->mutable_id()->set_id(topology->processor_num());
    add_processor->set_parallism(pp);
    add_processor->set_backup_num(backup_num);
}

//Remove Processor for app, pp is processor's paralism
void RemoveProcessor(Application& app, int& pp, int& backup_num) {
    Topology* topology = app.mutable_topology();
    Processor* remove_processor = topology->mutable_processors()->ReleaseLast();
    pp = remove_processor->parallism();
    backup_num = remove_processor->backup_num();
    delete remove_processor;
}

void WaitForPMEventOver(ProcessMaster& pm) {
    for (int i = 0; i < 100;  ++i) {
        if (pm.event_manager()->wait_event() && pm.event_manager()->EventSize() == 0
            && pm.event_manager()->ExecuEventSize() == 0) {
            return;
        }
        usleep(100 * 1000);
    }
    ASSERT_TRUE(false);
}

void KillApp(const AppID& app_id, ZKMetaManager* zm_meta) {
    std::string res;
    uint64_t session_id;
    LogIn(kRightVersion, kUser, kUser, res, session_id, zm_meta);
    Application app;
    AppID kill_app_id;
    *(app.mutable_id()) = app_id;
    app.set_session_id(session_id);
    ASSERT_EQ(0, rpc_call(GetPMUri(), kKillAppMethod, app, kill_app_id, NULL, 1000 * 60 * 10));
}

void FlowControlApp(const AppID& app_id,
        FlowControlRequest::Operation op, const std::string& processor, int32_t limit) {
    uint64_t session_id;
    string res;
    LogIn(kRightVersion, kUser, kUser, res, session_id, (ZKMetaManager*)MetaManager::GetMetaManager());
    FlowControlRequest request;
    FlowControlResponse response;
    request.set_session_id(session_id);
    request.mutable_app_id()->CopyFrom(app_id);

    request.set_processor(processor);
    request.set_cmd(op);
    request.set_limit(limit);
    std::cout << "set qps limit: " << limit << std::endl;
    ASSERT_EQ(0, rpc_call(GetPMUri(), kFlowControlMethod, request, response, NULL, 1000 * 60 * 10));
}

void BlackListOperation(const BlackListOperationRequest* request,
                        BlackListOperationResponse* response) {
    ASSERT_EQ(0, rpc_call(GetPMUri(), kBlackListMethod, *request, *response, NULL, 1000 * 60 * 10));
    

}

} // namespace test_client_op
} // namespace dstream
