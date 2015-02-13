/**
 * @brief test case of pm client
 * @author fangjun, fangjun02@baidu.com
 * @date   2013-01-29
 */
#include "common/logger.h"
#include "common/application_tools.h"
#include "client/pm_client.h"
#include "client/dstream_client.h"

#include "mock_zk.h"
#include "mock_hdfs_client.h"
#include "mock_pm.h"
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;

using namespace dstream;
using namespace client;
using namespace hdfs_client;
using namespace application_tools;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_pm_client");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief construction function, no need to test
**/
/*class test_PMClient_PMClient_suite : public ::testing::Test{
    protected:
        test_PMClient_PMClient_suite(){};
        virtual ~test_PMClient_PMClient_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_PMClient_PMClient_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_PMClient_PMClient_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_PMClient_PMClient_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief no need to test
**/
class test_PMClient_Reset_suite : public ::testing::Test {
protected:
    test_PMClient_Reset_suite() {};
    virtual ~test_PMClient_Reset_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_Reset_suite, *)
        pm_client = new (std::nothrow) PMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_Reset_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
private:
    PMClient* pm_client;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_Reset_suite, TestReset)
{
    pm_client->Reset();
    ASSERT_EQ(pm_client->m_login, false);
}

/**
 * @brief no need to test
**/
class test_PMClient_set_reconnection_interval_seconds_suite : public ::testing::Test {
protected:
    test_PMClient_set_reconnection_interval_seconds_suite() {};
    virtual ~test_PMClient_set_reconnection_interval_seconds_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_set_reconnection_interval_seconds_suite, *)
        pm_client = new (std::nothrow) PMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_set_reconnection_interval_seconds_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
private:
    PMClient* pm_client;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_set_reconnection_interval_seconds_suite, TestSetInterval)
{
    pm_client->set_reconnection_interval_seconds(256);
    ASSERT_EQ(256u, pm_client->m_reconnection_interval_seconds);
}

/**
 * @brief  no need to test
**/
class test_PMClient_set_reconnection_times_suite : public ::testing::Test {
protected:
    test_PMClient_set_reconnection_times_suite() {};
    virtual ~test_PMClient_set_reconnection_times_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_set_reconnection_times_suite, *)
        pm_client = new (std::nothrow) PMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_set_reconnection_times_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
private:
    PMClient* pm_client;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_set_reconnection_times_suite, TestSetTimes)
{
    pm_client->set_reconnection_times(256);
    ASSERT_EQ(256u, pm_client->m_reconnection_times);
}

/**
 * @brief test of Connect function
**/
class test_PMClient_Connect_suite : public ::testing::Test {
protected:
    test_PMClient_Connect_suite() {};
    virtual ~test_PMClient_Connect_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_Connect_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow)MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_Connect_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestConnectOK();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_Connect_suite::TestConnectOK() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);
    //mock_pm->StartPM();
    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillOnce(Return(error::CLIENT_CONNECT_PM_FAIL))
    .WillRepeatedly(Return(error::OK));

    ASSERT_NE(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_NE(error::OK, pm_client->Connect("", &dstream_user));

    mock_pm->StartPM();
    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));
    //assertting current version less equal 1-8-0-0, if great equal,
    //you should change the value 
    pm_client->m_login = false;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version("1-8-0-0");
    ASSERT_NE(error::OK, pm_client->Connect("", &dstream_user));
    
    pm_client->m_login = false;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(config_const::kPMMinRequiredClientVersion);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_login = false;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version("1-5-0-0");
    ASSERT_NE(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_login = true;
    dstream_user.set_username("wrong_client");
    dstream_user.set_password("wront_client");
    dstream_user.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_login = false;
    dstream_user.set_username("wrong_client");
    dstream_user.set_password("wront_client");
    dstream_user.set_version(VERSION);
    ASSERT_NE(error::OK, pm_client->Connect("", &dstream_user));

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_Connect_suite, TestConnectOK)
{
    TestConnectOK();
}

