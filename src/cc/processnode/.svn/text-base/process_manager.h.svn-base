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

#ifndef __DSTREAM_CC_PROCESSNODE_PROCESS_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PROCESS_MANAGER_H__ // NOLINT

#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <iostream>
#include <map>
#include <string>

#include "common/mutex.h"

namespace dstream {
namespace processnode {

class Process;
class OnExitCallback;

class ProcessManager {
public:
    typedef boost::shared_ptr<Process> ProcessPtr;
    typedef boost::weak_ptr<Process> WkProcessPtr;

    static ProcessManager* GetInstance();
    static bool DelayCleanUp();
    bool Running;

    ~ProcessManager();
    ProcessPtr CreateProcess(int pid, OnExitCallback* on_exit_callback);
    ProcessPtr QueryProcess(int pid);
    void DestroyProcess(int pid);
    void DestroyAllProcesses();

private:
    MutexLock m_lock;
    // map pid -> process structure
    std::map<int, ProcessPtr> m_process_map;

    ProcessManager();
    static ProcessManager* m_instance;
}; // class ProcessBuilder

} // namespace processnode
} // namespace dstream

#endif // NOLINT

