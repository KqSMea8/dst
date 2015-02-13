/**
* @file     dredis.h
* @brief    dredis·ÃÎÊ²ã·â×°
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#include "common/logger.h"
#include "storage/dredis.h"
#include "storage/dredis_hash.h"
#include "storage/dredis_list.h"

using namespace dredis;

namespace dstream {
namespace storage {

//-----------------------------------------------------------------------------
// RedisConnection
int RedisConnection::CopyRClient(const dredis::rclient::RClient& rc) {
    if (m_redis_client.CopyInit(rc) < rclient::OK) {
        DSTREAM_WARN("fail to copy redis conn config: [%s]\n", m_redis_client.ErrStr());
        return storage::COPY_CFG_FAIL;
    }
    for (int i = 0; i < m_retry; i++) {
        if (rclient::OK == m_redis_client.Connect()) {
            return storage::OK;
        }
    }
    DSTREAM_WARN("fail to connect redis server: [%s]", m_redis_client.ErrStr());
    return storage::SYNC_CONN_FAIL;
}

//-----------------------------------------------------------------------------
// Utils

void RedisStringToType(const std::string& s, StoreTableType* t) {
    if (NULL != t) {
        if (s == "hash") {
            *t = REDIS_HASH_TABLE;
        } else if (s == "list") {
            *t = REDIS_LIST_TABLE;
        } else {
            *t = INVALID_TABLE_TYPE;
        }
    }
}

void RedisTypeToString(const StoreTableType& t, std::string* s) {
    if (NULL != s) {
        switch (t) {
        case REDIS_HASH_TABLE:
            *s = "hash";
            break;
        case REDIS_LIST_TABLE:
            *s = "list";
            break;
        default:
            *s = "unknown";
        }
    }
}

bool GetAndCheckQueued(rclient::RClient& rc, const std::string& name) {
    rclient::RedisReply* r;
    if ((rc.GetReply(&r) < rclient::OK) ||
        (NULL == r) || !rclient::IsExecQueued(r)) {
        DSTREAM_WARN("fail to exec 'multi'@[%s]:[%s]\n",
                     name.c_str(), (NULL == r ? NULL : r->str));
        rc.FreeReplyObj(r);
        return false;
    }
    rc.FreeReplyObj(r);
    return true;
}

int CreateRedisTable(const std::string& table_name,
                     StoreTableType table_type,
                     bool replace_old,
                     dredis::rclient::RClient* rc) {
    // use transaction to exec table creation
    assert(NULL != rc);
    rclient::RedisReply* r;
    int ret = rclient::OK;
    ret = rc->AppendCmd(NULL, "multi");
    if ((ret < rclient::OK) || (rc->GetReply(&r) < rclient::OK) ||
        (NULL == r) || !rclient::IsReplyStatusOK(r)) {
        DSTREAM_WARN("fail to exec 'multi'@[%s]:[%s]\n",
                     table_name.c_str(), (NULL == r ? NULL : r->str));
        rc->FreeReplyObj(r);
        return storage::EXEC_CMD_ERR;
    }
    rc->FreeReplyObj(r);
    if (replace_old) {
        ret = rc->AppendCmd(NULL, "del %s", table_name.c_str());
        if ((ret < rclient::OK) || !GetAndCheckQueued(*rc, table_name)) {
            return storage::EXEC_CMD_ERR;
        }
    }
    switch (table_type) {
    case REDIS_LIST_TABLE:
        ret = rc->AppendCmd(NULL, "lpush %s placeholder", table_name.c_str());
        if ((ret < rclient::OK) || !GetAndCheckQueued(*rc, table_name)) {
            return storage::EXEC_CMD_ERR;
        }
        ret = rc->AppendCmd(NULL, "lpop %s", table_name.c_str());
        if ((ret < rclient::OK) || !GetAndCheckQueued(*rc, table_name)) {
            return storage::EXEC_CMD_ERR;
        }
        break;
    case REDIS_HASH_TABLE:
        ret = rc->AppendCmd(NULL, "hset %s placeholder value", table_name.c_str());
        if ((ret < rclient::OK) || !GetAndCheckQueued(*rc, table_name)) {
            return storage::EXEC_CMD_ERR;
        }
        ret = rc->AppendCmd(NULL, "hdel %s placeholder", table_name.c_str());
        if ((ret < rclient::OK) || !GetAndCheckQueued(*rc, table_name)) {
            return storage::EXEC_CMD_ERR;
        }
        break;
    default:
        DSTREAM_WARN("unknown table type[%d]@[%s]",
                     table_type, table_name.c_str());
        return storage::BAD_ARGS;
    }

    int i = 0;
    ret = rc->AppendCmd(NULL, "exec");
    if ((ret < rclient::OK) ||
        (rc->GetReply(&r) < rclient::OK) ||
        (NULL == r) || (r->type != REDIS_REPLY_ARRAY)) {
        goto ERR_RET;
    }
    // deal the real reply
    if (replace_old) { // for del
        if ((r->element[i] == NULL) ||
            (r->element[i]->type != REDIS_REPLY_INTEGER)) {
            goto ERR_RET;
        }
    }
    i++;
    if (REDIS_LIST_TABLE == table_type) {
        if ((r->element[i] == NULL) ||
            (r->element[i]->type != REDIS_REPLY_INTEGER)) {
            goto ERR_RET;
        }
        i++;
        if ((r->element[i] == NULL) ||
            ((r->element[i]->type != REDIS_REPLY_STRING) &&
             (r->element[i]->type != REDIS_REPLY_NIL))) {
            goto ERR_RET;
        }
        i++;
    } else {
        if ((r->element[i] == NULL) ||
            (r->element[i]->type != REDIS_REPLY_INTEGER)) {
            goto ERR_RET;
        }
        i++;
        if ((r->element[i] == NULL) ||
            (r->element[i]->type != REDIS_REPLY_INTEGER)) {
            goto ERR_RET;
        }
        i++;
    }
    rc->FreeReplyObj(r);
    return storage::OK;
ERR_RET:
    DSTREAM_WARN("fail to get reply@[%s]\n", table_name.c_str());
    rc->FreeReplyObj(r);
    return storage::EXEC_CMD_ERR;
}

int RedisTableFactory(const rclient::RClient& rc,
                      const std::string& table_name,
                      StoreTableType table_type,
                      IStoreTablePtr* pt) {
    if (NULL == pt) {
        return storage::BAD_ARGS;
    }

    int ret = storage::OK;
    switch (table_type) {
    case REDIS_LIST_TABLE:
        RedisListTable* plist = new (std::nothrow) RedisListTable(table_name);
        if (NULL == plist) {
            return storage::BAD_MEM_ALLOC;
        }
        ret = plist->CopyRClient(rc);
        if (ret < rclient::OK) {
            delete plist;
            plist = NULL;
            return ret;
        }
        pt->reset(plist);
        break;
    case REDIS_HASH_TABLE:
        RedisHashTable* phash = new (std::nothrow) RedisHashTable(table_name);
        if (NULL == phash) {
            return storage::BAD_MEM_ALLOC;
        }
        ret = phash->CopyRClient(rc);
        if (ret < rclient::OK) {
            delete phash;
            phash = NULL;
            return ret;
        }
        pt->reset(phash);
        break;
    default:
        return storage::UNKNOWN_TABLE_TYPE;
    }
    return storage::OK;
}

} // namespace storage
} // namespace dstream
