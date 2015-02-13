#include "common/logger.h"
#include "client/action.h"
#include "client/dstream_client.h"
#include "mock_pm_client.h"
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::An;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;

using namespace dstream;
using namespace client;


int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_action");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief  no need to test
**/
/*class test_Action_Action_suite : public ::testing::Test{
    protected:
        test_Action_Action_suite(){};
        virtual ~test_Action_Action_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_Action_Action_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_Action_Action_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_Action_Action_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief  no need to test
**/
class test_Action_SetAction_suite : public ::testing::Test {
protected:
    test_Action_SetAction_suite() {};
    virtual ~test_Action_SetAction_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_Action_SetAction_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_Action_SetAction_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_Action_SetAction_suite, TestSetAction)
{
    Action action;
    ASSERT_EQ(error::BAD_ARGUMENT, action.SetAction(NULL));
    ASSERT_EQ(error::CLIENT_NOT_SUPPORTED, action.SetAction("not support"));

    ASSERT_EQ(error::OK, action.SetAction("submit-app"));
}

/**
 * @brief  no need to test
**/
/*class test_Action_Run_suite : public ::testing::Test{
    protected:
        test_Action_Run_suite(){};
        virtual ~test_Action_Run_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_Action_Run_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_Action_Run_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_Action_Run_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
class test_CheckCommand_suite : public ::testing::Test {
protected:
    test_CheckCommand_suite() {};
    virtual ~test_CheckCommand_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CheckCommand_suite, *)
        g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CheckCommand_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CheckCommand_suite, EmptyCfg) {
    g_cmd_line_args.cfg_file = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, EmptyAction) {
    g_cmd_line_args.action = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, AppDIREmpty) {
    g_cmd_line_args.action = "submit-app";
    g_cmd_line_args.app_dir = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, AppIDorNameEmpty) {
    g_cmd_line_args.action = "kill-app";
    g_cmd_line_args.app_id = -1;
    g_cmd_line_args.app_name = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, UpdateUserEmpty) {
    g_cmd_line_args.action = "update-user";
    g_cmd_line_args.username = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, AddUserEmpty) {
    g_cmd_line_args.action = "add-user";
    g_cmd_line_args.username = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, DelUserEmpty) {
    g_cmd_line_args.action = "del-user";
    g_cmd_line_args.username = "";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, FlowControlArgs) {
    // no args
    g_cmd_line_args = CommandLineArgs();
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    g_cmd_line_args.action = "flow-control";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, FlowControlArgs2) {
    // miss appid or appname
    g_cmd_line_args = CommandLineArgs();
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    g_cmd_line_args.action = "flow-control";
    g_cmd_line_args.cmd = "sTart";
    g_cmd_line_args.processor = "fake-importer";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, FlowControlArgs3) {
    // miss processor name
    g_cmd_line_args = CommandLineArgs();
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    g_cmd_line_args.action = "flow-control";

    g_cmd_line_args.app_id = 1;
    g_cmd_line_args.cmd = "sTart";

    ASSERT_EQ(CheckCommand(), error::OK);
}

TEST_F(test_CheckCommand_suite, FlowControlArgs4) {
    // miss cmd
    g_cmd_line_args = CommandLineArgs();
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    g_cmd_line_args.action = "flow-control";
    g_cmd_line_args.app_id = 1;
    g_cmd_line_args.processor = "fake-importer";
    ASSERT_EQ(CheckCommand(), error::BAD_ARGUMENT);
}

TEST_F(test_CheckCommand_suite, FlowControlArgs5) {
    // OK
    std::cout << "============================" << std::endl;
    g_cmd_line_args = CommandLineArgs();
    g_cmd_line_args.cfg_file = "./dstream.cfg.xml";
    g_cmd_line_args.action = "flow-control";
    g_cmd_line_args.app_id = 1;
    g_cmd_line_args.cmd = "sTart";
    g_cmd_line_args.processor = "fake-importer";
    ASSERT_EQ(CheckCommand(), error::OK);
}

/**
 * @brief
**/
class test_CommandParse_suite : public ::testing::Test {
protected:
    test_CommandParse_suite() {};
    virtual ~test_CommandParse_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CommandParse_suite, *)
        argv[0] = const_cast<char*>("./dstream_client");
        argv[1] = const_cast<char*>("--conf");
        argv[2] = const_cast<char*>("./dstream.cfg.xml");
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CommandParse_suite, *)
    };
