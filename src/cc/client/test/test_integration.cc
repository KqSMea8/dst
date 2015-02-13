/**
* @file     test_application.cpp
* @brief    Dstream Client unit cases
* @author   konghui, konghui@baidu.com
* @version  1.0.0.0
* @date     2012-04-05
* @notice   most test cases in this file need an external hdfs and zookeeper,
*           their configs are configured in the cluster_config.xml
*           and dstrea.cfg.xml.
*           Also those cases interactive with hdfs need a hdfs client and should
*           be configured proper.
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <gtest/gtest.h>

#include <client/action.h>
#include <client/client_config.h>
#include <client/dstream_client.h>
#include <client/application.h>
#include <client/zk.h>
#include <metamanager/meta_manager.h>

#include "mock_zk.h"
#include "mock_hdfs_client.h"

using namespace dstream;
using namespace dstream::client;

int32_t test_InitAndLogin() {
    /* read config file */
    int32_t ret = error::OK;
    ret = g_cfg_map.InitCfgMapFromFile(g_cmd_line_args.cfg_file);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to init config map from file[%s], ErrStr[%s]",
                     g_cmd_line_args.cfg_file.c_str(), error::get_text(ret));
        return ret;
    }
    /* for debug */
    g_cfg_map.Dump();
    /* init hdfs client */
    g_pc.Init();
    /* register into zk */
    g_zk = new ZK(g_cfg_map.GetValue(config_const::kZKPathName));
    ret = g_zk->Register(g_user);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to register into zookeeper, ErrStr[%s]",
                     error::get_text(ret));
        return ret;
    }
    /* login into pm */
    ret = g_pc.LoginPM(g_cfg_map.GetValue(config_const::kZKPathName), g_user);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to login to pm , ErrStr[%s]", error::get_text(ret));
        return ret;
    }
    return error::OK;
}

void test_ResetGlobalVars() {
    /* reset app_id, may affect other test cases if not */
    g_app_id = -1;
    g_user.Clear();
    //reset g_cmd_line_args;
}

void test_Logout() {
    /* unregiste on zookeeper */
    g_pc.Reset();
    delete g_zk;
    g_zk = NULL;
}

void test_ResetClient() {
    test_Logout();
    test_ResetGlobalVars();
}

/* decommission app */
int32_t test_DecommissionApp(const std::string app_name) {
    g_cmd_line_args.app_name = app_name;
    int32_t ret = error::OK;
    ret = DecommissionAppAct();
    if (ret < error::OK) {
        DSTREAM_WARN("fail to decommission app[%s]", app_name.c_str());
        return ret;
    }
    /* delete meta cause meta data may still in the zookeeper
     * that would stop the new app submit */
    AppID app_id;
    app_id.set_id(g_app_id);
    meta_manager::MetaManager::InitMetaManagerContext(&g_cfg_map);
    meta_manager::MetaManager* meta = meta_manager::MetaManager::GetMetaManager();
    meta->DeleteApplication(app_id);
    meta->DestoryMetaManager();
    return error::OK;
}

int main(int argc, char** argv) {
    /* init log */
    zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
    logger::initialize(argv[0]);

    /* update zookeeper config */
    system("./update_cluster cluster_config.xml");
    system("./dstream_client_unit_test_pm dstream.cfg.xml >pm.log &");

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();

    system("killall -9 dstream_client_unit_test_pm");
    system("rm -f pm.log");
    meta_manager::MetaManager::InitMetaManagerContext(&g_cfg_map);
    meta_manager::MetaManager* meta = meta_manager::MetaManager::GetMetaManager();
    meta->DeletePM();
    meta->DestoryMetaManager();

    return ret;
}

/**
 * @brief   init and login test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 */
class test_InitAndLogin_suite : public ::testing::Test {
protected:
    test_InitAndLogin_suite() {};
    virtual ~test_InitAndLogin_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
        test_ResetClient();
    };
public:
};

