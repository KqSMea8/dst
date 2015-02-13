/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file joiner_context.h
 * @author lanbijia
 * @date 2012/11/5
 * @brief 定义Joiner类通用部分
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_JOINER_CONTEXT_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_JOINER_CONTEXT_H__ // NOLINT

// system
#include <sstream>
#include <string>
#include <vector>

#include "common/config.h"
#include "common/logger.h"
#include "generic_operator/error.h"
#include "generic_operator/timed_sliding_windows.h"
#include "processelement/tuple.h"


namespace dstream {
namespace generic_operator {

typedef std::string KeyType;
typedef std::string DataType;

#if 1
#define JOINER_DEBUG(fmt, ...) DSTREAM_DEBUG(fmt, ##__VA_ARGS__)
#define JOINER_INFO(fmt, ...) DSTREAM_INFO(fmt, ##__VA_ARGS__)
#define JOINER_WARN(fmt, ...) DSTREAM_WARN(fmt, ##__VA_ARGS__)
#define JOINER_ERROR(fmt, ...) DSTREAM_ERROR(fmt, ##__VA_ARGS__)
#define JOINER_FATAL(fmt, ...) do {\
    DSTREAM_ERROR(fmt, ##__VA_ARGS__); \
    abort(); \
} while (0)
#else
#define JOINER_DEBUG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define JOINER_INFO(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define JOINER_WARN(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define JOINER_ERROR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define JOINER_FATAL(fmt, ...) do { \
    printf(fmt, ##__VA_ARGS__); \
    abort(); \
} while (0)
#endif


class JoinerContext {
public:
    static const char* kJoinConfigRoot;
    static const char* kJoinConfigTag;
    static const char* kJoinStorageTag;
    // 滑动窗口的步长配置项名称
    static const char* kWindowStep;
    // 滑动窗口的总大小配置项名称
    static const char* kWindowSize;
    // 外部存储配置项名称
    static const char* kJoinWithStore;
    // 初始化是否从外部存储恢复
    static const char* kRestoreFromStore;
    // 是否忽略外部存储错误
    static const char* kIgnoreStoreError;
    // 是否使用批量插入方式
    static const char* kStoreBatchInsert;
    // 批量插入的数据大小
    static const char* kStoreBatchCount;
    // 批量插入的时间间隔
    static const char* kStoreBatchIntervalMs;
    // 滑动窗口的步长默认值
    static const int32_t kDefaultWindowStep;
    // 滑动窗口的总大小默认值
    static const int32_t kDefaultWindowSize;
    // 外部存储配置默认值
    static const bool kDefaultJoinWithStore;
    // 初始化是否从外部存储恢复默认值
    static const bool kDefaultRestoreFromStore;
    // 是否忽略外部存储错误默认值
    static const bool kDefaultIgnoreStoreError;
    // 是否使用批量插入方式默认值
    static const bool kDefaultStoreBatchInsert;
    // 批量插入的数据大小默认值
    static const int32_t kDefaultStoreBatchCount;
    // 批量插入的时间间隔默认值
    static const int32_t kDefaultStoreBatchIntervalMs;
};

} // namespace generic_operator
} // namespace dstream
#endif // __DSTREAM_GENERIC_OPERATOR_JOINER_CONTEXT_H__ NOLINT

