/**
 * @file    test_dstream_client.cc
 * @brief   DStream Client unit cases
 * @author  fangjun, fangjun@baidu.com
 * @version 1.3.0.0
 * @date    2012-08-14
 * @notice   most test cases in this file need an external hdfs and zookeeper,
 *          their configs are configured in the cluster_config.xml
 *          and dstream.cfg.xml
 *          Also those cases interactive with hdfs need a hdfs client and should be
 *          configured proper.
 */

#include <gtest/gtest.h>

#include <client/dstream_client.h>
#include <client/application.h>
#include <client/zk.h>
#include <client/action.h>
#include <client/client_config.h>
#include <metamanager/meta_manager.h>
#include <common/error.h>

TEST(test_is_valid_appname, HandleNoneZeroInput) {
    ASSERT_EQ(dstream::client::is_valid_appname("dstream_baidu"), true);
    ASSERT_EQ(dstream::client::is_valid_appname("*dstream_baidu"), false);
    ASSERT_EQ(dstream::client::is_valid_appname("0dstream_baidu"), false);
    ASSERT_EQ(dstream::client::is_valid_appname("0"), false);
}

TEST(test_is_digit, HandleNoneZeroInput) {
    ASSERT_EQ(dstream::client::is_digit("987437839"), true);
    ASSERT_EQ(dstream::client::is_digit("baidu"), false);
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

class test_Main_suite : public ::testing::Test {
protected:
    test_Main_suite() {};
    virtual ~test_Main_suite() {};
    virtual void SetUp() {
        argv[0] = const_cast<char*>("./dstream_client");
        argv[1] = const_cast<char*>("--conf");
        argv[2] = const_cast<char*>("./dstream.cfg.xml");
    };
    virtual void TeadDown() {};
public:
    int argc;
    char* argv[8];
};


TEST_F(test_Main_suite, MainTest) {
    /*  argc = 8;
      argv[3] = const_cast<char*>("--update-user");
      argv[4] = const_cast<char*>("--username");
      argv[5] = const_cast<char*>("test");
      argv[6] = const_cast<char*>("--password");
      argv[7] = const_cast<char*>("test");
      ASSERT_EQ(dstream::client::main(argc, argv), 2);
    */
}
