#include <gtest/gtest.h>

#include "processnode/app_router.h"
#include "mock_zk_wrapper.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;

using namespace dstream;
using namespace dstream::router;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_app_router");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief
**/
class test_AppRouter_Init_suite : public ::testing::Test {
protected:
    test_AppRouter_Init_suite() {};
    virtual ~test_AppRouter_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_Init_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_Init_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_Init_suite, InitConfigFail)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ASSERT_NE(error::OK, app_router_->Init(1, "not_exist_file"));
}


/**
 * @brief
**/
class test_AppRouter_Destroy_suite : public ::testing::Test {
protected:
    test_AppRouter_Destroy_suite() {};
    virtual ~test_AppRouter_Destroy_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_Destroy_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_Destroy_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_Destroy_suite, AppRouterDestroy)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));
}

/**
 * @brief
**/
class test_AppRouter_AddPE_suite : public ::testing::Test {
protected:
    test_AppRouter_AddPE_suite() {};
    virtual ~test_AppRouter_AddPE_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_AddPE_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_AddPE_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_AddPE_suite, AppRouterAddPE)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ASSERT_EQ(app_router_->AddPE(12345), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 1u);
    // add again
    ASSERT_EQ(app_router_->AddPE(12345), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 1u);
}

/**
 * @brief
**/
class test_AppRouter_DelPE_suite : public ::testing::Test {
protected:
    test_AppRouter_DelPE_suite() {};
    virtual ~test_AppRouter_DelPE_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_DelPE_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_DelPE_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_DelPE_suite, AppRouterDelPE)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    AppPE* t_apppe_ptr = new (std::nothrow) AppPE();
    ASSERT_TRUE(t_apppe_ptr);
    app_router_->m_all_watch_pes.insert(AppRouter::WatchPEPair(12345, t_apppe_ptr));

    // delete none
    ASSERT_EQ(app_router_->DelPE(12345), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 0u);
    // delete one
    ASSERT_EQ(app_router_->AddPE(12345), error::OK);
    ASSERT_EQ(app_router_->AddPE(54321), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 2u);

    ASSERT_EQ(app_router_->DelPE(12345), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 1u);
    ASSERT_EQ(app_router_->DelPE(54321), error::OK);
    ASSERT_EQ(app_router_->m_local_pes.size(), 0u);
}

/**
 * @brief
**/
class test_AppRouter_FlowCtrlApplication_L_suite : public ::testing::Test {
protected:
    test_AppRouter_FlowCtrlApplication_L_suite() {};
    virtual ~test_AppRouter_FlowCtrlApplication_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
**/
class test_AppRouter_GetPEList_suite : public ::testing::Test {
protected:
    test_AppRouter_GetPEList_suite() {};
    virtual ~test_AppRouter_GetPEList_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_GetPEList_suite, GetPEListOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    std::vector<std::string> peid_list;
    peid_list.push_back("1");
    peid_list.push_back("2");
    peid_list.push_back("3");

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPEList(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgumentPointee<1>(peid_list), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    PESet target_list;
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ASSERT_EQ(error::OK, app_router_->GetPEList(1, &target_list));
    ASSERT_EQ(target_list.size(), 3u);
}

TEST_F(test_AppRouter_GetPEList_suite, GetPEListParsePEFail)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    std::vector<std::string> peid_list;
    peid_list.push_back("0");
    peid_list.push_back("1");
    peid_list.push_back("2");

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPEList(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(peid_list), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    PESet target_list;
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ASSERT_EQ(error::OK, app_router_->GetPEList(1, &target_list));
    ASSERT_EQ(target_list.size(), 2u);
}

TEST_F(test_AppRouter_GetPEList_suite, GetPEListNoNodeFail)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPEList(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    PESet target_list;
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ASSERT_EQ(error::ZK_NO_NODE, app_router_->GetPEList(1, &target_list));
    ASSERT_EQ(target_list.size(), 0u);
}



/**
 * @brief
**/
class test_AppRouter_GetPNAddr_suite : public ::testing::Test {
protected:
    test_AppRouter_GetPNAddr_suite() {};
    virtual ~test_AppRouter_GetPNAddr_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetPNAddr_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_GetPNAddr_suite, GetPNAddrOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(12345);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(12345);

    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    std::string target_addr = "";
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ASSERT_EQ(error::OK, app_router_->GetPNAddr(pe, &target_addr));
    // TODO: sub_port + 10
    ASSERT_STREQ(target_addr.c_str(), "test_pn_host:7890");
}

TEST_F(test_AppRouter_GetPNAddr_suite, GetPNAddrNotAllocate)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ProcessorElement pe;
    std::string target_addr = "test";
    ASSERT_EQ(error::OK, app_router_->GetPNAddr(pe, &target_addr));
    ASSERT_STREQ(target_addr.c_str(), "");
}


TEST_F(test_AppRouter_GetPNAddr_suite, GetPNAddrZKNoNode)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(12345);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(12345);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    std::string target_addr = "test";
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ASSERT_EQ(error::ZK_NO_NODE, app_router_->GetPNAddr(pe, &target_addr));
    ASSERT_STREQ(target_addr.c_str(), "");
}

/**
 * @brief
**/
class test_AppRouter_GetApplication_suite : public ::testing::Test {
protected:
    test_AppRouter_GetApplication_suite() {};
    virtual ~test_AppRouter_GetApplication_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetApplication_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetApplication_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_GetApplication_suite, GetApplicationOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application app;
    app.mutable_id()->set_id(12345);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetApplication(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<1>(app), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    Application target_app;
    ASSERT_EQ(error::OK, app_router_->GetApplication(12345, &target_app));
    ASSERT_EQ(target_app.id().id(), app.id().id());
}

TEST_F(test_AppRouter_GetApplication_suite, GetApplicationZKNoNode)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetApplication(_, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    Application target_app;
    ASSERT_EQ(error::ZK_NO_NODE, app_router_->GetApplication(12345, &target_app));
}

/**
 * @brief
**/
class test_AppRouter_AddDelWatchPE_L_suite : public ::testing::Test {
protected:
    test_AppRouter_AddDelWatchPE_L_suite() {};
    virtual ~test_AppRouter_AddDelWatchPE_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_AddWatchPE_L_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_AddWatchPE_L_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_AddDelWatchPE_L_suite, AddWatchPE_LOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    uint64_t add_w_peid = 1;

    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);

    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    std::set<uint64_t> w_peids;
    w_peids.insert(6);
    w_peids.insert(7);
    ASSERT_EQ(app_router_->AddWatchPE_L(add_w_peid, w_peids), error::OK);
    ASSERT_EQ(app_router_->m_all_watch_pes.size(), 1u);
    ASSERT_EQ(app_router_->m_all_watch_pes[add_w_peid]->watching_pes.size(), 2u);

    // add exist PEID
    w_peids.clear();
    w_peids.insert(7);
    w_peids.insert(8);
    ASSERT_EQ(app_router_->AddWatchPE_L(add_w_peid, w_peids), error::OK);
    ASSERT_EQ(app_router_->m_all_watch_pes.size(), 1u);
    ASSERT_EQ(app_router_->m_all_watch_pes[add_w_peid]->watching_pes.size(), 3u);

    ASSERT_EQ(app_router_->DelWatchPE_L(add_w_peid, 6), error::OK);
    ASSERT_EQ(app_router_->DelWatchPE_L(add_w_peid, 7), error::OK);
    ASSERT_EQ(app_router_->DelWatchPE_L(add_w_peid, 8), error::OK);
    ASSERT_EQ(app_router_->m_all_watch_pes.size(), 0u);
}

TEST_F(test_AppRouter_AddDelWatchPE_L_suite, AddWatchPE_LZKNoNode)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    uint64_t add_w_peid = 1;

    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    // EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    //  .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    std::set<uint64_t> w_peids;
    w_peids.insert(6);
    w_peids.insert(7);
    // get PE fail
    ASSERT_EQ(app_router_->AddWatchPE_L(add_w_peid, w_peids), error::OK);
}

TEST_F(test_AppRouter_AddDelWatchPE_L_suite, AddWatchPE_LGetPNAddrFail)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    uint64_t add_w_peid = 1;

    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    std::set<uint64_t> w_peids;
    w_peids.insert(6);
    w_peids.insert(7);
    ASSERT_EQ(app_router_->AddWatchPE_L(add_w_peid, w_peids), error::OK);
    ASSERT_EQ(app_router_->m_all_watch_pes.size(), 1u);
    ASSERT_EQ(app_router_->m_all_watch_pes[add_w_peid]->watching_pes.size(), 2u);

    ASSERT_EQ(app_router_->DelWatchPE_L(add_w_peid, 6), error::OK);
    ASSERT_EQ(app_router_->DelWatchPE_L(add_w_peid, 7), error::OK);
    ASSERT_EQ(app_router_->m_all_watch_pes.size(), 0u);
}