public:
    int argc;
    char* argv[8];
};

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_CommandParse_suite, VersionTest) {
  argc = 2;
  argv[1] = const_cast<char*>("--version");
  ASSERT_EQ(CommandParse(argc, argv), error::OK);
}*/

TEST_F(test_CommandParse_suite, SubmitAppTest) {
    argc = 6;
    argv[3] = const_cast<char*>("--submit-app");
    argv[4] = const_cast<char*>("-d");
    argv[5] = const_cast<char*>("app_dir");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, CheckTopologyTest) {
    argc = 6;
    argv[3] = const_cast<char*>("--check-topology");
    argv[4] = const_cast<char*>("-d");
    argv[5] = const_cast<char*>("app_dir");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, DecommissionAppTest) {
    argc = 6;
    argv[3] = const_cast<char*>("--kill-app");
    argv[4] = const_cast<char*>("--app");
    argv[5] = const_cast<char*>("1");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, DecommissionAppNameTest) {
    argc = 6;
    argv[3] = const_cast<char*>("--kill-app");
    argv[4] = const_cast<char*>("--app");
    argv[5] = const_cast<char*>("test");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, DecommissionAppNameFailTest) {
    argc = 6;
    argv[3] = const_cast<char*>("--kill-app");
    argv[4] = const_cast<char*>("--app");
    argv[5] = const_cast<char*>("1test");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, UpdateUserTest) {
    argc = 8;
    argv[3] = const_cast<char*>("--update-user");
    argv[4] = const_cast<char*>("--username");
    argv[5] = const_cast<char*>("test");
    argv[6] = const_cast<char*>("--password");
    argv[7] = const_cast<char*>("test");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, FlowControlTest) {
    argc = 8;
    argv[3] = const_cast<char*>("--flow-control");
    argv[4] = const_cast<char*>("--app");
    argv[5] = const_cast<char*>("test");
    argv[6] = const_cast<char*>("--cmd");
    argv[7] = const_cast<char*>("start");
    ASSERT_EQ(CommandParse(argc, argv), error::OK);
}

TEST_F(test_CommandParse_suite, UnknownTest) {
    argc = 2;
    argv[3] = const_cast<char*>("--wrong");
    ASSERT_EQ(CommandParse(argc, argv), error::BAD_ARGUMENT);
}
/**
 * @brief
**/
class test_DefaultAct_suite : public ::testing::Test {
protected:
    test_DefaultAct_suite() {};
    virtual ~test_DefaultAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DefaultAct_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DefaultAct_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DefaultAct_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_CheckTopologyAct_suite : public ::testing::Test {
protected:
    test_CheckTopologyAct_suite() {};
    virtual ~test_CheckTopologyAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CheckTopologyAct_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CheckTopologyAct_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CheckTopologyAct_suite, TestCheckTopologyAct)
{
    g_cmd_line_args.app_dir = "";
    ASSERT_NE(error::OK, CheckTopologyAct());

    g_cmd_line_args.app_dir = "topo_check_test_correct_complicated_topo";
    ASSERT_EQ(error::OK, CheckTopologyAct());


}

/**
 * @brief
**/
class test_SubmitAppAct_suite : public ::testing::Test {
protected:
    test_SubmitAppAct_suite() {};
    virtual ~test_SubmitAppAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_SubmitAppAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_SubmitAppAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestSubmitAppAct();
private:
    MockPMClient* mock_pc;
};

void test_SubmitAppAct_suite::TestSubmitAppAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_dir = "";
    Application app;
    app.mutable_id()->set_id(static_cast<uint64_t>(-1));
    Application app_existed;
    app_existed.mutable_id()->set_id(static_cast<uint64_t>(1));

    // test of get app name fail
    ASSERT_NE(error::OK, SubmitAppAct());

