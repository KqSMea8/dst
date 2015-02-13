#include "transporter/transporter_queue.h"

namespace dstream {
namespace transporter {

TPAckQueue::TPAckQueue(uint64_t owner_id,
                       uint64_t vice_id,
                       const std::string& name,
                       const std::string& cnt_grp) :
    m_buf(NULL),
    m_name(name),
    m_capacity(config_const::kPNSendQueueSize),
    m_watermark(config_const::kPNSendWaterMark),
    m_owner_id(owner_id),
    m_vice_id(vice_id),
    m_cnt_grp(cnt_grp) {
    // xfered_num_(0),
    // remain_num_(0),
    // droped_num_(0),
    // xfered_size_(0),
    // remain_size_(0),
    // droped_size_(0) {
}

ErrCode TPAckQueue::Init(uint64_t capacity, double watermark) {
    // Alloc buffer
    SAFE_DELETE(m_buf);
    m_buf = new (std::nothrow) TRANS_BUFFER_TPYE(capacity,
                                                mem_mgr::MemMgr::Instance(),
                                                watermark);
    UNLIKELY_IF(NULL == m_buf) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    m_capacity  = capacity;
    m_watermark = watermark;
    return error::OK;
}

ErrCode TPAckQueue::Destroy() {
    SAFE_DELETE(m_buf);
    return error::OK;
}

} // namespace transporter
} // namespace dstream
