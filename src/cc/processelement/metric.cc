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

#include "processelement/metric.h"

namespace dstream {
namespace processelement {
namespace internal {

// ------------------------------------------------------------
// Metric Implementation
const char* Metric::kRecvTuplesCounter = "dstream.pe.recvTuplesCounter";
const char* Metric::kSendTuplesCounter = "dstream.pe.sendTuplesCounter";
const char* Metric::kRecvTimeTuplesCounter = "dstream.pe.recvTimeTuplesCounter";
const char* Metric::kSendTimeTuplesCounter = "dstream.pe.sendTimeTuplesCounter";
const char* Metric::kCpuUsageCounter = "dstream.pe.cpuUsageCounter";
const char* Metric::kMemUsageCounter = "dstream.pe.memUsageCounter";

Metric::Metric()
        : m_recv_tuples_counter(0),
        m_send_tuples_counter(0),
        m_recv_time_tuples_counter(0),
        m_send_time_tuples_counter(0) {
}

} // namespace internal
} // namespace processelement
} // namespace dstream
