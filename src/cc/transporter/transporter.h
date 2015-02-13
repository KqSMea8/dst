/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_TRANSPORTER_H__
#define __DSTREAM_CC_PROCESSNODE_TRANSPORTER_H__

#include <boost/unordered_map.hpp>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "common/id.h"
#include "common/utils.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_counters.h"
#include "transporter/transporter_inflow.h"
#include "transporter/transporter_outflow.h"
#include "transporter/transporter_protocol.h"
#include "transporter/transporter_tag.h"

namespace dstream {
namespace transporter {

/**
* @brief  Transporter works as transport layer for local pe. Eacho local pe
*         owns one transporter.
*         The transporter subsribe data from upstream pes and feeds dstream pes
*         data received from local pe.
*         -SUBSCRIBE
*         Tranportor subsribes data by the class Inflow, which will fetch data
*         periodically. The inflow can manage several upstream pes referd to
*         as Source.
*         -PUBLISH
*         Tranportor stores local pe's result data into Outflows, and pops
*         them to the downdstreams when receiving rpc. Each Outflow is responsibile
*         to a downdstream processor.
* @notice - Access Transporter by pointer instead of object
*         - This object shares lifecycle with local pe.
*/
class Transporter: public CESPObject {
    ESP_DECLARE_OBJECT(Transporter);

public:
    /**
    * @brief    constructor
    * @param    id      local pe id
    * @param    stub    Transporter stub
    * @param    if_stub inflow stub
    */
    Transporter(uint64_t id, ESPSTUB stub, ESPSTUB if_stub = INVALID_ESPSTUB);
    /**
    * @brief    start synchronously
    * @return   error::OK(=0): ok;
    *           other errcode(<0): fail;
    * @notice   not thread-safe
    */
    ErrCode Start();
    /**
    * @brief    stop asynchronously
    * @notice   thread-safe
    */
    void    Stop();
    /**
    * @brief    release
    * @return   reference count, object will be destroyed when if count=0
    * @author   konghui, konghui@baidu.com
    */
    int     Release();

public:
    //-----------------------------------------------
    // inflow management interface
    /**
    * @brief    Add/Del/Upd source pe
    * @param    tp pointer to local pe transporter
    * @param    sources the source info.(del will only take id)
    * @return   error::OK(=0): ok;
    *           other errcode(<0): fail;
    * @notice   `sources` will be free internal
    */
    ErrCode AddIfSource(const std::vector<IfSourceMeta>* sources);
    ErrCode DelIfSource(const std::vector<IfSourceMeta>* sources);
    ErrCode UpdIfSource(const std::vector<IfSourceMeta>* sources);

    //-----------------------------------------------
    // outflow management interface
    /**
    * @brief    Add/Del/Upd source pe
    * @param    tp pointer to local pe transporter
    * @param    sources the source info.(`del` will only take id)
    * @return   error::OK(=0): ok;
    *           other errcode(<0): fail;
    * @notice   `outflows` will be free internal
    */
    ErrCode AddOutflow(const std::vector<OfMeta>* outflows);
    ErrCode DelOutflow(const std::vector<OfMeta>* outflows);
    ErrCode UpdOutflow(const std::vector<OfMeta>* outflows);

    //-----------------------------------------------
    // outflow data interface
    /**
    * @brief     recv/fetch data to outflow
    * @param[IN] esp_msg esp msg
    * @author    konghui, konghui@baidu.com
    * @date      2013-01-13
    */
    MOCKVIRTUAL void OnOutflowRecv(ESPMessage* esp_msg);
    MOCKVIRTUAL void OnOutflowFetch(ESPMessage* esp_msg);

public:
    //-----------------------------------------------
    // set/get memeber
    /**
    * @brief    set data stategy
    * @return   error::OK
    *           others
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-05
    * @notice   1. not thread-safe;
    *           2. `SetDataStrategy()` should be called before `Start()`
    */
    ErrCode set_data_strategy(int strategy);
    int     data_strategy() const;

    /**
    * @brief    set queues' parameters
    * @param    name        queue name
    * @param    cnt_grp     queue's counter names
    * @param    capacity    queue size
    * @param    watermark   queue watermark
    * @return   error::OK(=0): ok;
    *           other errcode(<0): fail;
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-06
    * @notice   not thread-safe;
    */
    ErrCode set_queues(const  std::string& name,
                       const  std::string& cnt_grp,
                       size_t if_capacity,
                       size_t of_capacity,
                       double if_watermark = g_kKylinBufLowMark,
                       double of_watermark = g_kKylinBufLowMark);

    /**
    * @brief    set/get batch size/num
    * @param    batch_size batch size(byte)
    * @param    batch_num  batch num
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-14
    */
    ErrCode set_batch_size(int batch_size);
    int     batch_size() const;
    ErrCode set_batch_num(int batch_num);
    int     batch_num() const;

public:
    void OnCompletion(AsyncContext* ctx);

private:
    virtual ~Transporter(); // forbidden destructor, use Stop() and Release()
    MOCKVIRTUAL void _OnStop(AsyncContext* ctx = NULL);

