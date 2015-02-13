/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:	    2012/4/12
* @filename:    bfd_scheduler_algorithm.cc
* @author:	    zhanggengxin@baidu.com
* @brief:       scheduler algorithm
*/

#include "scheduler/bfd_scheduler_algorithm.h"

namespace dstream {
namespace scheduler {
using namespace application_tools;

int32_t BFDSchduerAlgorithm::AllocateResource(const ResReqSet& res_req_set,
                                              const PnResourceMap& resource_map,
                                              PidPnListMap& result,
                                              ResourceCompare* res_cmp,
                                              const PePnSetMap* pe_black_list,
                                              const PNGroup* pn_group,
                                              int max_pe_on_pn) {
    assert(NULL != res_cmp);
    if (res_req_set.empty()) {
        DSTREAM_WARN("[%s] resource map is empty", __FUNCTION__);
        return error::ALLOC_RESOURCE_FAIL;
    }

    // copy resource_map
    PnResourceMap cur_resource_map(resource_map);

    PePnSetMap::const_iterator pe_fail_end;
    if (NULL != pe_black_list) {
        pe_fail_end = pe_black_list->end();
    }
    PNIDSet empty_set;
    ResReqSet::iterator req_it, req_end;
    PNResStatics resource_statics;
    resource_statics.m_pe_num = 1;

    for (req_it = res_req_set.begin(), req_end = res_req_set.end();
         req_it != req_end; ++req_it) {

        if (req_it->m_num <= 0 &&
            req_it->m_pe_set.empty() &&
            req_it->m_group_name.empty()) {
            continue;
        }
        PNIDSet prefer_pn_list;
        if (const_cast<PNGroup*>(pn_group)->GetPreferPNList(req_it->m_group_name,
                prefer_pn_list) < error::OK) {
            DSTREAM_WARN("[%s] request has not set group name", __FUNCTION__);
            continue;
        }

        // Construct priority queue
        PriorityCompare pc;
        pc.set_res_cmp(res_cmp);
        ResourceQueue rq(pc);
        ConstructResourceQueue(cur_resource_map, req_it->m_resource, max_pe_on_pn, res_cmp, &rq);

        int reqeust_num = req_it->m_num;
        if (reqeust_num < static_cast<int>(req_it->m_pe_set.size())) {
            reqeust_num = req_it->m_pe_set.size();
        }
        PeidSetIter pe_it, pe_end;
        pe_it = req_it->m_pe_set.begin();
        pe_end = req_it->m_pe_set.end();
        PEID pe_id;
        resource_statics.m_total = req_it->m_resource;
        PnidNumMap pnid_num_map;
        PnidNumMapIter pnid_num_find;
        PNIDSet& fail_pn_set = empty_set;
        for (int i = 0; i < reqeust_num ; ++i) {
            if (NULL != pe_black_list && pe_it != pe_end) {
                PePnSetMap::const_iterator fail_pn_find = pe_black_list->find(*pe_it);
                if (fail_pn_find != pe_fail_end) {
                    fail_pn_set = fail_pn_find->second;
                } else {
                    fail_pn_set = empty_set;
                }
                pe_id = *pe_it;
                ++pe_it;
            } else {
                pe_id.set_id(0);
            }
            // used to save pe's black list pn
            // because the pn may be not in other pe's black list
            std::list<priority_queue_node> pq_node_list;
            PNIDSetIter fail_pn_find, fail_pn_end, prefer_pn_end;
            fail_pn_end = fail_pn_set.end();
            prefer_pn_end = prefer_pn_list.end();
            while (true) {
                if (rq.empty()) {
                    DSTREAM_WARN("[%s] Resoure requeue is empty", __FUNCTION__);
                    return error::RESOURCE_NOT_ENOUGH;
                }
                priority_queue_node node = rq.top();
                PNResStatics& res_statics = node.m_resource_statics;
                if (res_statics.m_pe_num <= 0) {
                    DSTREAM_INFO("[%s] pn [%s] pe [%d] more than [%d]",
                                 __FUNCTION__,
                                 node.m_pn_id.id().c_str(),
                                 max_pe_on_pn - res_statics.m_pe_num,
                                 max_pe_on_pn);
                    rq.pop();
                    continue;
                }
                DSTREAM_DEBUG("[%s] pn [%s] pe number [%d:%d] "
                              "memory [%lu:%lu] cpu [%lf:%lf] ", __FUNCTION__,
                              node.m_pn_id.id().c_str(),
                              max_pe_on_pn - res_statics.m_pe_num,
                              max_pe_on_pn,
                              res_cmp->GetResStaticsMemory(res_statics),
                              res_cmp->GetResStaticsMemory(resource_statics),
                              res_cmp->GetResStaticsCPU(res_statics),
                              res_cmp->GetResStaticsCPU(resource_statics));

                if (fail_pn_set.find(node.m_pn_id) != fail_pn_end) {
                    DSTREAM_INFO("[%s] pn [%s] in pe [%lu] "
                                 " black list", __FUNCTION__,
                                 node.m_pn_id.id().c_str(), pe_id.id());
                    pq_node_list.push_back(node);
                    rq.pop();
                    continue;
                }
                if (prefer_pn_list.find(node.m_pn_id) == prefer_pn_end) {
                    DSTREAM_DEBUG("[%s] pn [%s] is not is the prefer pn list",
                                  __FUNCTION__, node.m_pn_id.id().c_str());
                    pq_node_list.push_back(node);
                    rq.pop();
                    continue;
                }
                if ((*res_cmp)(resource_statics, res_statics)) {
                    rq.pop();
                    res_cmp->SubtractResource(resource_statics, &node.m_resource_statics, false);

                    // find resource in cur_resource_map
                    // subtract
                    PnResourceMap::iterator res_it = cur_resource_map.find(node.m_pn_id);
                    res_cmp->SubtractResource(resource_statics, &res_it->second, false);

                    --node.m_resource_statics.m_pe_num;
                    rq.push(node);
                    pnid_num_find = pnid_num_map.find(node.m_pn_id);
                    if (pnid_num_find != pnid_num_map.end()) {
                        ++pnid_num_find->second;
                    } else {
                        pnid_num_map[node.m_pn_id] = 1;
                    }
                    break;
                } else {
                    pq_node_list.push_back(node);
                    rq.pop();
                    DSTREAM_WARN("[%s] pn [%s] resource is not enough [%lf:%lf] [%lu:%lu] "
                                 "[%d:%d]", __FUNCTION__,
                                 node.m_pn_id.id().c_str(),
                                 res_cmp->GetResStaticsCPU(resource_statics),
                                 res_cmp->GetResStaticsCPU(res_statics),
                                 res_cmp->GetResStaticsMemory(resource_statics),
                                 res_cmp->GetResStaticsMemory(res_statics),
                                 resource_statics.m_pe_num, res_statics.m_pe_num);
                    continue;
                }
            }
            if (!pq_node_list.empty()) {
                std::list<priority_queue_node>::iterator pq_node_it, pq_node_end;
                for (pq_node_it = pq_node_list.begin(), pq_node_end = pq_node_list.end();
                     pq_node_it != pq_node_end; ++pq_node_it) {
                    rq.push(*pq_node_it);
                }
            }
        }
        result[req_it->m_processor_id] = pnid_num_map;
    }
    return error::OK;
}

int32_t BFDSchduerAlgorithm::AllocateResource(const Resource& resource,
                                              const PnResourceMap& resource_map,
                                              PNID& allocated_pn,
                                              ResourceCompare* res_cmp,
                                              const PNIDSet* pn_black_list,
                                              const PNIDSet& prefer_pn_list,
                                              int max_pe_on_pn) {
    PnResourceMap::const_iterator res_it, res_end;
    bool find_pn = false;
    PNResStatics res_statics, max_resource;
    res_statics.m_total = resource;
    res_statics.m_pe_num = 1;
    PNIDSet::const_iterator pnid_find, pnid_end;
    if (NULL != pn_black_list) {
        pnid_end = pn_black_list->end();
    }
    for (res_it = resource_map.begin(), res_end = resource_map.end();
         res_it != res_end; ++res_it) {
        if (res_it->second.m_pe_num >= max_pe_on_pn) {
            DSTREAM_INFO("[%s] pn [%s] pe number [%d] "
                         "more than [%d]", __FUNCTION__, res_it->first.id().c_str(),
                         res_it->second.m_pe_num, max_pe_on_pn);
            continue;
        }
        if (NULL != pn_black_list && !pn_black_list->empty()) {
            if ((pnid_find = pn_black_list->find(res_it->first)) != pnid_end) {
                DSTREAM_WARN("[%s] pn [%s] in black list", __FUNCTION__,
                             res_it->first.id().c_str());
                continue;
            }
        }
        if (!prefer_pn_list.empty()) {
            if ((pnid_find = prefer_pn_list.find(res_it->first)) == prefer_pn_list.end()) {
                DSTREAM_DEBUG("[%s] pn [%s] is not in the prefer pn list", __FUNCTION__,
                             res_it->first.id().c_str());
                continue;
            }
        }
        PNResStatics tmp_resource = res_it->second;
        tmp_resource.m_pe_num = max_pe_on_pn - res_it->second.m_pe_num;
        if ((*res_cmp)(res_statics, tmp_resource)) {
            if (!find_pn) {
                find_pn = true;
                max_resource = res_it->second;
                allocated_pn = res_it->first;
            } else {
                if ((*res_cmp)(max_resource, tmp_resource)) {
                    max_resource = res_it->second;
                    allocated_pn = res_it->first;
                }
            }
        }
    }
    return find_pn ? error::OK : error::ALLOC_RESOURCE_FAIL;
}

void BFDSchduerAlgorithm::ConstructResourceQueue(const PnResourceMap& resource_map,
                                                 const Resource& resource,
                                                 int max_pe_on_pn,
                                                 ResourceCompare* res_cmp,
                                                 ResourceQueue* rq) {
    PnResourceMap::const_iterator pn_res_end, pn_res_it;
    for (pn_res_it = resource_map.begin(), pn_res_end = resource_map.end();
         pn_res_it != pn_res_end; ++pn_res_it) {
        if (!(*res_cmp)(resource, pn_res_it->second)) {
            DSTREAM_DEBUG("resource request memory [%lu] cpu [%lf] filter pn [%s] memory [%lu] cpu [%lf]",
                          resource.memory(),
                          resource.cpu(),
                          pn_res_it->first.id().c_str(),
                          res_cmp->GetResStaticsMemory(pn_res_it->second),
                          res_cmp->GetResStaticsCPU(pn_res_it->second));
            continue;
        }

        priority_queue_node node(pn_res_it->first, pn_res_it->second);
        int& pe_num = node.m_resource_statics.m_pe_num;
        pe_num = max_pe_on_pn - pe_num;
        DSTREAM_DEBUG("[%s] pn [%s] memory [%lu] cpu [%lf] pe number "
                      "[%d]", __FUNCTION__,
                      pn_res_it->first.id().c_str(),
                      res_cmp->GetResStaticsMemory(node.m_resource_statics),
                      res_cmp->GetResStaticsCPU(node.m_resource_statics),
                      max_pe_on_pn - pe_num);
        rq->push(node);
    }
    DSTREAM_DEBUG("[%s] resource queue size [%zd]", __FUNCTION__, rq->size());
}

} // namespace scheduler
} // namespace dstream
