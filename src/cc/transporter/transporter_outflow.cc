#include "transporter/transporter_outflow.h"
#include <boost/foreach.hpp>
#include "common/dstream.h"
#include "common/id.h"

namespace dstream {
namespace transporter {

TPBranch::TPBranch(const std::string& pre, uint64_t id)
    : msg_ack(0),
      queue_ack(kAdvanceQueueAck),
      queue(NULL) {
    counter.id = id;
}

TPOutflow::TPOutflow(uint64_t id, uint64_t owner_id, uint64_t parall) :
    m_id(id),
    m_owner_id(owner_id),
    m_parall(parall),
    m_queue_size(config_const::kPNSendQueueSize),
    m_queue_soft(g_kKylinBufLowMark),
    m_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG) {
    m_counter.id = id;
}

TPOutflow::~TPOutflow() {
    Stop();
    DSTREAM_DEBUG("outflow[%p] delete", this);
}

ErrCode TPOutflow::Start() {
    uint64_t parall = m_parall;
    m_parall = 0;
    VERIFY_OR_RETURN(OnUpdParall(parall), error::OK);

    DSTREAM_INFO("outflow[%p id=%lu, owner=%lu, parall=%lu] start run...",
                 this, m_id, m_owner_id, m_parall);
    return error::OK;
}

void TPOutflow::Stop() {
    m_parall = 0;
    BOOST_FOREACH(TPBranch & branch, m_branches) {
        branch.queue->Destroy();
        SAFE_DELETE(branch.queue);
    }
    m_branches.clear();
    DSTREAM_INFO("outflow[%p] stop...", this);
}

ErrCode TPOutflow::set_data_strategy(int strategy) {
    VERIFY_OR_RETURN(IsValidDataStrategyType(strategy), error::BAD_ARGUMENT);
    m_data_strategy = strategy;
    return error::OK;
}

int TPOutflow::data_strategy() const {
    return m_data_strategy;
}

ErrCode TPOutflow::set_queues(const  std::string& name,
                              const  std::string& cnt_grp,
                              size_t size,
                              double watermark) {
    VERIFY_OR_RETURN(!name.empty(), error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(!cnt_grp.empty(), error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(size > 0, error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(watermark > 0, error::BAD_ARGUMENT);

    m_cnt_grp    = cnt_grp;
    m_queue_name = name;
    m_queue_size = size;
    m_queue_soft = watermark;
    return error::OK;
}

const TPOfCounter& TPOutflow::DumpCounter() {
    m_counter.cnt_branches.clear(); 
    for (uint64_t i = 0; i < m_parall; ++i) {
        m_branches[i].counter.cnt_queue = m_branches[i].queue->DumpCounter();
        TPBranchCounter& branch_cnt = m_branches[i].counter;
        branch_cnt.UpdateQps();
        m_counter.cnt_branches.push_back(branch_cnt);
    }
    m_counter.cnt_branch_num.Set(m_branches.size());

    return m_counter;
}

//-----------------------------------------------
// management interface
ErrCode TPOutflow::OnUpdParall(uint64_t n) {
    // 1. change the parall
    // 2. if the number of the parallelism decrease,
    //    move the remaining data into the new queue
    if (n > m_parall) { // increase
        for (uint64_t i = m_parall; i < n; ++i) {
            TPBranch branch(m_cnt_pre, MAKE_PROCESSOR_PE_ID(m_id, i));
            branch.queue = new (std::nothrow) TPAckQueue(m_owner_id,
                                                         MAKE_PROCESSOR_PE_ID(m_id, i),
                                                         m_queue_name,
                                                         m_cnt_grp.c_str());
            if (NULL == branch.queue) {
                DSTREAM_WARN("fail to alloc send queue[id=%lu, owner=%lu, seq"
                             "=%lu]", branch.counter.id, m_owner_id, i);
                return error::BAD_MEM_ALLOC;
            }
            ErrCode ret = branch.queue->Init(m_queue_size, m_queue_soft);
            if (error::OK != ret) {
                DSTREAM_WARN("fail to start send queue[%p, id=%lu, owner=%lu, "
                             "seq=%lu]", branch.queue, branch.counter.id,
                             m_owner_id, i);
                SAFE_DELETE(branch.queue);
                return error::START_QUEUE_FAIL;
            }
            m_branches.push_back(branch);
            m_parall++;
            // m_counter.cnt_branch_num.Add(1);
            DSTREAM_INFO("outflow[%p id=%lu, owner=%lu] start branch[%p "
                         "queue=%p, id=%lu, seq=%lu, capacity=%lu] ...",
                         this, m_id, m_owner_id, &m_branches[i], branch.queue,
                         branch.counter.id, i, m_queue_size);
        }
    } else if (n < m_parall) { // decrease
        for (uint64_t i = m_parall; i > n; i--) {
            TPBranch& branch = m_branches.back();
            DSTREAM_INFO("outflow[%p id=%lu, owner=%lu] del branch[%p "
                         "queue=%p, id=%lu, seq=%lu, capacity=%lu]",
                         this, m_id, m_owner_id, &branch, branch.queue,
                         branch.counter.id, i, m_queue_size);
            m_branches.pop_back();
            m_parall--;
            // m_counter.cnt_branch_num.Sub(1);
        }
    }
    return error::OK;
}

//-----------------------------------------------
// data interface
ErrCode TPOutflow::OnFetchData(IN    int32_t   err,
                               IN    uint64_t  seq,
                               INOUT uint64_t* ack,
                               INOUT int32_t*  size,
                               INOUT int32_t*  batch,
                               OUT   CBufWriteStream* bws) {
    UNLIKELY_IF(seq >= m_parall) { // unexpect msg due to:
        // 1. this new added source has not been created;
        // 2. the old source has been deletedr;
        *size  = 0;
        *batch = 0;
        return error::NOT_FOUND_SEND_QUEUE;
    }

    ErrCode ret = error::OK;
    TPBranch& branch = m_branches[seq];

    if (LIKELY(TPN_ACK_OK == err)) { // fetch data
        if (LIKELY(*ack == branch.msg_ack + 1)) { // OK
            ret = branch.queue->PopFront(branch.queue_ack);
            if (LIKELY(error::OK <= ret)) {
                branch.queue_ack = kAdvanceQueueAck; // advance ack
                if (LIKELY(0 != *size)) {
                    ret = branch.queue->Front(bws, size, batch, &branch.queue_ack);
                    if (LIKELY(error::OK == ret)) {
                        branch.counter.cnt_out_num.Add(*batch);
                        branch.counter.cnt_out_size.Add(*size);
                        branch.msg_ack++;
                    }
                } else { // 0 = size: just ack, donot fetch data
                    branch.counter.cnt_ack_without_fetch_num.Add(1);
                }
            }
            return ret;
        } else if (*ack == branch.msg_ack) { // OK, replay last data
            ret = branch.queue->Front(bws, size, batch, &branch.queue_ack);
            if (LIKELY(error::OK == ret)) {
                branch.counter.cnt_replay_num.Add(*batch);
                branch.counter.cnt_replay_size.Add(*size);
            }
            return ret;
        }
        branch.counter.cnt_ack_miss_fetch_num.Add(1);
        return error::DROP_MSG_DUE_ACK_MISS_MATCH;
    } else { // TPN_ACK_SYNC == err // resync ack
        *size  = 0;
        *batch = 0;
        if ((*ack == branch.msg_ack) && (0 != *ack)) { // net jitter, donot flush unacked data
            branch.counter.cnt_jitter_resync_num.Add(1);
            return error::OK;
        }
        branch.counter.cnt_other_resync_num.Add(1);
        // flush unacked data, due to:
        //  1. *ack == branch.msg_ack && 0 == *ack:
        //        down-stream pe migrated;
        //  2. *ack == branch.msg_ack + 1:
        //        ack from down-stream lost, but down-stream had recv last data;
        //  3. other ack number due to unknown reason;
        branch.msg_ack = *ack; // accept ack from down-stream pe
        branch.queue_ack = kAdvanceQueueAck; // reset queue_ack
        CBufWriteStream droped_tuples;
        int             droped_size;
        int             droped_batch;
        ret = branch.queue->PopFront(kFlushQueueUnack, &droped_tuples,
                                     &droped_size, &droped_batch); // flush unacked data
        if (error::OK == ret) { // OK, dump tuples
            std::string s;
            BufHandle* hdl = droped_tuples.GetBegin();
            DumpTuplesFromBufHdl(hdl, droped_size, droped_batch, &s);
            ChainFreeHdl(hdl, NULL);
            hdl = NULL;
            // DSTREAM_DEBUG("these tupels in outflow[%p id=%lu, owner=%lu, "
            DSTREAM_WARN("these tupels in outflow[%p id=%lu, owner=%lu, "
                         "queue=%p %s] may lost/drop due to ack sync: "
                         "%s", this, MAKE_PROCESSOR_PE_ID(m_id, seq),
                         m_owner_id, branch.queue,
                         branch.queue->DumpStatus().c_str(), s.c_str());
            branch.counter.cnt_may_drop_resync_num.Add(droped_batch);
            branch.counter.cnt_may_drop_resync_size.Add(droped_size);
        } // else: error::OK_NO_DATA_FLUSHED == ret or ret < 0;
        return ret;
    }
}

ErrCode TPOutflow::OnRecvData(IN int32_t   err,
                              IN uint64_t  seq,
                              IN int32_t   size,
                              IN int32_t   batch,
                              IN CBufReadStream brs) {
    DSTREAM_UNUSED_VAL(err);

    TPBranch& branch = m_branches[seq % m_parall];
    ErrCode ret = branch.queue->PushBack(&brs, size);
    UNLIKELY_IF(error::QUEUE_FULL == ret) { // queue full
        branch.counter.cnt_queue_full_num.Add(1);
        if (DST_WHEN_QUEUE_FULL_DROP_AGELONG == m_data_strategy) {
            // drop agelong data
            CBufWriteStream droped_tuples;
            int             droped_size  = size;
            int             droped_batch = batch;
            ret = _PopAgedTuples(seq, &droped_size,
                                 &droped_batch, &droped_tuples);
            UNLIKELY_IF(error::OK != ret) {
                return ret;
            }
            branch.counter.cnt_drop_agelog_data_num.Add(droped_batch);
            branch.counter.cnt_drop_agelog_data_size.Add(droped_size);
            std::string s;
            BufHandle* hdl = droped_tuples.GetBegin();
            DumpTuplesFromBufHdl(hdl, droped_size, droped_batch, &s);
            ChainFreeHdl(hdl, NULL);
            hdl = NULL;
            // DSTREAM_DEBUG("these tupels in outflow[%p id=%lu, owner=%lu, "
            DSTREAM_WARN("these tupels in outflow[%p id=%lu, owner=%lu, "
                         "queue=%p %s] may lost/drop due to queue full: %s",
                         this, MAKE_PROCESSOR_PE_ID(m_id, seq % m_parall),
                         m_owner_id, branch.queue,
                         branch.queue->DumpStatus().c_str(), s.c_str());
            // rewrite
            ret = branch.queue->PushBack(&brs, size);
            if (LIKELY(error::OK == ret)) {
                branch.counter.cnt_in_num.Add(batch);
                branch.counter.cnt_in_size.Add(size);
            }
            return ret;
        }
    }
    branch.counter.cnt_in_num.Add(batch);
    branch.counter.cnt_in_size.Add(size);
    return ret;
}

ErrCode TPOutflow::_PopAgedTuples(IN    uint64_t seq,
                                  INOUT int32_t* droped_size,
                                  INOUT int32_t* droped_batch,
                                  OUT   CBufWriteStream* droped_tuples) {
    TPBranch& branch = m_branches[seq % m_parall];
    branch.queue_ack = kAdvanceQueueAck; // reset queue ack
    return branch.queue->PopAgedTuples(droped_size,
                                       droped_batch,
                                       droped_tuples);
}


} // namespace transporter
} // namespace dstream
