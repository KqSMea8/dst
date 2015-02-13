/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file joiner_context.h
 * @author lanbijia
 * @date 2012/11/5
 * @brief ����Joiner��ͨ�ò���
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
    // �������ڵĲ�������������
    static const char* kWindowStep;
    // �������ڵ��ܴ�С����������
    static const char* kWindowSize;
    // �ⲿ�洢����������
    static const char* kJoinWithStore;
    // ��ʼ���Ƿ���ⲿ�洢�ָ�
    static const char* kRestoreFromStore;
    // �Ƿ�����ⲿ�洢����
    static const char* kIgnoreStoreError;
    // �Ƿ�ʹ���������뷽ʽ
    static const char* kStoreBatchInsert;
    // ������������ݴ�С
    static const char* kStoreBatchCount;
    // ���������ʱ����
    static const char* kStoreBatchIntervalMs;
    // �������ڵĲ���Ĭ��ֵ
    static const int32_t kDefaultWindowStep;
    // �������ڵ��ܴ�СĬ��ֵ
    static const int32_t kDefaultWindowSize;
    // �ⲿ�洢����Ĭ��ֵ
    static const bool kDefaultJoinWithStore;
    // ��ʼ���Ƿ���ⲿ�洢�ָ�Ĭ��ֵ
    static const bool kDefaultRestoreFromStore;
    // �Ƿ�����ⲿ�洢����Ĭ��ֵ
    static const bool kDefaultIgnoreStoreError;
    // �Ƿ�ʹ���������뷽ʽĬ��ֵ
    static const bool kDefaultStoreBatchInsert;
    // ������������ݴ�СĬ��ֵ
    static const int32_t kDefaultStoreBatchCount;
    // ���������ʱ����Ĭ��ֵ
    static const int32_t kDefaultStoreBatchIntervalMs;
};

} // namespace generic_operator
} // namespace dstream
#endif // __DSTREAM_GENERIC_OPERATOR_JOINER_CONTEXT_H__ NOLINT

