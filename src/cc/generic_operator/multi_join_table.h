/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
/**
 * @file join_table.h
 * @author luxing
 * @date 2012/09/24
 * @brief join table，提供给join算子用来存储、查找数据的表
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_MULTI_JOIN_TABLE_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_MULTI_JOIN_TABLE_H__ // NOLINT

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
 * @brief MultiJoinTable模版类，提供给join算子用来存储、查找数据的表
 *        本质上是哈希表实现，但是允许同一key对应多个value
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
class MultiJoinTable {
public:
    template<class, class, class, class, class, class> friend class MultiJoinTableIterator;

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
    MultiJoinTable();

    /**
     * @brief 构造函数
     *
     * @param bucket_count[in] 哈希表实现中初始bucket的数目
     * @param key_serializer 对key序列化和反序列化的类
     * @param value_serializer 对value序列化和反序列化的类
     * @param hash[in] 对key取哈希的哈希函数
     * @param equal[in] 对key进行相等比较的函数
     */
    MultiJoinTable(size_t bucket_count,
            const KeySerializer& key_serializer = KeySerializer(),
            const ValueSerializer& value_serializer = ValueSerializer(),
            const Hash& hash = Hash(),
            const Pred& equal = Pred());

    /**
     * @brief 析构函数
     *
     */
    virtual ~MultiJoinTable();

    /**
     * @brief 设置外部存储中对应的表
     *
     * @param store_table[in] 外部存储中对应的表的指针
     * @param restore[in] 是否从外部存储的表中恢复数据
     * @param ignore_store_error[in] 是否忽略外部存储的错误
     *                               默认情况下外部存储只是做备份，因此可以忽略错误
     * @param batch_insert 是否批量写入外部存储
     *                     另外，由于使用list在外部存储中存储，因此不支持从外部存储中删除key
     * @param batch_count 批量写入的record的条数
     * @param batch_interval_in_ms 批量写入的时间间隔，单位ms
     *                             以上两个条件任何一个先达到，则触发批量写外部存储
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    virtual int SetStore(storage::IStoreTablePtr store_table,
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
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    virtual int Insert(const K& key, const V& value);

    /**
     * @brief 从表中查找数据
     *
     * @param key[in] key
     * @param values[out] 用于存储查找结果的vector的指针
     *
     * @return 匹配的项的数目
     *         若为负数，则是出现错误的错误码
     */
    virtual int Find(const K& key, std::vector<V>* values);

    /**
     * @brief 从表中删除数据，如果配置了外部存储表，则同时从外部存储表删除对应数据
     *
     * @param key[in] 要删除的key
     *
     * @return 0 成功
     *         -2 外部存储错误
     */
    virtual int Remove(const K& key);

    /**
     * @brief 遍历外部存储表恢复数据时的回调，每读到一条数据会触发一次回调
     *
     * @param ctx[in] 传入的context指针
     * @param err[in] 读数据时的错误码
     * @param data[in] 读到的data
     *
     * @return OK 成功
     *         其他 出错的错误码
     */
    static int RestoreRecordCallback(void* ctx,
            int err,
            const std::string& data);

private:
    // insert kv into store
    int InsertIntoStore(const K& key, const V& value);

    // insert serialized kv into store
    int InsertSerializedKvIntoStore(const std::string& serialized_kv);

    // remove key from store
    int RemoveFromStore(const K& key);

    // 序列化kv对
    bool SerializeKv(const K& key, const V& value, std::string* output);

    // 序列化需要删除的key
    bool SerializeDeletedKey(const K& key, std::string* output);

    // 反序列化kv对
    bool DeserializeKv(const std::string& input, K* key, V* value, bool* is_deleted);

    typedef boost::unordered_multimap<K, V, Hash, Pred> UnorderedMultiMap;

    // 允许key重复的哈希表，用于存储和查找数据
    UnorderedMultiMap m_table;

    // 用于序列化和反序列化key
    KeySerializer m_key_serializer;
    // 用于序列化和反序列化value
    ValueSerializer m_value_serializer;

    // 外存中对应的表的指针
    boost::shared_ptr<storage::IListTable> m_store_list_table;

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

    // 缓存的需要批量写入的序列化好的kv对
    std::vector<std::string> m_batch_kv;
};

