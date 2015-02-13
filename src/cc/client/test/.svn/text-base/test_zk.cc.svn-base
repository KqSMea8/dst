/**
* @brief test zk for client
*
* @param  argc
* @param  argv
*
* @return
* @author fangjun,fangjun02@baidu.com
* @date   2013-02-20
*/
#include "mock_zk_client.h"
#include "client/zk.h"
#include "common/application_tools.h"
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;

using namespace dstream;
using namespace client;
using namespace zk_client;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_zk");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief  no need to test
**/
class test_ZKWatcher_OnEvent_suite : public ::testing::Test {
protected:
    test_ZKWatcher_OnEvent_suite() {};
    virtual ~test_ZKWatcher_OnEvent_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZKWatcher_OnEvent_suite, *)
        zk_ = new (std::nothrow) ZK("test");
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZKWatcher_OnEvent_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
    };
    void TestOnEvent();
private:
    ZK* zk_;
};

void test_ZKWatcher_OnEvent_suite::TestOnEvent() {
    zk_->m_connected = true;
    zk_->m_watcher.OnEvent(ZOO_SESSION_EVENT, 0, "test");
    //ASSERT_EQ(false, zk_->m_connected);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKWatcher_OnEvent_suite, TestOnEvent)
{
    //TestOnEvent();
}

/**
 * @brief  construct, no need to test
**/
class test_ZK_ZK_suite : public ::testing::Test {
protected:
    test_ZK_ZK_suite() {};
    virtual ~test_ZK_ZK_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_ZK_suite, *)
        zk_ = new (std::nothrow) ZK("test");
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_ZK_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
    };
    void TestZK();
private:
    ZK* zk_;
};

void test_ZK_ZK_suite::TestZK() {
    ASSERT_EQ(zk_->m_connected, false);
    ASSERT_STREQ(zk_->m_server_list.c_str(), "test");
    ASSERT_NE(zk_->m_client, reinterpret_cast<zk_client::ZkClient*>(NULL));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_ZK_suite, case_name1)
{
    TestZK();
}

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_ZK_ZK_suite, case_name2)
{
    //TODO
}*/

/**
 * @brief
**/
class test_ZK_Connection_suite : public ::testing::Test {
protected:
    test_ZK_Connection_suite() {};
    virtual ~test_ZK_Connection_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_Connection_suite, *)
        zk_ = new (std::nothrow) ZK();
        mock_zk_client_ = new (std::nothrow) MockZkClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_Connection_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
        /*if (NULL != mock_zk_client_) {
          delete mock_zk_client_;
          mock_zk_client_ = NULL;
        }*/
    };
    void TestConnection();
private:
    ZK* zk_;
    MockZkClient* mock_zk_client_;
};

void test_ZK_Connection_suite::TestConnection() {
    zk_->m_client = reinterpret_cast<ZkClient*>(mock_zk_client_);
    EXPECT_CALL(*mock_zk_client_, Connect())
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::COMM_CONNECT_FAIL))
    .WillOnce(Return(error::OK));
    EXPECT_CALL(*mock_zk_client_, SetConnBrokenWatcher(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::BAD_ARGUMENT));

    // test connection OK
    ASSERT_EQ(error::OK, zk_->Connection());

    // test if already m_connected
    ASSERT_EQ(error::OK, zk_->Connection());

    // test of Connect fail
    zk_->m_connected = false;
    ASSERT_NE(error::OK, zk_->Connection());

    // test of SetConnBrokenWatcher fail
    ASSERT_NE(error::OK, zk_->Connection());

    zk_->m_connected = false;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_Connection_suite, TestConnection)
{
    TestConnection();
}

/**
 * @brief
**/
class test_ZK_Register_suite : public ::testing::Test {
protected:
    test_ZK_Register_suite() {};
    virtual ~test_ZK_Register_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_Register_suite, *)
        zk_ = new (std::nothrow) ZK();
        mock_zk_client_ = new (std::nothrow) MockZkClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_Register_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
        /*if (NULL != mock_zk_client_) {
          delete mock_zk_client_;
          mock_zk_client_ = NULL;
        }*/

    };
    void TestRegister();
private:
    ZK* zk_;
    MockZkClient* mock_zk_client_;
};

