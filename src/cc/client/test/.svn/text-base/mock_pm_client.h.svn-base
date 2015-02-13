/**
* @brief  pm client operation mock
* @author fangjun, fangjun02@baidu.com
* @date   2013-02-03
*/
#ifndef __DSTREAM_CC_CLIENT_TEST_MOCK_PM_CLIENT_H__
#define __DSTREAM_CC_CLIENT_TEST_MOCK_PM_CLIENT_H__

#include "client/pm_client.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace dstream {
namespace client {

typedef std::set<User, application_tools::CompareUser> USER_SET;
class MockPMClient : public PMClient {
public:
    MOCK_METHOD2(GetAppTopology, int32_t(const std::string&, Application&));
    MOCK_METHOD1(SubmitApp, int32_t(Application*));
    MOCK_METHOD1(DecommissionApp, int32_t(const std::string));
    MOCK_METHOD1(DecommissionApp, int32_t(const int64_t));
    MOCK_METHOD0(GetUser, USER_SET());
    MOCK_METHOD2(UpdateUser, int32_t(const std::string, const std::string));
    MOCK_METHOD2(AddUser, int32_t(const std::string, const std::string));
    MOCK_METHOD1(DeleteUser, int32_t(const std::string));
    MOCK_METHOD1(AddSubTree, int32_t(const Application&));
    MOCK_METHOD1(DelSubTree, int32_t(const Application&));
    MOCK_METHOD1(AddImporter, int32_t(const Application&));
    MOCK_METHOD1(DelImporter, int32_t(const Application&));
    MOCK_METHOD1(UpdateParallelism, int32_t(const Application&));
    MOCK_METHOD3(FlowControlApp, int32_t(const int64_t, const std::string&, const std::string&));
    MOCK_METHOD3(FlowControlApp, int32_t(const std::string, const std::string&, const std::string&));
    MOCK_METHOD0(FlowControlInfo, std::string());
    MOCK_METHOD1(UpdateProcessor, int32_t(const Application&));
};

}//namespace client
}//namespace dstream
#endif
