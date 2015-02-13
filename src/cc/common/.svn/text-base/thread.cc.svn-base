/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    thread abstract class
*********************************************************************/

#include "common/logger.h"
#include "common/thread.h"

namespace dstream {
namespace thread {

static void* WorkingThread(void* thread_ptr) {
    Thread* thread = reinterpret_cast<Thread*>(thread_ptr);
    thread->Run();
    return NULL;
}

Thread::~Thread() {
    Stop();
}

bool Thread::Start() {
    MutexLockGuard lock_guard(m_working_lock);
    if (m_working_thread > 0) {
        DSTREAM_INFO("thread[%lu] is already working", m_working_thread);
        return true;
    }
    if (m_stop) {
        DSTREAM_WARN("thread[%lu] has stopped", m_working_thread);
        return false;
    }
    int ret = pthread_create(&m_working_thread, NULL, WorkingThread, this);
    if (0 != ret) {
        m_working_thread = 0;
        DSTREAM_WARN("create working thread fail: %s", strerror(ret));
        return false;
    }
    return (m_working = true);
}

void Thread::Stop(bool join_thread/* = true*/) {
    m_working_lock.lock();
    m_stop    = true;
    m_working = false;
    pthread_t working_thread_tmp = m_working_thread;
    m_working_thread = 0;
    m_working_lock.unlock();
    if (join_thread && working_thread_tmp > 0) {
        m_sleep_condition.Signal();
        pthread_join(working_thread_tmp, NULL);
    }
}

void Thread::set_stop(bool stop) {
    MutexLockGuard lock_guard(m_working_lock);
    m_stop = stop;
}

bool Thread::Join() {
    m_working_lock.lock();
    pthread_t thread_tmp = m_working_thread;
    m_working_lock.unlock();
    if (thread_tmp > 0) {
        pthread_join(thread_tmp, NULL);
    }
    MutexLock lock_guard(m_working_lock);
    m_working = false;
    m_stop = true;
    return true;
}

} // namespace thread
} // namespace dstream