void test_ZK_Register_suite::TestRegister() {
    User dstream_user;
    dstream_user.set_username("client_ut");
    dstream_user.set_password("client_ut");
    dstream_user.set_version(VERSION);

    zk_->m_client = reinterpret_cast<ZkClient*>(mock_zk_client_);
    EXPECT_CALL(*mock_zk_client_, Connect())
    // test register OK
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::ZK_CONNECT_ERROR));
    EXPECT_CALL(*mock_zk_client_, SystemRootExists(_, _))
    // test register OK
    .WillOnce(Return(error::OK))
    // test if already connected
    .WillOnce(Return(error::OK))
    // test if judge system root exists fail
    .WillOnce(Return(error::ZK_GET_NODE_FAIL))
    // test if system root does already exists
    .WillOnce(DoAll(SetArgPointee<1>(1),
                    Return(error::OK)))
    // test if create ephemeral node fail
    .WillOnce(Return(error::OK))
    // test if init system root fail
    .WillOnce(Return(error::OK));
    EXPECT_CALL(*mock_zk_client_, InitSystemRoot(_))
    // test register OK
    .WillOnce(Return(error::OK))
    // test if already connected
    .WillOnce(Return(error::OK))
    // test if system root does already exists
    .WillOnce(Return(error::OK))
    // test if init system root fail
    .WillOnce(Return(error::ZK_INIT_ERROR));
    EXPECT_CALL(*mock_zk_client_, CreateEphemeralNode(_, _, _))
    // test register OK
    .WillOnce(Return(error::OK))
    // test if already connected
    .WillOnce(Return(error::OK))
    // test if system root does already exists
    .WillOnce(Return(error::OK))
    // test if create ephemeral node fail
    .WillOnce(Return(error::ZK_SET_NODE_FAIL));

    // test register OK
    ASSERT_EQ(error::OK, zk_->Register(&dstream_user));

    // test if Connect fail
    zk_->m_connected = false;
    ASSERT_NE(error::OK, zk_->Register(&dstream_user));

    // test if already connected
    zk_->m_connected = true;
    ASSERT_EQ(error::OK, zk_->Register(&dstream_user));

    // test if judge system root exists fail
    ASSERT_NE(error::OK, zk_->Register(&dstream_user));

    // test if system root does already exists
    ASSERT_EQ(error::OK, zk_->Register(&dstream_user));

    // test if create ephemeral node fail
    ASSERT_EQ(error::OK, zk_->Register(&dstream_user));

    // test if init system root fail
    ASSERT_NE(error::OK, zk_->Register(&dstream_user));

    zk_->m_connected = false;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_Register_suite, TestRegister)
{
    TestRegister();
}

/**
 * @brief
**/
class test_ZK_UnRegister_suite : public ::testing::Test {
protected:
    test_ZK_UnRegister_suite() {};
    virtual ~test_ZK_UnRegister_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_UnRegister_suite, *)
        zk_ = new (std::nothrow) ZK();
        mock_zk_client_ = new (std::nothrow) MockZkClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_UnRegister_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
        /*if (NULL != mock_zk_client_) {
          delete mock_zk_client_;
          mock_zk_client_ = NULL;
        }*/
    };
    void TestUnRegister();
private:
    ZK* zk_;
    MockZkClient* mock_zk_client_;
};

void test_ZK_UnRegister_suite::TestUnRegister() {
    zk_->m_client = reinterpret_cast<ZkClient*>(mock_zk_client_);
    zk_->m_connected = true;

    EXPECT_CALL(*mock_zk_client_, DeleteEphemeralNode(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::ZK_DELETE_NODE_FAIL));

    // test UnRegister OK
    ASSERT_EQ(error::OK, zk_->UnRegister());

    // test if already connected
    zk_->m_connected = false;
    ASSERT_EQ(error::OK, zk_->UnRegister());

    // test if delete ephemeral node fail
    zk_->m_connected = true;
    ASSERT_NE(error::OK, zk_->UnRegister());

    zk_->m_connected = false;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_UnRegister_suite, TestUnRegister)
{
    TestUnRegister();
}

/**
 * @brief
**/
class test_ZK_GetPM_suite : public ::testing::Test {
protected:
    test_ZK_GetPM_suite() {};
    virtual ~test_ZK_GetPM_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_GetPM_suite, *)
        zk_ = new (std::nothrow) ZK();
        mock_zk_client_ = new (std::nothrow) MockZkClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_GetPM_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
        /*if (NULL != mock_zk_client_) {
          delete mock_zk_client_;
          mock_zk_client_ = NULL;
        }*/
    };
    void TestGetPM();
private:
    ZK* zk_;
    MockZkClient* mock_zk_client_;
};

