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
 * @brief join table���ṩ��join���������洢���������ݵı�
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
 * @brief MultiJoinTableģ���࣬�ṩ��join���������洢���������ݵı�
 *        �������ǹ�ϣ��ʵ�֣���������ͬһkey��Ӧ���value
 *        ����������ⲿ�洢�ж�Ӧ�ı����������ݻ�ͬʱ�洢���ⲿ�洢�У��������ݻָ�
 *
 * @param K key������
 * @param V value������
 * @param KeySerializer ��key�������л��ͷ����л����������
 * @param ValueSerializer ��value�������л��ͷ����л����������
 * @param Hash �������ϣֵ��function object������
 * @param Pred �����ж�key��ͬ��function object������
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

    // ���巵�صĴ�����
    enum ErrorCode {
        OK = 0,
        KEY_EXIST = -1,
        ARGUMENT_INVALID = -2,
        STORE_ERROR = -3,
        STORE_DATA_ERROR = -4,
        DATA_ERROR = -5
    };

    /**
     * @brief ���캯��
     */
    MultiJoinTable();

    /**
     * @brief ���캯��
     *
     * @param bucket_count[in] ��ϣ��ʵ���г�ʼbucket����Ŀ
     * @param key_serializer ��key���л��ͷ����л�����
     * @param value_serializer ��value���л��ͷ����л�����
     * @param hash[in] ��keyȡ��ϣ�Ĺ�ϣ����
     * @param equal[in] ��key������ȱȽϵĺ���
     */
    MultiJoinTable(size_t bucket_count,
            const KeySerializer& key_serializer = KeySerializer(),
            const ValueSerializer& value_serializer = ValueSerializer(),
            const Hash& hash = Hash(),
            const Pred& equal = Pred());

    /**
     * @brief ��������
     *
     */
    virtual ~MultiJoinTable();

    /**
     * @brief �����ⲿ�洢�ж�Ӧ�ı�
     *
     * @param store_table[in] �ⲿ�洢�ж�Ӧ�ı��ָ��
     * @param restore[in] �Ƿ���ⲿ�洢�ı��лָ�����
     * @param ignore_store_error[in] �Ƿ�����ⲿ�洢�Ĵ���
     *                               Ĭ��������ⲿ�洢ֻ�������ݣ���˿��Ժ��Դ���
     * @param batch_insert �Ƿ�����д���ⲿ�洢
     *                     ���⣬����ʹ��list���ⲿ�洢�д洢����˲�֧�ִ��ⲿ�洢��ɾ��key
     * @param batch_count ����д���record������
     * @param batch_interval_in_ms ����д���ʱ��������λms
     *                             �������������κ�һ���ȴﵽ���򴥷�����д�ⲿ�洢
     *
     * @return OK �ɹ�
     *         ���� ����Ĵ�����
     */
    virtual int SetStore(storage::IStoreTablePtr store_table,
            bool restore,
            bool ignore_store_error,
            bool batch_insert,
            uint32_t batch_count,
            uint32_t batch_interval_in_ms);

    /**
     * @brief ����в������ݣ�����������ⲿ�洢����ͬʱд���ⲿ�洢��
     *
     * @param key[in] key
     * @param value[in] value
     *
     * @return OK �ɹ�
     *         ���� ����Ĵ�����
     */
    virtual int Insert(const K& key, const V& value);

    /**
     * @brief �ӱ��в�������
     *
     * @param key[in] key
     * @param values[out] ���ڴ洢���ҽ����vector��ָ��
     *
     * @return ƥ��������Ŀ
     *         ��Ϊ���������ǳ��ִ���Ĵ�����
     */
    virtual int Find(const K& key, std::vector<V>* values);

    /**
     * @brief �ӱ���ɾ�����ݣ�����������ⲿ�洢����ͬʱ���ⲿ�洢��ɾ����Ӧ����
     *
     * @param key[in] Ҫɾ����key
     *
     * @return 0 �ɹ�
     *         -2 �ⲿ�洢����
     */
    virtual int Remove(const K& key);

    /**
     * @brief �����ⲿ�洢��ָ�����ʱ�Ļص���ÿ����һ�����ݻᴥ��һ�λص�
     *
     * @param ctx[in] �����contextָ��
     * @param err[in] ������ʱ�Ĵ�����
     * @param data[in] ������data
     *
     * @return OK �ɹ�
     *         ���� ����Ĵ�����
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

    // ���л�kv��
    bool SerializeKv(const K& key, const V& value, std::string* output);

    // ���л���Ҫɾ����key
    bool SerializeDeletedKey(const K& key, std::string* output);

    // �����л�kv��
    bool DeserializeKv(const std::string& input, K* key, V* value, bool* is_deleted);

    typedef boost::unordered_multimap<K, V, Hash, Pred> UnorderedMultiMap;

    // ����key�ظ��Ĺ�ϣ�����ڴ洢�Ͳ�������
    UnorderedMultiMap m_table;

    // �������л��ͷ����л�key
    KeySerializer m_key_serializer;
    // �������л��ͷ����л�value
    ValueSerializer m_value_serializer;

    // ����ж�Ӧ�ı��ָ��
    boost::shared_ptr<storage::IListTable> m_store_list_table;

    // �Ƿ�����ⲿ�洢�Ĵ���
    bool m_ignore_store_error;

    // �Ƿ�����д���ⲿ�洢
    bool m_batch_insert;
    // ����д���ⲿ�洢��record����
    uint32_t m_batch_count;
    // ����д���ⲿ�洢��ʱ��������λ����
    uint64_t m_batch_interval_in_ms;

    // �ϴ�����д���ⲿ�洢��ʱ��
    uint64_t m_last_batch_insert_time;

    // �������Ҫ����д������л��õ�kv��
    std::vector<std::string> m_batch_kv;
};

/**
 * @brief MultiJoinTableIteratorģ����
 *        ���ڱ���MultiJoinTable�е��������ݵĵ�����
 *
 * @param K key������
 * @param V value������
 * @param Hash �������ϣֵ��function object������
 * @param Pred �����ж�key��ͬ��function object������
 *
 * @note ʹ��iterator�����Ĺ����в��ܶԱ����κεĲ��롢ɾ���ȸ��Ĳ���
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
     * @brief ���캯��
     *
     * @param join_table[in] ��Ҫ������MultiJoinTable������
     */
    MultiJoinTableIterator(const MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>&
            join_table);

    /**
     * @brief ��ȡ��ǰ������������
     *
     * @param key[out] ���ص�ǰ��������key
     * @param value[out] ���ص�ǰ��������value
     *
     * @return true��ʾ������δ����
     *         false��ʾ��������
     */
    bool GetCurrent(K* key, V* value);

    /**
     * @brief iterator˳��
     */
    void Next();

private:
    // ��ǰ��iterator
    typename MultiJoinTable<K, V, KeySerializer, ValueSerializer,
                            Hash, Pred>::UnorderedMultiMap::const_iterator
    m_itr;

    // ���������ı�־iterator
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
