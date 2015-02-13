/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#ifndef __DSTREAM__CC_PROCESSELEMENT_LOOPBACK_H__ // NOLINT
#define __DSTREAM__CC_PROCESSELEMENT_LOOPBACK_H__ // NOLINT

#include <map>
#include <string>
#include <vector>
#include "common/cglimit_utils.h"
#include "common/counters.h"
#include "common/rpc_const.h"
#include "common/rwlock.h"
#include "processelement/context.h"
#include "processelement/pe_transporter.h"
#include "processelement/tuple.h"
#include "processelement/metric.h"
#include "transporter/transporter.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_stubmgr.h"

namespace dstream {
namespace processelement {

class LoopBack;

// counters to be used by Task
struct TaskCounters {
    common::Uint64RateCounter* recv_tuples_counter;
    common::Uint64RateCounter* recv_bytes_counter;
    common::Uint64RateCounter* send_tuples_counter;
    common::Uint64RateCounter* send_bytes_counter;
    common::Uint64NumCounter*  cpu_usage_counter;
    common::Uint64NumCounter*  mem_usage_counter;
    common::Uint64NumCounter*  importer_queue_counter;
    common::Uint64NumCounter*  importer_latency_counter;

    dstream::CpuStat last_cpu_stat;

    TaskCounters();
    ~TaskCounters();
    bool Init(uint64_t pe_id);
    bool Init(uint64_t pe_id, bool as_importer);
};

class LoopBackHeartbeatService: public rpc_const::PubSubHeartbeatProtocol {
public:
    explicit LoopBackHeartbeatService(LoopBack* loop_back);
    virtual std::string PUB_SUB_HEARTBEAT(const HeartbeatRequest* req,
                                          HeartbeatACK* ack);
private:
    LoopBack* loop_back_;
}; // class LoopBackHeartbeatService

// ------------------------------------------------------------
// LoopBack Interface.
class LoopBack : 
    public internal::Context,
    public internal::Metric,
    public CESPObject {
    ESP_DECLARE_OBJECT(LoopBack)

public:
    LoopBack();
    virtual ~LoopBack() {
        Stop();
    }

    virtual int32_t Start();
    virtual void    Stop();

    // called in OnFetchData to generator user specified tuples
    virtual void LoopSetUp(Tuples* tuples) { }
    // called in OnForwardData to do user specified works
    virtual void LoopTearDown(const Tuples& tuples) {};

    // on function
    virtual void OnReady(ESPMessage* msg);
    virtual void OnFetchData(ESPMessage* msg);
    virtual void OnForwardData(ESPMessage* msg);

    // rpc function
    virtual void OnHeartbeat(uint64_t peid, const std::map< std::string, std::string >& metrics);

    // ----------------------------------------
    typedef std::map<std::string, TupleTypeInfo*> TupleTypeInfoMap;

    TupleTypeInfo* FindTupleTypeInfoByTag(const std::string& tag);
    void   RegisterTupleTypeInfo(const std::string& tag, TupleTypeInfo* info);
    Tuple* AllocateTuple(const std::string& tag);
    void   DeallocateTuple(Tuple* tuple);

protected:
    // transporter data, add by konghui
    int SerialTuples(const Tuples& tuples, CBufWriteStream* bws, int* batch, size_t* size);
    int DeserialTuples(BufHandle* hdl, int batch, size_t size, Tuples* tuples);

protected:
    static const int kLoopBackServiceNumber = 1;

    uint64_t  m_loopback_id;
    ESPSTUB   m_if_stub;
    ESPSTUB   m_of_stub;
    LoopBackHeartbeatService m_srv_heartbeat;
    rpc::Service*            m_services[kLoopBackServiceNumber];
    TupleTypeInfoMap         m_tuple_type_info_map;
    dstream::RWLock          m_type_map_lock;
    Byte*                    m_buf;
    ByteSize                 m_buf_len;

    // --------------------
    // tuple fields.
    Tuples          m_recv_tuples;
    Tuples          m_send_tuples;
    MutexLock       m_send_tuples_lock; // use to protect emit in a another thread

    PEFetchCli*     m_if_cli;
    PEForwardCli*   m_of_cli;
    uint64_t        m_if_ack;
    uint64_t        m_of_ack;

    TaskCounters  m_counters;

//-----------------------------------------------------------------
// !!!!! Abandoned interfaces
public:
    void registerTupleTypeInfo(const std::string& tag, TupleTypeInfo* info) {
        return RegisterTupleTypeInfo(tag, info);
    }
    Tuple* allocateTuple(const std::string& tag) {
        return AllocateTuple(tag);
    }
    void deallocateTuple(Tuple* tuple) {
        return DeallocateTuple(tuple);
    }
}; // class LoopBack;


} // namespace processelement
} // namespace dstream

#endif // __DSTREAM__CC_PROCESSELEMENT_LOOPBACK_H__ NOLINT