/**
 * @brief   test init and login function
 * @author  fangjun, fangjun02@baidu.com
 * date     2012-08-15
 * @notice  init args: g_cmd_line_args.cfg_file
 */
TEST_F(test_InitAndLogin_suite, InCorrectInitCfg) {
    /* read config file */
    g_cmd_line_args.cfg_file = "./dstream_fake.cfg.xml";
    int32_t ret = error::OK;
    ret = g_cfg_map.InitCfgMapFromFile(g_cmd_line_args.cfg_file);
    ASSERT_NE(ret, error::OK);
}

/**
 * @brief   test init and login function
 * @author  fangjun, fangjun02@baidu.com
 * date     2012-08-15
 * @notice  init args: g_cmd_line_args.cfg_file
 */
TEST_F(test_InitAndLogin_suite, InCorrectLogin) {
    /* read config file */
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    int32_t ret = error::OK;
    ret = g_cfg_map.InitCfgMapFromFile(g_cmd_line_args.cfg_file);
    ASSERT_EQ(ret, error::OK);
    /* for debug */
    g_cfg_map.Dump();
    /* register into zk */
    g_zk = new ZK(g_cfg_map.GetValue(config_const::kZKPathName));
    ret = g_zk->Register(g_user);
    ASSERT_EQ(ret, error::OK);
    /* login into pm */
    g_user.set_username("fake_user");
    ret = g_pc.LoginPM(g_cfg_map.GetValue(config_const::kZKPathName), g_user);
    ASSERT_NE(ret, error::OK);
}
/**
* @brief    topology check test suite
* @author   konghui, konghui@baidu.com
* @date   2012-04-05
*/
class test_CheckTopology_suite : public ::testing::Test {
protected:
    test_CheckTopology_suite() {};
    virtual ~test_CheckTopology_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CheckTopology_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CheckTopology_suite, *)
    };

public:
};

/**
* @brief    test a correct complete topology
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, CorrectCompleteTopology) {
    g_cmd_line_args.app_dir = "./topo_check_test_correct_complicated_topo";
    ASSERT_EQ(CheckTopologyAct(), error::OK);
}

/**
* @brief    test a correct complete topology with mutil-tags
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-10
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, CorrectTopologyWithMultiTags) {
    g_cmd_line_args.app_dir = "./topo_check_test_correct_topo_with_multi_tags";
    ASSERT_EQ(CheckTopologyAct(), error::OK);
}

/**
* @brief    test a incorrect topology
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, IncorrectCompleteTopology) {
    g_cmd_line_args.app_dir = "./topo_check_test_incorrect_topo";
    ASSERT_NE(CheckTopologyAct(), error::OK);
}

/**
* @brief    test a incorrect topology with circles
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, CompleteTopologyWithCircles) {
    g_cmd_line_args.app_dir = "./topo_check_test_incorrect_topo_with_circles";
    ASSERT_NE(CheckTopologyAct(), error::OK);
}

/**
* @brief    test a incorrect topology with processor which subscribes none upstreams
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, CompleteTopologyWithoutSubscribes) {
    g_cmd_line_args.app_dir = "./topo_check_test_incorrect_topo_without_subscribes";
    ASSERT_NE(CheckTopologyAct(), error::OK);
}

/**
* @brief    test a correct sub-tree topology
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: g_cmd_line_args.app_dir
*/
TEST_F(test_CheckTopology_suite, CorrectSubTreeTopology) {
    g_cmd_line_args.app_dir = "./topo_check_test_correct_add_sub_tree";

    Application sub_tree;
    ASSERT_EQ(InitApp(g_cmd_line_args.app_dir, &sub_tree, true, true), error::OK);
    ASSERT_EQ(CheckTopology(sub_tree, false), error::OK);
}

