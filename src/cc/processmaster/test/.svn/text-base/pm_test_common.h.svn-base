/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/06
* @filename:    pm_test_common.h
* @author:      zhanggengxin@baidu.com
* @brief:       pm test common
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_PM_TEST_COMMON_H__
#define __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_PM_TEST_COMMON_H__

#include "common/proto/cluster_config.pb.h"
#include "gtest/gtest.h"
#include "metamanager/zk_meta_manager.h"
#include "processmaster/process_master.h"

namespace dstream {
namespace pm_test_common {
using namespace process_master;
using namespace zk_meta_manager;

//common functions used for pm test

//Get primay pm index form
int GetPrimaryPMIndex(ProcessMaster* pm, int pm_count);

//Get Application's pe number
int GetAppPENum(const Application& app);

//end of common function

/************************************************************************/
/* PM Test Base class                                                    */
/************************************************************************/
class PMTestBase : public ::testing::Test {
protected:
    void SetUp();
    void TearDown();
protected:
    ZKMetaManager* meta_;
    ClusterConfig old_cluster_config_;
private:
    void ClearContext();
};

} //namespace pm_test_common
} //namespace dstream

#endif // __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_PM_TEST_COMMON_H__
