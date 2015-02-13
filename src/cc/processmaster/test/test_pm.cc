/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test pm
*********************************************************************/

#include "common/logger.h"
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_pm");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

