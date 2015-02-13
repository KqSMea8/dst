/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/07/25
* @filename:    test_app_scheduler.h
* @author:      zhanggengxin@baidu.com
* @brief:       Test App Scheduler
*/

#include <stdlib.h>

#include "gtest/gtest.h"
#include "processmaster/pm_common.h"
#include "processmaster/pn_client_status.h"
#include "processmaster/mock_pn.h"

#include "processmaster/test/client_op.h"
#include "processmaster/test/pm_test_common.h"

static const int kMockPNNum = 4;

namespace dstream {
namespace test_app_scheduler {
using namespace client_op;
using namespace config_const;
using namespace auto_ptr;
using namespace mock_pn;
using namespace pm_test_common;
using namespace pn_client_status;
using namespace pm_common;
using namespace scheduler;

/************************************************************************/
/* Test App Scheduler                                                   */
/************************************************************************/
class TestAppScheduler : public PMTestBase
{
protected:
    void SetUp();

    //Test for schedule multi application
    void TestMultiApp();

    //Test for schedule add/del subtree
    void TestAddDelSubTree();

    //Test if scheduler can work
    void TestSchedulerWork();

    //Test Max PE number on pn
    void TestMaxPEOnPN();

    //Test press scheduler
    void TestPress();
private:
    void TestAddDelSub(bool resource_scheduler);
    void TestMultiApp(bool b_memory_cmp);
    //Test if scheduler can work
    void TestSchedulerWork(bool b_mem_comp);
    //Test Max PE number on pn
    void TestMaxPEOnPN(bool b_mem_comp);
    void TestPress(bool b_mem_comp);

    AutoPtr<Config> config_;
};

void TestAppScheduler::SetUp() {
    PMTestBase::SetUp();
    config_ = AutoPtr<Config>(new Config);
    config_->ReadConfig(kTestPMConfigFile);
    config_->SetValue(kGCIntervalName, "10000");
}

//Check pe number on pn
static void CheckPENumOnPN(const char* function, int line,
                           MockPN* pn, int pn_num, int size) {
    for ( int j = 0; j < 10; ++j) {
        bool check_success = true;
        for (int i = 0; i < pn_num; ++i) {
            int pe_num = pn[i].status().size();
            DSTREAM_INFO("AAAAA pe num %d, pn num %d",
                         pe_num, pn_num);
            if (pe_num != size) {
                DSTREAM_WARN("[%d:%s] check pn size fail "
                             "expect pe number [%d] actual [%d]", line, function,
                             size, pe_num);
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

void TestAppScheduler::TestMultiApp() {
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    ClearApp();
    TestMultiApp(true);
}

//Test for schedule multi application
void TestAppScheduler::TestMultiApp(bool b_memory_cmp) {
    ProcessMaster pm;
    pm.Start(config_);
    pm.gc()->Stop();
    PNMachines pn_machines;
    meta_->UpdatePNMachines(pn_machines);

    MockPN mock_pns[3];
    for (int i = 0; i < 3; ++i) {
        mock_pns[i].set_mem_basic(2);
        mock_pns[i].set_cpu_basic(16);
        mock_pns[i].StartPN();
        mock_pns[i].ReportStatus();
    }
    Application app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckApp(app.id());
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 2);
    Application app2 = SubmitApp();
    if (b_memory_cmp) {
        CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 2);
        MockPN more_pns[3];
        for (int i = 0; i < 3; ++i) {
            more_pns[i].set_mem_basic(2);
            more_pns[i].set_cpu_basic(16);
            more_pns[i].StartPN();
            if (i < 2) {
                more_pns[i].ReportStatus();
            }
        }
        usleep(300 * 1000);
        CheckPENumOnPN(__FUNCTION__, __LINE__, more_pns, 3, 0);
        more_pns[2].ReportStatus();
        WaitForPMEventOver(pm);
        CheckPENumOnPN(__FUNCTION__, __LINE__, more_pns, 3, 2);
        CheckApp(app2.id());
    } else {
        WaitForPMEventOver(pm);
        CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 4);
        CheckApp(app2.id());
    }
    KillApp(app2.id(), meta_);
    app2 = SubmitApp();
    if (b_memory_cmp) {
        usleep(300 * 1000);
        CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 2);
        KillApp(app.id(), meta_);
        WaitForPMEventOver(pm);
        CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 3, 2);
    }
    pm.StopForTest();
}

//Test for schedule add/del subtree
void TestAppScheduler::TestAddDelSubTree() {
    TestAddDelSub(false);
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    ClearApp();
    TestAddDelSub(true);
}

void TestAppScheduler::TestAddDelSub(bool resource_scheduler) {
    //Init Test Contex
    ProcessMaster pm;
    pm.Start(config_);
    MockPN mock_pns[4];
    for (int i = 0; i < 4; ++i) {
        mock_pns[i].set_mem_basic(5);
        mock_pns[i].set_cpu_basic(16);
        mock_pns[i].StartPN();
        mock_pns[i].ReportStatus();
    }
    PNMachines pn_machines;
    meta_->UpdatePNMachines(pn_machines);

    //Test for deal with app status
    Application app = SubmitApp();
    WaitForPMEventOver(pm);
    app.set_status(ADDTOPO);
    meta_->UpdateApplication(app);
    Application subtree_app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 4, 3);
    KillApp(app.id(), meta_);

