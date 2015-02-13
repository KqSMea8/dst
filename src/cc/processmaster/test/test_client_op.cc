/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test client operation
*********************************************************************/

#include "client_op.h"
#include "gtest/gtest.h"
#include "processmaster/pn_client_status.h"

namespace dstream {
namespace test_client_op {
using namespace rpc_const;
using namespace config_const;
using namespace application_tools;
using namespace meta_manager;
using namespace client_op;
using namespace pn_client_status;

/************************************************************************/
/* Test case for client operation                                       */
/************************************************************************/
class TestClientOp : public ::testing::Test {
protected:
    //setup and tear down
    void SetUp();
    void TearDown();

    void TestLogin();
    void TestSubmitApp(int start_app_id);
    void TestPMRestartSubmit();
    void TestKillApp();
    //Test for update topology
    void UpdateSubTree(UpdateTopology_Operation operation,
                       bool succ = true, bool check_pe_list = true);
    //Test for update parallelism
    enum ParaChange {
        ADD = 0,
        REDUCE
    };
    void TestUpdatePara(ParaChange para, bool succ = true);
    void TestUpdateTopoClientFail(UpdateTopology_Operation operatrion,
                                  AppStatus app_status);
    //test update topology of client and then pm restart
    void TestUTCFPMRestart(UpdateTopology_Operation operatrion,
                           AppStatus app_status);
    void TestMultiUpdateTopology();
    void TestPMRestart(UpdateTopology_Operation op, AppStatus app_status);

