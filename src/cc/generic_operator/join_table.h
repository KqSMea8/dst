/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
/**
 * @file join_table.h
 * @author luxing
 * @date 2012/10/30
 * @brief join table，提供给join算子用来存储、查找数据的表
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_JOIN_TABLE_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_JOIN_TABLE_H__ // NOLINT

#include <boost/unordered_map.hpp>
#include <sys/time.h>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "common/logger.h"
#include "generic_operator/default_serializer.h"
#include "storage/storage_table.h"

namespace dstream {
namespace generic_operator {

/**
 * @brief JoinTable模版类，提供给join算子用来存储、查找数据的表
 *        本质上是哈希表实现
 *        如果配置了外部存储中对应的表，则所有数据会同时存储在外部存储中，用于数据恢复
 *
 * @param K key的类型
 * @param V value的类型
 * @param KeySerializer 对key进行序列化和反序列化的类的类型
 * @param ValueSerializer 对value进行序列化和反序列化的类的类型
 * @param Hash 用来算哈希值的function object的类型
 * @param Pred 用来判断key相同的function object的类型
 */
template < class K,
         class V,
         class KeySerializer = DefaultSerializer<K>,
         class ValueSerializer = DefaultSerializer<V>,
         class Hash = boost::hash<K>,
         class Pred = std::equal_to<K> >
class JoinTable {
public:
    template<class, class, class, class, class, class> friend class JoinTableIterator;

    // 定义返回的错误码
    enum ErrorCode {
        OK = 0,
        KEY_EXIST = -1,
        ARGUMENT_INVALID = -2,
        STORE_ERROR = -3,
        STORE_DATA_ERROR = -4,
        DATA_ERROR = -5
    };

    /**
     * @brief 构造函数
     */
    JoinTable();

    /**
     * @brief 构造函数
     *
     * @param bucket_count[in] 哈希表实现中初始bucket的数目
     * @param key_serializer 对key序列化和反序列化的类
     * @param value_serializer 对value序列化和反序列化的类
     * @param hash[in] 对key取哈希的哈希函数
     * @param equal[in] 对key进行相等比较的函数
     */
    JoinTable(size_t bucket_count,
              const KeySerializer& key_serializer = KeySerializer(),
              const ValueSerializer& value_serializer = ValueSerializer(),
              const Hash& hash = Hash(),
              const Pred& equal = Pred());

    /**
     * @brief 析构函数
     *
     */
    virtual ~JoinTable();

    /**
     * @brief 设置外部存储中对应的表
     *
     * @param store_table[in] 外部存储中对应的表的指针
     * @param restore[in] 是否从外部存储的表中恢复数据
     * @param ignore_store_error[in] 是否忽略外部存储的错误
     *                               默认情况下外部存储只是做备份，因此可以忽略错误
     * @param batch_insert[in] 是否批量写入外部存储
     *                         另外，目前不支持批量从外部存储中删除，因为难以保证批量插入与删除的顺序
     *                         今后将通过异步插入与删除解决批量问题
     * @param batch_count[in] 批量写入的record的条数
     * @param batch_interval_in_ms[in] 批量写入的时间间隔，单位ms
     *                             以上两个条件任何一个先达到，则触发批量写外部存储
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    virtual int SetStore(dstream::storage::IStoreTablePtr store_table,
                         bool restore,
                         bool ignore_store_error,
                         bool batch_insert,
                         uint32_t batch_count,
                         uint32_t batch_interval_in_ms);

    /**
     * @brief 向表中插入数据，如果配置了外部存储表，则同时写入外部存储表
     *
     * @param key[in] key
     * @param value[in] value
     * @param replace[in] 如果key已存在，是否用新值替代旧值
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    virtual int Insert(const K& key, const V& value, bool replace = true);

    /**
     * @brief 从表中查找数据
     *
     * @param key[in] key
     * @param value[out] 用于输出查找结果的V的指针
     *
     * @return 匹配的项的数目，如果找到key则为1，否则为0
     *         若为负数，则是出现错误的错误码
     */
    virtual int Find(const K& key, V* value);

    /**
     * @brief 从表中删除数据，如果配置了外部存储表，则同时从外部存储表删除对应数据
     *
     * @param key[in] 要删除的key
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    virtual int Remove(const K& key);

    /**
     * @brief 遍历外部存储表恢复数据时的回调，每读到一条数据会触发一次回调
     *
     * @param ctx[in] 传入的context指针
     * @param err[in] 读数据时的错误码
     * @param key[in] 读到的key
     * @param value[in] 读到的value
     *
     * @return 0 成功
     *         其他 错误码
     */
    static int RestoreRecordCallback(void* ctx,
            int err,
            const std::string& key,
            const std::string& value);

private:
    // insert kv into store
    int InsertIntoStore(const K& key, const V& value);