/**
 * @brief
**/
class test_PMClient_LoginPM_suite : public ::testing::Test {
protected:
    test_PMClient_LoginPM_suite() {};
    virtual ~test_PMClient_LoginPM_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_LoginPM_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow)MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_LoginPM_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestLoginPM();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_LoginPM_suite::TestLoginPM() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);
    //mock_pm->StartPM();
    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillOnce(Return(error::CLIENT_CONNECT_PM_FAIL))
    .WillRepeatedly(Return(error::OK));

    ASSERT_NE(error::OK, pm_client->LoginPM("", &dstream_user));

    mock_pm->StartPM();
    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->LoginPM("", &dstream_user));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_LoginPM_suite, TestLoginPM)
{
    TestLoginPM();
}

/**
 * @brief
**/
class test_PMClient_SubmitApp_suite : public ::testing::Test {
protected:
    test_PMClient_SubmitApp_suite() {};
    virtual ~test_PMClient_SubmitApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_SubmitApp_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_hdfs_client = new (std::nothrow) MockHdfsClient();
        mock_zk = new (std::nothrow) MockZK();
        g_cmd_line_args.app_dir = "./topo_check_test_correct_complicated_topo/";
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_SubmitApp_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        /*if (NULL != mock_hdfs_client) {
          delete mock_hdfs_client;
          mock_hdfs_client = NULL;
        }*/
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestSubmitApp();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockHdfsClient* mock_hdfs_client;
    MockZK* mock_zk;
};

void test_PMClient_SubmitApp_suite::TestSubmitApp() {
    Application submit_app;
    ConstructFakeApp(&submit_app);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_hdfs_client = reinterpret_cast<HdfsClient*>(mock_hdfs_client);
    EXPECT_CALL(*mock_hdfs_client, TestAndRemoveDir(_))
    .WillOnce(Return(error::FILE_OPERATION_ERROR))
    .WillRepeatedly(Return(error::OK));
    EXPECT_CALL(*mock_hdfs_client, UploadFile(_, _, _))
    .WillOnce(Return(error::FILE_OPERATION_ERROR))
    .WillRepeatedly(Return(error::OK));
    //EXPECT_CALL(*mock_hdfs_client, MakeFile(_))
    //  .WillRepeatedly(Return(error::OK));

    // test of remove dir failed
    ASSERT_NE(error::OK, pm_client->SubmitApp(&submit_app));
    // test of upload file failed
    ASSERT_NE(error::OK, pm_client->SubmitApp(&submit_app));

    // test OK
    ASSERT_EQ(error::OK, pm_client->SubmitApp(&submit_app));

    // test of g_app_id is not -1
    g_app_id = 1;
    ASSERT_EQ(error::OK, pm_client->SubmitApp(&submit_app));

    // test of apply app id call failed
    g_app_id = -1;
    mock_pm->set_rpc_fail(ASKFORAPPID_FAIL);
    ASSERT_EQ(error::CLIENT_APPLY_APP_ID_FAIL, pm_client->SubmitApp(&submit_app));
    mock_pm->set_rpc_succ(ASKFORAPPID_FAIL);

    // test of submit app call failed
    mock_pm->set_rpc_fail(SUBMITAPP_FAIL);
    ASSERT_EQ(error::CLIENT_SUBMIT_APP_FAIL, pm_client->SubmitApp(&submit_app));
    mock_pm->set_rpc_succ(SUBMITAPP_FAIL);

    // test of notify pm failed
    mock_pm->set_rpc_fail(FILEUPLOADDONE_FAIL);
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, pm_client->SubmitApp(&submit_app));
    mock_pm->set_rpc_succ(FILEUPLOADDONE_FAIL);

    mock_pm->StopPM();
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_SubmitApp_suite, TestSubmitAppOK)
{
    TestSubmitApp();
}

