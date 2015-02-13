/**
* @file     dredis_sa.h
* @brief    dredis·ÃÎÊ²ã·â×°
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#include <string>
#include "common/logger.h"
#include "storage/dredis.h"
#include "storage/dredis_sa.h"

using namespace dredis;

namespace dstream {
namespace storage {

//-----------------------------------------------------------------------------
// DRedisSA

int DRedisSA::Init(const std::string& sn_list,
                   const std::string& master_name,
                   int retry, int timeout_ms, const std::string& token) {
    m_retry = retry;
    for (int i = 0; i < m_retry; i++) {
        if ((rclient::OK == m_redis_client.Init(sn_list, master_name,
                        rclient::MASTER_ONLY, 0, timeout_ms, token)) &&
            (rclient::OK == m_redis_client.Connect())) {
            DSTREAM_INFO("conn dredis[%s] with sn[%s] timeout_ms[%d] token"
                         "[%s] ...OK", master_name.c_str(), sn_list.c_str(),
                         timeout_ms, token.c_str());
            m_is_initialized  = true;
            m_is_valid = true;
            return storage::OK;
        }
        DSTREAM_WARN("fail to conn dredis:[%s]", m_redis_client.ErrStr());
    }
    return storage::SYNC_CONN_FAIL;
}

int DRedisSA::CreateTable(const std::string& table_name,
                          StoreTableType table_type,
                          bool replace_old,
                          IStoreTablePtr* pt) {
    _CHECK_DREDIS_OK_OR_RETURN_();
    if ((NULL == pt) || table_name.empty() ||
        !IsInValidStoreTableType(table_type)) {
        return storage::BAD_ARGS;
    }

    int ret = storage::OK;
    for (int i = 0; i < m_retry; i++) {
        ret = CreateRedisTable(table_name, table_type, replace_old, &m_redis_client);
        if (ret < storage::OK) {
            DSTREAM_WARN("fail to create table[%s]", table_name.c_str());
            continue;
        }
        ret = RedisTableFactory(m_redis_client, table_name, table_type, pt);
        if (ret < storage::OK) {
            DSTREAM_WARN("fail to alloc table[%s]:[%s]",
                         table_name.c_str(), storage::err_str(ret));
            return ret;
        }
        return storage::OK;
    }
    return ret;
}

int DRedisSA::DeleteTable(const std::string& table_name) {
    _CHECK_DREDIS_OK_OR_RETURN_();
    if (table_name.empty()) {
        return storage::BAD_ARGS;
    }

    rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "del %s", table_name.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_INTEGER)) {
            DSTREAM_WARN("fail to del table[%s]:[%s]", table_name.c_str(),
                         NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

int DRedisSA::GetTable(const std::string& table_name, IStoreTablePtr* pt) {
    _CHECK_DREDIS_OK_OR_RETURN_();
    if ((NULL == pt) || table_name.empty()) {
        return storage::BAD_ARGS;
    }

    bool exists = false;
    StoreTableType table_type = INVALID_TABLE_TYPE;
    if (TestTableExist(table_name, &exists, &table_type) < storage::OK) {
        return storage::EXEC_CMD_ERR;
    }
    int ret = RedisTableFactory(m_redis_client, table_name, table_type, pt);
    if (ret < storage::OK) {
        DSTREAM_WARN("no memory or unknown table type[%d]@[%s]",
                        table_type, table_name.c_str());
        return ret;
    }
    return storage::OK;
}

int DRedisSA::TestTableExist(const std::string& table_name, bool* exists) {
    return TestTableExist(table_name, exists, NULL);
}
int DRedisSA::TestTableExist(const std::string& table_name, bool* exists,
                             StoreTableType* p_table_type) {
    _CHECK_DREDIS_OK_OR_RETURN_();
    if ((NULL == exists) || table_name.empty()) {
        return storage::BAD_ARGS;
    }

    rclient::RedisReply* r = NULL;
    for (int i = 0; i < m_retry; i++) {
        r = m_redis_client.SendCmd(NULL, "type %s", table_name.c_str());
        if ((NULL == r) || (r->type != REDIS_REPLY_STATUS)) {
            DSTREAM_WARN("fail to get table[%s]:[%s]", table_name.c_str(),
                         NULL == r ? NULL : r->str);
            m_redis_client.FreeReplyObj(r);
            continue;
        }
        *exists = strcasecmp(r->str, "none") ? true : false;
        if (NULL != p_table_type) {
            RedisStringToType(r->str, p_table_type);
            if (INVALID_TABLE_TYPE == *p_table_type) {
                m_redis_client.FreeReplyObj(r);
                return storage::UNKNOWN_TABLE_TYPE;
            }
        }
        m_redis_client.FreeReplyObj(r);
        return storage::OK;
    }
    return storage::EXEC_CMD_ERR;
}

} // namespace storage
} // namespace dstream