    EXPECT_CALL(*mock_pc, GetAppTopology(_, _))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL))
    .WillOnce(DoAll(SetArgReferee<1>(app_existed),
                    Return(error::OK)))
    .WillRepeatedly(DoAll(SetArgReferee<1>(app),
                          Return(error::OK)));
    EXPECT_CALL(*mock_pc, SubmitApp(_))
    .WillOnce(Return(error::OK))
    .WillRepeatedly(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of get app topology fail
    g_cmd_line_args.app_dir = "topo_check_test_correct_complicated_topo";
    ASSERT_NE(error::OK, SubmitAppAct());

    // test of already existed
    g_cmd_line_args.app_dir = "topo_check_test_correct_complicated_topo";
    ASSERT_NE(error::OK, SubmitAppAct());

    // test of build app fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo";
    ASSERT_EQ(error::CLIENT_SUBMIT_APP_FAIL, SubmitAppAct());

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_complicated_topo";
    ASSERT_EQ(error::OK, SubmitAppAct());

    // test of submit app fail
    ASSERT_NE(error::OK, SubmitAppAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_SubmitAppAct_suite, TestSubmitAppAct)
{
    TestSubmitAppAct();
}

/**
 * @brief
**/
class test_DecommissionAppAct_suite : public ::testing::Test {
protected:
    test_DecommissionAppAct_suite() {};
    virtual ~test_DecommissionAppAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DecommissionAppAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DecommissionAppAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestKillAppAct();
private:
    MockPMClient* mock_pc;
};

void test_DecommissionAppAct_suite::TestKillAppAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, DecommissionApp(An<std::string>()))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_FLOW_CONTROL_FAIL));
    EXPECT_CALL(*mock_pc, DecommissionApp(An<int64_t>()))
    .WillOnce(Return(error::OK));

    ASSERT_EQ(error::OK, DecommissionAppAct());

    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, DecommissionAppAct());

    g_cmd_line_args.app_name = "";
    ASSERT_EQ(error::OK, DecommissionAppAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DecommissionAppAct_suite, TestKillAppAct)
{
    TestKillAppAct();
}

/**
 * @brief
**/
/*class test_DecommissionProcessorAct_suite : public ::testing::Test{
    protected:
        test_DecommissionProcessorAct_suite(){};
        virtual ~test_DecommissionProcessorAct_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_DecommissionProcessorAct_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_DecommissionProcessorAct_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_DecommissionProcessorAct_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
/*class test_RestartProcessorAct_suite : public ::testing::Test{
    protected:
        test_RestartProcessorAct_suite(){};
        virtual ~test_RestartProcessorAct_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RestartProcessorAct_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RestartProcessorAct_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_RestartProcessorAct_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
/*class test_MigratePeAct_suite : public ::testing::Test{
    protected:
        test_MigratePeAct_suite(){};
        virtual ~test_MigratePeAct_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_MigratePeAct_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_MigratePeAct_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_MigratePeAct_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
class test_GetUserAct_suite : public ::testing::Test {
protected:
    test_GetUserAct_suite() {};
    virtual ~test_GetUserAct_suite() {};
    virtual void SetUp() {
        //Called after every TEST_F(test_FlowControlAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_FlowControlAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestGetUserAct();
private:
    MockPMClient* mock_pc;
};

void test_GetUserAct_suite::TestGetUserAct() {
    USER_SET userinfo;
    User user;
    user.set_username("test");
    user.set_password("test");
    userinfo.insert(user);

    g_pc = reinterpret_cast<PMClient*>(mock_pc);

    EXPECT_CALL(*mock_pc, GetUser())
    .WillOnce(Return(userinfo));

    ASSERT_EQ(error::OK, GetUserAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_GetUserAct_suite, TestGetUserAct)
{
    TestGetUserAct();
}

/**
 * @brief
**/
class test_UpdateUserAct_suite : public ::testing::Test {
protected:
    test_UpdateUserAct_suite() {};
    virtual ~test_UpdateUserAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_UpdateUserAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_UpdateUserAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestUpdateUserAct();
private:
    MockPMClient* mock_pc;
};

void test_UpdateUserAct_suite::TestUpdateUserAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";
    USER_SET userinfo;
    User user;
    user.set_username("test");
    user.set_password("test");
    userinfo.insert(user);

    EXPECT_CALL(*mock_pc, UpdateUser(_, _))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_USER_OP_FAIL));
    EXPECT_CALL(*mock_pc, GetUser())
    .WillOnce(Return(userinfo));

    ASSERT_EQ(error::OK, UpdateUserAct());
    ASSERT_EQ(error::CLIENT_USER_OP_FAIL, UpdateUserAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_UpdateUserAct_suite, TestUpdateUserAct)
{
    TestUpdateUserAct();
}

