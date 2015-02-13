#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/utils.h"

using namespace dstream;


void TestGetLocalHostIP()
{
    std::string ip;
    GetLocalhostIP(&ip);
    ASSERT_TRUE(ip != "127.0.0.1");
}

void TestGetConfigMemory() {
    std::string memory = "12345";
    uint64_t config_memory = 12345;
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345K";
    config_memory *= 1024;
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345KB";
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345MB";
    config_memory *= 1024;
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345M";
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345G";
    config_memory *= 1024;
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "12345GB";
    ASSERT_EQ(GetConfigMemory(&memory), config_memory);
    memory = "asdf";
    ASSERT_EQ(GetConfigMemory(&memory), (uint64_t) - 1);
}

TEST(TestUtil, TestGetConfigMemory)
{
    TestGetConfigMemory();
    TestGetLocalHostIP();
}

TEST(TestUtil, TestHasExecutedFiles) {
    ASSERT_EQ(
        HasExecutedFiles("../../client/test/topo_check_test_incorrect_topo_without_executed_files/echo_exporter"),
        false);
    ASSERT_EQ(
        HasExecutedFiles("../../client/test/topo_check_test_correct_complicated_topo/echo_exporter"),
        true);
}

TEST(TestUtil, TestGetRemoteIP) {
    std::string ip;
    ASSERT_EQ(error::OK, GetRemotehostIP("svn.baidu.com", ip));
}

int main(int argc, char** argv) {
    logger::initialize("test_util");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
