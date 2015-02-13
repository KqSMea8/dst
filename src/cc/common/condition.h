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

#ifndef __DSTREAM_CC_COMMON_CONDITION_H__
#define __DSTREAM_CC_COMMON_CONDITION_H__

#include <pthread.h>
#include "SyncObj.h"    // from kylin
#include "common/logger.h"
#include "common/mutex.h"

namespace dstream {

class Condition {
public:
    explicit Condition(MutexLock& mutex);   // NOLINT
    ~Condition();
    void wait();
    // void timed_wait(const struct timespec* abstime);
    void timed_wait(const int32_t millseconds);
    void notify();
    void notifyAll();
private:
    MutexLock& mutex_;
    pthread_cond_t cond_;
}; // class Condition

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_CONDITION_H__
