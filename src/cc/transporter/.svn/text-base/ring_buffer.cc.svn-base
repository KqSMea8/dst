#include "transporter/ring_buffer.h"
#include "common/logger.h"

namespace dstream {
namespace ring_buffer {

RingBuffer::RingBuffer(int32_t size, mem_mgr::MemMgr::MemMgrPtr inst, double watermark)
    : m_buffer_size(size),
      m_buffer(NULL),
      m_watermark(watermark),
      m_w_count(0),
      m_r_count(0),
      m_push_pos(0),
      m_pending_pos(0),
      m_pop_pos(0),
      m_mem_mgr_ptr(inst) {
    assert(size > 0);
    if (m_mem_mgr_ptr) {
        // allocate from global mem pool
        assert(m_mem_mgr_ptr->AllocateMem(m_buffer_size) == error::OK);
    }

    m_buffer = new (std::nothrow) uint8_t[m_buffer_size];
    assert(m_buffer);
}

RingBuffer::~RingBuffer() {
    delete [] m_buffer;
    m_buffer = NULL;
    if (m_mem_mgr_ptr) {
        // return to global mem pool
        m_mem_mgr_ptr->ReleaseMem(m_buffer_size);
    }
}

std::string RingBuffer::DumpStatus() const {
    char t_status[256];
    snprintf(t_status, sizeof(t_status), "rb_ptr=%p, bs=%d, rws=%d, rrs=%d",
             this, m_buffer_size, m_w_count, m_r_count);
    return t_status;
}

ErrCode RingBuffer::_Push(CBufReadStream* buf, const int32_t len) {
    // assert(m_w_count >= m_r_count);
    UNLIKELY_IF(NULL == buf || 0 == len || len > m_buffer_size - m_w_count) {
        // len > total free space len
        DSTREAM_DEBUG("[%s] buf(%p) len(%d) total_free_len(%d)",
                      __FUNCTION__, buf, len, m_buffer_size - m_w_count);
        return error::BAD_ARGUMENT;
    }
    int32_t rear_len = m_buffer_size - m_push_pos;
    if (len <= rear_len) {
        int32_t rlen = buf->GetBuffer(reinterpret_cast<char*>(&m_buffer[m_push_pos]), len);
        assert(len == rlen);
        m_push_pos += len;
        m_push_pos %= m_buffer_size;
    } else { // len > rear_len
        int32_t rlen = buf->GetBuffer(reinterpret_cast<char*>(&m_buffer[m_push_pos]), rear_len);
        assert(rear_len == rlen);
        rlen = buf->GetBuffer(reinterpret_cast<char*>(m_buffer), len - rear_len);
        assert(len - rear_len == rlen);
        m_push_pos = len - rear_len;
    }
    m_w_count += len;
    m_r_count += len;
    if (m_w_count >= m_watermark * m_buffer_size) {
        return error::OK_SOFT_WATERMARK;
    }
    return error::OK;
}

// move pop_pos actually
// buf == NULL, ignore data
ErrCode RingBuffer::_Pop(uint8_t* begin_ptr,
                         int32_t* len,
                         CBufWriteStream* buf,
                         int32_t* batch) {
    UNLIKELY_IF(0 == *len || begin_ptr < m_buffer ||
                begin_ptr >= m_buffer + m_buffer_size) {
        DSTREAM_DEBUG("[%s] ptr(%p) len(%d) buf(%p) batch(%p)",
                      __FUNCTION__, begin_ptr, *len, buf, batch);
        return error::BAD_ARGUMENT;
    }
    ErrCode ret = error::OK;
    bool end_mark = false;
    if (*len >= m_w_count) {
        // no enough data to pop, pop all
        *len = m_w_count;
        end_mark = true;
    }
    ret = _DumpData(begin_ptr,
                    len/*INOUT*/,
                    buf,
                    batch,
                    &m_pop_pos,
                    true/*need move pos*/,
                    end_mark,
                    DUMP_MOST);
    m_w_count -= *len;
    return ret;
}

// return new begin_ptr & new rear_len
int RingBuffer::GetIntLen(uint8_t*& begin_ptr, int32_t* rear_len) {
    int ret = 0;
    int32_t move_len = 0;
    if (*rear_len >= static_cast<int32_t>(sizeof(int))) {
        ret = *(reinterpret_cast<int*>(begin_ptr));
    } else {
        memcpy(&ret, begin_ptr, *rear_len);
        memcpy(reinterpret_cast<uint8_t*>(&ret) + *rear_len, m_buffer, sizeof(int) - *rear_len);
    }
    move_len = ret/*data len*/ + sizeof(int);
    if (*rear_len > move_len) {
        begin_ptr += move_len;
        *rear_len -= move_len;
    } else {
        *rear_len = m_buffer_size - (move_len - *rear_len);
        begin_ptr = m_buffer + m_buffer_size - *rear_len;
    }
    return ret;
}

void RingBuffer::_ConstraintCount(uint8_t* begin_ptr,
                                  int32_t* max_len,
                                  bool reach_end,
                                  int32_t* batch) {
    int32_t len = *max_len;
    uint8_t* cur_ptr = begin_ptr;
    int32_t cur_rear_len = m_buffer_size - (begin_ptr - m_buffer);
    // get align bound len & batch number count
    int32_t batch_count = 0;
    int32_t align_len = 0;
    int32_t tmp_len = 0;
    if (reach_end) { // we may meet the end of data, so prevent from reading out of range
        do {
            tmp_len = GetIntLen(cur_ptr, &cur_rear_len) + sizeof(int);
            align_len += tmp_len;
            batch_count++;
        } while (align_len < len);
        assert(align_len == len);
    } else {
        do {
            // here we must read more data than we wanted, but don't worry about out of range
            tmp_len = GetIntLen(cur_ptr, &cur_rear_len) + sizeof(int);
            if (align_len + tmp_len > len) {
                break;
            }
            align_len += tmp_len;
            batch_count++;
        } while (1);
    }
    *max_len = align_len;
    *batch = batch_count;
}

void RingBuffer::_GreedyCount(uint8_t* begin_ptr,
                              int32_t* min_len,
                              bool /*reach_end*/,
                              int32_t* batch) {
    int32_t len = *min_len;
    uint8_t* cur_ptr = begin_ptr;
    int32_t cur_rear_len = m_buffer_size - (begin_ptr - m_buffer);
    // get align bound len & batch number count
    int32_t batch_count = 0;
    int32_t align_len = 0;
    int32_t tmp_len = 0;
    do {
        // here we must read more data than we wanted, but don't worry about out of range
        tmp_len = GetIntLen(cur_ptr, &cur_rear_len) + sizeof(int);
        align_len += tmp_len;
        batch_count++;
    } while (align_len < len);
    assert(align_len >= len);
    *min_len = align_len;
    *batch = batch_count;
}

ErrCode RingBuffer::_DumpData(uint8_t* begin_ptr,
                              int32_t* len,
                              CBufWriteStream* buf,
                              int32_t* batch,
                              int32_t* pos,
                              bool mv_pos,
                              bool reach_end,
                              DumpStrategy ds) {
    int32_t rear_len = m_buffer_size - (begin_ptr - m_buffer);
    int32_t batch_cnt = 0;
    if (DUMP_MOST == ds) {
        if (buf && batch) {  // we need data dump
            if (mv_pos) {
                _ConstraintCount(begin_ptr, len, reach_end, &batch_cnt);
            } else {
                // NOTICE: we can use 'len' directly when 'mv_pos' is false,
                //         which means this dump is re-ack operation
                batch_cnt = *batch;
            }
        }
        // else
        // do not modify 'len'
        // 'batch' must be NULL
    } else {
        _GreedyCount(begin_ptr, len, reach_end, &batch_cnt);
    }

    // DSTREAM_DEBUG("[%s] expect len(%d) align_len(%d)", __FUNCTION__, *len, align_len);
    ErrCode ret = error::OK;
    // read data
    if (*len <= rear_len) {
        if (buf) {
            UNLIKELY_IF(*len != buf->PutBuffer(reinterpret_cast<char*>(begin_ptr), *len)) {
                ret = error::SERIAIL_DATA_FAIL;
            }
        }
        if (mv_pos) {
            if (DUMP_MOST != ds ||
                /* else DUMP_MOST == ds */error::OK == ret) {
                *pos += *len;
                *pos %= m_buffer_size;
            }
        }
    } else {
        int32_t offset = *len - rear_len;
        if (buf) {
            UNLIKELY_IF(rear_len != buf->PutBuffer(reinterpret_cast<char*>(begin_ptr), rear_len)) {
                ret = error::SERIAIL_DATA_FAIL;
            }
            UNLIKELY_IF(offset != buf->PutBuffer(reinterpret_cast<char*>(m_buffer), offset)) {
                ret = error::SERIAIL_DATA_FAIL;
            }
        }
        if (mv_pos) {
            if (DUMP_MOST != ds ||
                /* else DUMP_MOST == ds */error::OK == ret) {
                *pos = offset;
            }
        }
    }
    if (batch) {
        *batch = batch_cnt;
    }
    return ret;
}


ErrCode RingBuffer::_Pending(uint8_t* begin_ptr,
                             int32_t* len,
                             CBufWriteStream* buf,
                             int32_t* batch,
                             bool mv_pos,
                             bool end_mark) {
    UNLIKELY_IF(0 == *len || NULL == begin_ptr) {
        // bad input argument or empty
        DSTREAM_DEBUG("[%s] len(%d) ptr(%p)", __FUNCTION__, *len, begin_ptr);
        return error::BAD_ARGUMENT;
    }
    ErrCode ret = _DumpData(begin_ptr,
                            len,
                            buf,
                            batch,
                            &m_pending_pos,
                            mv_pos,
                            end_mark,
                            DUMP_MOST);
    if ((error::OK == ret) && mv_pos) {
        m_r_count -= *len;
    }
    return ret;
}


ErrCode RingBuffer::BatchPush(CBufReadStream* brs, int32_t size) {
    if (size > m_buffer_size - m_w_count) {
        return error::QUEUE_FULL;
    }
    return _Push(brs, size);
}

ErrCode RingBuffer::BatchPending(CBufWriteStream* bws,
                                 INOUT int32_t* size,
                                 OUT   int32_t* batch,
                                 INOUT uint64_t* ack) {
    ErrCode ret = error::OK;
    uint8_t* data_ptr = reinterpret_cast<uint8_t*>(*ack);
    // expect size
    int32_t data_len = *size;
    if (NULL == bws || data_len < 0) {
        DSTREAM_DEBUG("[%s] bws(%p) size(%d) ack(%p)",
                      __FUNCTION__, bws, data_len, data_ptr);
        return error::BAD_ARGUMENT;
    }
    if (0 == data_len) {
        // size 0 is ok
        return error::OK;
    }
    UNLIKELY_IF(IsEmpty()) {
        return error::QUEUE_EMPTY;
    }
    int32_t batch_count = 0;
    DataItem meta;
    if (transporter::kAdvanceQueueAck != reinterpret_cast<uint64_t>(data_ptr)) {
        UNLIKELY_IF(data_ptr < m_buffer || data_ptr >= m_buffer + m_buffer_size) {
            DSTREAM_DEBUG("[%s] data_ptr(%p) out of range[%p, %p)",
                          __FUNCTION__, data_ptr, m_buffer, m_buffer + m_buffer_size);
            return error::BAD_ARGUMENT;
        }
        // check if any pending data
        if (m_pending.size() <= 0) {
            DSTREAM_FATAL("[%s] data_ptr(%p) invalid, no pending data",
                          __FUNCTION__, data_ptr);
            // cannot reach here!
        }
        // find pending data
        // get first elem of pending list
        meta = m_pending.front();
        data_len = meta.d_len;
        // end mark ?
        bool end_mark = true;
        if (m_r_count > 0 || m_pending.size() > 1) {
            end_mark = false;
        }
        assert(meta.d_ptr == data_ptr);
        ret = _Pending(data_ptr,
                       &data_len/*INOUT*/,
                       bws/*OUT*/,
                       &batch_count/*OUT*/,
                       false/*do not move pos*/,
                       end_mark);
        assert(meta.d_len == data_len);
    } else {
        // new data pending
        // record ptr first, m_pending_pos may change in _Pending
        meta.d_ptr = &m_buffer[m_pending_pos];
        bool end_mark = false;
        // get actual data len
        if (m_r_count <= data_len) {
            // no enough data to pending
            // get all rest data
            data_len = m_r_count;
            end_mark = true;
        }
        ret = _Pending(&m_buffer[m_pending_pos],
                       &data_len/*INOUT*/,
                       bws/*OUT*/,
                       &batch_count/*OUT*/,
                       true/*move pos*/,
                       end_mark);
        if (error::OK == ret) {
            meta.d_len = data_len;
            meta.d_batch = batch_count;
            m_pending.push_back(meta);
        } else {
            return ret;
        }
    }
    *size = meta.d_len;
    *batch = meta.d_batch;
    *ack = reinterpret_cast<uint64_t>(meta.d_ptr);
    return ret;
}

ErrCode RingBuffer::BatchPop(uint64_t ack,
                             OUT CBufWriteStream* bws,
                             OUT int32_t* size,
                             OUT int32_t* batch) {
    if (m_pending.size() <= 0) {
        return error::OK_NO_DATA_FLUSHED;
    }
    ErrCode ret = error::OK;
    int32_t pop_size = 0;
    if (transporter::kFlushQueueUnack == ack) {
        int32_t total_size = 0;
        int32_t total_batch = 0;
        int32_t pop_batch = 0;
        // pop all
        for (int i = m_pending.size(); i > 0; i--) {
            DataItem meta = m_pending.front();
            pop_size = meta.d_len;
            ret = _Pop(meta.d_ptr, &pop_size, bws, &pop_batch);
            UNLIKELY_IF(ret < error::OK) {
                return ret;
            }
            total_size += pop_size;
            total_batch += pop_batch;
            m_pending.pop_front();
        }
        pop_size = total_size;
        if (batch) {
            *batch = total_batch;
        }
    } else {
        DataItem meta = m_pending.front();
        // must ack the oldest one
        // assert(meta.d_ptr == (uint8_t*)ack);
        pop_size = meta.d_len;
        ret = _Pop(meta.d_ptr, &pop_size, bws, batch);
        UNLIKELY_IF(ret < error::OK) {
            return ret;
        }
        m_pending.pop_front();
    }
    if (size) {
        *size = pop_size;
    }
    return ret;
}

ErrCode RingBuffer::FlushPop(INOUT int32_t* size,
                             OUT CBufWriteStream* bws,
                             OUT int32_t* batch) {
    UNLIKELY_IF(NULL == size) {
        return error::BAD_ARGUMENT;
    }
    ErrCode ret = error::OK;
    int32_t expect_size = *size;
    int32_t pending_size = m_w_count - m_r_count;
    if (pending_size > 0) {
        // has pending data
        if (expect_size >= pending_size) {
            // flush out all pending data
            DataItem begin_meta = m_pending.front();
            ret = _DumpData(begin_meta.d_ptr,
                            &expect_size,
                            bws,
                            batch,
                            &m_pop_pos,
                            true/*move pos*/,
                            true/*no use*/,
                            DUMP_LEAST);
            UNLIKELY_IF(ret < error::OK) {
                return ret;
            }
            // no pending data
            m_pending.clear();
            m_pending_pos = m_pop_pos;
            m_w_count -= expect_size;
            m_r_count -= expect_size - pending_size;
        } else {
            // only flush data in pending
            int32_t tmp_len = 0;
            DataItem begin_meta = m_pending.front();
            while ((tmp_len < expect_size)
                   && (m_pending.size() > 0)) {
                // TODO(lanbijia@baidu.com): for test
                DataItem test_meta = m_pending.front();
                tmp_len += test_meta.d_len;
                m_pending.pop_front();
            }
            expect_size = tmp_len;
            // maybe pending item len = 0
            if (expect_size <= 0) {
                goto ERR_RET;
            }
            ret = _DumpData(begin_meta.d_ptr,
                            &expect_size,
                            bws,
                            batch,
                            &m_pop_pos,
                            true/*move pos*/,
                            true/*no use*/,
                            DUMP_LEAST);
            if (m_pending.size() <= 0) {
                m_pending_pos = m_pop_pos;
            }
            m_w_count -= expect_size;
            assert(expect_size <= pending_size);
        }
    } else {
        // no pending data
        // check expect_size
        if (expect_size > m_w_count) {
            expect_size = m_w_count;
        }
        if (expect_size <= 0) {
            goto ERR_RET;
        }
        ret = _DumpData(m_buffer + m_pop_pos,
                        &expect_size,
                        bws,
                        batch,
                        &m_pop_pos,
                        true/*move pos*/,
                        true/*no use*/,
                        DUMP_LEAST);
        // move pending pos according to pop pos
        m_pending_pos = m_pop_pos;
        m_w_count -= expect_size;
        m_r_count -= expect_size;
    }
ERR_RET:
    *size = expect_size;
    return ret;
}

} // namespace ring_buffer
} // namespace dstream
