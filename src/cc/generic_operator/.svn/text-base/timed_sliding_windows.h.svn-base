/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
/**
 * @file timed_sliding_windows.h
 * @author luxing
 * @date 2012/09/24
 * @brief 定义流式计算中时间驱动的滑动窗口
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TIMED_SLIDING_WINDOWS_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_TIMED_SLIDING_WINDOWS_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <string>
#include <utility>
#include <vector>

#include "common/logger.h"
#include "common/utils.h"
#include "storage/storage_accessor.h"

namespace dstream {
namespace generic_operator {

/**
 * @brief TimedSlidingWindows模版类，表示流式计算中时间驱动的滑动窗口
 * 滑动窗口的主要参数包括滑动的步长和总窗口大小
 * 滑动步长以秒为单位表示，总窗口大小是滑动步长的整数倍
 * 每个滑动的步长定义为一个“小窗口”（window），其中用类型T保存和处理数据
 * 随着新数据的到达，会驱动窗口滑动，不断有新的窗口创建和旧的窗口滑出
 * 新窗口创建和滑出时会调用用户自定义的回调函数
 *
 * 滑动窗口中集成了外部存储，使用户可以通过外部存储持久化窗口中的数据
 *
 * @param T 模版参数，window中用于保存和处理数据的类类型
 */
template<class T>
class TimedSlidingWindows {
public:
    template<class> friend class TimedSlidingWindowsIterator;
    typedef boost::shared_ptr<T> WindowDataPtr;

    // 定义返回的错误码
    enum ErrorCode {
        OK = 0,
        ARGUMENT_INVALID = -1,
        OUT_OF_MEMORY = -2,
        STORE_ERROR = -3,
        STORE_DATA_ERROR = -4,
        CALLBACK_ERROR = -5
    };

    /**
     * @brief 定义window被创建时的回调函数的prototype
     *        为同步回调
     *
     * @param window_data_ptr[in] 被创建的窗口数据的智能指针
     * @param ts[in] 被创建的窗口的时间戳，时间戳为窗口的起始时间
     * @param store_table[in] 与新创建窗口相关联的外部存储表的指针
     *        几种情况：
     *        1. 不使用外部存储时，则为NULL
     *        2. 使用外部存储时，为窗口对应外部存储表的指针
     *           1) 若外部存储中已有对应的表，则使用此表，可能需要恢复数据
     *           2) 否则，则在外部存储中创建新表
     *           3) 若访问外部存储读取旧表或创建新表失败，则为NULL
     * @param context[in] 返回用户传入的context指针
     *
     * @return 0 调用成功
     *         其他 调用失败的错误码
     */
    typedef int (*WindowCreatingCallback)(WindowDataPtr window_data_ptr,
            uint32_t ts,
            storage::IStoreTablePtr store_table,
            void* context);

    /**
     * @brief 定义window滑出时的回调函数的prototype
     *        根据初始化时的配置，可以为同步回调或异步回调
     *
     * @param window_data_ptr[in] 滑出的时间窗的数据的智能指针
     * @param ts[in] 滑出的时间窗的时间戳，为窗口的起始时间
     * @param context[in] 返回用户传入的context指针
     *
     * @return 0 调用成功
     *         其他 调用失败的错误码
     *
     */
    typedef int (*WindowSlidingOutCallback)(WindowDataPtr window_data_ptr,
            uint32_t ts,
            void* context);

    TimedSlidingWindows();
    virtual ~TimedSlidingWindows();

    /**
     * @brief 初始化函数
     *
     * @param step[in] 滑动窗口的步长
     * @param windows_size_by_step[in] 滑动窗口的总大小，为步长的倍数
     *
     * @return OK 初始化成功
     *         其他 出错的错误码
     */
    virtual int Init(uint32_t step, uint32_t windows_size_by_step);

