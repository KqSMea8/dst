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

#include "processnode/process.h"
#include <sys/wait.h>
#include "processnode/fd_capture.h"

namespace dstream {
namespace processnode {
Process::Process(uint32_t processid, OnExitCallback* callback)
    : m_pid(processid), m_input_stream(0), m_output_stream(0),
    m_error_stream(0), m_on_exit_callback(callback) {
}
int Process::GetInputStream() {
    return m_input_stream;
}
int Process::GetOutputStream() {
    return m_output_stream;
}
int Process::GetErrorStream() {
    return m_error_stream;
}
pid_t Process::pid() const {
    return m_pid;
}

Process::~Process() {
}

OnExitCallback* Process::GetOnExitCallback() const {
    return m_on_exit_callback;
}
void Process::Destroy() {
    DSTREAM_INFO("send SIGKILL signal to pe process(pid:%u)", m_pid);
    kill(-m_pid, SIGKILL); // kill all processes in pe process group
}

void Process::Stop() {
    // notify pe to stop
    kill(m_pid, SIGTERM);
    DSTREAM_INFO("send SIGTERM to pe process(pid:%u)", m_pid);
}

void Process::DestroyAllChildren() {
    ProcessManager* process_manager = ProcessManager::GetInstance();
    process_manager->DestroyAllProcesses();
}

void Process::SetStdOutWatcher(const FdWatcher::FdWatcherPtr& outw) {
    m_outw = outw;
}

void Process::SetStdErrWatcher(const FdWatcher::FdWatcherPtr& errw) {
    m_errw = errw;
}

} // namespace processnode
} // namespace dstream
