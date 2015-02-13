/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/12
* @filename:    resource_compare.h
* @author:      zhanggengxin@baidu.com
* @brief:       resource compare
*/

#ifndef __DSTREAM_CC_SCHEDULER_RESOURCE_COMPARE_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_RESOURCE_COMPARE_H__ // NOLINT

#include "common/proto/dstream_proto_common.pb.h"

namespace dstream {
namespace scheduler {

/************************************************************************/
/*pn resource statics : total/used/reserved                             */
/************************************************************************/
struct PNResStatics {
    PNResStatics() : m_pe_num(0) {}
    // total resource reported by pn
    Resource m_total;
    // resoured used by running processor
    Resource m_used;
    // resource reserved by processor, but the processor is not running
    Resource m_reserved;
    int m_pe_num;
};

/**************************************************************************/
/*define abstract class of compare resource and add/decrease two resource */
/*default compare resource by memory                                      */
/**************************************************************************/
class ResourceCompare {
public:
    virtual ~ResourceCompare() {}
    // compare two pn statics resources
    // if resource1 less than resource2 return true
    // compare by remained memory
    virtual bool operator() (const PNResStatics& pn_statics1,
                             const PNResStatics& pn_statics2) const;

    // compare resource and pn resource statics by memory
    // if pn resource statics can contain resource return true
    virtual bool operator()(const Resource& resource,
                            const PNResStatics& pn_res_statics) const;

    // Add add_resource to pn resource statics
    // Add resource by memory
    virtual void AddResource(const PNResStatics& add_resource,
                             PNResStatics* pn_res_statics) const;

    // subtract sub_resource from resource, return true success
    // Sub resource by memory
    virtual bool SubtractResource(const PNResStatics& sub_resource,
                                  PNResStatics* pn_res_statics,
                                  bool sub_pe_num = true) const;

    // add add_res to res
    virtual void AddResource(const Resource& add_res,
                             Resource* res) const;
    // Subtract sub_res from resource
    virtual void SubtractResource(const Resource& sub_res,
                                  Resource* resource) const;

    uint64_t GetResStaticsMemory(const PNResStatics& pn_statics) const;
    float GetResStaticsCPU(const PNResStatics& pn_statics) const;
};

// Get ResourceCompare by name
ResourceCompare* GetResourceCompareByName(const char* cmp_name = 0);

} // namespace scheduler
} // namespace dstream

#endif // NOLINT
