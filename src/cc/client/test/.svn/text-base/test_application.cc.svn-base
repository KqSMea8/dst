#include "common/logger.h"
#include "common/application_tools.h"
#include "client/application.h"
#include <gtest/gtest.h>

using namespace dstream;
using namespace client;
using namespace application_tools;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_client_config");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_InitCfg_suite : public ::testing::Test {
protected:
    test_InitCfg_suite() {};
    virtual ~test_InitCfg_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_InitCfg_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_InitCfg_suite, *)
    };
    void TestInitCfg();
};

void test_InitCfg_suite::TestInitCfg() {
    config::Config cfg;
    ASSERT_EQ(error::OK, InitCfg("topo_check_test_correct_complicated_topo", false, false, false, &cfg));
    ASSERT_EQ(error::OK, InitCfg("topo_check_test_correct_add_importer", false, true, false, &cfg));
    ASSERT_EQ(error::OK, InitCfg("topo_check_test_correct_add_sub_tree", true, false, false, &cfg));

    ASSERT_NE(error::OK, InitCfg("wrong_topo", false, false, false, &cfg));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_InitCfg_suite, TestInitCfg)
{
    TestInitCfg();
}
/**
 * @brief
**/
class test_GetAppName_suite : public ::testing::Test {
protected:
    test_GetAppName_suite() {};
    virtual ~test_GetAppName_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_GetAppName_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_GetAppName_suite, *)
    };
    void TestGetAppName();
};

void test_GetAppName_suite::TestGetAppName() {
    std::string app_name;
    ASSERT_EQ(error::OK, GetAppName("topo_check_test_correct_complicated_topo",
                                    false, false, false, &app_name));
    ASSERT_STREQ("word_count_test", app_name.c_str());

    ASSERT_NE(error::OK, GetAppName("wrong_topo", false, false, false, &app_name));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_GetAppName_suite, TestGetAppName)
{
    TestGetAppName();
}

/**
 * @brief
**/
class test_InitApp_suite : public ::testing::Test {
protected:
    test_InitApp_suite() {};
    virtual ~test_InitApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_InitApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_InitApp_suite, *)
    };
    void TestInitApp();
};

void test_InitApp_suite::TestInitApp() {
    Application app;
    ASSERT_EQ(error::FILE_OPERATION_ERROR, InitApp("non_exists_dir", false, false,
                                                   false, false, &app));

    // test of init cfg fail
    ASSERT_NE(error::OK, InitApp("topo_check_test_correct_add_importer",
                                 false, false, false, false, &app));

    // test of get str cfg iterm fail
    ASSERT_NE(error::OK, InitApp("topo_check_test_incorrect_config",
                                 false, false, false, false, &app));
    ASSERT_NE(error::OK, InitApp("topo_check_test_incorrect_config",
                                 false, false, false, false, &app));
    // test of app invalid
    // test of processor num empty
    // test of read processors fail (importer)
    // test of read processors fail (exporter)
    // test of read processors fail (work_processor)
    // test of exporter pu
    ASSERT_EQ(error::OK, InitApp("topo_check_test_correct_exporter_pub",
                                 false, false, false, false, &app));

    ASSERT_EQ(error::OK, InitApp("topo_check_test_correct_complicated_topo", false, false, false,
                                 false, &app));
    // test for QPSLimit conf
    std::cout << "====================================" << std::endl;
    app.Clear();
    ASSERT_EQ(error::OK, InitApp("topo_check_test_correct_qps_limit", false, false, false,
                                 false, &app));
    ASSERT_EQ(app.topology().processors(0).flow_control().qps_limit(), 1000);
    ASSERT_EQ(app.topology().processors(1).flow_control().qps_limit(), -1); // default: no limit
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_InitApp_suite, TestInitApp)
{
    TestInitApp();
}

/**
 * @brief
**/
class test_MergeSubTreeIntoApp_suite : public ::testing::Test {
protected:
    test_MergeSubTreeIntoApp_suite() {};
    virtual ~test_MergeSubTreeIntoApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MergeSubTreeIntoApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MergeSubTreeIntoApp_suite, *)
    };
    void TestMergeSubTree();
};

