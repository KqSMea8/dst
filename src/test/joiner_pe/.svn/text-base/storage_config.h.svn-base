/*
 * storage_config.h
 *
 *  Created on: 2012-12-5
 *      Author: lanbijia
 */

#ifndef CTR_RTS_COMMON_STORAGE_CONFIG_H
#define CTR_RTS_COMMON_STORAGE_CONFIG_H

// pesdk
#include "common/logger.h"
#include "common/config.h"
#include "common_utils.h"


class StorageConfig {
public:
    static const char* StorageConfig::kStorageConfigTag;
    static const char* StorageConfig::kServiceLocList;
    static const char* StorageConfig::kServiceName;
    static const char* StorageConfig::kServiceToken;
    static const char* StorageConfig::kRetry;
    static const char* StorageConfig::kTimeout;

    static const std::string kDefaultServiceLocList;
    static const std::string kDefaultServiceName;
    static const std::string kDefaultServiceToken;
    static const int32_t kDefaultRetry;
    static const int32_t kDefaultTimeout;

    bool CheckAndReadConfig(const dstream::config::Config& conf,
                            const std::string& storage_name);
    std::string GetMyStorageService(uint64_t hash_code);
public:
    std::string service_loc_list;
    std::string service_name;
    std::string service_token;
    int32_t timeout;
    int32_t retry;
};


#endif /* CTR_RTS_COMMON_STORAGE_CONFIG_H */
