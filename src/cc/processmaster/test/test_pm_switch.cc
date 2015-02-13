/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test primary and backup pm switch
*********************************************************************/

#include "common/rpc_client.h"
#include "common/rpc_const.h"
#include "processmaster/test/client_op.h"
#include "processmaster/test/pm_test_common.h"

namespace dstream {
namespace test_pm_switch {

using namespace config_const;
using namespace client_op;
using namespace meta_manager;
using namespace pm_test_common;
using namespace rpc_const;

/************************************************************************/
/* test for pm switch                                                   */
/************************************************************************/
class TestPMSwitch : public PMTestBase {
protected:
    void TestBackupPMMonitor();
    void TestPMDown();
};

/************************************************************************/
/* test for back up pm watch primary pm                                 */
/************************************************************************/
void TestPMSwitch::TestBackupPMMonitor() {
    std::string res;
    ProcessMaster pm[3];
    for (int i = 0; i < 3; ++i) {
        pm[i].Start(kTestPMConfigFile);
    }
    int master_index = GetPrimaryPMIndex(pm, 3);
    ASSERT_NE(master_index, -1);
    DSTREAM_INFO("[%s] client log in", __FUNCTION__);
    LogIn(&res, NULL, meta_);
    ASSERT_STREQ(kOK, res.c_str());
    pm[master_index].Stop();
    sleep(1);
    int new_master_index = GetPrimaryPMIndex(pm, 3);
    ASSERT_NE(new_master_index, -1);
    ASSERT_NE(new_master_index, master_index);
    DSTREAM_INFO("[%s] client log in", __FUNCTION__);
    LogIn(&res, NULL, meta_);
    ASSERT_STREQ(kOK, res.c_str());
    meta_->DeletePM();
    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    ASSERT_NE(-1, GetPrimaryPMIndex(pm, 3));
    DSTREAM_INFO("[%s] client log in", __FUNCTION__);
    LogIn(&res, NULL, meta_);
    ASSERT_STREQ(kOK, res.c_str());
}

/************************************************************************/
/* test for primary master down                                         */
/************************************************************************/
void TestPMSwitch::TestPMDown() {
    ProcessMaster pm;
    std::string res;
    pm.Start(kTestPMConfigFile);
    DSTREAM_INFO("first [%s] client log in", __FUNCTION__);
    LogIn(&res, NULL, meta_);
    ASSERT_STREQ(kOK, res.c_str());
    meta_->DeletePM();
    //NOTICE
    //we have to put sleep in a while, because kill pm will cause signal interupt
    while (1) {
        int ret = sleep(1);
        if (ret == 0) {
            break;
        }
    }
    DSTREAM_INFO("second [%s] client log in", __FUNCTION__);
    LogIn(&res, NULL, meta_);
    ASSERT_STREQ(kOK, res.c_str());
}

TEST_F(TestPMSwitch, test_pm_down) {
    TestPMDown();
}

TEST_F(TestPMSwitch, test_backup_pm_mon)
{
    TestBackupPMMonitor();
}

} //namespace test_pm_switch
} //namespace dstream
