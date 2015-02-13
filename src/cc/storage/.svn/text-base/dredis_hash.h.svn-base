/**
* @file     dredis_hash.h
* @brief    dredis hash表封装
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_DREDIS_HASH_H__ //NOLINT
#define __DSTREAM_STORAGE_DREDIS_HASH_H__ //NOLINT

#include <rclient.h>
#include <map>
#include <string>
#include <vector>
#include "storage/dredis_sa.h"
#include "storage/storage_accessor.h"
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

/**
* @brief    基于dredis的HashTable封装
* @author   konghui, konghui@baidu.com
* @notice   接口详细注释参见storage/storage_table.h
* @date     2012-10-25
*/
class RedisHashTable : public RedisConnection, public IHashTable {
public:
    explicit RedisHashTable(const std::string& n): RedisConnection(kDRedisRetry),
        IHashTable(n, REDIS_HASH_TABLE) {}
    ~RedisHashTable() {}
    int Count(uint64_t* n);
    int GetAll(HashTraverseCallBack cb, void* ctx);
    int Find(const std::string& key, std::string* value);
    int Insert(const std::string& key, const std::string& value);
    int Insert(const std::string& key, const std::string& value,
               bool update_if_exists);
    int Insert(const std::vector<std::string>& keys,
               const std::vector<std::string>& values,
               std::vector<int>* ret_vals);
    int Insert(const std::vector<std::string>& keys,
               const std::vector<std::string>& values,
               std::vector<int>* ret_vals,
               bool update_if_exists);
    int Erase(const std::string& key);
    int Erase(const std::vector<std::string>& keys,
              std::vector<int>* ret_vals);
    int Clear();
private:
};
typedef boost::shared_ptr<RedisHashTable> RedisHashTablePtr;

} // namespace storage
} // namespace dstream
#endif // __DSTREAM_STORAGE_DREDIS_HASH_H__ //NOLINT
