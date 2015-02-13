/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTER_INFLOW_H__
#define __DSTREAM_SRC_CC_TRANSPORTER_INFLOW_H__

#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <string>
#include <vector>
#include "common/id.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_counters.h"
#include "transporter/transporter_protocol.h"
#include "transporter/transporter_queue.h"

namespace dstream {
namespace transporter {

/**
* @brief  Inflow fetches data from upstream pes periodically.
* @notice This object is assoicated with `Transporter`.
*/
class TPInflow : public CESPObject {
    ESP_DECLARE_OBJECT(TPInflow);

public:
    /**
    * @brief    pending recv data
    * @author   konghui, konghui@baidu.com
    */
    struct PendingRecv {
        BufHandle*      hdl;    // data hdl
        uint64_t        ack;    // msg ack
        int32_t         size;   // data size
        int32_t         batch;  // tuple num

        PendingRecv();
        DSTREAM_FORCE_INLINE void Reset() {
            ChainFreeHdl(hdl, NULL);
        }
    };

    /**
    * @brief    pending fetch reuest
    * @author   konghui, konghui@baidu.com
    */
    struct PendingFetch {
        TPDataMsg       msg;      // data msg
        ESPADDRESS      afrom;    // esp addr
        uint64_t        lparam;   // msg id
        uint64_t        recv_time; // recv timestamp

        PendingFetch();
    };

    /**
    * @brief Source Status
    *        SOURCE_OK:      source is transmiting data;
    *        SOURCE_SYNC:    source talking ack num with upstream
    *        SOURCE_INVALID: source is searching new addr and stub of upstream
    */
    enum SourceStatus {
        SOURCE_OK      = 0,
        SOURCE_SYNC    = 1,
        SOURCE_INVALID = 2
    };

    /**
    * @brief    source for upstream pe
    * @author   konghui, konghui@baidu.com
    */
    struct TPSource {
        DLINK           link;       // requested by TKeyedQueue
        uint64_t        id;         // requested by TKeyedQueue, upstream pe id
        ESPNODE         node;       // upstream pe's ipport
        ESPADDRESS      addr;       // upstream pe's esp address(ip:port:stuof)
        SourceStatus    status;     // source status
        uint32_t        retry;      // retry
        uint64_t        ack;        // ack num
        uint64_t        stub_msg_id; // data msg id
        uint64_t        data_msg_id; // data msg id
        uint64_t        start_time; // timestamp for starting;
        uint64_t        last_send;  // timestamp for last sending msg;
        uint64_t        last_recv;  // timestamp for last recving msg;
        bool            has_pending_recv; // has pending recv data
        PendingRecv     pending_recv; // pending recv data
        TPSourceCounter counter;

        TPSource(uint64_t id, ESPNODE node);
    };

public:
    //-----------------------------------------------
    // inflow management interface
    TPInflow(CESPObject* host, uint64_t id, ESPSTUB stub);
    /**
    * @brief    start inflow synchronously
    * @return   error::OK(=0): ok;
    *           other errcode(<0): fail;
    * @notice   not thread-safe
    */
    ErrCode Start();
    /**
    * @brief    stop inflow asynchronously
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
    // source management interface
    /**
    * @brief      add/del/upd source
    * @param[IN]  sources parameters(id/addr) for source
    * @param[OUT] res     return values
    * @return     error::OK(=0): ok;
    *             other errcode(<0): fail;
    * @author     konghui, konghui@baidu.com
    * @date       2013-01-13
    */
    MOCKVIRTUAL ErrCode AddSource(const std::vector<IfSourceMeta>& sources,
                                  std::vector<ErrCode>* res = NULL);
    MOCKVIRTUAL ErrCode DelSource(const std::vector<IfSourceMeta>& sources,
                                  std::vector<ErrCode>* res = NULL);
    MOCKVIRTUAL ErrCode UpdSource(const std::vector<IfSourceMeta>& sources,
                                  std::vector<ErrCode>* res = NULL);

    //-----------------------------------------------
    // set/get member
    /**
    * @brief    set/get data strategy
    * @param    strategy data strategy, currenty supported:
    *                     1. DST_WHEN_QUEUE_FULL_DROP_AGELONG
    *                     2. DST_WHEN_QUEUE_FULL_HOLD
    * @return   data strategy
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-14
    */
    ErrCode set_data_strategy(int strategy);
    int     data_strategy() const;

