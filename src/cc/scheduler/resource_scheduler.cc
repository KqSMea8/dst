/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/12
* @filename:    resource_scheduler.cc
* @author:      zhanggengxin@baidu.com
* @brief:       resouece scheduler
*/
#include "boost/algorithm/string/trim.hpp"
#include "common/proto/pm.pb.h"
#include "scheduler/resource_scheduler.h"
#include "metamanager/meta_manager.h"
#include "pb_to_json.h"
namespace dstream {
namespace scheduler {
using namespace auto_ptr;
using namespace meta_manager;
using namespace config_const;

#define CHECK_INIT(init, line)                                     \
    if (!init) {                                                     \
        DSTREAM_WARN("Resource Scheduler is not init at [%d]", line);  \
        return error::RESOURCE_SCHEDULE_NOT_INIT;                      \
    }                                                                \

bool ResourceScheduler::Init(Config* config /* = NULL */) {
    MutexLockGuard lock_guard(m_lock);
    if (m_init) {
        DSTREAM_INFO("ResourceScheduler has already inited");
        return false;
    }
    // init pn group data structure
    m_pn_group = new PNGroup();
    if (m_pn_group == NULL) {
        DSTREAM_WARN("start pn group fail");
        return false;
    }
    m_pn_group_watcher = AutoPtr<zk_watcher::DStreamWatcher>(
                         new PNGroupWatcher(config, m_pn_group));
    if (!m_pn_group_watcher.PtrValid()) {
        DSTREAM_WARN("start pn group watcher fail");
        return false;
    }
    if (m_pn_group_watcher->StartWatchPNGroup() < error::OK) {
        DSTREAM_WARN("start watch pn group fail");
        return false;
    }
    if (m_pn_group->UpdatePNGroup() < error::OK) {
        DSTREAM_WARN("update pn group fail");
        return false;
    }

    std::string cmp_name = kPENumResourceCmp;
    std::string algorithm_name = kDefaultResourceAlgorithm;
    if (NULL != config) {
        std::string config_val;
        if (config->GetValue(kResourceCmpName, &config_val) == error::OK) {
            cmp_name = config_val;
        }
        if (config->GetValue(kResourceAlgorithmName, &config_val) == error::OK) {
            algorithm_name = config_val;
        }
    }
    m_res_cmp = ResourceComparePtr(GetResourceCompareByName(cmp_name.c_str()));
    if (!m_res_cmp.PtrValid()) {
        DSTREAM_WARN("Get ResourceCompare by name [%s] fail", cmp_name.c_str());
        return false;
    }
    m_algorithm = SchedulerAlgorithmPtr(GetAlgorithm(algorithm_name.c_str()));
    if (!m_algorithm.PtrValid()) {
        DSTREAM_WARN("Get SchedulerAlgorithm by name fail");
        return false;
    }
    m_resource_manager = ResourceManagerPtr(new (std::nothrow)ResourceManager);
    if (!m_resource_manager.PtrValid()) {
        DSTREAM_WARN("Init Resource Manager fail");
        return false;
    }
    DSTREAM_INFO("Init ResourceSchduer ok, compare is [%s] "
                 "algorithm is [%s]", cmp_name.c_str(), algorithm_name.c_str());
    return (m_init = Scheduler::Init(config));
}

ResourceScheduler::~ResourceScheduler() {
    if (m_pn_group_watcher.PtrValid()) {
        m_pn_group_watcher->UnWatchAll();
    }
    delete m_pn_group;
    m_pn_group = NULL;
}

bool ResourceScheduler::Update() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        DSTREAM_WARN("[%s] Meta is not init", __FUNCTION__);
        return false;
    }
    if (meta->GetClusterConfig(&m_cluster_config) != error::OK) {
        DSTREAM_WARN("[%s] GetClusterConfig fail, ResourceScheduler "
                     "udpate fail", __FUNCTION__);
        return false;
    }
    return true;
}

int32_t ResourceScheduler::AddPNResource(const PNID& pn_id,
                                         const Resource& total_resource,
                                         const list<Resource>* used_resouce) {
    CHECK_INIT(m_init, __LINE__);
    return m_resource_manager->AddPNResource(pn_id, total_resource,
                                            used_resouce, m_res_cmp);
}