void ConstructSimpleApp(Application& app, int process_num, int paralism)
{
    // timeval now;
    // gettimeofday(&now, NULL);
    // app.mutable_id()->set_id(now.tv_usec);
    app.mutable_id()->set_id(1);
    app.set_name("test");
    app.set_descr("used for test");
    User* app_user = app.mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app.mutable_topology();
    topology->set_processor_num(process_num);
    char process_name[1000];
    for (int i = 0; i < process_num; ++i) {
        Processor* processor = topology->add_processors();
        sprintf(process_name, "test_process_%d", i);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        if (i == 0) {
            processor->set_role(IMPORTER);
        }
        else if (i == process_num - 1) {
            processor->set_role(EXPORTER);
        }
        else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(paralism);
        if (i > 0) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            sprintf(process_name, "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
    }
    app.set_status(RUN);
}

void ConstructComplexApp(Application& app, int process_num, int paralism)
{
    // timeval now;
    // gettimeofday(&now, NULL);
    // app.mutable_id()->set_id(now.tv_usec);
    app.mutable_id()->set_id(1);
    app.set_name("test");
    app.set_descr("used for test");
    User* app_user = app.mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app.mutable_topology();
    topology->set_processor_num(process_num);
    char process_name[1000];
    for (int i = 0; i < process_num; ++i) {
        Processor* processor = topology->add_processors();
        sprintf(process_name, "test_process_%d", i);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        if (i == 0 || i == 1) {
            processor->set_role(IMPORTER);
        }
        else if (i == process_num - 1) {
            processor->set_role(EXPORTER);
        }
        else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(paralism);
        if (i > 1) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            sprintf(process_name, "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");

            Subscribe* sub1 = processor->mutable_subscribe();
            ProcessorSub* process_sub1 = sub1->add_processor();
            sprintf(process_name, "test_process_%d", i - 2);
            process_sub1->set_name(process_name);
            process_sub1->set_tags("all1");
            process_sub1->set_username("test");
            process_sub1->set_password("test");
        }
    }
    app.set_status(RUN);
}

const int kMaxProcessorNum = 255;
void ConstructAppForModifyImporter(Application& app, int importer_process_num, int paralism)
{
    if (importer_process_num > kMaxProcessorNum - 2) {
        importer_process_num = kMaxProcessorNum - 2;
    }
    // timeval now;
    // gettimeofday(&now, NULL);
    // app.mutable_id()->set_id(now.tv_usec);
    app.mutable_id()->set_id(1);
    app.set_name("test");
    app.set_descr("used for test");
    User* app_user = app.mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app.mutable_topology();
    topology->set_processor_num(importer_process_num + 2);
    char process_name[1000];
    int pro_id;
    for (pro_id = 0; pro_id < importer_process_num; ++pro_id) {
        Processor* processor = topology->add_processors();
        sprintf(process_name, "test_process_%d", pro_id);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(pro_id);
        // all is importer
        processor->set_role(IMPORTER);
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(paralism);
    }
    // add 2 down processors
    int i;
    int fix_down_pro1 = kMaxProcessorNum - 2;
    Processor* down_processor = topology->add_processors();
    sprintf(process_name, "test_process_%d", fix_down_pro1);
    down_processor->set_name(process_name);
    down_processor->set_descr(process_name);
    down_processor->mutable_id()->set_id(fix_down_pro1);
    down_processor->set_role(EXPORTER);
    down_processor->set_exec_cmd("test_bin");
    Resource* resource = down_processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    down_processor->set_parallism(paralism);
    down_processor->set_backup_num(paralism);
    // sub all up importers
    for (i = 0; i < importer_process_num; ++i) {
        Subscribe* sub = down_processor->mutable_subscribe();
        ProcessorSub* process_sub = sub->add_processor();
        sprintf(process_name, "test_process_%d", i);
        process_sub->set_name(process_name);
        process_sub->set_tags("all");
        process_sub->set_username("test");
        process_sub->set_password("test");
    }

    int fix_down_pro2 = kMaxProcessorNum - 1;
    down_processor = topology->add_processors();
    sprintf(process_name, "test_process_%d", fix_down_pro2);
    down_processor->set_name(process_name);
    down_processor->set_descr(process_name);
    down_processor->mutable_id()->set_id(fix_down_pro2);
    down_processor->set_role(EXPORTER);
    down_processor->set_exec_cmd("test_bin");
    resource = down_processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    down_processor->set_parallism(paralism);
    down_processor->set_backup_num(paralism);
    // sub all up importers
    for (i = 0; i < importer_process_num; ++i) {
        Subscribe* sub = down_processor->mutable_subscribe();
        ProcessorSub* process_sub = sub->add_processor();
        sprintf(process_name, "test_process_%d", i);
        process_sub->set_name(process_name);
        process_sub->set_tags("all");
        process_sub->set_username("test");
        process_sub->set_password("test");
    }

    app.set_status(RUN);
}

void ConstructAppForModifyWorker(Application& app, int worker_num, int paralism)
{
    if (worker_num > kMaxProcessorNum - 2) {
        worker_num = kMaxProcessorNum - 2;
    }
    // timeval now;
    // gettimeofday(&now, NULL);
    // app.mutable_id()->set_id(now.tv_usec);
    app.mutable_id()->set_id(1);
    app.set_name("test");
    app.set_descr("used for test");
    User* app_user = app.mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app.mutable_topology();
    topology->set_processor_num(worker_num + 2);
    char process_name[1000];

    // add 1 up processors
    int i;
    int fix_up_pro = kMaxProcessorNum - 2;
    Processor* down_processor = topology->add_processors();
    sprintf(process_name, "test_process_%d", fix_up_pro);
    down_processor->set_name(process_name);
    down_processor->set_descr(process_name);
    down_processor->mutable_id()->set_id(fix_up_pro);
    down_processor->set_role(IMPORTER);
    down_processor->set_exec_cmd("test_bin");
    Resource* resource = down_processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    down_processor->set_parallism(paralism);
    down_processor->set_backup_num(paralism);

    // add worker_num workers
    int pro_id;
    for (pro_id = 0; pro_id < worker_num; ++pro_id) {
        Processor* processor = topology->add_processors();
        sprintf(process_name, "test_process_%d", pro_id);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(pro_id);
        // all is importer
        processor->set_role(WORK_PROCESSOR);
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(paralism);
        // add sub
        Subscribe* sub = processor->mutable_subscribe();
        ProcessorSub* process_sub = sub->add_processor();
        sprintf(process_name, "test_process_%d", fix_up_pro);
        process_sub->set_name(process_name);
        process_sub->set_tags("all");
        process_sub->set_username("test");
        process_sub->set_password("test");
    }

    // add 1 down processor
    int fix_down_pro = kMaxProcessorNum - 1;
    down_processor = topology->add_processors();
    sprintf(process_name, "test_process_%d", fix_down_pro);
    down_processor->set_name(process_name);
    down_processor->set_descr(process_name);
    down_processor->mutable_id()->set_id(fix_down_pro);
    down_processor->set_role(EXPORTER);
    down_processor->set_exec_cmd("test_bin");
    resource = down_processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    down_processor->set_parallism(paralism);
    down_processor->set_backup_num(paralism);
    // sub all up workers
    for (i = 0; i < worker_num; ++i) {
        Subscribe* sub = down_processor->mutable_subscribe();
        ProcessorSub* process_sub = sub->add_processor();
        sprintf(process_name, "test_process_%d", i);
        process_sub->set_name(process_name);
        process_sub->set_tags("all");
        process_sub->set_username("test");
        process_sub->set_password("test");
    }

    app.set_status(RUN);
}

inline uint64_t ConstructPEID(uint64_t appid, uint64_t proid, uint64_t serial) {
    return MAKE_PEID(appid, proid, serial);
}

/**
 * @brief
 **/
class test_AppRouter_GetWatchDownProcessor_L_suite : public ::testing::Test {
protected:
    test_AppRouter_GetWatchDownProcessor_L_suite() {};
    virtual ~test_AppRouter_GetWatchDownProcessor_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetWatchDownProcessor_L_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetWatchDownProcessor_L_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_GetWatchDownProcessor_L_suite, GetWatchDonwProcessorOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application new_app;
    ConstructSimpleApp(new_app, 5, 3);

