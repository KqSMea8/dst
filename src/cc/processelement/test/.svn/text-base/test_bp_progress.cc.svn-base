/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

#include <common/utils.h>
#include <common/error.h>
#include "processelement/test/mock_zk_progress.h"
#include <gtest/gtest.h>
#include "common/logger.h"
#include "common/id.h"

#include <processelement/bp_progress.h>

using ::testing::_;
using ::testing::Return;

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_task_incounter");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_BPProgress_suite : public ::testing::Test {
protected:
    test_BPProgress_suite() {};
    virtual ~test_BPProgress_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_BPProgress_suite, *)
        appname = "dstream-test";
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_BPProgress_suite, *)
    };
    BPProgress progress;
    std::string appname;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_read__1)
{
    SubPoint vpoint;
    vpoint.set_pipelet(1);
    vpoint.set_msg_id(0x1234);
    vpoint.set_seq_id(0x2345);
    progress.m_sub_point[vpoint.pipelet()] = vpoint;

    std::vector<SubPoint> sub_point_list;
    progress.read(&sub_point_list);
    ASSERT_TRUE(sub_point_list.size() == 1);

    SubPoint& dpoint = sub_point_list[0];
    ASSERT_TRUE(dpoint.pipelet() == 1);
    ASSERT_TRUE(dpoint.msg_id() == 0x1234);
    ASSERT_TRUE(dpoint.seq_id() == 0x2345);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_update__1)
{
    SubPoint vpoint;
    vpoint.set_pipelet(1);
    vpoint.set_msg_id(0x1234);
    vpoint.set_seq_id(0x2345);

    progress.update(vpoint);
    ASSERT_TRUE(progress.m_sub_point.size() == 1);
    SubPoint& dpoint = progress.m_sub_point[1];
    ASSERT_TRUE(dpoint.pipelet() == 1);
    ASSERT_TRUE(dpoint.msg_id() == 0x1234);
    ASSERT_TRUE(dpoint.seq_id() == 0x2345);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_save_not_init)
{
    int ret = progress.save();
    ASSERT_TRUE(ret != 0);
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_save__1)
{
    // init ok, then deinit
    std::string conf("./conf/test_pe_progress.conf.xml");
    int ret = progress.Init(0x1234, appname, conf);
    ASSERT_TRUE(ret == 0);

    const uint32_t kPipeletID = 1;
    const int64_t kMessageID = 0x1234;
    const uint64_t kSequenceID = 0x2345;

    SubPoint vpoint;
    vpoint.set_pipelet(kPipeletID);
    vpoint.set_msg_id(kMessageID);
    vpoint.set_seq_id(kSequenceID);
    progress.update(vpoint);

    // add second pipelet
    vpoint.set_pipelet(kPipeletID + 1);
    vpoint.set_msg_id(kMessageID + 1);
    progress.update(vpoint);

    uint64_t start = (uint64_t)GetTimeUS();
    ret = progress.save();
    uint64_t stop = (uint64_t)GetTimeUS();
    ASSERT_TRUE(ret == 0);
    printf("====>save time consume: %lu\n", stop - start);

    // check
    progress.m_sub_point.clear();
    std::vector<uint32_t> pipelet_list;
    pipelet_list.push_back(kPipeletID);
    pipelet_list.push_back(kPipeletID + 1);

    start = (uint64_t)GetTimeUS();
    ret = progress.load(pipelet_list);
    stop = (uint64_t)GetTimeUS();
    ASSERT_TRUE(ret == 0);
    printf("====>load time consume: %lu\n", stop - start);

    ASSERT_TRUE(progress.m_sub_point.size() == 2);
    ASSERT_TRUE(progress.m_sub_point[kPipeletID].msg_id() == kMessageID);
    ASSERT_TRUE(progress.m_sub_point[kPipeletID + 1].msg_id() == kMessageID + 1);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_save_with_same_appname)
{
    // init ok, then deinit
    std::string conf("./conf/test_pe_progress.conf.xml");
    uint64_t peid = MAKE_PEID(1lu, 2lu, 3lu);
    int ret = progress.Init(peid, appname, conf);
    ASSERT_TRUE(ret == 0);

    const uint32_t kPipeletID = 1;
    const int64_t kMessageID = 0x1234;
    const uint64_t kSequenceID = 0x2345;

    SubPoint vpoint;
    vpoint.set_pipelet(kPipeletID);
    vpoint.set_msg_id(kMessageID);
    vpoint.set_seq_id(kSequenceID);
    progress.update(vpoint);
    ret = progress.save();
    ASSERT_TRUE(ret == 0);

    // same appname, but peid(same processorid+seqid) changed
    BPProgress progress2;
    uint64_t peid2 = MAKE_PEID(4lu, 2lu, 3lu);
    ret = progress2.Init(peid2, appname, conf);

    std::vector<uint32_t> pipelet_list;
    pipelet_list.push_back(kPipeletID);
    ret = progress2.load(pipelet_list);
    ASSERT_TRUE(ret == 0);

    ASSERT_TRUE(progress2.m_sub_point[kPipeletID].msg_id() == kMessageID);
    ASSERT_TRUE(progress2.m_sub_point[kPipeletID].seq_id() == kSequenceID);

    // same appname, diff peid(diff proid, seqid)
    BPProgress progress3;
    uint64_t peid3 = MAKE_PEID(1lu, 2lu, 4lu);
    ret = progress3.Init(peid3, appname, conf);

    ret = progress3.load(pipelet_list);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_load__1)
{
    // has been tested in upper case
    ASSERT_TRUE(true);
}

