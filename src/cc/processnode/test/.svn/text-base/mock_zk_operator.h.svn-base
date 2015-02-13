/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file mock_zk_operator.h
 * @author lanbijia
 * @date 2012/12/27
 * @brief mock class ZkOperator
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_ZK_OPERATOR_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_ZK_OPERATOR_H__

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mocked class
#include "processnode/zk_operator.h"

namespace dstream {
namespace router {

class MockZkOperator : public ZkOperator
{
public:
    MOCK_METHOD0(Connect, error::Code());
    MOCK_METHOD0(IsConnected, bool());
    MOCK_METHOD0(Disconnect, void());
    MOCK_METHOD3(NodeExists, error::Code(const char*, int32_t*, int32_t*));
    MOCK_METHOD2(GetNodeData, error::Code(const char*, zk_meta_manager::MetaZKData*&));
    MOCK_METHOD2(SetNodeData, error::Code(const char*, zk_meta_manager::MetaZKData&));
    MOCK_METHOD2(CreatePersistentNode, error::Code(const char*, zk_client::ZkNode*));
    MOCK_METHOD1(DeletePersistentNode, error::Code(const char*));
    MOCK_METHOD2(CreateEphemeralNode, error::Code(const char*, zk_client::ZkNode*));
    MOCK_METHOD3(CreateEphemeralNode, error::Code(const char*, zk_client::ZkNode*, int32_t));
    MOCK_METHOD1(DeleteEphemeralNode, error::Code(const char*));
    MOCK_METHOD2(GetChildList, error::Code(const char*, std::vector<std::string>*));
    MOCK_METHOD2(SetWatchEventCallBack, void(WatchEventCallback, void*));
};

} // end of router
} // end of dstream

#endif