    //-----------------------------------------------
    // inflow management interface
    /**
    * @brief        Add/Del/Upd inflow source
    * @param[in]    sources inflows
    */
    MOCKVIRTUAL void _OnInflowAddSource(IfSourceMgrCtx* ctx);
    MOCKVIRTUAL void _OnInflowDelSource(IfSourceMgrCtx* ctx);
    MOCKVIRTUAL void _OnInflowUpdSource(IfSourceMgrCtx* ctx);

    //-----------------------------------------------
    // outflow management interface
    /**
    * @brief        Add/Del/Upd outflow braches
    *               if no tags assigned, all data will be tranported.
    * @param[IN]    ctx outflows
    */
    MOCKVIRTUAL void _OnAddOutflow(OfMgrCtx* ctx);
    MOCKVIRTUAL void _OnDelOutflow(OfMgrCtx* ctx);
    MOCKVIRTUAL void _OnUpdOutflow(OfMgrCtx* ctx);

    //-----------------------------------------------
    // data interface
    void _SetOutflowChecker();
    void _DelOutflowChecker();
    void _OnCheckOutflow(AsyncContext* ctx);
    void _OnTriggerPendingRecv();
    void _OnTriggerPendingFetch();
    void _DumpCounters();
    ErrCode _OnOutflowRecv(IN    ESPADDRESS      addr,
                           IN    uint64_t        lparam,
                           INOUT TPDataMsg*      msg,
                           INOUT CBufReadStream* brs);
    ErrCode _OnOutflowFetch(IN ESPADDRESS addr,
                            IN uint64_t   lparam,
                            IN uint64_t   id,
                            IN TPDataMsg& msg);
    ErrCode _AckMsg(ESPADDRESS addr, uint64 lparam, const TPDataMsg& msg);

private:
    /**
    * @brief    pending recv data
    * @author   konghui, konghui@baidu.com
    */
    struct PendingRecv {
        BufHandle*     hdl;      // buf hander, store all tuples
        CBufReadStream brs;      // buf read stream, store current process bar
        TPDataMsg      msg;      // data msg head
        ESPADDRESS     addr;     // esp addr
        uint64_t       lparam;   // esp msg id
        uint64_t       recv_time; // timestamp for recv time

        PendingRecv();
        inline void Reset() {
            ChainFreeHdl(hdl, NULL);
            hdl = NULL;
            brs = CBufReadStream(NULL, NULL);
            msg.from_id = 0;
            msg.to_id   = 0;
            msg.size    = 0;
            msg.batch   = 0;
            msg.ack     = 0;
            addr        = INVALID_ESPADDRESS;
            lparam      = 0;
            recv_time   = 0;
        }
    };

    /**
    * @brief    pending fetch request
    * @author   konghui, konghui@baidu.com
    */
    struct PendingFetch {
        TPDataMsg       msg;      // data msg head
        ESPADDRESS      addr;     // esp addr
        uint64_t        lparam;   // esp msg id
        uint64_t        recv_time; // timestamp for recv time

        PendingFetch();
    };

    enum TPAction {
        TPS_STOP     = TPA_USER_BEGIN,
        TPA_OF_CHECK = TPA_USER_BEGIN + 1
    };

    static const std::string kQueueName;  // prefix name of queue
    static const std::string kIfQueueName; // prefix name of inflow queue
    static const std::string kOfQueueName; // prefix name of outflow queue

    typedef boost::unordered_map < uint64_t/*proccessor id*/, TPOutflow* > OutflowMap;
    typedef std::map < uint64_t/*proccessor id*/, PendingFetch > PendingFetchMap;

private:
    BufHandle*                m_phdl;       // shared buf handle ptr
    TPDataMsg*                m_pmsg;       // shared msg head ptr
    TPDataMsg                 m_msg;        // shared msg head
    char                      m_tag[kMaxTagSize]; // shared tag buffer

private:
    uint64_t                  m_id;         // local PEID
    bool                      m_running;    // running flag
    ESPSTUB                   m_stub;       // stub
    ESPSTUB                   m_if_stub;    // inflow stub
    TPInflow*                 m_inflow;     // one inflow for echo Transporter
    uint64_t                  m_outflow_ack; // ack for all outflows
    OutflowMap                m_outflow_map; // outflow map
    AsyncContext              m_outflow_chk_ctx; // context for check status
    uint64_t                  m_outflow_chk_interval_ms; // intervavl for check status
    TPTagMap                  m_tag_map;    // tags map, tag => subsribers

    // queue
    std::string               m_queue_name;   // queue name
    size_t                    m_if_queue_size; // inflow queue size
    size_t                    m_of_queue_size; // outflow queue size
    double                    m_if_queue_soft; // inflow queue soft watermark
    double                    m_of_queue_soft; // outflow queue soft watermark

    // data
    int32_t                   m_batch_size;       // max batch size
    int32_t                   m_batch_num;        // max batch size
    int                       m_data_strategy;    // strategy when status is in exception
    std::set<uint64_t>        m_full_queue_set;   // set of full queues' id
    bool                      m_has_pending_recv; // has pending recv data
    PendingRecv               m_pending_recv;     // pending fetch req
    PendingFetchMap           m_pending_fetch_map; // map of pending fetch

    // counter
    std::string               m_cnt_grp;             // counter group
    TPAllOfsCounter           m_cnt_all_ofs;         // counter for all outflows
    uint64_t                  m_cnt_update_interval; // internal for updating coutners
}; // class Transporter

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSNODE_TRANSPORTER_H__