TEST_F(test_BPProgress_suite, test_load_not_init)
{
    std::vector<uint32_t> pipelet_list;
    pipelet_list.push_back(1);
    int ret = progress.load(pipelet_list);
    ASSERT_TRUE(ret != 0);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_suite, test_GetPollInterval__1)
{
    progress.m_poll_interval = 0x1234;
    uint64_t interval = 0;
    progress.m_zk_progress = (ZKProgress*)0x1234;
    progress.GetPollInterval(&interval);
    printf("m_poll_interval: %lu, interval: %lu\n", progress.m_poll_interval, interval);
    ASSERT_TRUE(progress.m_poll_interval == interval);
}

/**
 * @brief
**/
class test_BPProgress_Init_suite : public ::testing::Test {
protected:
    test_BPProgress_Init_suite() {};
    virtual ~test_BPProgress_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_BPProgress_Init_suite, *)
        appname = "dstream-test";
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_BPProgress_Init_suite, *)
        meta_manager::MetaManager::DestoryMetaManager();
    };
    BPProgress progress;
    std::string appname;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_Init_suite, test_Init_fake_init)
{
    std::string conf("test_pe_progress.conf.xml");
    // fake init
    progress.m_zk_progress = (ZKProgress*)0x1234;
    int ret = progress.Init(0x1234, appname, conf);
    ASSERT_TRUE(ret == error::UNPOSSIBLE);
    progress.m_zk_progress = NULL;
}

TEST_F(test_BPProgress_Init_suite, test_Init_empty_conf)
{
    // conf file empty, use default(not exist)
    int ret = progress.Init(0x1234, appname, "");
    ASSERT_TRUE(ret == error::CONFIG_BAD_FILE);
}

TEST_F(test_BPProgress_Init_suite, test_Init_no_poll_interval)
{
    // no poll interval, use default
    std::string conf("./conf/test_pe_progress_no_interval.conf.xml");
    int ret = progress.Init(0x1234, appname, conf);
    ASSERT_TRUE(ret == 0);
    ASSERT_TRUE(progress.m_poll_interval == 100);
}

TEST_F(test_BPProgress_Init_suite, test_Init_has_interval)
{
    // user specified interval, do not modify conf file
    std::string conf("./conf/test_pe_progress.conf.xml");
    int ret = progress.Init(0x1234, appname, conf);
    ASSERT_TRUE(ret == 0);
    ASSERT_TRUE(progress.m_poll_interval == 12345);
}

TEST_F(test_BPProgress_Init_suite, test_Init_bad_zk)
{
    BPProgress progress2;
    std::string conf("./conf/test_pe_progress_bad_zk.conf.xml");
    int ret = progress2.Init(0x1234, appname, conf);
    printf("ret code: %d\n", ret);
    ASSERT_TRUE(ret == error::ZK_INIT_ERROR);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_Init_suite, test_Uninit__1)
{
    // init ok, then deinit
    std::string conf("./conf/test_pe_progress.conf.xml");
    int ret = progress.Init(0x1234, appname, conf);
    ASSERT_TRUE(ret == 0);
    progress.Uninit();
    ASSERT_TRUE(progress.m_zk_progress == NULL);
    ASSERT_TRUE(progress.m_pe_id == 0);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BPProgress_Init_suite, test_BPProgress__1)
{
    ASSERT_TRUE(true);
}

/**
 * @brief
**/
class test_ScribeRegister_Init_suite : public ::testing::Test {
protected:
    test_ScribeRegister_Init_suite() {};
    virtual ~test_ScribeRegister_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ScribeRegister_Init_suite, *)
        sr = new (std::nothrow) ScribeRegister();
        mock_zk_register = new (std::nothrow) MockZKRegister();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ScribeRegister_Init_suite, *)
        if (NULL != sr) {
            delete sr;
            sr = NULL;
        }
        if (NULL != mock_zk_register) {
            delete mock_zk_register;
            mock_zk_register = NULL;
        }
    };
private:
    ScribeRegister* sr;
    MockZKRegister* mock_zk_register;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ScribeRegister_Init_suite, test_Init_fail_init)
{
//    sr->m_zk_register = reinterpret_cast<ZKRegister*>(mock_zk_register);
//    EXPECT_CALL(*mock_zk_register, Init(_, _, _, _))
//    .WillOnce(Return(error::ZK_INIT_ERROR));
//    uint64_t peid = 1;
//    std::string app = "test";
//    std::string conf = "test.xml";
//    int port = 1;
//    ASSERT_EQ(error::ZK_INIT_ERROR, sr->Init(peid, app, conf, port));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_ScribeRegister_Init_suite, test_Init_succ_init)
{
//    sr->m_zk_register = reinterpret_cast<ZKRegister*>(mock_zk_register);
//    EXPECT_CALL(*mock_zk_register, Init(_, _, _, _))
//    .WillOnce(Return(error::OK));
//    uint64_t peid = 1;
//    std::string app = "test";
//    std::string conf = "test.xml";
//    int port = 1;
//    ASSERT_EQ(error::OK, sr->Init(peid, app, conf, port));
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