int32_t ResourceScheduler::RemovePN(const PNID& pn_id) {
    MutexLockGuard lock_gurad(m_lock);
    CHECK_INIT(m_init, __LINE__);
    // remove processor's used and allocated pn
    PidStaticsMapIter m_pid_staticsend, m_pid_staticsit;
    for (m_pid_staticsit = m_pid_statics.begin(),
         m_pid_staticsend = m_pid_statics.end();
         m_pid_staticsit != m_pid_staticsend; ++m_pid_staticsit) {
        m_pid_staticsit->second.m_pnid_num_map.erase(pn_id);
        m_pid_staticsit->second.m_used_pn.erase(pn_id);
    }
    return m_resource_manager->RemovePN(pn_id);
}

int32_t ResourceScheduler::AddResRequest(const resource_request& res_req) {
    ResReqSet res_req_set;
    res_req_set.insert(res_req);
    return AddResRequest(res_req_set);
}

int32_t ResourceScheduler::AddResRequest(const ResReqSet& res_req_set) {
    MutexLockGuard lock_gurad(m_lock);
    CHECK_INIT(m_init, __LINE__);
    DSTREAM_DEBUG("[%s] AddRequest size is [%zd]", __FUNCTION__,
                  res_req_set.size());
    if (res_req_set.empty()) {
        return error::OK;
    }

    if (!SchedulerCanWork()) {
        DSTREAM_WARN("[%s] AddRequest failed because Scheduler can not work", __FUNCTION__);
        return error::OK;
    }

    // calculate available resource first
    PnResourceMap current_resource;
    GetPNResourceMap(&current_resource);

    // Allocate Resource for request
    PidPnListMap allocate_result;
    int32_t res = error::OK;
    PePnSetMap pe_black_list;
    m_black_list.GetPEFailMap(&pe_black_list);
    if ((res = m_algorithm->AllocateResource(res_req_set, current_resource,
                                            allocate_result, m_res_cmp,
                                            &pe_black_list,
                                            m_pn_group,
                                            GetMaxPENumOnPN())) != error::OK) {
        DSTREAM_WARN("allocate resource fail");
        return res;
    }

    DSTREAM_DEBUG("[%s] allocate resource size [%zd]", __FUNCTION__,
                  allocate_result.size());
    // Update Processors' allocate information
    if ((res = UpdateResource(allocate_result.begin(),
                              allocate_result.end(),
                              res_req_set)) != error::OK) {
        DSTREAM_WARN("[%s] Update Allocated Resource fail", __FUNCTION__);
    }
    return res;
}

int32_t ResourceScheduler::RemoveResRequest(const ProcessorID& processor_id) {
    PidList pid_list;
    pid_list.push_back(processor_id);
    return RemoveResRequest(pid_list);
}

int32_t ResourceScheduler::RemoveResRequest(const PidList& pid_list) {
    DSTREAM_INFO("[%s] Remove Resource Request size [%zd]", __FUNCTION__,
                 pid_list.size());
    MutexLockGuard lock_guard(m_lock);
    CHECK_INIT(m_init, __LINE__);
    PidList::const_iterator pid_end, pid_it;
    PidStaticsMapIter m_pid_staticsfind;
    for (pid_it = pid_list.begin(), pid_end = pid_list.end();
         pid_it != pid_end; ++pid_it) {
        if ((m_pid_staticsfind = m_pid_statics.find(*pid_it)) != m_pid_statics.end()) {
            // remove allocated resource but not used resource
            PnidNumMapIter pnid_num_end, pnid_num_it;
            ProcessorStatics& ps = m_pid_staticsfind->second;
            PnidNumMap& m_used_pnnum = ps.m_used_pn;
            PnidNumMapIter m_used_pnend = m_used_pnnum.end();
            for (pnid_num_it = ps.m_pnid_num_map.begin(),
                 pnid_num_end = ps.m_pnid_num_map.end();
                 pnid_num_it != pnid_num_end; ++pnid_num_it) {
                int num = pnid_num_it->second;
                PnidNumMapIter m_used_pnfind = m_used_pnnum.find(pnid_num_it->first);
                if (m_used_pnfind != m_used_pnend) {
                    num -= m_used_pnfind->second;
                }
                m_resource_manager->ReturnReservedResource(pnid_num_it->first,
                                                           ps.m_resource,
                                                           m_res_cmp, num);
            }
            m_pid_statics.erase(m_pid_staticsfind);
        }
    }
    return error::OK;
}

