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

#ifndef __DSTREAM_CC_PROCESSNODE_PROCESS_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PROCESS_H__ // NOLINT

#include <iostream>
#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <string>
#include <unistd.h>

#include "processnode/fd_capture.h"
#include "processnode/process_builder.h"
#include "processnode/process_manager.h"

namespace dstream {
namespace processnode {

class OnExitCallback {
public:
    // the second param indicates if the process is exited normally,
    // if the child process is killed by signal, this value will be false.
    virtual void Call(int pid, int status, int exit_code, bool exited) = 0;
    virtual ~OnExitCallback() {}
};
class OnStartCallback {
    // this Callback will be called before the wait thread start.
    // note: this callback will be called in a lock area.
public:
    virtual void Call(ProcessManager::ProcessPtr process) = 0;
    virtual ~OnStartCallback() {}
};

class Process {
public:
    pid_t pid() const;
    int GetInputStream();
    int GetOutputStream();
    int GetErrorStream();
    ~Process();
    friend class ProcessManager;
    friend class ProcessBuilder;
    OnExitCallback* GetOnExitCallback() const;
    void Destroy();
    void Stop();
    static void DestroyAllChildren();

    void SetStdOutWatcher(const FdWatcher::FdWatcherPtr& outw);
    void SetStdErrWatcher(const FdWatcher::FdWatcherPtr& errw);

private:
    Process(uint32_t processid, OnExitCallback* on_exit_callback = NULL);
    uint32_t m_pid;
    int m_input_stream;
    int m_output_stream;
    int m_error_stream;
    int m_status;
    OnExitCallback* m_on_exit_callback;
    pthread_t m_wait_thread_id;

    FdWatcher::FdWatcherPtr m_outw; // fdwatcher for stdout
    FdWatcher::FdWatcherPtr m_errw; // fdwatcher for stderr
};

} // namespace processnode
} // namespace dstream

#endif // NOLINT
