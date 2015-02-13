/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTER_OUTFLOW_H__
#define __DSTREAM_SRC_CC_TRANSPORTER_OUTFLOW_H__

#include <string>
#include <vector>
#include "common/id.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_counters.h"
#include "transporter/transporter_protocol.h"
#include "transporter/transporter_queue.h"

namespace dstream {
namespace transporter {

struct TPBranch {
    uint64_t        msg_ack;   // ack between outflow and inflow
    uint64_t        queue_ack; // ack between outflow and queue
    TPAckQueue*     queue;     // queue
    TPBranchCounter counter;   // counter

    TPBranch(const std::string& pre, uint64_t id);
};

/**
* @brief  The Outflow stores data from local pe, and forwards them
*         to the downstream pes, which belong the same processor.
*/
class TPOutflow {
public:
    TPOutflow(uint64_t id, uint64_t owner_id, uint64_t parall);
    ~TPOutflow();

    //-----------------------------------------------
    /**
    * @brief    start TPOutflow
    * @return   error::OK(= 0): queue is OK
    *           < 0 : other failures;
    * @author   konghui, konghui@baidu.com
    * @notice   call this to start inflow, not thread-safe
    */
    ErrCode Start();
    /**
    * @brief    stop TPOutflow
    * @return   error::OK(= 0): queue is OK
    *           < 0 : other failures;
    * @author   konghui, konghui@baidu.com
    * @notice   thread-safe
    */
    void    Stop();

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
    * @return   error::OK
    *           others
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-06
    * @notice   not thread-safe;
    */
    ErrCode set_queues(const  std::string& name,
                       const  std::string& cnt_grp,
                       size_t capacity,
                       double watermark = g_kKylinBufLowMark);
    /**
    * @brief     test queue status
    * @param     seq sequence of outflow
    * @return    error::OK(= 0): queue is OK
    *            error::OK_SOFT_WATERMARK(> 0): queue reaches the soft warkmark
    *            error::QUEUE_FULL(< 0):   queue is full
    *            < 0 : other failures;
    * @author    konghui, konghui@baidu.com
    * @date      2013-03-10
    */
    DSTREAM_FORCE_INLINE ErrCode IsFull(uint64_t seq) const {
        return m_branches[seq % m_parall].queue->IsFull();
    }
    /**
    * @brief     get pe id from hash num
    * @param     seq hash num
    * @return    pe id
    * @author    konghui, konghui@baidu.com
    * @date      2013-03-10
    */
    DSTREAM_FORCE_INLINE uint64_t id(uint64_t seq) const {
        return MAKE_PROCESSOR_PE_ID(m_id, seq % m_parall);
    }
    /**
    * @brief    get branch's msg ack for hash num
    * @param    seq hash num
    * @return   branch's msg ack num
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-14
    */
    DSTREAM_FORCE_INLINE uint64_t ack(uint64_t seq) const {
        return m_branches[seq % m_parall].msg_ack;
    }

    DSTREAM_FORCE_INLINE const TPAckQueue* queue(uint64_t seq) const {
        return m_branches[seq % m_parall].queue;
    }

    DSTREAM_FORCE_INLINE const TPBranchCounter& counter(uint64_t seq) const {
        return m_branches[seq % m_parall].counter;
    }

    const TPOfCounter& DumpCounter();

    //-----------------------------------------------
    // management interface
    /**
    * @brief    update outflow's parallism
    * @param    n paramllism
    * @return   error::OK: success
    *           < 0 : other error code
    * @author   konghui, konghui@baidu.com
    */
    ErrCode OnUpdParall(uint64_t n);

    //-----------------------------------------------
    // data interface
    /**
    * @brief    fetch/recv data
    * @return   error::OK: success
    *           < 0 : other error code
    * @author   konghui, konghui@baidu.com
    */
    ErrCode OnFetchData(IN    int32_t   err,
                        IN    uint64_t  seq,
                        INOUT uint64_t* ack,
                        INOUT int32_t*  size,
                        INOUT int32_t*  batch,
                        OUT   CBufWriteStream* bws);
    ErrCode OnRecvData(IN     int32_t   err,
                       IN     uint64_t  seq,
                       IN     int32_t   size,
                       IN     int32_t   batch,
                       IN     CBufReadStream brs);

private:
    /**
    * @brief        pop aged tuples
    * @param[IN]    seq           sequence of outflow
    j   * @param[INOUT] droped_size   indcate: drop at least `droped_size` tuples,
    *                             return the real size been dropped
    * @param[INOUT] droped_batch  return the real size been dropped
    * @param[OUT]   droped_tuples return the tuples been dropped
    * @return   error::OK: success
    *           < 0 : other error code
    * @author   konghui, konghui@baidu.com
    * @date     2013-03-05
    */
    ErrCode _PopAgedTuples(uint64_t seq,
                           int32_t* droped_size,
                           int32_t* droped_batch,
                           CBufWriteStream* droped_tuples);

private:
    uint64_t                     m_id;         // ProcessorID
    uint64_t                     m_owner_id;   // local pe id
    uint64_t                     m_parall;     // parallelism
    std::vector<TPBranch>        m_branches;   // ack
    std::string                  m_queue_name; // queue name
    size_t                       m_queue_size; // queue size
    double                       m_queue_soft; // queue soft watermakr
    std::string                  m_cnt_grp;    // for counter
    std::string                  m_cnt_pre;    // for counter
    TPOfCounter                  m_counter;
    int                          m_data_strategy; // strategy when status is in exception
}; // class TPOutflow


} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTER_OUTFLOW_H__
