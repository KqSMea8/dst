/**
* @file     dredis_hash.h
* @brief    dredis hash±í·â×°
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#include "common/logger.h"
#include "storage/dredis_hash.h"

namespace dstream {
namespace storage {

//  exception of std::string for maximum number of characters
//  exception of std::vector for resize

//-----------------------------------------------------------------------------
int RedisHashTable::Count(uint64_t* n) {
    if (NULL == n) {
        return storage::BAD_ARGS;
    }
    dredis::rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "hlen %s", name_.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to get num from hash table[%s]:[%s]",
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

int RedisHashTable::GetAll(HashTraverseCallBack cb, void* ctx) {
    if (NULL == cb) {
        return storage::BAD_ARGS;
    }
    dredis::rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "hgetall %s", name_.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_ARRAY)) {
            DSTREAM_WARN("fail to get all from hash table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        for (size_t j = 0; j < r->elements; j = j + 2) {
            if ((NULL == r->element[j]) || (NULL == r->element[j + 1])) {
                cb(ctx, storage::GET_NULL_REPLY,
                   std::string(), std::string());
            } else if (r->element[j]->type != REDIS_REPLY_STRING) {
                cb(ctx, storage::GET_ERR_REPLY,
                   std::string(r->element[j]->str),
                   std::string(r->element[j + 1]->str));
            } else {
                cb(ctx, storage::OK,
                   std::string(r->element[j]->str, r->element[j]->len),
                   std::string(r->element[j + 1]->str, r->element[j + 1]->len));
            }
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisHashTable::Find(const std::string& k, std::string* v) {
    if (NULL == v) {
        return storage::BAD_ARGS;
    }
    dredis::rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "hget %s %b", name_.c_str(), k.data(), k.size());
        if ((NULL == r) ||
            ((r->type != REDIS_REPLY_STRING) && (r->type != REDIS_REPLY_NIL))) {
            DSTREAM_WARN("fail to get from hash table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
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

int RedisHashTable::Insert(const std::string& k, const std::string& v) {
    return Insert(k, v, false);
}

int RedisHashTable::Insert(const std::string& k, const std::string& v,
                           bool update_if_exists) {
    dredis::rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        if (update_if_exists) {
            r = m_redis_client.SendCmd(NULL, "hset %s %b %b", name_.c_str(),
                            k.data(), k.size(), v.data(), v.size());
        } else {
            r = m_redis_client.SendCmd(NULL, "hsetnx %s %b %b", name_.c_str(),
                            k.data(), k.size(), v.data(), v.size());
        }
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to insert data into hash table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        DUMP_REDIS_REPLY(r);
        int ret = storage::OK;
        if (0 == r->integer) {
            ret = storage::KEY_ALREADY_EXISTS;
        } else if (1 == r->integer) {
            ret = storage::OK;
        } else {
            ret = storage::EXEC_CMD_ERR;
        }
        m_redis_client.FreeReplyObj(r);
        return ret;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisHashTable::Insert(const std::vector<std::string>& keys,
                           const std::vector<std::string>& values,
                           std::vector<int>* ret_vals) {
    return Insert(keys, values, ret_vals, false);
}
int RedisHashTable::Insert(const std::vector<std::string>& keys,
                           const std::vector<std::string>& values,
                           std::vector<int>* ret_vals,
                           bool update_if_exists) {
    const size_t n = keys.size();

    if (NULL == ret_vals) {
        return storage::BAD_ARGS;
    }
    if (values.size() != n) {
        return storage::BAD_ARGS;
    }
    dredis::rclient::RedisReply* r = NULL;
    const char* cmd = update_if_exists ? "hset" : "hsetnx";
    for (size_t i = 0; i < static_cast<size_t>(m_retry); i++) {
        ret_vals->resize(n);
        size_t j;
        for (j = 0; j < n; j++) {
            if (m_redis_client.AppendCmd(NULL, "%s %s %b %b", cmd, name_.c_str(),
                                            keys[j].data(), keys[j].size(),
                                            values[j].data(), values[j].size())
                < dredis::rclient::OK) {
                DSTREAM_WARN("fail to %s data into hash table[%s]:[%s]",
                             cmd, name_.c_str(), m_redis_client.ErrStr());
                break;
            }
        }
        if (j < n) {
            continue;
        }
        for (j = 0; j < n; j++) {
            if (m_redis_client.GetReply(&r) < dredis::rclient::OK) {
                DSTREAM_WARN("fail get reply from hash table[%s]:[%s]",
                             name_.c_str(), m_redis_client.ErrStr());
                break;
            }
            if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
                DSTREAM_WARN("fail to %s data into hash table[%s]:[%s]",
                             cmd, name_.c_str(), NULL == r ? NULL : r->str);
                m_redis_client.FreeReplyObj(r);
                ret_vals->at(j) = storage::EXEC_CMD_ERR;
                continue;
            }
            if (0 == r->integer) {
                ret_vals->at(j) = storage::KEY_ALREADY_EXISTS;
            } else if (1 == r->integer) {
                ret_vals->at(j) = storage::OK;
            } else {
                ret_vals->at(j) = storage::EXEC_CMD_ERR;
            }
            m_redis_client.FreeReplyObj(r);
        }
        if (j >= n) {
            return storage::OK;
        }
        ret_vals->clear();
    } // end of 'retry for'
    return storage::EXEC_CMD_ERR;
}

int RedisHashTable::Erase(const std::string& k) {
    dredis::rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "hdel  %s %b", name_.c_str(), k.data(), k.size());
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to del data from hash table[%s]:[%s]",
                         name_.c_str(), NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int RedisHashTable::Erase(const std::vector<std::string>& keys,
                          std::vector<int>* ret_vals) {
    if (NULL == ret_vals) {
        return storage::BAD_ARGS;
    }
    dredis::rclient::RedisReply* r = NULL;
    const size_t n = keys.size();
    for (int i = 0; i < m_retry; i++) {
        ret_vals->resize(n);
        size_t j;
        for (j = 0; j < n; j++) {
            if (m_redis_client.AppendCmd(NULL, "hdel %s %b", name_.c_str(),
                                            keys[j].data(), keys[j].size())
                < dredis::rclient::OK) {
                DSTREAM_WARN("fail to erase data into hash table[%s]:[%s]",
                                name_.c_str(), m_redis_client.ErrStr());
                break;
            }
        }
        if (j < n) {
            continue;
        }
        for (j = 0; j < n; j++) {
            if (m_redis_client.GetReply(&r) < dredis::rclient::OK) {
                DSTREAM_WARN("fail get reply from hash table[%s]:[%s]",
                             name_.c_str(), m_redis_client.ErrStr());
                break;
            }
            if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
                DSTREAM_WARN("fail to erase data into hash table[%s]:[%s]",
                             name_.c_str(), NULL == r ? NULL : r->str);
                m_redis_client.FreeReplyObj(r);
                ret_vals->at(j) = storage::EXEC_CMD_ERR;
                continue;
            }
            ret_vals->at(j) = storage::OK;
            m_redis_client.FreeReplyObj(r);
        }
        if (j >= n) {
            return storage::OK;
        }
        ret_vals->clear();
    } // end of 'for reply'
    return storage::EXEC_CMD_ERR;
}

int RedisHashTable::Clear() {
    if (CreateRedisTable(name_, type_, true, &m_redis_client) < storage::OK) {
        DSTREAM_WARN("fail to clear hash table[%s]@dredis:[%s]",
                     name_.c_str(), m_redis_client.ErrStr());
        return storage::EXEC_CMD_ERR;
    }
    return storage::OK;
}

} // namespace storage
} // namespace dstream