/**
* @brief    decommission app test suite
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: ;
*           mock: pm/zookeeper/hdfs;
*/
class test_DecommissionApp_suite : public ::testing::Test {
protected:
    test_DecommissionApp_suite() {};
    virtual ~test_DecommissionApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DecommissionApp_suite, *)
        /* init some args */
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        ASSERT_EQ(test_InitAndLogin(), error::OK);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DecommissionApp_suite, *)
        test_ResetClient();
    };
public:
};

/**
* @brief    test decommission a app with application name
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
*/
TEST_F(test_DecommissionApp_suite, DecommissionAppWithAppName) {
    //  g_cmd_line_args.app_name = "word_count_test";
    //  ASSERT_EQ(DecommissionAppAct(), error::OK);
}

/**
* @brief    submit a new app test suite
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: ;
*           mock: pm/zookeeper/hdfs;
*/
class test_SubmitApp_suite : public ::testing::Test {
protected:
    test_SubmitApp_suite() {};
    virtual ~test_SubmitApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_SubmitApp_suite, *)
        /* init some args */
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_complicated_topo";

        ASSERT_EQ(test_InitAndLogin(), error::OK);

        GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir);
        test_DecommissionApp(g_cmd_line_args.app_name);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
    };
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        //Called after every TEST_F(test_SubmitApp_suite, *)
        test_ResetClient();
    };
public:
};

/**
* @brief    test submit an new application
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
*/
TEST_F(test_SubmitApp_suite, SubmitApp) {
    ASSERT_EQ(SubmitAppAct(), error::OK);
}

/**
* @brief    submit a wrong app test suite
* @author   fangjun, fangjun02@baidu.com
* @date     2012-08-20
* @notice   init args: ;
*           mock: pm/zookeeper/hdfs;
*/
class test_SubmitWrongApp_suite : public ::testing::Test {
protected:
    test_SubmitWrongApp_suite() {};
    virtual ~test_SubmitWrongApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_SubmitApp_suite, *)
        /* init some args */
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
        g_cmd_line_args.app_dir  = "./topo_check_test_incorrect_topo_without_executed_files";

        ASSERT_EQ(test_InitAndLogin(), error::OK);

        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_SubmitApp_suite, *)
        test_ResetClient();
    };
public:
};

/**
* @brief    test submit an wrong application
* @begin_version 1.0.0.0
* @author   fangjun, fangjun02@baidu.com
* @date     2012-08-20
*/
TEST_F(test_SubmitWrongApp_suite, SubmitWrongApp) {
    ASSERT_NE(SubmitAppAct(), error::OK);
}

/**
* @brief    add sub tree test suite
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: ;
*           mock: pm/zookeeper/hdfs;
*/
class test_AddSubTree_suite : public ::testing::Test {
protected:
    test_AddSubTree_suite() {};
    virtual ~test_AddSubTree_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AddSubTree_suite, *)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        /* init args for submit/decommission app */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);

        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* init args for sub tree */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_add_sub_tree";
    };
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir, true),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        //Called after every TEST_F(test_AddSubTree_suite, *)
        test_ResetClient();
    };
public:
};

/**
* @brief    test dynamic adding sub-tree into a running application
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
*/
TEST_F(test_AddSubTree_suite, AddSubTree) {
    ASSERT_EQ(AddSubTreeAct(), error::OK);
}

/**
* @brief    delete sub tree test suite
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
* @notice   init args: ;
*           mock: pm/zookeeper/hdfs;
*/
class test_DelSubTree_suite : public ::testing::Test {
protected:
    test_DelSubTree_suite() {};
    virtual ~test_DelSubTree_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelSubTree_suite, *)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        /* init args for submit/decommission app */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* init args for sub tree */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_add_sub_tree";
        /* add sub tree into app */
        ASSERT_EQ(AddSubTreeAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* sleep to wait pm change app status from UPDATING into RUNNING */
        sleep(10);
        /* init args fro sub tree*/
        g_cmd_line_args.app_dir = "./topo_check_test_correct_del_sub_tree";
    };
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir, true),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        //Called after every TEST_F(test_DelSubTree_suite, *)
        test_ResetClient();
    };