// Insert pnid into map
static void InsertPNIDNum(PnidNumMap& pnid_num_map, const PNID& pn_id) {
    PnidNumMapIter pnid_find = pnid_num_map.find(pn_id);
    if (pnid_find == pnid_num_map.end()) {
        pnid_num_map[pn_id] = 1;
    } else {
        ++pnid_find->second;
    }
}

int32_t ResourceScheduler::AddProcessorPN(const ProcessorID& pid,
                                          const PNID& pn_id) {
    MutexLockGuard lock_guard(m_lock);
    CHECK_INIT(m_init, __LINE__);
    PidStaticsMapIter m_pid_staticsfind = m_pid_statics.find(pid);
    if (m_pid_staticsfind == m_pid_statics.end()) {
        // Add ProcessorID Statics first
        PidStaInsertRes insert_res;
        insert_res = m_pid_statics.insert(PidStaticsPair(pid, ProcessorStatics()));
        if (!insert_res.second) {
            DSTREAM_WARN("insert processor [%lu] pn [%s] fail", pid.id(),
                         pn_id.id().c_str());
            return error::ADD_RESOURCE_REQUEST_FAIL;
        }
    } else {
        InsertPNIDNum(m_pid_staticsfind->second.m_pnid_num_map, pn_id);
        InsertPNIDNum(m_pid_staticsfind->second.m_used_pn, pn_id);
    }
    return error::OK;
}

int32_t ResourceScheduler::AllocatePN(const ProcessorID& pid,
                                      /*const Resource& resource,*/
                                      const Processor& processor,
                                      const PEID& pe_id, PNID& pn_id) {
    MutexLockGuard lock_guard(m_lock);
    CHECK_INIT(m_init, __LINE__);

    // find allocated pn first
    Resource resource = processor.resource_require();
    PnResourceMap pn_resource;
    GetPNResourceMap(&pn_resource);
    PidStaticsMapIter m_pid_staticsfind;
    m_pid_staticsfind = m_pid_statics.find(pid);
    PNIDSet pe_black_list;
    m_black_list.GetBlackList(pe_id, pe_black_list);
    int32_t ret = error::OK;
    if (m_pid_staticsfind != m_pid_statics.end()) {
        ProcessorStatics& pro_statics = m_pid_staticsfind->second;
        if (FindPN(pro_statics.m_pnid_num_map, pe_black_list, resource, pn_resource,
                   &(pro_statics.m_used_pn), &pn_id) == error::OK) {
            // Add Allocated to resource
            if (m_resource_manager->GetReservedResource(pn_id, resource,
                                                        m_res_cmp) == error::OK) {
                DSTREAM_INFO("[%s] Allocated reserved pn [%s] for processor [%lu] "
                             "pe [%lu]", __FUNCTION__, pn_id.id().c_str(),
                             pid.id(), pe_id.id());
                return error::OK;
            }
            // subtract used map : because find pn has changed used pn number
            PnidNumMap& pnid_map = m_pid_staticsfind->second.m_used_pn;
            PnidNumMapIter find_pn = pnid_map.find(pn_id);
            if (--find_pn->second <= 0) {
                pnid_map.erase(find_pn);
            }
        }
    }

    // find a pn that is not reserved to run the processor
    PNIDSet prefer_pn_list;
    if ((ret = m_pn_group->GetPreferPNList(processor.pn_group(), prefer_pn_list)) < error::OK) {
        return ret;
    }
    if (m_algorithm->AllocateResource(resource, pn_resource,
                                      pn_id, m_res_cmp, &pe_black_list,
                                      prefer_pn_list,
                                      GetMaxPENumOnPN()) == error::OK) {
        if (m_resource_manager->GetResource(pn_id, resource, m_res_cmp) == error::OK) {
            // update used pn information
            if (m_pid_staticsfind != m_pid_statics.end()) {
                PnidNumMap& used_map = m_pid_staticsfind->second.m_used_pn;
                PnidNumMapIter find_pn_num = used_map.find(pn_id);
                if (find_pn_num != used_map.end()) {
                    ++find_pn_num->second;
                }
            }
            DSTREAM_INFO("[%s] Allocated not reserved pn [%s] for processor [%lu] "
                         "pe [%lu]", __FUNCTION__, pn_id.id().c_str(),
                         pid.id(), pe_id.id());
            return error::OK;
        }
    }
    DSTREAM_WARN("[%s] allocated resource fail for processor [%lu] "
                 "pe [%lu]", __FUNCTION__, pid.id(), pe_id.id());
    return error::ALLOC_RESOURCE_FAIL;
}

