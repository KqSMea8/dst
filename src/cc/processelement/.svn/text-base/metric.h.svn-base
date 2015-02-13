/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#ifndef __DSTREAM_CC_PROCESSELEMENT_METRIC_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_METRIC_H__ // NOLINT

#include <stdint.h>
namespace dstream {
namespace processelement {
namespace internal {

class Metric {
public:
    Metric();
    static const char* kRecvTuplesCounter;
    static const char* kSendTuplesCounter;
    static const char* kRecvTimeTuplesCounter;
    static const char* kSendTimeTuplesCounter;
    static const char* kCpuUsageCounter;
    static const char* kMemUsageCounter;

protected:
    volatile int64_t m_recv_tuples_counter;
    volatile int64_t m_send_tuples_counter;
    volatile int64_t m_recv_time_tuples_counter;
    volatile int64_t m_send_time_tuples_counter;
}; // class Metric

} // namespace internal
} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_METRIC_H__ NOLINT
