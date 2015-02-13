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


#include "processnode/process_manager.h"

#include <dirent.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#include "common/config_const.h"
#include "common/file_meta.h"
#include "common/logger.h"
#include "common/utils.h"

#include "processnode/process.h"

namespace dstream {
namespace processnode {

static inline std::string GetProcFilePath(int pid) {
    std::string path_pre = config_const::kPeProcFilePath;
    StringAppendNumber(&path_pre , pid);
    return path_pre;
}

ProcessManager* ProcessManager::m_instance = NULL;

bool ProcessManager::DelayCleanUp() {
    std::map<std::string, std::string> meta;
    if (filemeta::GetMeta(config_const::kPeProcFilePath, &meta) != error::OK) {
        DSTREAM_WARN("try to get meta from path [%s]...but not found",
                     config_const::kPeProcFilePath);
    }
    typedef std::map<std::string, std::string>::iterator Iter;
    int pid = 0;
    for (Iter it = meta.begin(); it != meta.end(); it++) {
        pid = atoi(it->first.c_str());
        if (pid == 0) {
            DSTREAM_WARN("file [%s] shouldn't in path [%s]",
                         it->first.c_str(), config_const::kPeProcFilePath);
            continue;
        }
        std::string cmd = "rm -Rf " + it->second + "_bak";
        system(cmd.c_str());
        DSTREAM_DEBUG("run system(%s) to delete pe dir", cmd.c_str());
        cmd = "mv " + it->second + " " + it->second + "_bak";
        system(cmd.c_str());
        DSTREAM_DEBUG("run system(%s) to remove pe dir", cmd.c_str());
        kill(pid, SIGKILL);
    }
    filemeta::ClearMeta(config_const::kPeProcFilePath, meta);
    return true;
}

ProcessManager* ProcessManager::GetInstance() {
    if (m_instance != NULL) {
        return m_instance;
    }
    return m_instance = new ProcessManager();
}

ProcessManager::~ProcessManager() {
    Process::DestroyAllChildren();
    Running = false;
}

void ProcessManager::DestroyAllProcesses() {
    DSTREAM_INFO("start to destroy all child processes");
    std::map<int, ProcessPtr>::iterator iter;
    {
        MutexLockGuard lock_guard(m_lock);
        for (iter = m_process_map.begin(); iter != m_process_map.end(); ++iter) {
            DSTREAM_INFO("start to destroy process %d", iter->first);
            iter->second->Destroy();
        }
    }

    m_lock.lock();
    while (!m_process_map.empty()) {
        m_lock.unlock();
        usleep(50);
        m_lock.lock();
    }
    m_lock.unlock();
}
typedef void (sa_handler_t)(int);
sa_handler_t* signal_replaced_(int signum, sa_handler_t* handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); // block sigs of type being handled
    action.sa_flags = SA_RESTART; // restart syscall if possible
    if (sigaction(signum, &action, &old_action) < 0) {
        DSTREAM_WARN("SIGNAL ERROR");
    }
    return old_action.sa_handler;
}
/*static void on_term_sigs(int sig){
  DSTREAM_WARN("process recieved a signal %d ",sig);
  signal_replaced_(sig,SIG_DFL);
  ProcessManager::GetInstance()->DestroyAllProcesses();
  if(kill(getpid(),sig)<0){
    DSTREAM_WARN("send signal [%d] to itself [%d] failed",sig,getpid());
  }
}
*/
static int term_signals[] = {SIGABRT, SIGFPE, SIGINT, SIGSEGV};
ProcessManager::ProcessManager() {
    for (uint32_t i = 0; i != sizeof(term_signals) / sizeof(int); i++) { // NOLINT
        // signal_replaced_(term_signals[i],on_term_sigs);
    }
    Running = true;
}

ProcessManager::ProcessPtr ProcessManager::CreateProcess(int pid, OnExitCallback* callback) {
    DSTREAM_INFO("Create process pid:%d", pid);
    ProcessManager::ProcessPtr new_proc(new Process(pid, callback));
    MutexLockGuard lock_guard(m_lock);
    if (m_process_map.find(pid) != m_process_map.end()) {
        DSTREAM_ERROR("Unknown error:create two process return same pid. ");
        return ProcessManager::ProcessPtr();
    }
    m_process_map[pid] = new_proc;
    return new_proc;
}

ProcessManager::ProcessPtr ProcessManager::QueryProcess(int pid) {
    MutexLockGuard lock_guard(m_lock);
    std::map<int, ProcessPtr>::iterator find_proc = m_process_map.find(pid);
    if (find_proc != m_process_map.end()) {
        return find_proc->second;
    }
    DSTREAM_WARN("QueryProcess %d failed", pid);
    return ProcessPtr();
}

void ProcessManager::DestroyProcess(int pid) {
    MutexLockGuard lock_guard(m_lock);
    std::map<int, ProcessPtr>::iterator find_proc = m_process_map.find(pid);
    DSTREAM_INFO("Destroy process : %d", pid);
    if (find_proc == m_process_map.end()) {
        DSTREAM_WARN("Destory process %d ,but the process is not exist", pid);
        return;
    }
    m_process_map.erase(find_proc);
    DSTREAM_INFO("destroy process, process map size: %zd", m_process_map.size());
}

} // namespace processnode
} // namespace dstream
