/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test scheduler
*********************************************************************/

#include "common/dstream_type.h"
#include <gtest/gtest.h>

int main(int argc, char* argv[])
{
    ::dstream::logger::initialize("test_scheduler");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
