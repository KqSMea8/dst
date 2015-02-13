/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:	    2012/4/12
* @filename:    bfd_scheduler_algorithm.h
* @author:	    zhanggengxin@baidu.com
* @brief:       scheduler algorithm
*/

#ifndef __DSTREAM_CC_SCHEDULER_BFD_SCHEDULER_ALGORITHM_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_BFD_SCHEDULER_ALGORITHM_H__ // NOLINT

#include <deque>
#include <queue>
#include "scheduler/scheduler_algorithm.h"

namespace dstream {
namespace scheduler {

// priority queue node used for allocate resource

struct priority_queue_node {
    priority_queue_node() {}
    priority_queue_node(const PNID& pn_id,
                        const PNResStatics& resource_statics) {
        m_pn_id = pn_id;
        m_resource_statics = resource_statics;
    }
    PNID m_pn_id;
    PNResStatics m_resource_statics;
};

// compare resource priority

class PriorityCompare {
public:
    bool operator() (const priority_queue_node& p1,
                     const priority_queue_node& p2) {
        if ((*m_res_cmp)(p1.m_resource_statics, p2.m_resource_statics) &&
            (*m_res_cmp)(p2.m_resource_statics, p1.m_resource_statics)) {
            return p1.m_pn_id.id() < p2.m_pn_id.id();
        }
        return (*m_res_cmp)(p1.m_resource_statics, p2.m_resource_statics);
    }
    void set_res_cmp(ResourceCompare* res_cmp) { m_res_cmp = res_cmp; }
private:
    ResourceCompare* m_res_cmp;
};

/************************************************************************/
/* compare two resource request                                         */
/************************************************************************/
class ResReqCompare {
public:
    bool operator() (const resource_request& req1, const resource_request& req2) {
        PNResStatics statics1, statics2;
        statics1.m_total = req1.m_resource;
        statics2.m_total = req2.m_resource;
        if ((*m_res_cmp)(statics1, statics2) &&
            (*m_res_cmp)(statics2, statics1)) {
            return req1.m_processor_id.id() < req2.m_processor_id.id();
        }
        return (*m_res_cmp)(statics1, statics2);
    }
    void set_res_cmp(ResourceCompare* res_cmp) {m_res_cmp = res_cmp;}
private:
    ResourceCompare* m_res_cmp;
};

typedef std::set<resource_request, ResReqCompare> ResReqCmpSet;
typedef ResReqCmpSet::iterator ResReqSetIter;
typedef std::vector<priority_queue_node> PqNodeVec;
typedef std::priority_queue<priority_queue_node, PqNodeVec, PriorityCompare> ResourceQueue;

// BFD Algorithm to slove allocate problem
// The Algorithm is not the same as stand BFD Algorithm
// The difference lies in the algorithm allocate rosource average between pns
class BFDSchduerAlgorithm : public SchedulerAlgorithm {
public:
    int32_t AllocateResource(const ResReqSet& req_res_set,
                             const PnResourceMap& resource_map,
                             PidPnListMap& result,
                             ResourceCompare* res_cmp,
                             const PePnSetMap* pe_black_list,
                             const PNGroup* pn_group,
                             int max_pe_on_pn);

    int32_t AllocateResource(const Resource& resource,
                             const PnResourceMap& resource_map,
                             PNID& allocated_pn,
                             ResourceCompare* res_cmp,
                             const PNIDSet* pn_black_list,
                             const PNIDSet& prefer_pn_list,
                             int max_pe_on_pn);

private:
    void ConstructResourceQueue(const PnResourceMap& resource_map,
                                const Resource& resource,
                                int max_pe_on_pn,
                                ResourceCompare* res_cmp,
                                ResourceQueue* rq);
};


} // namespace scheduler
} // namespace dstream

#endif // NOLINT
