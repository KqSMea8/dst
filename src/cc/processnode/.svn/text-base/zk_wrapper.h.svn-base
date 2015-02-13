/** -*- c++ -*-
 * Copyright (C) 2011 Realtime Team (Baidu, Inc)
 *
 * This file is part of DStream.
 *
 * DStream is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 *
 */

#ifndef __DSTREAM_CC_PROCESSNODE_ZK_WRAPPER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_ZK_WRAPPER_H__ // NOLINT

#include <string>

#include "common/application_tools.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm.pb.h"
#include "common/proto/pn.pb.h"
#include "common/rwlock.h"
#include "common/utils.h"
#include "common/zk_client.h"

#include "metamanager/meta_manager.h"

namespace dstream {
namespace processnode {

#ifndef RESERVE_TAG
#define RESERVE_TAG "_ALL_TAG"
#endif

class ZK {
public:
    explicit ZK(Config* config): m_config(config) {
        meta_manager::MetaManager::InitMetaManagerContext(m_config);
    }
    virtual ~ZK() {
        meta_manager::MetaManager::GetMetaManager()->DestoryMetaManager();
    }

    meta_manager::MetaManager* GetMetaManager() {
        return meta_manager::MetaManager::GetMetaManager();
    }

    error::Code Register(const PN& pn);
    error::Code UnRegister();

    error::Code WatchPE(const PEID& peid, zk_client::Watcher* watcher);
    error::Code UnwatchPE(const PEID& peid);

    error::Code GetPM(PM* pm);
    error::Code GetPN(const PNID& pnid, PN* pn);
    error::Code GetApp(const AppID& app_id, Application* app);
    error::Code GetAppFlowControl(const AppID& app_id,
                                  const std::string& processor, FlowControl* ctl);
    error::Code GetPE(const AppID& app_id,
                      const PEID& peid,
                      ProcessorElement* pe);

private:
    Config*     m_config;
    PN          m_cur_pn_meta;
};

extern ZK* g_zk;

} // namespace processnode
} // namespace dstream
#endif // NOLINT
