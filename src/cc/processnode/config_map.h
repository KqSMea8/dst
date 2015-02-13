/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_PROCESSNODE_CONFIG_MAP_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_CONFIG_MAP_H__ // NOLINT

#include <string>
#include "common/config.h"
#include "common/config_const.h"

namespace dstream {
namespace processnode {

/**
* @brief    dstream client config map
* @author   konghui, konghui@baidu.com
* @date     2012-02-06
* @notice   this is a read-only map, to use this map, you should do:
*           I  : init all environments with default values as much as possible;
*           II : read configs from config file and cover the default values;
*           III: then you should use this map read-only;
*/
class PNConfigMap : public config::ConfigMap {
public:
    PNConfigMap();
    virtual ~PNConfigMap() {}
    error::Code InitCfgMapFromFile(std::string file);
    void Dump();
};

extern PNConfigMap g_pn_cfg_map;

} // namespace processnode
} // namespace dstream
#endif // __DSTREAM_CC_PROCESSNODE_CONFIG_MAP_H__ NOLINT
