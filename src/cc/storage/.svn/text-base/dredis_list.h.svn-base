/**
* @file     dredis_list.h
* @brief    dredis list表封装
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_DREDIS_LIST_H__ //NOLINT
#define __DSTREAM_STORAGE_DREDIS_LIST_H__ //NOLINT

#include <rclient.h>
#include <string>
#include <vector>
#include "storage/dredis.h"
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

/**
* @brief    基于dredis的ListTable封装
* @author   konghui, konghui@baidu.com
* @date     2012-10-25
* @notice   接口详细注释参见storage/storage_table.h
*/
class RedisListTable : public RedisConnection, public IListTable {
public:
    explicit RedisListTable(const std::string& n): RedisConnection(kDRedisRetry),
        IListTable(n, REDIS_LIST_TABLE) {}
    ~RedisListTable() {}
    int Count(uint64_t* n);
    int GetAll(ListTraverseCallBack cb, void* ctx);
    int GetFront(std::string* s);
    int GetBack(std::string* s);
    int PushFront(const std::string& value);
    int PushBack(const std::string& value);
    int PushFront(const std::vector<std::string>& values,
                    std::vector<int>* ret_vals);
    int PushBack(const std::vector<std::string>& values,
                    std::vector<int>* ret_vals);
    int PopFront(std::string* value);
    int PopBack(std::string* value);
    int PopFront(int n,
                    std::vector<std::string>* values,
                    std::vector<int>* ret_vals);
    int PopBack(int n,
                    std::vector<std::string>* values,
                    std::vector<int>* ret_vals);
    int Clear();

private:
    int Get(std::string* s, bool get_front);
    int Push(const std::string& value, bool push_front);
    int Push(const std::vector<std::string>& values,
             std::vector<int>* ret_vals, bool push_front);
    int Pop(std::string* value, bool pop_front);
    int Pop(int n,
            std::vector<std::string>* values,
            std::vector<int>* ret_vals, bool pop_front);
};

typedef boost::shared_ptr<RedisListTable> RedisListTablePtr;

} // namespace storage
} // namespace dstream
#endif //__DSTREAM_STORAGE_DREDIS_LIST_H__ // NOLINT
