/**
* @file     dredis.h
* @brief    dredis访问层封装
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_DREDIS_H__ //NOLINT
#define __DSTREAM_STORAGE_DREDIS_H__ //NOLINT

#include <rclient.h>
#include <string>
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

const int kDRedisRetry = 3; /*retry*/

/**
* @brief    DRedis连接类封装
* @author   konghui, konghui@baidu.com
* @date     2012-12-05
*/
class RedisConnection {
public:
    RedisConnection(): m_retry(kDRedisRetry) {}
    explicit RedisConnection(int r): m_retry(r) {}
    ~RedisConnection() {}
    int CopyRClient(const dredis::rclient::RClient& rc);
protected:
    int m_retry;
    dredis::rclient::RClient m_redis_client;
};

void RedisStringToType(const std::string& s, StoreTableType* t);
void RedisTypeToString(const StoreTableType& t, std::string* s);
bool GetAndCheckQueued(dredis::rclient::RClient& rc, const std::string& name);
int CreateRedisTable(const std::string& table_name,
                     StoreTableType table_type,
                     bool replace_old,
                     dredis::rclient::RClient* rc);
int RedisTableFactory(const dredis::rclient::RClient& rc,
                      const std::string& table_name,
                      StoreTableType table_type,
                      IStoreTablePtr* pt);

#define _CHECK_DREDIS_OK_OR_RETURN_() \
    do { \
        if (!m_is_initialized || !m_is_valid) { \
            return storage::NOT_INIT_YET; \
        } \
    } while (0);


// #ifndef _REDIS_DEBUG
// #define _REDIS_DEBUG 1
// #endif

#ifndef _REDIS_DEBUG
#define DUMP_REDIS_REPLY(r)
#else
#define DUMP_REDIS_REPLY(r) \
    do { \
        std::string _s; \
        dredis::rclient::DumpRedisReplyObj(r, &_s); \
        printf("Get Reply:[%s]\n", _s.c_str()); \
    } while (0);
#endif

} // namespace storage
} // namespace dstream
#endif // __DSTREAM_STORAGE_DREDIS_H__ //NOLINT
