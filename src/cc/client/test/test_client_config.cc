#include "client/client_config.h"
#include <gtest/gtest.h>


using namespace dstream;
using namespace client;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_client_config");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief  no need to test
**/
/*class test_ClientConfigMap_ClientConfigMap_suite : public ::testing::Test{
    protected:
        test_ClientConfigMap_ClientConfigMap_suite(){};
        virtual ~test_ClientConfigMap_ClientConfigMap_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_ClientConfigMap_ClientConfigMap_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_ClientConfigMap_ClientConfigMap_suite, *)
        };
};*/

/**
 * @brief
 * @begin_version
**/
/*TEST_F(test_ClientConfigMap_ClientConfigMap_suite, case_name1)
{
    //TODO
}*/

/**
 * @brief
**/
class test_ClientConfigMap_InitCfgMapFromFile_suite : public ::testing::Test {
protected:
    test_ClientConfigMap_InitCfgMapFromFile_suite() {};
    virtual ~test_ClientConfigMap_InitCfgMapFromFile_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ClientConfigMap_InitCfgMapFromFile_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ClientConfigMap_InitCfgMapFromFile_suite, *)
    };
    void TestInitCfgMapFromFile();
};

void test_ClientConfigMap_InitCfgMapFromFile_suite::TestInitCfgMapFromFile() {
    ASSERT_EQ(error::OK, g_cfg_map.InitCfgMapFromFile("dstream.cfg.xml"));
    ASSERT_NE(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_fake.cfg.xml"));

    // test of get zk root path fail
    ASSERT_EQ(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_without_zk_root.cfg.xml"));

    // test of get local host ip
    ASSERT_EQ(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_with_localhost.cfg.xml"));

    // test of get zk path fail
    ASSERT_NE(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_without_zk_path.cfg.xml"));

    // test of get local host ip fail
    //ASSERT_NE(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_fail_localhost.cfg.xml"));

    // test of get client user fail
    ASSERT_NE(error::OK, g_cfg_map.InitCfgMapFromFile("dstream_without_client.cfg.xml"));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ClientConfigMap_InitCfgMapFromFile_suite, TestInitCfgMapFromFile)
{
    TestInitCfgMapFromFile();
}

/**
 * @brief  no need to test
**/
class test_ClientConfigMap_Dump_suite : public ::testing::Test {
protected:
    test_ClientConfigMap_Dump_suite() {};
    virtual ~test_ClientConfigMap_Dump_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ClientConfigMap_Dump_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ClientConfigMap_Dump_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ClientConfigMap_Dump_suite, TestDump)
{
    ASSERT_EQ(error::OK, g_cfg_map.InitCfgMapFromFile("dstream.cfg.xml"));
    g_cfg_map.Dump();
}