    /**
    * @brief    set queues' parameters
    * @param    name        queue name
    * @param    cnt_grp     queue's counter names
    * @param    capacity    queue size
    * @param    watermark   queue watermark
    * @return   error::OK(=0): success
    *           < 0: other error code
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-06
    * @notice   not thread-safe;
    */
    ErrCode set_queues(const  std::string& name,
                       const  std::string& cnt_grp,
                       size_t capacity,
                       double watermark = g_kKylinBufLowMark);
    /**
    * @brief    set/get batch size/num
    * @param    batch_size batch size(byte)
    * @param    batch_num  batch num
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-14
    */
    void set_batch_size(int batch_size);
    int  batch_size() const;
    void set_batch_num(int batch_num);
    int  set_batch_num();
    const TPIfCounter& DumpCounter();

    //-----------------------------------------------
    // data interface, intenal function
    /**
    * @brief     recv/fetch data
    * @param[IN] esp_msg esp msg
    * @author    konghui, konghui@baidu.com
    * @date      2013-01-13
    */
    void OnRecvData(ESPMessage* esp_msg);
    void OnFetchData(ESPMessage* esp_msg);

    /**
    * @brief     recv query stub msg
    * @param[IN] esp_msp esp msg
    * @author    konghui, konghui@baidu.com
    * @date      2013-01-14
    */
    void OnQueryStubAck(ESPMessage* esp_msp);

    /**
    * @brief     check source status,
    *            called by TKeyedQueue.Enumerate
    * @param[IN] source source to be checked
    * @return    always true;
    * @author    konghui, konghui@baidu.com
    * @date      2013-01-13
    */
    bool CheckSource(TPSource* source);
    /**
    * @brief     trigger source's pending recv
    *            called by TKeyedQueue.Enumerate
    * @param[IN] source source to be checked
    * @return    true: queue still has space, continue trigger;
    *            false: queue full, break trigger;
    * @author    konghui, konghui@baidu.com
    * @date      2013-03-06
    */
    bool TriggerPendingRecv(TPSource* source);
    void OnCompletion(AsyncContext* pctx);

private:
    virtual ~TPInflow(); // forbidden destructor, use Stop() and Release()
    MOCKVIRTUAL void _OnStop(AsyncContext* ctx = NULL);

    //-----------------------------------------------
    // stub interface
    MOCKVIRTUAL ErrCode _ExtractStubMsg(ESPMessage* esp_msg);
    MOCKVIRTUAL void    _QueryStub(TPSource* source);

    //-----------------------------------------------
    // fetch/cancel source
    MOCKVIRTUAL void _SetSourceChecker();
    MOCKVIRTUAL void _DelSourceChecker();
    MOCKVIRTUAL void _DelSources();
    MOCKVIRTUAL void _OnCheckSource(AsyncContext* ctx);
    MOCKVIRTUAL void _OnTriggerPendingRecv();
    MOCKVIRTUAL void _OnTriggerPendingFetch();

    /**
    * @brief    ack data msg to upstream pe
    * @param    source source
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-19
    */
    MOCKVIRTUAL DSTREAM_FORCE_INLINE
    void _AckSource(TPSource* source) {
        _PostMsgToSource(source, 0, 0);
    }
    /**
    * @brief    send fetch data msg to upstream pe
    * @param    source source
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-19
    */
    MOCKVIRTUAL DSTREAM_FORCE_INLINE
    void _FetchSource(TPSource* source) {
        _PostMsgToSource(source, m_batch_size, m_batch_num);
    }
    /**
    * @brief    post data msg to updstream pe
    * @param    source     source
    * @param    batch_size batch size
    * @param    batch_num  batch num
    * @return   error::OK(=0): success
    *           < 0: other error code
    * @author   konghui, konghui@baidu.com
    * @date     2013-02-19
    */
    MOCKVIRTUAL void _PostMsgToSource(TPSource* source,
                                      int32_t batch_size,
                                      int32_t batch_num);

