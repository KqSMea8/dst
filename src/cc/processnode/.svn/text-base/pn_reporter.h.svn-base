/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/7/3
* @filename:    pn_reporter.h
* @author:      lanbijia
* @brief:       process node report to process master
*/

#ifndef __DSTREAM_CC_PROCESSNODE_PN_REPORTER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PN_REPORTER_H__ // NOLINT

#include "common/application_tools.h"
#include "common/condition.h"
#include "common/config.h"
#include "common/dstream_type.h"
#include "common/mutex.h"
#include "common/rpc.h"
#include "common/rpc_const.h"
#include "common/thread.h"
#include "common/zk_client.h"

#include "processmaster/zk_watcher.h"

#include "processnode/config_map.h"
#include "processnode/pe_status.h"

namespace dstream {
namespace processnode {


/************************************************************************/
/* class PNReporter : process node report to process master             */
/************************************************************************/

class PNReporter : public thread::Thread,
    public zk_watcher::DStreamWatcher,
    public PEStatus {
public:
    PNReporter(config::Config* config, uint64_t total_mem, float total_cpu)
        : DStreamWatcher(config),
          m_time_cond(m_lock),
          m_pm_reconnect(true),
          m_report_pm_conn(NULL),
          m_total_mem(total_mem),
          m_total_cpu(total_cpu) {}
    ~PNReporter();

    void Run();

    int32_t WatchPM();
    void OnNodeDelete(const char* /*path*/);
    void OnDataChange(const char* /*path*/);
    void OnSessionExpire(const char* /*path*/);

    void set_pm_reconnect() {
        m_pm_reconnect = true;
    }

    int32_t ConnectPM();
    void DisconnectPM();

    void SignalCond();

private:
    MutexLock m_lock;
    Condition m_time_cond;

    bool m_pm_reconnect;
    rpc::Connection* m_report_pm_conn;
    uint64_t m_total_mem;
    float m_total_cpu;
};

extern PNReporter* g_pn_reporter;

} // namespace processnode
} // namespace dstream

#endif // NOLINT

