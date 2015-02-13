#include <gtest/gtest.h>
#include "scheduler/resource_scheduler.h"

namespace dstream {
namespace test_resource_scheduler {
using namespace resource_scheduler;

/************************************************************************/
/* class for test resource scheduler                                    */
/* The test case will not test resource manager and scheduler algorithm */
/* because these case are in test_resource_manager/test_algorithm       */
/************************************************************************/
class TestResourceScheduler : public ::testing::Test
{
protected:
    void SetUp();

    void TestInit() ;

    //Test Remove PN
    void TestRemovePN();

    //Test Resource Request operation
    void TestAddResRequestList();
    void TestRemoveResRequestList();

    //Test Resource allocate/release operation
    void TestAllocatePN();
    void TestManualAllocatePN();
    void TestReleaseResource();

    //Test Clear All Processor
    void TestClearAllProcessor();
private:
    //Add PN
    void AddPN(int num, int total_memory, int used_memory);
    void AddResRequest(RES_REQ_LIST& req_list, int num, int memory);
    void AllocateResource(RES_REQ_LIST& req_list, int num = 1);
    void ReleaseResource(RES_REQ_LIST& res_req_list, int num = 1);

    ResourceScheduler scheduler_;
    int pn_serial_;
    int pid_serial_;

    //pn_id list
    vector<PNID> pnid_list_;
};

void TestResourceScheduler::SetUp() {
    pn_serial_ = 0;
    pid_serial_ = 0;
    ASSERT_TRUE(scheduler_.Init(NULL));
}

void TestResourceScheduler::AddPN(int num, int total_memory, int used_memory) {
    char pn_id_string_[1024];
    PNID pn_id;
    Resource total, used;
    total.set_memory(total_memory);
    used.set_memory(used_memory);
    list<Resource> res_list;
    res_list.push_back(used);
    for (int i = 0; i < num ; ++i) {
        sprintf(pn_id_string_, "test_resource_%d", ++pn_serial_);
        pn_id.set_id(pn_id_string_);
        ASSERT_EQ(error::OK, scheduler_.AddPNResource(pn_id, total, &res_list));
        pnid_list_.push_back(pn_id);
    }
}

void TestResourceScheduler::AddResRequest(RES_REQ_LIST& req_list,
                                          int num, int memory) {
    ProcessorID pid;
    Resource resource;
    pid.set_id(pid_serial_++);
    resource.set_memory(memory);
    req_list.push_back(resource_request(pid, resource, num));
}

void TestResourceScheduler::TestRemovePN() {

}

void TestResourceScheduler::AllocateResource(RES_REQ_LIST& res_req_list,
                                             int num) {
    //Allocate resource
    for (RES_REQ_LIST_ITER res_req_it = res_req_list.begin(),
         res_req_end = res_req_list.end();
         res_req_it != res_req_end; ++res_req_it) {
        int add_num = num;
        if (add_num == -1) {
            add_num = res_req_it->pe_set_.size();
        }
        for (int i = 0; i < add_num; ++i) {
            PNID pn_id;
            ASSERT_EQ(error::OK, scheduler_.AllocatePN(res_req_it->processor_id_,
                                                       res_req_it->resource_,
                                                       NULL, NULL, pn_id));
        }
    }
}

//check processor id statics used for average assignment
static void CheckPIDStatics(PID_STATICS_MAP& pid_statics, int diff) {
    for (PID_STATICS_MAP_ITER pid_it = pid_statics.begin(),
         pid_end = pid_statics.end(); pid_it != pid_end; ++pid_it) {
        PNID_NUM_MAP& pnid_statics_map = pid_it->second.pnid_num_map_;
        PNID_NUM_MAP& pnid_used = pid_it->second.used_pn_;
        ASSERT_EQ(pnid_statics_map.size(), pnid_used.size());
        for (PNID_NUM_MAP_ITER pnid_num_it = pnid_statics_map.begin();
             pnid_num_it != pnid_statics_map.end(); ++pnid_num_it) {
            PNID_NUM_MAP_ITER pnid_num_find = pnid_used.find(pnid_num_it->first);
            ASSERT_EQ(pnid_num_it->second + diff, pnid_num_find->second);
        }
    }
}

//release resource used for average assignment
void TestResourceScheduler::ReleaseResource(RES_REQ_LIST& res_req_list,
                                            int num /*= 1*/) {
    for (RES_REQ_LIST_ITER res_req_it = res_req_list.begin();
         res_req_it != res_req_list.end(); ++res_req_it) {
        for (int i = 0; i < pnid_list_.size(); ++i) {
            for (int k = 0; k < num; ++k) {
                ASSERT_EQ(error::OK, scheduler_.ReleaseResource(res_req_it->processor_id_,
                                                                res_req_it->resource_,
                                                                pnid_list_[i]));
            }
        }
    }
}

//check pn memory
static void CheckPNMemory(PNID_STATICS_MAP& pn_statics,
                          int reserved, int used) {
    DSTREAM_DEBUG("expect reserved [%d] used [%d]", reserved, used);
    for (PNID_STATICS_MAP_ITER pn_it = pn_statics.begin(),
         pn_end = pn_statics.end(); pn_it != pn_end; ++pn_it) {
        ASSERT_EQ(pn_it->second.reserved_.memory(), reserved);
        ASSERT_EQ(pn_it->second.used_.memory(), used);
    }
}

void TestResourceScheduler::TestAddResRequestList() {
    //Test Add Request
    AddPN(10, 8192, 0);
    RES_REQ_LIST res_req_list;
    AddResRequest(res_req_list, 20, 768);
    AddResRequest(res_req_list, 20, 768);
    ASSERT_EQ(scheduler_.AddResRequestList(res_req_list), error::OK);
    ResourceManager* resource_manager = scheduler_.resource_manager();
    PNID_STATICS_MAP pn_statics = resource_manager->PNResourceStatics();
    DSTREAM_DEBUG("check pn memory");
    CheckPNMemory(pn_statics, 4 * 768, 0);

    //Allocate resource
    AllocateResource(res_req_list, -1);
    //check processor's allocated information
    PID_STATICS_MAP pid_statics = scheduler_.pid_statics();
    CheckPIDStatics(pid_statics, 0);
    pn_statics = resource_manager->PNResourceStatics();
    DSTREAM_DEBUG("check pn memory");
    CheckPNMemory(pn_statics, 0, 4 * 768);

    //Allocate more resouce
    AllocateResource(res_req_list, 10);
    pn_statics = resource_manager->PNResourceStatics();
    //check pn's resource
    DSTREAM_DEBUG("check pn memory");
    CheckPNMemory(pn_statics, 0, 6 * 768);
    pid_statics = scheduler_.pid_statics();
    CheckPIDStatics(pid_statics, 1);

    //Release resource
    ReleaseResource(res_req_list);
    pid_statics = scheduler_.pid_statics();
    CheckPIDStatics(pid_statics, 0);
    DSTREAM_DEBUG("check pn memory");
    pn_statics = resource_manager->PNResourceStatics();
    CheckPNMemory(pn_statics, 0, 4 * 768);

    //Release resource
    ReleaseResource(res_req_list);
    pid_statics = scheduler_.pid_statics();
    CheckPIDStatics(pid_statics, -1);
    pn_statics = resource_manager->PNResourceStatics();
    DSTREAM_DEBUG("check pn memory");
    CheckPNMemory(pn_statics, 2 * 768, 2 * 768);

    //clear all resource
    ReleaseResource(res_req_list);
    PID_LIST pid_list;
    for (RES_REQ_LIST_ITER it = res_req_list.begin();
         it != res_req_list.end(); ++it) {
        pid_list.push_back(it->processor_id_);
    }
    ASSERT_EQ(scheduler_.RemoveResRequestList(pid_list), error::OK);
    pn_statics = resource_manager->PNResourceStatics();
    DSTREAM_DEBUG("check pn memory");
    CheckPNMemory(pn_statics, 0, 0);
}

void TestResourceScheduler::TestRemoveResRequestList() {

}

void TestResourceScheduler::TestAllocatePN() {

}

void TestResourceScheduler::TestManualAllocatePN() {

}

void TestResourceScheduler::TestReleaseResource() {

}

//Test ResourceScheduler init
void TestResourceScheduler::TestInit() {
    ResourceScheduler resource_scheduler;
    //Test ReInit
    ASSERT_TRUE(resource_scheduler.Init(NULL));
    ASSERT_FALSE(resource_scheduler.Init(NULL));

    //Test error config
    ResourceScheduler resource_scheduler1;
    Config config;
    config.ReadConfig(kTestPMConfigFile);
    config.SetValue(kResourceCmpName, "TestFakeResourceCompare");
    ASSERT_FALSE(resource_scheduler1.Init(&config));
}


void TestResourceScheduler::TestClearAllProcessor() {
    AddPN(10, 4096, 0);
    RES_REQ_LIST res_req_list;
    AddResRequest(res_req_list, 13, 512);
    AddResRequest(res_req_list, 14, 1024);
    ASSERT_EQ(scheduler_.AddResRequestList(res_req_list), error::OK);
    PID_STATICS_MAP statics = scheduler_.pid_statics();
    ASSERT_EQ(2, statics.size());

    PID_LIST pid_list;
    for (RES_REQ_LIST::iterator it = res_req_list.begin();
         it != res_req_list.end(); ++it) {
        pid_list.push_back(it->processor_id_);
    }
    ASSERT_EQ(scheduler_.RemoveResRequestList(pid_list), error::OK);
    res_req_list.clear();
    statics.clear();
    statics = scheduler_.pid_statics();
    ASSERT_EQ(0, statics.size());
    ResourceManager* resource_manager = scheduler_.resource_manager();
    PNID_STATICS_MAP pn_statics_map = resource_manager->PNResourceStatics();
    PNID_STATICS_MAP_ITER pn_statics_it, pn_statics_end;
    for (pn_statics_it = pn_statics_map.begin(),
         pn_statics_end = pn_statics_map.end();
         pn_statics_it != pn_statics_end; ++pn_statics_it) {
        ASSERT_EQ(pn_statics_it->second.reserved_.memory(), 0);
    }
}

//Test Case
TEST_F(TestResourceScheduler, TestRemovePN) {
    TestRemovePN();
}

TEST_F(TestResourceScheduler, TestInit) {
    TestInit();
}

TEST_F(TestResourceScheduler, TestAddResRequestList) {
    TestAddResRequestList();
}

TEST_F(TestResourceScheduler, TestRemoveResRequestList) {
    TestRemoveResRequestList();
}

TEST_F(TestResourceScheduler, TestAllocatePN) {
    TestAllocatePN();
}

TEST_F(TestResourceScheduler, TestManualAllocatePN) {
    TestManualAllocatePN();
}

TEST_F(TestResourceScheduler, TestReleaseResource) {
    TestReleaseResource();
}

TEST_F(TestResourceScheduler, TestClearAllProcessor) {
    TestClearAllProcessor();
}

} //namespace test_resource_scheduler
} //namespace dstream
