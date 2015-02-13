/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM__CC_PROCESSNODE_ON_PE_CALLBACK_H__ // NOLINT
#define __DSTREAM__CC_PROCESSNODE_ON_PE_CALLBACK_H__ // NOLINT

#include <string>
#include "processnode/process.h"
#include "processnode/process_builder.h"

namespace dstream {
namespace processnode {

class OnPEStartCallback: public OnStartCallback {
public:
    virtual ~OnPEStartCallback() {}
    void Call(ProcessManager::ProcessPtr process);
    // TODO(zhenpeng) max filesize is not implements now.
    // If done,max_file_size <=0 means no limit
    OnPEStartCallback(const std::string& pe_log_dir,
            const std::string& peid,
            int max_file_size,
            const std::string& pe_work_dir);
private:
    std::string m_pe_log_dir;
    std::string m_peid;
    int         m_max_file_size;
    std::string m_pe_work_dir;
};


class OnPEExitCallback: public OnExitCallback {
public:
    virtual ~OnPEExitCallback() {}
    void Call(int pid, int status, int exit_code, bool exited);
    PEID peid() const {
        return m_peid;
    }
    void set_peid(const PEID& id) {
        m_peid = id;
    }

private:
    PEID  m_peid;
};


} // namespace processnode
} // namespace dstream
#endif // __DSTREAM__CC_PROCESSNODE_ON_PE_CALLBACK_H__ NOLINT
