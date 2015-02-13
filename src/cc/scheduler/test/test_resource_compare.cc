#include <string>
#include "common/auto_ptr.h"
#include "common/config_const.h"
#include <gtest/gtest.h>
#include "scheduler/resource_compare.h"

namespace dstream {
namespace test_resource_compare {
using namespace config_const;
using namespace resource_compare;

//define auto type for resource release
typedef auto_ptr::AutoPtr<resource_compare::ResourceCompare> RESCMP_PTR;

//Test GetResourceCompareByName method
void TestGetResourceCompare() {
    //get default resource compare if cmp_name == 0
    RESCMP_PTR res_cmp_ptr = RESCMP_PTR(GetResourceCompareByName(NULL));
    ASSERT_TRUE(res_cmp_ptr.PtrValid());

    //get default resource compare using name {config_const::kDefaultResourceCmp}
    res_cmp_ptr = RESCMP_PTR(GetResourceCompareByName(kDefaultResourceCmp));
    ASSERT_TRUE(res_cmp_ptr.PtrValid());

    //get other resource compare by name
    std::string other_cmp = std::string(kDefaultResourceCmp) + "other";
    res_cmp_ptr = RESCMP_PTR(GetResourceCompareByName(other_cmp.c_str()));
    ASSERT_FALSE(res_cmp_ptr.PtrValid());
}

TEST(TestResourceCompare, TestGetResourceCompare) {
    TestGetResourceCompare();
}

//Test Case for compare resource : compare resource resource by memory
void TestCompareResource() {
    Resource resource1, resource2;
    RESCMP_PTR res_cmp_ptr = GetResourceCompareByName(NULL);
    resource1.set_memory(100);
    resource2.set_memory(1000);
    ASSERT_TRUE((*res_cmp_ptr)(resource1, resource2));
    ASSERT_FALSE((*res_cmp_ptr)(resource2, resource1));

    resource1.set_memory(1000);
    resource1.set_disk(1000);
    ASSERT_TRUE((*res_cmp_ptr)(resource1, resource2));
    ASSERT_TRUE((*res_cmp_ptr)(resource2, resource1));
}

TEST(TestResourceCompare, TestCompareResource) {
    TestCompareResource();
}

//test case for add resource
void TestAddResource() {
    RESCMP_PTR res_cmp_ptr = GetResourceCompareByName(NULL);
    Resource resource1, resource2;
    resource1.set_memory(100);
    resource2.set_memory(199);
    res_cmp_ptr->AddResource(resource1, resource2);
    ASSERT_EQ(299, resource1.memory());
}

TEST(TestResourceCompare, TestAddResource) {
    TestAddResource();
}

//test case for subtrackresource
void TestSubtrackresource() {
    RESCMP_PTR res_cmp_ptr = GetResourceCompareByName(NULL);
    Resource resource1, resource2;
    ASSERT_FALSE(res_cmp_ptr->SubtractResource(resource1, resource2));
    resource1.set_memory(100);
    ASSERT_TRUE(res_cmp_ptr->SubtractResource(resource1, resource2));
    resource2.set_memory(90);
    ASSERT_TRUE(res_cmp_ptr->SubtractResource(resource1, resource2));
    ASSERT_EQ(10, resource1.memory());
    ASSERT_TRUE(res_cmp_ptr->SubtractResource(resource2, resource1));
    ASSERT_EQ(80, resource2.memory());
}

TEST(TestResourceCompare, TestSubtrackresource) {
    TestSubtrackresource();
}

} //namespace test_resource_compare
} //namespace dstream
