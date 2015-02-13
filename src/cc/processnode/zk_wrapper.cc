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

#include "processnode/zk_wrapper.h"

#include <time.h>
#include "common/application_tools.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "common/rpc_const.h"
#include "common/utils.h"
#include "common/zk_client.h"

#include "metamanager/meta_manager.h"
#include "metamanager/zk_config.h"
#include "metamanager/zk_meta_manager.h"
#include "processnode/config_map.h"

namespace dstream {
namespace processnode {

ZK* g_zk = NULL;

error::Code ZK::Register(const PN& pn) {
    m_cur_pn_meta = pn;
    return meta_manager::MetaManager::GetMetaManager()->AddPN(pn);
}

error::Code ZK::UnRegister() {
    return meta_manager::MetaManager::GetMetaManager()->DeletePN(m_cur_pn_meta);
}

error::Code ZK::GetPM(PM* pm) {
    return meta_manager::MetaManager::GetMetaManager()->GetPM(pm);
}

error::Code ZK::GetPN(const PNID& pnid, PN* pn) {
    return meta_manager::MetaManager::GetMetaManager()->GetPN(pnid, pn);
}

error::Code ZK::GetApp(const AppID& app_id, Application* app) {
    // cache application
    return meta_manager::MetaManager::GetMetaManager()->GetApplication(app_id, app);
}

error::Code ZK::GetAppFlowControl(const AppID& app_id, const std::string& processor, FlowControl* flow_control) {
    return meta_manager::MetaManager::GetMetaManager()->GetAppFlowControl(app_id, processor, flow_control);
}

error::Code ZK::GetPE(const AppID& app_id, const PEID& peid, ProcessorElement* pe) {
    return meta_manager::MetaManager::GetMetaManager()->GetProcessorElement(app_id, peid, pe);
}

} // namespace processnode
} // namespace dstream
