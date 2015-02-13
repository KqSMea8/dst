#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/utils.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/zk_client.h"

#include "processnode/config_map.h"
#include "processnode/rpc_port_manager.h"

using namespace std;
using namespace dstream;
using namespace dstream::processnode;

namespace dstream {
namespace processnode {

class TestRpcPortManager : public ::testing::Test
{
protected:
    void RpcPortManagerTest(const uint32_t& num);
};

void TestRpcPortManager::RpcPortManagerTest(const uint32_t& num)
{
    RpcPortManager port_manager(1000, num);
    for (int i = 0; i < num; ++i) {
        uint32_t port = port_manager.getNextPERpcPort();
        DSTREAM_DEBUG("next port : %lu", port);
        ASSERT_EQ(1000 + i, port);
    }
    /* there is no port to assign */
    ASSERT_EQ(-1, port_manager.getNextPERpcPort());

    /* put some ports back */
    port_manager.putBackPERpcPort(1000);
    port_manager.putBackPERpcPort(1009);
    port_manager.putBackPERpcPort(1100);

    ASSERT_EQ(1000, port_manager.getNextPERpcPort());
    ASSERT_EQ(1009, port_manager.getNextPERpcPort());
    ASSERT_EQ(1100, port_manager.getNextPERpcPort());

    /* there is no port to assign, again */
    ASSERT_EQ(-1, port_manager.getNextPERpcPort());

}



TEST_F(TestRpcPortManager, test_rpc_port_manager)
{
    RpcPortManagerTest(1024);
}


} // end processnode
} // end dstream

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_rpc_port_manager");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