public:
};

/**
* @brief    test dynamic deleting sub-tree into a running application
* @begin_version 1.0.0.0
* @author   konghui, konghui@baidu.com
* @date     2012-04-05
*/
TEST_F(test_DelSubTree_suite, DelSubTree) {
    ASSERT_EQ(DelSubTreeAct(), error::OK);
}

/**
 * @brief   add importer test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-13
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_AddImporter_suite : public ::testing::Test {
protected:
    test_AddImporter_suite() {};
    virtual ~test_AddImporter_suite() {};
    virtual void SetUp() {
        // Called before every TEST_F(test_AddImporter_suite,*)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        /* init args for submit/ decommission app */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* init args for dynamic importer */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_add_importer";
    }
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir, false, true),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        // Called after every TEST_F(test_AddImporter_suite, *)
        test_ResetClient();
    };
};
/**
 * @brief   test dynamic adding importer into a running application
 * @begin_version 1.3.0.0
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-13
 */
TEST_F(test_AddImporter_suite, AddImporter) {
    ASSERT_EQ(AddImporterAct(), error::OK);
}

/**
 * @brief   multi add/delete importer test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-12-04
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_DynamicImporterPress_suite : public ::testing::Test {
protected:
    test_DynamicImporterPress_suite() {};
    virtual ~test_DynamicImporterPress_suite() {};
    virtual void SetUp() {
        // Called before every TEST_F(test_DynamicImporterPress_suite,*)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        /* init args for submit/ decommission app */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* init args for dynamic importer */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_add_multi_importer";
    }
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir, false, true),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        // Called after every TEST_F(test_AddImporter_suite, *)
        test_ResetClient();
    };
};
/**
 * @brief   test dynamic adding or deleting multi importer into a running application
 * @begin_version 1.4.0.0
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-12-04
 */
TEST_F(test_DynamicImporterPress_suite, DynamicImporter) {
    ASSERT_EQ(AddImporterAct(), error::OK);

    sleep(10);
    ASSERT_EQ(DelImporterAct(), error::OK);

    sleep(10);
    /* init args for add new importer */
    g_cmd_line_args.app_dir = "./topo_check_test_correct_add_importer";
    ASSERT_EQ(AddImporterAct(), error::OK);
}
/**
 * @brief   del importer test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-13
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_DelImporter_suite : public ::testing::Test {
protected:
    test_DelImporter_suite() {};
    virtual ~test_DelImporter_suite() {};
    virtual void SetUp() {
        // Called before every TEST_F(test_DelImporter_suite,*)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";

        /* init args for submit/ decommission app */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* init args for dynamic importer */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_add_importer";
        /* add importer into app */
        ASSERT_EQ(AddImporterAct(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;

        /* sleep to wait pm change app status from UPDATING into RUNNING */
        sleep(10);
        /* init args for dynamic importer */
        g_cmd_line_args.app_dir = "./topo_check_test_correct_del_importer";
    }
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir, false, true),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        // Called after every TEST_F(test_DelImporter_suite, *)
        test_ResetClient();
    };
};
/**
 * @brief   test dynamic deleting importer into a running application
 * @begin_version 1.3.0.0
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-13
 */
TEST_F(test_DelImporter_suite, DelImporter) {
    ASSERT_EQ(DelImporterAct(), error::OK);
}

/**
 * @brief   flow control operation test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_FlowControl_suite : public ::testing::Test {
protected:
    test_FlowControl_suite() {};
    virtual ~test_FlowControl_suite() {};
    virtual void SetUp() {
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
        /* init args for submit/decommission app */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);

        /* flow control with g_app_id */
        g_cmd_line_args.app_id = g_app_id;

        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
    };
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        test_ResetClient();
    };
public:
};

