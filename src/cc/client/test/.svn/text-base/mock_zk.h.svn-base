/**
* @brief  zk operation mock
* @author fangjun, fangjun02@baidu.com
* @date   2013-01-29
*/
#ifndef __DSTREAM_CC_CLIENT_TEST_MOCK_ZK_H__
#define __DSTREAM_CC_CLIENT_TEST_MOCK_ZK_H__

#include "client/zk.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace dstream {
namespace client {

class MockZK : public ZK {
public:
    MOCK_METHOD0(Connection, int32_t());
    MOCK_METHOD1(Register, int32_t(User*));
    MOCK_METHOD0(UnRegister, int32_t());
    MOCK_METHOD3(GetPM, int32_t(uint32_t, uint32_t, PM*));
    MOCK_METHOD4(GetApp, int32_t(const std::string&, uint32_t, uint32_t, Application*));
    MOCK_METHOD2(GetAppID, int32_t(const std::string&, int64_t*));
};

}//namespace client
}//namespace dstream
#endif