/**
 * @brief
**/
class test_PMClient_AddSubTree_suite : public ::testing::Test {
protected:
    test_PMClient_AddSubTree_suite() {};
    virtual ~test_PMClient_AddSubTree_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_AddSubTree_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_hdfs_client = new (std::nothrow) MockHdfsClient();
        mock_zk = new (std::nothrow) MockZK();
        g_cmd_line_args.app_dir = "./topo_check_test_correct_add_sub_tree/";
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_AddSubTree_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        /*if (NULL != mock_hdfs_client) {
          delete mock_hdfs_client;
          mock_hdfs_client = NULL;
        }*/
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestAddSubTree();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockHdfsClient* mock_hdfs_client;
    MockZK* mock_zk;
};

void test_PMClient_AddSubTree_suite::TestAddSubTree() {
    Application sub_tree;
    //TODO
    ConstructFakeSubTree(true, &sub_tree);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_hdfs_client = reinterpret_cast<HdfsClient*>(mock_hdfs_client);
    EXPECT_CALL(*mock_hdfs_client, UploadFile(_, _, _))
    .WillOnce(Return(error::FILE_OPERATION_ERROR))
    .WillRepeatedly(Return(error::OK));
    //EXPECT_CALL(*mock_hdfs_client, MakeFile(_))
    //  .WillRepeatedly(Return(error::OK));

    // test of upload file fail
    ASSERT_NE(error::OK, pm_client->AddSubTree(sub_tree));

    ASSERT_EQ(error::OK, pm_client->AddSubTree(sub_tree));

    // test of update topology call fail
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    ASSERT_NE(error::OK, pm_client->AddSubTree(sub_tree));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);

    // test of fail to get app from pm
    mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    ASSERT_NE(error::OK, pm_client->AddSubTree(sub_tree));
    mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);

    // test of topo INVALID
    mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
    ASSERT_NE(error::OK, pm_client->AddSubTree(sub_tree));
    mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);

    // test of update topo call fail again
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    ASSERT_NE(error::OK, pm_client->AddSubTree(sub_tree));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_AddSubTree_suite, TestAddSubTree)
{
    TestAddSubTree();
}

/**
 * @brief
*/
class test_PMClient_DelSubTree_suite : public ::testing::Test {
protected:
    test_PMClient_DelSubTree_suite() {};
    virtual ~test_PMClient_DelSubTree_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_DelSubTree_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
        g_cmd_line_args.app_dir = "./topo_check_test_correct_add_sub_tree/";
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_DelSubTree_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestDelSubTree();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_DelSubTree_suite::TestDelSubTree() {
    Application sub_tree;
    //TODO
    ConstructFakeSubTree(false, &sub_tree);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_EQ(error::OK, pm_client->DelSubTree(sub_tree));

    // test of update topology call fail
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    ASSERT_NE(error::OK, pm_client->DelSubTree(sub_tree));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);

    // test of fail to get app from pm
    mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    ASSERT_NE(error::OK, pm_client->DelSubTree(sub_tree));
    mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);

    // test of topo INVALID
    /*mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
    ASSERT_NE(error::OK, pm_client->DelSubTree(sub_tree));
    mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);*/

    // test of update topo call fail again
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    ASSERT_NE(error::OK, pm_client->DelSubTree(sub_tree));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);

    mock_pm->StopPM();

}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_DelSubTree_suite, TestDelSubTree)
{
    TestDelSubTree();
}

/**
 * @brief
**/
class test_PMClient_AddImporter_suite : public ::testing::Test {
protected:
    test_PMClient_AddImporter_suite() {};
    virtual ~test_PMClient_AddImporter_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_AddImporter_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_hdfs_client = new (std::nothrow) MockHdfsClient();
        mock_zk = new (std::nothrow) MockZK();
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_AddImporter_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        /*if (NULL != mock_hdfs_client) {
            delete mock_hdfs_client;
            mock_hdfs_client = NULL;
        }*/
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestAddImporter();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockHdfsClient* mock_hdfs_client;
    MockZK* mock_zk;
};

