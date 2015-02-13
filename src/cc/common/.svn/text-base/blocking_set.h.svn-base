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

#ifndef __DSTREAM_CC_COMMON_BLOCKING_SET_H__
#define __DSTREAM_CC_COMMON_BLOCKING_SET_H__

#include <cassert>
#include <deque>
#include <set>
#include "common/condition.h"
#include "common/mutex.h"

namespace dstream {

template<typename T, typename Cmp>
class BlockingSet {
public :
    BlockingSet() :
        m_mutex(), m_notEmpty(m_mutex), m_data_set() {
    }

    void Put(const T& x) {
        MutexLockGuard lock(m_mutex);
        m_data_set.insert(x);
        m_notEmpty.notify();
    }

    T Take() {
        MutexLockGuard lock(m_mutex);
        while (m_data_set.empty()) {
            m_notEmpty.wait();
        }
        assert(!m_data_set.empty());
        typename std::set<T, Cmp>::iterator it = m_data_set.begin();
        T front(*it);
        m_data_set.erase(it);
        return front;
    }

    bool Empty() const {
        MutexLockGuard lock(m_mutex);
        return m_data_set.empty();
    }

    size_t size() const {
        MutexLockGuard lock(m_mutex);
        return m_data_set.size();
    }

private:
    mutable MutexLock   m_mutex;
    Condition           m_notEmpty;
    std::set<T, Cmp>    m_data_set;
};

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_BLOCKING_QUEUE_H__