    AppRouter::ProcessorMap p_map;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, p_map), error::OK);

    // two watch PEID
    uint64_t pe1 = ConstructPEID(1/*appid*/, 1/*proid*/, 0/*serial*/);
    uint64_t pe2 = ConstructPEID(1/*appid*/, 3/*proid*/, 2/*serial*/);
    ASSERT_EQ(app_router_->AddPE(pe1), error::OK);
    ASSERT_EQ(app_router_->AddPE(pe2), error::OK);

    AppRouter::WPMap down_processors;
    ASSERT_EQ(app_router_->GetWatchDownProcessor_L(p_map, down_processors), error::OK);
    // we concern processor(2,4) for down_processor
    ASSERT_EQ(down_processors.size(), 2u);
    AppRouter::WPMap::iterator find_iter;

    for (find_iter = down_processors.begin();
         find_iter != down_processors.end();
         ++find_iter) {
        DSTREAM_WARN("watch down processorid(%lu)", find_iter->first);
    }

    find_iter = down_processors.find(2);
    ASSERT_TRUE(find_iter != down_processors.end());
    find_iter = down_processors.find(4);
    ASSERT_TRUE(find_iter != down_processors.end());

    // clear
    app_router_->DestroyProcssorMap(&p_map);
    ASSERT_EQ(p_map.size(), 0u);
}

/**
 * @brief
 **/
class test_AppRouter_GetWatchUpProcessor_L_suite : public ::testing::Test {
protected:
    test_AppRouter_GetWatchUpProcessor_L_suite() {};
    virtual ~test_AppRouter_GetWatchUpProcessor_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetWatchUpProcessor_L_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetWatchUpProcessor_L_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_GetWatchUpProcessor_L_suite, GetWatchUpProcessor_LOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application new_app;
    ConstructSimpleApp(new_app, 5, 3);

    AppRouter::ProcessorMap p_map;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, p_map), error::OK);

    // two watch PEID
    uint64_t pe1 = ConstructPEID(1/*appid*/, 1/*proid*/, 0/*serial*/);
    uint64_t pe2 = ConstructPEID(1/*appid*/, 3/*proid*/, 2/*serial*/);
    ASSERT_EQ(app_router_->AddPE(pe1), error::OK);
    ASSERT_EQ(app_router_->AddPE(pe2), error::OK);

    AppRouter::WPMap up_processors;
    ASSERT_EQ(app_router_->GetWatchUpProcessor_L(p_map, up_processors), error::OK);
    // we concern processor(0,2) for up_processor
    ASSERT_EQ(up_processors.size(), 2u);
    AppRouter::WPMap::iterator find_iter;

    for (find_iter = up_processors.begin();
         find_iter != up_processors.end();
         ++find_iter) {
        DSTREAM_WARN("watch up processorid(%lu)", find_iter->first);
    }

    find_iter = up_processors.find(0);
    ASSERT_TRUE(find_iter != up_processors.end());
    find_iter = up_processors.find(2);
    ASSERT_TRUE(find_iter != up_processors.end());

    // clear
    app_router_->DestroyProcssorMap(&p_map);
    ASSERT_EQ(p_map.size(), 0u);

}

/**
 * @brief
 **/
class test_AppRouter_GetProcessorPEIDs_suite : public ::testing::Test {
protected:
    test_AppRouter_GetProcessorPEIDs_suite() {};
    virtual ~test_AppRouter_GetProcessorPEIDs_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetProcessorPEIDs_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetProcessorPEIDs_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_GetProcessorPEIDs_suite, GetProcessorPEIDsOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    PESet pe_list;
    app_router_->GetProcessorPEIDs(1, 1, 0, 10, pe_list);
    ASSERT_EQ(pe_list.size(), 10u);
}

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_GetProcessorPEIDs_suite, GetProcessorPEIDsOK1)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    PESet pe_list;
    app_router_->GetProcessorPEIDs(1, 1, 10, pe_list);
    ASSERT_EQ(pe_list.size(), 10u);
}

/**
 * @brief
 **/
class test_AppRouter_ParseProcessors_suite : public ::testing::Test {
protected:
    test_AppRouter_ParseProcessors_suite() {};
    virtual ~test_AppRouter_ParseProcessors_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_ParseProcessors_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_ParseProcessors_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_ParseProcessors_suite, ParseProcessorsOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application new_app;
    ConstructSimpleApp(new_app, 5, 3);

    AppRouter::ProcessorMap p_map;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, p_map), error::OK);
    ASSERT_EQ(p_map.size(), 5u);
    ASSERT_EQ(p_map[0]->up_processor.size(), 0u);
    ASSERT_EQ(p_map[0]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[1]->up_processor.size(), 1u);
    ASSERT_EQ(p_map[1]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[2]->up_processor.size(), 1u);
    ASSERT_EQ(p_map[2]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[3]->up_processor.size(), 1u);
    ASSERT_EQ(p_map[3]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[4]->up_processor.size(), 1u);
    ASSERT_EQ(p_map[4]->down_processor.size(), 0u);

    app_router_->DestroyProcssorMap(&p_map);
    ASSERT_EQ(p_map.size(), 0u);
}

TEST_F(test_AppRouter_ParseProcessors_suite, ParseComplexProcessorsOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application new_app;
    ConstructComplexApp(new_app, 5, 3);

    AppRouter::ProcessorMap p_map;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, p_map), error::OK);
    ASSERT_EQ(p_map.size(), 5u);
    ASSERT_EQ(p_map[0]->up_processor.size(), 0u);
    ASSERT_EQ(p_map[0]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[1]->up_processor.size(), 0u);
    ASSERT_EQ(p_map[1]->down_processor.size(), 2u);
    ASSERT_EQ(p_map[2]->up_processor.size(), 2u);
    ASSERT_EQ(p_map[2]->down_processor.size(), 2u);
    ASSERT_EQ(p_map[3]->up_processor.size(), 2u);
    ASSERT_EQ(p_map[3]->down_processor.size(), 1u);
    ASSERT_EQ(p_map[4]->up_processor.size(), 2u);
    ASSERT_EQ(p_map[4]->down_processor.size(), 0u);

    app_router_->DestroyProcssorMap(&p_map);
    ASSERT_EQ(p_map.size(), 0u);
}

TEST_F(test_AppRouter_ParseProcessors_suite, ArgumentError)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application invalid_app;
    AppRouter::ProcessorMap p_map;
    ASSERT_EQ(app_router_->ParseProcessors(invalid_app, p_map), error::BAD_ARGUMENT);
    ASSERT_EQ(p_map.size(), 0u);
}




/**
 * @brief
 **/
class test_AppRouter_GetAllDiffProcessor_L_suite : public ::testing::Test {
protected:
    test_AppRouter_GetAllDiffProcessor_L_suite() {};
    virtual ~test_AppRouter_GetAllDiffProcessor_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetAllDiffProcessor_L_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetAllDiffProcessor_L_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_GetAllDiffProcessor_L_suite, GetAllDiffProcessor_L_UpdateProcessor)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    ConstructSimpleApp(old_app, 5, 3);
    Application new_app;
    ConstructSimpleApp(new_app, 5, 4);

    ASSERT_EQ(app_router_->ParseProcessors(old_app, app_router_->m_all_processor), error::OK);
    AppRouter::ProcessorMap new_processor;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processor), error::OK);

    std::set<uint64_t> add_pro;
    std::set<uint64_t> del_pro;
    std::set<uint64_t> update_pro;
    ASSERT_EQ(app_router_->GetAllDiffProcessor_L(new_processor, add_pro, del_pro, update_pro),
              error::OK);
    ASSERT_EQ(add_pro.size(), 0u);
    ASSERT_EQ(del_pro.size(), 0u);
    ASSERT_EQ(update_pro.size(), 5u);

    app_router_->DestroyProcssorMap(&new_processor);
}

TEST_F(test_AppRouter_GetAllDiffProcessor_L_suite, GetAllDiffProcessor_L_AddProcessor)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    ConstructSimpleApp(old_app, 4, 3);
    Application new_app;
    ConstructSimpleApp(new_app, 5, 3);

    ASSERT_EQ(app_router_->ParseProcessors(old_app, app_router_->m_all_processor), error::OK);
    AppRouter::ProcessorMap new_processor;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processor), error::OK);

    std::set<uint64_t> add_pro;
    std::set<uint64_t> del_pro;
    std::set<uint64_t> update_pro;
    ASSERT_EQ(app_router_->GetAllDiffProcessor_L(new_processor, add_pro, del_pro, update_pro),
              error::OK);
    ASSERT_EQ(add_pro.size(), 1u);
    std::set<uint64_t>::iterator iter;
    iter = add_pro.find(4);
    ASSERT_TRUE(iter != add_pro.end());
    ASSERT_EQ(del_pro.size(), 0u);
    ASSERT_EQ(update_pro.size(), 0u);

    app_router_->DestroyProcssorMap(&new_processor);
}

