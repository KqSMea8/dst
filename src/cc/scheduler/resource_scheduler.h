/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/12
* @filename:    resource_scheduler.h
* @author:      zhanggengxin@baidu.com
* @brief:       resouece scheduler
*/

#ifndef __DSTREAM_CC_SCHEDULER_RESOURCE_SCHEDULER_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_RESOURCE_SCHEDULER_H__ // NOLINT

#include <list>
#include <map>
#include <string>
#include <utility>
#include "common/error.h"
#include "processmaster/zk_watcher.h"
#include "scheduler/pn_group.h"
#include "scheduler/resource_manager.h"
#include "scheduler/scheduler.h"
#include "scheduler/scheduler_algorithm.h"

namespace dstream {
namespace scheduler {

// define ResourceCompare/SchedulerAlgorithm Auto Ptr type
typedef auto_ptr::AutoPtr<ResourceCompare> ResourceComparePtr;
typedef auto_ptr::AutoPtr<ResourceManager> ResourceManagerPtr;
typedef auto_ptr::AutoPtr<SchedulerAlgorithm> SchedulerAlgorithmPtr;

/************************************************************************/
/* processor allocated information                                      */
/************************************************************************/
struct ProcessorStatics {
    Resource m_resource;
    PnidNumMap m_pnid_num_map;
    PnidNumMap m_used_pn;
};

// defind processor_id->statics map
typedef std::map<ProcessorID, ProcessorStatics, ProcessorIDComapre> PidStaticsMap;
typedef PidStaticsMap::iterator PidStaticsMapIter;
typedef std::pair<ProcessorID, ProcessorStatics> PidStaticsPair;
typedef std::pair<PidStaticsMapIter, bool> PidStaInsertRes;

/************************************************************************/
/* resource scheduler                                                   */
/************************************************************************/
class ResourceScheduler : public scheduler::Scheduler {
public:
    ResourceScheduler() : m_init(false) {}
    virtual ~ResourceScheduler();
    // init ResourceScheduler
    bool Init(config::Config* config = NULL);
    bool Update();

    // Add PN resource Resource
    int32_t AddPNResource(const PNID& pn_id,
                          const Resource& total_resource,
                          const std::list<Resource>* used_resouce);
    // Remove pn from resource map : return {error::OK} if success
    int32_t RemovePN(const PNID& pn_id);


    // Add Resource request
    // if the resource register can satify the request will return {error::OK}
    // else return {error::RESOURCE_NOT_SATISFY}
    // if the request add to resource map fail will return {error::ADD_REQ_FAIL}
    // if the request is already exist return {error:: RESOURCE_REQ_EXIST}
    int32_t AddResRequest(const resource_request& res_req);
    int32_t AddResRequest(const ResReqSet& res_req_list);

    // Remove Resource request : return {error::OK} if success
    // return {error::NO_REQUEST} if processor_id not exists
    int32_t RemoveResRequest(const ProcessorID& processor_id);
    int32_t RemoveResRequest(const PidList& pid_list);
    int32_t AddProcessorPN(const ProcessorID& pid, const PNID& pn_id);

    // Allocate PN for resource request
    // excludes and prefer_list are blacklist/white list, will be NULL if no black/white list
    // if return {error::OK} then pn_id is the allocate result
    // return {error::OK} if allocate an pn
    // return {error::ALLOCATE_PN_FAIL} if allocate fail
    int32_t AllocatePN(const ProcessorID& pid,
                       /*const Resource& resource,*/
                       const Processor& processor,
                       const PEID& pe_id, PNID& pn_id);
    // add allocated processor manual
    int32_t ManualAllocatedPN(const ProcessorID& pid, const Resource& resource,
                              const PEID& pe_id, const PNID& pn_id);
    // Return Allocated PN for available : return {error::OK} if success
    int32_t ReleaseResource(const ProcessorID& pid, const Resource& resource,
                            const PNID& pn_id, bool add_reserved = true);

    // select primary pe from pn list
    int32_t SelectPrimary(const PNIDSet& pn_set, PNID* pn_id);

    // Clear All Allocated information
    void ClearAllProcessor();
    // Rebuild All Processors' allocated information
    void RebuildAllProcessor();

    // get if scheduler can work
    virtual bool SchedulerCanWork();

    // return pn_statics for test
    PidStaticsMap pid_statics();
    ResourceManager* resource_manager() {
        return m_resource_manager;
    }
    virtual dstream::error::Code Dump(std::string* output);


private:
    // Update Processors' allocated information
    int32_t UpdateResource(PidPnListMapIter pid_begin,
                           PidPnListMapIter pid_end,
                           const ResReqSet& res_req_list);
    // Rollback Updated Processors' information if update fail
    void Rollback(PidPnListMapIter pid_begin,
                  PidPnListMapIter pid_end);
    // calculate pn's available resource
    void GetPNResourceMap(PnResourceMap* pn_resource);
    // find pn for running processor
    int32_t FindPN(const PnidNumMap& total,
                   // const PNIDSet& prefer_pn_list,
                   const PNIDSet& pe_black_list,
                   const Resource& resource,
                   const PnResourceMap& pn_res_map,
                   PnidNumMap* used,
                   PNID* pn_id);

private:
    bool m_init;
    MutexLock m_lock;

    // processor assignments
    PidStaticsMap m_pid_statics;

    ResourceComparePtr m_res_cmp;
    ResourceManagerPtr m_resource_manager;
    SchedulerAlgorithmPtr m_algorithm;
};

} // namespace scheduler
} // namespace dstream
#endif // NOLINT
