/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/14
* @filename:    process_master.cc
* @author:      zhanggengxin
* @brief:       process master
*/

#include "processmaster/process_master.h"

#include "metamanager/meta_manager.h"
#include "metamanager/zk_config.h"
#include "processmaster/app_status.h"
#include "processmaster/check_app_task.h"
#include "processmaster/dstream_event_manager.h"
#include "processmaster/pm_common.h"
#include "processmaster/pm_watcher.h"
#include "processmaster/pn_client_status.h"

namespace dstream {
namespace process_master {
using namespace auto_ptr;
using namespace check_app_task;
using namespace dstream_event_manager;
using namespace event_manager;
using namespace meta_manager;
using namespace pm_common;
using namespace pm_gc;
using namespace pn_client_status;
using namespace pm_watcher;
using namespace zk_watcher;
using namespace pm_rpc_server;
using namespace scheduler;

bool ProcessMaster::Init() {
    m_pm_watcher = AutoPtr<DStreamWatcher>(new PMWatcher(m_config, this));
    if (RegisterAndWatch()) {
        m_scheduler = AutoPtr<Scheduler>(GetScheduler(m_config));
        if (!m_scheduler.PtrValid()) {
            DSTREAM_WARN("start scheduler fail");
            return false;
        }
        if (!m_scheduler->Init(m_config)) {
            DSTREAM_WARN("Init Scheduler fail");
            return false;
        }
        m_event_manager = AutoPtr<EventManager>(new DStreamEventManager(m_config, m_scheduler));
        if (!m_event_manager.PtrValid() || !m_event_manager->Start()) {
            DSTREAM_WARN("start event manager fail");
            return false;
        }
        // first suspend event manager
        m_event_manager->Suspend();

        m_gc = AutoPtr<GC>(new GC(m_config, m_event_manager, m_scheduler));
        if (PNClientStatus::InitPNStatus(m_event_manager, m_gc, m_scheduler) != error::OK) {
            DSTREAM_WARN("init pn status fail");
            return false;
        }
        // init app status
        if (app_status::AppStatus::InitAppStatus() != error::OK) {
            DSTREAM_WARN("init app status fail");
            return false;
        }
        m_rpc_server = AutoPtr<PMRPCServer>(new PMRPCServer);
        if (!m_rpc_server->Init(m_event_manager, m_pm_watcher, m_gc, m_scheduler) ||
            !m_rpc_server->StartServer()) {
            DSTREAM_WARN("start rpc server fail");
            return false;
        }
        DSTREAM_INFO("rpc server start success");
        m_pm.set_host(m_rpc_server->server_ip());
        m_pm.set_report_port(m_rpc_server->server_port());
        m_pm.set_submit_port(m_rpc_server->server_port());
        m_pm.set_httpd_port(m_httpd_port);
        MetaManager* meta = MetaManager::GetMetaManager();
        if (!meta->InitMetaManager(m_config)) {
            DSTREAM_WARN("Init meta manager fail");
            return false;
        }
        if (meta->UpdatePM(m_pm) == error::OK) {
            DSTREAM_INFO("[%s] add pm information success: "
                         " [%s:%d]", __FUNCTION__, m_pm.host().c_str(),
                         m_pm.report_port());
            AppCheckTask app_check_task(m_event_manager, m_gc, m_scheduler);
            if (!app_check_task.RunCheckTask()) {
                DSTREAM_WARN("check app task fail");
                return false;
            }
            m_gc->Start();
            // resume event manager
            m_event_manager->Resume();
            return true;
        } else {
            DSTREAM_WARN("add pm information fail");
            return false;
        }
    }
    DSTREAM_WARN("pm init fail");
    return false;
}

ProcessMaster::~ProcessMaster() {
    Stop();
    if (m_pm_watcher.PtrValid()) {
        m_pm_watcher->UnWatchAll();
    }
    if (m_gc.PtrValid()) {
        m_gc->Stop();
    }
    if (m_rpc_server.PtrValid()) {
        m_rpc_server->StopServer();
    }
    if (m_event_manager.PtrValid()) {
        m_event_manager->Stop();
    }
}

void ProcessMaster::Run() {
    MutexLockGuard lock_guard(m_lock);
    m_working = true;
    while (m_working && !m_stop) {
        m_working = false;
        MetaManager* meta = MetaManager::GetMetaManager();
        if (NULL != meta) {
            m_working = (meta->InitMetaManager(m_config) && Init());
        }
        DSTREAM_INFO("start pm working thread");
        if (m_wait_for_start) {
            m_wait_for_start = false;
            // DSTREAM_DEBUG("notify condition");
            m_condition->notifyAll();
        }
        if (!m_working) {
            DSTREAM_WARN("start pm fail");
            return;
        }
        m_working_master = true;
        DSTREAM_INFO("pm is working master");
        m_condition->wait();
        DSTREAM_DEBUG("condition is ok");
    }
    if (m_wait_for_start) {
        m_wait_for_start = false;
        DSTREAM_DEBUG("notify condition");
        m_condition->notifyAll();
    }
    DSTREAM_INFO("[%s] pm working thread run over", __FUNCTION__);
}

bool ProcessMaster::Start(const char* config_file) {
    if (NULL == config_file) {
        config_file = config_const::kPMConfigFile;
    }
    AutoPtr<Config> pm_config = AutoPtr<Config>(new Config);
    if (pm_config->ReadConfig(config_file) != error::OK) {
        DSTREAM_WARN("[%s] read config [%s] fail", __FUNCTION__, config_file);
        return false;
    }
    return Start(pm_config);
}

bool ProcessMaster::Start(AutoPtr<Config>& pm_config) {
    if (error::OK != GetLocalhostIP(&m_local_host_ip)) {
        DSTREAM_ERROR("[%s] get host ip fail, fail to start pm", __FUNCTION__);
        return false;
    }
    if (!pm_config.PtrValid()) {
        DSTREAM_WARN("[%s] config is invalid", __FUNCTION__);
        return false;
    }
    MutexLockGuard lock_guard(m_lock);
    m_config = pm_config;
    g_config = m_config;
    m_condition = AutoPtr<Condition>(new Condition(m_lock));
    if (!MetaManager::InitMetaManagerContext(m_config)) {
        DSTREAM_WARN("[%s] init meta manager fail, start pm fail", __FUNCTION__);
        return false;
    }
    m_wait_for_start = true;
    if (!Thread::Start()) {
        m_wait_for_start = false;
        DSTREAM_WARN("[%s] start thread fail", __FUNCTION__);
        return false;
    }
    m_condition->wait();
    DSTREAM_INFO("[%s] start pm over...", __FUNCTION__);
    return m_working;
}

void ProcessMaster::Stop() {
    m_lock.lock();
    DSTREAM_INFO("[%s] pm stop", __FUNCTION__);
    m_stop = true;
    if (m_condition.PtrValid() && m_working) {
        m_working = false;
        DSTREAM_DEBUG("notify condition");
        m_condition->notifyAll();
    }
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta) {
        m_lock.unlock();
        Thread::Stop();
        return;
    }
    if (m_working_master) {
        meta->DeletePM();
    } else {
        meta->DeleteBackupPM(m_pm);
    }
    m_working_master = false;
    m_lock.unlock();
    Thread::Stop();
}

void ProcessMaster::StopForTest() {
    Stop();
    if (m_pm_watcher.PtrValid()) {
        m_pm_watcher->UnWatchAll();
    }
    if (m_gc.PtrValid()) {
        m_gc->Stop();
    }
    if (m_rpc_server.PtrValid()) {
        m_rpc_server->StopServer();
    }
    if (m_event_manager.PtrValid()) {
        m_event_manager->Stop();
    }
}

void ProcessMaster::NodeDelete(const char* path) {
    DSTREAM_INFO("[%s] node %s delete", __FUNCTION__, path);
    if (m_config.PtrValid()) {
        zk_config::ZKConfig zk_config_instance(m_config);
        std::string node_path = path;
        // if delete node is pm
        if (zk_config_instance.PMPath() == node_path) {
            MutexLockGuard lock_guard(m_lock);
            if (m_working && !m_stop) {
                m_working_master = false;
                m_watch_master_event = false;
                DSTREAM_DEBUG("notify condition");
                m_condition->notifyAll();
            } else {
                if (m_watch_master_event) {
                    m_watch_master_event = false;
                    m_condition->notifyAll();
                }
            }
            return;
        }
        PNClientStatus* status = PNClientStatus::GetPNClientStatus();
        int pos = node_path.rfind('/');
        std::string parent_path = node_path.substr(0, pos);
        std::string node_name = node_path.substr(pos + 1);
        // if delete node is pn node
        if (parent_path == zk_config_instance.PNRootPath()) {
            DSTREAM_WARN("pn %s fail", node_name.c_str());
            PNID pn_id;
            *(pn_id.mutable_id()) = node_name;
            if (status->RemovePNStatus(pn_id) != error::OK) {
                DSTREAM_WARN("remove pn [%s] fail", node_name.c_str());
            }
            return;
        }
        if (parent_path == zk_config_instance.ClientRootPath()) {
            status->RemoveClient(node_path);
        }
        // delete node is backup pm
        if (path == m_pm.id()) {
            MutexLockGuard lock_guard(m_lock);
            if (m_watch_master_event) {
                m_watch_master_event = false;
                DSTREAM_DEBUG("notify condition");
                m_condition->notify();
            }
            return;
        }
    }
}

void ProcessMaster::NodeChanged(const char* path) {
    DSTREAM_INFO("[%s] node [%s] changed", __FUNCTION__, path);
    if (m_config.PtrValid()) {
        zk_config::ZKConfig zk_config_instance(m_config);
        if (zk_config_instance.PMPath() == path) {
            DSTREAM_INFO("[%s] pm changed", __FUNCTION__);
            MutexLockGuard lock_guard(m_lock);
            if (m_working_master) {
                PM meta_pm;
                MetaManager* meta = MetaManager::GetMetaManager();
                if (NULL != meta && meta->GetPM(&meta_pm) == error::OK) {
                    if (!PMEqual(m_pm, meta_pm)) {
                        if (meta->UpdatePM(m_pm) != error::OK) {
                            DSTREAM_WARN("[%s] working master update pm fail", __FUNCTION__);
                        }
                    }
                }
            }
        }
    }
}

bool ProcessMaster::PMEqual(const PM& pm1, const PM& pm2, bool rec) {
    if (pm1.has_host()) {
        if ((pm2.has_host() && pm1.host() != pm2.host()) ||
            !pm2.has_host()) {
            DSTREAM_INFO("[%s] host not equal", __FUNCTION__);
            return false;
        }
    }
    if (pm1.has_submit_port()) {
        if ((pm2.has_submit_port() && pm2.submit_port() != pm1.submit_port()) ||
            !pm2.has_submit_port()) {
            DSTREAM_WARN("[%s] submit port not equal", __FUNCTION__);
            return false;
        }
    }
    if (rec) {
        return PMEqual(pm2, pm1, false);
    }
    return true;
}

// Do not modify this function
bool ProcessMaster::RegisterAndWatch() {
    int32_t res = error::OK;
    MetaManager* meta_manager = MetaManager::GetMetaManager();
    m_pm.set_version(VERSION);
    while (!m_stop) {
        m_pm.set_host(m_local_host_ip);
        if ((res = meta_manager->AddPM(m_pm)) == error::OK) {
            // create pm success and watch all child
            DSTREAM_INFO("create pm success, so this is working master");
            if ((res = m_pm_watcher->StartWatchAll()) != error::OK) {
                meta_manager->DeletePM();
            }
            return res == error::OK;
        } else {
            // pm node exist
            if (res == error::ZK_NODE_EXISTS &&
                m_pm_watcher->StartWatchPM() == error::OK) {
                meta_manager->GetPM(&m_pm);
                DSTREAM_INFO("[%s] master pm is:[%s:%d]", __FUNCTION__,
                             m_pm.host().c_str(),
                             m_pm.submit_port());
                m_pm.set_host(m_local_host_ip);
                if ((res = meta_manager->AddBackupPM(&m_pm)) != error::OK) {
                    DSTREAM_WARN("[%s] backup pm register self fail", __FUNCTION__);
                    return false;
                }
                meta_manager->GetBackupPM(&m_pm);
                DSTREAM_INFO("[%s] Add backup pm [%s]", __FUNCTION__,
                             m_pm.host().c_str());
                if (m_pm_watcher->StartWatch(m_pm.id().c_str()) != error::OK) {
                    DSTREAM_WARN("[%s] backup pm [%s] watch self fail", __FUNCTION__,
                                 m_pm.id().c_str());
                    return false;
                }
                DSTREAM_INFO("[%s] master pm already, wait for pm exist", __FUNCTION__);
                m_working = true;
                if (m_wait_for_start) {
                    m_wait_for_start = false;
                    DSTREAM_DEBUG("notify condition");
                    m_condition->notifyAll();
                }
                m_watch_master_event = true;
                m_condition->wait();
                if (m_stop) {
                    DSTREAM_INFO("[%s] pm stop", __FUNCTION__);
                    return false;
                }
                DSTREAM_INFO("[%s] master pm stop", __FUNCTION__);
                m_pm_watcher->UnWatchPM();
                m_pm_watcher->UnWatch(m_pm.id().c_str());
                if ((res = meta_manager->DeleteBackupPM(m_pm)) != error::OK &&
                    res != error::ZK_NO_NODE) {
                    DSTREAM_WARN("[%s] backup pm [%s] delete self fail", __FUNCTION__,
                                 m_pm.id().c_str());
                    return false;
                }
            } else {
                DSTREAM_WARN("watch pm fail");
                return false;
            }
        }
    }
    return false;
}

} // namespace process_master
} // namespace dstream