void test_MergeSubTreeIntoApp_suite::TestMergeSubTree() {
    Application old_tree;
    Application sub_tree;
    Application fake;
    ConstructFakeAppClient(&old_tree);
    ConstructFakeSubTree(true, &sub_tree);

    // test of wrong app name
    fake.CopyFrom(sub_tree);
    fake.set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, MergeSubTreeIntoApp(fake, &old_tree));


    // test of new tp is NULL
    /*Topology* topo = old_tree.release_topology();
    delete topo;
    ASSERT_EQ(error::BAD_ARGUMENT, MergeSubTreeIntoApp(&old_tree, sub_tree));
    */

    // test of add exporter
    fake.Clear();
    fake.CopyFrom(sub_tree);
    fake.mutable_topology()->mutable_processors(0)->set_role(EXPORTER);
    ASSERT_EQ(error::OK, MergeSubTreeIntoApp(fake, &old_tree));

    // test of check topology fail
    fake.Clear();
    fake.CopyFrom(sub_tree);
    Subscribe* sub = fake.mutable_topology()->mutable_processors(0)->mutable_subscribe();
    sub->mutable_processor(0)->set_name("wrong");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, MergeSubTreeIntoApp(fake, &old_tree));


    // test of OK
    old_tree.Clear();
    ConstructFakeAppClient(&old_tree);
    ASSERT_EQ(error::OK, MergeSubTreeIntoApp(sub_tree, &old_tree));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MergeSubTreeIntoApp_suite, TestMergeSubTree)
{
    TestMergeSubTree();
}

/**
 * @brief
**/
class test_DelSubTreeFromApp_suite : public ::testing::Test {
protected:
    test_DelSubTreeFromApp_suite() {};
    virtual ~test_DelSubTreeFromApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelSubTreeFromApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DelSubTreeFromApp_suite, *)
    };
    void TestDelSubTree();
};

void test_DelSubTreeFromApp_suite::TestDelSubTree() {
    Application old_tree;
    Application sub_tree;
    Application fake;
    ConstructFakeAppClient(&old_tree);
    ConstructFakeSubTree(false, &sub_tree);

    // test of wrong app name
    fake.CopyFrom(sub_tree);
    fake.set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, DelSubTreeFromApp(fake, &old_tree));

    // test of new tp is NULL
    fake.Clear();
    fake.CopyFrom(old_tree);
    Topology* topo = fake.release_topology();
    delete topo;
    ASSERT_EQ(error::BAD_ARGUMENT, DelSubTreeFromApp(sub_tree, &fake));

    // test of sub tree bigger
    fake.CopyFrom(sub_tree);
    fake.mutable_topology()->set_processor_num(256);
    ASSERT_EQ(error::BAD_ARGUMENT, DelSubTreeFromApp(fake, &old_tree));

    ASSERT_EQ(error::OK, DelSubTreeFromApp(sub_tree, &old_tree));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DelSubTreeFromApp_suite, TestDelSubTree)
{
    TestDelSubTree();
}

/**
 * @brief
**/
class test_MergeImporterIntoApp_suite : public ::testing::Test {
protected:
    test_MergeImporterIntoApp_suite() {};
    virtual ~test_MergeImporterIntoApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MergeImporterIntoApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MergeImporterIntoApp_suite, *)
    };
    void TestMergeImporter();
};

void test_MergeImporterIntoApp_suite::TestMergeImporter() {
    Application old_tree;
    Application importer;
    Application fake;
    ConstructFakeAppClient(&old_tree);
    ConstructFakeImporter(true, &importer);

    // test of wrong app name
    fake.CopyFrom(importer);
    fake.set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, MergeImporterIntoApp(fake, &old_tree));

    // test of new tp is NULL
    /*Topology* topo = old_tree.release_topology();
    delete topo;
    ASSERT_EQ(error::BAD_ARGUMENT, MergeImporterIntoApp(&old_tree, sub_tree));
    */

    // test of check topology fail
    fake.Clear();
    fake.CopyFrom(importer);
    Subscribe* sub = fake.mutable_topology()->mutable_processors(1)->mutable_subscribe();
    sub->mutable_processor(0)->set_name("wrong");
    ASSERT_EQ(error::BAD_ARGUMENT, MergeImporterIntoApp(fake, &old_tree));

    // test of OK
    old_tree.Clear();
    ConstructFakeAppClient(&old_tree);
    ASSERT_EQ(error::OK, MergeImporterIntoApp(importer, &old_tree));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MergeImporterIntoApp_suite, TestMergeImporter)
{
    TestMergeImporter();
}

