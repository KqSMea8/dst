#include "transporter/queue_buffer.h"
#include <string>
#include <vector>
#include "common/logger.h"

namespace dstream {
namespace queue_buffer {

QueueBuffer::QueueBuffer(int32_t size, mem_mgr::MemMgr::MemMgrPtr inst, double watermark)
    : m_buffer_size(size),
      m_o_buffer_size(m_buffer_size),
      m_watermark(watermark),
      m_w_count(0),
      m_r_count(0),
      m_ack(1),
      m_mem_mgr_ptr(inst) {
    assert(size > 0);
    _ClearPending();
    if (m_mem_mgr_ptr) {
        // allocate from global mem pool
        if (m_mem_mgr_ptr->AllocateMem(m_buffer_size) < error::OK) {
            // allocate fail, set o_m_buffer_size to 0 as mark
            m_o_buffer_size = 0;
        }
    }
}

QueueBuffer::~QueueBuffer() {
    int32_t len = m_w_count;
    // release all datas
    if (_DumpData(&len, NULL, NULL, true, DUMP_LEAST) < error::OK) {
        DSTREAM_ERROR("queue_buffer fail to release data");
    }
    // m_o_buffer_size == 0 means there is no mem allocated while queue_buffer construct
    if (m_mem_mgr_ptr && m_o_buffer_size != 0) {
        // return to global mem pool
        m_mem_mgr_ptr->ReleaseMem(m_buffer_size);
    }
}

std::string QueueBuffer::DumpStatus() const {
    char t_status[256];
    snprintf(t_status, sizeof(t_status),
             "rb_ptr=%p, bs=%lu, rws=%lu, rrs=%lu, "
             "lst_data(len=%d, batch=%d, ack=%lu), qs(%zu)",
             this, m_buffer_size, m_w_count, m_r_count, m_last_data.d_len,
             m_last_data.d_batch, m_ack, m_queue.size());
    return t_status;
}

ErrCode QueueBuffer::_Push(CBufReadStream* buf, const int32_t len) {
    // assert(m_w_count >= m_r_count);
    UNLIKELY_IF(NULL == buf || 0 == len ||
                static_cast<uint64_t>(len) > m_buffer_size - m_w_count) {
        // len > total free space len
        DSTREAM_DEBUG("[%s] buf(%p) len(%d) total_free_len(%lu)",
                      __FUNCTION__, buf, len, m_buffer_size - m_w_count);
        return error::BAD_ARGUMENT;
    }

    // put all data into queue
    std::vector<DataItem*> t_data_list;
    int32_t t_data_len = 0;
    while (t_data_len < len) {
        DataItem* data = new (std::nothrow) DataItem;
        if (NULL == data) {
            return error::BAD_MEMORY_ALLOCATION;
        }
        // get data len from buf
        assert(sizeof(data->d_len) == buf->GetBuffer(reinterpret_cast<char*>(&data->d_len),
                                                     sizeof(data->d_len)));
        data->d_ptr = new (std::nothrow) uint8_t[data->d_len];
        if (NULL == data->d_ptr) {
            return error::BAD_MEMORY_ALLOCATION;
        }
        // get data from buf
        assert(data->d_len == buf->GetBuffer(reinterpret_cast<char*>(data->d_ptr),
                                             data->d_len));
        t_data_list.push_back(data);
        t_data_len += data->d_len + sizeof(data->d_len);
    }
    BOOST_FOREACH(DataItem * data_ptr, t_data_list) {
        m_queue.push_back(data_ptr);
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
ErrCode QueueBuffer::_Pop(int32_t* len,
                          CBufWriteStream* buf,
                          int32_t* batch) {
    UNLIKELY_IF(0 >= *len) {
        DSTREAM_DEBUG("[%s] len(%d) buf(%p) batch(%p)",
                      __FUNCTION__, *len, buf, batch);
        return error::BAD_ARGUMENT;
    }
    ErrCode ret = error::OK;
    if (static_cast<uint64_t>(*len) > m_w_count) {
        // no enough data to pop, pop all
        *len = m_w_count;
    }
    ret = _DumpData(len/*INOUT*/,
                    buf,
                    batch,
                    true/*erase data*/,
                    DUMP_MOST);
    // whatever 'ret' is, modify m_w_count
    m_w_count -= *len;
    return ret;
}

ErrCode QueueBuffer::_DumpData(int32_t* len,
                               CBufWriteStream* buf,
                               int32_t* batch,
                               bool erase,
                               DumpStrategy ds) {
    ErrCode ret = error::OK;
    int32_t total_len = 0;
    int32_t batch_cnt = 0;

    size_t queue_size = m_queue.size();
    BOOST_FOREACH(DataItem * data_ptr, m_queue) {
        // dump data less than 'len'
        if ((DUMP_MOST == ds) &&
            (total_len + data_ptr->d_len + static_cast<int32_t>(sizeof(data_ptr->d_len)) > *len)) {
            break;
        }
        // put data
        if (buf) {
            UNLIKELY_IF(sizeof(data_ptr->d_len) !=
                            buf->PutBuffer(reinterpret_cast<char*>(&data_ptr->d_len),
                                           sizeof(data_ptr->d_len))) {
                ret = error::SERIAIL_DATA_FAIL;
            }
            UNLIKELY_IF(data_ptr->d_len !=
                            buf->PutBuffer(reinterpret_cast<char*>(data_ptr->d_ptr),
                                           data_ptr->d_len)) {
                ret = error::SERIAIL_DATA_FAIL;
            }
        }
        total_len += data_ptr->d_len + sizeof(data_ptr->d_len);
        batch_cnt++;
        // dump data equal to or more than 'len'
        if ((DUMP_LEAST == ds) && (total_len >= *len)) {
            break;
        }
    }
    *len = total_len;
    if (erase && batch_cnt) {
        // erase queue data
        // m_queue.erase(m_queue.begin(), m_queue.begin() + batch_cnt);
        int32_t i = batch_cnt;
        while (i--) {
            DataItem* t_item = m_queue.front();
            delete t_item->d_ptr;
            delete t_item;
            m_queue.pop_front();
        }
        assert(queue_size == m_queue.size() + batch_cnt);
    }

    if (batch) {
        *batch = batch_cnt;
    }
    return ret;
}


ErrCode QueueBuffer::_Pending(int32_t* len,
                              CBufWriteStream* buf,
                              int32_t* batch,
                              bool replay) {
    UNLIKELY_IF(0 == *len) {
        // bad input argument or empty
        DSTREAM_DEBUG("[%s] len(%d)", __FUNCTION__, *len);
        return error::BAD_ARGUMENT;
    }
    ErrCode ret = _DumpData(len, buf, batch, false, DUMP_MOST);
    if (error::OK == ret && !replay) {
        m_r_count -= *len;
    }
    return ret;
}

ErrCode QueueBuffer::BatchPush(CBufReadStream* brs, int32_t size) {
    if (static_cast<uint64_t>(size) > m_buffer_size - m_w_count) {
        // extend mem
        // if ((m_mem_mgr_ptr == NULL) ||
        //     (m_mem_mgr_ptr->AllocateMem(m_o_buffer_size) < error::OK)) {
        return error::QUEUE_FULL;
        // } else {
        //   m_buffer_size += m_o_buffer_size;
        // }
    }
    return _Push(brs, size);
}

ErrCode QueueBuffer::BatchPending(CBufWriteStream* bws,
                                  INOUT int32_t* size,
                                  OUT   int32_t* batch,
                                  INOUT uint64_t* ack) {
    ErrCode ret = error::OK;

    // expect size
    int32_t data_len = *size;
    if (NULL == bws || data_len < 0) {
        DSTREAM_DEBUG("[%s] bws(%p) size(%d) ack(%lu)",
                      __FUNCTION__, bws, data_len, *ack);
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
    if (m_ack - 1 == *ack) {    //TODO : 修改为先判断transporter::kAdvanceQueueAck， 注意这个值可能的和ack重合的地方
        // check if any pending data
        assert(m_last_data.d_len >= 0);
        // find pending data
        // get first elem of pending list
        data_len = m_last_data.d_len;
        ret = _Pending(&data_len/*INOUT*/,
                       bws/*OUT*/,
                       &batch_count/*OUT*/,
                       true/*replay mode*/);
        assert(m_last_data.d_len == data_len &&
               m_last_data.d_batch == batch_count);
    } else if (transporter::kAdvanceQueueAck == *ack) {
        // new data pending
        // get actual data len
        if (m_r_count < static_cast<uint64_t>(data_len)) {
            // no enough data to pending
            // get all rest data
            data_len = m_r_count;
        }
        ret = _Pending(&data_len/*INOUT*/,
                       bws/*OUT*/,
                       &batch_count/*OUT*/,
                       false/*new data*/);
        if (error::OK == ret) {
            // record last data
            m_last_data.d_len = data_len;
            m_last_data.d_batch = batch_count;
            *ack = _AckForward();
        } else {
            return ret;
        }
    } else {
        DSTREAM_FATAL("[%s] un-corresponding pending data, last ack(%lu), input ack(%lu)",
                      __FUNCTION__, m_ack, *ack);
        // cannot reach here!
    }
    *size = m_last_data.d_len;
    *batch = m_last_data.d_batch;
    return ret;
}

ErrCode QueueBuffer::BatchPop(uint64_t ack,
                              OUT CBufWriteStream* bws,
                              OUT int32_t* size,
                              OUT int32_t* batch) {
    if (0 > m_last_data.d_len) {
        return error::OK_NO_DATA_FLUSHED;
    }
    ErrCode ret = error::OK;
    int32_t pop_size = 0;
    if (1/*transporter::kFlushQueueUnack == ack*/) {
        pop_size = m_last_data.d_len;
        ret = _Pop(&pop_size, bws, batch);
        UNLIKELY_IF(ret < error::OK) {
            return ret;
        }
        assert(pop_size == m_last_data.d_len);
        _ClearPending();

        // shrink mem
        // TODO(maybesomerd@baidu.com): maybe add some strategy
        if (m_buffer_size - m_w_count > m_o_buffer_size) {
            if (m_mem_mgr_ptr) {
                m_mem_mgr_ptr->ReleaseMem(m_o_buffer_size);
                m_buffer_size -= m_o_buffer_size;
                assert(m_buffer_size >= m_o_buffer_size);
            }
        }
    }
    if (size) {
        *size = pop_size;
    }
    return ret;
}

ErrCode QueueBuffer::FlushPop(INOUT int32_t* size,
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
            ret = _DumpData(&expect_size,
                            bws,
                            batch,
                            true/*erase data*/,
                            DUMP_LEAST);
            UNLIKELY_IF(ret < error::OK) {
                return ret;
            }
            // no pending data
            _ClearPending();
            m_w_count -= expect_size;
            m_r_count -= expect_size - pending_size;
        } else {
            // only flush all data in pending
            expect_size = pending_size;
            // maybe pending item len = 0
            if (expect_size <= 0) {
                goto ERR_RET;
            }
            ret = _DumpData(&expect_size,
                            bws,
                            batch,
                            true/*erase data*/,
                            DUMP_LEAST);
            _ClearPending();
            m_w_count -= expect_size;
            assert(expect_size <= pending_size);
        }
    } else {
        // no pending data
        assert(m_last_data.d_len < 0);
        // check expect_size
        if (static_cast<uint64_t>(expect_size) > m_w_count) {
            expect_size = m_w_count;
        }
        if (expect_size <= 0) {
            goto ERR_RET;
        }
        ret = _DumpData(&expect_size,
                        bws,
                        batch,
                        true/*erase data*/,
                        DUMP_LEAST);
        m_w_count -= expect_size;
        m_r_count -= expect_size;
    }
ERR_RET:
    *size = expect_size;
    return ret;
}

} // namespace queue_buffer
} // namespace dstream
