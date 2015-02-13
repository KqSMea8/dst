#include <gtest/gtest.h>

#include "processnode/zk_operator.h"
#include "mock_zk_operator.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;

using namespace dstream;
using namespace dstream::router;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_zk_operator");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief
**/
class test_ZkOperator_ZkOperator_suite : public ::testing::Test {
protected:
    test_ZkOperator_ZkOperator_suite() {};
    virtual ~test_ZkOperator_ZkOperator_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_ZkOperator_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_ZkOperator_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_ZkOperator_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_Connect_suite : public ::testing::Test {
protected:
    test_ZkOperator_Connect_suite() {};
    virtual ~test_ZkOperator_Connect_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_Connect_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_Connect_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_Connect_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_IsConnected_suite : public ::testing::Test {
protected:
    test_ZkOperator_IsConnected_suite() {};
    virtual ~test_ZkOperator_IsConnected_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_IsConnected_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_IsConnected_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_IsConnected_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_Disconnect_suite : public ::testing::Test {
protected:
    test_ZkOperator_Disconnect_suite() {};
    virtual ~test_ZkOperator_Disconnect_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_Disconnect_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_Disconnect_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_Disconnect_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_NodeExists_suite : public ::testing::Test {
protected:
    test_ZkOperator_NodeExists_suite() {};
    virtual ~test_ZkOperator_NodeExists_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_NodeExists_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_NodeExists_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_NodeExists_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_GetNodeData_suite : public ::testing::Test {
protected:
    test_ZkOperator_GetNodeData_suite() {};
    virtual ~test_ZkOperator_GetNodeData_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_GetNodeData_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_GetNodeData_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_GetNodeData_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_SetNodeData_suite : public ::testing::Test {
protected:
    test_ZkOperator_SetNodeData_suite() {};
    virtual ~test_ZkOperator_SetNodeData_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_SetNodeData_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_SetNodeData_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_SetNodeData_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_CreatePersistentNode_suite : public ::testing::Test {
protected:
    test_ZkOperator_CreatePersistentNode_suite() {};
    virtual ~test_ZkOperator_CreatePersistentNode_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_CreatePersistentNode_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_CreatePersistentNode_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_CreatePersistentNode_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_DeletePersistentNode_suite : public ::testing::Test {
protected:
    test_ZkOperator_DeletePersistentNode_suite() {};
    virtual ~test_ZkOperator_DeletePersistentNode_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_DeletePersistentNode_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_DeletePersistentNode_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_DeletePersistentNode_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_CreateEphemeralNode_suite : public ::testing::Test {
protected:
    test_ZkOperator_CreateEphemeralNode_suite() {};
    virtual ~test_ZkOperator_CreateEphemeralNode_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_CreateEphemeralNode_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_CreateEphemeralNode_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_CreateEphemeralNode_suite, case_name1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_CreateEphemeralNode_suite, case_name2)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_DeleteEphemeralNode_suite : public ::testing::Test {
protected:
    test_ZkOperator_DeleteEphemeralNode_suite() {};
    virtual ~test_ZkOperator_DeleteEphemeralNode_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_DeleteEphemeralNode_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_DeleteEphemeralNode_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_DeleteEphemeralNode_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkOperator_SetWatchEventCallBack_suite : public ::testing::Test {
protected:
    test_ZkOperator_SetWatchEventCallBack_suite() {};
    virtual ~test_ZkOperator_SetWatchEventCallBack_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkOperator_SetWatchEventCallBack_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkOperator_SetWatchEventCallBack_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkOperator_SetWatchEventCallBack_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_GetZKData_suite : public ::testing::Test {
protected:
    test_GetZKData_suite() {};
    virtual ~test_GetZKData_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_GetZKData_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_GetZKData_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_GetZKData_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_SetZKData_suite : public ::testing::Test {
protected:
    test_SetZKData_suite() {};
    virtual ~test_SetZKData_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_SetZKData_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_SetZKData_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_SetZKData_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkWrapper_ZkWrapper_suite : public ::testing::Test {
protected:
    test_ZkWrapper_ZkWrapper_suite() {};
    virtual ~test_ZkWrapper_ZkWrapper_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_ZkWrapper_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_ZkWrapper_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkWrapper_ZkWrapper_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ZkWrapper_Init_suite : public ::testing::Test {
protected:
    test_ZkWrapper_Init_suite() {};
    virtual ~test_ZkWrapper_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_Init_suite, *)
        // new ZkWrapper
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_Init_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkWrapper_Init_suite, InitAlreadyConnect)
{
    std::string test_config_file = "test_zk_operator.cfg.xml";
    ASSERT_NE(zk_wrapper_, reinterpret_cast<router::ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    config::Config conf;
    ASSERT_EQ(conf.ReadConfig(test_config_file), error::OK);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->Init(conf, NULL, NULL), error::OK);
}

/**
 * @brief
**/
class test_ZkWrapper_Destroy_suite : public ::testing::Test {
protected:
    test_ZkWrapper_Destroy_suite() {};
    virtual ~test_ZkWrapper_Destroy_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_Destroy_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_Destroy_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkWrapper_Destroy_suite, Destroy)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    zk_wrapper_->Destroy();
}

/**
 * @brief
**/
class test_ZkWrapper_GetPM_suite : public ::testing::Test {
protected:
    test_ZkWrapper_GetPM_suite() {};
    virtual ~test_ZkWrapper_GetPM_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_GetPM_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_GetPM_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZkWrapper_GetPM_suite, GetPMOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct pm
    PM pm;
    pm.set_host("test_pm_host");
    pm.set_id("test_pm_id");
    pm.set_submit_port(1234);
    pm.set_report_port(4321);
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(pm.ByteSize()));
    // delete Buf inner zk_wrapper_->GetPM
    ASSERT_TRUE(pm.SerializeToArray(zk_data.m_buf, zk_data.m_len));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    PM target_pm;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPM(&target_pm), error::OK);
    ASSERT_STREQ(target_pm.host().c_str(), pm.host().c_str());
}

TEST_F(test_ZkWrapper_GetPM_suite, GetPMZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    PM target_pm;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPM(&target_pm), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_GetPM_suite, GetPMGetZkDataFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));
    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    PM target_pm;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPM(&target_pm), error::FAILED_EXPECTATION);
}

TEST_F(test_ZkWrapper_GetPM_suite, GetPMParseFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct wrong data
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(1));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    PM target_pm;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPM(&target_pm), error::SERIAIL_DATA_FAIL);
}

class test_ZkWrapper_GetPN_suite : public ::testing::Test {
protected:
    test_ZkWrapper_GetPN_suite() {};
    virtual ~test_ZkWrapper_GetPN_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_GetPN_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_GetPN_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

TEST_F(test_ZkWrapper_GetPN_suite, GetPNOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct pn
    PN pn;
    pn.set_host("test_pn_host");
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(pn.ByteSize()));
    ASSERT_TRUE(pn.SerializeToArray(zk_data.m_buf, zk_data.m_len));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    PN target_pn;
    PNID pnid;
    pnid.set_id("12345");
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPN(pnid, &target_pn), error::OK);
    ASSERT_STREQ(target_pn.host().c_str(), pn.host().c_str());
}

TEST_F(test_ZkWrapper_GetPN_suite, GetPNZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    PN target_pn;
    PNID pnid;
    pnid.set_id("12345");
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPN(pnid, &target_pn), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_GetPN_suite, GetPNGetZkDataFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));
    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    PN target_pn;
    PNID pnid;
    pnid.set_id("12345");
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPN(pnid, &target_pn), error::FAILED_EXPECTATION);
}

TEST_F(test_ZkWrapper_GetPN_suite, GetPNParseFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct wrong data
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(1));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    PN target_pn;
    PNID pnid;
    pnid.set_id("12345");
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetPN(pnid, &target_pn), error::SERIAIL_DATA_FAIL);
}

