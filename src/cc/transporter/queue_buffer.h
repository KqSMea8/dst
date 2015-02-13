#ifndef __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__
#define __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <deque>
#include <list>
#include <string>
#include "common/logger.h"
#include "common/mem_mgr.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace queue_buffer {

// typedef boost::shared_array<uint8_t> DataUnitPtr;

struct DataItem {
    int32_t     d_len;
    uint8_t*    d_ptr;
    int32_t     d_batch;

    DataItem()
        : d_len(0),
          d_ptr(NULL),
          d_batch(0) {
    }

    DataItem(uint8_t* ptr,
             int32_t len,
             int32_t batch)
        : d_len(len),
          d_ptr(ptr),
          d_batch(batch) {}
};

typedef error::Code ErrCode;

class QueueBuffer {
public:
    QueueBuffer(int32_t size, mem_mgr::MemMgr::MemMgrPtr inst, double watermark = 0.9);
    virtual ~QueueBuffer();

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

    // for test
    bool HasPending() {
        return m_last_data.d_len >= 0;
    }

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

    DSTREAM_FORCE_INLINE int64_t _AckForward() {
        return m_ack++;
    }
    DSTREAM_FORCE_INLINE void _ClearPending() {
        m_last_data.d_len = -1;
        if (m_last_data.d_ptr) {
            delete m_last_data.d_ptr;
            m_last_data.d_ptr = NULL;
        }
        m_last_data.d_batch = -1;
    }

    ErrCode _DumpData(int32_t* len,
                      CBufWriteStream* buf,
                      int32_t* batch,
                      bool erase,
                      DumpStrategy ds);
    ErrCode _Push(CBufReadStream* buf, const int32_t len);
    ErrCode _Pending(int32_t* len,
                     CBufWriteStream* buf,
                     int32_t* batch,
                     bool replay);
    ErrCode _Pop(int32_t* len,
                 CBufWriteStream* buf,
                 int32_t* batch);

private:
    typedef std::deque<DataItem*> QueueList;

    uint64_t    m_buffer_size;
    uint64_t    m_o_buffer_size;
    double      m_watermark;
    uint64_t    m_w_count; // size that already write into buffer
    uint64_t    m_r_count; // size that can be read from buffer
    uint64_t    m_ack;
    DataItem    m_last_data;
    QueueList   m_queue;
    mem_mgr::MemMgr::MemMgrPtr m_mem_mgr_ptr;
};

} // namespace queue_buffer
} // namespace dstream

#endif  // __DSTREAM_CC_TRANSPORTER_RING_BUFFER_H__