TEST_F(test_AppRouter_GetAllDiffProcessor_L_suite, GetAllDiffProcessor_L_DelProcessor)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    ConstructSimpleApp(old_app, 5, 3);
    Application new_app;
    ConstructSimpleApp(new_app, 4, 3);

    ASSERT_EQ(app_router_->ParseProcessors(old_app, app_router_->m_all_processor), error::OK);
    AppRouter::ProcessorMap new_processor;
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processor), error::OK);

    std::set<uint64_t> add_pro;
    std::set<uint64_t> del_pro;
    std::set<uint64_t> update_pro;
    ASSERT_EQ(app_router_->GetAllDiffProcessor_L(new_processor, add_pro, del_pro, update_pro),
              error::OK);
    ASSERT_EQ(add_pro.size(), 0u);
    ASSERT_EQ(del_pro.size(), 1u);
    std::set<uint64_t>::iterator iter;
    iter = del_pro.find(4);
    ASSERT_TRUE(iter != add_pro.end());
    ASSERT_EQ(update_pro.size(), 0u);

    app_router_->DestroyProcssorMap(&new_processor);
}

/**
 * @brief
 **/
class test_AppRouter_DiffProcessor_suite : public ::testing::Test {
protected:
    test_AppRouter_DiffProcessor_suite() {};
    virtual ~test_AppRouter_DiffProcessor_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_DiffProcessor_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_DiffProcessor_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_DiffProcessor_suite, ProcessorIDAndNameDiff)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));
    // diff name
    AppProcessor new_pro;
    AppProcessor old_pro;
    new_pro.cur_pro.set_name("new_pro");
    old_pro.cur_pro.set_name("old_pro");
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));

    // diff id
    new_pro.cur_pro.mutable_id()->set_id(0);
    new_pro.cur_pro.set_name("same_name");
    old_pro.cur_pro.mutable_id()->set_id(1);
    old_pro.cur_pro.set_name("same_name");
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));
}

TEST_F(test_AppRouter_DiffProcessor_suite, ProcessorParalDiff)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));
    // same name & id
    AppProcessor new_pro;
    AppProcessor old_pro;
    new_pro.cur_pro.mutable_id()->set_id(1);
    new_pro.cur_pro.set_name("same_name");
    old_pro.cur_pro.mutable_id()->set_id(1);
    old_pro.cur_pro.set_name("same_name");

    std::set<uint64_t> add_pes;
    std::set<uint64_t> del_pes;
    // add paral
    new_pro.cur_pro.set_parallism(5);
    old_pro.cur_pro.set_parallism(4);
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro, &add_pes, &del_pes));
    ASSERT_EQ(add_pes.size(), 1u);
    ASSERT_EQ(del_pes.size(), 0u);

    // del paral
    add_pes.clear();
    del_pes.clear();
    new_pro.cur_pro.set_parallism(4);
    old_pro.cur_pro.set_parallism(6);
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro, &add_pes, &del_pes));
    ASSERT_EQ(del_pes.size(), 2u);
    ASSERT_EQ(add_pes.size(), 0u);
}

TEST_F(test_AppRouter_DiffProcessor_suite, ProcessorTagDiff)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));
    // same name & id
    AppProcessor new_pro;
    AppProcessor old_pro;
    new_pro.cur_pro.set_name("same_name");
    old_pro.cur_pro.set_name("same_name");
    new_pro.cur_pro.mutable_id()->set_id(1);
    old_pro.cur_pro.mutable_id()->set_id(1);
    // same paral
    new_pro.cur_pro.set_parallism(5);
    old_pro.cur_pro.set_parallism(5);
    // diff tag size
    TagSet tag_list;
    tag_list.insert("aaa");
    tag_list.insert("bbb");
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(3, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));

    TagSet tag_list1;
    // diff tag sub
    new_pro.sub_tags.clear();
    old_pro.sub_tags.clear();
    tag_list1.insert("aaa");
    tag_list1.insert("bbb");
    tag_list1.insert("ccc");
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list1));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));

    // tag size equal, but changed
    new_pro.sub_tags.clear();
    old_pro.sub_tags.clear();
    tag_list1.clear();
    tag_list1.insert("aaa");
    tag_list1.insert("ccc");
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list1));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    ASSERT_TRUE(app_router_->DiffProcessor(new_pro, old_pro));

    // no diff
    new_pro.sub_tags.clear();
    old_pro.sub_tags.clear();
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    new_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(1, tag_list));
    old_pro.sub_tags.insert(std::pair<uint64_t, TagSet >(2, tag_list));
    ASSERT_TRUE(!app_router_->DiffProcessor(new_pro, old_pro));
}

/**
 * @brief
 **/
class test_AppRouter_DiffApplication_L_suite : public ::testing::Test {
protected:
    test_AppRouter_DiffApplication_L_suite() {};
    virtual ~test_AppRouter_DiffApplication_L_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_DiffApplication_L_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_DiffApplication_L_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
 **/
TEST_F(test_AppRouter_DiffApplication_L_suite, NewApplication)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 0u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, SimpleAddExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;

    uint64_t add_w_peid = ConstructPEID(1, 4, 1);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(4) serial(1)
    // processor(4) serial(2)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 2)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 6, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, SimpleDelExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 6, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;

    uint64_t add_w_peid = ConstructPEID(1, 5, 1);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(5) serial(1)
    // processor(4) serial(2)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 5, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 2)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, SimpleModifyParalism)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    // add local pe
    // processor(0) serial(2)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 4);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
}

error::Code MockDiffApplication_LWithoutNotify(AppRouter* app_router,
                                               const AppRouter::ProcessorMap& new_all_processor,
                                               ProcessorSet& add_pro,
                                               ProcessorSet& del_pro,
                                               ProcessorSet& update_pro,
                                               PESet& add_watch_pe,
                                               PESet& del_watch_pe,
                                               ProcessorSet& add_watch_pro,
                                               ProcessorSet& del_watch_pro,
                                               ProcessorSet& update_watch_pro,
                                               ProcessorSet& update_flow_control_pro) {
    error::Code err_ret = error::OK;
    // all-change: add_pro, del_pro, update_pro
    // up-stream: add_watch_pe, del_watch_pe
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    if ((err_ret = app_router->GetAllDiffProcessor_L(new_all_processor,
                                                     add_pro, del_pro,
                                                     update_pro)) < error::OK) {
        DSTREAM_ERROR("[%s] fail to get diff processors, app_id(%lu)",
                      __FUNCTION__, app_router->m_app_id);
        return err_ret;
    }

    // map<ProcessorID, set<peid> watch_owner>
    AppRouter::WPMap all_watch_up_pro, all_watch_down_pro;
    err_ret = app_router->GetWatchUpProcessor_L(new_all_processor, all_watch_up_pro);
    if (err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get watching up processor, app_id(%lu)",
                      __FUNCTION__, app_router->m_app_id);
        return err_ret;
    }
    err_ret = app_router->GetWatchDownProcessor_L(new_all_processor, all_watch_down_pro);
    if (err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get watching down processor, app_id(%lu)",
                      __FUNCTION__, app_router->m_app_id);
        return err_ret;
    }
    DSTREAM_WARN("[%s] get all watching processors, new_up_pro(%zu), new_down_pro(%zu)",
                 __FUNCTION__, all_watch_up_pro.size(), all_watch_down_pro.size());

    AppRouter::WPMap old_all_watch_up_pro, old_all_watch_down_pro;
    err_ret = app_router->GetWatchUpProcessor_L(app_router->m_all_processor, old_all_watch_up_pro);
    if (err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get old watching up processor, app_id(%lu)",
                      __FUNCTION__, app_router->m_app_id);
        return err_ret;
    }
    err_ret = app_router->GetWatchDownProcessor_L(app_router->m_all_processor, old_all_watch_down_pro);
    if (err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get old watching down processor, app_id(%lu)",
                      __FUNCTION__, app_router->m_app_id);
        return err_ret;
    }
    DSTREAM_WARN("[%s] get all watching processors, old_up_pro(%zu), old_down_pro(%zu)",
                 __FUNCTION__, old_all_watch_up_pro.size(), old_all_watch_down_pro.size());
    // update
    // =========================================
    app_router->GetUpdateChange_L(new_all_processor, update_pro,
                                  all_watch_up_pro, all_watch_down_pro,
                                  update_watch_pro, update_flow_control_pro,
                                  add_watch_pe, del_watch_pe);
    // add
    // =========================================
    app_router->GetAddChange(new_all_processor, add_pro,
                             all_watch_up_pro, all_watch_down_pro,
                             add_watch_pro, add_watch_pe);
    // del
    // =========================================
    app_router->GetDelChange(app_router->m_all_processor, del_pro,
                             old_all_watch_up_pro, old_all_watch_down_pro,
                             del_watch_pro, del_watch_pe);
    return error::OK;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_DiffApplication_L_suite, UpdateAddParalism)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(0) serial(2)
    // processor(2) serial(0)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 0)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 4);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    ASSERT_EQ(add_pro.size(), 0u);
    ASSERT_EQ(del_pro.size(), 0u);
    ASSERT_EQ(update_pro.size(), 5u);
    for (uint64_t proid = 0; proid < update_pro.size(); ++proid) {
        exist_iter = update_pro.find(proid);
        ASSERT_TRUE(exist_iter != update_pro.end());
    }

    // processor(2) add 2 up pe
    // processor(4) add 2 up pe
    ASSERT_EQ(add_watch_pe.size(), 4u);
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 0, 3));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 1, 3));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 2, 3));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 3, 3));

    ASSERT_EQ(del_watch_pe.size(), 0u);
    ASSERT_EQ(add_watch_pro.size(), 0u);
    ASSERT_EQ(del_watch_pro.size(), 0u);
    // processor(0) update 1 down processor(2)
    // processor(2) update 2 down processor(3,4)
    // processor(4) has no down processor
    ASSERT_EQ(update_watch_pro.size(), 3u);
    exist_iter = update_watch_pro.find(2);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
    exist_iter = update_watch_pro.find(3);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
    exist_iter = update_watch_pro.find(4);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
}

