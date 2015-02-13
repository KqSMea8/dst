/*
 * storage_config.cpp
 *
 *  Created on: 2012-12-5
 *      Author: lanbijia
 */
#include "storage_config.h"


using namespace dstream;

const char* StorageConfig::kStorageConfigTag = "Storage";
const char* StorageConfig::kServiceLocList = "ServiceLocList";
const char* StorageConfig::kServiceName = "ServiceName";
const char* StorageConfig::kServiceToken = "ServiceToken";
const char* StorageConfig::kRetry = "Retry";
const char* StorageConfig::kTimeout = "Timeout";

const std::string StorageConfig::kDefaultServiceLocList = "127.0.0.1:5712";
const std::string StorageConfig::kDefaultServiceName = "redis_master";
const std::string StorageConfig::kDefaultServiceToken = "default_token";
const int32_t StorageConfig::kDefaultRetry = 0;
const int32_t StorageConfig::kDefaultTimeout = 10;

bool StorageConfig::CheckAndReadConfig(const dstream::config::Config& conf,
                                       const std::string& storage_name) {
    uint32_t fail_cnt = 0;
    uint32_t expect_cnt = 0;
    // 找到对应storage_name的配置
    std::string read_str;
    std::string prefix_str = kStorageConfigTag;
    // 读取外部存储定位服务器列表
    read_str = prefix_str + "." + storage_name + "." + kServiceLocList;
    expect_cnt++;
    if (dstream::error::OK > conf.GetValue(read_str, service_loc_list)) {
        service_loc_list = kDefaultServiceLocList;
        DSTREAM_WARN("[%s] read '%s' fail, set to kDefaultServiceLocList = %s.", __FUNCTION__,
                     kServiceLocList, kDefaultServiceLocList.c_str());
        fail_cnt++;
    } else {
        DSTREAM_DEBUG("[%s] read '%s' = %s.", __FUNCTION__, kServiceLocList, service_loc_list.c_str());
    }

    // 读取外部存储服务名称
    read_str = prefix_str + "." + storage_name + "." + kServiceName;
    expect_cnt++;
    if (dstream::error::OK > conf.GetValue(read_str, service_name)) {
        service_name = kDefaultServiceName;
        DSTREAM_WARN("[%s] read '%s' fail, set to kServiceName = %s.",
                     __FUNCTION__, kServiceName, kDefaultServiceName.c_str());
        fail_cnt++;
    } else {
        DSTREAM_DEBUG("[%s] read '%s' = %s.", __FUNCTION__, kServiceName, service_name.c_str());
    }

    // 读取外部存储服务Token
    read_str = prefix_str + "." + storage_name + "." + kServiceToken;
    expect_cnt++;
    if (dstream::error::OK > conf.GetValue(read_str, service_token)) {
        service_token = kDefaultServiceToken;
        DSTREAM_WARN("[%s] read '%s' fail, set to kDefaultServiceToken = %s.",
                     __FUNCTION__, kServiceToken, kDefaultServiceToken.c_str());
        fail_cnt++;
    } else {
        DSTREAM_DEBUG("[%s] read '%s' = %s.", __FUNCTION__, kServiceToken, service_token.c_str());
    }

    // 重试次数
    read_str = prefix_str + "." + storage_name + "." + kRetry;
    if (dstream::error::OK > conf.GetIntValue(read_str, retry)) {
        retry = kDefaultRetry;
        DSTREAM_WARN("[%s] read '%s' fail, set to kDefaultRetry = %u.",
                     __FUNCTION__, kRetry, kDefaultRetry);
    }

    // 超时设置
    read_str = prefix_str + "." + storage_name + "." + kTimeout;
    if (dstream::error::OK > conf.GetIntValue(read_str, timeout)) {
        timeout = kDefaultTimeout;
        DSTREAM_WARN("[%s] read '%s' fail, set to kDefaultTimeout = %u.",
                     __FUNCTION__, kTimeout, kDefaultTimeout);
    }
    return (fail_cnt < expect_cnt);
}

std::string StorageConfig::GetMyStorageService(uint64_t hash_code) {
    std::string select_service_name = "";
    // 从外部存储服务名称列表中分析出list
    std::vector<std::string> service_name_list;
    int32_t split_num = SplitStringEx(service_name, ',', &service_name_list);
    if (split_num) {
        select_service_name = service_name_list[hash_code % service_name_list.size()];
    }
    return select_service_name;
}