/**
 * @brief
**/
class test_AddUserAct_suite : public ::testing::Test {
protected:
    test_AddUserAct_suite() {};
    virtual ~test_AddUserAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AddUserAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AddUserAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestAddUserAct();
private:
    MockPMClient* mock_pc;
};

void test_AddUserAct_suite::TestAddUserAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";
    USER_SET userinfo;
    User user;
    user.set_username("test");
    user.set_password("test");
    userinfo.insert(user);

    EXPECT_CALL(*mock_pc, AddUser(_, _))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_USER_OP_FAIL));
    EXPECT_CALL(*mock_pc, GetUser())
    .WillOnce(Return(userinfo));

    ASSERT_EQ(error::OK, AddUserAct());
    ASSERT_EQ(error::CLIENT_USER_OP_FAIL, AddUserAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AddUserAct_suite, TestAddUserAct)
{
    TestAddUserAct();
}

/**
 * @brief
**/
class test_DelUserAct_suite : public ::testing::Test {
protected:
    test_DelUserAct_suite() {};
    virtual ~test_DelUserAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelUserAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DelUserAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestDelUserAct();
private:
    MockPMClient* mock_pc;
};

void test_DelUserAct_suite::TestDelUserAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";
    USER_SET userinfo;
    User user;
    user.set_username("test");
    user.set_password("test");
    userinfo.insert(user);

    EXPECT_CALL(*mock_pc, DeleteUser(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_USER_OP_FAIL));
    EXPECT_CALL(*mock_pc, GetUser())
    .WillOnce(Return(userinfo));

    ASSERT_EQ(error::OK, DelUserAct());
    ASSERT_EQ(error::CLIENT_USER_OP_FAIL, DelUserAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DelUserAct_suite, TestDelUserAct)
{
    TestDelUserAct();
}

/**
 * @brief
**/
class test_AddSubTreeAct_suite : public ::testing::Test {
protected:
    test_AddSubTreeAct_suite() {};
    virtual ~test_AddSubTreeAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelSubTreeAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AddImporterAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestAddSubTreeAct();
private:
    MockPMClient* mock_pc;
};

void test_AddSubTreeAct_suite::TestAddSubTreeAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, AddSubTree(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_add_sub_tree";
    ASSERT_EQ(error::OK, AddSubTreeAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, AddSubTreeAct());

    // test of init app fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo";
    ASSERT_NE(error::OK, AddSubTreeAct());

    // test of check topology fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo_without_subscribes";
    ASSERT_NE(error::OK, AddSubTreeAct());
}


/**
 * @brief
 * @begin_version
**/
TEST_F(test_AddSubTreeAct_suite, TestAddSubTreeAct)
{
    TestAddSubTreeAct();
}

/**
 * @brief
**/
class test_DelSubTreeAct_suite : public ::testing::Test {
protected:
    test_DelSubTreeAct_suite() {};
    virtual ~test_DelSubTreeAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelSubTreeAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AddImporterAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestDelSubTreeAct();
private:
    MockPMClient* mock_pc;
};

