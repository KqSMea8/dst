/**
* @file     client_config.cc
* @brief    dstream client config map.
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2011-04-06
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/


#include <map>
#include <string>

#include "client/action.h"
#include "client/client_config.h"
#include "client/dstream_client.h"
#include "common/config_const.h"
#include "common/utils.h"

namespace dstream {
namespace  client {

ClientConfigMap g_cfg_map;

ClientConfigMap::ClientConfigMap() {
#ifdef SetClientDefaultValue
#undef SetClientDefaultValue
#endif
#define SetClientDefaultValue(k, v) \
    m_cfg_map[k]        = v

#ifdef SetClientDefaultIntValue
#undef SetClientDefaultIntValue
#endif
#define SetClientDefaultIntValue(k, v) \
    snprintf(str_val, sizeof(str_val), "%d", v); \
    m_cfg_map[k]        = str_val

    char str_val[128];
    str_val[127] = '\0';

    /* zookeeper */
    SetClientDefaultIntValue(config_const::kZKRetryName, config_const::kDefaultZKRetry);
    SetClientDefaultIntValue(config_const::kZKTimeoutName, config_const::kDefaultZKTimeoutMs);
    SetClientDefaultValue(config_const::kZKRootPathName, config_const::kDefaultZKRootPath);
    SetClientDefaultValue(config_const::kPMRootPathName, config_const::kDefaultPMRootPath);
    SetClientDefaultValue(config_const::kPNRootPathName, config_const::kDefaultPNRootPath);
    SetClientDefaultValue(config_const::kAppRootPathName, config_const::kDefaultAppRootPath);
    SetClientDefaultValue(config_const::kConfigRootPathName, config_const::kDefaultConfigRootPath);
    SetClientDefaultValue(config_const::kClientRootName, config_const::kDefaultClientRootPath);
    /* hdfs */
    SetClientDefaultValue(config_const::kHDFSClientDirName, config_const::kHDFSClientDir);
    SetClientDefaultIntValue(config_const::kHDFSRetryName, config_const::kDefaultHDFSRetry);
    SetClientDefaultIntValue(config_const::kHDFSTimeoutName, config_const::kDefaultHDFSTimeoutMs);
    /* pm */
    SetClientDefaultIntValue(config_const::kClientToPMRetryName, config_const::kClientToPMRetry);
    SetClientDefaultIntValue(config_const::kClientToPMTimeoutName,
                             config_const::kClientToPMTimeoutMs);

#undef SetClientDefaultIntValue
#undef SetClientDefaultValue
}

error::Code ClientConfigMap::InitCfgMapFromFile(std::string file) {
#ifdef SetClientConfigValue
#undef SetClientConfigValue
#endif
#define SetClientConfigValue(k, v) \
    if (cfg.GetValue(k, &(v)) == error::OK) { \
        m_cfg_map[k] = v; \
    } \

    config::Config cfg;
    std::string value;

    error::Code ret = cfg.ReadConfig(file);
    if (ret < error::OK) {
        DSTREAM_ERROR("fail to read config file:[%s]", file.c_str());
        return ret;
    }

    /* zookeeper */
    /* compat old codes */
    SetClientConfigValue(config_const::kZKPathName, value);
    SetClientConfigValue(config_const::kZKRetryName, value);
    SetClientConfigValue(config_const::kZKTimeoutName, value);
    if (cfg.GetValue(config_const::kZKRootPathName, &value)
        != error::OK) { /* try to read env read config files */
        SetClientConfigValue(config_const::kPMRootPathName, value);
        SetClientConfigValue(config_const::kPNRootPathName, value);
        SetClientConfigValue(config_const::kAppRootPathName, value);
        SetClientConfigValue(config_const::kConfigRootPathName, value);
        SetClientConfigValue(config_const::kClientRootName, value);
    } else {/* try to combine env from root */
        m_cfg_map[config_const::kPMRootPathName] = value + "/PM";
        m_cfg_map[config_const::kPNRootPathName] = value + "/PN";
        m_cfg_map[config_const::kClientRootName] = value + "/Client";
        m_cfg_map[config_const::kAppRootPathName] = value + "/App";
        m_cfg_map[config_const::kConfigRootPathName] = value + "/Config";
    }
    /* hdfs */
    SetClientConfigValue(config_const::kHDFSClientDirName, value);
    SetClientConfigValue(config_const::kHDFSRetryName, value);
    SetClientConfigValue(config_const::kHDFSTimeoutName, value);
    /* localhost ip */
    SetClientConfigValue(config_const::kLocalHostIPName, value);

    std::string str_val;
    if (m_cfg_map[config_const::kLocalHostIPName].empty()) {
        if (error::OK != GetLocalhostIP(&str_val)) {
            DSTREAM_ERROR("fail to get local host ip");
            return error::GET_HOST_IP_FAIL;
        }
        m_cfg_map[config_const::kLocalHostIPName] = str_val;
    }
    /* gen hostname with "ip+timestamp" */
    time_t t = time(NULL);
    str_val  = m_cfg_map[config_const::kLocalHostIPName];
    str_val += "-";
    StringAppendNumber(&str_val, static_cast<ssize_t>(t));
    m_cfg_map[config_const::kClientHostNameName] = str_val;
    /* use info */
    SetClientConfigValue(config_const::kClientUserNameName, value);
    SetClientConfigValue(config_const::kClientPasswordName, value);
    g_user.set_username(m_cfg_map[config_const::kClientUserNameName]);
    g_user.set_password(m_cfg_map[config_const::kClientPasswordName]);
    g_user.set_hostip(m_cfg_map[config_const::kLocalHostIPName]);
    g_user.set_hostname(m_cfg_map[config_const::kClientHostNameName]);
#ifdef VERSION
    g_user.set_version(VERSION);
#else
    g_user, set_version(kUnknownClientVersion);
#endif
    /* pm */
    SetClientConfigValue(config_const::kClientToPMRetryName, value);
    SetClientConfigValue(config_const::kClientToPMTimeoutName, value);

    /* check those env should not empty */
    if (m_cfg_map[config_const::kZKPathName].empty()) {
        DSTREAM_ERROR("failt to get zookeeper address");
        return error::CONFIG_BAD_VALUE;
    }
    // no use
    /*if (m_cfg_map[config_const::kLocalHostIPName].empty()) {
      DSTREAM_ERROR("failt to get local address");
      return error::CONFIG_BAD_VALUE;
    }*/
    if (m_cfg_map[config_const::kClientUserNameName].empty() ||
        m_cfg_map[config_const::kClientPasswordName].empty()) {
        DSTREAM_WARN("fail to get user info: username:[%s], password:[%s]",
                     m_cfg_map[config_const::kClientUserNameName].c_str(),
                     m_cfg_map[config_const::kClientPasswordName].c_str());
        return error::CONFIG_BAD_VALUE;
    }

#undef SetPNConfigValue
    m_has_init = true;
    return error::OK;
}

void ClientConfigMap::Dump() {
    std::map<std::string, std::string>::const_iterator it;
    printf("[Config Map]\n");
    for (it = m_cfg_map.begin(); it != m_cfg_map.end(); it++) {
        printf("---%30s = %s\n", it->first.c_str(), it->second.c_str());
    }
}

} // namespace client
} // namespace dstream