    //Test for Add subtree
    app = SubmitApp();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 4, 3);
    AddSubTreeTest(subtree_app);
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 4, 5);
    if (resource_scheduler) {
        //Test for resource limit
        app = SubmitApp(true, 1, 8);
        usleep(300 * 1000);
        CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 4, 5);
        KillApp(app.id(), meta_);
    }

    //Test for delete sub tree
    DelSubTreeTest(subtree_app);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pns, 4, 3);
    pm.StopForTest();
}

void TestAppScheduler::TestSchedulerWork() {
    TestSchedulerWork(false);
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    ClearApp();
    TestSchedulerWork(true);
}

//Test if scheudler can work
void TestAppScheduler::TestSchedulerWork(bool /*resource_scheduler*/) {
    //Init Context
    ProcessMaster pm;
    pm.Start(config_);
    pm.gc()->Stop();
    MockPN mock_pns[3];
    Scheduler* scheduler =  pm.scheduler();
    PNMachines pn_machines;
    meta_->UpdatePNMachines(pn_machines);

    //Test for pn report number
    ASSERT_FALSE(scheduler->SchedulerCanWork());
    for (int i = 0; i < 3; ++i) {
        mock_pns[i].set_mem_basic(2);
        mock_pns[i].set_cpu_basic(16);
        mock_pns[i].StartPN();
    }
    ASSERT_FALSE(scheduler->SchedulerCanWork());
    for (int i = 0; i < 3; ++i) {
        mock_pns[i].ReportStatus();
    }

    ClusterConfig cluster_config;
    meta_->GetClusterConfig(&cluster_config);
    cluster_config.set_min_host_percentage(0.7);
    cluster_config.set_check_pn_size(true);
    meta_->UpdateClusterConfig(cluster_config);
    scheduler->Update();
    meta_->GetPNMachines(&pn_machines);
    pn_machines.add_pn_list("add_new_machines1");
    meta_->UpdatePNMachines(pn_machines);
    ASSERT_TRUE(scheduler->SchedulerCanWork());
    pn_machines.add_pn_list("add_new_machines2");
    meta_->UpdatePNMachines(pn_machines);
    ASSERT_FALSE(scheduler->SchedulerCanWork());
    MockPN add_pn;
    add_pn.StartPN();
    add_pn.ReportStatus();
    ASSERT_FALSE(scheduler->SchedulerCanWork());
    pm.StopForTest();
}

void TestAppScheduler::TestMaxPEOnPN() {
    TestMaxPEOnPN(false);
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    ClearApp();
    TestMaxPEOnPN(true);
}

void TestAppScheduler::TestMaxPEOnPN(bool /*b_memory_cmp*/) {
    ProcessMaster pm;
    pm.Start(config_);
    pm.gc()->Stop();
    PNMachines pn_machines;
    meta_->UpdatePNMachines(pn_machines);

    MockPN mock_pn[2];
    mock_pn[0].StartPN();
    mock_pn[0].ReportStatus();

    Application app = SubmitApp(true, 2, 4);
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pn, 1, 12);
    Application app2 = SubmitApp(true, 2, 4);
    sleep(1);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pn, 1, 12);

    mock_pn[1].StartPN();
    mock_pn[1].ReportStatus();
    WaitForPMEventOver(pm);
    CheckPENumOnPN(__FUNCTION__, __LINE__, mock_pn, 2, 12);
    pm.StopForTest();
}

void TestAppScheduler::TestPress() {
    TestPress(false);
    config_->SetValue(kResourceCmpName, kMemoryCompare);
    ClearApp();
    TestPress(true);
}

void TestAppScheduler::TestPress(bool /*b_mem_comp*/) {
    ProcessMaster pm;
    pm.Start(config_);
    pm.gc()->Stop();
    PNMachines pn_machines;
    meta_->UpdatePNMachines(pn_machines);
    ClusterConfig cluster_config;
    meta_->GetClusterConfig(&cluster_config);
    cluster_config.set_max_pe_num_per_pn(20);
    meta_->UpdateClusterConfig(cluster_config);

    MockPN mock_pns[5];
    for (int i = 0; i < 5; ++i) {
        mock_pns[i].StartPN();
        mock_pns[i].ReportStatus();
    }

    srand(time(NULL));
    Application app[4];
    for (int i = 0; i < 4; ++i) {
        app[i] = SubmitApp();
    }
    WaitForPMEventOver(pm);
    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 3; ++j) {
            int index = rand() % 4;
            if (app[index].has_id()) {
                KillApp(app[index].id(), meta_);
                app[index] = SubmitApp(true, rand() % 2 + 2, rand() % 2 + 2);
            }
        }
        int add_index = rand() % 4;
        WaitForPMEventOver(pm);
        AddSubTreeTest(app[add_index]);
        WaitForPMEventOver(pm);
        int del_index = rand() % 4;
        DelSubTreeTest(app[del_index]);
        WaitForPMEventOver(pm);
    }
    pm.StopForTest();
}

TEST_F(TestAppScheduler, TestMultiApp)
{
    TestMultiApp();
}

TEST_F(TestAppScheduler, TestAddDelSubTree)
{
    TestAddDelSubTree();
}

TEST_F(TestAppScheduler, TestCanScheduler)
{
    TestSchedulerWork();
}

TEST_F(TestAppScheduler, TestMaxPEOnPN)
{
    TestMaxPEOnPN();
}

TEST_F(TestAppScheduler, TestPress)
{
    //  TestPress();
}

} //namespace test_app_scheduler
} //namespace dstream
