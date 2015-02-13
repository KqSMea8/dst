/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/12
* @filename:    scheduler_algorithm.h
* @author:      zhanggengxin@baidu.com
* @brief:       scheduler algorithm
*/

#ifndef __DSTREAM_CC_SCHEDULER_SCHEDULER_ALGORITHM_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_SCHEDULER_ALGORITHM_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "common/dstream_type.h"
#include "scheduler/pn_group.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace scheduler {

// define pnid->numbers type
// used for maintain processor's assignment information
typedef std::map<PNID, int, PNIDCompare> PnidNumMap;
typedef PnidNumMap::iterator PnidNumMapIter;
typedef PnidNumMap::const_iterator ConstPnidNumMapIter;

// define processor id -> pn id list type
typedef std::map<ProcessorID, PnidNumMap, ProcessorIDComapre> PidPnListMap;
typedef std::pair<ProcessorID, PnidNumMap> PidPnListPair;
typedef PidPnListMap::iterator PidPnListMapIter;

typedef std::map<PNID, PNResStatics, PNIDCompare> PnResourceMap;
typedef PnResourceMap::iterator PnResourceMapIter;
typedef std::pair<PNID, PNResStatics> PnResourcePair;
typedef std::pair<PnResourceMapIter, bool> PnResourceInsertResult;

/************************************************************************/
/* SchedulerAlgorithm : allocate resource for request                   */
/************************************************************************/
class SchedulerAlgorithm {
public:
    virtual ~SchedulerAlgorithm() {}
    // AllocateResource for request
    // return {error::OK} if success
    // return {error::RESOURCE_NOT_ENOUGH} if resouce is not enough
    // return {error::ALLOC_RESOURCE_FAIL} if fail for other reason
    virtual int32_t AllocateResource(const ResReqSet& req_res_set,
                                     const PnResourceMap& resource_map,
                                     PidPnListMap& result,
                                     ResourceCompare* res_cmp,
                                     const PePnSetMap* pe_black_list,
                                     const PNGroup* pn_group,
                                     int max_pe_on_pn) = 0;

    virtual int32_t AllocateResource(const Resource& resource,
                                     const PnResourceMap& resource_map,
                                     PNID& allocated_pn,
                                     ResourceCompare* res_cmp,
                                     const PNIDSet* pn_black_list,
                                     const PNIDSet& prefer_pn_list,
                                     int max_pe_on_pn) = 0;
};

/******************************************************************************/
/* BFD Algorithm to slove allocate problem                                    */
/* The Algorithm is not the same as stand BFD Algorithm                       */
/* The difference lies in the algorithm allocate rosource average between pns */
/******************************************************************************/
// class BFDSchduerAlgorithm : public SchedulerAlgorithm {
// public:
//     int32_t AllocateResource(const scheduler::ResReqSet& req_res_set,
//                              const PnResourceMap& resource_map,
//                              PidPnListMap& result,
//                              resource_compare::ResourceCompare* res_cmp,
//                              const black_list::PePnSetMap* pe_black_list,
//                              const pn_group::PNGroup* pn_group,
//                              int max_pe_on_pn);
// 
//     int32_t AllocateResource(const Resource& resource,
//                              const PnResourceMap& resource_map,
//                              PNID& allocated_pn,
//                              resource_compare::ResourceCompare* res_cmp,
//                              const PNIDSet* pn_black_list,
//                              const PNIDSet& prefer_pn_list,
//                              int max_pe_on_pn);
// };

// Get SchedulerAlgorithm by name
SchedulerAlgorithm* GetAlgorithm(const char* algorithm_name = 0);

} // namespace scheduler
} // namespace dstream
#endif // NOLINT
