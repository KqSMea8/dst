/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTOR_COMMON_H__
#define __DSTREAM_SRC_CC_TRANSPORTOR_COMMON_H__

#include <string>
#include "BufStream.h"
#include "ESP.h"
#include "ESPObject.h"
#include "Kylin.h"
#include "Trace.h"
#include "common/dstream.h"
#include "common/error.h"
#include "common/utils.h"

namespace dstream {
namespace transporter {

typedef error::Code ErrCode;

#ifndef MOCKVIRTUAL
#define MOCKVIRTUAL
#endif

//-----------------------------------------------
// kylin
extern volatile bool g_init;            // esp system init
extern CESPSystem    g_esp;             // esp system
extern uint16_t      g_esp_port;        // esp listen port
extern uint32_t      g_precision_ms;    // precision of kylin system

extern int    g_kKylinExecWorkers;      // num of worker thread
extern int    g_kKylinNetWorkers;       // num of network thread
extern int    g_kKylinDiskWorkers;      // num of disk thread
extern int    g_kKylinBufSmallTranBufNum;  // num of small buf
extern int    g_kKylinBufSmallTranBufSize; // small buf size
extern int    g_kKylinBufBigTranBufNum; // num of big buf in one block,
                                        // each block is 64MB,
                                        // so each buf size is 64MB/32 = 2MB
extern double g_kKylinBufLowMark;       // soft-watermark to small buf
extern int    g_kKylinWriteWaterMark;   // 30%

/**
* @brief    start/stop kylin and esp system
* @return   true: ok; false: fail
* @author   konghui, konghui@baidu.com
* @date     2013-01-13
*/
bool TPInitKylin();
void TPStopKylin();
void TPPrintStatus(int intv);
void TPSetTranBuf(int nSmallBuff, int nBigBuff, int smallBufSize, int wWaterMark);

DSTREAM_FORCE_INLINE int TPGetFreeTranBufPercent() {
    return GetFreeTranBufNum() * 100 / GetTotalTranBufNum();
}

//-----------------------------------------------
// timer
/**
* @brief    get time
* @return   timestamp
* @author   konghui, konghui@baidu.com
* @date     2013-01-13
*/
DSTREAM_FORCE_INLINE uint64_t GetUint64TimeMs() {
    return static_cast<uint64_t>(GetTimeMS());
}

/**
* @brief    test if b is after then a
* @return   true: b - a > interval; else false:
* @author   konghui, konghui@baidu.com
* @date     2013-01-13
*/
DSTREAM_FORCE_INLINE bool IsAfter(uint64_t a, uint64_t b) {
    return static_cast<int64_t>(b) - static_cast<int64_t>(a) > 0;
}
DSTREAM_FORCE_INLINE bool IsAfter(uint64_t a, uint64_t b, uint64_t interval) {
    return static_cast<int64_t>(b) - static_cast<int64_t>(a) >
           static_cast<int64_t>(interval);
}

//-----------------------------------------------

APF_ERROR QueueDiskExec(int naction,
                        CAsyncClient* pclient,
                        AsyncContext* ctx,
                        int nwhich);

//-----------------------------------------------
// tuples
/**
* @brief    dump tuples from bufstream into string
* @param[IN]    hdl     buf handler
* @param[IN]    brs     buf stream
* @param[IN]    size    size of tuples
* @param[IN]    batch   num of tuples
* @param[OUT]   s       to store the tuple string
* @return       error::OK: success;
*               error::BAD_ARGUMENT:   bad arguments
*               error::DATA_CORRUPTED: corrupted data;
* @author   konghui, konghui@baidu.com
* @date     2013-02-01
*/
ErrCode DumpTuplesFromBufHdl(IN  BufHandle* hdl,
                             IN  int size,
                             IN  int batch,
                             OUT std::string* s);
ErrCode DumpTuplesFromBufHdlWithMsgHead(IN  BufHandle* hdl,
                                        IN  int size,
                                        IN  int batch,
                                        OUT std::string* s);
ErrCode DumpTuplesFromBufStream(IN  CBufReadStream* brs,
                                IN  int size,
                                IN  int batch,
                                OUT std::string* s);

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTOR_COMMON_H__