void test_DelSubTreeAct_suite::TestDelSubTreeAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, DelSubTree(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_del_sub_tree";
    ASSERT_EQ(error::OK, DelSubTreeAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, DelSubTreeAct());

    // test of init app fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo";
    ASSERT_NE(error::OK, DelSubTreeAct());

    // test of check topology fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo_with_circles";
    ASSERT_NE(error::OK, DelSubTreeAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DelSubTreeAct_suite, TestDelSubTreeAct)
{
    TestDelSubTreeAct();
}

/**
 * @brief
**/
class test_AddImporterAct_suite : public ::testing::Test {
protected:
    test_AddImporterAct_suite() {};
    virtual ~test_AddImporterAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AddImporterAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AddImporterAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestAddImporterAct();
private:
    MockPMClient* mock_pc;
};

void test_AddImporterAct_suite::TestAddImporterAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, AddImporter(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_add_importer";
    ASSERT_EQ(error::OK, AddImporterAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, AddImporterAct());

    // test of init app fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo";
    ASSERT_NE(error::OK, AddImporterAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AddImporterAct_suite, TestAddImporterAct)
{
    TestAddImporterAct();
}

/**
 * @brief
**/
class test_DelImporterAct_suite : public ::testing::Test {
protected:
    test_DelImporterAct_suite() {};
    virtual ~test_DelImporterAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelImporterAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DelImporterAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestDelImporterAct();
private:
    MockPMClient* mock_pc;
};

void test_DelImporterAct_suite::TestDelImporterAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, DelImporter(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_del_importer";
    ASSERT_EQ(error::OK, DelImporterAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, DelImporterAct());

    // test of init app fail
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_topo";
    ASSERT_NE(error::OK, DelImporterAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DelImporterAct_suite, TestDelImporterAct)
{
    TestDelImporterAct();
}

/**
 * @brief
**/
class test_UpdateParallelismAct_suite : public ::testing::Test {
protected:
    test_UpdateParallelismAct_suite() {};
    virtual ~test_UpdateParallelismAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_UpdateParallelismAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_UpdateParallelismAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestUpdateParaAct();
private:
    MockPMClient* mock_pc;
};

void test_UpdateParallelismAct_suite::TestUpdateParaAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, UpdateParallelism(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_add_para";
    ASSERT_EQ(error::OK, UpdateParallelismAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, UpdateParallelismAct());

    // test of init app fail
    g_cmd_line_args.app_dir = "";
    ASSERT_NE(error::OK, UpdateParallelismAct());

    // test of up importer para
    g_cmd_line_args.app_dir = "topo_check_test_incorrect_add_para";
    ASSERT_EQ(error::CLIENT_UPDATE_PARA_FAIL, UpdateParallelismAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_UpdateParallelismAct_suite, TestUpdateParaAct)
{
    TestUpdateParaAct();
}

/**
 * @brief
**/
class test_FlowControlAct_suite : public ::testing::Test {
protected:
    test_FlowControlAct_suite() {};
    virtual ~test_FlowControlAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_FlowControlAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_FlowControlAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestFlowControlAct();
private:
    MockPMClient* mock_pc;
};

void test_FlowControlAct_suite::TestFlowControlAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, FlowControlApp(An<std::string>(), _, _))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_FLOW_CONTROL_FAIL));
    EXPECT_CALL(*mock_pc, FlowControlApp(An<int64_t>(), _, _))
    .WillOnce(Return(error::OK));

    ASSERT_EQ(error::OK, FlowControlAct());

    ASSERT_EQ(error::CLIENT_FLOW_CONTROL_FAIL, FlowControlAct());

    g_cmd_line_args.app_name = "";
    ASSERT_EQ(error::OK, FlowControlAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_FlowControlAct_suite, TestFlowControlAct)
{
    TestFlowControlAct();
}

/**
 * @brief
**/
class test_UpdateProcessorAct_suite : public ::testing::Test {
protected:
    test_UpdateProcessorAct_suite() {};
    virtual ~test_UpdateProcessorAct_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_UpdateProcessorAct_suite, *)
        mock_pc = new (std::nothrow) MockPMClient();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_UpdateProcessorAct_suite, *)
        if (mock_pc != NULL) {
            delete mock_pc;
            mock_pc = NULL;
        }
    };
    void TestUpdateProcessorAct();
private:
    MockPMClient* mock_pc;
};

void test_UpdateProcessorAct_suite::TestUpdateProcessorAct() {
    g_pc = reinterpret_cast<PMClient*>(mock_pc);
    g_cmd_line_args.app_name = "test";

    EXPECT_CALL(*mock_pc, UpdateProcessor(_))
    .WillOnce(Return(error::OK))
    .WillOnce(Return(error::CLIENT_NOTIFY_PM_FAIL));

    // test of OK
    g_cmd_line_args.app_dir = "topo_check_test_correct_update_processor";
    ASSERT_EQ(error::OK, UpdateProcessorAct());

    // test of g_pc fail
    ASSERT_EQ(error::CLIENT_NOTIFY_PM_FAIL, UpdateProcessorAct());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_UpdateProcessorAct_suite, TestUpdateProcessorAct)
{
    TestUpdateProcessorAct();
}
