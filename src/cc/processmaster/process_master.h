/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/14
* @filename:    process_master.h
* @author:      zhanggengxin
* @brief:       process master
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_PROCESS_MASTER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_PROCESS_MASTER_H__ // NOLINT

#include <string>
#include "processmaster/pm_rpc_server.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace process_master {

/************************************************************************/
/*class ProcessMaster : manager meta data and schedule task             */
/************************************************************************/
class ProcessMaster : public thread::Thread {
public:
    ProcessMaster() : m_watch_master_event(false), m_working_master(false), m_httpd_port(0) {}
    ~ProcessMaster();
    void NodeDelete(const char* path);
    void NodeChanged(const char* path);
    bool working() {
        return m_working;
    }
    void Run();
    bool stop() {
        return m_stop;
    }
    bool Start(const char* config_file);
    bool Start(auto_ptr::AutoPtr<config::Config>& config);
    virtual void Stop();
    virtual void Stop(bool join_thread) {}

    // get event_manager used for test
    event_manager::EventManager* event_manager() {
        return m_event_manager;
    }
    config::Config* config() {
        return m_config;
    }
    bool working_master() const {
        return m_working_master;
    }
    scheduler::Scheduler* scheduler() {
        return m_scheduler;
    }
    pm_gc::GC* gc() {
        return m_gc;
    }
    void StopForTest();

    void SetHttpdPort(uint32_t port) {
        m_httpd_port = port;
    }

private:
    bool Init();
    bool RegisterAndWatch();
    // compare two pm
    bool PMEqual(const PM& pm1, const PM& pm2, bool rec = true);
    auto_ptr::AutoPtr<config::Config> m_config;
    auto_ptr::AutoPtr<pm_rpc_server::PMRPCServer> m_rpc_server;
    auto_ptr::AutoPtr<zk_watcher::DStreamWatcher> m_pm_watcher;
    MutexLock m_lock;
    auto_ptr::AutoPtr<Condition> m_condition;
    auto_ptr::AutoPtr<pm_gc::GC> m_gc;
    auto_ptr::AutoPtr<event_manager::EventManager> m_event_manager;
    auto_ptr::AutoPtr<scheduler::Scheduler> m_scheduler;
    PM m_pm;
    std::string m_local_host_ip;
    bool m_wait_for_start;
    bool m_watch_master_event;
    // used for test
    bool m_working_master;
    uint32_t m_httpd_port;
};

} // namespace process_master
} // namespace dstream

#endif // NOLINT

