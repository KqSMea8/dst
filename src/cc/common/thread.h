/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    thread abstract class
*********************************************************************/

#ifndef __DSTREAM_COMMON_THREAD_H__ // NOLINT
#define __DSTREAM_COMMON_THREAD_H__ // NOLINT

#include <pthread.h>
#include "common/condition.h"
#include "common/mutex.h"

namespace dstream {
namespace thread {

/************************************************************************/
/* thread abstract class                                                */
/************************************************************************/
class Thread {
public:
    Thread() : m_stop(false), m_working(false), m_working_thread(0) {}
    virtual ~Thread();
    bool Start();
    virtual void Run() = 0;
    virtual void Stop(bool join_thread = true);
    void set_stop(bool stop);
    bool Join();
    bool working() {
        return m_working;
    }
protected:
    bool m_stop;
    bool m_working;
    pthread_t m_working_thread;
    MutexLock m_working_lock;
    CCond m_sleep_condition;
};

} // namespace thread
} // namespace dstream

#endif // __DSTREAM_COMMON_THREAD_H__ //NOLINT
