/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/08/05
* @filename:    test_pm_stop.cc
* @author:      zhanggengxin@baidu.com
* @brief:       Test for pm start/stop
*/

#include "gtest/gtest.h"
#include "processmaster/process_master.h"

namespace dstream {
namespace test_pm_stop {

using namespace process_master;
using namespace rpc;
using namespace config_const;
using namespace pm_rpc_server;
using namespace pm_gc;
using namespace scheduler;


/************************************************************************/
/* Test PM start/stop and delete pm node                                */
/************************************************************************/
class TestPMStop : public ::testing::Test
{
protected:
    void SetUp() {
        Connection::kserverPollTimeoutMillSeconds = 10;
    }
    void TestMasterPMStop();
    void TestBackupPMStop();
    void TestGCStop();
};

//Test Master pm stop
void TestPMStop::TestMasterPMStop() {
    ProcessMaster pm_;
    ASSERT_TRUE(pm_.Start(kTestPMConfigFile));
    ASSERT_TRUE(pm_.working_master());
    ASSERT_TRUE(pm_.working());
    pm_.Stop();
    sleep(1);
    ASSERT_FALSE(pm_.working());
    ASSERT_FALSE(pm_.working_master());
}

//Test BackupPM stop
void TestPMStop::TestBackupPMStop() {
    ProcessMaster pm_, backup_pm_;
    pm_.Start(kTestPMConfigFile);
    usleep(100 * 1000);
    backup_pm_.Start(kTestPMConfigFile);
    ASSERT_TRUE(backup_pm_.working());
    ASSERT_FALSE(backup_pm_.working_master());
    backup_pm_.Stop();
    ASSERT_FALSE(backup_pm_.working());
}

//Test GC stop
void TestPMStop::TestGCStop() {
    ProcessMaster pm_;
    pm_.Start(kTestPMConfigFile);
    GC* gc = pm_.gc();
    ASSERT_TRUE(gc->working());
    gc->Stop();
    ASSERT_FALSE(gc->working());
}

TEST_F(TestPMStop, TestMasterPMStop)
{
    TestMasterPMStop();
}

TEST_F(TestPMStop, TestBackupPMStop)
{
    TestBackupPMStop();
}

TEST_F(TestPMStop, TestGCStop)
{
    TestGCStop();
}

} //namespace test_pm_start
} //namespace dstream
