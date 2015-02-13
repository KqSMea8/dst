/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test for dynamic type
*********************************************************************/

#include <gtest/gtest.h>

#include "common/dynamic_type.h"
#include "common/logger.h"
#include "common/test/test_dynamic.pb.h"
#include <iostream>

namespace dstream {
namespace test_dynamic_type {

using namespace dstream::dynamic_type;

TEST(TestDynamicType, TestGetType) {
    TestDynamicTest test1;
    test1.set_name("test1");
    dstream::dynamic_type::DynamicProtoReflect reflect;
    const std::string path = ".";
    reflect.SetProtoPath(path);
    const ProtoMessage* msg_type = reflect.GetProtoType("test_dynamic", "TestDynamicTest");
    ASSERT_FALSE((msg_type == 0));
    ProtoMessage* new_msg = msg_type->New();
    ASSERT_TRUE((new_msg != 0));
    std::string conv;
    test1.SerializeToString(&conv);
    new_msg->ParseFromString(conv);
    conv = "";
    new_msg->SerializeToString(&conv);
    TestDynamicTest test2;
    test2.ParseFromString(conv);
    ASSERT_STREQ("test1", test2.name().c_str());
}

TEST(TestDynamicType, TestGetTypeInfo)
{
    std::string tag = "test_string";
    std::string file, type;
    ASSERT_FALSE(GetDynamicTypeInfo(tag, &file, &type));
    tag = "test_file.test_type";
    ASSERT_TRUE(GetDynamicTypeInfo(tag, &file, &type));
    ASSERT_STREQ(file.c_str(), "test_file");
    ASSERT_STREQ(type.c_str(), "test_type");
    tag = "test_file.test_type.test";
    ASSERT_TRUE(GetDynamicTypeInfo(tag, &file, &type));
    ASSERT_STREQ(file.c_str(), "test_file");
    ASSERT_STREQ(type.c_str(), "test_type.test");
}

} // namespace test_dynamic_type
} // namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_dynamic_type");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

