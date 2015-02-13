/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/13
* @filename:    zk_config.cc
* @author:      zhanggengxin
* @brief:       zk config
*/

#include "metamanager/zk_config.h"
#include "common/config_const.h"

namespace dstream {
namespace zk_config {
using namespace config_const;

int ZKConfig::Timeout() {
    std::string timeout;
    return GetValueInfo(kZKTimeoutName, &timeout) ?
           atoi(timeout.c_str()) :
           kDefaultZKTimeoutMs;
}

std::string ZKConfig::PMPath() {
    std::string pm_path;
    return GetValueInfo(kPMRootPathName, &pm_path) ?
           pm_path : GetSysPath() + "/PM";
}

std::string ZKConfig::BackupPMRoot() {
    std::string backup_pm_path;
    if (!GetValueInfo(kBackupPMRootName, &backup_pm_path)) {
        backup_pm_path = GetSysPath();
    }
    return GetSysPath() + "/BackupPM";
}

std::string ZKConfig::PNRootPath() {
    std::string pn_root_path;
    return GetValueInfo(kPNRootPathName, &pn_root_path) ?
           pn_root_path : GetSysPath() + "/PN";
}

std::string ZKConfig::PNGroupRootPath() {
    std::string pn_group_root_path;
    return GetValueInfo(kPNGroupRootPathName, &pn_group_root_path) ?
           pn_group_root_path : GetSysPath() + "/PNGroup";
}

std::string ZKConfig::SubPointRootPath() {
    std::string sub_point_root_path;
    return GetValueInfo(kSubPointRootPathName, &sub_point_root_path) ?
           sub_point_root_path : GetSysPath() + "/SubPoint";
}

std::string ZKConfig::ScribeRootPath() {
    std::string scribe_root_path;
    return GetValueInfo(kScribeRootPathName, &scribe_root_path) ?
           scribe_root_path : GetSysPath() + "/Scribe";
}

std::string ZKConfig::AppRootPath() {
    std::string app_root_path;
    return GetValueInfo(kAppRootPathName, &app_root_path) ?
           app_root_path : GetSysPath() + "/App";
}

std::string ZKConfig::ClusterConfigPath() {
    std::string cluster_path;
    return GetValueInfo(kConfigRootPathName, &cluster_path) ?
           cluster_path : GetSysPath() + "/Config";
}

std::string ZKConfig::ClientRootPath() {
    std::string client_root;
    return GetValueInfo(kClientRootName, &client_root) ?
           client_root : GetSysPath() + "/Client";
}

std::string ZKConfig::GetZKPath() {
    std::string zk_addr;
    return GetValueInfo(kZKPathName, &zk_addr) ? zk_addr : "";
}

std::string ZKConfig::GetSysPath() {
    std::string sys_path;
    return GetValueInfo(kZKRootPathName, &sys_path) ?
           sys_path : kDefaultSysPath;
}

bool ZKConfig::GetValueInfo(const std::string& key_item, std::string* value) {
    Config* config = const_cast<Config*>(config_);
    if (NULL != config_) {
        return config->GetValue(key_item, value) == error::OK;
    } else {
        return GetValue(key_item, value) == error::OK;
    }
    return ((NULL != config_ && config->GetValue(key_item, value) == error::OK) ||
            GetValue(key_item, value) == error::OK);
}

} // namespace zk_config
} // namespace dstream
