/**
* @file     dredis_sa.h
* @brief    dredis访问层封装
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_DREDIS_SA_H__ // NOLINT
#define __DSTREAM_STORAGE_DREDIS_SA_H__ // NOLINT

#include <rclient.h>
#include <string>
#include "storage/dredis.h"
#include "storage/dredis_hash.h"
#include "storage/dredis_list.h"
#include "storage/storage_accessor.h"
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

/**
* @brief    DRedis访问层
* @author   konghui, konghui@baidu.com
* @date     2012-12-05
* @notice   接口详细注释参见storage/storage_accessor.h
*/
class DRedisSA: public RedisConnection, public StoreAccessor {
public:
    DRedisSA(): RedisConnection(kDRedisRetry), StoreAccessor(DREDIS) {}
    ~DRedisSA() {}

    /**
    * @brief    初始化并连接dredis
    *
    * @param    sn_list[in] sentinel地址序列，通过','分割多个地址，
    *                       内部随机选择一个sentinel进行连接。
    *                       eg: 127.0.0.1:8712,192.168.0.2:8712,192.168.0.2:8713
    * @param    master_name[in]    Redis Master name.
    * @param    retry[in]   重试次数，初始化与查询均使用该参数，默认3次
    * @param    timeout_ms[in] 超时时间，ms, 初始化与查询均使用该参数，
    *                          默认20000ms,  0为不超时；
    * @param    token[in]   认证密码，默认无
    * @return   成功: = 0, storage::OK
    *           失败：< 0, storage::SYNC_CONN_FAIL
    * @notice   本接口不支持sharding;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-21
    */
    int Init(const std::string& sn_list,
             const std::string& master_name,
             int retry, int timeout_ms, const std::string& token);
    int CreateTable(const std::string& table_name,
                    StoreTableType table_type,
                    bool replace_old,
                    IStoreTablePtr* pt);
    int DeleteTable(const std::string& table_name);
    int GetTable(const std::string& table_name, IStoreTablePtr* pt);
    int TestTableExist(const std::string& table_name, bool* exists);
    int TestTableExist(const std::string& table_name, bool* exists,
                       StoreTableType* p_table_type);
};
} // namespace storage
} // namespace dstream // NOLINT
#endif //__DSTREAM_STORAGE_DREDIS_SA_H__ // NOLINT