class test_ZkWrapper_AddPN_suite : public ::testing::Test {
protected:
    test_ZkWrapper_AddPN_suite() {};
    virtual ~test_ZkWrapper_AddPN_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_AddPN_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_AddPN_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

TEST_F(test_ZkWrapper_AddPN_suite, AddPNOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));
    EXPECT_CALL(*mock_operator, CreateEphemeralNode(_, _, _))
    .WillOnce(Return(error::OK));

    PN add_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->AddPN(add_pn), error::OK);
}

TEST_F(test_ZkWrapper_AddPN_suite, AddPNZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    PN add_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->AddPN(add_pn), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_AddPN_suite, AddPNZkFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    EXPECT_CALL(*mock_operator, CreateEphemeralNode(_, _, _))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    PN add_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->AddPN(add_pn), error::FAILED_EXPECTATION);
}

class test_ZkWrapper_DeletePN_suite : public ::testing::Test {
protected:
    test_ZkWrapper_DeletePN_suite() {};
    virtual ~test_ZkWrapper_DeletePN_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_DeletePN_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_DeletePN_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

TEST_F(test_ZkWrapper_DeletePN_suite, DeletePNOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    EXPECT_CALL(*mock_operator, DeleteEphemeralNode(_))
    .WillOnce(Return(error::OK));

    PN del_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->DeletePN(del_pn), error::OK);

}

