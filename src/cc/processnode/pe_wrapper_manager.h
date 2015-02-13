/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:      2012/07/09
* @filename:    pe_wrapper_manager.h
* @author:      lanbijia
* @brief:       PEWrapper manager
*/


#ifndef __DSTREAM_CC_PROCESSNODE_PE_WRAPPER_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PE_WRAPPER_MANAGER_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <map>
#include <string>
#include <vector>

#include "processnode/pe_wrapper.h"
#include "common/application_tools.h"
#include "common/blocking_set.h"
#include "common/thread.h"
#include "common/error.h"
#include "processelement/context.h"

#include "processnode/fd_capture.h"
#include "processnode/pn_reporter.h"
#include "processnode/process_builder.h"

namespace dstream {
namespace processnode {

#define MAX_QUEUE_LEN 100

class LogCaptureThread: public thread::Thread {
public:
    explicit LogCaptureThread(int max_roll_time_);
    ~LogCaptureThread();

    void Run();

private:
    int max_roll_time_;
};

class PEWrapperManager {
public:
    typedef std::map<uint64_t, PEWrapper::PEWrapperPtr> PEWrapperMap;
    typedef boost::shared_ptr<PEWrapperManager> PEWrapperManagerPtr;
    typedef boost::weak_ptr<PEWrapperManager> wPEWrapperManagerPtr;

    PEWrapperManager();
    virtual ~PEWrapperManager();

    // for debug
    std::string DumpPEWrapperMapInfo();
    std::string DumpPEUpDownStreamInfo(uint64_t peid);
    std::string DumpPEIDList();
    // for httpd server
    error::Code Dump(std::string* output);

    std::string PrintStatus(); // dump status, add by konghui

    // for PN reporter
    int32_t GetPEInfo(proto_rpc::RPCRequest<PNReport>* req,
            std::vector<PEID>* peid_vector);
    void ClearFailPEInfo(std::vector<PEID>* peid_vector);

    // util functions
    PEWrapper::PEWrapperPtr GetPEWrapper(uint64_t peid);
    error::Code CheckAndInsertPEWrapper(
        const PEID& peid,
        const AppID& app_id,
        const Processor& processor,
        const BackupPEID& bpeid,
        const uint64_t last_assign_time);
    error::Code RemovePEWrapper(uint64_t peid);
    error::Code GCPEWrapper(uint64_t peid);
    void DestoryAllPEWrapper();

    // for Unit Test
    PEWrapperMap& GetPEWrapperMap() {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
        return m_pe_wrapper_map;
    }
    PEWrapperMap& GetFailPEWrapperMap() {
        MutexLockGuard lock_guard(m_pe_failed_map_lock);
        return m_pe_failed_map;
    }
    // singleton
    static PEWrapperManagerPtr GetPEWrapperManager();

protected:
    RWLock       m_pe_wrapper_map_lock;
    PEWrapperMap m_pe_wrapper_map;

    MutexLock    m_pe_failed_map_lock;
    PEWrapperMap m_pe_failed_map; 

private:
    LogCaptureThread m_log_capture_thread;

    volatile bool m_is_destory;
    static wPEWrapperManagerPtr m_instance;
};

} // namespace processnode
} // namespace dstream


#endif // NOLINT