    void UpdateTopoRequest(UpdateTopology& ut_res, std::string& rpc_res,
                           UpdateTopology_Operation operation);
    void UpdateTopologyResult(UpdateTopology& ut, std::string& res,
                              UpdateTopology_Operation op);
    int GetPNStatusSize();
    //test for flow control
    void TestFlowControl();
    // 
    void TestBlackListOperation();
protected:
    list<MockPN> pn_list_;
    ProcessMaster pm_;
    ZKMetaManager* meta_;
    ClusterConfig old_cluster_config;
};

void TestClientOp::SetUp() {
    int pn_num = 4;
    InitMeta(kZKMetaManagerName, &old_cluster_config);
    ClearApp();
    meta_ = (ZKMetaManager*)MetaManager::GetMetaManager();
    meta_->DeletePM();
    pm_.Start(kTestPMConfigFile);
    for (int i = 0; i < pn_num; ++i) {
        pn_list_.insert(pn_list_.end(), MockPN())->StartPN();
    }
    for (list<MockPN>::iterator pn_it = pn_list_.begin();
         pn_it != pn_list_.end(); ++pn_it) {
        pn_it->ReportStatus();
    }
}

void TestClientOp::TearDown() {
    if (old_cluster_config.users_size() > 0) {
        meta_->UpdateClusterConfig(old_cluster_config);
    }
    pm_.StopForTest();
    ClearApp();
    MetaManager::DestoryMetaManager();
}

/************************************************************************/
/* Test for login : valid user and invalid user , also for session id   */
/* test client  same version ,client no version,client wrong verion     */
/************************************************************************/
void TestClientOp::TestLogin() {
    std::string res;
    uint64_t session_id[10];
    LogIn(kRightVersion, kUser, kUser, res, session_id[0], meta_);
    ASSERT_STREQ(kOK, res.c_str());
    LogIn(kPMMinRequiredClientVersion, kUser, kUser, res, session_id[0], meta_);
    ASSERT_STREQ(kOK, res.c_str());
    LogIn(kLessEqualVersion, kUser, kUser, res, session_id[0], meta_);
    ASSERT_STREQ(kFail, res.c_str());
    LogIn(kGreatEqualVersion, kUser, kUser, res, session_id[0], meta_);
    ASSERT_STREQ(kFail, res.c_str());
    LogIn(kNoVersion, kUser, kUser, res, session_id[0], meta_);
    ASSERT_STREQ(kFail, res.c_str());
    for (int i = 0; i < 10; ++i) {
        LogIn(kRightVersion, kUser, kUser, res, session_id[i], meta_);
        ASSERT_STREQ(kOK, res.c_str());
        if (i > 0) {
            ASSERT_GT(session_id[i], session_id[i - 1]);
        }
    }
    uint64_t sid = session_id[0];
    LogIn(kRightVersion, kUser, "noteqer", res, session_id[0], meta_);
    ASSERT_STREQ(kFail, res.c_str());
    client_info ci;
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    ASSERT_EQ(status->FindClient(sid, &ci), error::OK);
    meta_->zk_client()->DeleteEphemeralNode(ci.user.hostname().c_str());
    sleep(1);
    ASSERT_NE(status->FindClient(sid, &ci), error::OK);
    AppID app_id;
    app_id.set_session_id(sid);
    res = "";
    rpc_call(GetPMUri(), kApplyAppIDMethod, app_id, app_id, &res, 1000 * 60 * 10);
    ASSERT_EQ(kFail, res);
}

/************************************************************************/
/* Test for Submit App, include                                         */
/*App ID is uniq, when client is not connect, the app is killed         */
/************************************************************************/
void TestClientOp::TestSubmitApp(int start_app_id)
{
    if (pm_.event_manager() != NULL) {
        pm_.event_manager()->Suspend();
    }
    for (int i = 0; i < 5; ++i) {
        Application submit_app = SubmitApp();
        ASSERT_EQ(start_app_id + i, static_cast<int>(submit_app.id().id()));
    }
    list<Application> app_list;
    ASSERT_EQ(error::OK, meta_->GetApplicationList(&app_list));
    char path[4096];
    for (list<Application>::iterator app_iter = app_list.begin();
         app_iter != app_list.end();
         ++app_iter) {
        sprintf(path, "%s/%s/%zd/app", kHdfsPath, kUser,
                static_cast<ssize_t>(app_iter->id().id()));
        ASSERT_STREQ(path, app_iter->app_path().path().c_str());
        sprintf(path, "%s/%s/%zd/check_point", kHdfsPath, kUser,
                static_cast<ssize_t>(app_iter->id().id()));
        ASSERT_STREQ(path, app_iter->checkpoint_path().path().c_str());
        list<ProcessorElement> pe_list;
        ASSERT_EQ(error::OK, meta_->GetAppProcessElements(app_iter->id(), &pe_list));
        for (list<ProcessorElement>::iterator pe_iter = pe_list.begin();
             pe_iter != pe_list.end();
             ++pe_iter) {
            Processor processor;
            ASSERT_TRUE(GetProcessor(*app_iter, *pe_iter, &processor));
            uint64_t pe_int = pe_iter->pe_id().id();
            uint64_t serial = ((pe_int & 0xFFFFFFFF) >> 8);
            sprintf(path, "%ld_%s_%zu", app_iter->id().id(),
                    processor.name().c_str(),
                    static_cast<size_t>(serial));
            ASSERT_STREQ(path, pe_iter->mutable_log()->pipe_name().c_str());
        }
    }
}

/************************************************************************/
/* Test submit app when pm restart of appid increase                    */
/************************************************************************/
void TestClientOp::TestPMRestartSubmit()
{
    pm_.Stop();
    {
        ProcessMaster test_pm;
        test_pm.Start(kTestPMConfigFile);
        test_pm.event_manager()->Suspend();
        TestSubmitApp(1);
    }
    pn_list_.clear();
    MetaManager::DestoryMetaManager();
    InitMeta(kZKMetaManagerName);
    meta_ = (ZKMetaManager*)MetaManager::GetMetaManager();
    meta_->DeletePM();
    ProcessMaster test_pm;
    ASSERT_TRUE(test_pm.Start(kTestPMConfigFile));
    for (int i = 0; i < 60 * 10; ++i) {
        if (test_pm.working_master()) {
            break;
        }
        usleep(100 * 1000);
    }
    ASSERT_TRUE(test_pm.working_master());
    test_pm.event_manager()->Suspend();
    TestSubmitApp(6);
}

int TestClientOp::GetPNStatusSize() {
    int size = 0;
    for (list<MockPN>::iterator pn_iter = pn_list_.begin();
         pn_iter != pn_list_.end(); ++pn_iter) {
        size += pn_iter->status().size();
    }
    return size;
}

/************************************************************************/
/* Test Kill App                                                        */
/************************************************************************/
void TestClientOp::TestKillApp()
{
    Application submit_app = SubmitApp();
    WaitForPMEventOver(pm_);
    ASSERT_LT(0, GetPNStatusSize());
    KillApp(submit_app.id(), meta_);
    WaitForPMEventOver(pm_);
    ASSERT_NE(error::OK, meta_->GetApplication(submit_app.id(), &submit_app));
    ASSERT_EQ(0, GetPNStatusSize());
}

/************************************************************************/
/* Add UpdateTopology Request                                           */
/************************************************************************/
void TestClientOp::UpdateTopoRequest(UpdateTopology& ut_res, std::string& res,
                                     UpdateTopology_Operation operation) {
    Application* app = ut_res.mutable_app();
    *app = SubmitApp();
    uint64_t session_id;
    LogIn(kRightVersion, kUser, kUser, res, session_id, meta_);
    app->set_session_id(session_id);
    UpdateTopology ut;
    ut.set_session_id(session_id);
    *(ut.mutable_app()) = *app;
    ut.set_type(operation);
    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod,
                          ut, ut_res, &res, 1000 * 60 * 10));
    ut_res.set_session_id(session_id);
}

