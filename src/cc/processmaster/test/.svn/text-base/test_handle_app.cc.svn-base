/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/04/07
* @filename:    test_handle_app.h
* @author:      zhanggengxin@baidu.com
* @brief:       test handle app
*/

#include "gtest/gtest.h"
#include "processmaster/pn_client_status.h"
#include "processmaster/mock_pn.h"
#include "client_op.h"
#include "pm_test_common.h"

static const int kMockPNNum = 4;

namespace dstream {
namespace test_handle_app {

using namespace client_op;
using namespace mock_pn;
using namespace pm_test_common;
using namespace pn_client_status;
using namespace config_const;

/************************************************************************/
/* Test PM                                                              */
/************************************************************************/
class TestHandleApp : public PMTestBase
{
protected:
    void TestClientFail();
    void TestHandleNewApp();
    void TestGC();
    void TestPMPNDown();
    void TestPNReport();
    void TestRestart();
    void SetUp();
    void TearDown() {
        pm_.StopForTest();
    }
private:
    int PNStatusSize();
    ProcessMaster pm_;
    MockPN pn_list_[kMockPNNum];
};

void TestHandleApp::SetUp() {
    PMTestBase::SetUp();
    pm_.Start(kTestPMConfigFile);
    for (int i = 0; i < kMockPNNum; ++i) {
        pn_list_[i].StartPN();
        pn_list_[i].ReportStatus();
    }
}

int TestHandleApp::PNStatusSize() {
    int status_size = 0;
    for (int i = 0; i < kMockPNNum; ++i) {
        status_size += pn_list_[i].status().size();
    }
    return status_size;
}

void TestHandleApp::TestClientFail() {
    Application app = SubmitApp(false);
    client_info ci;
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    ASSERT_EQ(error::OK, status->FindClient(app.id().session_id(), &ci));
    meta_->zk_client()->DeleteEphemeralNode(ci.user.hostname().c_str());
    usleep(200 * 1000);
    pm_.gc()->SignalGCSleep();
    usleep(200 * 1000);
    ASSERT_NE(error::OK, meta_->GetApplication(app.id(), &app));
}

void TestHandleApp::TestHandleNewApp() {
    Application app = SubmitApp();
    WaitForPMEventOver(pm_);
    CheckApp(app.id());
    for (int i = 0; i < kMockPNNum; ++i) {
        ASSERT_GT(pn_list_[i].status().size(), 0u);
    }
    PE_LIST pe_list;
    ASSERT_EQ(error::OK, meta_->GetAppProcessElements(app.id(), &pe_list));
    ASSERT_EQ(pe_list.size(), static_cast<unsigned int>(GetAppPENum(app)));
    ASSERT_EQ(static_cast<unsigned int>(PNStatusSize()), pe_list.size());
    app = SubmitApp();
    WaitForPMEventOver(pm_);
    CheckApp(app.id());
    pm_.StopForTest();
}

void TestHandleApp::TestGC() {
    Application app = SubmitApp();
    WaitForPMEventOver(pm_);

    //check for kill extra pe
    int status_size = PNStatusSize();
    StatusSet::iterator pn_pe_status = pn_list_[0].status().begin();
    pn_list_[1].AddBackupPE(app.id(), pn_pe_status->m_pe_id,
                            pn_pe_status->m_backup_pe_id);
    ASSERT_EQ(status_size + 1, PNStatusSize());
    pn_list_[1].ReportStatus();
    pm_.gc()->SignalGCSleep();
    //sleep(1);
    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    WaitForPMEventOver(pm_);
    ASSERT_EQ(status_size, PNStatusSize());
}

/************************************************************************/
/* submit app first and then pm down ,before pm start pn also down      */
/************************************************************************/
void TestHandleApp::TestPMPNDown()
{
    Application submit_app = SubmitApp();
    Application sumit_app2 = SubmitApp();
    WaitForPMEventOver(pm_);
    pm_.Stop();
    //modify submit_app2
    list<ProcessorElement> pe_list;
    meta_->GetAppProcessElements(sumit_app2.id(), &pe_list);
    list<ProcessorElement>::iterator pe_it, pe_end;
    for (pe_it = pe_list.begin(), pe_end = pe_list.end();
         pe_it != pe_end; ++pe_it) {
        pe_it->clear_primary();
        pe_it->mutable_backups(0)->clear_pn_id();
    }
    int status_size = PNStatusSize();
    MockPN& down_pn = pn_list_[0];
    int down_pn_status_size = down_pn.status().size();
    ASSERT_GT(down_pn_status_size, 0);
    down_pn.KillPN();
    PN pn;
    ASSERT_NE(error::OK, meta_->GetPN(down_pn.pn_id(), &pn));
    ASSERT_EQ(status_size - down_pn_status_size, PNStatusSize());
    ProcessMaster new_pm;
    new_pm.Start(kTestPMConfigFile);
    for (int i = 1; i < kMockPNNum; ++i) {
        pn_list_[i].ReportStatus();
    }
    WaitForPMEventOver(new_pm);
    CheckApp(submit_app.id());
    CheckApp(sumit_app2.id());
    ASSERT_EQ(status_size, PNStatusSize());
}

void TestHandleApp::TestPNReport() {
    Application app = SubmitApp();
    WaitForPMEventOver(pm_);
    int status_size = PNStatusSize();
    DSTREAM_INFO("status size %d", status_size);
    StatusSet::iterator pn_pe_status;
    for (int i = 0; i < kMockPNNum; ++i) {
        if (pn_list_[i].status().size() == 0) {
            continue;
        }
        pn_pe_status = pn_list_[i].status().begin();
        pn_list_[i].KillPE(app.id(), pn_pe_status->m_pe_id,
                           pn_pe_status->m_backup_pe_id);
    }
    ASSERT_EQ(status_size - kMockPNNum, PNStatusSize());
    for (int i = 0; i < kMockPNNum; ++i) {
        pn_list_[i].ReportStatus();
    }
    WaitForPMEventOver(pm_);
    CheckApp(app.id());
    ASSERT_EQ(status_size, PNStatusSize());

    //check for pm restart check pn's assign
    pm_.Stop();
    pn_pe_status = pn_list_[2].status().begin();
    pn_list_[2].KillPE(app.id(), pn_pe_status->m_pe_id,
                       pn_pe_status->m_backup_pe_id, false);
    pn_pe_status = pn_list_[3].status().begin();
    pn_list_[3].KillPE(app.id(), pn_pe_status->m_pe_id,
                       pn_pe_status->m_backup_pe_id);
    ASSERT_EQ(status_size - 2 , PNStatusSize());
    ProcessMaster pm2;
    pm2.Start(kTestPMConfigFile);
    for (int i = 0; i < kMockPNNum; ++i) {
        pn_list_[i].ReportStatus();
    }
    WaitForPMEventOver(pm2);
    ASSERT_EQ(status_size, PNStatusSize());
    CheckApp(app.id());
}

void TestHandleApp::TestRestart() {
    //check pm assign unsigned app
    pm_.event_manager()->Suspend();
    Application app = SubmitApp();
    ASSERT_EQ(0, PNStatusSize());
    pm_.Stop();
    ProcessMaster pm1;
    pm1.Start(kTestPMConfigFile);
    for (int i = 0; i < kMockPNNum; ++i) {
        pn_list_[i].ReportStatus();
    }
    WaitForPMEventOver(pm1);
    CheckApp(app.id());
    ASSERT_EQ(GetAppPENum(app), PNStatusSize());
    pm1.Stop();

    //kill submitting apps
    app.set_status(SUBMIT);
    ASSERT_EQ(error::OK, meta_->UpdateApplication(app));
    ProcessMaster pm2;
    pm2.Start(kTestPMConfigFile);
    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    ASSERT_NE(error::OK, meta_->GetApplication(app.id(), &app));
    for (int i = 0; i < kMockPNNum; ++i) {
        pn_list_[i].ReportStatus();
    }
    WaitForPMEventOver(pm2);
    ASSERT_EQ(0, PNStatusSize());
}

TEST_F(TestHandleApp, TestClientFail)
{
    TestClientFail();
}

TEST_F(TestHandleApp, TestHandleNewApp)
{
    TestHandleNewApp();
}

TEST_F(TestHandleApp, TestGC)
{
    TestGC();
}

TEST_F(TestHandleApp, TestPMPNDown)
{
    TestPMPNDown();
}

TEST_F(TestHandleApp, TestPNReport)
{
    TestPNReport();
}

TEST_F(TestHandleApp, TestRestart)
{
    TestRestart();
}

} //namespace test_handle_app
} //namespace dstream
