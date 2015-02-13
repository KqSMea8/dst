/**
* @file     zk.h
* @brief    zookeeper client for dstream client.
* @author   konghui, konghui@baidu.com
* @date     2011-12-23
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_CLIENT_ZK_H__ //NOLINT
#define __DSTREAM_CC_CLIENT_ZK_H__ //NOLINT

#include <string>
#include "common/proto/application.pb.h"
#include "common/proto/pm.pb.h"
#include "common/zk_client.h"

namespace dstream {
namespace client {

class ZKWatcher: public zk_client::Watcher {
public:
    void OnEvent(int type, int state, const char* path);
};

class ZK {
public:
    ZK() : m_connected(false),
        m_server_list(""),
        m_client(NULL) {}

    explicit ZK(std::string svr_list): m_connected(false),
        m_server_list(svr_list) {
        m_client = new (std::nothrow) zk_client::ZkClient(svr_list.c_str());
    }
    virtual ~ZK();

    virtual int32_t Connection();
    virtual int32_t Register(User* user);
    virtual int32_t UnRegister();
    virtual int32_t GetPM(uint32_t retry, uint32_t interval_secs, PM* pm);
    virtual int32_t GetApp(const std::string& app_name, uint32_t retry,
                           uint32_t interval_secs, Application* app);
    virtual int32_t GetAppID(const std::string& app_name, int64_t* app_id);

private:
    volatile bool         m_connected;
    std::string           m_server_list;
    std::string           m_register_path;
    zk_client::ZkClient*  m_client;
    ZKWatcher             m_watcher;
};

extern ZK* g_zk;

} // namespace client
} // namespace dstream
#endif // __DSTREAM_CC_CLIENT_ZK_H__ NOLINT