    /**
     * @brief 设置访问外部存储
     *
     * @param store_accessor[in] 访问外部存储的指针，若为NULL则不使用外部存储
     * @param table_name_in_store[in] 整体时间窗在外部存储中对应的表的名字
     * @param restore_from_store[in] 是否从外部存储中恢复数据
     * @param ignore_store_error[in] 是否忽略外部存储的错误
     * @param window_data_store_type[in] 小窗口数据在外部存储中对应的表的类型
     *
     * @return OK 初始化成功
     *         其他 出错的错误码
     */
    virtual int SetStore(storage::StoreAccessor* store_accessor,
            const std::string& table_name_in_store,
            bool restore_from_store,
            bool ignore_store_error,
            storage::StoreTableType window_data_store_type);

    /**
     * @brief 设置窗口被创建时的回调函数
     * @param creating_callback[in] 窗口被创建时的回调函数的指针
     * @param context[in] 用户自定义的回调时传回的context指针
     */
    virtual void SetWindowCreatingCallback(WindowCreatingCallback creating_callback,
            void* context);

    /**
     * @brief 设置窗口移出时的回调函数
     * @param sliding_out_callback[in] 窗口滑出时的回调函数的指针
     * @param context[in] 用户自定义的回调时传回的context指针
     * @param async[in] 是否为异步回调
     */
    virtual void SetWindowSlidingOutCallback(WindowSlidingOutCallback sliding_out_callback,
            void* context,
            bool async);

    /**
     * @brief 获取输入时间所属的窗口
     *        分三种情况，假设当前总窗口的时间范围为 [t1,t2) ：
     *        1. 若 t1 <= time < t2，则返回已创建的time所属的窗口
     *        2. 若 time >= t2，则驱动窗口滑动，同时创建新的窗口，并返回time所属的窗口
     *           新窗口创建时会同步地调用相应的回调函数
     *           旧窗口滑出时会同步或者异步地调用相应的回调函数，可能有多个窗口被滑出
     *        3. 若 time < t1，则返回NULL
     *
     * @param time[in] 输入时间
     * @param window_data_ptr[out] 获取的窗口数据的智能指针
     *
     * @return OK 初始化成功
     *         其他 出错的错误码
     */
    virtual int GetWindow(uint32_t time, WindowDataPtr* window_data_ptr);

    /**
     * @brief 遍历外部存储表恢复数据时的回调，每读到一条数据会触发一次回调
     *
     * @param ctx[in] 传入的context指针
     * @param err[in] 读数据时的错误码
     * @param key[in] 读到的key
     * @param value[in] 读到的value
     *
     * @return 0 成功
     *         其他 出现错误的错误码
     */
    static int RestoreRecordCallback(void* ctx, int err, const std::string& key,
            const std::string& value);

private:
    // 获得指定时间戳对应窗口的位置，调用可能会slide out旧窗口
    uint32_t GetWindowPosition(uint32_t time);

    // 在指定位置创建窗口，同时调用创建窗口时的回调函数
    int CreateWindow(uint32_t window_idx, uint32_t ts, storage::IStoreTablePtr window_data_table);

    // 移出指定位置的窗口，同时调用被移出窗口的回调函数
    int SlideOutWindow(uint32_t window_idx, uint32_t ts);

    // 从外部存储中恢复小窗口数据
    int RestoreWindowData(const std::string& ts_str, const std::string& table_name);

    // 从外部存储中删除key和table
    int DeleteKeyAndTableFromStore(const std::string& key, const std::string& table_name);

    typedef std::vector<WindowDataPtr> DataVector;
    typedef std::vector<storage::IStoreTablePtr> DataStoreTableVector;

    // 窗口数据保存在数组中，滑动时仅移动窗口起始位置的标记
    DataVector m_data_vector;
    // 与窗口数据对应的外部存储表的指针的数据
    DataStoreTableVector m_data_store_table_vector;
    // 步长
    uint32_t m_step;
    // 总窗口大小，为步长的倍数
    uint32_t m_windows_size_by_step;
    // 总窗口的起始时间戳
    uint32_t m_start_ts;
    // 起始窗口在数组中的位置
    uint32_t m_start_idx;