    // remove key from store
    int RemoveFromStore(const K& key);

    typedef boost::unordered_map<K, V, Hash, Pred> UnorderedMap;

    // 哈希表，用于存储和查找数据
    UnorderedMap m_table;

    // 用于序列化和反序列化key
    KeySerializer m_key_serializer;
    // 用于序列化和反序列化value
    ValueSerializer m_value_serializer;

    // 外存中对应的表的指针
    boost::shared_ptr<storage::IHashTable> m_store_hash_table;

    // 是否忽略外部存储的错误
    bool m_ignore_store_error;

    // 是否批量写入外部存储
    bool m_batch_insert;
    // 批量写入外部存储的record数量
    uint32_t m_batch_count;
    // 批量写入外部存储的时间间隔，单位毫秒
    uint64_t m_batch_interval_in_ms;

    // 上次批量写入外部存储的时间
    uint64_t m_last_batch_insert_time;

    // 缓存的需要批量写入的key
    std::vector<std::string> m_batch_keys;
    // 缓存的需要批量写入的value
    std::vector<std::string> m_batch_values;
};

/**
 * @brief JoinTableIterator模版类
 *        用于遍历JoinTable中的所有数据的迭代器
 *
 * @param K key的类型
 * @param V value的类型
 * @param Hash 用来算哈希值的function object的类型
 * @param Pred 用来判断key相同的function object的类型
 *
 * @note 使用iterator遍历的过程中不能对表有任何的插入、删除等更改操作
 */
template < class K,
         class V,
         class KeySerializer = DefaultSerializer<K>,
         class ValueSerializer = DefaultSerializer<V>,
         class Hash = boost::hash<K>,
         class Pred = std::equal_to<K> >
class JoinTableIterator {
public:
    /**
     * @brief 构造函数
     *
     * @param join_table[in] 需要遍历的JoinTable的引用
     */
    JoinTableIterator(
            const JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>& join_table);

    /**
     * @brief 获取当前遍历到的数据
     *
     * @param key[out] 返回当前遍历到的key
     * @param value[out] 返回当前遍历到的value
     *
     * @return true表示遍历尚未结束
     *         false表示遍历结束
     */
    bool GetCurrent(K* key, V* value);

    /**
     * @brief iterator顺移
     */
    void Next();

private:
    // 当前的iterator
    typename JoinTable<K, V, KeySerializer, ValueSerializer,
                       Hash, Pred>::UnorderedMap::const_iterator
    m_itr;

    // 遍历结束的标志iterator
    typename JoinTable<K, V, KeySerializer, ValueSerializer,
                       Hash, Pred>::UnorderedMap::const_iterator
    m_itr_end;
};


