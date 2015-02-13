/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <time.h>
#include "common/condition.h"
#include <time.h>
#include "common/logger.h"

namespace dstream {

// ------------------------------------------------------------
// Condition Implementation

Condition::Condition(MutexLock& mutex): mutex_(mutex) { // NOLINT
    int ret = pthread_cond_init(&cond_, NULL);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_init(%p) failed(%s)", &cond_, strerror(ret));
    }
}

Condition::~Condition() {
    int ret = pthread_cond_destroy(&cond_);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_destroy(%p) failed(%s)", &cond_, strerror(ret));
    }
}

void Condition::wait() {
    int ret = pthread_cond_wait(&cond_, mutex_.getPthreadMutex());
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_wait(%p) failed(%s)", &cond_, strerror(ret));
    }
}

/*void Condition::timed_wait(const struct timespec* abstime) {
  int ret = pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), abstime);
  if(ret < 0) {
    DSTREAM_FATAL("pthread_cond_timedwait(%p) failed(%s)", &cond_, strerror(ret));
  }
}*/

void Condition::timed_wait(const int32_t millseconds) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += millseconds / 1000;
    ts.tv_nsec += (millseconds % 1000) * 1000 * 1000;

    int ret = pthread_cond_timedwait(&cond_, mutex_.getPthreadMutex(), &ts);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_timedwait(%p) failed(%s)", &cond_, strerror(ret));
    }
}

void Condition::notify() {
    int ret = pthread_cond_signal(&cond_);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_signal(%p) failed(%s)", &cond_, strerror(ret));
    }
}

void Condition::notifyAll() {
    int ret = pthread_cond_broadcast(&cond_);
    if (ret < 0) {
        DSTREAM_FATAL("pthread_cond_broadcast(%p) failed(%s)", &cond_, strerror(ret));
    }
}

} // namespace dstream