    // 窗口被创建时的回调函数
    WindowCreatingCallback m_creating_callback;
    // 创建回调函数的context指针
    void* m_creating_callback_context;
    // 窗口滑出时的回调函数
    WindowSlidingOutCallback m_sliding_out_callback;
    // 滑出回调函数的context指针
    void* m_sliding_out_callback_context;
    // 是否异步回调，只对滑出回调有效，创建回调始终为同步
    bool m_async_callback;

    // 访问外部存储的指针，若为NULL则不使用外部存储
    storage::StoreAccessor* m_store_accessor;
    // 外部存储中总窗口对应的表的名字，小窗口对应的表将使用这个名字作前缀
    std::string m_table_name_in_store;
    // 是否忽略外部存储的错误
    bool m_ignore_store_error;

    // 小窗口数据在外部存储中对应的表的类型
    storage::StoreTableType m_window_data_store_type;

    // 在外部存储中对应的哈希表，用来存储从时间戳key到小窗口对应表名的映射
    boost::shared_ptr<storage::IHashTable> m_store_hash_table;

    // 从外部存储遍历表时获取的数据，用于数据恢复
    std::vector<std::pair<std::string, std::string> > m_store_datas;
};


/**
 * @brief TimedSlidingWindowsIterator模版类
 *        用于遍历TimedSlidingWindows中的所有窗口的迭代器
 *
 * @param T 模版参数，window中用于保存和处理数据的类类型
 *
 * @note 遍历过程中不能对所遍历的TimedSlidingWindows对象进行其他操作
 */
template<class T>
class TimedSlidingWindowsIterator {
public:
    /**
     * @brief 构造函数
     *
     * @param windows[in] 需要遍历的TimedSlidingWindows的引用
     */
    explicit TimedSlidingWindowsIterator(const TimedSlidingWindows<T>& windows);

    /**
     * @brief 获取当前窗口的数据
     *
     * @param window_data_ptr[out] 返回当前窗口数据的智能指针
     * @param ts[out] 返回当前窗口的时间戳
     *
     * @return true表示遍历尚未结束
     *         false表示已遍历完最后的窗口，遍历结束
     */
    bool GetCurrent(typename TimedSlidingWindows<T>::WindowDataPtr* window_data_ptr, uint32_t* ts);

    /**
     * @brief iterator顺移
     */
    void Next();

private:
    // 需要遍历的TimedSlidingWindows的指针
    const TimedSlidingWindows<T>* windows_;
    // 当前遍历到的窗口位置
    uint32_t cur_idx_;
    // 当前遍历到的窗口的时间戳
    uint32_t cur_ts_;
    // 尚需要遍历的窗口的数目
    uint32_t left_;
};


template<class T>
TimedSlidingWindows<T>::TimedSlidingWindows()
        : m_step(0),
        m_windows_size_by_step(0),
        m_start_ts(0),
        m_start_idx(0),
        m_creating_callback(NULL),
        m_creating_callback_context(NULL),
        m_sliding_out_callback(NULL),
        m_sliding_out_callback_context(NULL),
        m_async_callback(false),
        m_store_accessor(NULL),
        m_ignore_store_error(true) {
}

template<class T>
TimedSlidingWindows<T>::~TimedSlidingWindows() {
}

template<class T>
int TimedSlidingWindows<T>::Init(uint32_t step, uint32_t windows_size_by_step) {
    if (0 == step || 0 == windows_size_by_step) {
        return ARGUMENT_INVALID;
    }
    m_step = step;
    m_windows_size_by_step = windows_size_by_step;
    m_start_idx = windows_size_by_step;
    m_data_vector.resize(windows_size_by_step);
    m_data_store_table_vector.resize(windows_size_by_step);
    return OK;
}

