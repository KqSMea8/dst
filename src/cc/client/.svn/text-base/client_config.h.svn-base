/**
* @file     client_config.cc
* @brief    dstream client config map.
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2011-04-06
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <string>
#include "common/config_const.h"
#include "metamanager/zk_config.h"

#ifndef __DSTREAM_CC_CLIENT_CONFIG_H__ // NOLINT
#define __DSTREAM_CC_CLIENT_CONFIG_H__ // NOLINT
namespace dstream {
namespace client {

/**
* @brief    dstream client config map
* @author   konghui, konghui@baidu.com
* @date     2012-04-06
* @notice   this is a read-only map, to use this map, you should do:
*           I  : init all environments with default values as much as possible;
*           II : read configs from config file and cover the default values;
*           III: then you should use this map read-only;
*/
class ClientConfigMap : public config::ConfigMap {
public:
    ClientConfigMap();
    virtual ~ClientConfigMap() {}
    error::Code InitCfgMapFromFile(std::string file);
    void Dump();
};

extern ClientConfigMap g_cfg_map;

} // namespace client
} // namespace dstream

#endif //__DSTREAM_CC_CLIENT_CONFIG_H__ // NOLINT
