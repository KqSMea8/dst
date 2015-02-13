#ifndef __DSTREAM_CC_CLIENT_TEST_HDFS_CLIENT_H__
#define __DSTREAM_CC_CLIENT_TEST_HDFS_CLIENT_H__

#include "common/hdfs_client.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace dstream {
namespace hdfs_client {

class MockHdfsClient : public HdfsClient {
public:
    MOCK_CONST_METHOD3(UploadFile, int32_t(const std::string&, const std::string&, bool));
    MOCK_CONST_METHOD2(DownloadFile, int32_t(const std::string&, const std::string&));
    MOCK_CONST_METHOD2(ListFile, int32_t(const std::string&, std::vector<std::string>&));
    MOCK_CONST_METHOD1(RemoveFile, int32_t(const std::string&));
    MOCK_CONST_METHOD1(MakeFile, int32_t(const std::string&));
    MOCK_CONST_METHOD2(MoveFile, int32_t(const std::string&, const std::string&));
    MOCK_CONST_METHOD1(TestAndRemoveDir, int32_t(const std::string&));
    MOCK_CONST_METHOD2(CopyOverWriteFile, int32_t(const std::string&, const std::string&));
};

}//namespace hdfs_client
}//namespace dstream
#endif
