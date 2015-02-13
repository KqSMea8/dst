/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    pm watcher
*********************************************************************/

#ifndef __DSTREAM_PROCESSMASTER_PM_WATCHER_H__ // NOLINT
#define __DSTREAM_PROCESSMASTER_PM_WATCHER_H__ // NOLINT

#include "processmaster/process_master.h"
#include "processmaster/zk_watcher.h"

namespace dstream {
namespace pm_watcher {

/************************************************************************/
/* pm watcher, process select primary pm and pn offline event           */
/************************************************************************/
class PMWatcher : public zk_watcher::DStreamWatcher {
public:
    PMWatcher(config::Config* config, process_master::ProcessMaster* pm)
        : DStreamWatcher(config), m_pm(pm) {}
    void set_pm(process_master::ProcessMaster* pm) {
        m_pm = pm;
    }
    void OnSessionExpire(const char* path);
    void OnNodeDelete(const char* path);
private:
    process_master::ProcessMaster* m_pm;
};

} // namespace pm_watcher
} // namespace dstream

#endif // NOLINT
