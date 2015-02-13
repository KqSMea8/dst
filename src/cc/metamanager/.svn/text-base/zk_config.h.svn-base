/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/13
* @filename:    zk_config.h
* @author:      zhanggengxin
* @brief:       zk config
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_ZK_CONFIG_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_ZK_CONFIG_H__ // NOLINT

#include <cstddef>
#include <string>

#include "common/config.h"

namespace dstream {
namespace zk_config {

/************************************************************************/
/* config including zookeeper                                           */
/************************************************************************/
class ZKConfig : public config::Config {
public:
    ZKConfig() : config_(NULL) {}
    explicit ZKConfig(const Config* config) : config_(config) {}
    ~ZKConfig() {}
    int Timeout();
    std::string PMPath();
    std::string BackupPMRoot();
    std::string PNRootPath();
    std::string PNGroupRootPath();
    std::string SubPointRootPath();
    std::string ScribeRootPath();
    std::string AppRootPath();
    std::string ClusterConfigPath();
    std::string ClientRootPath();
    std::string GetZKPath();
    std::string GetSysPath();
private:
    bool GetValueInfo(const std::string& key_item, std::string* value);
    const Config* config_;
};

} // namespace zk_config
} // namespace dstream

#endif // NOLINT
