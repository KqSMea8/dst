/** -*- c++ -*-
 * Copyright (C) 2011 Realtime Team (Baidu, Inc)
 *
 * This file is part of DStream.
 *
 * DStream is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 */

#ifndef __DSTREAM_CC_COMMON_MUTEX_H__ // NOLINT
#define __DSTREAM_CC_COMMON_MUTEX_H__ // NOLINT

#include <pthread.h>
#include <sys/types.h>

namespace dstream {

// ------------------------------------------------------------
// attributes of current threads
class CurrentThread {
public:
    static pid_t tid();
}; // class CurrentThread

// ------------------------------------------------------------
// mutex lock interface.
class MutexLock {
public:
    MutexLock();
    ~MutexLock();
    void lock();
    int  trylock();
    void unlock();
    pthread_mutex_t* getPthreadMutex();
private:
    pthread_mutex_t m_mutex;
}; // MutexLock

// ------------------------------------------------------------
// mutex lock guard use C++ ctor and dtor.
class MutexLockGuard {
public:
    explicit MutexLockGuard(MutexLock& mutex)   // NOLINT
        : m_mutex(mutex) {
        m_mutex.lock();
    }
    ~MutexLockGuard() {
        m_mutex.unlock();
    }

private:
    MutexLock& m_mutex;
}; // class MutexLockGuard

#define DS_SCOPE_LOCK_BEGIN(mutex) \
    { \
    MutexLockGuard lock(mutex);

#define DS_SCOPE_LOCK_END() \
    }

} // namespace dstream

// Prevent misuse like:
// MutexLockGuard(m_mutex);
// A tempory object doesn't hold the lock for long!
#define MutexLockGuard(x) error "Missing guard object name"

#endif // NOLINT
