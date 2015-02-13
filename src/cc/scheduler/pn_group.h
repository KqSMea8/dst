/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/12/20
* @filename:    pn_group.h
* @author:      fangjun
* @brief:       pn_group
*/

#ifndef __DSTREAM_CC_SCHEDULER_PNGROUP_H__ // NOLINT
#define __DSTREAM_CC_SCHEDULER_PNGROUP_H__ // NOLINT

#include <list>
#include <string>
#include "common/proto/cluster_config.pb.h"
#include "common/proto/dstream_proto_common.pb.h"
#include "common/proto/pn.pb.h"
#include "common/rwlock.h"
#include "common/zk_client.h"
#include "processmaster/zk_watcher.h"

namespace dstream {
namespace scheduler {

class PNGroup {
public:
    PNGroup() {}
    virtual ~PNGroup() {}

    /**
    * @brief  Update local pn group in memory according to info on zk
    *
    * @return
    * @author   fangjun, fangjun02@baidu.com
    * @date   2013-01-04
    */
    int32_t UpdatePNGroup();
    /**
    * @brief  Get pn id list of the group name
    *
    * @param  group_name : the pn group name
    * @param  pn_list    : all the pnid in this group
    *
    * @return
    * @author   fangjun, fangjun02@baidu.com
    * @date   2013-01-04
    */
    int32_t GetPreferPNList(const std::string& group_name, PNIDSet& pn_list);

    // used for test
    void GetPNGroup(PNGroups* pn_group) {
        *pn_group = m_pn_group;
    }
    uint32_t GetGroupSize() {
        return m_pn_group.group_list_size();
    }

protected:
    /**
    * @brief  Get the pnid registed on zk according to the pn hostname and the pn info on zk
    *
    * @param  pn_name : pn hostname
    * @param  pn_list : pn info on zk
    * @param  pn_id   : the pnid of the hostname
    *
    * @return
    * @author   fangjun, fangjun02@baidu.com
    * @date   2013-01-04
    */
    int32_t GetPNID(const std::string& pn_name, const std::list<PN>& pn_list, PNID& pn_id);

private:
    RWLock m_pn_group_lock;
    /**
    * @brief  group info in memory
    * @author   fangjun, fangjun02@baidu.com
    * @date   2013-01-04
    */
    PNGroups m_pn_group;
};

class PNGroupWatcher : public zk_watcher::DStreamWatcher {
public:
    PNGroupWatcher(config::Config* config, PNGroup* pn_group_ptr)
        : DStreamWatcher(config), m_pn_group_ptr(pn_group_ptr) {}
    virtual ~PNGroupWatcher();

    /**
    * @brief  This function will e trigged by all the child event of the pn group node
    *
    * @param  type
    * @param  state
    * @param  path
    * @author   fangjun, fangjun02@baidu.com
    * @date   2013-01-04
    */
    virtual void OnEvent(int type, int state, const char* path);
private:
    PNGroup* m_pn_group_ptr;
};

} // namespace scheduler
} // namespace dstream

#endif // NOLINT
