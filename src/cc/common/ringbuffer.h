#ifndef __DSTREAM_SRC_CC_COMMON_RINGBUFFER_H__
#define __DSTREAM_SRC_CC_COMMON_RINGBUFFER_H__

#include <stdint.h>

namespace dstream {
namespace common {

class RingBuffer {
public:
    RingBuffer(int32_t max);
    virtual ~RingBuffer();

    int32_t Init();
    inline bool IsEmpty() {
        return m_count == 0;
    }
    inline bool IsEnough(int32_t size) {
        return m_max > (m_count + size);
    }
    inline bool ReachWaterMark() {
        return m_count > (m_max * m_watermark);
    }
    int32_t Put(const char* buf, int32_t len);
    int32_t Get(char* buf, int32_t len);

private:
    char* m_buffer;
    int32_t m_max;
    int32_t m_head;
    int32_t m_tail;
    int32_t m_count;
    double m_watermark;
};

} // namespace common
} // namespace dstream

#endif