template<class T>
int TimedSlidingWindows<T>::SetStore(storage::StoreAccessor* store_accessor,
        const std::string& table_name_in_store,
        bool restore_from_store,
        bool ignore_store_error,
        storage::StoreTableType window_data_store_type) {
    if (NULL == store_accessor) {
        DSTREAM_ERROR("bad parameter store_accessor=NULL");
        return ARGUMENT_INVALID;
    }
    m_store_accessor = store_accessor;
    m_table_name_in_store = table_name_in_store;
    m_ignore_store_error = ignore_store_error;
    m_window_data_store_type = window_data_store_type;

    // 初始化外部存储，如果需要，从外部存储中恢复数据
    bool exist = false;
    int store_ret = store_accessor->TestTableExist(table_name_in_store, &exist);
    if (storage::OK != store_ret) {
        DSTREAM_ERROR("failed to access store to test table [%s] exist, err: %d",
                      table_name_in_store.c_str(),
                      store_ret);
        return STORE_ERROR;
    }
    storage::IStoreTablePtr store_table;
    if (exist) {
        DSTREAM_INFO("table [%s] exists.", table_name_in_store.c_str());
        store_ret = store_accessor->GetTable(table_name_in_store, &store_table);
        if (storage::OK != store_ret || store_table == NULL) {
            DSTREAM_ERROR("failed to get table [%s] from store, err: %d",
                          table_name_in_store.c_str(),
                          store_ret);
            return STORE_ERROR;
        }
        if (storage::HASH_TABLE != store_table->type()) {
            // existing table's type is not as expected, delete the table
            DSTREAM_WARN("existing table [%s]'s type [%d] is not hash, "
                    "will delete the table and create a new one",
                    table_name_in_store.c_str(), store_table->type());
            // delete the table from store
            store_ret = store_accessor->DeleteTable(table_name_in_store);
            if (storage::OK != store_ret) {
                DSTREAM_WARN("delete table [%s] from store failed, err: %d",
                             table_name_in_store.c_str(),
                             store_ret);
                return STORE_ERROR;
            }
            exist = false;
        }
    }
    if (!exist) {
        // create the table if it does not exist
        store_ret = store_accessor->CreateTable(table_name_in_store, storage::HASH_TABLE, true,
                                                &store_table);
        if (storage::OK != store_ret) {
            DSTREAM_ERROR("failed to create table [%s], err: %d",
                          table_name_in_store.c_str(),
                          store_ret);
            return STORE_ERROR;
        }
        DSTREAM_INFO("created table [%s]", table_name_in_store.c_str());
    }
    m_store_hash_table = boost::dynamic_pointer_cast<storage::IHashTable>(store_table);
    // assert store accessor bug
    assert(NULL != m_store_hash_table);
    if (exist && restore_from_store) {
        // restore data
        DSTREAM_INFO("will restore data from table [%s]", table_name_in_store.c_str());
        m_store_datas.clear();
        int store_ret = m_store_hash_table->GetAll(RestoreRecordCallback, this);
        if (storage::OK != store_ret) {
            DSTREAM_ERROR("get all from table [%s] failed, err: %d",
                          table_name_in_store.c_str(),
                          store_ret);
            return STORE_ERROR;
        }
        for (size_t i = 0; i < m_store_datas.size(); ++i) {
            int err_ret = RestoreWindowData(m_store_datas[i].first, m_store_datas[i].second);
            if (OK != err_ret) {
                DSTREAM_WARN("failed to restore window data from [%s]:[%s], err: %d",
                             m_store_datas[i].first.c_str(),
                             m_store_datas[i].second.c_str(),
                             err_ret);
                if (m_ignore_store_error) {
                    continue;
                } else {
                    return err_ret;
                }
            }
        }
        DSTREAM_INFO("data restored from table [%s]", table_name_in_store.c_str());
    }

    return OK;
}

template<class T>
int TimedSlidingWindows<T>::RestoreRecordCallback(void* ctx,
        int err,
        const std::string& key,
        const std::string& value) {
    TimedSlidingWindows<T>* p_this = reinterpret_cast<TimedSlidingWindows<T>*>(ctx);
    assert(NULL != p_this);
    if (err != storage::OK) {
        DSTREAM_WARN("ignore bad data: %d", err);
        return err;
    }
    p_this->m_store_datas.push_back(std::make_pair(key, value));
    return storage::OK;
}