/**
 * @brief
**/
class test_DelImporterFromApp_suite : public ::testing::Test {
protected:
    test_DelImporterFromApp_suite() {};
    virtual ~test_DelImporterFromApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DelImporterFromApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DelImporterFromApp_suite, *)
    };
    void TestDelImporter();
};

void test_DelImporterFromApp_suite::TestDelImporter() {
    Application old_tree;
    Application importer;
    Application fake;
    ConstructFakeAppClient(&old_tree);
    ConstructFakeImporter(false, &importer);

    // test of wrong app name
    fake.CopyFrom(importer);
    fake.set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, DelImporterFromApp(fake, &old_tree));

    // test of check importer fail
    fake.CopyFrom(importer);
    fake.mutable_topology()->mutable_processors(0)->set_name("test_process_4");;
    ASSERT_EQ(error::BAD_ARGUMENT, DelImporterFromApp(fake, &old_tree));

    // test of new tp is NULL
    fake.Clear();
    fake.CopyFrom(old_tree);
    Topology* topo = fake.release_topology();
    delete topo;
    ASSERT_EQ(error::BAD_ARGUMENT, DelImporterFromApp(importer, &fake));

    ASSERT_EQ(error::OK, DelImporterFromApp(importer, &old_tree));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DelImporterFromApp_suite, TestDelImporter)
{
    TestDelImporter();
}

/**
 * @brief
**/
class test_ClearOldTag_suite : public ::testing::Test {
protected:
    test_ClearOldTag_suite() {};
    virtual ~test_ClearOldTag_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ClearOldTag_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ClearOldTag_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ClearOldTag_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_UpdateParallelismFromApp_suite : public ::testing::Test {
protected:
    test_UpdateParallelismFromApp_suite() {};
    virtual ~test_UpdateParallelismFromApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_UpdateParallelismFromApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_UpdateParallelismFromApp_suite, *)
    };
    void TestUpdatePara();
};

void test_UpdateParallelismFromApp_suite::TestUpdatePara() {
    Application old_tree;
    Application update;
    ConstructFakeAppClient(&old_tree);
    ConstructFakeUpPara(&update);

    ASSERT_EQ(error::OK, UpdateParallelismFromApp(update, &old_tree));

    // test of different name
    update.set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, UpdateParallelismFromApp(update, &old_tree));

    // test of old tree no Topology
    Topology* topo = old_tree.release_topology();
    delete topo;
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, UpdateParallelismFromApp(update, &old_tree));

    // processor num does not match
    update.mutable_topology()->set_processor_num(256);
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, UpdateParallelismFromApp(update, &old_tree));

    // processor not exist
    update.mutable_topology()->mutable_processors(0)->set_name("wrong");
    ASSERT_EQ(error::CLIENT_MERGER_TOPO_FAIL, UpdateParallelismFromApp(update, &old_tree));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_UpdateParallelismFromApp_suite, TestUpdatePara)
{
    TestUpdatePara();
}

/**
 * @brief
**/
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
    void TestCheckTopology();
};

void test_CheckTopology_suite::TestCheckTopology() {
    Application app;
    ConstructFakeAppClient(&app);

    // test of process name conflict
    Application fake;
    fake.CopyFrom(app);
    int processor_size = fake.mutable_topology()->processors_size();
    for (int i = 0; i < processor_size; i++) {
        if (fake.mutable_topology()->processors(i).name() == "test_process_2") {
            fake.mutable_topology()->mutable_processors(i)->set_name("test_process_1");
        }
    }
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckTopology(fake, false));

    // test of init app to dag fail
    Application fake2;
    fake2.CopyFrom(app);
    fake2.mutable_topology()->set_processor_num(256);
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckTopology(fake2, false));

    // test of processor has no subscribe
    Application fake3;
    fake3.CopyFrom(app);
    for (int i = 0; i < processor_size; i++) {
        if (fake3.mutable_topology()->processors(i).name() == "test_process_2") {
            Subscribe* sub = fake3.mutable_topology()->mutable_processors(i)->release_subscribe();
            delete sub;
        }
    }
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckTopology(fake3, false));


    // test of subscribe nonexist processor
    Application fake4;
    fake4.CopyFrom(app);
    for (int i = 0; i < processor_size; i++) {
        if (fake4.mutable_topology()->processors(i).name() == "test_process_2") {
            Subscribe* sub = fake4.mutable_topology()->mutable_processors(i)->mutable_subscribe();
            sub->mutable_processor(0)->set_name("non_exist");
        }
    }
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckTopology(fake4, true));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CheckTopology_suite, TestCheckTopology)
{
    TestCheckTopology();
}

