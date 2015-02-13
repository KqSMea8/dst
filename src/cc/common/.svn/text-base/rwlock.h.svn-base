/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief read write lock interface.
 *
 */

#ifndef __DSTREAM_CC_COMMON_RWLOCK_H__ // NOLINT
#define __DSTREAM_CC_COMMON_RWLOCK_H__ // NOLINT

#include <pthread.h>
namespace dstream {

// ------------------------------------------------------------
// Read Write Lock
class RWLock {
public:
    RWLock();
    ~RWLock();
    void LockForRead();
    void LockForWrite();
    void Unlock();
private:
    pthread_rwlock_t m_lock;
};

// ------------------------------------------------------------
// rw lock guard use C++ ctor and dtor.
class RWLockGuard {
public:
    explicit RWLockGuard(RWLock& mutex, bool is_write): m_mutex(mutex) { //NOLINT
        if (is_write) {
            m_mutex.LockForWrite();
        } else {
            m_mutex.LockForRead();
        }
    }
    ~RWLockGuard() {
        m_mutex.Unlock();
    }
private:
    RWLock& m_mutex;
}; // class MutexLockGuard

} // namespace dstream

// Prevent misuse like:
// MutexLockGuard(mutex_);
// A tempory object doesn't hold the lock for long!
#define RWLockGuard(x, y) error "Missing guard object name"

#endif // __DSTREAM_CC_COMMON_RWLOCK_H__ NOLINT
