/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

#include <processnode/prolog.h>
#include <common/logger.h>
#include <gtest/gtest.h>

using namespace dstream;
using namespace dstream::processnode;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_prolog");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_Prolog_suite : public ::testing::Test {
protected:
    test_Prolog_suite() {};
    virtual ~test_Prolog_suite() {};
    virtual void SetUp() {
        peid = 0x1234;
        workdir = ".";
        last_time = time(NULL);
        exe = "test-bin";
    };
    virtual void TearDown() {
    };

    uint64_t peid;
    std::string workdir;
    uint64_t last_time;
    std::string exe;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_Prolog_suite, test_Prolog_normal)
{
    exe = "prolog_normal";
    ASSERT_EQ(error::OK, Prolog::RunProlog(peid, workdir, last_time, exe));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_Prolog_suite, test_Prolog_timeout)
{
    exe = "prolog_timeout";
    ASSERT_EQ(error::CHILD_WAIT_TIMEOUT, Prolog::RunProlog(peid, workdir, last_time, exe));
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_Prolog_suite, test_RunProlog__1)
{
    exe = "prolog_recur_timeout";
    ASSERT_EQ(error::CHILD_WAIT_TIMEOUT, Prolog::RunProlog(peid, workdir, last_time, exe));
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
