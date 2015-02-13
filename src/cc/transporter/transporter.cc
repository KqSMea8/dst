#include "transporter/transporter.h"
#include <boost/foreach.hpp>
#include <string>
#include "common/id.h"
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {
namespace transporter {

// TODO(konghui@baidu.com):
// 1. remove the stategy of 'drop msg package due to queue_full'

ESP_BEGIN_MAP_MESSAGE(Transporter, CESPObject)
ESP_MAP_MESSAGE_RAW(TPE_OF_RECV_LOCALPE, OnOutflowRecv)
ESP_MAP_MESSAGE_RAW(TPE_OF_SUB_FETCH, OnOutflowFetch)
ESP_END_MAP_MESSAGE

const std::string Transporter::kQueueName   = "TPQueue";
const std::string Transporter::kIfQueueName = "If";
const std::string Transporter::kOfQueueName = "Of";

#define S_TP_OFPR_FMT \
    "OfPendingRecv[%p]=[hdl=%p, msg="S_TP_DATA_MSG_FMT", afrom=" \
    S_DOTTED_ESPADDR", lparam=%lu, recv_time=%lu]"
#define DOTTED_TP_OFPR(pr) \
    &(pr), (pr).hdl, DOTTED_TP_DATA_MSG((pr).msg), \
    DOTTED_ESPADDR((pr).addr), (pr).lparam, (pr).recv_time

#define S_TP_OFPF_FMT \
    "OfPendingFetch[%p]=[msg="S_TP_DATA_MSG_FMT", afrom="S_DOTTED_ESPADDR \
    ", lparam=%lu, recv_time=%lu]"
#define DOTTED_TP_OFPF(pf) \
    &(pf), DOTTED_TP_DATA_MSG((pf).msg), DOTTED_ESPADDR((pf).addr), \
    (pf).lparam, (pf).recv_time

Transporter::PendingRecv::PendingRecv()
    : hdl(NULL),
      brs(NULL, NULL),
      addr(INVALID_ESPADDRESS),
      lparam(0),
      recv_time(0) {
}

Transporter::PendingFetch::PendingFetch()
    : addr(INVALID_ESPADDRESS),
      lparam(0),
      recv_time(0) {
}

Transporter::Transporter(uint64_t id, ESPSTUB stub, ESPSTUB if_stub)
    : m_id(id),
      m_running(false),
      m_stub(stub),
      m_if_stub(if_stub),
      m_inflow(NULL),
      m_outflow_ack(0),
      m_outflow_chk_interval_ms(config_const::kDataCheckIntervalMs),
      m_queue_name(kQueueName),
      m_if_queue_size(config_const::kPNRecvQueueSize),
      m_of_queue_size(config_const::kPNSendQueueSize),
      m_if_queue_soft(g_kKylinBufLowMark),
      m_of_queue_soft(g_kKylinBufLowMark),
      m_batch_size(config_const::kPNTuplesBatchSizeByte),
      m_batch_num(config_const::kPNTuplesBatchNum),
      m_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
      m_has_pending_recv(false),
      m_cnt_grp(NumberToString(m_id)) {
    m_cnt_update_interval = config_const::kCounterDumpIntervalMs / m_outflow_chk_interval_ms;
    AddRef();
}

Transporter::~Transporter() {
    _OnStop();
}

int Transporter::Release() {
    int n = CAsyncClient::Release();
    if (0 == n) {
        DSTREAM_DEBUG("transporeter[%p] detete", this);
        delete this;
    }
    return n;
}

ErrCode Transporter::Start() {
    ErrCode ret = error::OK;

    if (INVALID_ESPSTUB != m_if_stub) {
        m_inflow = new (std::nothrow) TPInflow(this, m_id, m_if_stub); // associates with Inflow
        VERIFY_OR_RETURN(NULL != m_inflow, error::BAD_MEM_ALLOC);
        ret = m_inflow->set_data_strategy(m_data_strategy);
        VERIFY_OR_RETURN(error::OK == ret, ret);
        m_inflow->set_batch_size(m_batch_size);
        m_inflow->set_batch_num(m_batch_num);
        ret = m_inflow->set_queues(m_queue_name + kIfQueueName,
                                   m_cnt_grp, m_if_queue_size, m_if_queue_soft);
        VERIFY_OR_RETURN(error::OK == ret, ret);
        ret = m_inflow->Start();
        VERIFY_OR_RETURN(ret >= error::OK, ret);
    }

    VERIFY_OR_RETURN(ESPE_OK == BindStub(m_stub), error::BIND_STUB_FAIL);

    _SetOutflowChecker();

    m_running = true;
    DSTREAM_INFO("transporter[%p id=%lu, stub=%u, if_stub=%u, "
                 "inflow=%p, batch_size=%d, batch_num=%d, "
                 "data_stategy='%s'] start run...", this, m_id,
                 m_stub, m_if_stub, m_inflow, m_batch_size,
                 m_batch_num, DataStrategyTypeToStr(m_data_strategy));
    return error::OK;
}

void Transporter::Stop() {
    AsyncContext* ctx = new (std::nothrow ) AsyncContext();
    VERIFY_OR_RETURN(NULL != ctx, BLANK);
    QueueExec(error::OK, TPS_STOP, this, ctx, false);

    DSTREAM_INFO("transporter[%p] stop...", this);
}

void Transporter::_OnStop(AsyncContext* ctx) {
    m_running = false;
    UnbindStub();

    _DelOutflowChecker();

    if (NULL != m_inflow) {
        m_inflow->Stop();
        m_inflow->Release();
        m_inflow = NULL;
    }
    BOOST_FOREACH(OutflowMap::value_type & pair, m_outflow_map) {
        pair.second->Stop();
        SAFE_DELETE(pair.second);
    }
    m_cnt_all_ofs.cnt_of_num.Set(0);
    m_outflow_map.clear();
    SAFE_DELETE(ctx);
}

ErrCode Transporter::set_data_strategy(int strategy) {
    VERIFY_OR_RETURN(IsValidDataStrategyType(strategy), error::BAD_ARGUMENT);
    DSTREAM_INFO("strategy@transporter[%p id=%lu]: '%s' => '%s'",
                 this, m_id, DataStrategyTypeToStr(m_data_strategy),
                 DataStrategyTypeToStr(strategy));
    m_data_strategy = strategy;
    return error::OK;
}

int Transporter::data_strategy() const {
    return m_data_strategy;
}

ErrCode Transporter::set_queues(const  std::string& name,
                                const  std::string& cnt_grp,
                                size_t if_size,
                                size_t of_size,
                                double if_watermark,
                                double of_watermark) {
    VERIFY_OR_RETURN(!name.empty(),    error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(!cnt_grp.empty(), error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(if_size > 0,      error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(of_size > 0,      error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(if_watermark > 0, error::BAD_ARGUMENT);
    VERIFY_OR_RETURN(of_watermark > 0, error::BAD_ARGUMENT);

    m_cnt_grp       = cnt_grp;
    m_queue_name    = name;
    m_if_queue_size = if_size;
    m_of_queue_size = of_size;
    m_if_queue_soft = if_watermark;
    m_of_queue_soft = of_watermark;
    return error::OK;
}

ErrCode Transporter::set_batch_size(int batch_size) {
    m_batch_size = batch_size < kMinBatchSize ? kMinBatchSize : batch_size;
    return error::OK;
}

int  Transporter::batch_size() const {
    return m_batch_size;
}

ErrCode Transporter::set_batch_num(int batch_num) {
    m_batch_num = batch_num < 1 ? 1 : batch_num;
    return error::OK;
}

int  Transporter::batch_num() const {
    return m_batch_num;
}

void Transporter::_SetOutflowChecker() {
    DLINK_INITIALIZE(&(m_outflow_chk_ctx.link));
    g_pExecMan->DelayExec(TPA_OF_CHECK, this,
                          m_outflow_chk_interval_ms,
                          &m_outflow_chk_ctx);
}

void Transporter::_DelOutflowChecker() {
    g_pExecMan->CancelExec(&m_outflow_chk_ctx);
}

void Transporter::_OnCheckOutflow(AsyncContext* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    UNLIKELY_IF(m_cnt_update_interval-- <= 0) {
        _DumpCounters();
        m_cnt_update_interval = config_const::kCounterDumpIntervalMs / m_outflow_chk_interval_ms;
    }

    if (LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark)) {
        UNLIKELY_IF(0 != m_pending_fetch_map.size()) { // trigger pending fetch
            _OnTriggerPendingFetch();
        }
    }
    UNLIKELY_IF(m_has_pending_recv) { // trigger pending recv
        _OnTriggerPendingRecv();
    }

    _SetOutflowChecker();
}

void Transporter::_DumpCounters() {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    m_cnt_all_ofs.UpdateQps();

    TPCounterDumpCtx* cnt_ctx = new TPCounterDumpCtx(m_id, m_cnt_all_ofs);
    if (NULL != m_inflow) {
        cnt_ctx->if_cnt     = m_inflow->DumpCounter();
        cnt_ctx->has_if_cnt = true;
    }
    // copy all outflow's counters
    BOOST_FOREACH(OutflowMap::value_type & pair, m_outflow_map) {
        cnt_ctx->ofs_cnt.cnt_ofs.push_back(pair.second->DumpCounter());
    }

    QueueDiskExec(TPA_CNT_DUMP,
                  static_cast<CAsyncClient*>(g_pCounterDumper), cnt_ctx, 0);
}

//-----------------------------------------------
// inflow management interface
ErrCode Transporter::AddIfSource(const std::vector<IfSourceMeta>* sources) {
    IfSourceMgrCtx* ctx = new IfSourceMgrCtx();
    ctx->sources = sources;
    QueueExec(error::OK, TPA_IF_SOURCE_ADD, this, ctx, false);
    return error::OK;
}

ErrCode Transporter::DelIfSource(const std::vector<IfSourceMeta>* sources) {
    IfSourceMgrCtx* ctx = new IfSourceMgrCtx();
    ctx->sources = sources;
    QueueExec(error::OK, TPA_IF_SOURCE_DEL, this, ctx, false);
    return error::OK;
}
ErrCode Transporter::UpdIfSource(const std::vector<IfSourceMeta>* sources) {
    IfSourceMgrCtx* ctx = new IfSourceMgrCtx();
    ctx->sources = sources;
    QueueExec(error::OK, TPA_IF_SOURCE_UPD, this, ctx, false);
    return error::OK;
}

//-----------------------------------------------
// outflow management interface
ErrCode Transporter::AddOutflow(const std::vector<OfMeta>* outflows) {
    OfMgrCtx* ctx = new OfMgrCtx();
    ctx->outflows = outflows;
    QueueExec(error::OK, TPA_OF_ADD, this, ctx, false);
    return error::OK;
}
ErrCode Transporter::DelOutflow(const std::vector<OfMeta>* outflows) {
    OfMgrCtx* ctx = new OfMgrCtx();
    ctx->outflows = outflows;
    QueueExec(error::OK, TPA_OF_DEL, this, ctx, false);
    return error::OK;
}
ErrCode Transporter::UpdOutflow(const std::vector<OfMeta>* outflows) {
    OfMgrCtx* ctx = new OfMgrCtx();
    ctx->outflows = outflows;
    QueueExec(error::OK, TPA_OF_UPD, this, ctx, false);
    return error::OK;
}

//-----------------------------------------------
// outflow data interface

static inline std::string VecToString(const std::vector<std::string>& tags) {
    std::string out;
    uint64_t i = 0;
    BOOST_FOREACH(std::string s, tags) {
        if (++i != 0) {
            out += ",";
        }
        out += s;
    }
    return out;
}

/**
* @brief        Add/Del/Upd outflow
*               if no tags assigned, all data will be subscribed.
* @param[in]    ctx outflows
* @todo         handle fail
*
*/
void Transporter::_OnAddOutflow(OfMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    ErrCode ret = error::OK;
    const std::vector<OfMeta>& of_metas = *(ctx->outflows);
    for (size_t i = 0; i < of_metas.size(); ++i) {
        const OfMeta& of_meta = of_metas[i];
        OutflowMap::iterator it = m_outflow_map.find(of_meta.id);
        if (it != m_outflow_map.end()) {
            DSTREAM_WARN("try add already existent outflow[%p "
                         "id=%lu, owner=" "%lu , parall=%lu, tags=%s",
                         it->second, of_meta.id, m_id, of_meta.parall,
                         VecToString(of_meta.tags).c_str());
            continue;
        }
        TPOutflow* of = new(std::nothrow)
        TPOutflow(of_meta.id, m_id, of_meta.parall);
        UNLIKELY_IF(NULL == of) {
            DSTREAM_WARN("fail to alloc outflow[id=%lu, owner=%lu]", of_meta.id, m_id);
            goto ERR_RET;
        }
        UNLIKELY_IF(error::OK !=
                    (ret = of->set_data_strategy(m_data_strategy)) ||
                    error::OK !=
                    (ret = of->set_queues(m_queue_name + kOfQueueName,
                                          m_cnt_grp,
                                          m_of_queue_size,
                                          m_of_queue_soft)) ||
                    error::OK != (ret = of->Start())) {
            DSTREAM_WARN("fail to start outflow[%p id=%lu, owner=%lu]: %s",
                         of, of_meta.id, m_id, error::ErrStr(ret));
            SAFE_DELETE(of);
            continue;
        }
        ret = m_tag_map.SubTags(of, of_meta.tags); // set tags

        std::string s;
        for (uint64_t j = 0; j < of_meta.parall; ++j) {
            if (0 != j) {
                s += ", ";
            }
            StringAppendNumber(&s, MAKE_PROCESSOR_PE_ID(of_meta.id, j));
        }
        UNLIKELY_IF(error::OK != ret) {
            DSTREAM_WARN("fail to add outflow[%p id=%lu, owner=%lu, "
                         "parall=%lu, pe=%s, tags=%s] to SubMap[%p]:"
                         " %s", of, of_meta.id, m_id, of_meta.parall,
                         s.c_str(), VecToString(of_meta.tags).c_str(),
                         &m_tag_map, error::ErrStr(ret));
            SAFE_DELETE(of);
            goto ERR_RET;
        }
        m_outflow_map[of_meta.id] = of;
        m_cnt_all_ofs.cnt_of_num.Set(m_outflow_map.size());

        DSTREAM_INFO("add outflow[%p id=%lu, owner=%lu, parall=%lu, "
                     "pe=%s, tags=%s]", of, of_meta.id,
                     m_id, of_meta.parall, s.c_str(),
                     VecToString(of_meta.tags).c_str());
        s.clear();
        m_tag_map.Dump(&s);
        DSTREAM_INFO("TagMap=%s", s.c_str());
    }

ERR_RET:
    SAFE_DELETE(ctx->outflows);
    SAFE_DELETE(ctx);
}

void Transporter::_OnDelOutflow(OfMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    const std::vector<OfMeta>& of_metas = *(ctx->outflows);
    for (size_t i = 0; i < of_metas.size(); ++i) {
        const OfMeta& of_meta = of_metas[i];
        OutflowMap::iterator it = m_outflow_map.find(of_meta.id);
        if (it != m_outflow_map.end()) {
            TPOutflow* of = it->second;
            m_tag_map.DelTags(of); // del tags
            m_full_queue_set.erase(of_meta.id);
            m_pending_fetch_map.erase(of_meta.id);
            of->Stop();
            m_outflow_map.erase(it);
            m_cnt_all_ofs.cnt_of_num.Set(m_outflow_map.size());
            DSTREAM_INFO("del outflow[%p id=%lu, owner=%lu]]",
                         of, of_meta.id, m_id);
        } else {
            DSTREAM_WARN("try del nonexistent outflow[id=%lu, "
                         "owner=%lu, tags=%s]", of_meta.id, m_id,
                         VecToString(of_meta.tags).c_str());
        }
    }
    std::string s;
    m_tag_map.Dump(&s);
    DSTREAM_INFO("TagMap=%s", s.c_str());

    SAFE_DELETE(ctx->outflows);
    SAFE_DELETE(ctx);
}

void Transporter::_OnUpdOutflow(OfMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    const std::vector<OfMeta>& of_metas = *(ctx->outflows);
    for (size_t i = 0; i < of_metas.size(); ++i) {
        const OfMeta& of_meta = of_metas[i];
        OutflowMap::iterator it = m_outflow_map.find(of_meta.id);
        if (it != m_outflow_map.end()) {
            it->second->OnUpdParall(of_meta.parall); // update parall
            m_tag_map.UpdTags(it->second, of_meta.tags); // update tags
            std::string s;
            for (uint64_t j = 0; j < of_meta.parall; ++j) {
                if (0 != j) {
                    s += ", ";
                }
                StringAppendNumber(&s, MAKE_PROCESSOR_PE_ID(of_meta.id, j));
            }
            DSTREAM_INFO("upd outflow[%p id=%lu, owner=%lu, parall=%lu,"
                         " pe=%s, tags=%s]", it->second, of_meta.id, m_id,
                         of_meta.parall, s.c_str(),
                         VecToString(of_meta.tags).c_str());

        } else {
            DSTREAM_WARN("try upd nonexistent outflow[id=%lu, owner=%lu, "
                         "parall=%lu, tags=%s]", of_meta.id, m_id,
                         of_meta.parall, VecToString(of_meta.tags).c_str());
        }
    }
    std::string s;
    m_tag_map.Dump(&s);
    DSTREAM_INFO("TagMap=%s", s.c_str());

    SAFE_DELETE(ctx->outflows);
    SAFE_DELETE(ctx);
}

//-----------------------------------------------
// inflow management interface
void Transporter::_OnInflowAddSource(IfSourceMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);
    if (NULL != m_inflow) {
        m_inflow->AddSource(*ctx->sources);
    }
    SAFE_DELETE(ctx->sources);
    SAFE_DELETE(ctx);
}
void Transporter::_OnInflowDelSource(IfSourceMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);
    if (NULL != m_inflow) {
        m_inflow->DelSource(*ctx->sources);
    }
    SAFE_DELETE(ctx->sources);
    SAFE_DELETE(ctx);
}
void Transporter::_OnInflowUpdSource(IfSourceMgrCtx* ctx) {
    VERIFY_OR_RETURN(m_running == true, BLANK);
    if (NULL != m_inflow) {
        m_inflow->UpdSource(*ctx->sources);
    }
    SAFE_DELETE(ctx->sources);
    SAFE_DELETE(ctx);
}

//-----------------------------------------------
// outflow data interface
ErrCode Transporter::_AckMsg(ESPADDRESS addr, uint64 lparam, const TPDataMsg& msg) {
    if (LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark)) {
        m_phdl           = AllocateHdl(true);
        m_phdl->nDataLen = sizeof(TPDataMsg);
        m_pmsg           = reinterpret_cast<TPDataMsg*>(m_phdl->pBuf);
        *m_pmsg          = msg;
        int32_t ret = PostMsg(addr, TPE_OF_RECV_LOCALPE, lparam, m_phdl);
        UNLIKELY_IF(ret != ESPE_OK) {
            DSTREAM_WARN("fail to post msg: "S_TP_DATA_MSG_FMT": %s",
                         DOTTED_TP_DATA_MSG(*m_pmsg),
                         error::ErrStr(ret + error::KYLIN_E_BEGIN));
        }
        ChainFreeHdl(m_phdl, NULL);
        m_phdl = NULL;
        return error::OK;
    }
    return error::ESP_TOUCH_WATERMARK;
}

ErrCode Transporter::_OnOutflowRecv(IN    ESPADDRESS      addr,
                                    IN    uint64_t        lparam,
                                    INOUT TPDataMsg*      msg,
                                    INOUT CBufReadStream* brs) {
    int      tag_len;
    int      tuple_len;
    uint64_t hash;
    bool     touch_soft_water_mark = false;
    ErrCode  ret = error::OK;
    for (; msg->batch > 0; msg->batch--) {
        CBufReadStream meta_brs(*brs);
        CBufReadStream tuple_brs(*brs);
        UNLIKELY_IF(!meta_brs.GetObject<int>(&tuple_len) ||
                    !meta_brs.GetObject<uint64_t>(&hash) ||
                    !meta_brs.GetObject<int>(&tag_len)   ||
                    (tag_len >= static_cast<int>(sizeof(m_tag))) ||
                    (tag_len != meta_brs.GetBuffer(m_tag, tag_len)) ||
                    (tuple_len >= kMaxMsgSize - static_cast<int>(sizeof(tuple_len)))) {
            ret = error::DATA_CORRUPTED;
            DSTREAM_WARN("%s: fail to extract tuple meta: lparam=%lu, "
                         S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                         lparam, DOTTED_TP_DATA_MSG(*msg));
            DSTREAM_FATAL("%s: fail to extract tuple meta: lparam=%lu, "
                          S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                          lparam, DOTTED_TP_DATA_MSG(*msg));
            return ret;
        }

        tuple_len += sizeof(tuple_len); // for total len
        const std::vector<TPOutflow*>* outs = m_tag_map.GetSubs(m_tag);
        if (NULL != outs) {
            if (DST_WHEN_QUEUE_FULL_HOLD == m_data_strategy) { // hold
                for (size_t j = 0; j < outs->size(); ++j) { // test queue status
                    ret = outs->at(j)->IsFull(hash);
                    if (error::OK != ret) {
                        const TPOutflow& of = *outs->at(j);
                        DSTREAM_WARN("delay write data into due to %s: "
                                      "outflow[%p, peid=%lu, owner=%lu, "
                                      "queue[%p, %s]: lparam=%lu, hash=%lu, "
                                      "size=%d, batch=1", error::ErrStr(ret),
                                      &of, of.id(hash), m_id, of.queue(hash),
                                      of.queue(hash)->DumpStatus().c_str(),
                                      lparam, hash, tuple_len);
                        return ret;
                    }
                }
            }
            for (size_t j = 0; j < outs->size(); ++j) {
                TPOutflow& of = *outs->at(j);
                ret = of.OnRecvData(msg->err, hash, tuple_len, 1, tuple_brs);
                if (LIKELY(error::OK == ret)) {
                    // DSTREAM_DEBUG("write data into outflow[%p, peid=%lu, "
                    //              "owner=%lu]...OK: hash=%lu, size=%d, batch"
                    //              "=1", &of, of.id(hash), m_id, hash, tuple_len);
                } else if (error::OK_SOFT_WATERMARK == ret) {
                    DSTREAM_WARN("queue of outflow[%p, peid=%lu, owner=%lu] "
                                  "is under soft-watermark: queue[%p, %s]: "
                                  "lparam=%lu, "S_TP_DATA_MSG_FMT,
                                  &of, of.id(hash), m_id, of.queue(hash),
                                  of.queue(hash)->DumpStatus().c_str(),
                                  lparam, DOTTED_TP_DATA_MSG(*msg));
                    if (DST_WHEN_QUEUE_FULL_HOLD == m_data_strategy) { // hold
                        m_full_queue_set.insert(of.id(hash));
                        touch_soft_water_mark = true;
                    }
                } else if (error::QUEUE_FULL == ret) { // should not happened
                    DSTREAM_WARN("!!!queue of outflow[%p, peid=%lu, owner=%lu]"
                                 " is full: queue[%p, %s]: lparam=%lu, "
                                 S_TP_DATA_MSG_FMT"; should not happens, "
                                 "please check data stategy and queue size",
                                 &of, of.id(hash), m_id, of.queue(hash),
                                 of.queue(hash)->DumpStatus().c_str(),
                                 lparam, DOTTED_TP_DATA_MSG(*msg));
                    return ret;
                } else {
                    DSTREAM_WARN("fail to write data into  outflow[%p, peid"
                                 "=%lu, peid=%lu]: queue[%p, %s]: lparam=%lu, "
                                 S_TP_DATA_MSG_FMT": %s", &of, of.id(hash), m_id,
                                 of.queue(hash),
                                 of.queue(hash)->DumpStatus().c_str(), lparam,
                                 DOTTED_TP_DATA_MSG(*msg), error::ErrStr(ret));
                    return ret;
                }
            } // end of for outs
        } else {
            std::string s;
            DumpTuplesFromBufStream(&tuple_brs, tuple_len, 1, &s);
            DSTREAM_DEBUG("drop data with tag[%s] due to no subs: lparam=%lu "
                          S_TP_DATA_MSG_FMT": %s", m_tag, lparam,
                          DOTTED_TP_DATA_MSG(*msg), s.c_str());
            m_cnt_all_ofs.cnt_drop_nosubs_num.Add(1);
            m_cnt_all_ofs.cnt_drop_nosubs_size.Add(tuple_len);
        }
        UNLIKELY_IF(!brs->Seek(tuple_len)) { // skip this tuple
            ret = error::DATA_CORRUPTED;
            DSTREAM_FATAL("%s: fail to seek tuple len: lparam=%lu, "
                          S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                          lparam, DOTTED_TP_DATA_MSG(*msg));
            return ret;
        }
        msg->size   -= tuple_len;
        m_cnt_all_ofs.cnt_in_num.Add(1);
        m_cnt_all_ofs.cnt_in_size.Add(tuple_len);
    } // end of for msg.batch

    return touch_soft_water_mark ? error::OK_SOFT_WATERMARK : error::OK;
}

void Transporter::OnOutflowRecv(ESPMessage* esp_msg) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    CBufReadStream brs(esp_msg->req.pHdl, NULL);

    int32_t ret = error::OK;
    if (LIKELY(brs.GetObject<TPDataMsg>(&m_msg))) {
        if (LIKELY((m_msg.from_id == m_id) && (m_msg.to_id == m_id))) {
            if (LIKELY(m_msg.ack == m_outflow_ack)) {
                if (LIKELY(!m_has_pending_recv)) {
                    ret = error::OK;
                } else {
                    m_pending_recv.addr      = esp_msg->aFrom.addr;
                    m_pending_recv.lparam    = esp_msg->lParam;
                    m_pending_recv.recv_time = GetUint64TimeMs();
                    DSTREAM_DEBUG("update pending recv msg: this=%p, "
                                  S_TP_OFPR_FMT, this,
                                  DOTTED_TP_OFPR(m_pending_recv));
                    m_cnt_all_ofs.cnt_pending_recv_num.Add(m_msg.batch);
                    m_cnt_all_ofs.cnt_pending_recv_size.Add(m_msg.size);
                    return;
                }
            } else {
                m_cnt_all_ofs.cnt_drop_ack_miss_num.Add(m_msg.batch);
                m_cnt_all_ofs.cnt_drop_ack_miss_size.Add(m_msg.size);
                ret = error::DROP_MSG_DUE_ACK_MISS_MATCH;
            }
        } else {
            ret = error::DROP_MSG_DUE_ID_MISS_MATCH;
        }
    } else {
        DSTREAM_FATAL("corrupt msg: fail to get msg");
        ret = error::DATA_CORRUPTED;
        m_outflow_ack++;
        m_msg.from_id = m_id;
        m_msg.to_id = m_id;
    }
    UNLIKELY_IF(error::OK != ret) {
        DSTREAM_WARN("%s: this=%p, outflow_ack=%lu: "S_TP_DATA_MSG_FMT,
                     error::ErrStr(ret), this, m_outflow_ack,
                     DOTTED_TP_DATA_MSG(m_msg));
        goto ERR_RET;
    }

    DSTREAM_DEBUG("get msg: this=%p, lparam=%lu, "S_TP_DATA_MSG_FMT,
                  this, static_cast<uint64_t>(esp_msg->lParam), DOTTED_TP_DATA_MSG(m_msg));

    UNLIKELY_IF(0UL == m_outflow_map.size()) { // exporter, just ignore
        DSTREAM_DEBUG("this=%p exporter[id=%lu] get msg: "S_TP_DATA_MSG_FMT,
                      this, m_id, DOTTED_TP_DATA_MSG(m_msg));
        ret = TPN_ACK_OK;
        m_outflow_ack++;
        goto ERR_RET;
    }

    int org_size = m_msg.size;
    ret = _OnOutflowRecv(esp_msg->aFrom.addr, esp_msg->lParam, &m_msg, &brs);
    UNLIKELY_IF(ret < error::OK) {
        if (error::QUEUE_FULL == ret || error::DATA_CORRUPTED == ret) {
            // skip this data
            m_outflow_ack++;
            std::string s;
            DumpTuplesFromBufStream(&brs, m_msg.size, m_msg.batch, &s);
            DSTREAM_WARN("drop these data due to: %s: this=%p, lparam=%lu"
                         S_TP_DATA_MSG_FMT": %s", error::ErrStr(ret),
                         this, static_cast<uint64_t>(esp_msg->lParam),
                         DOTTED_TP_DATA_MSG(m_msg), s.c_str());
            if (LIKELY(error::QUEUE_FULL == ret)) {
                m_cnt_all_ofs.cnt_drop_queue_full_num.Add(m_msg.batch);
                m_cnt_all_ofs.cnt_drop_queue_full_size.Add(m_msg.size);
            }
        }
    } else {
        if (0 != m_pending_fetch_map.size()) { // trigger pending fetch
            _OnTriggerPendingFetch();
        }
        if (LIKELY(error::OK == ret)) {
            m_outflow_ack++;
        } else if (error::OK_SOFT_WATERMARK == ret) { // hold
            if (DST_WHEN_QUEUE_FULL_HOLD == m_data_strategy) { // hold
                UNLIKELY_IF(TPGetFreeTranBufPercent() < g_kKylinWriteWaterMark) {
                    std::string s;
                    DumpTuplesFromBufStream(&brs, m_msg.size, m_msg.batch, &s);
                    DSTREAM_WARN("fail to pending msg due to %s: this=%p: "
                                 "lparam=%lu, "S_TP_DATA_MSG_FMT
                                 ": drop these data: %s",
                                 error::ErrStr(error::ESP_TOUCH_WATERMARK),
                                 this, static_cast<uint64_t>(esp_msg->lParam),
                                 DOTTED_TP_DATA_MSG(m_msg), s.c_str());
                    m_cnt_all_ofs.cnt_drop_tranbuf_full_num.Add(m_msg.batch);
                    m_cnt_all_ofs.cnt_drop_tranbuf_full_size.Add(m_msg.size);
                    return;
                }
                m_has_pending_recv = true;
                m_pending_recv.hdl = CloneHdl(esp_msg->req.pHdl, NULL);
                m_pending_recv.msg = m_msg;
                m_pending_recv.addr   = esp_msg->aFrom.addr;
                m_pending_recv.lparam = esp_msg->lParam;
                m_pending_recv.recv_time = GetUint64TimeMs();
                m_cnt_all_ofs.cnt_pending_recv_num.Add(m_msg.batch);
                m_cnt_all_ofs.cnt_pending_recv_size.Add(m_msg.size);

                CBufReadStream remain_brs(m_pending_recv.hdl, NULL);
                remain_brs.Seek(org_size - m_msg.size + static_cast<int>(sizeof(TPDataMsg)));
                m_pending_recv.brs = remain_brs;
                DSTREAM_WARN("pending recv due to queue full: touch the "
                              "soft-watermark: this=%p, lparam=%lu, "
                              S_TP_OFPR_FMT, this, static_cast<uint64_t>(esp_msg->lParam),
                              DOTTED_TP_OFPR(m_pending_recv));
                return;
            }
        } else { // should not happend
            DSTREAM_FATAL("!!!should not happen: %s: this=%p, lparam=%lu, "
                          S_TP_DATA_MSG_FMT, error::ErrStr(ret), this,
                          static_cast<uint64_t>(esp_msg->lParam), DOTTED_TP_DATA_MSG(m_msg));
        }
    }

ERR_RET:
    m_msg.err = ret;
    m_msg.ack = m_outflow_ack;
    _AckMsg(esp_msg->aFrom.addr, esp_msg->lParam, m_msg);
}

ErrCode Transporter::_OnOutflowFetch(ESPADDRESS addr,
                                     uint64_t   lparam,
                                     uint64_t   id,
                                     TPDataMsg& msg) {
    ErrCode ret = error::OK;
    // find send queue
    uint64_t pro_id = EXTRACT_APPPRO_ID(id);
    uint64_t seq    = EXTRACT_SEQUENCE_ID(id);
    OutflowMap::const_iterator it = m_outflow_map.find(pro_id);
    UNLIKELY_IF(it == m_outflow_map.end()) {
        ret = error::NOT_FOUND_SEND_QUEUE;
        DSTREAM_WARN("%s: "S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                     DOTTED_TP_DATA_MSG(msg));
        return ret;
    }

    UNLIKELY_IF(TPGetFreeTranBufPercent() < g_kKylinWriteWaterMark) {
        ret = error::ESP_TOUCH_WATERMARK;
        DSTREAM_DEBUG("%s : this=%p, lparam=%lu, "S_TP_DATA_MSG_FMT,
                      error::ErrStr(ret), this, lparam,
                      DOTTED_TP_DATA_MSG(msg));
        return ret;
    }

    TPOutflow& of = *it->second;
    CBufWriteStream bws;
    bws.PutObject<TPDataMsg>(msg); // reserve for msg head
    uint64_t ack   = msg.ack;
    int32_t  size  = msg.size;
    int32_t  batch = msg.batch;
    ret = of.OnFetchData(msg.err, seq, &ack, &size, &batch, &bws);
    UNLIKELY_IF(m_has_pending_recv) { // updatae full queue set
        if (error::OK == of.IsFull(seq)) {
            m_full_queue_set.erase(msg.from_id);
        }
    }
    UNLIKELY_IF(ret < error::OK) { // inclue queue empty
        if (error::QUEUE_EMPTY == ret) {
            DSTREAM_DEBUG("%s: this=%p of[%p, id=%lu, owner=%lu, ack=%lu, "
                          "queue=%p]: lparam=%lu, "S_TP_DATA_MSG_FMT,
                          error::ErrStr(ret), this, &of, id, m_id,
                          of.ack(seq), of.queue(seq), lparam,
                          DOTTED_TP_DATA_MSG(msg));
        } else {
            DSTREAM_WARN("%s: this=%p of[%p, id=%lu, owner=%lu, ack=%lu, "
                         "queue=%p]: lparam=%lu, "S_TP_DATA_MSG_FMT,
                         error::ErrStr(ret), this, &of, id, m_id,
                         of.ack(seq), of.queue(seq), lparam,
                         DOTTED_TP_DATA_MSG(msg));
        }
        return ret;
    }
    m_cnt_all_ofs.cnt_out_num.Add(0 == size ? 0 : batch);
    m_cnt_all_ofs.cnt_out_size.Add(size);

    UNLIKELY_IF(0 == size && TPN_ACK_OK == msg.err) { // inflow ack msg, donot ack
        DSTREAM_DEBUG("deal flush msg ok: this=%p, addr="S_DOTTED_ESPADDR
                      ", lparam=%lu, " S_TP_DATA_MSG_FMT, this,
                      DOTTED_ESPADDR(addr), lparam, DOTTED_TP_DATA_MSG(msg));
        return error::OK;
    }

    // OK, ack msg
    m_phdl          = bws.GetBegin();
    m_pmsg          = new(m_phdl->pBuf) TPDataMsg();
    *m_pmsg         = msg;
    m_pmsg->from_id = msg.to_id;
    m_pmsg->to_id   = msg.from_id;
    m_pmsg->err     = msg.err;
    m_pmsg->ack     = ack;
    m_pmsg->size    = size;
    m_pmsg->batch   = batch;
    int esp_ret = PostMsg(addr, TPE_OF_SUB_FETCH_ACK, lparam, m_phdl);
    UNLIKELY_IF(ESPE_OK != esp_ret) {
        ret = (ErrCode)(error::KYLIN_E_BEGIN + esp_ret);
        DSTREAM_WARN("fail to post msg: %s: this=%p, addr="S_DOTTED_ESPADDR
                     "lparam=%lu, "S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                     this, DOTTED_ESPADDR(addr), lparam,
                     DOTTED_TP_DATA_MSG(*m_pmsg));
    }
    DSTREAM_DEBUG("ack msg: this=%p, addr="S_DOTTED_ESPADDR", lparam=%lu, "
                  S_TP_DATA_MSG_FMT, this, DOTTED_ESPADDR(addr),
                  lparam, DOTTED_TP_DATA_MSG(*m_pmsg));
    ChainFreeHdl(m_phdl, NULL);
    m_phdl = NULL;
    return ret;
}

void Transporter::OnOutflowFetch(ESPMessage* esp_msg) {
    VERIFY_OR_RETURN(m_running == true, BLANK);

    CBufReadStream  brs(esp_msg->req.pHdl, NULL);
    UNLIKELY_IF(!brs.GetObject<TPDataMsg>(&m_msg)) {
        DSTREAM_WARN("corrupt msg: fail to extract Msg Head: this=%p, "
                     S_ESP_MSG_FMT, this, DOTTED_ESP_MSG(*esp_msg));
        return;
    }

    DSTREAM_DEBUG("get msg: this=%p, lparam=%lu, "S_TP_DATA_MSG_FMT, this,
                  static_cast<uint64_t>(esp_msg->lParam), DOTTED_TP_DATA_MSG(m_msg));
    ErrCode ret = _OnOutflowFetch(esp_msg->aFrom.addr,
                                  esp_msg->lParam,
                                  m_msg.from_id, m_msg);
    UNLIKELY_IF(ret < error::OK) {
        UNLIKELY_IF(error::QUEUE_EMPTY != ret &&
                    error::ESP_TOUCH_WATERMARK != ret) {
            return;
        }
        // pending fetch
        PendingFetch pf;
        pf.msg       = m_msg;
        pf.addr      = esp_msg->aFrom.addr;
        pf.lparam    = esp_msg->lParam;
        pf.recv_time = GetUint64TimeMs();
        m_pending_fetch_map[m_msg.from_id] = pf;
        m_cnt_all_ofs.cnt_pending_fetch_num.Add(1);
        DSTREAM_DEBUG("pending fetch due to %s: this=%p, lparam=%lu, "
                      S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                      this, static_cast<uint64_t>(esp_msg->lParam),
                      DOTTED_TP_DATA_MSG(m_msg));
    }
    if (m_has_pending_recv) {
        _OnTriggerPendingRecv(); // trigger pending recv
    }
}

void Transporter::_OnTriggerPendingRecv() {
    DSTREAM_DEBUG("trigger pending recv: this=%p "S_TP_OFPR_FMT,
                  this, DOTTED_TP_OFPR(m_pending_recv));
    ErrCode ret = _OnOutflowRecv(m_pending_recv.addr, m_pending_recv.lparam,
                                 &m_pending_recv.msg, &m_pending_recv.brs);
    if (LIKELY(error::OK == ret)) {
        m_outflow_ack++;
        ret = (ErrCode)TPN_ACK_OK;
    } else if (error::OK_SOFT_WATERMARK == ret) { // hold
        return;
    } else {
        if (error::QUEUE_FULL == ret || error::DATA_CORRUPTED == ret) {
            // skip this data
            m_outflow_ack++;
            std::string s;
            DumpTuplesFromBufStream(&m_pending_recv.brs,
                                    m_pending_recv.msg.size,
                                    m_pending_recv.msg.batch, &s);
            DSTREAM_WARN("del pending msg: due to: %s: this=%p "
                         S_TP_OFPR_FMT": %s", error::ErrStr(ret), this,
                         DOTTED_TP_OFPR(m_pending_recv), s.c_str());
            if (LIKELY(error::QUEUE_FULL == ret)) {
                m_cnt_all_ofs.cnt_drop_queue_full_num.Add(m_pending_recv.msg.batch);
                m_cnt_all_ofs.cnt_drop_queue_full_size.Add(m_pending_recv.msg.size);
            }
        }
    }
    DSTREAM_DEBUG("del pending recv: due to: %s: this=%p, "
                  S_TP_OFPR_FMT, error::ErrStr(ret), this,
                  DOTTED_TP_OFPR(m_pending_recv));
    m_has_pending_recv = false;
    m_pending_recv.Reset();

    m_pending_recv.msg.err = ret;
    m_pending_recv.msg.ack = m_outflow_ack;
    _AckMsg(m_pending_recv.addr, m_pending_recv.lparam, m_pending_recv.msg);
    return;
}

void Transporter::_OnTriggerPendingFetch() {
    for (PendingFetchMap::iterator it = m_pending_fetch_map.begin();
         it != m_pending_fetch_map.end();) {
        PendingFetch& pf = it->second;
        uint64_t id = pf.msg.from_id;
        DSTREAM_DEBUG("trigger pending fetch: this=%p "
                      S_TP_OFPF_FMT, this, DOTTED_TP_OFPF(pf));
        ErrCode ret = _OnOutflowFetch(pf.addr, pf.lparam, id, pf.msg);
        UNLIKELY_IF(ret < error::OK) {
            if (error::QUEUE_EMPTY == ret) { // keep pending
                it++;
                continue;
            } else if (error::ESP_TOUCH_WATERMARK == ret) {
                it++;
                break;
            }
        }
        DSTREAM_DEBUG("del pending fetch due to: %s: this=%p "
                      S_TP_OFPF_FMT, error::ErrStr(ret),
                      this, DOTTED_TP_OFPF(pf));
        m_pending_fetch_map.erase(it++);
    }
}

void Transporter::OnCompletion(AsyncContext* ctx) {
    if (LIKELY(ctx->nAction < AA_USER_BEGIN)) {
        CESPObject::OnCompletion(ctx);
        return;
    }

    switch (ctx->nAction) {
    case TPS_STOP:
        return _OnStop(ctx);
    case TPA_IF_SOURCE_ADD:
        return _OnInflowAddSource(static_cast<IfSourceMgrCtx*>(ctx));
    case TPA_IF_SOURCE_DEL:
        return _OnInflowDelSource(static_cast<IfSourceMgrCtx*>(ctx));
    case TPA_IF_SOURCE_UPD:
        return _OnInflowUpdSource(static_cast<IfSourceMgrCtx*>(ctx));
    case TPA_OF_ADD:
        return _OnAddOutflow(static_cast<OfMgrCtx*>(ctx));
    case TPA_OF_DEL:
        return _OnDelOutflow(static_cast<OfMgrCtx*>(ctx));
    case TPA_OF_UPD:
        return _OnUpdOutflow(static_cast<OfMgrCtx*>(ctx));
    case TPA_OF_CHECK:
        return _OnCheckOutflow(ctx);
    default:
        DSTREAM_WARN("unkown AsyncContext=[%p nErrCode=%d, nActon=%d, "
                     "pClient=%p, fProc = %p]", ctx, ctx->nErrCode,
                     ctx->nAction, ctx->pClient, ctx->fProc);
        return;
    }
}

} // namespace transporter
} // namespace dstream