/**
 * @brief
**/
class test_CheckImporter_suite : public ::testing::Test {
protected:
    test_CheckImporter_suite() {};
    virtual ~test_CheckImporter_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CheckImporter_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CheckImporter_suite, *)
    };
    void TestCheckImporter();
};

void test_CheckImporter_suite::TestCheckImporter() {
    Application app;
    ConstructFakeAppClient(&app);
    Application importer_add, importer_del;
    ConstructFakeImporter(true, &importer_add);
    ConstructFakeImporter(false, &importer_del);

    // test of processor name not unique
    Application importer_fake1;
    importer_fake1.CopyFrom(importer_add);
    Processor* wrong_pro = importer_fake1.mutable_topology()->add_processors();
    wrong_pro->set_name("test_process_3");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake1, ADD_IMPORTER));


    // test of add an existed importer
    Application importer_fake2;
    importer_fake2.CopyFrom(importer_add);
    importer_fake2.mutable_topology()->mutable_processors(0)->set_name("test_process_0");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake2, ADD_IMPORTER));

    // test of del an unexisted importer
    Application importer_fake3;
    importer_fake3.CopyFrom(importer_add);
    importer_fake3.mutable_topology()->mutable_processors(0)->set_name("wrong");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake3, DEL_IMPORTER));

    // test of subscribe pe not exist
    Application importer_fake4;
    importer_fake4.CopyFrom(importer_add);
    importer_fake4.mutable_topology()->mutable_processors(1)->set_name("wrong");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake4, ADD_IMPORTER));

    // test of subscribe an unexisted importer
    Application importer_fake5;
    importer_fake5.CopyFrom(importer_add);
    Subscribe* sub = importer_fake5.mutable_topology()->mutable_processors(1)->mutable_subscribe();
    sub->mutable_processor(0)->set_name("wrong");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake5, ADD_IMPORTER));

    // test of modify subscribe orignal importer
    /*Application importer_fake6;
    importer_fake6.CopyFrom(importer_add);
    sub = importer_fake5.mutable_topology()->mutable_processors(1)->mutable_subscribe();
    sub->mutable_processor(0)->set_name("test_process_1");
    ASSERT_EQ(error::CLIENT_CHECK_TP_FAIL, CheckImporter(&app, importer_fake6, ADD_IMPORTER));
    */
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CheckImporter_suite, TestCheckImporter)
{
    TestCheckImporter();
}

/**
 * @brief
**/
class test_BuildApp_suite : public ::testing::Test {
protected:
    test_BuildApp_suite() {};
    virtual ~test_BuildApp_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_BuildApp_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_BuildApp_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BuildApp_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_ConstructProcessorID_suite : public ::testing::Test {
protected:
    test_ConstructProcessorID_suite() {};
    virtual ~test_ConstructProcessorID_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ConstructProcessorID_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ConstructProcessorID_suite, *)
    };
    void TestConstructProcessorID();
};

void test_ConstructProcessorID_suite::TestConstructProcessorID() {
    Application app;
    ConstructFakeAppClient(&app);
    Topology* tp = app.mutable_topology();
    ASSERT_EQ(3, ConstructProcessorID(tp));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ConstructProcessorID_suite, TestConstructProcessorID)
{
    TestConstructProcessorID();
}

class test_persist_progress_suite : public ::testing::Test {
protected:
    test_persist_progress_suite() {};
    virtual ~test_persist_progress_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
public:
};

TEST_F(test_persist_progress_suite, test_read_persist_progress)
{
    // read true
    std::string app_dir = "topo_check_test_persist_progress";
    Application app;
    int ret = InitApp(app_dir, false, false, false, false, &app);
    ASSERT_TRUE(ret == 0);

    Processor importer = app.topology().processors(0);
    ASSERT_TRUE(importer.has_persist_progress());
    ASSERT_TRUE(importer.persist_progress() == "true");

    // read false
    Application app2;
    app_dir = "topo_check_test_persist_progress_false";
    ret = InitApp(app_dir, false, false, false, false, &app2);
    ASSERT_TRUE(ret == 0);

    Processor importer2 = app2.topology().processors(0);
    ASSERT_TRUE(importer2.has_persist_progress());
    ASSERT_TRUE(importer2.persist_progress() == "false");
}