void TestClientOp::UpdateTopologyResult(UpdateTopology& ut, std::string& res,
                                        UpdateTopology_Operation op) {
    ut.set_type(op);
    UpdateTopology response;
    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod, ut,
                          response, &res, 1000 * 60 * 10));
}

/************************************************************************/
/* Test Add SubTree success                                             */
/************************************************************************/
void TestClientOp::UpdateSubTree(UpdateTopology_Operation operation,
                                 bool succ /*= true*/,
                                 bool check_pe_list /*= true*/) {
    UpdateTopology ut_res, ut_response;
    std::string res;
    UpdateTopoRequest(ut_res, res, operation);
    ASSERT_STREQ(kOK, res.c_str());
    CheckApp(ut_res.app().id());
    if (!check_pe_list) {
        pm_.event_manager()->Suspend();
    }
    if (operation == UpdateTopology_Operation_ADD) {
        ASSERT_EQ(ut_res.app().status(), ADDTOPO);
    } else {
        ASSERT_EQ(ut_res.app().status(), DELTOPO);
    }
    Application* modify_app = ut_res.mutable_app();
    Application app;
    PE_LIST pe_list, new_pe_list;
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(modify_app->id(), &pe_list));
    Topology* topology = modify_app->mutable_topology();
    int pp = 3;
    int backup_num = 1;
    if (operation == UpdateTopology_Operation_ADD) {
        Processor* add_processor = topology->add_processors();
        ProcessorID* pid = add_processor->mutable_id();
        pid->set_id(topology->processor_num());
        add_processor->set_parallism(pp);
        add_processor->set_backup_num(backup_num);
        add_processor->set_pn_group("default");
        add_processor->mutable_resource_require()->set_memory(1024 * 1024 * 1024);
    } else {
        Processor* remove_processor = topology->mutable_processors()->ReleaseLast();
        pp = remove_processor->parallism();
        backup_num = remove_processor->backup_num();
        delete remove_processor;
    }
    if (succ) {
        if (operation == UpdateTopology_Operation_ADD) {
            ut_res.set_type(UpdateTopology_Operation_ADD_SUB_OK);
        } else {
            ut_res.set_type(UpdateTopology_Operation_DEL_SUB_OK);
        }
    } else {
        if (operation == UpdateTopology_Operation_ADD) {
            ut_res.set_type(UpdateTopology_Operation_ADD_SUB_FAIL);
        } else {
            ut_res.set_type(UpdateTopology_Operation_DEL_SUB_FAIL);
        }
    }
    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod, ut_res,
                          ut_response, &res, 1000 * 60 * 10));
    ASSERT_STREQ(kOK, res.c_str());
    if (!check_pe_list) {
        return;
    }
    //sleep(1);
    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    ASSERT_EQ(error::OK, meta_->GetApplication(modify_app->id(), &app));
    ASSERT_EQ(app.status(), RUN);
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(modify_app->id(),
                                                      &new_pe_list));
    if (succ) {
        if (operation == UpdateTopology_Operation_ADD) {
            ASSERT_EQ(pe_list.size() + pp * backup_num, new_pe_list.size());
        } else {
            ASSERT_EQ(pe_list.size() - pp * backup_num, new_pe_list.size());
        }
        CheckApp(app.id());
    } else {
        ASSERT_EQ(pe_list.size(), new_pe_list.size());
    }
}