// add allocated processor manual
// this method will tell pn not calculate all available resource
// so this will ignore allocated resource
int32_t ResourceScheduler::ManualAllocatedPN(const ProcessorID& pid,
                                             const Resource& resource,
                                             const PEID& pe_id,
                                             const PNID& pn_id) {
    MutexLockGuard lock_guard(m_lock);
    CHECK_INIT(m_init, __LINE__);

    // check whether the pn is in allocated
    PidStaticsMapIter statics = m_pid_statics.find(pid);
    if (statics != m_pid_statics.end()) {
        PnidNumMap& allocated_map = statics->second.m_pnid_num_map;
        PnidNumMapIter allocated_find = allocated_map.find(pn_id);
        if (allocated_find != allocated_map.end()) {
            PnidNumMap& used_map = statics->second.m_used_pn;
            PnidNumMapIter used_find = used_map.find(pn_id);
            if (used_find != used_map.end()) {
                // there is allocated resource
                if (allocated_find->second > used_find->second) {
                    if (m_resource_manager->GetReservedResource(pn_id, resource,
                                                                m_res_cmp) == error::OK) {
                        ++used_find->second;
                        DSTREAM_INFO("[%s] allocated pn [%s] for processor [%lu] "
                                     "pe [%lu]", __FUNCTION__, pn_id.id().c_str(),
                                     pid.id(), pe_id.id());
                        return error::OK;
                    }
                }
            } else {
                if (m_resource_manager->GetReservedResource(pn_id, resource,
                                                            m_res_cmp) == error::OK) {
                    used_map[pn_id] = 1;
                    DSTREAM_INFO("[%s] allocated pn [%s] for processor [%lu] "
                                 "pe [%lu]", __FUNCTION__, pn_id.id().c_str(),
                                 pid.id(), pe_id.id());
                    return error::OK;
                }
            }
        }
    }

    // add resource ignore allocated resource
    if (m_resource_manager->GetResource(pn_id, resource,
                                        m_res_cmp, 1, true) == error::OK) {
        DSTREAM_INFO("[%s] allocated pn [%s] for processor [%lu] "
                     "pe [%lu]", __FUNCTION__, pn_id.id().c_str(),
                     pid.id(), pe_id.id());
        return error::OK;
    }
    DSTREAM_WARN("[%s] allocated resource processor [%lu] of "
                 " pe [%lu] fail", __FUNCTION__, pid.id(), pe_id.id());
    return error::ALLOC_RESOURCE_FAIL;
}

int32_t ResourceScheduler::ReleaseResource(const ProcessorID& pid,
                                           const Resource& resource,
                                           const PNID& pn_id,
                                           bool add_reserved /*= true*/) {
    MutexLockGuard lock_guard(m_lock);
    CHECK_INIT(m_init, __LINE__);
    // update resource manager
    DSTREAM_DEBUG("[%s] pn [%s] return resource memory [%lu] cpu [%lf]", __FUNCTION__,
                  pn_id.id().c_str(),
                  resource.memory(),
                  resource.cpu());
    m_resource_manager->ReturnResource(pn_id, resource, m_res_cmp);

    if (!add_reserved) {
        return error::OK;
    }
    // update processor allocated information
    PidStaticsMapIter ps_find = m_pid_statics.find(pid);
    if (ps_find != m_pid_statics.end()) {
        PnidNumMap& used = ps_find->second.m_used_pn;
        PnidNumMapIter pnid_num_find = used.find(pn_id);
        if (pnid_num_find != used.end()) {
            // int used_num = pnid_num_find->second;
            if (--(pnid_num_find->second) == 0) {
                used.erase(pnid_num_find);
            }
            // PnidNumMapIter pnid_total_find;
            // pnid_total_find = ps_find->second.m_pnid_num_map.find(pn_id);
            // if (pnid_total_find->second >= used_num) {
            //   m_resource_manager->AddReservedResource(pn_id, resource, m_res_cmp);
            // }
        }
    }
    return error::OK;
}

