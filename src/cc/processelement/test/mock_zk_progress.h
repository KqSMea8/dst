// Copyright (c) 2012 Baidu.com, Inc. All Rights Reserved
// @author chaihua(chaihua@baidu.com)
// @brief

#ifndef  __DSTREAM_CC_PROCESSELEMENT_TEST_MOCKZKPROGRESS_H__
#define  __DSTREAM_CC_PROCESSELEMENT_TEST_MOCKZKPROGRESS_H__

#include <gmock/gmock.h>
#include "processelement/zk_client.h"

namespace dstream {
namespace processelement {

class MockZKProgress : public ZKProgress {
public:
    MOCK_METHOD2(Init, int32_t(uint64_t, Config*));
    MOCK_METHOD1(SetProgress, int32_t(const struct BPPoint&));
    MOCK_METHOD1(GetProgress, int32_t(struct BPPoint*));
};

class MockZKRegister : public ZKRegister {
public:
    MOCK_METHOD4(Init, int32_t(uint64_t, const std::string&,
                               const std::string&, int));
};

}  // namespace dstream
}  // namespace processelement

#endif  // __DSTREAM_CC_PROCESSELEMENT_TEST_MOCKZKPROGRESS_H__
