/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/12
* @filename:    resource_compare.cc
* @author:      zhanggengxin@baidu.com
* @brief:       resource compare
*/

#include "scheduler/resource_compare.h"

#include "common/dstream_type.h"

namespace dstream {
namespace scheduler {
using namespace config_const;

// compare two pn statics resources
// if resource1 less than resource2 return true
bool ResourceCompare::operator() (const PNResStatics& res_statics1,
                                  const PNResStatics& res_statics2) const {
//    if (GetResStaticsCPU(res_statics1) < GetResStaticsCPU(res_statics2)) {
//        return true;
//    } else if (GetResStaticsCPU(res_statics1) > GetResStaticsCPU(res_statics2)) {
//        return false;
//    } else {
//        return (GetResStaticsMemory(res_statics1) <=
//                GetResStaticsMemory(res_statics2));
//    }
    return (GetResStaticsMemory(res_statics1) <=
            GetResStaticsMemory(res_statics2)) &&
           (GetResStaticsCPU(res_statics1) <=
            GetResStaticsCPU(res_statics2));
}

// compare resource and pn resource statics
// if pn resource statics can contain resource return true
bool ResourceCompare::operator()(const Resource& resource,
                                 const PNResStatics& pn_res_statics) const {
    uint64_t res_memory = 0;
    float res_cpu = 0;
    if (resource.has_memory()) {
        res_memory = resource.memory();
    }
    if (resource.has_cpu()) {
        res_cpu = resource.cpu();
    }
    return res_cpu <= GetResStaticsCPU(pn_res_statics) &&
           res_memory <= GetResStaticsMemory(pn_res_statics);
}

// Add add_resource to resource by memoryh
void ResourceCompare::AddResource(const PNResStatics& add_resource,
                                  PNResStatics* resource) const {
    AddResource(add_resource.m_total, &(resource->m_total));
    AddResource(add_resource.m_used, &(resource->m_used));
    AddResource(add_resource.m_reserved, &(resource->m_reserved));
}

// Subtract sub_resource from resource by memory
bool ResourceCompare::SubtractResource(const PNResStatics& sub_resource,
                                       PNResStatics* resource,
                                       bool) const {
    SubtractResource(sub_resource.m_total, &(resource->m_total));
    SubtractResource(sub_resource.m_used, &(resource->m_used));
    SubtractResource(sub_resource.m_reserved, &(resource->m_reserved));
    return true;
}

// GetResource Statics of pn by remained memory
uint64_t ResourceCompare::GetResStaticsMemory(const PNResStatics& pn_statics) const {
    uint64_t res_memory = 0;
    if (pn_statics.m_total.has_memory()) {
        res_memory = pn_statics.m_total.memory();
    }
    if (pn_statics.m_reserved.has_memory()) {
        if (res_memory >= pn_statics.m_reserved.memory()) {
            res_memory -= pn_statics.m_reserved.memory();
        } else {
            res_memory = 0;
        }
    }
    if (pn_statics.m_used.has_memory()) {
        if (res_memory >= pn_statics.m_used.memory()) {
            res_memory -= pn_statics.m_used.memory();
        } else {
            res_memory = 0;
        }
    }
    return res_memory;
}

// GetResource Statics of pn by remained cpu
float ResourceCompare::GetResStaticsCPU(const PNResStatics& pn_statics) const {
    float res_cpu = 0;
    if (pn_statics.m_total.has_cpu()) {
        res_cpu = pn_statics.m_total.cpu();
    }
    if (pn_statics.m_reserved.has_cpu()) {
        if (res_cpu >= pn_statics.m_reserved.cpu()) {
            res_cpu -= pn_statics.m_reserved.cpu();
        } else {
            res_cpu = 0;
        }
    }
    if (pn_statics.m_used.has_cpu()) {
        if (res_cpu >= pn_statics.m_used.cpu()) {
            res_cpu -= pn_statics.m_used.cpu();
        } else {
            res_cpu = 0;
        }
    }
    return res_cpu;
}

// Add add_res to res by memory and cpu
void ResourceCompare::AddResource(const Resource& add_res,
                                  Resource* res)const {
    Resource add_resource = add_res;
    if (!add_resource.has_memory()) {
        add_resource.set_memory(0);
    }
    if (res->has_memory()) {
        res->set_memory(res->memory() + add_resource.memory());
    } else {
        res->set_memory(add_resource.memory());
    }
    if (!add_resource.has_cpu()) {
        add_resource.set_cpu(0);
    }
    if (res->has_cpu()) {
        res->set_cpu(res->cpu() + add_resource.cpu());
    } else {
        res->set_cpu(add_resource.cpu());
    }
}

// Sub sub_res from resource by memory and cpu
void ResourceCompare::SubtractResource(const Resource& sub_res,
                                       Resource* resource) const {
    if (!resource->has_memory()) {
        resource->set_memory(0);
    } else {
        if (sub_res.has_memory()) {
            if (resource->memory() >= sub_res.memory()) {
                resource->set_memory(resource->memory() - sub_res.memory());
            } else {
                resource->set_memory(0);
            }
        }
    }
    if (!resource->has_cpu()) {
        resource->set_cpu(0);
    } else {
        if (sub_res.has_cpu()) {
            if (resource->cpu() >= sub_res.cpu()) {
                resource->set_cpu(resource->cpu() - sub_res.cpu());
            } else {
                resource->set_cpu(0);
            }
        }
    }
}

ResourceCompare* GetResourceCompareByName(const char* cmp_name /* = 0 */) {
    DSTREAM_INFO("[%s] resource compare is [%s]", __FUNCTION__,
                  kMemoryCompare);
    return new (std::nothrow)ResourceCompare;
}

} // namespace scheduler
} // namespace dstream
