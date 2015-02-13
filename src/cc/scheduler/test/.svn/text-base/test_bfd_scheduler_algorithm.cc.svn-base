/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/04/24
* @filename:    test_bfd_scheduler_algorithm.h
* @author:      zhanggengxin@baidu.com
* @brief:       test bfd scheduler algorithm
*/

#include "common/dstream_type.h"
#include <gtest/gtest.h>
#include "scheduler/resource_compare.h"
#include "scheduler/scheduler_algorithm.h"

namespace dstream {
namespace test_bfd_scheduler_algorithm {
using namespace resource_compare;
using namespace scheduler_algorithm;

typedef AutoPtr<SchedulerAlgorithm> ALGORITHM_PTR;
typedef AutoPtr<ResourceCompare> RES_CMP_PTR;

long pn_start_id = 0;

//Add PN Resouce into map
void AddPNResource(PN_RESOURCE_MAP& resource_map, int memory, int num = 1) {
    char buffer[1024];
    PNID pn_id;
    Resource resource;

    for (int i = 0; i < num; ++i) {
        sprintf(buffer, "test_host_%d", ++pn_start_id);
        resource.set_memory(memory);
        pn_id.set_id(buffer, strlen(buffer));
        pn_res_statics pn_res_statics;
        pn_res_statics.resource_ = resource;
        resource_map[pn_id] = pn_res_statics;
    }
}

//Construct Fake Resource Map
void ConstructPNResouceMap(PN_RESOURCE_MAP& resouce_map) {
    //Add resoruce of 4 * 1024/3 * 2048/4096
    AddPNResource(resouce_map, 1024, 4);
    AddPNResource(resouce_map, 2048, 3);
    AddPNResource(resouce_map, 4096);
}

void AddResourceRequest(RES_REQ_LIST& req_list, int id, int memory, int num) {
    ProcessorID pid;
    pid.set_id(id);
    Resource res;
    res.set_memory(memory);
    req_list.push_back(resource_request(pid, res, num));
}

//Test Allocate Resource list
void TestAllocateResourceList() {
    ALGORITHM_PTR algorithm = ALGORITHM_PTR(GetAlgorithm(NULL));
    RES_CMP_PTR res_cmp = RES_CMP_PTR(GetResourceCompareByName(NULL));
    PN_RESOURCE_MAP resouce_map;
    ConstructPNResouceMap(resouce_map);

    //construct resource request list
    long id = 1;
    RES_REQ_LIST res_req_list;
    AddResourceRequest(res_req_list, id++, 512, 5);
    AddResourceRequest(res_req_list, id++, 3152, 1);
    AddResourceRequest(res_req_list, id++, 1835, 3);

    PID_PNLIST_MAP pid_pnlist_map;
    ASSERT_EQ(error::OK, algorithm->AllocateResource(res_req_list,
                                                     resouce_map,
                                                     pid_pnlist_map, res_cmp, 1000));
    ASSERT_EQ(3, pid_pnlist_map.size());
    //Test for resource not enough
    res_req_list.begin()->num_ = 10;
    ASSERT_NE(error::OK, algorithm->AllocateResource(res_req_list,
                                                     resouce_map,
                                                     pid_pnlist_map, res_cmp, 1000));

    //test for equal assignment
    res_req_list.clear();
    resouce_map.clear();
    pid_pnlist_map.clear();
    AddPNResource(resouce_map, 4096, 6);
    AddResourceRequest(res_req_list, id++, 1024, 8);
    AddResourceRequest(res_req_list, id++, 678, 10);
    AddResourceRequest(res_req_list, id++, 768, 9);
    ASSERT_EQ(error::OK, algorithm->AllocateResource(res_req_list,
                                                     resouce_map,
                                                     pid_pnlist_map, res_cmp, 1000));
    ASSERT_EQ(pid_pnlist_map.size(), 3);
    PID_PNLIST_MAP_ITER pid_it, pid_end;
    for (pid_it = pid_pnlist_map.begin(), pid_end = pid_pnlist_map.end();
         pid_it != pid_end; ++pid_it) {
        ASSERT_TRUE((pid_it->second.size() == 6 || pid_it->second.size() == 5 ));
    }
}

TEST(TestBFDSchedulerAlgorithm, TestAllocateResourceList) {
    TestAllocateResourceList();
}

//Test Allocate Resource for on request
void TestAllocateResource() {
    Resource resource;
    resource.set_memory(100);
    PN_RESOURCE_MAP resouce_map;
    ConstructPNResouceMap(resouce_map);
    ALGORITHM_PTR algorithm = ALGORITHM_PTR(GetAlgorithm(NULL));
    RES_CMP_PTR res_cmp = RES_CMP_PTR(GetResourceCompareByName(NULL));
    PNID pn_id;
    ASSERT_EQ(error::OK, algorithm->AllocateResource(resource,
                                                     resouce_map,
                                                     pn_id, res_cmp, 1000));
    ASSERT_TRUE(pn_id.has_id());
    PN_RESOURCE_MAP_ITER find_res = resouce_map.find(pn_id);
    ASSERT_EQ(find_res->second.memory(), 4096);
    resource.set_memory(4096);
    ASSERT_EQ(error::OK, algorithm->AllocateResource(resource,
                                                     resouce_map,
                                                     pn_id, res_cmp, 1000));
    resource.set_memory(4097);
    ASSERT_NE(error::OK, algorithm->AllocateResource(resource,
                                                     resouce_map,
                                                     pn_id, res_cmp, 1000));
    AddPNResource(resouce_map, 5000);
    ASSERT_EQ(error::OK, algorithm->AllocateResource(resource,
                                                     resouce_map,
                                                     pn_id, res_cmp, 1000));
}

TEST(TestBFDSchedulerAlgorithm, TestAllocateResource) {
    TestAllocateResource();
}

} //namespace test_scheduler_algorithm
} //namespace dstream