TEST_F(test_AppRouter_DiffApplication_L_suite, UpdateDelParalism)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 4);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(0) serial(2)
    // processor(2) serial(0)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 0)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    ASSERT_EQ(add_pro.size(), 0u);
    ASSERT_EQ(del_pro.size(), 0u);
    ASSERT_EQ(update_pro.size(), 5u);
    for (uint64_t proid = 0; proid < update_pro.size(); ++proid) {
        exist_iter = update_pro.find(proid);
        ASSERT_TRUE(exist_iter != update_pro.end());
    }
    ASSERT_EQ(add_watch_pe.size(), 0u);
    // processor(0) has no up pe
    // processor(2) del 2 up pe
    // processor(4) del 2 up pe
    ASSERT_EQ(del_watch_pe.size(), 4u);
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 0, 3));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 1, 3));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 2, 3));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 3, 3));

    ASSERT_EQ(add_watch_pro.size(), 0u);
    ASSERT_EQ(del_watch_pro.size(), 0u);
    // processor(0) update 1 down processor(2)
    // processor(2) update 2 down processor(3,4)
    // processor(4) has no down processor
    ASSERT_EQ(update_watch_pro.size(), 3u);
    exist_iter = update_watch_pro.find(2);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
    exist_iter = update_watch_pro.find(3);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
    exist_iter = update_watch_pro.find(4);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
}

TEST_F(test_AppRouter_DiffApplication_L_suite, DelExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(0) serial(2)
    // processor(2) serial(0)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 0)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructComplexApp(new_app, 4, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(new_processors.size(), 4u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    ASSERT_EQ(add_pro.size(), 0u);
    // delete processor(4)
    ASSERT_EQ(del_pro.size(), 1u);
    exist_iter = del_pro.find(4);
    ASSERT_TRUE(exist_iter != del_pro.end());
    ASSERT_EQ(update_pro.size(), 0u);
    // no up pe changed
    ASSERT_EQ(add_watch_pe.size(), 0u);
    ASSERT_EQ(del_watch_pe.size(), 0u);
    // no down pro add
    ASSERT_EQ(add_watch_pro.size(), 0u);
    // processor(2)'s down processor(4) has deleted
    ASSERT_EQ(del_watch_pro.size(), 1u);
    exist_iter = del_watch_pro.find(4);
    ASSERT_TRUE(exist_iter != del_watch_pro.end());
    ASSERT_EQ(update_watch_pro.size(), 0u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, AddExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(0) serial(2)
    // processor(2) serial(0)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 0)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructComplexApp(new_app, 6, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(new_processors.size(), 6u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    // add processor(5)
    ASSERT_EQ(add_pro.size(), 1u);
    exist_iter = add_pro.find(5);
    ASSERT_TRUE(exist_iter != add_pro.end());
    ASSERT_EQ(del_pro.size(), 0u);
    ASSERT_EQ(update_pro.size(), 0u);
    // no up pe changed
    ASSERT_EQ(add_watch_pe.size(), 0u);
    ASSERT_EQ(del_watch_pe.size(), 0u);
    // processor(4)'s down processor(5) has added
    ASSERT_EQ(add_watch_pro.size(), 1u);
    exist_iter = add_watch_pro.find(5);
    ASSERT_TRUE(exist_iter != add_watch_pro.end());
    // no down pro deleted
    ASSERT_EQ(del_watch_pro.size(), 0u);
    // no down pro updated
    ASSERT_EQ(update_watch_pro.size(), 0u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, AddImporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyImporter(old_app, 4/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(kMaxProcessorNum-1) serial(2)
    // processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: add processor(4)
    ConstructAppForModifyImporter(new_app, 5/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
    ASSERT_EQ(new_processors.size(), 7u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    // add importer processor(4)
    ASSERT_EQ(add_pro.size(), 1u);
    exist_iter = add_pro.find(4);
    ASSERT_TRUE(exist_iter != add_pro.end());
    ASSERT_EQ(del_pro.size(), 0u);
    // update processor(1000-1, 1000-2)'s sub info
    ASSERT_EQ(update_pro.size(), 2u);
    exist_iter = update_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_pro.end());
    exist_iter = update_pro.find(kMaxProcessorNum - 2);
    ASSERT_TRUE(exist_iter != update_pro.end());

    // add 3 up pe of importer processor(4)
    ASSERT_EQ(add_watch_pe.size(), 3u);
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 0));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 1));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 2));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());

    ASSERT_EQ(del_watch_pe.size(), 0u);
    ASSERT_EQ(add_watch_pro.size(), 0u);
    // no down pro deleted
    ASSERT_EQ(del_watch_pro.size(), 0u);
    // no down pro updated
    ASSERT_EQ(update_watch_pro.size(), 0u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, DelImporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyImporter(old_app, 5/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(kMaxProcessorNum-1) serial(2)
    // processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructAppForModifyImporter(new_app, 4/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 7u);
    ASSERT_EQ(new_processors.size(), 6u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    ASSERT_EQ(add_pro.size(), 0u);
    // del importer processor(4)
    ASSERT_EQ(del_pro.size(), 1u);
    exist_iter = del_pro.find(4);
    ASSERT_TRUE(exist_iter != del_pro.end());

    // update processor(1000-1, 1000-2)'s sub info
    ASSERT_EQ(update_pro.size(), 2u);
    exist_iter = update_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_pro.end());
    exist_iter = update_pro.find(kMaxProcessorNum - 2);
    ASSERT_TRUE(exist_iter != update_pro.end());

    ASSERT_EQ(add_watch_pe.size(), 0u);
    // del 3 up pe of importer processor(4)
    ASSERT_EQ(del_watch_pe.size(), 3u);
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 0));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 1));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 2));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());

    ASSERT_EQ(add_watch_pro.size(), 0u);
    // no down pro deleted
    ASSERT_EQ(del_watch_pro.size(), 0u);
    // no down pro updated
    ASSERT_EQ(update_watch_pro.size(), 0u);
}

TEST_F(test_AppRouter_DiffApplication_L_suite, DelWorker)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyWorker(old_app, 5/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // exporter processor(kMaxProcessorNum-1) serial(2)
    // worker processor(2) serial(1)
    // worker processor(4) serial(1), to be deleted
    // importer processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 1)), error::OK);
    // watch himself
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructAppForModifyWorker(new_app, 4/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 7u);
    ASSERT_EQ(new_processors.size(), 6u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    ASSERT_EQ(add_pro.size(), 0u);
    // del importer processor(4)
    ASSERT_EQ(del_pro.size(), 1u);
    exist_iter = del_pro.find(4);
    ASSERT_TRUE(exist_iter != del_pro.end());

    // update exporter(kMaxProcessorNum-1)'s sub info
    // TODO: something maybe not work
    ASSERT_EQ(update_pro.size(), 1u);
    exist_iter = update_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_pro.end());

    ASSERT_EQ(add_watch_pe.size(), 0u);
    // del 3 up pe of worker processor(4)
    ASSERT_EQ(del_watch_pe.size(), 3u);
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 0));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 1));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());
    pe_exist_iter = del_watch_pe.find(ConstructPEID(1, 4, 2));
    ASSERT_TRUE(pe_exist_iter != del_watch_pe.end());

    ASSERT_EQ(add_watch_pro.size(), 0u);
    // importer(kMaxProcessorNum-2) delete down processor(4)
    ASSERT_EQ(del_watch_pro.size(), 1u);
    exist_iter = del_watch_pro.find(4);
    ASSERT_TRUE(exist_iter != del_watch_pro.end());

    // exporter(kMaxProcessorNum-1) update to delete sub to processor(4)
    ASSERT_EQ(update_watch_pro.size(), 1u);
    exist_iter = update_watch_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
}