/**
 * @brief   test get status of flow control function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_FlowControl_suite, GetStatus) {
    g_cmd_line_args.cmd = "status";
    ASSERT_EQ(FlowControlAct(), error::OK);
}

/**
 * @brief   test stop flow control function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_FlowControl_suite, StopFlow) {
    g_cmd_line_args.cmd = "stop";
    ASSERT_EQ(FlowControlAct(), error::OK);
}

/**
 * @brief   test start of flow control function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_FlowControl_suite, StartFlow) {
    g_cmd_line_args.cmd = "start";
    ASSERT_EQ(FlowControlAct(), error::OK);
}

/**
 * @brief   update parallelism operation test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_UpdatePara_suite : public ::testing::Test {
protected:
    test_UpdatePara_suite() {};
    virtual ~test_UpdatePara_suite() {};
    virtual void SetUp() {
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
        /* init args for submit/decommission app */
        g_cmd_line_args.app_dir  = "./topo_check_test_correct_complicated_topo";
        /* init config and login into pm */
        ASSERT_EQ(test_InitAndLogin(), error::OK);
        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
        /* submit app */
        ASSERT_EQ(SubmitAppAct(), error::OK);

        /* flow control with g_app_id */
        g_cmd_line_args.app_id = g_app_id;

        /* reset app_id, may affect other test cases if not */
        g_app_id = -1;
    };
    virtual void TearDown() {
        /* decommission app */
        ASSERT_EQ(GetAppName(g_cmd_line_args.app_name, g_cmd_line_args.app_dir),
                  error::OK);
        ASSERT_EQ(test_DecommissionApp(g_cmd_line_args.app_name), error::OK);

        test_ResetClient();
    };
public:
};

/**
 * @brief   test add parallelism function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_UpdatePara_suite, AddPara) {
    g_cmd_line_args.app_dir = "./topo_check_test_correct_add_para";
    ASSERT_EQ(UpdateParallelismAct(), error::OK);
}

/**
 * @brief   test reduce parallelism function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_UpdatePara_suite, ReducePara) {
    g_cmd_line_args.app_dir = "./topo_check_test_correct_reduce_para";
    ASSERT_EQ(UpdateParallelismAct(), error::OK);
}

/**
 * @brief   test incorrect add parallelism function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-10-16
 * @notice  init args:
 */
TEST_F(test_UpdatePara_suite, AddImporterPara) {
    g_cmd_line_args.app_dir = "./topo_check_test_incorrect_add_para";
    ASSERT_NE(UpdateParallelismAct(), error::OK);
}

/**
 * @brief   user operation test suite
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 * @notice  init args:;
 *          mock: pm/zookeeper/hdfs;
 */
class test_UserOperation_suite : public ::testing::Test {
protected:
    test_UserOperation_suite() {};
    virtual ~test_UserOperation_suite() {};
    virtual void SetUp() {
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
        ASSERT_EQ(test_InitAndLogin(), error::OK);
    };
    virtual void TearDown() {
        test_ResetClient();
    };
public:
};

/**
 * @brief   test get user function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 * @notice  init args:
 */
TEST_F(test_UserOperation_suite, GetUser) {
    ASSERT_EQ(GetUserAct(), error::OK);
}

/**
 * @brief   test add user function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 * @notice  init args: username, password
 */
TEST_F(test_UserOperation_suite, AddUser) {
    g_cmd_line_args.username = "test";
    g_cmd_line_args.password = "test";
    ASSERT_EQ(AddUserAct(), error::OK);
}

/**
 * @brief   test delete user function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 * @notice  init args: username
 */
TEST_F(test_UserOperation_suite, DelUser) {
    g_cmd_line_args.username = "test";
    ASSERT_EQ(DelUserAct(), error::OK);
}

/**
 * @brief   test update user function
 * @author  fangjun, fangjun02@baidu.com
 * @date    2012-08-15
 * @notice  init args: username
 */
TEST_F(test_UserOperation_suite, UpdateUser) {
    g_cmd_line_args.username = "client_unit_test";
    g_cmd_line_args.password = "test";
    ASSERT_EQ(UpdateUserAct(), error::OK);
}
