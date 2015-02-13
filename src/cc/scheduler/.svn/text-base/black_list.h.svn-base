/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/08/03
* @filename:    black_list.h
* @author:      zhanggengxin
* @brief:       black list
*/

#ifndef __DSTREAM_CC_SCHEDULER_BLACKLIST_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_BLACKLIST_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <utility>
#include "common/application_tools.h"
#include "common/dstream_type.h"
#include "common/proto/cluster_config.pb.h"

namespace dstream {
namespace scheduler {

// fail pn information
struct FailPNInfo {
    FailPNInfo(PNID pn_id); // NOLINT
    void AddFail();
    PNID m_pn_id;
    std::set<double> m_fail_time_set;
};

class FailPNInfoCompare {
public:
    bool operator() (const FailPNInfo& pid1, const FailPNInfo& pid2) {
        return pid1.m_pn_id.id() < pid2.m_pn_id.id();
    }
};

typedef std::set<FailPNInfo, FailPNInfoCompare> FailPnSet;
typedef FailPnSet::iterator FailPnSetIter;

typedef std::map<PEID, FailPnSet, application_tools::PEIDCompare> PeFailPnMap;
typedef PeFailPnMap::iterator PeFailPnMapIter;
typedef std::pair<PEID, FailPnSet> PeFailMapPair;

typedef std::map<PEID, PNIDSet, application_tools::PEIDCompare> PePnSetMap;


// BlackList : The blacklist is in memory
class BlackList : public thread::Thread {
public:
    BlackList() : m_config(NULL), m_run_interval(0) {}
    void set_config(config::Config* config) {
        m_config = config;
    }
    ~BlackList() {
        Stop();
    }
    // Add pe fail on pn
    int32_t AddFailPE(const PEID& pe_id, const PNID& pn_id);
    // Get Black list of pe
    int32_t GetBlackList(const PEID& pe_id, PNIDSet& black_set);
    // Get AllBlackList
    int32_t GetPNBlackList(PNIDSet* black_set);
    int32_t GetPEFailMap(PePnSetMap* pe_fail_map);
    // Clear Black list
    
    int32_t ErasePEBlackList(const PEID& pe_id);
    int32_t GetPEBlackList(PeFailPnMap* p_pe_failpn_map);
    int32_t ClearBlackList();

    // Add black list pn : used for users
    int32_t AddFailPN(const PNID& pn_id);
    // Remove black list pn : used for users
    int32_t RemoveFailPN(const PNID& pn_id);

    void Run();

    void set_run_interval(int run_interval) {
        m_run_interval = run_interval;
    }
    void SignalSleep() {
        m_sleep_condition.Signal();
    }

    // update black list according to meta
    void UpdateBlackList();

private:
    PeFailPnMap m_pe_failpn_map;
    std::set<PNID, PNIDCompare> m_black_list_pn; 
    MutexLock m_map_lock;
    ClusterConfig m_cluster_config;
    config::Config* m_config;
    int m_run_interval;
};

} // namespace scheduler
} // namespace dstream

#endif // NOLINT
