/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file mock_zk_wrapper.h
 * @author lanbijia
 * @date 2012/12/28
 * @brief mock class ZkWrapper
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_ZK_WRAPPER_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_ZK_WRAPPER_H__

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mocked class
#include "processnode/zk_operator.h"

namespace dstream {
namespace router {

class MockZkWrapper : public router::ZkWrapper
{
public:
    MOCK_METHOD3(Init, error::Code(const config::Config&, router::ZkOperator::WatchEventCallback,
                                   void*));
    MOCK_METHOD0(Destroy, void());
    MOCK_METHOD1(GetPM, error::Code(PM*));
    MOCK_METHOD2(GetPN, error::Code(const PNID&, PN*));
    MOCK_METHOD1(AddPN, error::Code(const PN*));
    MOCK_METHOD1(DeletePN, error::Code(const PN&));
    MOCK_METHOD2(GetApplication, error::Code(const AppID&, Application*));
    MOCK_METHOD2(GetPEList, error::Code(const AppID&, std::vector<std::string>*));
    MOCK_METHOD3(GetProcessorElement, error::Code(const AppID&, const PEID&, ProcessorElement*));
};

} // end of router
} // end of dstream

#endif
