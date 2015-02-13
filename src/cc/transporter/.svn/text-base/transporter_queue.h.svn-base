/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTOR_QUEUE_H__
#define __DSTREAM_SRC_CC_TRANSPORTOR_QUEUE_H__

#include <string>
#include <vector>
#include "common/config_const.h"
#include "common/mem_mgr.h"
#include "transporter/queue_buffer.h"
#include "transporter/ring_buffer.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_counters.h"

namespace dstream {
namespace transporter {

#define TRANS_BUFFER_TPYE queue_buffer::QueueBuffer
// #define TRANS_BUFFER_TPYE ring_buffer::RingBuffer

/**
*@brief ack queue
*/
class TPAckQueue {
public:
    TPAckQueue(uint64_t owner_id, uint64_t vice_id,
               const std::string& name, const std::string& cnt_grp);
    virtual ~TPAckQueue() {
        Destroy();
    }
    MOCKVIRTUAL ErrCode Init(uint64_t capacity,
                             double watermark = g_kKylinBufLowMark);
    MOCKVIRTUAL ErrCode Destroy();

    MOCKVIRTUAL DSTREAM_FORCE_INLINE uint64_t Size() {
        return m_buf->WriteSize();
    }
    MOCKVIRTUAL DSTREAM_FORCE_INLINE uint64_t UnackedSize() {
        return m_buf->WriteSize() - m_buf->ReadSize();
    }

    /**
    * @brief    test queue status
    * @return   error::OK(= 0): queue is OK
    *           error::OK_SOFT_WATERMARK(> 0): queue reaches the soft warkmark
    *           error::QUEUE_FULL(< 0):   queue is full
    *           < 0 : other failures;
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-21
    */
    MOCKVIRTUAL DSTREAM_FORCE_INLINE ErrCode IsFull()  {
        return m_buf->Status();
    }
    /**
    * @brief    test queue empty
    * @return   true: queue is empty
    *           false: queue is not empty
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-21
    */
    MOCKVIRTUAL DSTREAM_FORCE_INLINE bool Empty() {
        return m_buf->IsEmpty();
    }

    MOCKVIRTUAL DSTREAM_FORCE_INLINE std::string DumpStatus() const {
        return m_buf->DumpStatus();
    }

    MOCKVIRTUAL DSTREAM_FORCE_INLINE TPQueueCounter DumpCounter() {
        return TPQueueCounter(m_capacity, m_watermark, Size(), UnackedSize());
    }

    //-----------------------------------------------
    // data interface
    /**
    * @brief    write batch data in transaction.
    * @param[in]    brs     tuples to be write
    * @param[inout] size    data size
    * @return   error::OK(= 0) : success;
    *           error::OK_SOFT_WATERMARK(> 0): success but reaches the soft-watermark;
    *           error::QUEUE_FULL(< 0): fail due to queue has no enough space;
    *           < 0 : other failures;
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-21
    */
    MOCKVIRTUAL inline
    ErrCode PushBack(IN CBufReadStream* brs, IN int32_t size) {
        return m_buf->BatchPush(brs, size);
    }

    /**
    * @brief    read data from ack;
    * @param[OUT]    bws    buf stream to hold data
    * @param[INOUT]  size   indicate at most `size` data to read and
                            return the real size been read
    * @param[INOUT]  batch  return num of tuples been read
    * @param[INOUT]  ack    data posistion
    * @return   error::OK(= 0) : success;
    *           error::QUEUE_EMPTY(< 0): fail due to queue empty;
    *           error::BAD_ARGUMENT: fail due to bws == NULL and other param error
    *           error::SERIAIL_DATA_FAIL: fail while bws->PutBuffer error
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-21
    */
    MOCKVIRTUAL inline
    ErrCode Front(OUT   CBufWriteStream* bws,
                  INOUT int32_t*  size,
                  INOUT int32_t*  batch,
                  INOUT uint64_t* ack) {
        return m_buf->BatchPending(bws, size, batch, ack);
    }
    /**
    * @brief    pop unacked data
    * @param[IN]    ack    data posistion
    * @param[OUT]   bws    buf stream to hold data
    * @param[OUT]   size   size of data been read
    * @param[OUT]   batch  num of data been read
    * @return
    * @return   error::OK(= 0) : success;
    *           error::OK_NO_DATA_FLUSHED(> 0): no data flushed;
    *           error::BAD_ARGUMENT: param error
    *           error::SERIAIL_DATA_FAIL: fail while bws->PutBuffer error
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-21
    */
    MOCKVIRTUAL DSTREAM_FORCE_INLINE
    ErrCode PopFront(uint64_t ack) {
        return PopFront(ack, NULL, NULL, NULL);
    }
    MOCKVIRTUAL inline
    ErrCode PopFront(IN  uint64_t ack,
                     OUT CBufWriteStream* bws,
                     OUT int32_t* size,
                     OUT int32_t* batch) {
        return m_buf->BatchPop(ack, bws, size, batch);
    }

    /**
    * @brief    drop `size` data from head
    *
    * @param[INOUT]  size   indicate at least `size` data to dropped and
                            return the real size been  dropped
    * @param[OUT]    batch  hold the num of dropped data
    * @param[OUT]    bws    hold the dropped data
    * @return   error::OK(= 0) : success;
    *           error::BAD_ARGUMENT: param error
    *           error::SERIAIL_DATA_FAIL: fail while bws->PutBuffer error
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-05
    */
    MOCKVIRTUAL inline
    ErrCode PopAgedTuples(INOUT int32_t* size,
                          OUT   int32_t* batch,
                          OUT   CBufWriteStream* bws) {
        return m_buf->FlushPop(size, bws, batch);
    }

protected:
    TRANS_BUFFER_TPYE*  m_buf;
    std::string         m_name;
    size_t              m_capacity;
    double              m_watermark;
    uint64_t            m_owner_id;  // 1. used by counter as the main key;
                                    // 2. used to alloc preepmtive memory;
    uint64_t            m_vice_id;   // 1. used by counter as the main key;
    std::string         m_cnt_grp;
    // uint64_t    xfered_num_;  // size of received data
    // uint64_t    remain_num_;  // size of data in the queue
    // uint64_t    droped_num_;  // size of dropped data
    // uint64_t    xfered_size_; // size of received data
    // uint64_t    remain_size_; // size of data in the queue
    // uint64_t    droped_size_; // size of dropped data
}; // class TPAckQueue

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTOR_QUEUE_H__
