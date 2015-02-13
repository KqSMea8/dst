/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/04/17
* @filename:    resource_manager.cc
* @author:      zhanggengxin@baidu.com
* @brief:       resource manager
*/

#include "scheduler/resource_manager.h"

namespace dstream {
namespace scheduler {

#define CHECK_PN()                                                        \
    MutexLockGuard lock_guard(m_lock);                                            \
    if (NULL == res_cmp) {                                                   \
        DSTREAM_WARN("resource compare is null");                              \
        return error::RESOURCE_FAIL;                                            \
    }                                                                         \
    PnidStaticsMapIter statics_find = m_pn_statics.find(pn_id);              \
    if (statics_find == m_pn_statics.end()) {                                   \
        DSTREAM_WARN("pn [%s] is not in resource manager", pn_id.id().c_str());  \
        return error::PN_STATICS_NOT_EXIST;                                      \
    }                                                                          \

// Add PN Resource
// return {error::OK} if add pn resource success
// return {error::PN_STATICS_EXIST} if pn is already
// return {error::ADD_PN_RESOURCE_FAIL} if add pn resource fail
int32_t ResourceManager::AddPNResource(const PNID& pn_id,
                                       const Resource& total_resource,
                                       const std::list<Resource>* used_resouce,
                                       const ResourceCompare* res_cmp) {
    MutexLockGuard lock_guard(m_lock);
    assert(NULL != res_cmp);
    PnidStaticsMapIter statics_find = m_pn_statics.find(pn_id);
    if (statics_find != m_pn_statics.end()) {
        DSTREAM_WARN("[%s] pn [%s] is already in resource manager", __FUNCTION__,
                     pn_id.id().c_str());
        return error::PN_STATICS_EXIST;
    }
    PnidStaticsInsertRes add_pn;
    add_pn = m_pn_statics.insert(PnidStaticsPair(pn_id, PNResStatics()));
    PNResStatics& res_statics = add_pn.first->second;
    res_statics.m_total = total_resource;
    if (NULL != used_resouce && !used_resouce->empty()) {
        res_statics.m_pe_num = used_resouce->size();
        PNResStatics used_statics;
        used_statics.m_pe_num = 1;
        std::list<Resource>::const_iterator used_end, used_it;
        for (used_it = used_resouce->begin(), used_end = used_resouce->end();
             used_it != used_end; ++used_it) {
            used_statics.m_used = *used_it;
            res_cmp->AddResource(used_statics, &res_statics);
        }
    }
    return error::OK;
}

// Add reserved pn resource of pn_id
int32_t ResourceManager::AddReservedResource(const PNID& pn_id,
                                             const Resource& resource,
                                             const ResourceCompare* res_cmp,
                                             int add_num/* = 1*/) {
    CHECK_PN();
    // Add resource in reserved resource
    for (int i = 0; i < add_num; ++i) {
        res_cmp->AddResource(resource, &(statics_find->second.m_reserved));
    }
    return error::OK;
}

// Get Resource from reserved resource for use
// return {error::OK} if reserved resource satisfy request
// if return error::OK the used resource will add
int32_t ResourceManager::GetReservedResource(const PNID& pn_id,
                                             const Resource& resource,
                                             const ResourceCompare* res_cmp,
                                             int num /* = 1 */) {
    CHECK_PN();
    Resource total_resource;
    for (int i = 0; i < num; ++i) {
        res_cmp->AddResource(resource, &total_resource);
    }
    PNResStatics reserved_statics, total_statics;
    reserved_statics.m_total = statics_find->second.m_reserved;
    total_statics.m_total = total_resource;
    // compare two resource : when two is equal must return ok
    if ((*res_cmp)(reserved_statics, total_statics) &&
        !(*res_cmp)(total_statics, reserved_statics)) {
        DSTREAM_WARN("[%s] reserved resource is not enough", __FUNCTION__);
        return error::RESOURCE_NOT_ENOUGH;
    }
    // update reserved/used information of pn
    res_cmp->SubtractResource(total_resource, &(statics_find->second.m_reserved));
    res_cmp->AddResource(total_resource, &(statics_find->second.m_used));
    statics_find->second.m_pe_num += num;
    return error::OK;
}

// Return allocated resource
int32_t ResourceManager::ReturnReservedResource(const PNID& pn_id,
                                                const Resource& resource,
                                                const ResourceCompare* res_cmp,
                                                int num) {
    CHECK_PN();
    for (int i = 0; i < num; ++i) {
        res_cmp->SubtractResource(resource, &(statics_find->second.m_reserved));
    }
    return error::OK;
}

// GetResource from remained resource
// if {iginore_allocated == true} the reserved resource will be used
// else the resereved resource cannot be used
int32_t ResourceManager::GetResource(const PNID& pn_id,
                                     const Resource& resource,
                                     const ResourceCompare* res_cmp,
                                     int add_num /* = 1 */,
                                     bool ignore_reserved /* = false */) {
    CHECK_PN();
    PNResStatics remained_statics = statics_find->second;
    if (ignore_reserved) {
        remained_statics.m_reserved.Clear();
    }
    PNResStatics request_statics;
    for (int i = 0; i < add_num; ++i) {
        res_cmp->AddResource(resource, &(request_statics.m_total));
    }
    if ((*res_cmp)(remained_statics, request_statics) &&
        !(*res_cmp)(request_statics, remained_statics)) {
        DSTREAM_WARN("[%s] pn [%s] resource is not enough", __FUNCTION__,
                     pn_id.id().c_str());
        return error::RESOURCE_NOT_ENOUGH;
    }
    res_cmp->AddResource(request_statics.m_total, &(statics_find->second.m_used));
    statics_find->second.m_pe_num += add_num;
    return error::OK;
}

// check is pn_id can satisfy resource require
bool ResourceManager::CheckResource(const PNID& pn_id,
                                    const Resource& resource,
                                    const ResourceCompare* res_cmp) {
    CHECK_PN();
    PNResStatics request_statics;
    request_statics.m_pe_num = 1;
    request_statics.m_total = resource;
    if ((*res_cmp)(statics_find->second, request_statics) &&
        !(*res_cmp)(request_statics, statics_find->second)) {
        return false;
    }
    return true;
}

int32_t ResourceManager::RemovePN(const PNID& pn_id) {
    MutexLockGuard lock_guard(m_lock);
    PnidStaticsMapIter statics_find = m_pn_statics.find(pn_id);
    if (statics_find == m_pn_statics.end()) {
        DSTREAM_WARN("[%s] cannot find pn [%s]", __FUNCTION__, pn_id.id().c_str());
        return error::PN_STATICS_NOT_EXIST;
    } else {
        DSTREAM_INFO("[%s] remove pn [%s]", __FUNCTION__, pn_id.id().c_str());
        m_pn_statics.erase(statics_find);
    }
    return error::OK;
}

int32_t ResourceManager::ReturnResource(const PNID& pn_id,
                                        const Resource& resource,
                                        const ResourceCompare* res_cmp,
                                        int rm_num) {
    CHECK_PN();
    if (resource.has_memory() && resource.memory() > 0) {
        DSTREAM_INFO("pn %s reserved pe %d", pn_id.id().c_str(), statics_find->second.m_pe_num);
        statics_find->second.m_pe_num -= rm_num;
        if (statics_find->second.m_pe_num < 0) {
            statics_find->second.m_pe_num = 0;
        }
    }
    Resource& used = statics_find->second.m_used;
    for (int i = 0; i < rm_num; ++i) {
        res_cmp->SubtractResource(resource, &used);
    }
    return error::OK;
}

PnidStaticsMap ResourceManager::PNResourceStatics() {
    MutexLockGuard lock_guard(m_lock);
    return m_pn_statics;
}

int ResourceManager::pn_number() {
    MutexLockGuard lock_guard(m_lock);
    return m_pn_statics.size();
}

} // namespace scheduler
} // namespace dstream
