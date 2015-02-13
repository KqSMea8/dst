/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/12/20
* @filename:    pn_group.cc
* @author:      fangjun
* @brief:       pn_group
*/

#include "scheduler/pn_group.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace scheduler {

using namespace meta_manager;

int32_t PNGroup::UpdatePNGroup() {
    int32_t ret = error::OK;
    MetaManager* meta = MetaManager::GetMetaManager();
    PNGroups zk_pn_group;
    if ((ret = meta->GetPNGroups(&zk_pn_group)) < error::OK) {
        DSTREAM_WARN("fail to get pn group info on zk");
        return ret;
    }
    m_pn_group_lock.LockForWrite();
    m_pn_group = zk_pn_group;
    m_pn_group_lock.Unlock();
    return ret;
}

int32_t PNGroup::GetPNID(const std::string& pn_name, const std::list<PN>& pn_list, PNID& pn_id) {
    std::string ip;
    if (error::OK != GetRemotehostIP(pn_name, ip)) {
        DSTREAM_WARN("get remote host [%s] ip fail", pn_name.c_str());
        return error::GET_REMOTEHOST_FAIL;
    }

    std::list<PN>::const_iterator it = pn_list.begin();
    for (it = pn_list.begin(); it != pn_list.end(); it++) {
        DSTREAM_DEBUG("search pn name is %s, current pn name is %s",
                      ip.c_str(), it->pn_id().id().c_str());
        size_t found = it->pn_id().id().find(ip);
        if (found != std::string::npos) {
            pn_id = it->pn_id();
            return error::OK;
        }
    }
    DSTREAM_DEBUG("pn [%s] does not register on zk", pn_name.c_str());
    return error::FOUND_ZK_PN_FAIL;
}

int32_t PNGroup::GetPreferPNList(const std::string& group_name, PNIDSet& prefer_pn_list) {
    int32_t ret = error::OK;
    MetaManager* meta = MetaManager::GetMetaManager();
    std::list<PN> pn_list;
    if ((ret = meta->GetPNList(&pn_list)) < error::OK) {
        DSTREAM_WARN("fail to Get PN List on meta");
        return ret;
    }
    PNIDSet other_pn_set;
    for (int i = 0; i < m_pn_group.group_list_size(); i++) {
        if (group_name != "default") {
            if (m_pn_group.group_list(i).group_name() == group_name) {
                for (int j = 0; j < m_pn_group.group_list(i).pn_list_size(); j++) {
                    PNID pn_id;
                    if (GetPNID(m_pn_group.group_list(i).pn_list(j), pn_list, pn_id) < error::OK) {
                        DSTREAM_WARN("found pn [%s]'s pn id fail",
                                     m_pn_group.group_list(i).pn_list(j).c_str());
                        continue;
                    }
                    DSTREAM_DEBUG("found calified pn is %s", pn_id.id().c_str());
                    prefer_pn_list.insert(pn_id);
                }
            }
        } else {
            for (int j = 0; j < m_pn_group.group_list(i).pn_list_size(); j++) {
                PNID pn_id;
                if (GetPNID(m_pn_group.group_list(i).pn_list(j), pn_list, pn_id) < error::OK) {
                    continue;
                }
                other_pn_set.insert(pn_id);
            }
        }
    }
    if (group_name == "default") {
        std::list<PN>::iterator pn_iter, pn_end;
        pn_end = pn_list.end();
        PNIDSetIter pn_find, pn_find_end;
        pn_find_end = other_pn_set.end();
        for (pn_iter = pn_list.begin(); pn_iter != pn_end; pn_iter++) {
            if ((pn_find = other_pn_set.find(pn_iter->pn_id())) == pn_find_end) {
                prefer_pn_list.insert(pn_iter->pn_id());
            }
        }
    }
    return error::OK;
}

PNGroupWatcher::~PNGroupWatcher() {
    DStreamWatcher::UnWatchAll();
}

void PNGroupWatcher::OnEvent(int type, int stat, const char* path) {
    DSTREAM_DEBUG("PN Group found a event!");
    if (NULL != m_pn_group_ptr) {
        m_pn_group_ptr->UpdatePNGroup();
    }
}

} // namespace scheduler
} // namespace dstream
