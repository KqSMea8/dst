/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file single_key_joiner.h
 * @author lanbijia
 * @date 2012/9/22
 * @brief 定义Joiner类，实现Join操作
 *        1. 通过Record接口将数据记录在时间窗口中（时间窗口可以配置使用外部存储）；
 *        2. 通过Join接口将外部数据与时间窗口中的数据进行join操作；
 *        3. 通过RegisterDataTimeOutCallBack接口注册回调函数，用于时间窗口中的数据过期时的回调处理
 *           （目前实现的是同步回调）；
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_SINGLE_KEY_JOINER_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_SINGLE_KEY_JOINER_H__ // NOLINT

#include <string>
#include "generic_operator/join_table.h"
#include "generic_operator/joiner_context.h"

namespace dstream {
namespace generic_operator {


/**
 * @brief 单值joiner
 */
class SingleKeyJoiner : public JoinerContext {
public:
    typedef dstream::generic_operator::error::Code ErrorCode;
    typedef void (*DataTimeOutCallBack)(const KeyType&, const DataType&, void*);

    SingleKeyJoiner();
    virtual ~SingleKeyJoiner();

    /**
     * @brief 调用Init函数初始化Joiner
     *
     * @param conf [in] 初始化使用的配置
     * @param id [in] 为joiner分配的id，用于从外部存储中存储和恢复数据
     * @param store_accessor [in] 用于访问外部存储
     * @return ErrorCode 初始化是否成功
     *            error::OK 成功
     */
    virtual ErrorCode Initialize(const dstream::config::Config& conf,
            const std::string& id,
            const std::string& name,
            storage::StoreAccessor* store_accessor);

    /**
     * @brief 销毁
     *
     * 1. 向外部存储中dump数据
     * 2. 销毁内部结构
     */
    virtual void Destroy();

    /**
     * @brief 同步join函数，当需要join数据时调用
     *
     * @param keys [in] join数据时使用的key集合（用户指定）
     * @param is_reserve[in] 是否保留窗口中的数据
     * @param join_value [out] 如果join成功，返回join成功的tuple（最多仅有一个tuple）
     * @return ErrorCode 返回join的结果
     *         error::OK 成功
     *         error::JOINER_KEY_NOT_FOUND 没有找到join的Key
     *         error::UNINIT 未初始化
     */
    virtual ErrorCode Join(const KeyType& keys, bool is_reserve, DataType* join_value);

    /**
     * @brief 同步数据插入函数，当需要将join数据保存入时间窗时调用
     *
     * @param time [in] 插入数据的时间
     * @param replace [in] 是否替换原有数据
     * @param keys [in] 保存数据时使用的key集合（用户指定）
     * @param tuple [in] 输入需要保存的tuple（需由用户保证限定为某一类tuple）
     * @return ErrorCode 返回join的结果
     *         error::OK 成功
     *         error::JOINER_GET_WINDOW_FAIL 获取时间窗口失败
     *         error::JOINER_RECORD_DATA_FAIL 插入时间窗失败
     */
    virtual ErrorCode Record(uint32_t time,
            bool replace, const KeyType& keys, const DataType& value);

    /**
     * @brief 用户注册数据过期时的回调函数，其原型为DataTimeOutCallBack
     *
     * @param call_back [in] DataTimeOutCallBack回调函数指针
     */
    virtual void RegisterDataTimeOutCallBack(DataTimeOutCallBack call_back, void* context);

protected:
    typedef JoinTable<KeyType, DataType> KVTable;
    typedef boost::shared_ptr<KVTable> KVTablePtr;
    typedef JoinTableIterator<KeyType, DataType> KVTableIterator;
    typedef TimedSlidingWindows<KVTable> TimeWindow;

    // 时间窗口指针
    TimeWindow* m_t_table;
    // 外部存储访问器指针
    storage::StoreAccessor* m_store_accessor;
    // 数据超时回调函数
    DataTimeOutCallBack m_timeout_call_back;
    // 回调函数使用的上下文指针
    void* m_timeout_context;

    /**
     * @brief 读取配置文件中的相关配置
     *
     * @param conf [in] 配置文件操作对象
     * @param id [in] joiner对应的配置项id
     * @return bool 读取成功返回true，否则false
     */
    bool CheckAndReadConfig(const dstream::config::Config& conf, const std::string& id);

    /**
     * @brief 初始化：
     * 1. 初始化内部结构
     * 2. 从外部存储中恢复数据（如果外部存储中有需要恢复的数据）
     *
     * @param conf [in] 初始化使用的配置
     * @param id [in] 为joiner分配的id，用于从外部存储中存储和恢复数据
     * @param time_table [in] 时间窗口指针
     * @param store_accessor [in] 用于访问外部存储
     * @return 初始化是否成功
     */
    ErrorCode Init(const dstream::config::Config& conf,
            const std::string& id,
            const std::string& name,
            TimeWindow* time_table,
            storage::StoreAccessor* store_accessor);

    /**
     * @brief 定义window被创建时的回调函数
     *
     * @param window_data[in] 被创建的窗口数据的指针
     * @param ts[in] 被创建的窗口的时间戳，时间戳为窗口的起始时间
     * @param store_table[in] 与新创建窗口相关联的外部存储表的指针
     * @param context[in] 记录Joiner this指针
     */
    static int OnWindowCreatingCallback(KVTablePtr window_data,
            uint32_t ts,
            storage::IStoreTablePtr store_table,
            void* context);

    /**
     * @brief 定义window滑出时的回调函数的prototype
     *        根据初始化时的配置，可以为同步回调或异步回调
     *
     * @param window_data[in] 滑出的时间窗的数据
     * @param ts[in] 滑出的时间窗的时间戳，为窗口的起始时间
     * @param context[in] 记录Joiner this指针
     */
    static int OnWindowSlidingOutCallback(KVTablePtr window_data,
            uint32_t ts,
            void* context);

private:
    // Joiner唯一标识（用于外部存储建表保存数据）
    std::string m_id;
    // Joiner名称(多个joiner并发名称相同)
    std::string m_name;
    // 初始化标识
    bool m_inited;
    // 窗口步长
    int32_t m_step_in_sec;
    // 窗口长度
    int32_t m_window_size_by_step;
    // 外部存储标识
    bool m_join_with_store;
    // 是否从外部存储恢复数据
    bool m_restore_from_store;
    // 是否忽略外部存储错误
    bool m_ignore_store_error;
    // 是否使用batch插入方式
    bool m_batch_insert;
    int32_t m_batch_count;
    int32_t m_batch_interval_in_ms;
};

} // namespace generic_operator
} // namespace dstream

#endif // NOLINT
