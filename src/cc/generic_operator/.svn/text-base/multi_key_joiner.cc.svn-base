/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file multi_joiner.cc
 * @author lanbijia
 * @date 2012/9/22
 * @brief MultiJoiner类实现
 */

#include "generic_operator/multi_key_joiner.h"

namespace dstream {
namespace generic_operator {

MultiKeyJoiner::MultiKeyJoiner()
        : m_t_table(NULL),
        m_store_accessor(NULL),
        m_timeout_context(NULL),
        m_inited(false),
        m_step_in_sec(0),
        m_window_size_by_step(0),
        m_join_with_store(MultiKeyJoiner::kDefaultJoinWithStore),
        m_restore_from_store(MultiKeyJoiner::kDefaultRestoreFromStore),
        m_ignore_store_error(MultiKeyJoiner::kDefaultIgnoreStoreError),
        m_batch_insert(MultiKeyJoiner::kDefaultStoreBatchInsert),
        m_batch_count(MultiKeyJoiner::kDefaultStoreBatchCount),
        m_batch_interval_in_ms(MultiKeyJoiner::kDefaultStoreBatchIntervalMs) {
    m_timeout_call_back = NULL;
}

MultiKeyJoiner::~MultiKeyJoiner() {
    Destroy();
}

bool MultiKeyJoiner::CheckAndReadConfig(const dstream::config::Config& conf,
        const std::string& id) {
    uint32_t fail_cnt = 0;
    uint32_t expect_cnt = 0;
    // 找到对应id的配置
    std::string read_str;
    std::string prefix_str = kJoinConfigTag;
    // 读取窗口步长
    read_str = prefix_str + "." + id + "." + kWindowStep;
    expect_cnt++;
    if (dstream::error::OK > conf.GetIntValue(read_str, &m_step_in_sec)) {
        m_step_in_sec = kDefaultWindowStep;
        JOINER_WARN("[%s] read '%s' fail, set to kDefaultWindowStep = %u.",
                    __FUNCTION__, kWindowStep, kDefaultWindowStep);
        fail_cnt++;
    } else {
        JOINER_DEBUG("[%s] read '%s' = %u.", __FUNCTION__, kWindowStep, m_step_in_sec);
    }

    // 读取窗口大小
    read_str = prefix_str + "." + id + "." + kWindowSize;
    expect_cnt++;
    if (dstream::error::OK > conf.GetIntValue(read_str, &m_window_size_by_step)) {
        m_window_size_by_step = kDefaultWindowSize;
        JOINER_WARN("[%s] read '%s' fail, set to kWindowSize = %d.",
                    __FUNCTION__, kWindowSize, kDefaultWindowSize);
        fail_cnt++;
    } else {
        JOINER_DEBUG("[%s] read '%s' = %u.", __FUNCTION__, kWindowSize, m_window_size_by_step);
    }

    // 读取外部存储配置
    read_str = prefix_str + "." + id + "." + kJoinWithStore;
    expect_cnt++;
    std::string store_str;
    if (dstream::error::OK > conf.GetValue(read_str, &store_str)) {
        store_str = (kDefaultJoinWithStore ? "true" : "false");
        JOINER_WARN("[%s] read '%s' fail, set to kDefaultJoinWithStore = %d.",
                    __FUNCTION__, kJoinWithStore, kDefaultJoinWithStore);
        fail_cnt++;
    }
    m_join_with_store = ("true" == store_str);
    JOINER_DEBUG("[%s] read '%s' = %d.", __FUNCTION__, kJoinWithStore, m_restore_from_store);

    // 读取其他外部存储配置
    if (m_join_with_store) {
        // 初始化时是否从外部存储恢复
        read_str = prefix_str + "." + id + "." + kJoinStorageTag + "." + kRestoreFromStore;
        expect_cnt++;
        std::string restore_str;
        if (dstream::error::OK > conf.GetValue(read_str, &restore_str)) {
            restore_str = (kDefaultRestoreFromStore ? "true" : "false");
            JOINER_WARN("[%s] read '%s' fail, set to kDefaultRestoreFromStore = %d.",
                        __FUNCTION__, kRestoreFromStore, kDefaultRestoreFromStore);
            fail_cnt++;
        }
        m_restore_from_store = ("true" == restore_str);

        // 是否忽略外部存储错误
        read_str = prefix_str + "." + id + "." + kJoinStorageTag + "." + kIgnoreStoreError;
        expect_cnt++;
        std::string ignore_store_error_str;
        if (dstream::error::OK > conf.GetValue(read_str, &ignore_store_error_str)) {
            ignore_store_error_str = (kDefaultIgnoreStoreError ? "true" : "false");
            JOINER_WARN("[%s] read '%s' fail, set to kDefaultIgnoreStoreError = %d.",
                        __FUNCTION__, kIgnoreStoreError, kDefaultIgnoreStoreError);
            fail_cnt++;
        }
        m_ignore_store_error = ("true" == ignore_store_error_str);

        // 是否使用批量插入方式
        read_str = prefix_str + "." + id + "." + kJoinStorageTag  + "." + kStoreBatchInsert;
        expect_cnt++;
        std::string store_batch_insert_str;
        if (dstream::error::OK > conf.GetValue(read_str, &store_batch_insert_str)) {
            store_batch_insert_str = (kDefaultStoreBatchInsert ? "true" : "false");
            JOINER_WARN("[%s] read '%s' fail, set to kDefaultStoreBatchInsert = %d.",
                        __FUNCTION__, kStoreBatchInsert, kDefaultStoreBatchInsert);
            fail_cnt++;
        }
        m_batch_insert = ("true" == store_batch_insert_str);

        if (m_batch_insert) {
            // 批量插入的数量
            read_str = prefix_str + "." + id + "." + kJoinStorageTag  + "." + kStoreBatchCount;
            expect_cnt++;
            if (dstream::error::OK > conf.GetIntValue(read_str, &m_batch_count)) {
                m_batch_count = kDefaultStoreBatchCount;
                JOINER_WARN("[%s] read '%s' fail, set to kDefaultStoreBatchCount = %d.",
                            __FUNCTION__, kStoreBatchCount, kDefaultStoreBatchCount);
                fail_cnt++;
            } else {
                JOINER_DEBUG("[%s] read '%s' = %u.", __FUNCTION__, kStoreBatchCount, m_batch_count);
            }

            // 批量插入时间间隔
            read_str = prefix_str + "." + id + "." + kJoinStorageTag  + "." + kStoreBatchIntervalMs;
            expect_cnt++;
            if (dstream::error::OK > conf.GetIntValue(read_str, &m_batch_interval_in_ms)) {
                m_batch_interval_in_ms = kDefaultStoreBatchIntervalMs;
                JOINER_WARN("[%s] read '%s' fail, set to kDefaultStoreBatchIntervalMs = %d.",
                            __FUNCTION__, kStoreBatchIntervalMs, kDefaultStoreBatchIntervalMs);
                fail_cnt++;
            } else {
                JOINER_DEBUG("[%s] read '%s' = %u.",
                        __FUNCTION__, kStoreBatchIntervalMs, m_batch_interval_in_ms);
            }
        }
    }

    return (fail_cnt < expect_cnt);
}

MultiKeyJoiner::ErrorCode MultiKeyJoiner::Initialize(const dstream::config::Config& conf,
        const std::string& id,
        const std::string& name,
        storage::StoreAccessor* store_accessor) {
    if (m_inited) {
        return error::OK;
    }

    m_t_table = new (std::nothrow) TimeWindow();
    if (NULL == m_t_table) {
        JOINER_ERROR("[%s] fail to alloc time window.", __FUNCTION__);
        return error::JOINER_ALLOC_WINDOW_FAIL;
    }

    return Init(conf, id, name, m_t_table, store_accessor);
}

MultiKeyJoiner::ErrorCode MultiKeyJoiner::Init(const dstream::config::Config& conf,
        const std::string& id,
        const std::string& name,
        TimeWindow* time_table,
        storage::StoreAccessor* store_accessor) {
    // 读取配置文件
    if (!CheckAndReadConfig(conf, name)) {
        JOINER_ERROR("[%s] fail to read config item(%s).", __FUNCTION__, name.c_str());
        return error::JOINER_ID_NOT_EXIST;
    }

    // 设置外部存储
    if (m_restore_from_store) {
        if (NULL == store_accessor) {
            JOINER_WARN("[%s] cannot get store accessor, init without store", __FUNCTION__);
            m_restore_from_store = false;
        } else {
            m_store_accessor = store_accessor;
        }
    }

    // 注册回调函数
    if (time_table) {
        time_table->SetWindowCreatingCallback(OnWindowCreatingCallback, this);
        time_table->SetWindowSlidingOutCallback(OnWindowSlidingOutCallback, this, true);
    } else {
        JOINER_WARN("[%s] time table is NULL, fail to set callback.", __FUNCTION__);
    }

    // 确认时间窗口创建成功
    // 初始化窗口
    if (time_table) {
        int ret = 0;
        if (0 != time_table->Init(m_step_in_sec, m_window_size_by_step)) {
            JOINER_ERROR("[%s] fail to init TimeWindow.", __FUNCTION__);
            return error::JOINER_WINDOW_INIT_ERROR;
        }
        // 是否使用外部存储
        if (m_join_with_store && NULL != store_accessor) {
            ret = time_table->SetStore(store_accessor,
                    id, m_restore_from_store, m_ignore_store_error, storage::LIST_TABLE);
            if (0 != ret) {
                JOINER_ERROR("[%s] fail to init window Store, error(%d).", __FUNCTION__, ret);
                return error::JOINER_WINDOW_STORE_ERROR;
            }
        } else {
            JOINER_WARN("[%s] time window init without Store, store_accessor(%p).",
                        __FUNCTION__, store_accessor);
        }
    }

    m_id = id;
    m_name = name;
    m_inited = true;

    return error::OK;
}

void MultiKeyJoiner::Destroy() {
    // 清理工作
    if (m_t_table) {
        delete m_t_table;
    }
    m_t_table = NULL;

    m_store_accessor = NULL;
    m_timeout_call_back = NULL;
    m_timeout_context = NULL;

    m_inited = false;
}

MultiKeyJoiner::ErrorCode MultiKeyJoiner::Join(const KeyType& keys,
        bool is_reserve,
        std::vector<DataType>* join_values) {
    if (!m_inited) {
        return error::JOINER_UNINIT;
    }
    if (NULL == join_values) {
        return error::JOINER_ARGUMENT_INVALID;
    }

    // 清空可能传入的数据
    join_values->clear();

    int ret = 0;

    KVTablePtr t;
    uint32_t ts = 0;
    // 遍历时间窗口中的所有元素
    for (TimedSlidingWindowsIterator<KVTable> iter(*m_t_table);
            iter.GetCurrent(&t, &ts);
            iter.Next(), ts = 0) {
        // assert(t && 0 != ts);
        // skip NULL window
        if (NULL == t) {
            continue;
        }

        // 多值join，每次join返回的结果可能为多个
        std::vector<DataType> t_value_list;
        ret = t->Find(keys, &t_value_list);
        if (ret < 0) { // find error
            JOINER_ERROR("[%s] find from window@ts(%u) fail error(%d).", __FUNCTION__, ts, ret);
            continue;
        }
        if (0 == ret) {
            // no data found
            continue;
        }
        if (!is_reserve) {
            // 删除窗口中的数据
            ret = t->Remove(keys);
            if (0 != ret) {
                JOINER_ERROR("[%s] remove data from window@ts(%u) fail error(%d).",
                        __FUNCTION__, ts, ret);
            }
        }
        std::vector<DataType>::const_iterator data_iter;
        for (data_iter = t_value_list.begin();
                data_iter != t_value_list.end();
                ++data_iter) {
            join_values->push_back(*data_iter);
        }
    }

    if (0 == join_values->size()) {
        return error::JOINER_KEY_NOT_FOUND;
    }
    return error::OK;
}

MultiKeyJoiner::ErrorCode MultiKeyJoiner::Record(uint32_t tuple_time,
        bool replace,
        const KeyType& keys,
        const DataType& value) {
    if (!m_inited) {
        return error::JOINER_UNINIT;
    }
    int ret = 0;

    // 获取特定时间的数据块
    KVTablePtr t;
    ret = m_t_table->GetWindow(tuple_time, &t);
    if ((0 != ret) || NULL == t) {
        JOINER_WARN("[%s] cannot get window @time(%u) error(%d).", __FUNCTION__, tuple_time, ret);
        return error::JOINER_GET_WINDOW_FAIL;
    }
    if (replace) {
        // 需要替换已有数据，删除之前的数据
        ret = t->Remove(keys);
        if (0 != ret) {
            JOINER_ERROR("[%s] remove data from window@ts(%u) fail error(%d).",
                    __FUNCTION__, tuple_time, ret);
        }
    }
    // 向数据块中插入数据
    ret = t->Insert(keys, value);
    if (0 != ret) {
        JOINER_WARN("[%s] insert data of time(%u) fail, error(%d).", __FUNCTION__, tuple_time, ret);
        return error::JOINER_RECORD_DATA_FAIL;
    }

    return error::OK;
}

int MultiKeyJoiner::OnWindowCreatingCallback(MultiKeyJoiner::KVTablePtr window_data,
        uint32_t ts,
        storage::IStoreTablePtr store_table,
        void* context) {
    int ret = 0;
    JOINER_INFO("[%s] create window @ts(%u).", __FUNCTION__, ts);
    if (NULL == context) {
        JOINER_WARN("[%s] joiner ptr is NULL.", __FUNCTION__);
        return -1;
    }
    MultiKeyJoiner* joiner_this = reinterpret_cast<MultiKeyJoiner*>(context);
    if (NULL == window_data) {
        JOINER_WARN("[%s] window data is NULL.", __FUNCTION__);
        return ret;
    }

    if (joiner_this->m_join_with_store) {
        // 初始化KVTable
        ret = window_data->SetStore(store_table,
                joiner_this->m_restore_from_store,
                joiner_this->m_ignore_store_error,
                joiner_this->m_batch_insert,
                joiner_this->m_batch_count,
                joiner_this->m_batch_interval_in_ms);
        if (0 != ret) {
            JOINER_ERROR("[%s] init kv table fail, error(%d).", __FUNCTION__, ret);
        }
    }
    return ret;
}

int MultiKeyJoiner::OnWindowSlidingOutCallback(MultiKeyJoiner::KVTablePtr window_data,
        uint32_t ts,
        void* context) {
    int ret = 0;
    JOINER_INFO("[%s] window @ts(%u) timeout.", __FUNCTION__, ts);
    if (NULL == context) {
        JOINER_WARN("[%s] joiner ptr is NULL.", __FUNCTION__);
        return -1;
    }
    MultiKeyJoiner* joiner_this = reinterpret_cast<MultiKeyJoiner*>(context);
    // 获取window_data中的所有数据 & 调用DataTimeOutCallBack
    if (NULL == window_data) {
        JOINER_WARN("[%s] window data is NULL.", __FUNCTION__);
        return ret;
    }
    KeyType k;
    DataType v;
    for (KVTableIterator iter(*window_data);
         iter.GetCurrent(&k, &v);
         iter.Next()) {
        // 调用DataTimeOutCallBack
        if (joiner_this->m_timeout_call_back) {
            JOINER_DEBUG("[%s] call user define callback.", __FUNCTION__);
            (*(joiner_this->m_timeout_call_back))(k, v, joiner_this->m_timeout_context);
        }
    }
    return ret;
}

void MultiKeyJoiner::RegisterDataTimeOutCallBack(DataTimeOutCallBack call_back, void* context) {
    m_timeout_call_back = call_back;
    m_timeout_context = context;
}
} // namespace generic_operator
} // namespace dstream