template<class T>
int TimedSlidingWindows<T>::RestoreWindowData(const std::string& ts_str,
        const std::string& table_name) {
    char* endptr = NULL;
    uint32_t ts = strtoul(ts_str.c_str(), &endptr, 10);
    if (ts_str.size() == 0 || endptr == NULL || *endptr != '\0') {
        DSTREAM_WARN("failed to parse timestamp from [%s]", ts_str.c_str());
        // delete the illegal key and its associated table
        return DeleteKeyAndTableFromStore(ts_str, table_name);
    }
    uint32_t window_idx = GetWindowPosition(ts);
    if (window_idx >= m_windows_size_by_step || NULL != m_data_vector[window_idx]) {
        // the timestamp is too old, remove it from store
        DSTREAM_INFO("delete table [%s] from store, ts=%s, idx=%d",
                     table_name.c_str(),
                     ts_str.c_str(),
                     window_idx);
        return DeleteKeyAndTableFromStore(ts_str, table_name);
    } else {
        bool exist = false;
        int store_ret = m_store_accessor->TestTableExist(table_name, &exist);
        if (storage::OK != store_ret) {
            DSTREAM_WARN("failed to test table exist for [%s], err: %d",
                    table_name.c_str(), store_ret);
            return STORE_ERROR;
        }
        if (!exist) {
            // delete the key without associated window data table
            store_ret = m_store_hash_table->Erase(ts_str);
            if (storage::OK != store_ret) {
                DSTREAM_WARN("failed to erase key [%s] from [%s], err: %d",
                             ts_str.c_str(),
                             m_table_name_in_store.c_str(),
                             store_ret);
                return STORE_ERROR;
            }
            return OK;
        }
        storage::IStoreTablePtr window_data_table;
        store_ret = m_store_accessor->GetTable(table_name, &window_data_table);
        if (storage::OK != store_ret) {
            DSTREAM_WARN("failed to get table [%s], err: %d", table_name.c_str(), store_ret);
            return STORE_ERROR;
        }
        // now we have got the window data's table
        int err_ret = CreateWindow(window_idx, ts, window_data_table);
        if (OK != err_ret) {
            DSTREAM_WARN("failed to create window, err: %d", err_ret);
            return err_ret;
        }
    }
    return OK;
}

template<class T>
int TimedSlidingWindows<T>::DeleteKeyAndTableFromStore(const std::string& key,
        const std::string& table_name) {
    assert(NULL != m_store_accessor);
    int store_ret = m_store_accessor->DeleteTable(table_name);
    if (storage::OK != store_ret) {
        DSTREAM_WARN("failed to delete table [%s] from store, err: %d",
                table_name.c_str(), store_ret);
        return STORE_ERROR;
    }
    assert(NULL != m_store_hash_table);
    store_ret = m_store_hash_table->Erase(key);
    if (storage::OK != store_ret) {
        DSTREAM_WARN("failed to erase key [%s] from [%s], err: %d", key.c_str(),
                     m_table_name_in_store.c_str(), store_ret);
        return STORE_ERROR;
    }
    return OK;
}

template<class T>
void TimedSlidingWindows<T>::SetWindowCreatingCallback(WindowCreatingCallback creating_callback,
        void* context) {
    m_creating_callback = creating_callback;
    m_creating_callback_context = context;
}

template<class T>
void TimedSlidingWindows<T>::SetWindowSlidingOutCallback(WindowSlidingOutCallback
        sliding_out_callback,
        void* context,
        bool async) {
    m_sliding_out_callback = sliding_out_callback;
    m_sliding_out_callback_context = context;
    m_async_callback = async;
}

