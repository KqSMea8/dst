/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/04/17
* @filename:    resource_manager.h
* @author:      zhanggengxin@baidu.com
* @brief:       resource manager
*/

#include <list>
#include <map>
#include <utility>
#include "common/dstream_type.h"
#include "scheduler/resource_compare.h"

#ifndef __DSTREAM_CC_SCHEDULER_SCHEDULER_RESOURCE_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_SCHEDULER_RESOURCE_MANAGER_H__ // NOLINT

namespace dstream {
namespace scheduler {

// define PNID->PNResourceStatics map
typedef std::map<PNID, PNResStatics, PNIDCompare> PnidStaticsMap;
typedef PnidStaticsMap::iterator PnidStaticsMapIter;
typedef std::pair<PNID, PNResStatics> PnidStaticsPair;
typedef std::pair<PnidStaticsMapIter, bool> PnidStaticsInsertRes;

/************************************************************************/
/* Resource Manager : manage pn resource                                */
/************************************************************************/
class ResourceManager {
public:
    virtual ~ResourceManager() {}
    // init resource manager : return {error::OK} if success
    virtual int32_t Init(config::Config* /*config*/) {
        return error::OK;
    }

    // Add PN Resource
    // return {error::OK} if add pn resource success
    // return {error::PN_STATICS_EXIST} if pn is already
    // return {error::ADD_PN_RESOURCE_FAIL} if add pn resource fail
    virtual int32_t AddPNResource(const PNID& pn_id,
                                  const Resource& total_resource,
                                  const std::list<Resource>* used_resouce,
                                  const ResourceCompare* res_cmp);
    // Add Allocated pn resource of pn_id
    virtual int32_t AddReservedResource(const PNID& pn_id,
                                        const Resource& resource,
                                        const ResourceCompare* res_cmp,
                                        int add_num = 1);
    // Get allocated resource
    virtual int32_t GetReservedResource(const PNID& pn_id,
                                        const Resource& resource,
                                        const ResourceCompare* res_cmp,
                                        int num = 1);

    // Release allocated resource
    virtual int32_t ReturnReservedResource(const PNID& pn_id,
                                           const Resource& resource,
                                           const ResourceCompare* res_cmp,
                                           int num);

    // Add used resource of pn_id
    virtual int32_t GetResource(const PNID& pn_id,
                                const Resource& resource,
                                const ResourceCompare* res_cmp,
                                int add_num = 1,
                                bool ignore_allocated = false);
    // Remove used resource of pn_id
    virtual int32_t ReturnResource(const PNID& pn_id,
                                   const Resource& resource,
                                   const ResourceCompare* res_cmp,
                                   int rm_num = 1);

    // check if then pn can allocated resource
    virtual bool CheckResource(const PNID& pn_id, const Resource& resource,
                               const ResourceCompare* res_cmp);

    // Remove pn from resource map : return {error::OK} if success
    // return {error::PN_STATICS_NOT_EXIST} if pn_id not exist
    virtual int32_t RemovePN(const PNID& pn_id);
    PnidStaticsMap PNResourceStatics();
    int pn_number();
protected:
    MutexLock m_lock;
    PnidStaticsMap m_pn_statics;
};

} // namespace scheduler
} // namespace dstream
#endif // NOLINT