/******************************************************
 * Test for Update parallelism                        *
 * ***************************************************/
void TestClientOp::TestUpdatePara(ParaChange para, bool succ) {
    UpdateTopology ut_res, ut_response;
    std::string res;
    UpdateTopoRequest(ut_res, res, UpdateTopology_Operation_UPDATE_PARA);
    ASSERT_STREQ(kOK, res.c_str());

    ASSERT_EQ(ut_res.app().status(), UPPARA);

    Application* modify_app = ut_res.mutable_app();
    Application app;
    PE_LIST pe_list, new_pe_list;
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(modify_app->id(), &pe_list));
    Topology* topology = modify_app->mutable_topology();
    Processor* processor = topology->mutable_processors(0);
    int parallelism = processor->parallism();
    if (para == ADD) {
        processor->set_parallism(parallelism + 1);
    } else {
        processor->set_parallism(parallelism - 1);
    }

    if (succ) {
        ut_res.set_type(UpdateTopology_Operation_UPDATE_PARA_OK);
    } else {
        ut_res.set_type(UpdateTopology_Operation_UPDATE_PARA_FAIL);
    }

    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod, ut_res,
                          ut_response, &res, 1000 * 60 * 10));
    ASSERT_STREQ(kOK, res.c_str());

    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    ASSERT_EQ(error::OK, meta_->GetApplication(modify_app->id(), &app));
    ASSERT_EQ(app.status(), RUN);
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(modify_app->id(),
                                                      &new_pe_list));
    if (succ) {
        if (para == ADD) {
            ASSERT_EQ(pe_list.size() + 1, new_pe_list.size());
        } else {
            ASSERT_EQ(pe_list.size() - 1, new_pe_list.size());
        }
        CheckApp(app.id());
    } else {
        ASSERT_EQ(pe_list.size(), new_pe_list.size());
    }
}

void TestClientOp::TestUpdateTopoClientFail(UpdateTopology_Operation operatrion,
                                            AppStatus /*app_status*/) {
    UpdateTopology ut_req;
    Application app;
    std::string res;
    UpdateTopoRequest(ut_req, res, operatrion);
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    client_info ci;
    ASSERT_EQ(status->FindClient(ut_req.session_id(), &ci), error::OK);
    usleep(100 * 1000);
    ASSERT_EQ(error::OK, meta_->GetApplication(ut_req.app().id(), &app));
    ASSERT_NE(app.status(), RUN);
    pn_list_.begin()->KillPN();
    meta_->zk_client()->DeleteEphemeralNode(ci.user.hostname().c_str());
    usleep(100 * 1000);
    ASSERT_EQ(error::OK, meta_->GetApplication(ut_req.app().id(), &app));
    ASSERT_EQ(app.status(), RUN);
    CheckApp(ut_req.app().id());
}

void TestClientOp::TestUTCFPMRestart(UpdateTopology_Operation operatrion,
                                     AppStatus app_status) {
    pm_.event_manager()->Suspend();
    UpdateTopology ut_req;
    std::string res;
    UpdateTopoRequest(ut_req, res, operatrion);
    pm_.Stop();
    list<Application> app_list;
    ASSERT_EQ(error::OK, meta_->GetApplicationList(&app_list));
    ASSERT_EQ(app_list.begin()->status(), app_status);
    ProcessMaster pm2;
    pm2.Start(kTestPMConfigFile);
    usleep(100 * 1000);
    app_list.clear();
    ASSERT_EQ(error::OK, meta_->GetApplicationList(&app_list));
    ASSERT_EQ(app_list.begin()->status(), RUN);
    return;
}

void TestClientOp::TestMultiUpdateTopology() {
    pm_.event_manager()->Suspend();
    std::string res;
    UpdateTopology ut_res, ut_response;
    AppID app_id;
    UpdateTopoRequest(ut_res, res, UpdateTopology_Operation_ADD);
    app_id = ut_res.app().id();
    ASSERT_STREQ(kOK, res.c_str());
    uint64_t session_id;
    LogIn(kRightVersion, kUser, kUser, res, session_id, meta_);
    ut_res.set_session_id(session_id);
    ut_res.set_type(UpdateTopology_Operation_ADD);
    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod, ut_res,
                          ut_response, &res, 1000 * 60 * 10));
    ASSERT_STREQ(kFail, res.c_str());
    LogIn(kRightVersion, kUser, kUser, res, session_id, meta_);
    ut_res.set_session_id(session_id);
    ut_res.set_type(UpdateTopology_Operation_DEL);
    *(ut_res.mutable_app()->mutable_id()) = app_id;
    ASSERT_EQ(0, rpc_call(GetPMUri(), kUpdateTopologyMethod,
                          ut_res, ut_response, &res, 1000 * 60 * 10));
    ASSERT_STREQ(kFail, res.c_str());
}