void test_ZK_GetPM_suite::TestGetPM() {
    zk_->m_client = reinterpret_cast<ZkClient*>(mock_zk_client_);

    PM mock_pm;
    mock_pm.set_host("test_pm");
    mock_pm.set_id("test_pm_id");
    mock_pm.set_version(VERSION);
    ZkNodeData zk_data;
    zk_data.m_len = mock_pm.ByteSize();
    zk_data.m_buf = static_cast<char*>(malloc(zk_data.m_len));

    ASSERT_TRUE(mock_pm.SerializeToArray(zk_data.m_buf, zk_data.m_len));
    EXPECT_CALL(*mock_zk_client_, Connect())
    .WillOnce(Return(error::ZK_CONNECT_ERROR));
    //  .WillOnce(Return(error::OK));
    EXPECT_CALL(*mock_zk_client_, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgPointee<1>(zk_data),
                    Return(error::OK)));

    PM pm;

    // test of connect zk fail
    zk_->m_connected = false;
    ASSERT_NE(error::OK, zk_->GetPM(1, 0, &pm));

    // test of GetPM OK
    zk_->m_connected = true;
    ASSERT_EQ(error::OK, zk_->GetPM(1, 1, &pm));
    ASSERT_STREQ(pm.host().c_str(), mock_pm.host().c_str());
    ASSERT_STREQ(pm.version().c_str(), mock_pm.version().c_str());

    //NOTICE. we can not deallocate this buf which may make core
    //a memory leak here
    //free(zk_data.m_buf);
    zk_->m_connected = false;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_GetPM_suite, TestGetPM)
{
    TestGetPM();
}

/**
 * @brief
**/
class test_ZK_GetApp_suite : public ::testing::Test {
protected:
    test_ZK_GetApp_suite() {};
    virtual ~test_ZK_GetApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_GetApp_suite, *)
        zk_ = new (std::nothrow) ZK();
        mock_zk_client_ = new (std::nothrow) MockZkClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_GetApp_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
        /*if (NULL != mock_zk_client_) {
          delete mock_zk_client_;
          mock_zk_client_ = NULL;
        }*/
    };
    void TestGetApp();
private:
    ZK* zk_;
    MockZkClient* mock_zk_client_;
};

void test_ZK_GetApp_suite::TestGetApp() {
    zk_->m_client = reinterpret_cast<ZkClient*>(mock_zk_client_);

    Application mock_app;
    application_tools::ConstructFakeApp(&mock_app);
    std::list<zk_client::ZkNode*> app_list;
    ZkNode zk_node;
    ZkNodeData zk_data;
    zk_data.m_len = mock_app.ByteSize();
    zk_data.m_buf = static_cast<char*>(malloc(zk_data.m_len));
    ASSERT_TRUE(mock_app.SerializeToArray(zk_data.m_buf, zk_data.m_len));
    zk_node.data = zk_data;
    app_list.push_back(&zk_node);

    mock_app.set_name("other");
    ZkNode zk_node_other;
    ZkNodeData zk_data_other;
    zk_data_other.m_len = mock_app.ByteSize();
    zk_data_other.m_buf = static_cast<char*>(malloc(zk_data_other.m_len));
    ASSERT_TRUE(mock_app.SerializeToArray(zk_data_other.m_buf, zk_data_other.m_len));
    zk_node_other.data = zk_data_other;
    app_list.push_back(&zk_node_other);


    EXPECT_CALL(*mock_zk_client_, Connect())
    .WillOnce(Return(error::ZK_CONNECT_ERROR));
    //  .WillOnce(Return(error::OK));
    EXPECT_CALL(*mock_zk_client_, GetNodeChildren(_, _))
    .WillOnce(DoAll(SetArgPointee<1>(app_list),
                    Return(error::OK)));

    Application app;
    std::string app_name = "other";

    // test of connect zk fail
    zk_->m_connected = false;
    ASSERT_NE(error::OK, zk_->GetApp(app_name, 1, 0, &app));

    zk_->m_connected = true;
    ASSERT_EQ(error::OK, zk_->GetApp(app_name, 1, 1, &app));

    //NOTICE. we can not deallocate this buf which may make core
    //a memory leak here
    //free(zk_data.m_buf);
    zk_->m_connected = false;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_GetApp_suite, TestGetApp)
{
    TestGetApp();
}

/**
 * @brief
**/
class test_ZK_GetAppID_suite : public ::testing::Test {
protected:
    test_ZK_GetAppID_suite() {};
    virtual ~test_ZK_GetAppID_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZK_GetAppID_suite, *)
        zk_ = new (std::nothrow) ZK();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZK_GetAppID_suite, *)
        if (NULL != zk_) {
            delete zk_;
            zk_ = NULL;
        }
    };
    void TestGetAppID() {
        std::string name = "test";
        int64_t i = 1;
        ASSERT_EQ(error::OK, zk_->GetAppID(name, &i));

        zk_->m_connected = false;
    }
private:
    ZK* zk_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZK_GetAppID_suite, TestGetAppID)
{
    TestGetAppID();
}

