/**
* @brief  zk client mock
* @author fangjun, fangjun02@baidu.com
* @date   2013-01-29
*/
#ifndef __DSTREAM_CC_CLIENT_TEST_MOCK_ZK_CLIENT_H__
#define __DSTREAM_CC_CLIENT_TEST_MOCK_ZK_CLIENT_H__

#include "common/zk_client.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace dstream {
namespace client {

class MockZkClient : public zk_client::ZkClient {
public:
    ~MockZkClient() {}
    MOCK_METHOD0(Connect, error::Code());
    MOCK_METHOD2(SystemRootExists, error::Code(const char*, int32_t*));
    MOCK_METHOD1(InitSystemRoot, error::Code(const char*));
    MOCK_METHOD2(InitSystemRoot, error::Code(const char*, std::vector<zk_client::ZkNode*>));
    MOCK_METHOD2(CreateEphemeralNode, error::Code(const char*, zk_client::ZkNode*));
    MOCK_METHOD3(CreateEphemeralNode, error::Code(const char*, zk_client::ZkNode*, int32_t));
    MOCK_METHOD1(DeleteEphemeralNode, error::Code(const char*));
    MOCK_METHOD2(GetNodeData, error::Code(const char*, zk_client::ZkNodeData*));
    MOCK_METHOD1(SetConnBrokenWatcher, error::Code(zk_client::Watcher*));
    MOCK_METHOD2(GetNodeChildren, error::Code(const char*, std::list<zk_client::ZkNode*>*));
};

}//namespace client
}//namespace dstream
#endif
