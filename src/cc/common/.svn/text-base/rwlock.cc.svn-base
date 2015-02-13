/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief read write lock implementation.
 *
 */

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "common/logger.h"
#include "common/rwlock.h"

namespace dstream {

// ------------------------------------------------------------
// RWLock Implementation
RWLock::RWLock() {
    int ret = pthread_rwlock_init(&m_lock, NULL);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_rwlock_init(%p) failed(%s)", &m_lock, strerror(ret));
    }
}

RWLock::~RWLock() {
    int ret = pthread_rwlock_destroy(&m_lock);
    if (ret < 0) {
        DSTREAM_WARN("pthread_rwlock_destroy(%p) failed(%s)", &m_lock, strerror(ret));
    }
}

void RWLock::LockForRead() {
    int ret = pthread_rwlock_rdlock(&m_lock);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_rwlock_rdlock(%p) failed(%s)", &m_lock, strerror(ret));
    }
}

void RWLock::LockForWrite() {
    int ret = pthread_rwlock_wrlock(&m_lock);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_rwlock_wrlock(%p) failed(%s)", &m_lock, strerror(ret));
    }
}

void RWLock::Unlock() {
    int ret = pthread_rwlock_unlock(&m_lock);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_rwlock_unlock(%p) failed(%s)", &m_lock, strerror(ret));
    }
}

} // namespace dstream
