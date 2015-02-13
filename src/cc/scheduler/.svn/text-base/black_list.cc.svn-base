/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/08/03
* @filename:    black_list.cc
* @author:      zhanggengxin
* @brief:       black list
*/

#include "scheduler/black_list.h"

#include <string>

#include "common/application_tools.h"
#include "common/utils.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace scheduler {
using namespace application_tools;
using namespace config_const;
using meta_manager::MetaManager;

FailPNInfo::FailPNInfo(PNID pn_id) : m_pn_id(pn_id) {
    AddFail();
}

void FailPNInfo::AddFail() {
    m_fail_time_set.insert(GetTimeMS());
}

// Add pe fail on pn
int32_t BlackList::AddFailPE(const PEID& pe_id, const PNID& pn_id) {
    if (!m_stop) {
        MutexLockGuard lock_gurad(m_map_lock);
        DSTREAM_INFO("[%s] PE [%lu] fail on [%s]", __FUNCTION__,
                     pe_id.id(),
                     pn_id.id().c_str());
        PeFailPnMapIter find = m_pe_failpn_map.find(pe_id);
        if (find != m_pe_failpn_map.end()) {
            FailPnSet& fail_pn_set = find->second;
            FailPnSetIter fail_pn_find = fail_pn_set.find(pn_id);
            if (fail_pn_find != fail_pn_set.end()) {
                const_cast< set<double>& >(fail_pn_find->m_fail_time_set).insert(GetTimeMS());
            } else {
                fail_pn_set.insert(FailPNInfo(pn_id));
            }
        } else {
            FailPnSet fail_pn_set;
            fail_pn_set.insert(FailPNInfo(pn_id));
            m_pe_failpn_map.insert(PeFailMapPair(pe_id, fail_pn_set));
        }
    }
    return error::OK;
}

// Get black list of pe
int32_t BlackList::GetBlackList(const PEID& pe_id,
                                set<PNID, PNIDCompare>& black_set) {
    if (!m_stop) {
        int max_fail_num = kDefaultMaxPEFailOnPnNum;
        if (m_cluster_config.has_pe_failpn_max_num() &&
            m_cluster_config.pe_failpn_max_num() > 0) {
            max_fail_num = m_cluster_config.pe_failpn_max_num();
        }
        MutexLockGuard lock_gurad(m_map_lock);
        PeFailPnMapIter find = m_pe_failpn_map.find(pe_id);
        if (find != m_pe_failpn_map.end()) {
            FailPnSet& fail_pn_set = find->second;
            FailPnSetIter it, end;
            for (it = fail_pn_set.begin(), end = fail_pn_set.end(); it != end; ++it) {
                DSTREAM_DEBUG("[%s] pe [%lu] fail on pn [%s] [%zu] times "
                              "max time [%d]", __FUNCTION__,
                              pe_id.id(),
                              it->m_pn_id.id().c_str(),
                              it->m_fail_time_set.size(),
                              max_fail_num);
                if (it->m_fail_time_set.size() >= static_cast<size_t>(max_fail_num)) {
                    DSTREAM_INFO("[%s] pn [%s] is in pe [%lu] black list", __FUNCTION__,
                                 it->m_pn_id.id().c_str(),
                                 pe_id.id());
                    black_set.insert(it->m_pn_id);
                }
            }
        }
        if (!m_black_list_pn.empty()) {
            set<PNID, PNIDCompare>::iterator it, end;
            for (it = m_black_list_pn.begin(), end = m_black_list_pn.end();
                 it != end; ++it) {
                black_set.insert(*it);
            }
        }
    }
    return error::OK;
}


// Get AllBlackList
int32_t BlackList::GetPNBlackList(PNIDSet* black_set) {
    MutexLockGuard lock_guard(m_map_lock);
    if (!m_stop) {
        *black_set = m_black_list_pn;
    }
    return error::OK;
}

/** 
* @brief    clear pe blacklist
* 
* @param    pe_id clear the parameter peid corresponding blacklist
* 
* @return   ok
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-08-07
*/
int32_t BlackList::ErasePEBlackList(const PEID& pe_id) {
    if(!m_stop) {
        MutexLockGuard lock(m_map_lock);
        PeFailPnMapIter find = m_pe_failpn_map.find(pe_id);
        if (find != m_pe_failpn_map.end()) {
            m_pe_failpn_map.erase(find); 
        }
    }
    return error::OK;
}

/** 
* @brief    clear all black list in memory
* 
* @return   
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-08-07
*/
int32_t BlackList::ClearBlackList() {
    if(!m_stop) {
        MutexLockGuard lock(m_map_lock);
        m_pe_failpn_map.clear();
    }
    return error::OK;
}

int32_t BlackList::GetPEFailMap(PePnSetMap* pe_fail_map) {
    if (!m_stop) {
        int max_fail_num = kDefaultMaxPEFailOnPnNum;
        if (m_cluster_config.has_pe_failpn_max_num() &&
            m_cluster_config.pe_failpn_max_num() > 0) {
            max_fail_num = m_cluster_config.pe_failpn_max_num();
        }
        MutexLockGuard lock_gurad(m_map_lock);
        PeFailPnMapIter pe_it, pe_end;
        for (pe_it = m_pe_failpn_map.begin(), pe_end = m_pe_failpn_map.end();
             pe_it != pe_end; ++pe_it) {
            FailPnSet& fail_pn_set = pe_it->second;
            FailPnSetIter it, end;
            PNIDSet pn_set;
            for (it = fail_pn_set.begin(), end = fail_pn_set.end(); it != end; ++it) {
                if (it->m_fail_time_set.size() >= static_cast<size_t>(max_fail_num)) {
                    pn_set.insert(it->m_pn_id);
                }
            }
            if (!fail_pn_set.empty()) {
                (*pe_fail_map)[pe_it->first] = pn_set;
            }
        }
    }
    return error::OK;
}

