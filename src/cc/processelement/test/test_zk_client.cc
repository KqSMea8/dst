#include "processelement/zk_client.h"
#include <gtest/gtest.h>

using namespace dstream;
using namespace processelement;

const char* kPEZKDefaultConfFile = "./test_pe_progress.conf.xml";

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_zk_client");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_ZKProgress_Init_suite : public ::testing::Test {
protected:
    test_ZKProgress_Init_suite() {};
    virtual ~test_ZKProgress_Init_suite() {};
    virtual void SetUp();
    virtual void TearDown();
    void TestInit();

private:
    config::Config conf_;
    std::string    config_file_;
    uint64_t       pe_id_;
    ZKProgress*    zk_progress_;
};

void test_ZKProgress_Init_suite::SetUp() {
    if ("" == config_file_) {
        config_file_ = kPEZKDefaultConfFile;
    }
    conf_.ReadConfig(config_file_);
    pe_id_ = 123456789;
}

void test_ZKProgress_Init_suite::TearDown() {
    if (NULL != zk_progress_) {
        delete zk_progress_;
        zk_progress_ = NULL;
    }
}

void test_ZKProgress_Init_suite::TestInit() {
    zk_progress_ = new (std::nothrow) ZKProgress();
    if (NULL == zk_progress_) {
        DSTREAM_WARN("allocate zk progress object fail");
        return ;
    }
    std::string path("test-path");
    ASSERT_EQ(error::OK, zk_progress_->Init(pe_id_, path, &conf_));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKProgress_Init_suite, TestInit)
{
    TestInit();
}

/**
 * @brief
**/
class test_ZKProgress_GetProgress_suite : public ::testing::Test {
protected:
    test_ZKProgress_GetProgress_suite() {};
    virtual ~test_ZKProgress_GetProgress_suite() {};
    virtual void SetUp();
    virtual void TearDown();
    void TestGetProgress();
private:
    config::Config conf_;
    std::string    config_file_;
    uint64_t       pe_id_;
    ZKProgress*    zk_progress_;
    std::string    path_;
};

void test_ZKProgress_GetProgress_suite::SetUp() {
    if ("" == config_file_) {
        config_file_ = kPEZKDefaultConfFile;
    }
    conf_.ReadConfig(config_file_);
    pe_id_ = 123456789;
    path_ = "test-path";
}

void test_ZKProgress_GetProgress_suite::TearDown() {
    if (NULL != zk_progress_) {
        delete zk_progress_;
        zk_progress_ = NULL;
    }
}

void test_ZKProgress_GetProgress_suite::TestGetProgress() {
    zk_progress_ = new (std::nothrow) ZKProgress();
    if (NULL == zk_progress_) {
        DSTREAM_WARN("allocate zk progress object fail");
        return ;
    }
    ASSERT_EQ(error::OK, zk_progress_->Init(pe_id_, path_, &conf_));

    SubPoint point;
    point.set_pipelet(1);
    point.set_msg_id(12345);
    point.set_seq_id(67890);

    ASSERT_EQ(error::OK, zk_progress_->SetProgress(point));

    SubPoint zk_point;
    zk_point.set_pipelet(1);
    ASSERT_EQ(error::OK, zk_progress_->GetProgress(&zk_point));
    ASSERT_EQ(12345u, zk_point.msg_id());
    ASSERT_EQ(67890u, zk_point.seq_id());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKProgress_GetProgress_suite, TestGetProgress)
{
    TestGetProgress();
}

/**
 * @brief
**/
class test_ZKProgress_SetProgress_suite : public ::testing::Test {
protected:
    test_ZKProgress_SetProgress_suite() {};
    virtual ~test_ZKProgress_SetProgress_suite() {};
    virtual void SetUp();
    virtual void TearDown();
    virtual void TestSetProgress();
private:
    config::Config conf_;
    std::string    config_file_;
    uint64_t       pe_id_;
    ZKProgress*    zk_progress_;
    std::string    path_;
};

void test_ZKProgress_SetProgress_suite::SetUp() {
    if ("" == config_file_) {
        config_file_ = kPEZKDefaultConfFile;
    }
    conf_.ReadConfig(config_file_);
    pe_id_ = 123456789;
    path_ = "test-path";
}

void test_ZKProgress_SetProgress_suite::TearDown() {
    if (NULL != zk_progress_) {
        delete zk_progress_;
        zk_progress_ = NULL;
    }
}

void test_ZKProgress_SetProgress_suite::TestSetProgress() {
    zk_progress_ = new (std::nothrow) ZKProgress();
    if (NULL == zk_progress_) {
        DSTREAM_WARN("allocate zk progress object fail");
        return ;
    }
    ASSERT_EQ(error::OK, zk_progress_->Init(pe_id_, path_, &conf_));

    SubPoint point;
    point.set_pipelet(1);
    point.set_msg_id(12345);
    point.set_seq_id(67890);
    ASSERT_EQ(error::OK, zk_progress_->SetProgress(point));

    SubPoint zk_point;
    zk_point.set_pipelet(1);
    ASSERT_EQ(error::OK, zk_progress_->GetProgress(&zk_point));
    ASSERT_EQ(12345u, zk_point.msg_id());
    ASSERT_EQ(67890u, zk_point.seq_id());

    point.set_msg_id(67890);
    point.set_seq_id(12345);
    ASSERT_EQ(error::OK, zk_progress_->SetProgress(point));

    ASSERT_EQ(error::OK, zk_progress_->GetProgress(&zk_point));
    ASSERT_EQ(67890u, zk_point.msg_id());
    ASSERT_EQ(12345u, zk_point.seq_id());
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKProgress_SetProgress_suite, TestSetProgress)
{
    TestSetProgress();
}

/**
 * @brief
**/
class test_ZKRegister_Init_suite : public ::testing::Test {
protected:
    test_ZKRegister_Init_suite() {};
    virtual ~test_ZKRegister_Init_suite() {};
    virtual void SetUp() {
        zk_register_ = new (std::nothrow) ZKRegister();
    }
    virtual void TearDown() {
        if (NULL != zk_register_) {
            delete zk_register_;
            zk_register_ = NULL;
        }
    }
    void TestConfigFail();
    void TestInitSucc();
private:
    uint64_t       pe_id_;
    std::string    app_name_;
    std::string    config_file_;
    int            port_;
    ZKRegister*    zk_register_;
};

void test_ZKRegister_Init_suite::TestConfigFail() {
    pe_id_ = 1;
    app_name_ = "test";
    config_file_ = "./conf/test_pe_progress_bad_zk.conf.xml";
    port_ = 1;
    ASSERT_EQ(error::ZK_INIT_ERROR,
              zk_register_->Init(pe_id_, app_name_, config_file_, port_));
}

void test_ZKRegister_Init_suite::TestInitSucc() {
    pe_id_ = 1;
    app_name_ = "test";
    config_file_ = "./conf/test_pe_progress.conf.xml";
    port_ = 1;
    ASSERT_EQ(error::OK,
              zk_register_->Init(pe_id_, app_name_, config_file_, port_));
    meta_manager::MetaManager::GetMetaManager()->DelScribeImporter("test");
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKRegister_Init_suite, test_config_fail)
{
    TestConfigFail();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ZKRegister_Init_suite, test_init_succ)
{
    TestInitSucc();
}