int32_t ResourceScheduler::SelectPrimary(const PNIDSet& pn_set, PNID* pn_id) {
    if (pn_set.size() <= 0) {
        DSTREAM_WARN("select primary pe fail");
        return error::SELECT_PRIMARY_FAIL;
    }
    *pn_id = *(pn_set.begin());
    return error::OK;
}

// Add add map to first total_map
static void MergeMap(PnidNumMap& total_map, const PnidNumMap& add_map) {
    PnidNumMap::const_iterator add_it, add_end;
    PnidNumMapIter total_find;
    for (add_it = add_map.begin(), add_end = add_map.end();
         add_it != add_end; ++add_it) {
        if ((total_find = total_map.find(add_it->first)) == total_map.end()) {
            total_map[add_it->first] = add_it->second;
        } else {
            total_find->second += add_it->second;
        }
    }
}

// Update Processors' allocated information
int32_t ResourceScheduler::UpdateResource(PidPnListMapIter pid_begin,
                                          PidPnListMapIter pid_end,
                                          const ResReqSet& res_req_set) {
    PidPnListMapIter pid_it = pid_begin;
    PnidNumMapIter pnid_num_end, pnid_num_it;
    ResReqSet::const_iterator pid_res_find;
    while (pid_it != pid_end) {
        // Add Processor Assignment first
        // this function is used internally so do not check pid's exist
        resource_request res_request(pid_it->first, Resource());
        const Resource& resource = res_req_set.find(res_request)->m_resource;
        PidStaticsMapIter insert_res;
        PidStaInsertRes pid_insert_res;
        if ((insert_res = m_pid_statics.find(pid_it->first)) == m_pid_statics.end()) {
            DSTREAM_DEBUG("[%s] insert process [%lu]", __FUNCTION__,
                          pid_it->first.id());
            pid_insert_res = m_pid_statics.insert(PidStaticsPair(pid_it->first,
                                                  ProcessorStatics()));
            if (!pid_insert_res.second) {
                DSTREAM_WARN("insert processor id [%lu] fail", pid_it->first.id());
                Rollback(pid_begin, pid_it);
                return error::ADD_RESOURCE_REQUEST_FAIL;
            }
            insert_res = pid_insert_res.first;
        }
        insert_res->second.m_resource = resource;
        MergeMap(insert_res->second.m_pnid_num_map, pid_it->second);

        DSTREAM_DEBUG("[%s] processor [%lu] allocate [%zd] pn", __FUNCTION__,
                      pid_it->first.id(),
                      pid_it->second.size());
        // Update pn
        for (pnid_num_it = pid_it->second.begin(),
             pnid_num_end = pid_it->second.end();
             pnid_num_it != pnid_num_end; ++pnid_num_it) {
            if (m_resource_manager->AddReservedResource(pnid_num_it->first,
                                                        resource, m_res_cmp,
                                                        pnid_num_it->second) != error::OK) {
                DSTREAM_WARN("[%s] add allocated resource to pn [%s] "
                             " fail", __FUNCTION__, pnid_num_it->first.id().c_str());
                ++pid_it;
                Rollback(pid_begin, pid_it);
                return error::ADD_RESOURCE_REQUEST_FAIL;
            }
            DSTREAM_DEBUG("[%s] pn [%s] add reserved resource [%lu] "
                          "num [%d]", __FUNCTION__, pnid_num_it->first.id().c_str(),
                          resource.memory(), pnid_num_it->second);
        }
        ++pid_it;
    }
    return error::OK;
}

