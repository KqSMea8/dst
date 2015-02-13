/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <string>

#include "processnode/pe_wrapper_manager.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/file_meta.h"
#include "common/logger.h"
#include "common/utils.h"

#include "processnode/fd_capture.h"
#include "processnode/on_pe_callback.h"
#include "processnode/pn_reporter.h"

namespace dstream {
namespace processnode {
OnPEStartCallback::OnPEStartCallback(const std::string& pe_log_dir, const std::string& peid,
        int max_file_size, const std::string& pe_work_dir)
        :m_pe_log_dir(pe_log_dir),
        m_peid(peid),
        m_max_file_size(max_file_size),
        m_pe_work_dir(pe_work_dir) {
}

void OnPEStartCallback::Call(ProcessManager::ProcessPtr process) {
    filemeta::WriteMeta(config_const::kPeProcFilePath, NumberToString(process->pid()),
            m_pe_work_dir);
    FdCapture* capture = FdCapture::GetInstance();
    if (!IsDir(m_pe_log_dir.c_str())) {
        string mkdir_cmd = "mkdir -p " + m_pe_log_dir;
        system(mkdir_cmd.c_str());
        DSTREAM_INFO("run cmd [%s]", mkdir_cmd.c_str());
    }
    if (!IsDir(m_pe_log_dir.c_str())) {
        DSTREAM_WARN("create PE log dir in [%s] failed...", m_pe_log_dir.c_str());
    }
    FdWatcher::FdWatcherPtr outw(new FdWatcher(m_pe_log_dir, m_peid + ".out",
                process->GetOutputStream(), m_max_file_size));
    FdWatcher::FdWatcherPtr errw(new FdWatcher(m_pe_log_dir, m_peid + ".err",
                process->GetErrorStream(), m_max_file_size));
    capture->AddWatcher(outw);
    capture->AddWatcher(errw);
    DSTREAM_INFO("added fd watchers for pe [%s]", m_peid.c_str());
}

void OnPEExitCallback::Call(int pid, int status, int exit_code, bool exited) {
    DSTREAM_INFO("get pe:[%lu] exit signal, exit code:[%d], [%s] whose pid is [%d],status[%d]",
                 m_peid.id(), exit_code, exited ? "normal exit" : "killed", pid, status);

    // clear pe enviroment
    PEWrapper::PEWrapperPtr wrapper =
            PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(m_peid.id());
    if (wrapper && wrapper->status() != PEWrapper::UPDATING) {
        wrapper->set_status(PEWrapper::DELETING);
        error::Code ret = wrapper->StartGCThread(m_peid.id());
        assert(ret == error::OK);
        DSTREAM_INFO("GC thread for pe(%lu) start OK.", m_peid.id());
    }

    // clear pe meta file
    DSTREAM_DEBUG("clear pe env successful for peid [%lu] pid [%d]", m_peid.id(), pid);
    std::string pe_dir;
    if (filemeta::GetMeta(config_const::kPeProcFilePath, NumberToString(pid),
                &pe_dir) != error::OK) {
        DSTREAM_WARN("Can not find the pe_dir [%s] for pe [peid->%lu]",
                pe_dir.c_str(), m_peid.id());
    }

    if (filemeta::ClearMeta(config_const::kPeProcFilePath, NumberToString(pid)) != error::OK) {
        DSTREAM_WARN("ClearMeta [%d] in path [%s] failed", pid, config_const::kPeProcFilePath);
    }

    DSTREAM_DEBUG("clear meta for pe whose pid is %d successful", pid);

    // to notify updating thread
    if (wrapper && wrapper->status() == PEWrapper::UPDATING) {
        wrapper->NotifyUpdate();
    }
}

} // namespace processnode
} // namespace dstream