void test_PMClient_AddImporter_suite::TestAddImporter() {
    Application importer;
    //TODO
    ConstructFakeImporter(true, &importer);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_hdfs_client = reinterpret_cast<HdfsClient*>(mock_hdfs_client);
    EXPECT_CALL(*mock_hdfs_client, UploadFile(_, _, _))
    .WillOnce(Return(error::FILE_OPERATION_ERROR))
    .WillRepeatedly(Return(error::OK));
    //EXPECT_CALL(*mock_hdfs_client, MakeFile(_))
    //  .WillRepeatedly(Return(error::OK));

    //test of upload file fail
    ASSERT_NE(error::OK, pm_client->AddImporter(importer));

    ASSERT_EQ(error::OK, pm_client->AddImporter(importer));

    // test of update topology call fail
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);

    // test of fail to get app from pm
    mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);

    // test of topo INVALID
    /*mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
    ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);
    */

    // test of update topo call fail again
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_AddImporter_suite, TestAddImporter)
{
    TestAddImporter();
}

/**
 * @brief
**/
class test_PMClient_DelImporter_suite : public ::testing::Test {
protected:
    test_PMClient_DelImporter_suite() {};
    virtual ~test_PMClient_DelImporter_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_DelImporter_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_DelImporter_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestDelImporter();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_DelImporter_suite::TestDelImporter() {
    Application importer;
    //TODO
    ConstructFakeImporter(false, &importer);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_EQ(error::OK, pm_client->DelImporter(importer));

    // test of update topology call fail
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    ASSERT_NE(error::OK, pm_client->DelImporter(importer));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);

    // test of fail to get app from pm
    mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    ASSERT_NE(error::OK, pm_client->DelImporter(importer));
    mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);

    // test of topo INVALID
    /*  mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
      ASSERT_NE(error::OK, pm_client->DelImporter(importer));
      mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);
    */
    // test of update topo call fail again
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    ASSERT_NE(error::OK, pm_client->DelImporter(importer));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);

    mock_pm->StopPM();
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_DelImporter_suite, TestDelImporter)
{
    TestDelImporter();
}

/**
 * @brief
**/
class test_PMClient_UpdateParallelism_suite : public ::testing::Test {
protected:
    test_PMClient_UpdateParallelism_suite() {};
    virtual ~test_PMClient_UpdateParallelism_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_UpdateParallelism_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_UpdateParallelism_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestUpdatePara();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_UpdateParallelism_suite::TestUpdatePara() {
    Application uppara;
    //TODO
    ConstructFakeUpPara(&uppara);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_EQ(error::OK, pm_client->UpdateParallelism(uppara));

    // test of update topology call fail
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    ASSERT_NE(error::OK, pm_client->UpdateParallelism(uppara));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);

    // test of fail to get app from pm
    mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    ASSERT_NE(error::OK, pm_client->UpdateParallelism(uppara));
    mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);

    // test of topo INVALID
    /*  mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
      ASSERT_NE(error::OK, pm_client->UpdateParallelism(uppara));
      mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);
    */
    // test of update topo call fail again
    mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    ASSERT_NE(error::OK, pm_client->UpdateParallelism(uppara));
    mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);


    mock_pm->StopPM();
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_UpdateParallelism_suite, TestUpdatePara)
{
    TestUpdatePara();
}

/**
 * @brief
**/
class test_PMClient_DecommissionApp_suite : public ::testing::Test {
protected:
    test_PMClient_DecommissionApp_suite() {};
    virtual ~test_PMClient_DecommissionApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_DecommissionApp_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_DecommissionApp_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestDecommissionAppOK();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_DecommissionApp_suite::TestDecommissionAppOK() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_EQ(error::OK, pm_client->DecommissionApp("client_ut"));
    ASSERT_EQ(error::OK, pm_client->DecommissionApp(1));
    ASSERT_NE(error::OK, pm_client->DecommissionApp("wrong_client_ut"));
    ASSERT_NE(error::OK, pm_client->DecommissionApp(-1));