void TestClientOp::TestPMRestart(UpdateTopology_Operation op,
                                 AppStatus app_status)
{
    UpdateSubTree(op, true, false);
    pm_.Stop();
    list<Application> app_list;
    PE_LIST pe_list;
    ASSERT_EQ(error::OK, meta_->GetApplicationList(&app_list));
    ASSERT_EQ(1u, app_list.size());
    ASSERT_EQ(app_list.begin()->status(), app_status);
    meta_->GetAppProcessElements(app_list.begin()->id(), &pe_list);
    int pe_size = pe_list.size();
    ProcessMaster pm2;
    pm2.Start(kTestPMConfigFile);
    for (list<MockPN>::iterator pn_it = pn_list_.begin();
         pn_it != pn_list_.end(); ++pn_it) {
        pn_it->ReportStatus();
    }
    WaitForPMEventOver(pm2);
    app_list.clear();
    ASSERT_EQ(error::OK, meta_->GetApplicationList(&app_list));
    CheckApp(app_list.begin()->id());
    pe_list.clear();
    meta_->GetAppProcessElements(app_list.begin()->id(), &pe_list);
    ASSERT_NE(pe_list.size(), static_cast<unsigned int>(pe_size));
}

void TestClientOp::TestFlowControl()
{
    Application submit_app = SubmitApp();
    WaitForPMEventOver(pm_);
    ASSERT_LT(0, GetPNStatusSize());
    Application current_app;

    const Processor& importer = submit_app.topology().processors(0);
    // run
    FlowControlApp(submit_app.id(), FlowControlRequest_Operation_SET, importer.name(), INT32_MAX);
    WaitForPMEventOver(pm_);
    ASSERT_EQ(error::OK, meta_->GetApplication(submit_app.id(), &current_app));
    ASSERT_EQ(importer.name(), current_app.topology().processors(0).name());
    std::cout << "importer name: " << importer.name() << std::endl;
    std::cout << "got importer name: " << current_app.topology().processors(0).name() << std::endl;
    std::cout << "QPS limit: " << current_app.topology().processors(0).flow_control().qps_limit() << std::endl;
    ASSERT_TRUE(current_app.topology().processors(0).flow_control().qps_limit() == INT32_MAX);

    // stop
    FlowControlApp(submit_app.id(), FlowControlRequest_Operation_SET, importer.name(), 0);
    WaitForPMEventOver(pm_);
    ASSERT_EQ(error::OK, meta_->GetApplication(submit_app.id(), &current_app));
    std::cout << "QPS limit: " << current_app.topology().processors(0).flow_control().qps_limit() << std::endl;
    ASSERT_TRUE(current_app.topology().processors(0).flow_control().qps_limit() == 0);

    KillApp(submit_app.id(), meta_);
    WaitForPMEventOver(pm_);
    ASSERT_NE(error::OK, meta_->GetApplication(submit_app.id(), &submit_app));
    ASSERT_EQ(0, GetPNStatusSize());
}
void TestClientOp::TestBlackListOperation() {
    Application submit_app = SubmitApp();
    WaitForPMEventOver(pm_);
    ASSERT_LT(0, GetPNStatusSize());
    PE_LIST pe_list;
    PNIDSet pn_black_list;
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(submit_app.id(), &pe_list));
    scheduler::Scheduler* scheduler = pm_.scheduler();
    PE_LIST::iterator pe_it = pe_list.begin();
    list<MockPN>::iterator pn_it = pn_list_.begin();
    BlackListOperationRequest get_request;
    BlackListOperationRequest request;
    BlackListOperationResponse response;
    int cnt = 0;
    for(; pe_it != pe_list.end() && pn_it != pn_list_.end(); ++pn_it, ++pe_it) {
        for (int i = 0; i < 3; ++i) {
            scheduler->AddFailPE(pe_it->pe_id(), pn_it->pn_id());
            usleep(1000);
        }
        scheduler->GetPEBlackList(pe_it->pe_id(), &pn_black_list);
        ASSERT_EQ(pn_black_list.size(), 1u);
        ++cnt;
        get_request.add_pe_id()->set_id(pe_it->pe_id().id());
        pn_black_list.clear();
    }
 
    get_request.set_operation(BlackListOperationRequest::GET);
    BlackListOperation(&get_request, &response);
    ASSERT_EQ(cnt, response.pe_pn_blacklist_size());
    pn_it = pn_list_.begin();
    for (int i = 0; i < response.pe_pn_blacklist_size(); ++i) {
        ASSERT_EQ(response.pe_pn_blacklist(i).pn_id_size(), 1);
        ASSERT_STREQ(response.pe_pn_blacklist(i).pn_id(0).id().c_str(), pn_it->pn_id().id().c_str());
        ASSERT_TRUE(pn_it++ != pn_list_.end());
    }
    
    response.clear_pe_pn_blacklist();
    request.set_operation(BlackListOperationRequest::ERASE);
    request.clear_pe_id();
    request.add_pe_id()->set_id(pe_list.begin()->pe_id().id());
    BlackListOperation(&request, &response);
    scheduler->GetPEBlackList(pe_list.begin()->pe_id(), &pn_black_list);
    ASSERT_TRUE(pn_black_list.empty());
    pn_black_list.clear();
    for(int i = 0; i < 3; ++i) {
        scheduler->AddFailPE(pe_it->pe_id(), pn_list_.begin()->pn_id());
        usleep(1000);
    }

    request.clear_pe_id();
    request.set_operation(BlackListOperationRequest::CLEAR);
    BlackListOperation(&request, &response);

    pe_it = pe_list.begin();
    pn_it = pn_list_.begin();
    for(; pe_it != pe_list.end() && pn_it != pn_list_.end(); ++pn_it, ++pe_it) {

        scheduler->GetPEBlackList(pe_it->pe_id(), &pn_black_list);
        ASSERT_TRUE(pn_black_list.empty());
        pn_black_list.clear();
    }

    
}

