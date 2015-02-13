/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/04
* @filename:    zk_watcher.h
* @author:      zhanggengxin
* @brief:       ZKWatcher
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_ZK_WATCHER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_ZK_WATCHER_H__ // NOLINT

#include <string>

#include "common/dstream_type.h"
#include "common/zk_client.h"

namespace dstream {
namespace zk_watcher {

/************************************************************************/
/* DStream Watcher : using to handle dstream zk event                   */
/************************************************************************/
class DStreamWatcher : public zk_client::Watcher {
public:
    explicit DStreamWatcher(config::Config* config) : m_config(config), m_init(false) {}
    virtual ~DStreamWatcher() {}
    void OnEvent(int type, int state, const char* /*path*/);
    virtual void OnNodeDelete(const char* /*path*/) {}
    virtual void OnDataChange(const char* /*path*/) {}
    virtual void OnSessionExpire(const char* /*path*/) {}
    virtual int32_t StartWatch(const std::string& path);
    virtual int32_t StartWatchPM();
    virtual int32_t StartWatchPN(const std::string& pn_name);
    virtual int32_t StartWatchPNGroup();
    virtual int32_t StartWatchAll();
    virtual int32_t StartWatchChildDelete(const std::string& path);
    virtual int32_t StopWatch();
    virtual int32_t UnWatch(const std::string& path);
    virtual int32_t UnWatchPM();
    virtual int32_t UnWatchPN(const std::string& pn_name);
    virtual int32_t UnWatchPNGroup();
    virtual int32_t UnWatchChildDelete(const std::string& path);
    void UnWatchAll();

protected:
    config::Config* m_config;

private:
    int32_t CreateZK();
    bool m_init;
    auto_ptr::AutoPtr<zk_client::ZkClient> m_zk_client;
};

} // namespace zk_watcher
} // namespace dstream

#endif // NOLINT
