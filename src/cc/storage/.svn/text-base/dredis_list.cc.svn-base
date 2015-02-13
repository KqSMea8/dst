/**
* @file     dredis_list.h
* @brief    dredis list±í·â×°
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#include "storage/dredis_list.h"
#include "common/logger.h"

using namespace dredis;

namespace dstream {
namespace storage {

//  exception of std::string for maximum number of characters
//  exception of std::vector for resize

//-----------------------------------------------------------------------------
// RedisListTable
int RedisListTable::Count(uint64_t* n) {
    if (NULL == n) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "llen %s", name_.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to get num from list table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        m_redis_client.FreeReplyObj(r);
        *n = r->integer;
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::GetAll(ListTraverseCallBack cb, void* ctx) {
    if (NULL == cb) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "lrange %s 0 -1", name_.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_ARRAY)) {
            DSTREAM_WARN("fail to get all from list table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        for (size_t j = 0; j < r->elements; j++) {
            if (NULL == r->element[j]) {
                cb(ctx, storage::GET_NULL_REPLY, std::string());
            } else if (r->element[j]->type != REDIS_REPLY_STRING) {
                cb(ctx, storage::GET_ERR_REPLY, std::string(r->element[j]->str));
            } else {
                cb(ctx, storage::OK, std::string(r->element[j]->str,
                                                 r->element[j]->len));
            }
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::GetFront(std::string* s) {
    return Get(s, true);
}

int RedisListTable::GetBack(std::string* s) {
    return Get(s, false);
}
int RedisListTable::Get(std::string* s, bool get_front) {
    if (NULL == s) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    const int idx = get_front ? 0 : -1;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "lindex %s %d", name_.c_str(), idx);
        if ((NULL == r) ||
            ((r->type != REDIS_REPLY_STRING) && (r->type != REDIS_REPLY_NIL))) {
            DSTREAM_WARN("fail to get data from list table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        if (r->type == REDIS_REPLY_STRING) {
            s->assign(r->str, r->len);
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::PushFront(const std::string& v) {
    return Push (v, true);
}

int RedisListTable::PushBack(const std::string& v) {
    return Push (v, false);
}

int RedisListTable::Push(const std::string& v, bool push_front) {
    rclient::RedisReply* r = NULL;
    const char* cmd = push_front ? "lpush" : "rpush";
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "%s %s %b", cmd, name_.c_str(), v.data(), v.size());
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to %s data into list table[%s]:[%s]",
                         cmd, name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::PushFront(const std::vector<std::string>& values,
                              std::vector<int>* ret_vals) {
    return Push (values, ret_vals, true);
}

int RedisListTable::PushBack(const std::vector<std::string>& values,
                                std::vector<int>* ret_vals) {
    return Push (values, ret_vals, false);
}

int RedisListTable::Push(const std::vector<std::string>& values,
                            std::vector<int>* ret_vals,
                            bool push_front) {
    if (NULL == ret_vals) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    const size_t n = values.size();
    const char* cmd = push_front ? "lpush" : "rpush";
    for (size_t i = 0; i < static_cast<size_t>(m_retry); i++) {
        ret_vals->resize(n);
        size_t j;
        for (j = 0; j < n; j++) {
            if (m_redis_client.AppendCmd(NULL, "%s %s %b", cmd, name_.c_str(),
                                            values[j].data(), values[j].size())
                < rclient::OK) {
                DSTREAM_WARN("fail to %s data into list table[%s]:[%s]",
                             cmd, name_.c_str(), m_redis_client.ErrStr());
                break;
            }
        }
        if (j < n) {
            continue;
        }
        for (j = 0; j < n; j++) {
            if (m_redis_client.GetReply(&r) < rclient::OK) {
                DSTREAM_WARN("fail to get reply from list table[%s]:[%s]",
                             name_.c_str(), m_redis_client.ErrStr());
                break;
            }
            if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
                DSTREAM_WARN("fail to %s data into list table[%s]:[%s]",
                             cmd, name_.c_str(), NULL == r ? NULL : r->str);
                m_redis_client.FreeReplyObj(r);
                ret_vals->at(j) = storage::EXEC_CMD_ERR;
                continue;
            }
            m_redis_client.FreeReplyObj(r);
            ret_vals->at(j) = storage::OK;
        }
        if (j >= n) {
            return storage::OK;
        }
        ret_vals->clear();
    } // end of 'retry for'
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::PopFront(std::string* v) {
    return Pop(v, true);
}
int RedisListTable::PopBack(std::string* v) {
    return Pop(v, false);
}
int RedisListTable::Pop(std::string* v, bool pop_front) {
    if (NULL == v) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    const char* cmd = pop_front ? "lpop" : "rpop";
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "%s %s", cmd, name_.c_str());
        if ((NULL == r) ||
            ((r->type != REDIS_REPLY_STRING) && (r->type != REDIS_REPLY_NIL))) {
            DSTREAM_WARN("fail to %s from list table[%s]:[%s]",
                         cmd, name_.c_str(), NULL == r ? NULL : r->str);
            DUMP_REDIS_REPLY(r);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        if (r->type == REDIS_REPLY_STRING) {
            v->assign(r->str, r->len);
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::PopFront(int n,
                             std::vector<std::string>* values,
                             std::vector<int>* ret_vals) {
    return Pop (n, values, ret_vals, true);
}

int RedisListTable::PopBack(int n,
                            std::vector<std::string>* values,
                            std::vector<int>* ret_vals) {
    return Pop (n, values, ret_vals, false);
}

int RedisListTable::Pop(int n,
                        std::vector<std::string>* values,
                        std::vector<int>* ret_vals,
                        bool pop_front) {
    if (NULL == values) {
        return storage::BAD_ARGS;
    }
    if (NULL == ret_vals) {
        return storage::BAD_ARGS;
    }
    rclient::RedisReply* r = NULL;
    const char* cmd = pop_front ? "lpop" : "rpop";
    values->resize(n);
    for (size_t i = 0; i < static_cast<size_t>(m_retry); i++) {
        ret_vals->resize(n);
        int j;
        for (j = 0; j < n; j++) {
            if (m_redis_client.AppendCmd(NULL, "%s %s", cmd, name_.c_str())
                < rclient::OK) {
                DSTREAM_WARN("fail to %s data from list table[%s]:[%s]",
                             cmd, name_.c_str(), m_redis_client.ErrStr());
                break;
            }
        }
        if (j < n) {
            continue;
        }
        for (j = 0; j < n; j++) {
            if (m_redis_client.GetReply(&r) < rclient::OK) {
                DSTREAM_WARN("fail to get reply from list table[%s]:[%s]",
                             name_.c_str(), m_redis_client.ErrStr());
                break;
            }
            if ((NULL == r) || (r->type != REDIS_REPLY_STRING)) {
                DSTREAM_WARN("fail to %s data into list table[%s]:[%s]",
                             cmd, name_.c_str(), NULL == r ? NULL : r->str);
                m_redis_client.FreeReplyObj(r);
                ret_vals->at(j) = storage::EXEC_CMD_ERR;
                continue;
            }
            values->at(j).assign(r->str, r->len);
            ret_vals->at(j) = storage::OK;
            m_redis_client.FreeReplyObj(r);
        }
        if (j >= n) {
            return storage::OK;
        }
        values->clear();
        ret_vals->clear();
    } // end of 'retry for'
    return storage::EXEC_CMD_ERR;
}

int RedisListTable::Clear() {
    if (CreateRedisTable(name_, type_, true, &m_redis_client) < storage::OK) {
        DSTREAM_WARN("fail to clear list table[%s]@dredis:[%s]",
                     name_.c_str(), m_redis_client.ErrStr());
        return storage::EXEC_CMD_ERR;
    }
    return storage::OK;
}

} // namespace storage
} // namespace dstream
