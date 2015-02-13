/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <map>

#include "common/config.h"
#include "common/rpc_const.h"
#include "processelement/context.h"
#include "processnode/config_map.h"

namespace dstream {
namespace processnode {

PNConfigMap g_pn_cfg_map;

PNConfigMap::PNConfigMap() {
#ifdef SetPNDefaultValue
#undef SetPNDefaultValue
#endif
#define SetPNDefaultValue(k, v) \
    m_cfg_map[k]        = v

#ifdef SetPNDefaultIntValue
#undef SetPNDefaultIntValue
#endif
#define SetPNDefaultIntValue(k, v) \
    do { \
        if (8 == sizeof(v)) { \
            snprintf(str_val, sizeof(str_val), "%lu", static_cast<uint64_t>(v)); \
        } else { \
            snprintf(str_val, sizeof(str_val), "%d", static_cast<int>(v)); \
        } \
        m_cfg_map[k] = str_val; \
    } while (0);

    char str_val[128];
    str_val[127] = '\0';

    SetPNDefaultIntValue(config_const::kZKRetryName, config_const::kDefaultZKRetry);
    SetPNDefaultIntValue(config_const::kZKTimeoutName, config_const::kDefaultZKTimeoutMs);
    SetPNDefaultValue(config_const::kPNWorkDirName, config_const::kPNWorkDir);
    SetPNDefaultValue(config_const::kPMRootPathName, config_const::kDefaultPMRootPath);
    SetPNDefaultValue(config_const::kPNRootPathName, config_const::kDefaultPNRootPath);
    SetPNDefaultValue(config_const::kAppRootPathName, config_const::kDefaultAppRootPath);
    SetPNDefaultValue(config_const::kConfigRootPathName, config_const::kDefaultConfigRootPath);
    SetPNDefaultValue(config_const::kClientRootName, config_const::kDefaultClientRootPath);
    SetPNDefaultIntValue(config_const::kPNToPMRetryName, config_const::kPNToPMRetry);
    SetPNDefaultIntValue(config_const::kPNSmallTranBufNum, config_const::kDefaultPNSmallTranBufNum);
    SetPNDefaultIntValue(config_const::kPNSmallTranBufSize, config_const::kDefaultPNSmallTranBufSize);
    SetPNDefaultIntValue(config_const::kPNBigTranBufNum, config_const::kDefaultPNBigTranBufNum);
    SetPNDefaultIntValue(config_const::kPNWriteWaterMark, config_const::kDefaultPNWriteWaterMark);
    SetPNDefaultIntValue(config_const::kPNToPMTimeoutName, config_const::kPNToPMTimeoutMs);
    SetPNDefaultValue(config_const::kHDFSClientDirName, config_const::kHDFSClientDir);
    SetPNDefaultIntValue(config_const::kHDFSRetryName, config_const::kDefaultHDFSRetry);
    SetPNDefaultIntValue(config_const::kHDFSTimeoutName, config_const::kDefaultHDFSTimeoutMs);
    SetPNDefaultIntValue(config_const::kPNPNListenThreadNumName, config_const::kPNListenThreadNum);
    SetPNDefaultIntValue(config_const::kPNPEListenThreadNumName, config_const::kPNListenThreadNum);
    SetPNDefaultIntValue(config_const::kPNPMListenThreadNumName, config_const::kPNListenThreadNum);
    SetPNDefaultIntValue(config_const::kPNDebugListenThreadNumName,
            config_const::kPNListenThreadNum);
    SetPNDefaultIntValue(config_const::kPNListenPortName, config_const::kPNListenPort);
    SetPNDefaultIntValue(config_const::kPNDebugListenPortName, config_const::kPNDebugListenPort);
    SetPNDefaultIntValue(config_const::kPNPNListenPortName, config_const::kPNPNListenPort);
    SetPNDefaultIntValue(config_const::kPNPEListenPortName, config_const::kPNPEListenPort);
    SetPNDefaultIntValue(config_const::kImporterPortName, config_const::kImporterPort);
    SetPNDefaultValue(config_const::kImporterTypeName, config_const::kImporterType);
    SetPNDefaultIntValue(config_const::kPNSubChkIntervalSecName,
            config_const::kPNSubChkIntervalSec);
    SetPNDefaultIntValue(config_const::kPNSubQueueSizeName, config_const::kPNSubQueueSize);
    SetPNDefaultIntValue(config_const::kPNSendQueueSizeName, config_const::kPNSendQueueSize);
    SetPNDefaultIntValue(config_const::kPNRecvQueueSizeName, config_const::kPNRecvQueueSize);
    SetPNDefaultIntValue(config_const::kPNTuplesBatchCountName, config_const::kPNTuplesBatchCount);
    SetPNDefaultValue(config_const::kUserLogLocalStoreName,
            config_const::kDefaultUserLogLocalStore);
    SetPNDefaultValue(config_const::kUserLogHdfsStoreName, config_const::kDefaultUserLogHdfsStore);
    SetPNDefaultValue(config_const::kUserLogConsoleShowName,
            config_const::kDefaultUserLogConsoleShow);
    SetPNDefaultValue(config_const::kUserLogTimeFormatName,
            config_const::kDefaultUserLogTimeFormat);

    SetPNDefaultValue(config_const::kUseCglimitName, config_const::kDefaultUseCglimit);
    SetPNDefaultValue(config_const::kPNSuspendWhenQueueFullName ,
                      config_const::kPNSuspendWhenQueueFull);

#undef SetPNDefaultIntValue
#undef SetPNDefaultValue
}

error::Code PNConfigMap::InitCfgMapFromFile(std::string file) {
#ifdef SetPNConfigValue
#undef SetPNConfigValue
#endif
#define SetPNConfigValue(k, v) \
    if (cfg.GetValue(k, &(v)) == error::OK) { \
        m_cfg_map[k] = v; \
    }

    config::Config cfg;
    std::string value;

    error::Code ret = cfg.ReadConfig(file);
    if (ret < error::OK) {
        DSTREAM_ERROR("fail to read config file:[%s]", file.c_str());
        return ret;
    }

    SetPNConfigValue(config_const::kZKPathName, value);
    SetPNConfigValue(config_const::kZKRetryName, value);
    SetPNConfigValue(config_const::kZKRootPathName, value);
    SetPNConfigValue(config_const::kZKTimeoutName, value);
    SetPNConfigValue(config_const::kHDFSClientDirName, value);
    SetPNConfigValue(config_const::kPNWorkDirName, value);
    SetPNConfigValue(config_const::kPMRootPathName, value);
    SetPNConfigValue(config_const::kPNRootPathName, value);
    SetPNConfigValue(config_const::kAppRootPathName, value);
    SetPNConfigValue(config_const::kConfigRootPathName, value);
    SetPNConfigValue(config_const::kClientRootName, value);
    SetPNConfigValue(config_const::kPNToPMTimeoutName, value);
    SetPNConfigValue(config_const::kHDFSRetryName, value);
    SetPNConfigValue(config_const::kPNPNListenThreadNumName, value);
    SetPNConfigValue(config_const::kPNPMListenThreadNumName, value);
    SetPNConfigValue(config_const::kPNPEListenThreadNumName, value);
    SetPNConfigValue(config_const::kPNDebugListenThreadNumName, value);
    SetPNConfigValue(config_const::kPNListenPortName, value);
    SetPNConfigValue(config_const::kPNDebugListenPortName, value);
    SetPNConfigValue(config_const::kPNPNListenPortName, value);
    SetPNConfigValue(config_const::kPNPEListenPortName, value);
    SetPNConfigValue(config_const::kImporterTypeName, value);
    SetPNConfigValue(config_const::kPNSubChkIntervalSecName, value);
    SetPNConfigValue(config_const::kPNSendQueueSizeName, value);
    SetPNConfigValue(config_const::kPNRecvQueueSizeName, value);
    SetPNConfigValue(config_const::kPNSendWaterMarkName, value);
    SetPNConfigValue(config_const::kPNRecvWaterMarkName, value);
    SetPNConfigValue(config_const::kPNTuplesBatchCountName, value);
    SetPNConfigValue(config_const::kLocalHostIPName, value);
    SetPNConfigValue(config_const::kUserLogTimeFormatName, value);
    SetPNConfigValue(config_const::kUserLogLocalStoreName, value);
    SetPNConfigValue(config_const::kUserLogHdfsStoreName, value);
    SetPNConfigValue(config_const::kUserLogConsoleShowName, value);
    SetPNConfigValue(config_const::kUseCglimitName, value);
    SetPNConfigValue(config_const::kPNTotalMemory, value);
    SetPNConfigValue(config_const::kPNTotalCPU, value);
    SetPNConfigValue(config_const::kPNBufferMemory, value);
    SetPNConfigValue(config_const::kPNSuspendWhenQueueFullName, value);
    SetPNConfigValue(config_const::kPNSmallTranBufSize, value);
    SetPNConfigValue(config_const::kPNSmallTranBufNum, value);
    SetPNConfigValue(config_const::kPNBigTranBufNum, value);
    SetPNConfigValue(config_const::kPNWriteWaterMark, value);

    std::string str_val;
    if (m_cfg_map[config_const::kLocalHostIPName].empty()) {
        if (error::OK != GetLocalhostIP(&str_val)) {
            DSTREAM_ERROR("fail to get local host ip");
            return error::GET_HOST_IP_FAIL;
        }
        m_cfg_map[config_const::kLocalHostIPName] = str_val;
    }

    str_val = rpc_const::GetUri(m_cfg_map[config_const::kLocalHostIPName],
            atoi(m_cfg_map[config_const::kPNDebugListenPortName].c_str()));
    m_cfg_map[config_const::kPNDebugListenUriName] = str_val;

    if (m_cfg_map[config_const::kZKPathName].empty()) {
        DSTREAM_ERROR("failt to get zookeeper address");
        return error::CONFIG_BAD_VALUE;
    }
    if (m_cfg_map[config_const::kLocalHostIPName].empty()) {
        DSTREAM_ERROR("failt to get local address");
        return error::CONFIG_BAD_VALUE;
    }

#undef SetPNConfigValue
    m_has_init = true;
    return error::OK;
}

void PNConfigMap::Dump() {
    std::map<std::string, std::string>::const_iterator it;
    printf("[Config Map]\n");
    for (it = m_cfg_map.begin(); it != m_cfg_map.end(); it++) {
        printf("---%30s = %s\n", it->first.c_str(), it->second.c_str());
    }
}

} // namespace processnode
} // namespace dstream
