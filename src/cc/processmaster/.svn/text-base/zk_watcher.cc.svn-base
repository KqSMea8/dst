/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/04
* @filename:    zk_watcher.cc
* @author:      zhanggengxin
* @brief:       ZKWatcher
*/

#include "processmaster/zk_watcher.h"

#include <assert.h>
#include <cstddef>

#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "metamanager/zk_config.h"

namespace dstream {
namespace zk_watcher {
using namespace config_const;
using namespace auto_ptr;
using zk_config::ZKConfig;

#define CHECK_INIT()                    \
    int32_t watch_res = CreateZK();       \
    if (watch_res != error::OK) {          \
        return watch_res;                   \
    }                                     \
    if (!m_zk_client.PtrValid() && m_init) {  \
        DSTREAM_INFO("fake watch");         \
        return error::OK;                   \
    }                                     \

void DStreamWatcher::OnEvent(int type, int state, const char* path) {
    if (type == ZOO_SESSION_EVENT) {
        if (state == ZOO_EXPIRED_SESSION_STATE) {
            return OnSessionExpire(path);
        }
    }
    if (type == ZOO_DELETED_EVENT) {
        return OnNodeDelete(path);
    }
    if (type == ZOO_CHANGED_EVENT) {
        return OnDataChange(path);
    }
    if (type == ZOO_CHILD_EVENT) {
    }
}

int32_t DStreamWatcher::StartWatch(const std::string& path) {
    CHECK_INIT();
    m_zk_client->SetConnBrokenWatcher(this);
    return m_zk_client->WatchNode(path.c_str(), this);
}

int32_t DStreamWatcher::StartWatchAll() {
    CHECK_INIT();
    int create_zk_res = error::OK;
    ZKConfig zk_config(m_config);
    if ((create_zk_res = m_zk_client->WatchChildrenDelete(zk_config.PNRootPath().c_str(),
                         this)) != error::OK) {
        DSTREAM_WARN("watch pn child fail");
        return create_zk_res;
    }
    if ((create_zk_res = m_zk_client->WatchNode(zk_config.ClusterConfigPath().c_str(),
                                                this)) != error::OK) {
        m_zk_client->UnWatchChildrenDelete(zk_config.PNRootPath().c_str());
        DSTREAM_WARN("watch config fail");
    }
    if ((create_zk_res = StartWatchPM()) != error::OK) {
        DSTREAM_WARN("watch pm fail");
    }
    return create_zk_res;
}

int32_t DStreamWatcher::StartWatchPM() {
    return StartWatch(ZKConfig(m_config).PMPath());
}

int32_t DStreamWatcher::StartWatchPN(const std::string& pn_name) {
    CHECK_INIT();
    ZKConfig zk_config(m_config);
    std::string pn_path = zk_config.PNRootPath() + "/" + pn_name;
    return StartWatch(pn_path);
}

int32_t DStreamWatcher::StartWatchPNGroup() {
    CHECK_INIT();
    int create_zk_res = error::OK;
    ZKConfig zk_config(m_config);
    DSTREAM_INFO("begin to watch %s", zk_config.PNGroupRootPath().c_str());
    if ((create_zk_res = m_zk_client->WatchChildrenAll(zk_config.PNGroupRootPath().c_str(),
                         this)) != error::OK) {
        DSTREAM_WARN("watch path[%s] fail", zk_config.PNGroupRootPath().c_str());
    }
    return create_zk_res;
}

int32_t DStreamWatcher::StartWatchChildDelete(const std::string& path) {
    CHECK_INIT();
    int create_zk_res = error::OK;
    ZKConfig zk_config(m_config);
    if ((create_zk_res = m_zk_client->WatchChildrenDelete(path.c_str(),
                         this)) != error::OK) {
        DSTREAM_WARN("watch path[%s] fail", path.c_str());
    }
    return create_zk_res;
}

int32_t DStreamWatcher::UnWatchPM() {
    return UnWatch(ZKConfig(m_config).PMPath());
}

int32_t DStreamWatcher::UnWatchPN(const std::string& pn_name) {
    CHECK_INIT();
    std::string pn_path = ZKConfig(m_config).PNRootPath() + "/" + pn_name;
    return UnWatch(pn_path);
}

int32_t DStreamWatcher::UnWatchPNGroup() {
    CHECK_INIT();
    ZKConfig zk_config(m_config);
    return m_zk_client->UnWatchChildrenAll(zk_config.PNGroupRootPath().c_str());
}

int32_t DStreamWatcher::StopWatch() {
    //  m_zk_client.Release();
    m_zk_client = AutoPtr<zk_client::ZkClient>();
    return 0;
}

int32_t DStreamWatcher::UnWatch(const std::string& path) {
    CHECK_INIT();
    return m_zk_client->UnWatchNode(path.c_str());
}

int32_t DStreamWatcher::UnWatchChildDelete(const std::string& path) {
    CHECK_INIT();
    return m_zk_client->UnWatchChildrenDelete(path.c_str());
}

int32_t DStreamWatcher::CreateZK() {
    if (m_zk_client.PtrValid()) {
        return error::OK;
    }
    ZKConfig zk_config(m_config);
    m_zk_client = AutoPtr<zk_client::ZkClient>(
        new zk_client::ZkClient(zk_config.GetZKPath().c_str(),
                                zk_config.Timeout(), this));
    if (!m_zk_client.PtrValid()) {
        return error::CREATE_ZK_FAIL;
    }
    m_init = true;
    return m_zk_client->Connect();
}

void DStreamWatcher::UnWatchAll() {
    //  m_zk_client.Release();
    m_zk_client = AutoPtr<zk_client::ZkClient>();
}

} // namespace zk_watcher
} // namespace dstream
