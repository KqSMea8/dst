/************************************************************************/
/* Test Black list                                                      */
/************************************************************************/

#include "gtest/gtest.h"

#include "processmaster/process_master.h"
#include "processmaster/test/client_op.h"
#include "processmaster/test/pm_test_common.h"
#include "scheduler/black_list.h"

namespace dstream {
namespace test_blacklist {

using namespace auto_ptr;
using namespace config_const;
using namespace process_master;
using namespace pm_test_common;
using namespace event_manager;
using namespace pm_rpc_server;
using namespace scheduler;
using namespace client_op;

//Test Black list Class
class TestBlackList : public PMTestBase {
protected:
    void SetUp();

    //Test Add Fail PE and GetBlack List
    void TestPEBlackList();

    //Test black list when schedule application
    void TestAppBlackList();

protected:
    void TestAppBlackListInstance();
    AutoPtr<Config> config_;
};

//Init MetaManager
//modify cluster config, save old config
void TestBlackList::SetUp() {
    PMTestBase::SetUp();
    config_ = AutoPtr<Config>(new Config);
    config_->ReadConfig(kTestPMConfigFile);
    ClusterConfig cluster_config;
    meta_->GetClusterConfig(&cluster_config);
    cluster_config.set_pefail_reserve_time(1);
    meta_->UpdateClusterConfig(cluster_config);
    config_->SetValue(kGCIntervalName, "500");
}

//Test Add Fail PE and GetBlack List
void TestBlackList::TestPEBlackList() {
    //Init context
    ProcessMaster pm;
    BlackList black_list;
    pm.Start(config_);
    pm.gc()->Stop();
    EventManager* event_manager = pm.event_manager();

    //submit app
    Application app = SubmitApp();
    usleep(100 * 1000);
    //stop event manager for there is no mock pn
    event_manager->set_wait_time(4000);

    //check pe black list
    PE_LIST pe_list;
    meta_->GetAppProcessElements(app.id(), &pe_list);
    PE_LIST::iterator pe_it = pe_list.begin();
    PNID pn_ids[3];
    char pn_id_name[158];
    PNIDSet pn_black_list;
    for (int i = 0; i < 3; ++i) {
        sprintf(pn_id_name, "test_blacklistpn_%d", i);
        pn_ids[i].set_id(pn_id_name);
    }
    for (int i = 0; i < 3 && pe_it != pe_list.end(); ++i, ++pe_it) {
        black_list.AddFailPE(pe_it->pe_id(), pn_ids[i]);
        black_list.GetBlackList(pe_it->pe_id(), pn_black_list);
        ASSERT_TRUE(pn_black_list.empty());
        for (int j = 0; j < 2; ++j) {
            usleep(1000);
            black_list.AddFailPE(pe_it->pe_id(), pn_ids[i]);
        }
        black_list.GetBlackList(pe_it->pe_id(), pn_black_list);
        ASSERT_EQ(pn_black_list.size(), 1u);
        pn_black_list.clear();
    }
    for (int i =  0; i < 3; ++i) {
        black_list.AddFailPE(pe_list.begin()->pe_id(), pn_ids[1]);
        usleep(1000);
    }
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_EQ(pn_black_list.size(), 2u);
    black_list.ErasePEBlackList(pe_list.begin()->pe_id());
    pn_black_list.clear();
    pe_it = pe_list.begin();
    for(int i = 0; i < 3 && pe_it != pe_list.end();++i, ++pe_it) {
        black_list.GetBlackList(pe_it->pe_id(), pn_black_list);
        if(pe_it != pe_list.begin()) {
            ASSERT_EQ(pn_black_list.size(), 1u);
        } else {    
            ASSERT_TRUE(pn_black_list.empty());
        }
        pn_black_list.clear();
    }
    black_list.ClearBlackList();
    for (pe_it = pe_list.begin(); pe_it != pe_list.end(); ++pe_it) { 
        black_list.GetBlackList(pe_it->pe_id(), pn_black_list);
        ASSERT_TRUE(pn_black_list.empty());
        pn_black_list.clear();
    }

    for (int i =  0; i < 3; ++i) {
        black_list.AddFailPE(pe_list.begin()->pe_id(), pn_ids[0]);
        black_list.AddFailPE(pe_list.begin()->pe_id(), pn_ids[1]);
        usleep(1000);
    }
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_EQ(pn_black_list.size(), 2u);
    PN pn;
    *(pn.mutable_pn_id()) = pn_ids[0];
    meta_->AddPN(pn);
    pn_black_list.clear();
    black_list.UpdateBlackList();
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_EQ(pn_black_list.size(), 1u);

    //test update black list
    sleep(1);
    black_list.UpdateBlackList();
    pn_black_list.clear();
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_TRUE(pn_black_list.empty());

    //check for add pn black list
    black_list.AddFailPN(pn.pn_id());
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_TRUE(pn_black_list.size() == 1);
    black_list.AddFailPN(pn_ids[1]);
    pn_black_list.clear();
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_TRUE(pn_black_list.size() == 2);
    pn_black_list.clear();
    black_list.UpdateBlackList();
    black_list.GetBlackList(pe_list.begin()->pe_id(), pn_black_list);
    ASSERT_TRUE(pn_black_list.size() == 1);
}

//Check pe number on pn
static void CheckPENumOnPN(const char* function, int line,
                           MockPN* pn, int pn_num, int size) {
    for (int j = 0; j < 10; ++j) {
        bool check_success = true;
        for (int i = 0; i < pn_num; ++i) {
            int pn_pe_num = pn[i].status().size();
            if (pn_pe_num != size) {
                DSTREAM_WARN("[%d:%s] check pe size on pn [%d] fail "
                             "expect:[%d] actual :[%d]", line, function,
                             i, size, pn_pe_num);
                check_success = false;
                break;
            }
        }
        if (check_success) {
            return;
        } else {
            sleep(1);
        }
    }
    ASSERT_TRUE(false);
}

//Test black list when schedule application
void TestBlackList::TestAppBlackList() {
    DSTREAM_INFO("[%s] Test for pn number scheduler", __FUNCTION__);
    TestAppBlackListInstance();
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    DSTREAM_INFO("[%s] Test for memory resource scheduler", __FUNCTION__);
    ClearApp();
    TestAppBlackListInstance();
    DSTREAM_INFO("[%s] TestAppBlackList over", __FUNCTION__);
}

void TestBlackList::TestAppBlackListInstance() {
    //Init Test Context
    ProcessMaster pm;
    pm.Start(config_);
    pm.gc()->Stop();
    pm.event_manager()->set_wait_time(200);
    ClusterConfig cluster_config;
    meta_->GetClusterConfig(&cluster_config);
    cluster_config.set_max_pe_num_per_pn(8);
    cluster_config.set_pefail_reserve_time(2);
    meta_->UpdateClusterConfig(cluster_config);

    MockPN mock_pns[3];
    for (int i = 0; i < 3; ++i) {
        mock_pns[i].StartPN();
        mock_pns[i].ReportStatus();
    }
    Application app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 2);