TEST_F(test_AppRouter_DiffApplication_L_suite, AddWorker)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyWorker(old_app, 4/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 0, 2);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // exporter processor(kMaxProcessorNum-1) serial(2)
    // worker processor(2) serial(1)
    // importer processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructAppForModifyWorker(new_app, 5/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
    ASSERT_EQ(new_processors.size(), 7u);

    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;
    ASSERT_EQ(MockDiffApplication_LWithoutNotify(app_router_,
                                                 new_processors,
                                                 add_pro,
                                                 del_pro,
                                                 update_pro,
                                                 add_watch_pe,
                                                 del_watch_pe,
                                                 add_watch_pro,
                                                 del_watch_pro,
                                                 update_watch_pro,
                                                 update_flow_control_pro), error::OK);
    ProcessorSet::const_iterator exist_iter;
    PESet::const_iterator pe_exist_iter;
    // add importer processor(4)
    ASSERT_EQ(add_pro.size(), 1u);
    exist_iter = add_pro.find(4);
    ASSERT_TRUE(exist_iter != add_pro.end());
    // no pro deleted
    ASSERT_EQ(del_pro.size(), 0u);

    // update processor(kMaxProcessorNum-1)'s sub info
    // TODO: something maybe not work
    ASSERT_EQ(update_pro.size(), 1u);
    exist_iter = update_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_pro.end());

    // add 3 up pe of worker processor(4)
    ASSERT_EQ(add_watch_pe.size(), 3u);
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 0));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 1));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());
    pe_exist_iter = add_watch_pe.find(ConstructPEID(1, 4, 2));
    ASSERT_TRUE(pe_exist_iter != add_watch_pe.end());

    ASSERT_EQ(del_watch_pe.size(), 0u);
    // importer(kMaxProcessorNum-2) add down processor(4)
    ASSERT_EQ(add_watch_pro.size(), 1u);
    exist_iter = add_watch_pro.find(4);
    ASSERT_TRUE(exist_iter != add_watch_pro.end());

    ASSERT_EQ(del_watch_pro.size(), 0u);
    // exporter(kMaxProcessorNum-1) update to add sub to processor(4)
    ASSERT_EQ(update_watch_pro.size(), 1u);
    exist_iter = update_watch_pro.find(kMaxProcessorNum - 1);
    ASSERT_TRUE(exist_iter != update_watch_pro.end());
}


/**
 * @brief
**/
class test_AppRouter_GetProcessorElement_suite : public ::testing::Test {
protected:
    test_AppRouter_GetProcessorElement_suite() {};
    virtual ~test_AppRouter_GetProcessorElement_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_AppRouter_GetProcessorElement_suite, *)
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_AppRouter_GetProcessorElement_suite, *)
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_AppRouter_GetProcessorElement_suite, GetProcessorElementOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    ProcessorElement pe;
    pe.mutable_pe_id()->set_id(12345);

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::FAILED_EXPECTATION))
    .WillOnce(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ProcessorElement target_pe;
    ASSERT_EQ(error::OK, app_router_->GetProcessorElement(12345, &target_pe));
    ASSERT_EQ(pe.pe_id().id(), target_pe.pe_id().id());
}


TEST_F(test_AppRouter_GetProcessorElement_suite, GetProcessorElementZKNoNode)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());

    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);
    ProcessorElement target_pe;
    ASSERT_EQ(error::ZK_NO_NODE, app_router_->GetProcessorElement(12345, &target_pe));
}

struct PENotify {
    uint64_t owner;
    uint64_t peid;
    std::string pn_addr;

    PENotify()
        : owner(0), peid(0), pn_addr("")
    {
    }

    PENotify(const PENotify& obj) {
        owner = obj.owner;
        peid = obj.peid;
        pn_addr = obj.pn_addr;
    }

    PENotify& operator=(const PENotify& obj) {
        owner = obj.owner;
        peid = obj.peid;
        pn_addr = obj.pn_addr;
        return *this;
    }

    bool operator==(const PENotify& obj) const {
        return (owner == obj.owner && peid == obj.peid);
    }
};

struct ProcessorNotify {
    uint64_t owner;
    uint64_t proid;
    uint32_t paral;
    TagSet tags;

    bool operator==(const ProcessorNotify& obj) const {
        return (owner == obj.owner && proid == obj.proid);
    }
};

class TestAppRouterNotify : public AppRouter {
protected:
    // reload notify transmitter interface
    // ---------------------------
    virtual void NotifyFunc(AppRouter::NotifyEvent type, uint64_t notify_peid, void* notify_ctx) {
        if (type < AppRouter::PE_EVENT_END) {
            PEInfo* ctx = static_cast<PEInfo*>(notify_ctx);
            PENotify pe_info;
            pe_info.owner = notify_peid;
            pe_info.peid = ctx->pe_id;
            pe_info.pn_addr = ctx->location;
            switch (type) {
            case AppRouter::ADD_UP_PE:
                add_up_pe.push_back(pe_info);
                break;
            case AppRouter::DEL_UP_PE:
                del_up_pe.push_back(pe_info);
                break;
            case AppRouter::UPDATE_UP_PE:
                update_up_pe.push_back(pe_info);
                break;
            default:
                DSTREAM_ERROR("unknown pe notify type(%d)", type);
                break;
            } // end switch
        } else if (type < AppRouter::PROCESSOR_EVENT_END) {
            ProcessorInfo* ctx = static_cast<ProcessorInfo*>(notify_ctx);
            ProcessorNotify pro_info;
            pro_info.owner = notify_peid;
            pro_info.proid = ctx->pro_id;
            pro_info.paral = ctx->parallel;
            pro_info.tags = ctx->tags;
            switch (type) {
            case AppRouter::ADD_DOWN_PRO:
                add_down_pro.push_back(pro_info);
                break;
            case AppRouter::DEL_DOWN_PRO:
                del_down_pro.push_back(pro_info);
                break;
            case AppRouter::UPDATE_DOWN_PRO:
                update_down_pro.push_back(pro_info);
                break;
            default:
                DSTREAM_ERROR("unknown pe notify type(%d)", type);
                break;
            } // end switch
        } else if (type == AppRouter::APP_FLOW_CTRL) {
            CtrlInfo* ctx = static_cast<CtrlInfo*>(notify_ctx);
            flow_contrl_pro.push_back(*ctx);
        }
    }

public:
    std::vector<PENotify> add_up_pe;
    std::vector<PENotify> del_up_pe;
    std::vector<PENotify> update_up_pe;
    std::vector<ProcessorNotify> add_down_pro;
    std::vector<ProcessorNotify> del_down_pro;
    std::vector<ProcessorNotify> update_down_pro;
    std::vector<CtrlInfo> flow_contrl_pro;
};

class test_AppRouter_RouterNotify_suite : public ::testing::Test {
protected:
    test_AppRouter_RouterNotify_suite() {};
    virtual ~test_AppRouter_RouterNotify_suite() {};
    virtual void SetUp() {
        app_router_ = new (std::nothrow) TestAppRouterNotify();
    };
    virtual void TearDown() {
        delete app_router_;
    };
private:
    TestAppRouterNotify* app_router_;
};

TEST_F(test_AppRouter_RouterNotify_suite, UpdateAddParalism)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // processor(0) serial(2)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 4);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 2 local pes
    // processor(0) serial(2) has no up pe
    // processor(4) serial(1) has 3 paralism * 2 up pe
    // processor(4) serial(1) add 2 up pe
    ASSERT_EQ(app_router_->add_up_pe.size(), 8u);
    find_pe.owner = ConstructPEID(1, 4, 1);
    find_pe.peid = ConstructPEID(1, 2, 3);
    find_iter = find(app_router_->add_up_pe.begin(), app_router_->add_up_pe.end(), find_pe);
    ASSERT_TRUE(app_router_->add_up_pe.end() != find_iter);
    find_pe.owner = ConstructPEID(1, 4, 1);
    find_pe.peid = ConstructPEID(1, 3, 3);
    find_iter = find(app_router_->add_up_pe.begin(), app_router_->add_up_pe.end(), find_pe);
    ASSERT_TRUE(app_router_->add_up_pe.end() != find_iter);
    // no del_up_pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 0u);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 2 local pes
    // processor(0) has 1 down pro
    // processor(4) has no down pro
    ASSERT_EQ(app_router_->add_down_pro.size(), 1u);
    // no del_down_pro
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // processor(0) update_down_pro (2)
    ASSERT_EQ(app_router_->update_down_pro.size(), 1u);
    find_pro.owner = ConstructPEID(1, 0, 2);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 2);
    find_pro_iter = find(app_router_->update_down_pro.begin(), app_router_->update_down_pro.end(),
                         find_pro);
    ASSERT_TRUE(app_router_->update_down_pro.end() != find_pro_iter);
}

