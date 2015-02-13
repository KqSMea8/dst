#ifndef __DSTREAM_CC_PROCESSNODE_MEM_MGR_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_MEM_MGR_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include <boost/weak_ptr.hpp>

#include "common/error.h"
#include "common/mutex.h"
#include "common/rwlock.h"

namespace dstream {
namespace mem_mgr {

class MemMgr : boost::noncopyable {
public:
    typedef boost::shared_ptr<MemMgr> MemMgrPtr;
    typedef boost::weak_ptr<MemMgr> wMemMgrPtr;

    static MemMgrPtr Instance() {
        MemMgrPtr ins = pInstance.lock();
        if (!ins) {
            ins.reset(new (std::nothrow) MemMgr());
            pInstance = ins;
        }
        return ins;
    }

public:
    MemMgr() : m_total_mem(0), m_free_mem(0) {
        m_stat_buf_len = 4096;
        m_stat_buf = new char[m_stat_buf_len];
    }
    ~MemMgr() {
        delete [] m_stat_buf;
    }
    error::Code Init(uint64_t total_memory);

    // sync interface
    // ---------------------------
    error::Code AllocateMem(uint64_t size);
    error::Code ReleaseMem(uint64_t size);
    uint64_t GetTotalMem() {
        return m_total_mem;
    }
    std::string DumpMemStat();

protected:
    static wMemMgrPtr pInstance;
    static const char* kTraceTag;

private:
    MutexLock m_mem_lock;
    uint64_t m_total_mem;
    uint64_t m_free_mem;

    char* m_stat_buf;
    size_t m_stat_buf_len;
};

} // namespace mem_mgr
} // namespace dstream

#endif // NOLINT