    //test for pe black list
    mock_pns[2].set_rpc_success(false);
    Application test_black_app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, &mock_pns[2], 1, 2);
    CheckPENumOnPN(__FUNCTION__, __LINE__, &mock_pns[0], 2, 5);
    mock_pns[2].set_rpc_success(true);
    Application app2 = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 6);

    //Test for update pe black list
    mock_pns[0].set_rpc_success(false);
    app = SubmitApp();
    sleep(1);
    CheckPENumOnPN(__FUNCTION__, __LINE__, &mock_pns[1], 2, 8);
    CheckPENumOnPN(__FUNCTION__, __LINE__, &mock_pns[0], 1, 6);
    cluster_config.set_pefail_reserve_time(1);
    meta_->UpdateClusterConfig(cluster_config);
    mock_pns[0].set_rpc_success(true);
    usleep(1100 * 1000);
    CheckPENumOnPN(__FUNCTION__, __LINE__, &mock_pns[0], 1, 8);
    cluster_config.set_pefail_reserve_time(10);
    meta_->UpdateClusterConfig(cluster_config);

    //Test for special app fail on pn
    KillApp(app.id(), meta_);
    KillApp(test_black_app.id(), meta_);
    AppID fail_app_id;
    fail_app_id.set_id(app.id().id() + 1);
    mock_pns[2].SetFailAppID(fail_app_id);
    app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 6);
    app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 8);

    //stop test pm
    pm.StopForTest();
}

//Test PN Blacklist
TEST_F(TestBlackList, TestPEBlackList)
{
    TestPEBlackList();
}

TEST_F(TestBlackList, TestAppBlackList)
{
    TestAppBlackList();
}

} //namespace test_blacklist
} //namespace dstream