template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::JoinTable()
    : m_ignore_store_error(true),
    m_batch_insert(false),
    m_batch_count(0),
    m_batch_interval_in_ms(0),
    m_last_batch_insert_time(0) {
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::JoinTable(size_t bucket_count,
        const KeySerializer& key_serializer,
        const ValueSerializer& value_serializer,
        const Hash& hash,
        const Pred& equal)
    : m_table(bucket_count, hash, equal),
    m_key_serializer(key_serializer),
    m_value_serializer(value_serializer),
    m_ignore_store_error(true),
    m_batch_insert(false),
    m_batch_count(0),
    m_batch_interval_in_ms(0),
    m_last_batch_insert_time(0) {
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::~JoinTable() {
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::SetStore(
        dstream::storage::IStoreTablePtr store_table,
        bool restore,
        bool ignore_store_error,
        bool batch_insert,
        uint32_t batch_count,
        uint32_t batch_interval_in_ms) {
    if (NULL == store_table) {
        return ARGUMENT_INVALID;
    }
    m_store_hash_table = boost::dynamic_pointer_cast<dstream::storage::IHashTable>(store_table);
    if (NULL == m_store_hash_table) {
        return ARGUMENT_INVALID;
    }
    m_ignore_store_error = ignore_store_error;
    m_batch_insert = batch_insert;
    m_batch_count = batch_count;
    m_batch_interval_in_ms = batch_interval_in_ms;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_last_batch_insert_time =  tv.tv_sec * 1000 + tv.tv_usec / 1000;

    if (restore) {
        // restore data from store table
        m_table.clear();
        int store_ret = m_store_hash_table->GetAll(RestoreRecordCallback, this);
        if (0 != store_ret) {
            DSTREAM_WARN("get all records from store failed, err: %d", store_ret);
            return STORE_ERROR;
        }
        DSTREAM_INFO("data restored");
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::RestoreRecordCallback(void* ctx,
        int /*err*/,
        const std::string& key,
        const std::string& value) {
    JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>* p_this =
        reinterpret_cast<JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>*>(ctx);
    assert(NULL != p_this);
    K k;
    V v;
    if (!p_this->m_key_serializer.Deserialize(key, &k)) {
        DSTREAM_WARN("failed to deserialize key");
        return STORE_DATA_ERROR;
    }
    if (!p_this->m_value_serializer.Deserialize(value, &v)) {
        DSTREAM_WARN("failed to deserialize value");
        return STORE_DATA_ERROR;
    }
    p_this->m_table[k] = v;
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Insert(const K& key,
        const V& value, bool replace) {
    if (replace) {
        m_table[key] = value;
    } else {
        std::pair<typename UnorderedMap::iterator, bool> ret =
            m_table.insert(std::make_pair(key, value));
        if (!ret.second) {
            // key already exists
            return KEY_EXIST;
        }
    }
    if (NULL != m_store_hash_table) {
        // insert data into store
        int ret = InsertIntoStore(key, value);
        if (STORE_ERROR == ret && m_ignore_store_error) {
            return OK;
        } else {
            return ret;
        }
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Find(const K& key, V* value) {
    if (NULL == value) {
        return ARGUMENT_INVALID;
    }
    int count = 0;
    typename UnorderedMap::iterator itr = m_table.find(key);
    if (m_table.end() != itr) {
        *value = itr->second;
        count = 1;
    }
    return count;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Remove(const K& key) {
    m_table.erase(key);
    if (NULL != m_store_hash_table) {
        // remove from store
        int ret = RemoveFromStore(key);
        if (STORE_ERROR == ret && m_ignore_store_error) {
            return OK;
        } else {
            return ret;
        }
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::InsertIntoStore(const K& key,
        const V& value) {
    assert(NULL != m_store_hash_table);
    std::string serialized_key;
    std::string serialized_value;
    if (!m_key_serializer.Serialize(key, &serialized_key)) {
        DSTREAM_ERROR("failed to serialize key");
        return DATA_ERROR;
    }
    if (!m_value_serializer.Serialize(value, &serialized_value)) {
        DSTREAM_ERROR("failed to serialize key");
        return DATA_ERROR;
    }
    if (m_batch_insert) {
        m_batch_keys.push_back(serialized_key);
        m_batch_values.push_back(serialized_value);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t cur_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        if (m_batch_keys.size() >= m_batch_count
            || cur_time >= m_last_batch_insert_time + m_batch_interval_in_ms) {
            std::vector<int> ret_vals;
            int ret = m_store_hash_table->Insert(m_batch_keys, m_batch_values, &ret_vals, true);
            m_batch_keys.clear();
            m_batch_values.clear();
            m_last_batch_insert_time = cur_time;
            if (storage::OK != ret) {
                DSTREAM_ERROR("batch insert into storage failed, err: %d", ret);
                return STORE_ERROR;
            } else {
                DSTREAM_INFO("batch insert succeeded");
                return OK;
            }
        }
    } else {
        int store_ret = m_store_hash_table->Insert(serialized_key, serialized_value, true);
        if (storage::OK != store_ret) {
            DSTREAM_ERROR("insert into storage failed, err: %d", store_ret);
            return STORE_ERROR;
        } else {
            DSTREAM_INFO("insert succeeded");
            return OK;
        }
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::RemoveFromStore(const K& key) {
    assert(NULL != m_store_hash_table);
    std::string serialized_key;
    if (!m_key_serializer.Serialize(key, &serialized_key)) {
        DSTREAM_ERROR("failed to serialize key");
        return DATA_ERROR;
    }
    int store_ret = m_store_hash_table->Erase(serialized_key);
    if (storage::OK != store_ret) {
        DSTREAM_ERROR("remove key from storage failed, err: [%d]", store_ret);
        return STORE_ERROR;
    } else {
        return OK;
    }
}


template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
JoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::JoinTableIterator(
    const JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>& join_table) {
    m_itr = join_table.m_table.begin();
    m_itr_end = join_table.m_table.end();
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
bool JoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::GetCurrent(K* key,
        V* value) {
    if (NULL == key || NULL == value) {
        return false;
    }
    bool ret = false;
    if (m_itr_end != m_itr) {
        *key = m_itr->first;
        *value = m_itr->second;
        ret = true;
    }
    return ret;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
void JoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Next() {
    ++m_itr;
}

} // namespace generic_operator
} // namespace dstream

#endif // __DSTREAM_GENERIC_OPERATOR_JOIN_TABLE_H__ NOLINT