    mock_pm->StopPM();

}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_DecommissionApp_suite, TestDecommissionAppOK)
{
    TestDecommissionAppOK();
}

/**
 * @brief
**/
class test_PMClient_FlowControlApp_suite : public ::testing::Test {
protected:
    test_PMClient_FlowControlApp_suite() {};
    virtual ~test_PMClient_FlowControlApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_FlowControlApp_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_FlowControlApp_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestFlowControlApp();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_FlowControlApp_suite::TestFlowControlApp() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    ASSERT_EQ(error::OK, pm_client->FlowControlApp("test", "fake-importer", "start"));
    ASSERT_EQ(error::OK, pm_client->FlowControlApp(1, "fake-importer", "start"));

    // test of request fail
    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, pm_client->FlowControlApp("test", "fake-importer", "wrong"));
    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, pm_client->FlowControlApp(1, "fake-importer", "wrong"));

    // test of flow control call fail
    mock_pm->set_rpc_fail(FLOWCONTROL_FAIL);
    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, pm_client->FlowControlApp("test", "fake-importer", "start"));
    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, pm_client->FlowControlApp(1, "fake-importer", "start"));
    mock_pm->set_rpc_succ(FLOWCONTROL_FAIL);

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_FlowControlApp_suite, TestFlowControlApp)
{
    TestFlowControlApp();
}

/**
 * @brief
**/
class test_PMClient_SetFlowControlRequest_suite : public ::testing::Test {
protected:
    test_PMClient_SetFlowControlRequest_suite() {};
    virtual ~test_PMClient_SetFlowControlRequest_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_SetFlowControlRequest_suite, *)
        pm_client = new (std::nothrow) PMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_SetFlowControlRequest_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestSetFlowControlRequest();
private:
    PMClient* pm_client;
};

void test_PMClient_SetFlowControlRequest_suite::TestSetFlowControlRequest() {
    FlowControlRequest req;
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("start", "fake-importer", &req));
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("START", "fake-importer", &req));
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("stop", "fake-importer", &req));
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("STOP", "fake-importer", &req));
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("status", "fake-importer", &req));
    ASSERT_EQ(error::OK, pm_client->SetFlowControlRequest("STATUS", "fake-importer", &req));
    ASSERT_NE(error::OK, pm_client->SetFlowControlRequest("wrong", "fake-importer", &req));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_SetFlowControlRequest_suite, TestSetFlowControlRequest)
{
    TestSetFlowControlRequest();
}

/**
 * @brief
**/
class test_PMClient_SetFlowControlResult_suite : public ::testing::Test {
protected:
    test_PMClient_SetFlowControlResult_suite() {};
    virtual ~test_PMClient_SetFlowControlResult_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_SetFlowControlResult_suite, *)
        pm_client = new (std::nothrow) PMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_SetFlowControlResult_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestSetFlowControlResult();
private:
    PMClient* pm_client;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_SetFlowControlResult_suite, TestSetFlowControlResult)
{
    ASSERT_TRUE(true);
}

/**
 * @brief
**/
class test_PMClient_FlowControlInfo_suite : public ::testing::Test {
protected:
    test_PMClient_FlowControlInfo_suite() {};
    virtual ~test_PMClient_FlowControlInfo_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_FlowControlInfo_suite, *)
        pm_client = new (std::nothrow) PMClient();
        sprintf(pm_client->flow_control_info, "Hello World");
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_FlowControlInfo_suite, *)
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestFlowControlInfo();
private:
    PMClient* pm_client;
};

void test_PMClient_FlowControlInfo_suite::TestFlowControlInfo() {
    ASSERT_EQ("Hello World", pm_client->FlowControlInfo());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_FlowControlInfo_suite, TestFlowControlInfo)
{
    TestFlowControlInfo();
}

