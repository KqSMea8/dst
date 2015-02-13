#include "common/mem_mgr.h"
#include <string>
#include <google/malloc_extension.h>
#include "common/application_tools.h"

namespace dstream {
namespace mem_mgr {

MemMgr::wMemMgrPtr MemMgr::pInstance;
const char* MemMgr::kTraceTag = "MemMgrTrace";

error::Code MemMgr::Init(uint64_t total_memory) {
    m_total_mem = total_memory;
    m_free_mem = m_total_mem;
    return error::OK;
}

error::Code MemMgr::AllocateMem(uint64_t size) {
    error::Code ret = error::OK;
    if (size <= 0) {
        return error::BAD_ARGUMENT;
    }
    MutexLockGuard lock(m_mem_lock);
    if (size > m_free_mem) {
        DSTREAM_WARN("[%s] out of memory", kTraceTag);
        ret = error::BAD_MEMORY_ALLOCATION;
    } else {
        m_free_mem -= size;
        DSTREAM_DEBUG("[%s] allocate %lu Bytes", kTraceTag, size);
    }
    return ret;
}

error::Code MemMgr::ReleaseMem(uint64_t size) {
    error::Code ret = error::OK;
    if (size <= 0) {
        return ret;
    }
    MutexLockGuard lock(m_mem_lock);
    m_free_mem += size;
    assert(m_total_mem == 0 || m_free_mem <= m_total_mem);

    DSTREAM_DEBUG("[%s] release %lu Bytes", kTraceTag, size);
    MallocExtension::instance()->ReleaseFreeMemory();
    return ret;
}

std::string MemMgr::DumpMemStat() {
    std::string dump_str;
    MallocExtension::instance()->GetStats(m_stat_buf, m_stat_buf_len);
    dump_str.assign(m_stat_buf);
    return dump_str;
}

} // namespace mem_mgr
} // namespace dstream

