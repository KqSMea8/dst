/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_PE_WRAPPER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PE_WRAPPER_H__ // NOLINT

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <float.h>
#include <stdint.h>
#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "processnode/zk_wrapper.h"
#include "common/application_tools.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "common/rwlock.h"
#include "processelement/context.h"
#include "processnode/on_pe_callback.h"
#include "processnode/process.h"
#include "transporter/transporter.h"
#include "transporter/transporter_stubmgr.h"

namespace dstream {
namespace processnode {

class GCPEWrapper : public thread::Thread {
public:
    typedef boost::shared_ptr<GCPEWrapper> GCPEWrapperPtr;
    explicit GCPEWrapper(uint64_t peid)
        : m_peid(peid) {}
    ~GCPEWrapper() {}
    void Run();

private:
    uint64_t m_peid;
};

class CreatePEWrapper : public thread::Thread {
public:
    typedef boost::shared_ptr<CreatePEWrapper> CreatePEWrapperPtr;
    explicit CreatePEWrapper(uint64_t peid)
        : m_peid(peid) {}
    ~CreatePEWrapper() {}
    void Run();

private:
    uint64_t m_peid;
};

class UpdatePEWrapper : public thread::Thread {
public:
    typedef boost::shared_ptr<UpdatePEWrapper> UpdatePEWrapperPtr;
    explicit UpdatePEWrapper(uint64_t peid)
        : m_peid(peid) {}
    ~UpdatePEWrapper() {}
    void Run();

private:
    uint64_t m_peid;
};

typedef CreatePEMessage CompeletePEID;

class PEWrapper : public boost::enable_shared_from_this<PEWrapper>,
        dstream::processelement::internal::Context {
public:
    typedef boost::shared_ptr<PEWrapper> PEWrapperPtr;

    enum PEWrapperType {
        INVALID = 0,
        CREATING,
        RUNNING,
        UPDATING,
        DELETING,
        ZOMBIE,
    };

    PEWrapper();
    virtual ~PEWrapper();

    bool normal_exit() {
        return m_normal_exit;
    }
    void set_normal_exit(bool normal_exit) {
        m_normal_exit = normal_exit;
    }

    std::string name() {
        return m_name;
    }

    uint64_t serial() {
        return m_serial;
    }

    void set_last_assign_time(uint64_t t) {
        m_last_assign_time = t;
    }
    uint64_t last_assign_time() {
        return m_last_assign_time;
    }

    CompeletePEID& cpeid() {
        return m_cpeid;
    }
    void set_cpeid(const CompeletePEID& c_peid) {
        m_cpeid = c_peid;
    }

    PEID& peid() {
        return m_peid;
    }
    void set_peid(const PEID& pe_id) {
        m_peid = pe_id;
    }

    Processor& processor() {
        return m_processor;
    }
    void set_processor(const Processor& p) {
        m_processor = p;
    }

    ProcessorRole role() const {
        return m_processor.role();
    }

    ProcessManager::ProcessPtr process() {
        return m_process;
    }
    void set_process(ProcessManager::ProcessPtr pp) {
        m_process = pp;
    }

    std::string& work_dir() {
        return m_work_dir;
    }

    std::string& app_uri() {
        return m_app_uri;
    }

    std::string& app_name() {
        return m_app_name;
    }

    std::string& log_path() {
        return m_log_path;
    }
    void set_log_path(const std::string& path) {
        m_log_path = path;
    }

    int recv_queue_size() const {
        return m_recv_queue_size;
    }
    int send_queue_size() const {
        return m_send_queue_size;
    }

    void set_flow_control(FlowControl fc) {
        m_flow_control = fc;
    }

    FlowControl& flow_control() {
        return m_flow_control;
    }

    PEWrapperType status() {
        MutexLockGuard lock_guard(m_status_lock);
        return m_status;
    }

    void set_status(PEWrapperType st) {
        MutexLockGuard lock_guard(m_status_lock);
        assert(st >= INVALID && st <= ZOMBIE);
        m_status =  st;
    }

    uint32_t revision() const {
        return m_processor.cur_revision();
    }

    void set_revision(uint32_t ver) {
        m_processor.set_cur_revision(ver);
    }

    void update_revision(uint32_t ver) {
        m_update_revision = ver;
    }

    // --------------- interface --------------
    error::Code CreateSelf();
    error::Code DestroySelf();
    error::Code UpdateSelf();

    error::Code InitLocalPE();
    error::Code DestroyLocalPE();

    error::Code StartCreateThread(uint64_t peid);
    error::Code StartGCThread(uint64_t peid);
    error::Code StartUpdateThread(uint64_t peid);

    // --------------- transporter --------------
    error::Code StartTransporter();
    error::Code DestroyTransporter();

    transporter::Transporter* GetTransporter() {
        m_tp->AddRef();
        return m_tp;
    }
    void ReleaseTranspoter() {
        m_tp->Release();
    }

    // ---------------- utils ----------------
    error::Code UpdatePEConfig();
    error::Code GenProcessEnv(std::map<std::string, std::string>* env);

    error::Code GenLocalPEEnv();
    error::Code ClearLocalPEEnv(const std::string& work_dir);

    error::Code StartPEProcess(const std::map<std::string, std::string>& pe_env);
    void StopPEProcess();
    void RestartPEProcess();
    void NotifyUpdate();

    // for debug & unit test
    std::string DumpPEInfo();
    std::string DumpUpStreamInfo();
    std::string DumpDownStreamInfo();

    bool CheckAndSetStatus(PEWrapperType to_check, PEWrapperType to_set) {
        MutexLockGuard lock_guard(m_status_lock);
        if (m_status == to_check) {
            m_status = to_set;
            return true;
        }
        return false;
    }

    void ResetUpdateThread() {
        if (m_update_thread_ptr) {
            m_update_thread_ptr.reset();
        }
    }
private:
    GCPEWrapper::GCPEWrapperPtr         m_gc_thread_ptr;
    CreatePEWrapper::CreatePEWrapperPtr m_create_thread_ptr;
    UpdatePEWrapper::UpdatePEWrapperPtr m_update_thread_ptr;

    // pe wrapper status
    PEWrapperType m_status;
    // zk meta
    CompeletePEID m_cpeid;
    logConfig     m_log_config;
    Processor     m_processor; 
    PEID          m_peid;     

    // pe info
    ProcessManager::ProcessPtr m_process;
    std::string m_name;
    uint64_t    m_serial;
    uint64_t    m_last_assign_time;
    std::string m_work_dir;
    std::string m_app_uri;
    std::string m_log_path;
    uint64_t    m_tuple_batch_count;
    uint64_t    m_tuple_batch_size;
    std::string m_dynamic_proto_dir;
    int         m_output_frequency;
    int         m_ttl;
    int         m_send_queue_size;
    int         m_recv_queue_size;
    float       m_send_watermark;
    float       m_recv_watermark;
    int         m_data_strategy;
    uint64_t    m_fetch_timeout_ms;
    uint64_t    m_send_timeout_ms;
    int         m_exec_workers;
    int         m_net_workers;
    int         m_disk_workers;
    int         m_small_tran_buf_num;
    int         m_small_tran_buf_size;
    int         m_big_tran_buf_num;
    float       m_buf_low_mark;

    transporter::Transporter* m_tp;

    // flag of normal exit
    volatile bool m_normal_exit;
    FlowControl m_flow_control;
    std::string m_app_name; 

    // for update
    uint32_t  m_update_revision; 
    MutexLock m_update_lock;
    Condition m_update_cond;
    MutexLock m_status_lock;

protected:
    void RunProlog(const std::string& work_dir);
    void RunEpilog(const std::string& work_dir);
};

} // namespace processnode
} // namespace dstream

#endif //  __DSTREAM_CC_PROCESSNODE_PE_WRAPPER_H__ NOLINT