/**
 * @brief MultiJoinTableIterator模版类
 *        用于遍历MultiJoinTable中的所有数据的迭代器
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
class MultiJoinTableIterator {
public:
    /**
     * @brief 构造函数
     *
     * @param join_table[in] 需要遍历的MultiJoinTable的引用
     */
    MultiJoinTableIterator(const MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>&
            join_table);

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
    typename MultiJoinTable<K, V, KeySerializer, ValueSerializer,
                            Hash, Pred>::UnorderedMultiMap::const_iterator
    m_itr;

    // 遍历结束的标志iterator
    typename MultiJoinTable<K, V, KeySerializer, ValueSerializer,
                            Hash, Pred>::UnorderedMultiMap::const_iterator
    m_itr_end;
};


template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::MultiJoinTable()
        : m_ignore_store_error(true),
        m_batch_insert(false),
        m_batch_count(0),
        m_batch_interval_in_ms(0),
        m_last_batch_insert_time(0) {
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::MultiJoinTable(
        size_t bucket_count,
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
MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::~MultiJoinTable() {
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::SetStore(
        dstream::storage::IStoreTablePtr store_table,
        bool restore,
        bool ignore_store_error,
        bool batch_insert,
        uint32_t batch_count,
        uint32_t batch_interval_in_ms) {
    if (NULL == store_table) {
        return ARGUMENT_INVALID;
    }
    m_store_list_table = boost::dynamic_pointer_cast<dstream::storage::IListTable>(store_table);
    if (NULL == m_store_list_table) {
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
        int store_ret = m_store_list_table->GetAll(RestoreRecordCallback, this);
        if (0 != store_ret) {
            DSTREAM_WARN("get all records from store failed, err: %d", store_ret);
            return STORE_ERROR;
        }
        DSTREAM_INFO("data restored");
    }

    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::RestoreRecordCallback(
        void* ctx,
        int /*err*/,
        const std::string& data) {
    MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>* p_this =
        reinterpret_cast<MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>*>(ctx);
    assert(NULL != p_this);
    K k;
    V v;
    bool is_deleted = false;
    if (!p_this->DeserializeKv(data, &k, &v, &is_deleted)) {
        DSTREAM_WARN("failed to deserialize kv");
        return STORE_DATA_ERROR;
    }
    if (is_deleted) {
        p_this->m_table.erase(k);
    } else {
        p_this->m_table.insert(std::make_pair(k, v));
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Insert(const K& key,
        const V& value) {
    m_table.insert(std::make_pair(key, value));
    if (NULL != m_store_list_table) {
        // insert data into store
        return InsertIntoStore(key, value);
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Find(const K& key,
        std::vector<V>* values) {
    if (NULL == values) {
        return ARGUMENT_INVALID;
    }
    int count = 0;
    std::pair<typename UnorderedMultiMap::iterator, typename UnorderedMultiMap::iterator>
    range = m_table.equal_range(key);
    for (typename UnorderedMultiMap::iterator itr = range.first;
         itr != range.second;
         ++itr) {
        values->push_back(itr->second);
        ++count;
    }
    return count;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Remove(const K& key) {
    m_table.erase(key);
    if (NULL != m_store_list_table) {
        // remove from store
        return RemoveFromStore(key);
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::InsertIntoStore(const K& key,
        const V& value) {
    assert(NULL != m_store_list_table);
    std::string serialized_kv;
    if (!SerializeKv(key, value, &serialized_kv)) {
        DSTREAM_WARN("failed to serialized kv");
        return DATA_ERROR;
    }
    return InsertSerializedKvIntoStore(serialized_kv);
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::InsertSerializedKvIntoStore(
        const std::string& serialized_kv) {
    if (m_batch_insert) {
        m_batch_kv.push_back(serialized_kv);
        struct timeval tv;
        gettimeofday(&tv, NULL);
        uint64_t cur_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        if (m_batch_kv.size() >= m_batch_count
            || cur_time >= m_last_batch_insert_time + m_batch_interval_in_ms) {
            std::vector<int> ret_vals;
            int ret = m_store_list_table->PushBack(m_batch_kv, &ret_vals);
            m_batch_kv.clear();
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
        if (storage::OK != m_store_list_table->PushBack(serialized_kv)) {
            DSTREAM_ERROR("insert into storage failed");
            return STORE_ERROR;
        } else {
            DSTREAM_INFO("insert succeeded");
            return OK;
        }
    }
    return OK;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
bool MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::SerializeKv(const K& key,
        const V& value, std::string* output) {
    std::string serialized_key;
    std::string serialized_value;
    if (!m_key_serializer.Serialize(key, &serialized_key)) {
        DSTREAM_ERROR("failed to serialize key");
        return false;
    }
    if (!m_value_serializer.Serialize(value, &serialized_value)) {
        DSTREAM_ERROR("failed to serialize key");
        return false;
    }
    uint32_t key_size = serialized_key.size();
    output->append(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
    output->append(serialized_key.data(), serialized_key.size());
    uint32_t value_size = serialized_value.size();
    output->append(reinterpret_cast<const char*>(&value_size), sizeof(value_size));
    output->append(serialized_value.data(), serialized_value.size());
    return true;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
bool MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::SerializeDeletedKey(
        const K& key, std::string* output) {
    std::string serialized_key;
    if (!m_key_serializer.Serialize(key, &serialized_key)) {
        DSTREAM_ERROR("failed to serialize key");
        return false;
    }
    uint32_t key_size = serialized_key.size();
    output->append(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
    output->append(serialized_key.data(), serialized_key.size());
    return true;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
bool MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::DeserializeKv(
        const std::string& input, K* key, V* value, bool* is_deleted) {
    const char* kv_data = input.data();
    uint32_t kv_len = input.size();

    if (kv_len < sizeof(uint32_t)) {
        DSTREAM_WARN("serialized kv data is too small to hold key_len: %u", kv_len);
        return false;
    }
    uint32_t key_len = *reinterpret_cast<const uint32_t*>(kv_data);
    kv_data += sizeof(uint32_t);
    kv_len -= sizeof(uint32_t);
    if (kv_len < key_len) {
        DSTREAM_WARN("real key data is too small: %u < %u", kv_len, key_len);
        return false;
    }
    std::string serialized_key(kv_data, key_len);
    kv_data += key_len;
    kv_len -= key_len;

    if (!m_key_serializer.Deserialize(serialized_key, key)) {
        DSTREAM_ERROR("failed to serialize key");
        return false;
    }

    if (0 == kv_len) {
        // the key is deleted
        *is_deleted = true;
        return true;
    } else {
        // deserialize the value
        *is_deleted = false;
        if (kv_len < sizeof(uint32_t)) {
            DSTREAM_WARN("serialized kv data is too small to hold value_len: %u", kv_len);
            return false;
        }
        uint32_t value_len = *reinterpret_cast<const uint32_t*>(kv_data);
        kv_data += sizeof(uint32_t);
        kv_len -= sizeof(uint32_t);
        if (kv_len < value_len) {
            DSTREAM_WARN("real value data is too small: %u < %u", kv_len, value_len);
            return false;
        }
        std::string serialized_value(kv_data, value_len);

        if (!m_value_serializer.Deserialize(serialized_value, value)) {
            DSTREAM_ERROR("failed to serialize value");
            return false;
        }
    }
    return true;
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
int MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>::RemoveFromStore(
        const K& key) {
    assert(NULL != m_store_list_table);
    std::string serialized_deleted_key;
    if (!SerializeDeletedKey(key, &serialized_deleted_key)) {
        DSTREAM_WARN("failed to serialized deleted key");
        return DATA_ERROR;
    }
    return InsertSerializedKvIntoStore(serialized_deleted_key);
}


template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
MultiJoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::MultiJoinTableIterator(
        const MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>& join_table) {
    m_itr = join_table.m_table.begin();
    m_itr_end = join_table.m_table.end();
}

template<class K, class V, class KeySerializer, class ValueSerializer, class Hash, class Pred>
bool MultiJoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::GetCurrent(K* key,
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
void MultiJoinTableIterator<K, V, KeySerializer, ValueSerializer, Hash, Pred>::Next() {
    ++m_itr;
}

} // namespace generic_operator
} // namespace dstream

#endif // __DSTREAM_GENERIC_OPERATOR_MULTI_JOIN_TABLE_H__ NOLINT
