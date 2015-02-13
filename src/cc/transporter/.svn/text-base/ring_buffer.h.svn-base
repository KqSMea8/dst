#ifndef __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__
#define __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__

#include <list>
#include <string>
#include "common/logger.h"
#include "common/mem_mgr.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace ring_buffer {

struct DataItem {
    int32_t     d_len;
    uint8_t*    d_ptr; // ack id
    int32_t     d_batch;

    DataItem()
        : d_len(0),
          d_ptr(NULL),
          d_batch(0) {}

    DataItem(uint8_t* ptr, int32_t len, int32_t batch)
        : d_len(len), d_ptr(ptr), d_batch(batch) {}
};

typedef error::Code ErrCode;

class RingBuffer {
public:
    RingBuffer(int32_t size, mem_mgr::MemMgr::MemMgrPtr inst, double watermark = 0.9);
    virtual ~RingBuffer();

    bool IsFull() {
        return m_w_count == m_buffer_size;
    }
    bool IsEmpty() {
        return m_r_count == 0;
    }
    uint32_t ReadSize() {
        return m_r_count;
    }
    uint32_t WriteSize() {
        return m_w_count;
    }
    ErrCode Status() {
        if (IsFull()) {
            return error::QUEUE_FULL;
        } else if (m_w_count >= m_watermark * m_buffer_size) {
            return error::OK_SOFT_WATERMARK;
        }
        return error::OK;
    }

    std::string DumpStatus() const;

    /**
     * @brief Push data into buffer with 'size'
     *
     * @param brs [in] input data
     * @param size [in] input data size
     * @return ErrCode error::OK push ok
     *                 others push fail, buffer full
     */
    ErrCode BatchPush(CBufReadStream* brs, int32_t size);

    /**
     * @brief Get data from buffer, we can re-get same data with specific 'ack'
     *
     * @param brs [out] output data
     * @param size [in/out] input data size & output data size must <= input size
     * @param batch [out] how many data blocks of this size
     * @param ack [in/out] if 'ack' not equal to 'kAdvanceQueueAck',
     *                     re-get old data block with this 'ack'
     *                     otherwise get new data block, then fill 'ack' as output
     * @return ErrCode error::OK pending ok
     *                 others pending fail, buffer empty
     */
    ErrCode BatchPending(OUT CBufWriteStream* bws,
                         INOUT int32_t* size,
                         OUT   int32_t* batch,
                         INOUT uint64_t* ack);

    /**
     * @brief Pop data out from buffer with specific maximum 'size'
     *        NOTICE: now whatever 'ack' is, we only pop out oldest data
     *
     * @param ack [in] specific 'ack' to find the specific data block to pop out
     *                 'kFlushQueueUnack' means to pop all data out
     * @param bws [out] if not NULL, write flush data into
     * @param size [out] if not NULL, write bytes size of data into
     * @param batch [out] if not NULL, write batch count of data into
     * @return ErrCode  error::OK pop ok
     *                  others
     */
    ErrCode BatchPop(uint64_t ack,
                     OUT CBufWriteStream* bws = NULL,
                     OUT int32_t* size = NULL,
                     OUT int32_t* batch = NULL);

    /**
     * @brief Flush & erase data out from buffer with specific minimum 'size'
     *
     * @param size [in/out] expect minimum size to flush
     * @param bws [out] if not NULL, write flush data into
     * @param batch [out] if not NULL, write batch count of data into
     * @return ErrCode  error::OK flush ok
     *                  others
     */
    ErrCode FlushPop(INOUT int32_t* size,
                     OUT CBufWriteStream* bws,
                     OUT int32_t* batch);

protected:
    enum DumpStrategy {
        DUMP_MOST = 0,
        DUMP_LEAST
    };
    DSTREAM_FORCE_INLINE int GetIntLen(uint8_t*& begin_ptr, int32_t* rear_len);
    DSTREAM_FORCE_INLINE ErrCode _Push(CBufReadStream* buf, const int32_t len);
    DSTREAM_FORCE_INLINE ErrCode _Pop(uint8_t* begin_ptr,
                                      int32_t* len,
                                      CBufWriteStream* buf,
                                      int32_t* batch);
    ErrCode _Pending(uint8_t* begin_ptr,
                     int32_t* len,
                     CBufWriteStream* buf,
                     int32_t* batch,
                     bool mv_pos,
                     bool reach_end);
    void _ConstraintCount(uint8_t* begin_ptr,
                          int32_t* max_len,
                          bool reach_end,
                          int32_t* batch);
    void _GreedyCount(uint8_t* begin_ptr,
                      int32_t* min_len,
                      bool /*reach_end*/,
                      int32_t* batch);
    ErrCode _DumpData(uint8_t* begin_ptr,
                      int32_t* len,
                      CBufWriteStream* buf,
                      int32_t* batch,
                      int32_t* pos,
                      bool mv_pos,
                      bool reach_end,
                      DumpStrategy ds);

private:
    typedef std::list<DataItem> PendingList;

    int32_t     m_buffer_size;
    uint8_t*    m_buffer;
    double      m_watermark;

    int32_t     m_w_count; // size that already write into buffer
    int32_t     m_r_count; // size that can be read from buffer
    int32_t     m_push_pos;
    int32_t     m_pending_pos;
    int32_t     m_pop_pos;

    PendingList m_pending;

    mem_mgr::MemMgr::MemMgrPtr m_mem_mgr_ptr;
};

} // namespace ring_buffer
} // namespace dstream

#endif  // __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__