TEST_F(TestClientOp, TestLogin) {
    TestLogin();
}

TEST_F(TestClientOp, TestSubmitApp)
{
    TestSubmitApp(1);
}

TEST_F(TestClientOp, TestPMRestartSubmit)
{
    TestPMRestartSubmit();
}

TEST_F(TestClientOp, TestKillApp)
{
    TestKillApp();
}

TEST_F(TestClientOp, TestAddSubOK)
{
    UpdateSubTree(UpdateTopology_Operation_ADD);
}

TEST_F(TestClientOp, TestAddSubFail)
{
    UpdateSubTree(UpdateTopology_Operation_ADD, false);
}

TEST_F(TestClientOp, TestAddParaOK)
{
    TestUpdatePara(ADD, true);
}

TEST_F(TestClientOp, TestReduceParaOK)
{
    TestUpdatePara(REDUCE, true);
}

TEST_F(TestClientOp, TestUpdateParaFail)
{
    TestUpdatePara(ADD, false);
}

TEST_F(TestClientOp, TestDelSubOK)
{
    UpdateSubTree(UpdateTopology_Operation_DEL);
}

TEST_F(TestClientOp, TestDelSubFail)
{
    UpdateSubTree(UpdateTopology_Operation_DEL, false);
}

TEST_F(TestClientOp, TestMultiUpdateTopology)
{
    TestMultiUpdateTopology();
}

TEST_F(TestClientOp, TestPMRestart_ADDSUB)
{
    TestPMRestart(UpdateTopology_Operation_ADD, ADDTOPO);
}

TEST_F(TestClientOp, TestUpdateTopoClientFail_ADD_PM_RESTART)
{
    TestUTCFPMRestart(UpdateTopology_Operation_ADD, ADDTOPO);
}

TEST_F(TestClientOp, TestUpdateTopoClientFail_DEL_PM_RESTART)
{
    TestUTCFPMRestart(UpdateTopology_Operation_DEL, DELTOPO);
}

TEST_F(TestClientOp, TestFlowControl)
{
    TestFlowControl();
}
TEST_F(TestClientOp, TestBlackListOperation) {
    TestBlackListOperation();
}

}
} //namespace dstream