// Black list thread run, update black list
void BlackList::Run() {
    int sleep_ms = kDefaultGCInterval;
    std::string sleep_time;
    if (NULL != m_config &&
        m_config->GetValue(kGCIntervalName, &sleep_time) == error::OK
        && (sleep_ms = atoi(sleep_time.c_str())) <= 0) {
        sleep_ms = kDefaultGCInterval;
    }
    m_run_interval = sleep_ms;
    while (!m_stop) {
        UpdateBlackList();
        m_sleep_condition.Wait(m_run_interval);
        m_sleep_condition.Init();
    }
}

// update black list according to meta
void BlackList::UpdateBlackList() {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL != meta) {
        if (meta->GetClusterConfig(&m_cluster_config) != error::OK) {
            DSTREAM_WARN("[%s] GetCluster Config fail", __FUNCTION__);
            return;
        }
        int pe_fail_reserve_time = kDefaultPEFailReserveTime;
        if (m_cluster_config.has_pefail_reserve_time() &&
            m_cluster_config.pefail_reserve_time() > 0) {
            pe_fail_reserve_time = m_cluster_config.pefail_reserve_time();
        }
        pe_fail_reserve_time *= 1000;
        // DSTREAM_DEBUG("[%s] pe fail reserved time [%d]", __FUNCTION__,
        //                                                 pe_fail_reserve_time);
        MutexLockGuard lock(m_map_lock);
        double now_time = GetTimeMS();
        set<PNID, PNIDCompare> meta_pnid_set;
        list<PN> meta_pn_list;
        bool get_pnlist_succ = false;
        if (meta->GetPNList(&meta_pn_list) == error::OK) {
            get_pnlist_succ = true;
            for (list<PN>::iterator it = meta_pn_list.begin(), end = meta_pn_list.end();
                 it != end; ++it) {
                meta_pnid_set.insert(it->pn_id());
            }
        }
        set<PNID, PNIDCompare>::iterator pn_find, pn_end;
        pn_end = meta_pnid_set.end();
        // udpate pe fail pn map
        if (!m_pe_failpn_map.empty()) {
            PeFailPnMapIter it, end;
            for (it = m_pe_failpn_map.begin(), end = m_pe_failpn_map.end(); it != end;) {
                ERROR_CODE res = error::OK;
                AppID pe_app_id;
                pe_app_id.set_id(app_id(it->first));
                ProcessorElement pe;
                if ((res = meta->GetProcessorElement(pe_app_id, it->first,
                                                     &pe)) == error::ZK_NO_NODE) {
                    m_pe_failpn_map.erase(it++);
                    continue;
                }
                FailPnSet& fail_pn_set = it->second;
                FailPnSetIter fail_it, fail_end;
                for (fail_it = fail_pn_set.begin(), fail_end = fail_pn_set.end();
                     fail_it != fail_end;) {
                    if (now_time - * (fail_it->m_fail_time_set.rbegin()) > pe_fail_reserve_time ||
                        (get_pnlist_succ &&
                         (pn_find = meta_pnid_set.find(fail_it->m_pn_id)) == pn_end)) {
                        fail_pn_set.erase(fail_it++);
                        continue;
                    }
                    ++fail_it;
                }
                if (fail_pn_set.empty()) {
                    m_pe_failpn_map.erase(it++);
                    continue;
                }
                ++it;
            }
        }

        // update pn black list
        if (!m_black_list_pn.empty()) {
            set<PNID, PNIDCompare>::iterator it, end;
            for (it = m_black_list_pn.begin(), end = m_black_list_pn.end(); it != end;) {
                if (get_pnlist_succ && (pn_find = meta_pnid_set.find(*it)) == pn_end) {
                    DSTREAM_INFO("[%s] remove blacklist pn [%s]", __FUNCTION__,
                                 it->id().c_str());
                    m_black_list_pn.erase(it++);
                    continue;
                }
                ++it;
            }
        }
    }
}

int32_t BlackList::AddFailPN(const PNID& pn_id) {
    if (!m_stop) {
        DSTREAM_INFO("[%s] Add Fail pn [%s]", __FUNCTION__, pn_id.id().c_str());
        MutexLockGuard lock_gurad(m_map_lock);
        m_black_list_pn.insert(pn_id);
    }
    return error::OK;
}

int32_t BlackList::RemoveFailPN(const PNID& pn_id) {
    if (!m_stop) {
        DSTREAM_INFO("[%s] Add Fail pn [%s]", __FUNCTION__, pn_id.id().c_str());
        MutexLockGuard lock_gurad(m_map_lock);
        m_black_list_pn.erase(pn_id);
    }
    return error::OK;
}

} // namespace scheduler
} // namespace dstream