    //-----------------------------------------------
    // data from source
    /**
    * @brief        extract TPDataMsg head into msg_
    * @param[IN]    esp_msg esp msg
    * @param[INOUT] brs buf read stream
    * @return   error::OK(=0): success
    *           < 0: other error code
    * @author   konghui, konghui@baidu.com
    * @date     2013-01-14
    */
    MOCKVIRTUAL ErrCode _ExtractDataMsg(ESPMessage* esp_msg,
                                        CBufReadStream* brs);
    /**
    * @brief    recv data msg from upstream
    * @param    source source
    * @param    err    error code from upstream
    *                  TPN_ACK_OK:   fetch data request
    *                  TPN_ACK_SYNC: sync ack number
    * @param    ack    ack number
    * @param    brs    buf read stream
    * @param    size   tuples size
    * @param    batch  tuples num
    * @return   error::OK(=0): success
    *           < 0: other error code
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-19
    */
    MOCKVIRTUAL ErrCode _OnSourceRecv(IN    TPSource* source,
                                      IN    int32_t   err,
                                      IN    uint64_t  ack,
                                      INOUT CBufReadStream* brs,
                                      IN    int32_t   size,
                                      IN    int32_t   batch);
    /**
    * @brief
    *
    * @param    addr    client esp addr
    * @param    lparam  esp msg id
    * @param    err     error code from upstream, not used yet
    * @param    ack     ack number
    * @param    size    as in: at most `size` bytes tuples will be read
    *                   as out: store readed tuples size
    * @param    batch   store readed tuples number
    *
    * @return
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-19
    */
    MOCKVIRTUAL ErrCode _OnFetchData(IN ESPADDRESS addr,
                                     IN uint64     lparam,
                                     IN int32_t    err,
                                     IN uint64_t   ack,
                                     IN int32_t    size,
                                     IN int32_t    batch);
    MOCKVIRTUAL ErrCode _FetchData(IN    uint64_t ack,
                                   INOUT int32_t* size,
                                   INOUT int32_t* batch,
                                   OUT   CBufWriteStream* bws);
    /**
    * @brief     pop aged tuples
    * @param[INOUT] size at least `size` bytes tuples will be dropped
    * @param[IN] number of dropped tuples
    * @return    error::OK: success
    *            < 0 : other error code
    * @author    konghui, konghui@baidu.com
    * @date      2013-03-05
    */
    MOCKVIRTUAL ErrCode _PopAgedTuples(INOUT int32_t* size, OUT int32_t* batch);

private:
    /**
    * @brief    status for transporter
    * @author   konghui, konghui@baidu.com
    * @date     2013-01-13
    */
    enum {
        TPA_IF_STOP         = TPA_USER_BEGIN + 0, // stop
        TPA_IF_CHECK_SOURCE = TPA_USER_BEGIN + 1  // check source
    };
    typedef boost::unordered_map<uint64_t, TPSource*> TPSourceMap;

private:
    BufHandle*          m_phdl;      // shared buffer handler ptr
    TPDataMsg*          m_pmsg;      // shared msg head ptr
    TPDataMsg           m_msg;       // shared msg head
    TPStubMsg*          m_pstub_msg; // shared msg head ptr
    TPStubMsg           m_stub_msg;  // shared msg head

private:
    bool                m_running;  // running flag
    CESPObject*         m_owner;    // owner, to share kylin exec thread
    uint64_t            m_id;       // local peid
    ESPSTUB             m_stub;     // esp stub
    TPSourceMap         m_sources;  // updstream pes

    // queue
    TPAckQueue*         m_queue;     // queue
    uint64_t            m_msg_ack;   // ack between inflow and pe
    uint64_t            m_queue_ack; // ack between inflow and queue
    std::string         m_queue_name; // queue name
    size_t              m_queue_size; // queue size
    double              m_queue_soft; // queue soft watermakr

    // data, status
    int32_t             m_batch_size;    // max batch size
    int32_t             m_batch_num;     // max batch num
    int32_t             m_data_strategy; // strategy when status is in exception
    uint64_t            m_fetch_data_timeout_ms; // fetch data timeout
    uint64_t            m_check_interval_ms; // intervavl for check status
    AsyncContext        m_check_source_ctx;  // context for check status
    uint32_t            m_max_retry;         // max retry for data fetch, stub query

    // pending
    int32_t             m_pending_recv_num;  // num of pending recv data
    int32_t             m_has_pending_fetch; // has pending fetch req
    PendingFetch        m_pending_fetch;     // pending fetch req

    // counter
    std::string         m_cnt_grp;           // counters' group
    TPIfCounter         m_counter;           // counter
}; // class TPInflow

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTER_INFLOW_H__
