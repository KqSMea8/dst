/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test for application tools
*********************************************************************/

#include "common/application_tools.h"
#include "common/hdfs_client.h"
#include "gtest/gtest.h"

namespace dstream {
namespace test_application_tools {

using namespace dstream::hdfs_client;
using namespace application_tools;

TEST(test_app_tools, test_lower_str)
{
    ASSERT_STREQ(LowerString("Hdfs").c_str(), "hdfs");
}

TEST(test_app_tools, test_get_process_info)
{
    std::map<pid_t, Resource> res_map;
    GetProcessInfo(&res_map);
    ASSERT_GT(res_map.size(), 0u);
    res_map.clear();
    char buffer[1024 * 1024];
    GetProcessInfo(&res_map, buffer);
    ASSERT_GT(res_map.size(), 0u);
}

std::string test_buffer;
int32_t TestExecutor(const std::string& cmd, int retry, char* buf, int* ret)
{
    (void)(cmd);
    (void)(retry);
    (void)(ret);
    memcpy(buf, test_buffer.c_str(), test_buffer.size());
    buf[test_buffer.size()] = '\0';
    return 0;
}

void dump_res_map(std::map<pid_t, Resource>& res_map)
{
    std::map<pid_t, Resource>::iterator iter = res_map.begin();
    for (; iter != res_map.end(); ++iter) {
        printf("pid: %d, cpu: %lf, mem: %ld\n", iter->first, iter->second.cpu(), iter->second.memory());
    }
}

TEST(test_app_tools, test_get_cg_process_info)
{
    std::map<pid_t, Resource> res_map;
    GetCGProcessInfo(&res_map);
    dump_res_map(res_map);
}

}//namespace test_application_tools
}//namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("processnode");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