/**
 * @brief
**/
class test_PMClient_GetAppTopology_suite : public ::testing::Test {
protected:
    test_PMClient_GetAppTopology_suite() {};
    virtual ~test_PMClient_GetAppTopology_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_GetAppTopology_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_GetAppTopology_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != pm_client) {
            pm_client = NULL;
        }
    };
    void TestGetAppTopo();
private:
    PMClient* pm_client;
    MockZK* mock_zk;
};

void test_PMClient_GetAppTopology_suite::TestGetAppTopo() {
    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetApp(_, _, _, _))
    .WillOnce(Return(error::ZK_NO_NODE))
    .WillOnce(Return(error::OK));

    Application app;
    ASSERT_EQ(error::ZK_NO_NODE, pm_client->GetAppTopology("test", app));
    ASSERT_EQ(error::OK, pm_client->GetAppTopology("test", app));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_GetAppTopology_suite, TestGetAppTopo)
{
    TestGetAppTopo();
}

/**
 * @brief Update Process function test
**/
class test_PMClient_UpdateProcessor_suite : public ::testing::Test {
protected:
    test_PMClient_UpdateProcessor_suite() {};
    virtual ~test_PMClient_UpdateProcessor_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_UpdateProcessor_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
        mock_hdfs_client = new (std::nothrow) MockHdfsClient();
        g_cmd_line_args.app_dir = "./topo_check_test_correct_update_processor/";
        pm_client->Init();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_UpdateProcessor_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        /*if (NULL != mock_hdfs_client) {
          delete mock_hdfs_client;
          mock_hdfs_client = NULL;
        }*/
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            delete pm_client;
            pm_client = NULL;
        }
    };
    void TestUpdateProcessor();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
    MockHdfsClient* mock_hdfs_client;
};

void test_PMClient_UpdateProcessor_suite::TestUpdateProcessor() {
    Application update;

    ConstructFakeUpdate(&update);
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    Application app;
    app.mutable_app_path()->set_path("hdfs://hello");
    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));
    EXPECT_CALL(*mock_zk, GetApp(_, _, _, _))
    .WillOnce(Return(error::ZK_NO_NODE))
    .WillRepeatedly(DoAll(SetArgumentPointee<3>(app),
                          Return(error::OK)));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    pm_client->m_hdfs_client = reinterpret_cast<HdfsClient*>(mock_hdfs_client);
    EXPECT_CALL(*mock_hdfs_client, UploadFile(_, _, _))
    .WillOnce(Return(error::FILE_OPERATION_ERROR))
    .WillRepeatedly(Return(error::OK));
    //EXPECT_CALL(*mock_hdfs_client, MakeFile(_))
    //  .WillRepeatedly(Return(error::OK));

    //test of get app fail
    ASSERT_NE(error::OK, pm_client->UpdateProcessor(update));
    //test of upload file fail
    ASSERT_NE(error::OK, pm_client->UpdateProcessor(update));
    //
    //  ASSERT_EQ(error::OK, pm_client->AddImporter(importer));
    //
    //  // test of update topology call fail
    //  mock_pm->set_rpc_fail(UPDATETOPOLOGY_FAIL);
    //  ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    //  mock_pm->set_rpc_succ(UPDATETOPOLOGY_FAIL);
    //
    //  // test of fail to get app from pm
    //  mock_pm->set_rpc_fail(ADD_SUB_GET_FAIL);
    //  ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    //  mock_pm->set_rpc_succ(ADD_SUB_GET_FAIL);
    //
    //  // test of topo INVALID
    //  /*mock_pm->set_rpc_fail(INVAILD_TOPO_FAIL);
    //  ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    //  mock_pm->set_rpc_succ(INVAILD_TOPO_FAIL);
    //  */
    //
    //  // test of update topo call fail again
    //  mock_pm->set_rpc_fail(UPDATETOPOLOGY_ACK_FAIL);
    //  ASSERT_NE(error::OK, pm_client->AddImporter(importer));
    //  mock_pm->set_rpc_succ(UPDATETOPOLOGY_ACK_FAIL);

    mock_pm->StopPM();

}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_UpdateProcessor_suite, TestUpdateProcessor)
{
    TestUpdateProcessor();
}