template<class T>
int TimedSlidingWindows<T>::GetWindow(uint32_t time, WindowDataPtr* window_data_ptr) {
    if (0 == m_windows_size_by_step || 0 == m_step || NULL == window_data_ptr) {
        // wrong windows
        return ARGUMENT_INVALID;
    }

    uint32_t window_idx = GetWindowPosition(time);
    if (window_idx >= m_windows_size_by_step) {
        window_data_ptr->reset();
        return OK;
    } else {
        if (NULL == m_data_vector[window_idx]) {
            int err_ret = CreateWindow(window_idx, time - time % m_step, storage::IStoreTablePtr());
            if (OK != err_ret) {
                return err_ret;
            }
        }
        *window_data_ptr = m_data_vector[window_idx];
        return OK;
    }
}

template<class T>
uint32_t TimedSlidingWindows<T>::GetWindowPosition(uint32_t time) {
    uint32_t ret_idx = m_windows_size_by_step;
    if (m_start_idx >= m_windows_size_by_step) {
        // no window has been created, need to create a new window
        // create the new window at the end, to avoid data lost caused by mis-ordering
        m_start_idx = 0;
        if (time < m_step * (m_windows_size_by_step - 1)) {
            ret_idx = time / m_step;
        } else {
            ret_idx = m_windows_size_by_step - 1;
        }
        m_start_ts = time - m_step * ret_idx;
        m_start_ts -= m_start_ts % m_step;
    } else if (time < m_start_ts) {
        // time is too old, need to ignore it
        ret_idx = m_windows_size_by_step;
    } else if (time >= m_start_ts + m_step * m_windows_size_by_step) {
        // need to create new window and slide out old ones
        uint32_t slide_num = (time - m_start_ts) / m_step - m_windows_size_by_step + 1;
        if (slide_num > m_windows_size_by_step) {
            slide_num = m_windows_size_by_step;
        }
        for (uint32_t i = 0; i < slide_num; ++i) {
            // slide out one old window
            SlideOutWindow(m_start_idx, m_start_ts);
            m_data_vector[m_start_idx].reset();
            // set ret_idx and move m_start_idx to next
            m_start_ts += m_step;
            ret_idx = m_start_idx;
            ++m_start_idx;
            if (m_start_idx >= m_windows_size_by_step) {
                m_start_idx -= m_windows_size_by_step;
            }
        }
        // calculate and set m_start_ts
        m_start_ts = time - (m_step * (m_windows_size_by_step - 1));
        m_start_ts -= m_start_ts % m_step;
    } else {
        ret_idx = m_start_idx + (time - m_start_ts) / m_step;
        if (ret_idx >= m_windows_size_by_step) {
            ret_idx -= m_windows_size_by_step;
        }
    }
    return ret_idx;
}

template<class T>
int TimedSlidingWindows<T>::CreateWindow(uint32_t window_idx, uint32_t ts,
        storage::IStoreTablePtr window_data_table) {
    assert(NULL == m_data_vector[window_idx]);
    assert(NULL == m_data_store_table_vector[window_idx]);
    m_data_vector[window_idx].reset(new (std::nothrow) T());
    if (NULL == m_data_vector[window_idx]) {
        DSTREAM_ERROR("failed to allocate new memory");
        return OUT_OF_MEMORY;
    }

    // create table from store
    if (NULL != m_store_hash_table) {
        assert(NULL != m_store_accessor);
        std::string ts_str = NumberToString(ts);
        std::string data_table_name = m_table_name_in_store + "_" + ts_str;
        if (NULL == window_data_table) {
            // try to create new table
            bool store_error = false;
            do {
                // first, insert the ts and associated table name into store
                int store_ret = m_store_hash_table->Insert(ts_str, data_table_name);
                if (storage::OK != store_ret) {
                    DSTREAM_WARN("failed to insert key [%s] into store, err: %d",
                            ts_str.c_str(), store_ret);
                    store_error = false;
                    break;
                }
                // create a new table and replace table with the same name if it exists
                store_ret = m_store_accessor->CreateTable(data_table_name,
                        m_window_data_store_type, true, &window_data_table);
                if (storage::OK != store_ret) {
                    window_data_table.reset();
                    DSTREAM_WARN("failed to create table [%s], err: %d",
                            data_table_name.c_str(), store_ret);
                    store_error = false;
                    break;
                }
                DSTREAM_INFO("table [%s] created", data_table_name.c_str());
            } while (false);
            if (store_error && !m_ignore_store_error) {
                return STORE_ERROR;
            }
        }
        m_data_store_table_vector[window_idx] = window_data_table;
    }

    // invoke callback
    int ret = 0;
    if (NULL != m_creating_callback) {
        ret = m_creating_callback(m_data_vector[window_idx],
                ts, window_data_table, m_creating_callback_context);
        if (0 != ret) {
            DSTREAM_ERROR("creating callback failed, return: %d", ret);
            return CALLBACK_ERROR;
        }
    }

    return OK;
}

