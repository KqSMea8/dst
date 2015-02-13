/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/11
* @filename:    scheduler.h
* @author:      zhanggengxin
* @brief:       schduler
*/

#ifndef __DSTREAM_CC_SCHEDULER_SCHEDULER_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_SCHEDULER_H__ // NOLINT

#include <list>
#include <set>
#include <string>
#include "common/error.h"
#include "common/dstream_type.h"
#include "common/proto/application.pb.h"
#include "common/proto/cluster_config.pb.h"
#include "common/proto/pn.pb.h"

#include "processmaster/zk_watcher.h"

#include "scheduler/black_list.h"
#include "scheduler/pn_group.h"
#include "scheduler/resource_compare.h"

namespace dstream {
namespace scheduler {

/************************************************************************/
/* resource request unit                                                */
/************************************************************************/
struct resource_request {
    resource_request(ProcessorID pid, const Resource res) {
        m_processor_id = pid;
        m_resource = res;
        m_num = 0;
        m_group_name = "";
    }
    bool operator < (const resource_request& res_req) const {
        return m_processor_id.id() < res_req.m_processor_id.id();
    }
    int m_num;
    ProcessorID m_processor_id;
    Resource m_resource;
    application_tools::PeidSet m_pe_set;
    std::string m_group_name;
};

// define resource request list
typedef std::set<resource_request> ResReqSet;
typedef ResReqSet::iterator ResReqSetIter;

typedef std::list<ProcessorID> PidList;
typedef PidList::iterator PidListIter;

/************************************************************************/
/* Scheduler Abstract class                                              */
/************************************************************************/
class Scheduler {
public:
    virtual ~Scheduler() {}
    // Init Scheduler : return {true} if success
    virtual bool Init(config::Config* config = NULL) {
        m_black_list.set_config(config);
        m_black_list.Start();
        return true;
    }
    // Update Scheduler
    virtual bool Update() = 0;

    // Add PN resource Resource
    virtual int32_t AddPNResource(const PNID& pn_id,
                                  const Resource& total_resource,
                                  const std::list<Resource>* used_resouce) = 0;
    // Remove pn from resource map : return {error::OK} if success
    virtual int32_t RemovePN(const PNID& pn_id) = 0;

    // Add Resource request
    // if the resource register can satify the request will return {error::OK}
    // else return {error::RESOURCE_NOT_SATISFY}
    // if the request add to resource map fail will return {error::ADD_REQ_FAIL}
    // if the request is already exist return {error:: RESOURCE_REQ_EXIST}
    virtual int32_t AddResRequest(const resource_request& res_req) = 0;
    virtual int32_t AddResRequest(const ResReqSet& res_req_list) = 0;
    // Remove Resource request : return {error::OK} if success
    // return {error::NO_REQUEST} if processor_id not exists
    virtual int32_t RemoveResRequest(const ProcessorID& processor_id) = 0;
    virtual int32_t RemoveResRequest(const PidList& pid_list) = 0;
    // Add processor id -> used pn
    virtual int32_t AddProcessorPN(const ProcessorID& pid, const PNID& pn_id) = 0;

    // Allocate PN for resource request
    // excludes and prefer_list are blacklist/white list, will be NULL if no black/white list
    // if return {error::OK} then pn_id is the allocate result
    // return {error::OK} if allocate an pn
    // return {error::ALLOCATE_PN_FAIL} if allocate fail
    virtual int32_t AllocatePN(const ProcessorID& pid,
                               /*const Resource& resource,*/
                               const Processor& processor,
                               const PEID& pe_id, PNID& pn_id) = 0;
    // add allocated processor manual
    virtual int32_t ManualAllocatedPN(const ProcessorID& pid,
                                      const Resource& resource,
                                      const PEID& pe_id,
                                      const PNID& pn_id) = 0;
    // Return Allocated PN for available : return {error::OK} if success
    virtual int32_t ReleaseResource(const ProcessorID& pid,
                                    const Resource& resource,
                                    const PNID& pn_id,
                                    bool add_reserved = true) = 0;
    // select primary pe from pn list
    virtual int32_t SelectPrimary(const PNIDSet& pn_set, PNID* pn_id) = 0;
    
    // dump relative struct
    virtual dstream::error::Code Dump(std::string *output) {
        return dstream::error::OK;
    }
    
    // Clear All Allocated information
    virtual void ClearAllProcessor() {}
    // Rebuild All Processors' allocated information
    virtual void RebuildAllProcessor() {}

    // get if scheduler can work
    virtual bool SchedulerCanWork() {
        return true;
    }

    // Add PEFail
    int32_t AddFailPE(const PEID& pe_id, const PNID& pn_id) {
        return m_black_list.AddFailPE(pe_id, pn_id);
    }
    
    // erase pe blacklist
    int32_t ErasePEBlackList(const PEID& pe_id) {
        return m_black_list.ErasePEBlackList(pe_id);
    }
    // clean blacklist
    int32_t ClearBlackList() {
        return m_black_list.ClearBlackList();
    }
    int32_t GetPEBlackList(const PEID& pe_id, std::set<PNID, PNIDCompare>* black_set) {
        return m_black_list.GetBlackList(pe_id, *black_set);
    }
    
    // used for unit test
    PNGroup* pn_group() {
        return m_pn_group;
    }

protected:
    static bool CanScheduler(int report_pn_num);
    int GetMaxPENumOnPN();
    ClusterConfig m_cluster_config;
    BlackList m_black_list;

    PNGroup* m_pn_group;
    auto_ptr::AutoPtr<zk_watcher::DStreamWatcher> m_pn_group_watcher;
};

// Get scheduler According to config
// the method will return no null value when create scheduler success
// the pointer returned should be managed by the method caller
scheduler::Scheduler* GetScheduler(config::Config* config);

} // namespace scheduler
} // namespace dstream

#endif // NOLINT