TEST_F(test_ZkWrapper_DeletePN_suite, DeletePNZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    PN del_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->DeletePN(del_pn), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_DeletePN_suite, DeletePNZkFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    EXPECT_CALL(*mock_operator, DeleteEphemeralNode(_))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    PN del_pn;
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->DeletePN(del_pn), error::FAILED_EXPECTATION);
}

class test_ZkWrapper_GetApplication_suite : public ::testing::Test {
protected:
    test_ZkWrapper_GetApplication_suite() {};
    virtual ~test_ZkWrapper_GetApplication_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_GetApplication_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_GetApplication_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

TEST_F(test_ZkWrapper_GetApplication_suite, GetApplicationOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct app
    Application app;
    app.set_name("test_app");
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(app.ByteSize()));
    ASSERT_TRUE(app.SerializeToArray(zk_data.m_buf, zk_data.m_len));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    Application target_app;
    AppID appid;
    appid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetApplication(appid, &target_app), error::OK);
    ASSERT_STREQ(target_app.name().c_str(), app.name().c_str());
}

TEST_F(test_ZkWrapper_GetApplication_suite, GetApplicationZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    Application target_app;
    AppID appid;
    appid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetApplication(appid, &target_app), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_GetApplication_suite, GetApplicationGetZkDataFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));
    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    Application target_app;
    AppID appid;
    appid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetApplication(appid, &target_app), error::FAILED_EXPECTATION);
}

TEST_F(test_ZkWrapper_GetApplication_suite, GetApplicationParseFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct wrong data
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(1));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    Application target_app;
    AppID appid;
    appid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetApplication(appid, &target_app), error::SERIAIL_DATA_FAIL);
}

class test_ZkWrapper_GetProcessorElement_suite : public ::testing::Test {
protected:
    test_ZkWrapper_GetProcessorElement_suite() {};
    virtual ~test_ZkWrapper_GetProcessorElement_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ZkWrapper_GetProcessorElement_suite, *)
        zk_wrapper_ = new (std::nothrow) router::ZkWrapper();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ZkWrapper_GetProcessorElement_suite, *)
        delete zk_wrapper_;
    };
private:
    router::ZkWrapper* zk_wrapper_;
};

TEST_F(test_ZkWrapper_GetProcessorElement_suite, GetProcessorElementOK)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct pe
    ProcessorElement pe;
    pe.set_serial(2012);
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(pe.ByteSize()));
    ASSERT_TRUE(pe.SerializeToArray(zk_data.m_buf, zk_data.m_len));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    ProcessorElement target_pe;
    AppID appid;
    appid.set_id(12);
    PEID peid;
    peid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetProcessorElement(appid, peid, &target_pe), error::OK);
    ASSERT_EQ(target_pe.serial(), pe.serial());
}

TEST_F(test_ZkWrapper_GetProcessorElement_suite, GetProcessorElementZkNotConnect)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(false));

    ProcessorElement target_pe;
    AppID appid;
    appid.set_id(12);
    PEID peid;
    peid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetProcessorElement(appid, peid, &target_pe), error::CONNECT_ZK_FAIL);
}

TEST_F(test_ZkWrapper_GetProcessorElement_suite, GetProcessorElementGetZkDataFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));
    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION));

    ProcessorElement target_pe;
    AppID appid;
    appid.set_id(12);
    PEID peid;
    peid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetProcessorElement(appid, peid, &target_pe), error::FAILED_EXPECTATION);
}

TEST_F(test_ZkWrapper_GetProcessorElement_suite, GetProcessorElemenParseFail)
{
    ASSERT_NE(zk_wrapper_, reinterpret_cast<ZkWrapper*>(NULL));

    MockZkOperator* mock_operator = new (std::nothrow) MockZkOperator();
    ASSERT_NE(mock_operator, reinterpret_cast<MockZkOperator*>(NULL));
    EXPECT_CALL(*mock_operator, IsConnected())
    .WillOnce(Return(true));

    // contruct wrong data
    zk_meta_manager::MetaZKData zk_data;
    ASSERT_TRUE(zk_data.AllocateBuf(1));

    EXPECT_CALL(*mock_operator, GetNodeData(_, _))
    .WillOnce(DoAll(SetArgReferee<1>(&zk_data), Return(error::OK)));

    ProcessorElement target_pe;
    AppID appid;
    appid.set_id(12);
    PEID peid;
    peid.set_id(12345);
    zk_wrapper_->m_zk_operator = ZkOperator::ZkOperatorPtr(mock_operator);
    ASSERT_EQ(zk_wrapper_->GetProcessorElement(appid, peid, &target_pe), error::SERIAIL_DATA_FAIL);
}

