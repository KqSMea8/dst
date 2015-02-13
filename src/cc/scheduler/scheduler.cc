/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/11
* @filename:    scheduler.cc
* @author:      zhanggengxin
* @brief:       schduler
*/

#include "scheduler/scheduler.h"

#include <math.h>
#include "metamanager/meta_manager.h"
#include "scheduler/resource_scheduler.h"

namespace dstream {
namespace scheduler {
using namespace config_const;
using namespace meta_manager;

Scheduler* GetScheduler(Config* config) {
    if (NULL == config) {
        DSTREAM_INFO("Config is null so return plain scheduler");
        return new ResourceScheduler;
    }
    std::string scheduler_name;
    if (config->GetValue(kSchedulerName, &scheduler_name) != error::OK) {
        scheduler_name = kResourceScheduler;
    }
    if (scheduler_name == kResourceScheduler) {
        DSTREAM_INFO("get resource scheduler");
        return new ResourceScheduler;
    }
    DSTREAM_WARN("scheduler [%s] is wrong", scheduler_name.c_str());
    return NULL;
}

// if the pn number can work
bool Scheduler::CanScheduler(int report_pn_num) {
    if (report_pn_num <= 0) {
        DSTREAM_WARN("[%s] report number [%d] less than 1", __FUNCTION__, report_pn_num);
        return false;
    }
    MetaManager* meta = MetaManager::GetMetaManager();
    assert(NULL != meta);
    ClusterConfig cluster_config;
    if (meta->GetClusterConfig(&cluster_config) != error::OK) {
        DSTREAM_ERROR("[%s] Get ClusterConfig Fail", __FUNCTION__);
        return false;
    }
    if (cluster_config.has_check_pn_size() && cluster_config.check_pn_size()) {
        PNMachines pn_machines;
        ERROR_CODE res = error::OK;
        if ((res = meta->GetPNMachines(&pn_machines)) != error::OK) {
            if (res == error::ZK_NO_NODE) {
                DSTREAM_INFO("[%s] backup machines is not set", __FUNCTION__);
                return true;
            }
            DSTREAM_WARN("[%s] GetPNMachins error", __FUNCTION__);
            return false;
        }
        float min_pn_per = kMinActivePNPercentage;
        if (cluster_config.has_min_host_percentage() &&
            cluster_config.min_host_percentage() > 0 &&
            cluster_config.min_host_percentage() <= 1.0) {
            min_pn_per = cluster_config.min_host_percentage();
        }
        int machines_size = pn_machines.pn_list_size();
        int requried_num = static_cast<int>(ceil(machines_size * min_pn_per));
        if (requried_num > report_pn_num) {
            DSTREAM_WARN("[%s] Scheduler cannot work : pn number [%d] "
                         "report pn [%d] required pn [%d]", __FUNCTION__,
                         machines_size,
                         report_pn_num,
                         requried_num);
            return false;
        }
    }
    return true;
}

int Scheduler::GetMaxPENumOnPN() {
    int max_pe_on_pn = kDefaultMaxPENumOnPN;
    if (m_cluster_config.has_max_pe_num_per_pn() &&
        m_cluster_config.max_pe_num_per_pn() > 0) {
        max_pe_on_pn = m_cluster_config.max_pe_num_per_pn();
    }
    return max_pe_on_pn;
}

} // namespace scheduler
} // namespace dstream