template<class T>
int TimedSlidingWindows<T>::SlideOutWindow(uint32_t window_idx, uint32_t ts) {
    // invoke callback
    int err_ret = OK;
    if (NULL != m_sliding_out_callback) {
        int ret = m_sliding_out_callback(m_data_vector[window_idx],
                ts, m_sliding_out_callback_context);
        if (0 != ret) {
            DSTREAM_ERROR("sliding out callback failed, return: %d", ret);
            err_ret = CALLBACK_ERROR;
        }
    }
    if (NULL != m_data_vector[window_idx]) {
        m_data_vector[window_idx].reset();
        // delete the corresponding table in store
        if (NULL != m_data_store_table_vector[window_idx]) {
            std::string table_name = m_data_store_table_vector[window_idx]->name();
            // release table first
            m_data_store_table_vector[window_idx].reset();
            // delete the table
            int store_ret = m_store_accessor->DeleteTable(table_name);
            if (storage::OK != store_ret) {
                DSTREAM_WARN("failed to delete table [%s], err: %d", table_name.c_str(), store_ret);
                err_ret = STORE_ERROR;
            }
            // delete the key from hash table
            std::string ts_str = NumberToString(ts);
            store_ret = m_store_hash_table->Erase(ts_str);
            if (storage::OK != store_ret) {
                DSTREAM_WARN("failed to erase key [%s] from [%s], err: %d", ts_str.c_str(),
                             m_table_name_in_store.c_str(), store_ret);
                err_ret = STORE_ERROR;
            }
            DSTREAM_INFO("table [%s] deleted", table_name.c_str());
        }
    }
    return err_ret;
}


template<class T>
TimedSlidingWindowsIterator<T>::TimedSlidingWindowsIterator(const TimedSlidingWindows<T>& windows) {
    windows_ = &windows;
    cur_idx_ = windows.m_start_idx;
    cur_ts_ = windows.m_start_ts;
    if (windows.m_start_idx >= windows.m_windows_size_by_step) {
        left_ = 0;
    } else {
        left_ = windows.m_windows_size_by_step;
    }
}

template<class T>
bool TimedSlidingWindowsIterator<T>::GetCurrent(typename TimedSlidingWindows<T>::WindowDataPtr*
        window_data_ptr,
        uint32_t* ts) {
    if (NULL == window_data_ptr || NULL == ts) {
        return false;
    }
    bool ret = false;
    if (left_ > 0) {
        *window_data_ptr = windows_->m_data_vector[cur_idx_];
        *ts = cur_ts_;
        ret = true;
    }
    return ret;
}

template<class T>
void TimedSlidingWindowsIterator<T>::Next() {
    if (left_ > 0) {
        ++cur_idx_;
        if (cur_idx_ >= windows_->m_windows_size_by_step) {
            cur_idx_ = 0;
        }
        cur_ts_ += windows_->m_step;
        --left_;
    }
}


} // namespace generic_operator
} // namespace dstream

#endif // __DSTREAM_GENERIC_OPERATOR_TIMED_SLIDING_WINDOWS_H__ NOLINT
