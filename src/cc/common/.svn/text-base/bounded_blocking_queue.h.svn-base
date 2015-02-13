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

#ifndef __DSTREAM_CC_COMMON_BOUNDED_BLOCKING_QUEUE_H__
#define __DSTREAM_CC_COMMON_BOUNDED_BLOCKING_QUEUE_H__

#include <cassert>
#include <deque>
#include "common/condition.h"
#include "common/mutex.h"

namespace dstream {

template<typename T>
class BoundedBlockingQueue {
public:
    explicit BoundedBlockingQueue(int maxSize):
        m_mutex(), m_notEmpty(m_mutex), m_notFull(m_mutex), m_queue() {
        m_size = maxSize;
    }

    void Put(const T& x) {
        MutexLockGuard lock(m_mutex);
        while (m_queue.size() == m_size) {
            m_notFull.wait();
        }
        assert(!(m_queue.size() == m_size));
        m_queue.push_back(x);
        m_notEmpty.notify();
    }

    T Take() {
        MutexLockGuard lock(m_mutex);
        while (m_queue.empty()) {
            m_notEmpty.wait();
        }
        assert(!m_queue.empty());
        T front(m_queue.front());
        m_queue.pop_front();
        m_notFull.notify();
        return front;
    }

    bool Empty() const {
        MutexLockGuard lock(m_mutex);
        return m_queue.empty();
    }

    bool Full() const {
        MutexLockGuard lock(m_mutex);
        return m_queue.size() == m_size;
    }

    size_t size() const {
        MutexLockGuard lock(m_mutex);
        return m_queue.size();
    }

private:
    mutable MutexLock   m_mutex;
    Condition           m_notEmpty;
    Condition           m_notFull;
    std::deque<T>       m_queue;
    size_t              m_size;
}; // class BoundedBlockingQueue

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_BOUNDED_BLOCKING_QUEUE_H__