// Rollback
// this method and rollback method is complex so do not modify if not necessery
void ResourceScheduler::Rollback(PidPnListMapIter pid_begin,
                                 PidPnListMapIter pid_end) {
    PnidNumMapIter pnid_num_end, pnid_num_it;
    PidStaticsMapIter pid_find;
    while (pid_begin != pid_end) {
        // find processor request first
        pid_find = m_pid_statics.find(pid_begin->first);
        if (pid_find == m_pid_statics.end()) {
            ++pid_begin;
            continue;
        }

        // update pn resource statics
        PnidNumMap& pnid_num_map = pid_begin->second;
        for (pnid_num_it = pnid_num_map.begin(), pnid_num_end = pnid_num_map.end();
             pnid_num_it != pnid_num_end; ++pnid_num_it) {
            m_resource_manager->GetReservedResource(pnid_num_it->first,
                                                    pid_find->second.m_resource,
                                                    m_res_cmp,
                                                    pnid_num_it->second);
        }

        // remove processor request
        m_pid_statics.erase(pid_find);
    }
}

void ResourceScheduler::GetPNResourceMap(PnResourceMap* pn_resource) {
    *pn_resource = m_resource_manager->PNResourceStatics();
    PnidStaticsMapIter pn_statics_it, pn_statics_end;
    PNIDSet pn_black_list;
    m_black_list.GetPNBlackList(&pn_black_list);
    PNIDSetIter pn_black_end = pn_black_list.end();
    int max_pe_num_on_pn = GetMaxPENumOnPN();
    for (pn_statics_it = pn_resource->begin(), pn_statics_end = pn_resource->end();
         pn_statics_it != pn_statics_end;) {
        if (pn_black_list.find(pn_statics_it->first) != pn_black_end) {
            DSTREAM_INFO("[%s] pn [%s] in black list", __FUNCTION__,
                         pn_statics_it->first.id().c_str());
            pn_resource->erase(pn_statics_it++);
            continue;
        }
        if (pn_statics_it->second.m_pe_num >= max_pe_num_on_pn) {
            DSTREAM_INFO("[%s] pn [%s] pe number [%d] "
                         "more than [%d]", __FUNCTION__,
                         pn_statics_it->first.id().c_str(),
                         pn_statics_it->second.m_pe_num,
                         max_pe_num_on_pn);
            pn_resource->erase(pn_statics_it++);
            continue;
        }
        ++pn_statics_it;
    }
}

int32_t ResourceScheduler::FindPN(const PnidNumMap& total,
                                  const PNIDSet& pe_black_list,
                                  const Resource& /*resource*/,
                                  const PnResourceMap& pn_res_map,
                                  PnidNumMap* used,
                                  PNID* pn_id) {
    int used_num = -1;
    DSTREAM_DEBUG("[%s] total map size [%zd] used map "
                  "[%zd]", __FUNCTION__, total.size(), used->size());
    ConstPnidNumMapIter pnid_it, pnid_end;
    PnidNumMapIter pnid_find, pnid_select;
    bool find = false;
    PNIDSet::const_iterator pe_black_find, pe_black_end/*, prefer_pn_end*/;
    PnResourceMap::const_iterator pn_res_end = pn_res_map.end();
    pe_black_end = pe_black_list.end();
    // prefer_pn_end = prefer_pn_list.end();
    for (pnid_it = total.begin(), pnid_end = total.end();
         pnid_it != pnid_end; ++pnid_it) {
        if (pn_res_map.find(pnid_it->first) == pn_res_end) {
            DSTREAM_INFO("[%s] pn [%s] not in resource map", __FUNCTION__,
                         pnid_it->first.id().c_str());
            continue;
        }
        if (pe_black_list.find(pnid_it->first) != pe_black_end) {
            DSTREAM_WARN("[%s] pn [%s] in black list", __FUNCTION__,
                         pnid_it->first.id().c_str());
            continue;
        }
        // find if the pnid_it->first is in the prefer_pn_list
        // for(PNIDSetIter it = prefer_pn_list.begin(); it != prefer_pn_list.end(); it++) {
        //   DSTREAM_DEBUG("[%s] prefer pn list has %s", __FUNCTION__,
        //                                               it->id().c_str());
        // }
        // if (prefer_pn_list.find(pnid_it->first) == prefer_pn_end) {
        //   DSTREAM_INFO("[%s] pn [%s] not in prefer pn list", __FUNCTION__,
        //                                                      pnid_it->first.id().c_str());
        //   continue;
        // }

        // find a pn which run process's number is minimal
        pnid_find = used->find(pnid_it->first);
        if (pnid_find == used->end()) {
            (*used)[pnid_it->first] = 1;
            *pn_id = pnid_it->first;
            return error::OK;
        }
        if (pnid_find->second > pnid_it->second) {
            continue;
        }
        if (used_num == -1) {
            find = true;
            used_num = pnid_it->second - pnid_find->second;
            *pn_id = pnid_it->first;
            pnid_select = pnid_find;
        } else {
            if (used_num < pnid_it->second - pnid_find->second) {
                find = true;
                used_num = pnid_it->second - pnid_find->second;
                *pn_id = pnid_it->first;
                pnid_select = pnid_find;
            }
        }
    }
    if (find) {
        ++(pnid_select->second);
    }
    return find ? (error::OK) : (error::ALLOC_BUFFER_FAIL);
}

