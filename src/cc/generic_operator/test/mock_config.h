/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file mock_config.h
 * @author lanbijia
 * @date 2012/9/26
 * @brief mock class Config
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_CONFIG_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_CONFIG_H__

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mocked class
#include "common/error.h"
#include "common/config.h"

namespace dstream {
namespace generic_operator {

using namespace dstream::config;

class MockConfig : public dstream::config::Config
{
public:
    //MOCK_CONST_METHOD2(GetValue, dstream::error::Code(const std::string&, std::string*));
    //MOCK_CONST_METHOD1(GetValue, std::string(const std::string&));
    //MOCK_CONST_METHOD2(GetIntValue, dstream::error::Code(const std::string&, int32_t*));
    MOCK_CONST_METHOD2(GetValue, dstream::error::Code(const std::string&, std::string*));
    //MOCK_METHOD1(GetValue, std::string(const std::string&));
    MOCK_CONST_METHOD2(GetIntValue, dstream::error::Code(const std::string&, int32_t*));
};

} // namespace generic_operator
} // namespace dstream

#endif
