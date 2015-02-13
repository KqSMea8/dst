#include "common/ringbuffer.h"
#include "common/error.h"
#include "common/logger.h"

namespace dstream {
namespace common {

RingBuffer::RingBuffer(int max) : m_max(max), m_watermark(0.8) {
}

RingBuffer::~RingBuffer() {
    if (NULL != m_buffer) {
        delete []m_buffer;
        m_buffer = NULL;
    }
}

int32_t RingBuffer::Init() {
    m_buffer = new (std::nothrow) char[m_max];
    if (NULL == m_buffer) {
        DSTREAM_WARN("allocate memory fail");
        return error::BAD_MEM_ALLOC;
    }
    m_head = m_tail = m_count = 0;
    return error::OK;
}

int32_t RingBuffer::Put(const char* buf, int32_t len) {
    // judge if enough space to put
    if (m_count + len > m_max) {
        DSTREAM_WARN("ring buffer no enough space");
        return error::QUEUE_FULL;
    }
    int32_t rear_len = m_max - m_tail;
    // put buffer accross queue size
    if (rear_len < len) {
        memcpy(m_buffer + m_tail, buf, rear_len);
        memcpy(m_buffer, buf + rear_len, len - rear_len);
        m_tail = len - rear_len;
    // put buffer not accross queue size
    } else {
        memcpy(m_buffer + m_tail, buf, len);
        m_tail += len;
    }
    m_count += len;
    return error::OK;
}

int32_t RingBuffer::Get(char* buf, int32_t len) {
    if (IsEmpty()) {
        return error::QUEUE_EMPTY;
    }
    int32_t rear_len = m_max - m_head;
    // get buffer accross queue size
    if (rear_len < len) {
        memcpy(buf, m_buffer + m_head, rear_len);
        memcpy(buf + rear_len, m_buffer, len - rear_len);
        m_head = len - rear_len;
    // get buffer not accross queue size
    } else {
        memcpy(buf, m_buffer + m_head, len);
        m_head += len;
    }
    m_count -= len;
    return error::OK;
}

} // namespace common
} // namespace dstream
