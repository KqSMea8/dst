/************************************************************************/
/* Test for resource manager                                            */
/************************************************************************/
#include "common/auto_ptr.h"
#include <gtest/gtest.h>
#include "scheduler/resource_manager.h"

namespace dstream {
namespace test_resource_manager {
using namespace auto_ptr;
using namespace resource_manager;

typedef AutoPtr<ResourceCompare> RES_CMP_PTR;

//Test AddPN Resource
void TestAddPNResouce() {
    PNID pn_id;
    pn_id.set_id("test_host_1");
    ResourceManager rm;
    Resource resource;
    resource.set_memory(1024);
    PNID_STATICS_MAP statics;

    RES_CMP_PTR   res_cmp = GetResourceCompareByName();
    //test for add pn resource
    ASSERT_EQ(error::OK, rm.AddPNResource(pn_id, resource,
                                          NULL, res_cmp));
    ASSERT_NE(error::OK, rm.AddPNResource(pn_id, resource,
                                          NULL, res_cmp));
    statics = rm.PNResourceStatics();
    ASSERT_EQ(1, statics.size());
    //test for allocate resource
    resource.set_memory(512);
    ASSERT_EQ(error::OK, rm.AddReservedResource(pn_id, resource, res_cmp));
    ASSERT_EQ(error::OK, rm.GetReservedResource(pn_id, resource, res_cmp, 1));
    ASSERT_EQ(error::RESOURCE_NOT_ENOUGH, rm.GetReservedResource(pn_id, resource,
              res_cmp));
    Resource      resource1;
    resource1.set_memory(768);
    ASSERT_EQ(error::RESOURCE_NOT_ENOUGH, rm.GetResource(pn_id,
              resource1, res_cmp));
    resource1.set_memory(512);
    ASSERT_EQ(error::OK, rm.AddReservedResource(pn_id, resource1, res_cmp));
    ASSERT_EQ(error::OK, rm.ReturnResource(pn_id, resource1, res_cmp));
    resource1.set_memory(768);
    ASSERT_EQ(error::RESOURCE_NOT_ENOUGH, rm.GetResource(pn_id,
              resource1, res_cmp));
    ASSERT_EQ(error::OK, rm.GetResource(pn_id, resource1, res_cmp, 1, true));
    resource1.set_memory(512);
    ASSERT_FALSE(rm.CheckResource(pn_id, resource1, res_cmp));
    resource1.set_memory(768);
    ASSERT_EQ(error::OK, rm.ReturnResource(pn_id, resource1, res_cmp));
    ASSERT_TRUE(rm.CheckResource(pn_id, resource, res_cmp));

    ASSERT_EQ(error::OK, rm.RemovePN(pn_id));
    statics = rm.PNResourceStatics();
    ASSERT_EQ(0, statics.size());
}

TEST(TestResourceManager, TestAddPNResouce) {
    TestAddPNResouce();
}

} //namespace test_resource_manager
} //namespace dstream