TEST_F(test_AppRouter_RouterNotify_suite, UpdateDelParalism)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 4);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // processor(0) serial(2)
    // processor(4) serial(1)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 2 local pes
    // processor(0) serial(2) has no up pe
    // processor(4) serial(1) has 4 paralism * 2 up pe
    ASSERT_EQ(app_router_->add_up_pe.size(), 8u);
    // processor(4) serial(1) del 2 up pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 2u);
    find_pe.owner = ConstructPEID(1, 4, 1);
    find_pe.peid = ConstructPEID(1, 2, 3);
    find_iter = find(app_router_->del_up_pe.begin(), app_router_->del_up_pe.end(), find_pe);
    ASSERT_TRUE(app_router_->del_up_pe.end() != find_iter);
    find_pe.owner = ConstructPEID(1, 4, 1);
    find_pe.peid = ConstructPEID(1, 3, 3);
    find_iter = find(app_router_->del_up_pe.begin(), app_router_->del_up_pe.end(), find_pe);
    ASSERT_TRUE(app_router_->del_up_pe.end() != find_iter);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 2 local pes
    // processor(0) has 1 down pro
    // processor(4) has no down pro
    ASSERT_EQ(app_router_->add_down_pro.size(), 1u);
    // no del_down_pro
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // processor(0) update_down_pro (2)
    ASSERT_EQ(app_router_->update_down_pro.size(), 1u);
    find_pro.owner = ConstructPEID(1, 0, 2);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 2);
    find_pro_iter = find(app_router_->update_down_pro.begin(), app_router_->update_down_pro.end(),
                         find_pro);
    ASSERT_TRUE(app_router_->update_down_pro.end() != find_pro_iter);
}

TEST_F(test_AppRouter_RouterNotify_suite, AddImporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyImporter(old_app, 4/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // processor(kMaxProcessorNum-1) serial(2)
    // processor(1) serial (1)
    // processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 1, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: add processor(4)
    ConstructAppForModifyImporter(new_app, 5/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(new_processors.size(), 7u);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) serial(2) has 3 paralism * 4 up pes
    // processor(kMaxProcessorNum-2) serial(0) has 3 paralism * 4 up pes
    // processor(1) serial(1) has no up pe
    // processor(kMaxProcessorNum-1) serial(2) add 3 up pe
    // processor(kMaxProcessorNum-2) serial(0) add 3 up pe
    ASSERT_EQ(app_router_->add_up_pe.size(), 30u);
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 1, 2);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->add_up_pe.begin(), app_router_->add_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->add_up_pe.end() != find_iter);
    }
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 2, 0);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->add_up_pe.begin(), app_router_->add_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->add_up_pe.end() != find_iter);
    }
    // no del_up_pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 0u);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has no down pro
    // processor(kMaxProcessorNum-2) has no down pro
    // processor(1) serial(1) has 2 down pro
    ASSERT_EQ(app_router_->add_down_pro.size(), 2u);
    // no del_down_pro
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);
}

TEST_F(test_AppRouter_RouterNotify_suite, DelImporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyImporter(old_app, 5/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // processor(kMaxProcessorNum-1) serial(2)
    // processor(1) serial (1)
    // processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 1, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: del processor(4)
    ConstructAppForModifyImporter(new_app, 4/*importer num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(new_processors.size(), 6u);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) serial(2) has 3 paralism * 5 up pes
    // processor(kMaxProcessorNum-2) serial(0) has 3 paralism * 5 up pes
    // processor(1) serial(1) has no up pe
    ASSERT_EQ(app_router_->add_up_pe.size(), 30u);
    // processor(kMaxProcessorNum-1) serial(2) del 3 up pe
    // processor(kMaxProcessorNum-2) serial(0) del 3 up pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 6u);
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 1, 2);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->del_up_pe.begin(), app_router_->del_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->del_up_pe.end() != find_iter);
    }
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 2, 0);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->del_up_pe.begin(), app_router_->del_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->del_up_pe.end() != find_iter);
    }
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has no down pro
    // processor(kMaxProcessorNum-2) has no down pro
    // processor(1) serial(1) has 2 down pro
    ASSERT_EQ(app_router_->add_down_pro.size(), 2u);
    // no del_down_pro
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);

}

TEST_F(test_AppRouter_RouterNotify_suite, AddExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(4) serial(1)
    // processor(3) serial(2)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 3, 2)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 6, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);

    // check result
    // PENotify find_pe;
    // std::vector<PENotify>::iterator find_iter;
    // we add 2 local pes
    // processor(4) serial(1) has 3 paralism * 2 up pes
    // processor(3) serial(2) has 3 paralism * 2 up pes
    ASSERT_EQ(app_router_->add_up_pe.size(), 12u);
    // no del_up_pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 0u);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 2 local pes
    // processor(4) has no down pro
    // processor(3) has 1 down pros
    // processor(3,4) add_down_processor(5)
    ASSERT_EQ(app_router_->add_down_pro.size(), 3u);
    find_pro.owner = ConstructPEID(1, 3, 2);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 5);
    find_pro_iter = find(app_router_->add_down_pro.begin(), app_router_->add_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->add_down_pro.end() != find_pro_iter);
    find_pro.owner = ConstructPEID(1, 4, 1);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 5);
    find_pro_iter = find(app_router_->add_down_pro.begin(), app_router_->add_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->add_down_pro.end() != find_pro_iter);
    // no del_down_pro
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);
}

TEST_F(test_AppRouter_RouterNotify_suite, DelExporter)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructComplexApp(old_app, 6, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    // add local pe
    // processor(4) serial(1)
    // processor(3) serial(2)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 4, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 3, 2)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    ConstructComplexApp(new_app, 5, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);

    // check result
    // PENotify find_pe;
    // std::vector<PENotify>::iterator find_iter;
    // we add 2 local pes
    // processor(4) serial(1) has 3 paralism * 2 up pes
    // processor(3) serial(2) has 3 paralism * 2 up pes
    ASSERT_EQ(app_router_->add_up_pe.size(), 12u);
    // no del_up_pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 0u);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 3 local pes
    // processor(4) has 1 down pro
    // processor(3) has 2 down pros
    ASSERT_EQ(app_router_->add_down_pro.size(), 3u);
    // processor(3,4) del_down_processor(5)
    ASSERT_EQ(app_router_->del_down_pro.size(), 2u);
    find_pro.owner = ConstructPEID(1, 3, 2);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 5);
    find_pro_iter = find(app_router_->del_down_pro.begin(), app_router_->del_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->del_down_pro.end() != find_pro_iter);
    find_pro.owner = ConstructPEID(1, 4, 1);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 5);
    find_pro_iter = find(app_router_->del_down_pro.begin(), app_router_->del_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->del_down_pro.end() != find_pro_iter);

    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);
}

TEST_F(test_AppRouter_RouterNotify_suite, AddWorker)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyWorker(old_app, 4/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // exporter processor(kMaxProcessorNum-1) serial(2)
    // worker processor(2) serial(1)
    // importer processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructAppForModifyWorker(new_app, 5/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 7u);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has 3 paralism * 4 up pes
    // processor(kMaxProcessorNum-2) has no up pe
    // worker processor(2) has 3 up pes
    // exporter processor(kMaxProcessorNum-1) add 3 up pes
    ASSERT_EQ(app_router_->add_up_pe.size(), 18u);
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 1, 2);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->add_up_pe.begin(), app_router_->add_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->add_up_pe.end() != find_iter);
    }
    // no del_up_pe
    ASSERT_EQ(app_router_->del_up_pe.size(), 0u);
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has no down pro
    // processor(kMaxProcessorNum-2) has 4 down pros
    // worker processor(2) has 1 down pro
    // importer processor(kMaxProcessorNum-2) add 1 down processor(4)
    ASSERT_EQ(app_router_->add_down_pro.size(), 6u);
    // find new add down pro
    find_pro.owner = ConstructPEID(1, kMaxProcessorNum - 2, 0);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 4);
    find_pro_iter = find(app_router_->add_down_pro.begin(), app_router_->add_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->add_down_pro.end() != find_pro_iter);
    // no del_down_processor
    ASSERT_EQ(app_router_->del_down_pro.size(), 0u);
    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);

}

TEST_F(test_AppRouter_RouterNotify_suite, DelWorker)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    uint64_t add_w_peid = ConstructPEID(1, 2, 3);
    ProcessorElement pe;
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(add_w_peid);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(add_w_peid);
    PN pn;
    pn.set_host("test_pn_host");
    pn.set_sub_port(7890);
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillRepeatedly(DoAll(SetArgPointee<2>(pe), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructAppForModifyWorker(old_app, 5/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    // add local pe
    // exporter processor(kMaxProcessorNum-1) serial(2)
    // worker processor(2) serial(1)
    // importer processor(kMaxProcessorNum-2) serial(0)
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 1, 2)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 2, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, kMaxProcessorNum - 2, 0)), error::OK);

    Application new_app;
    AppRouter::ProcessorMap new_processors;
    // NOTICE: delete processor(4)
    ConstructAppForModifyWorker(new_app, 4/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(new_app, new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 7u);
    ASSERT_EQ(app_router_->DiffApplication_L(new_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 6u);

    // check result
    PENotify find_pe;
    std::vector<PENotify>::iterator find_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has 3 paralism * 5 up pes
    // processor(kMaxProcessorNum-2) has no up pe
    // worker processor(2) has 3 up pes
    ASSERT_EQ(app_router_->add_up_pe.size(), 18u);
    // exporter processor(kMaxProcessorNum-1) del 3 up pes
    ASSERT_EQ(app_router_->del_up_pe.size(), 3u);
    for (int i = 0; i < 3; ++i) {
        find_pe.owner = ConstructPEID(1, kMaxProcessorNum - 1, 2);
        find_pe.peid = ConstructPEID(1, 4, i);
        find_iter = find(app_router_->del_up_pe.begin(), app_router_->del_up_pe.end(), find_pe);
        ASSERT_TRUE(app_router_->del_up_pe.end() != find_iter);
    }
    // no update_up_pe
    ASSERT_EQ(app_router_->update_up_pe.size(), 0u);

    ProcessorNotify find_pro;
    std::vector<ProcessorNotify>::iterator find_pro_iter;
    // we add 3 local pes
    // processor(kMaxProcessorNum-1) has no down pro
    // processor(kMaxProcessorNum-2) has 5 down pros
    // worker processor(2) has 1 down pro
    ASSERT_EQ(app_router_->add_down_pro.size(), 6u);
    // importer processor(kMaxProcessorNum-2) del 1 down processor(4)
    ASSERT_EQ(app_router_->del_down_pro.size(), 1u);
    find_pro.owner = ConstructPEID(1, kMaxProcessorNum - 2, 0);
    find_pro.proid = MAKE_APP_PROCESSOR_ID(app_router_->m_app_id, 4);
    find_pro_iter = find(app_router_->del_down_pro.begin(), app_router_->del_down_pro.end(), find_pro);
    ASSERT_TRUE(app_router_->del_down_pro.end() != find_pro_iter);

    // no update down processor need notify
    ASSERT_EQ(app_router_->update_down_pro.size(), 0u);

}

TEST_F(test_AppRouter_RouterNotify_suite, FlowControl)
{
    ASSERT_NE(app_router_, reinterpret_cast<TestAppRouterNotify*>(NULL));

    Application old_app;
    AppRouter::ProcessorMap old_processors;
    ConstructSimpleApp(old_app, 5/*worker num*/, 3);
    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    app_router_->m_all_processor = old_processors;
    app_router_->m_app_id = 1;
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 0)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 1)), error::OK);
    ASSERT_EQ(app_router_->AddPE(ConstructPEID(1, 0, 2)), error::OK);

    // modify flow control
    old_app.mutable_topology()->mutable_processors(0)->mutable_flow_control()->set_qps_limit(100);

    ASSERT_EQ(app_router_->ParseProcessors(old_app, old_processors), error::OK);
    ASSERT_EQ(app_router_->m_all_processor.size(), 5u);
    ASSERT_EQ(app_router_->DiffApplication_L(old_processors), error::OK);
    ASSERT_EQ(app_router_->flow_contrl_pro.size(), 3u);
    ASSERT_EQ(app_router_->flow_contrl_pro.front().flag.qps_limit(), 100);
}