// Clear All Processors' allocated processor
void ResourceScheduler::ClearAllProcessor() {
    MutexLockGuard lock_guard(m_lock);
    if (m_init) {
        PidStaticsMapIter m_pid_staticsend, m_pid_staticsit;
        for (m_pid_staticsit = m_pid_statics.begin(),
             m_pid_staticsend = m_pid_statics.end();
             m_pid_staticsit != m_pid_staticsend; ++m_pid_staticsit) {
            PnidNumMap& statics = m_pid_staticsit->second.m_pnid_num_map;
            PnidNumMap& used = m_pid_staticsit->second.m_used_pn;
            PnidNumMapIter allocated_it, allocated_end, used_find, used_end;
            used_end = used.end();
            for (allocated_it = statics.begin(), allocated_end = statics.end();
                 allocated_it != allocated_end; ++allocated_it) {
                used_find = used.find(allocated_it->first);
                if (used_find != used_end) {
                    allocated_it->second -= used_find->second;
                }
                if (allocated_it->second > 0) {
                    m_resource_manager->ReturnResource(allocated_it->first,
                                                       m_pid_staticsit->second.m_resource,
                                                       m_res_cmp, allocated_it->second);
                }
            }
        }
    }
}

void ResourceScheduler::RebuildAllProcessor() {
}

PidStaticsMap ResourceScheduler::pid_statics() {
    MutexLockGuard lock_guard(m_lock);
    return m_pid_statics;
}

bool ResourceScheduler::SchedulerCanWork() {
    return CanScheduler(m_resource_manager->pn_number());
}

dstream::error::Code ResourceScheduler::Dump(std::string* output) {
    PnidStaticsMap pn_statics_map = m_resource_manager->PNResourceStatics();
    // dump PNResourceStatics
    PnidStaticsPB pn_statics_pb;
    PnidStaticsMapIter ps_iter = pn_statics_map.begin();
    for (; ps_iter != pn_statics_map.end(); ++ps_iter) {
        PnidStaticsKV* pn_statics_kv = pn_statics_pb.add_pn_statics_kv();
        pn_statics_kv->mutable_pn_id()->CopyFrom(ps_iter->first);
        PNResStaticsPB* pn_res_statics = pn_statics_kv->mutable_pn_res_statics();
        pn_res_statics->mutable_total()->CopyFrom(ps_iter->second.m_total);
        pn_res_statics->mutable_used()->CopyFrom(ps_iter->second.m_used);
        pn_res_statics->mutable_reserved()->CopyFrom(ps_iter->second.m_reserved);
        pn_res_statics->set_pe_num(ps_iter->second.m_pe_num);
    }
    // dump to json
    std::string json_str("");
    std::string tmp;
    json_str += "{\"pn_resource_statics\":";
    ProtoMessageToJson(pn_statics_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += "}";
    output->assign(json_str);
    return dstream::error::OK;
}

} // namespace scheduler
} // namespace dstream