/**
 * @brief  deprecated, no need to test
**/
/*class test_PMClient_MigratePE_suite : public ::testing::Test{
    protected:
        test_PMClient_MigratePE_suite(){};
        virtual ~test_PMClient_MigratePE_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_PMClient_MigratePE_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_PMClient_MigratePE_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_PMClient_MigratePE_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief  deprecated, no need to test
**/
/*class test_PMClient_KillPElement_suite : public ::testing::Test{
    protected:
        test_PMClient_KillPElement_suite(){};
        virtual ~test_PMClient_KillPElement_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_PMClient_KillPElement_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_PMClient_KillPElement_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_PMClient_KillPElement_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief  deprecated, no need to test
**/
/*class test_PMClient_DecommissionPn_suite : public ::testing::Test{
    protected:
        test_PMClient_DecommissionPn_suite(){};
        virtual ~test_PMClient_DecommissionPn_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_PMClient_DecommissionPn_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_PMClient_DecommissionPn_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_PMClient_DecommissionPn_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
class test_PMClient_GetUser_suite : public ::testing::Test {
protected:
    test_PMClient_GetUser_suite() {};
    virtual ~test_PMClient_GetUser_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_GetUser_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_GetUser_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            pm_client = NULL;
        }
    };
    void TestGetUserOK();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_GetUser_suite::TestGetUserOK() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    std::set<User, application_tools::CompareUser>userinfo;
    userinfo = pm_client->GetUser();
    ASSERT_EQ(1u, userinfo.size());
    ASSERT_EQ("client_ut", userinfo.begin()->username());
    ASSERT_EQ("client_ut", userinfo.begin()->password());

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_GetUser_suite, TestGetUserOK)
{
    TestGetUserOK();
}

/**
 * @brief
**/
class test_PMClient_OperateUser_suite : public ::testing::Test {
protected:
    test_PMClient_OperateUser_suite() {};
    virtual ~test_PMClient_OperateUser_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_PMClient_OperateUser_suite, *)
        pm_client = new (std::nothrow) PMClient();
        mock_pm = new (std::nothrow) MockPM();
        mock_zk = new (std::nothrow) MockZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_PMClient_OperateUser_suite, *)
        if (NULL != mock_zk) {
            delete mock_zk;
            mock_zk = NULL;
        }
        if (NULL != mock_pm) {
            delete mock_pm;
            mock_pm = NULL;
        }
        if (NULL != pm_client) {
            pm_client = NULL;
        }
    };
    void TestOperateUser();
private:
    PMClient* pm_client;
    MockPM* mock_pm;
    MockZK* mock_zk;
};

void test_PMClient_OperateUser_suite::TestOperateUser() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    mock_pm->StartPM();

    g_zk = reinterpret_cast<ZK*>(mock_zk);
    EXPECT_CALL(*mock_zk, GetPM(_, _, _))
    .WillRepeatedly(Return(error::OK));

    pm_client->m_pm.set_host(mock_pm->m_pm.host());
    pm_client->m_pm.set_submit_port(mock_pm->m_pm.submit_port());
    pm_client->m_pm.set_version(VERSION);
    ASSERT_EQ(error::OK, pm_client->Connect("", &dstream_user));

    std::set<User, application_tools::CompareUser>userinfo;
    ASSERT_EQ(error::OK, pm_client->UpdateUser("modify", "modify"));
    ASSERT_EQ(error::OK, pm_client->AddUser("add", "add"));
    ASSERT_EQ(error::OK, pm_client->DeleteUser("client_ut"));

    mock_pm->set_rpc_fail(USEROP_FAIL);
    ASSERT_NE(error::OK, pm_client->DeleteUser("client_ut"));
    mock_pm->set_rpc_succ(USEROP_FAIL);

    mock_pm->StopPM();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_PMClient_OperateUser_suite, TestOperateUser)
{
    TestOperateUser();
}