/**
 * @brief
 **/
class test_AppRouter_DiffProcessorElement_L_suite : public ::testing::Test {
protected:
    test_AppRouter_DiffProcessorElement_L_suite() {};
    virtual ~test_AppRouter_DiffProcessorElement_L_suite() {};
    virtual void SetUp() {
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

TEST_F(test_AppRouter_DiffProcessorElement_L_suite, DiffOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    // pe not in m_all_watch_pes
    ProcessorElement pe;
    pe.mutable_pe_id()->set_id(12345);
    ASSERT_EQ(error::OK, app_router_->DiffProcessorElement_L(pe));

    // pe in the same pn
    AppPE* t_apppe_ptr = new (std::nothrow) AppPE();
    ASSERT_TRUE(t_apppe_ptr);
    BackupPE* backup_pe = pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(12345);
    BackupPEID* bakcup_peid = pe.add_primary();
    bakcup_peid->set_id(12345);
    t_apppe_ptr->app_pe = pe;
    t_apppe_ptr->pn_addr = "test_pn";
    app_router_->m_all_watch_pes.insert(AppRouter::WatchPEPair(12345, t_apppe_ptr));
    ASSERT_EQ(error::OK, app_router_->DiffProcessorElement_L(pe));
}

TEST_F(test_AppRouter_DiffProcessorElement_L_suite, DiffZKFail)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    // pn addr is empty
    AppPE* t_apppe_ptr = new (std::nothrow) AppPE();
    ASSERT_TRUE(t_apppe_ptr);
    ProcessorElement pe;
    pe.mutable_pe_id()->set_id(12345);
    t_apppe_ptr->app_pe = pe;
    t_apppe_ptr->pn_addr = "";
    app_router_->m_all_watch_pes.insert(AppRouter::WatchPEPair(12345, t_apppe_ptr));

    // mock zk
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    ProcessorElement target_pe;
    target_pe.mutable_pe_id()->set_id(12345);
    BackupPE* backup_pe = target_pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(12345);
    BackupPEID* bakcup_peid = target_pe.add_primary();
    bakcup_peid->set_id(12345);
    ASSERT_EQ(error::ZK_NO_NODE, app_router_->DiffProcessorElement_L(target_pe));
}

TEST_F(test_AppRouter_DiffProcessorElement_L_suite, ZKOK_PNAddrEmpty)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    // pn addr is empty
    AppPE* t_apppe_ptr = new (std::nothrow) AppPE();
    ASSERT_TRUE(t_apppe_ptr);
    ProcessorElement pe;
    pe.mutable_pe_id()->set_id(12345);
    t_apppe_ptr->app_pe = pe;
    t_apppe_ptr->pn_addr = "";
    app_router_->m_all_watch_pes.insert(AppRouter::WatchPEPair(12345, t_apppe_ptr));

    // mock zk
    PN pn;
    pn.set_host("");
    pn.set_sub_port(0);
    std::string pn_addr = "";
    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetPN(_, _))
    .WillOnce(DoAll(SetArgPointee<1>(pn), Return(error::OK)));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    ProcessorElement target_pe;
    target_pe.mutable_pe_id()->set_id(12345);
    BackupPE* backup_pe = target_pe.add_backups();
    backup_pe->mutable_backup_pe_id()->set_id(12345);
    BackupPEID* bakcup_peid = target_pe.add_primary();
    bakcup_peid->set_id(12345);
    ASSERT_EQ(error::OK, app_router_->DiffProcessorElement_L(target_pe));
}

/**
 * @brief
 **/
class test_AppRouter_PEEventProcess_suite : public ::testing::Test {
protected:
    test_AppRouter_PEEventProcess_suite() {};
    virtual ~test_AppRouter_PEEventProcess_suite() {};
    virtual void SetUp() {
        app_router_ = new (std::nothrow) AppRouter();
    };
    virtual void TearDown() {
        delete app_router_;
    };
private:
    AppRouter* app_router_;
};

TEST_F(test_AppRouter_PEEventProcess_suite, DiffOK)
{
    ASSERT_NE(app_router_, reinterpret_cast<AppRouter*>(NULL));

    MockZkWrapper* mock_zk_wrapper = new (std::nothrow) MockZkWrapper();
    ASSERT_NE(mock_zk_wrapper, reinterpret_cast<MockZkWrapper*>(NULL));
    EXPECT_CALL(*mock_zk_wrapper, GetProcessorElement(_, _, _))
    .WillOnce(Return(error::ZK_NO_NODE));
    EXPECT_CALL(*mock_zk_wrapper, Destroy());
    app_router_->m_zk_wrapper = ZkWrapper::ZkWrapperPtr(mock_zk_wrapper);

    app_router_->PEEventProcess(123);
}

