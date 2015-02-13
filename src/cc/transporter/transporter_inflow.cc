#include "transporter/transporter_inflow.h"
#include <string>
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {
namespace transporter {

ESP_BEGIN_MAP_MESSAGE(TPInflow, CESPObject)
ESP_MAP_MESSAGE_RAW(TPE_IF_FETCH_SOURCE_ACK, OnRecvData)
ESP_MAP_MESSAGE_RAW(TPE_IF_LOCALPE_FETCH, OnFetchData)
ESP_MAP_MESSAGE_RAW(TPE_SM_QUERY_OF_ACK, OnQueryStubAck)
ESP_END_MAP_MESSAGE

TPInflow::PendingRecv::PendingRecv()
    : hdl(NULL),
      size(0),
      batch(0) {
}

TPInflow::PendingFetch::PendingFetch()
    : afrom(INVALID_ESPADDRESS),
      lparam(0),
      recv_time(0) {
}

#define S_TP_SOURCE_FMT \
    "TPSource[%p]=[id=%lu, addr="S_DOTTED_ESPADDR", status=%s, retry=%u, " \
    "ack=%lu, start_time=%lu, last_send=%lu, last_recv=%lu, now=%lu]"
#define DOTTED_TP_SOURCE(s) \
    &(s), (s).id, DOTTED_ESPADDR((s).addr), SourceStatusStr((s).status), \
    (s).retry, (s).ack, (s).start_time, (s).last_send, (s).last_recv, GetUint64TimeMs()
#define S_TP_IFPR_FMT \
    "IfPendingRecv[%p]=[hdl=%p, ack=%lu, size=%d, batch=%d]"
#define DOTTED_TP_IFPR(pr) \
    &(pr), (pr).hdl, (pr).ack, (pr).size, (pr).batch
#define S_TP_IFPF_FMT \
    "IfPendingFetch[%p]=[msg="S_TP_DATA_MSG_FMT", afrom="S_DOTTED_ESPADDR \
    ", lparam=%lu, recv_time=%lu]"
#define DOTTED_TP_IFPF(pf) \
    &(pf), DOTTED_TP_DATA_MSG((pf).msg), DOTTED_ESPADDR((pf).afrom), \
    (pf).lparam, (pf).recv_time

static const char* SourceStatusStr(int err) {
    switch (err) {
    case TPInflow::SOURCE_OK:
        return "OK";
    case TPInflow::SOURCE_SYNC:
        return "SYNC";
    case TPInflow::SOURCE_INVALID:
        return "INVALID";
    default:
        return "unkown status";
    };
}

TPInflow::TPSource::TPSource(uint64_t id, ESPNODE node)
    : id(id),
      node(node),
      addr(INVALID_ESPADDRESS),
      status(SOURCE_INVALID),
      retry(0),
      ack(0),
      stub_msg_id(0),
      data_msg_id(0),
      last_send(0),
      last_recv(0),
      has_pending_recv(false) {
    counter.id = id;
    start_time = GetUint64TimeMs();
    DLINK_INITIALIZE(&link);
}

TPInflow::TPInflow(CESPObject* host, uint64_t id, ESPSTUB stub) :
    m_running(false),
    m_owner(host),
    m_id(id),
    m_stub(stub),
    m_queue(NULL),
    m_msg_ack(0),
    m_queue_ack(kAdvanceQueueAck),
    m_queue_size(config_const::kPNRecvQueueSize),
    m_queue_soft(g_kKylinBufLowMark),
    m_batch_size(config_const::kPNTuplesBatchSizeByte),
    m_batch_num(config_const::kPNTuplesBatchNum),
    m_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
    m_fetch_data_timeout_ms(config_const::kDataFetchTimeoutMs),
    m_check_interval_ms(config_const::kDataCheckIntervalMs),
    m_max_retry(config_const::kDataFetchRetry),
    m_pending_recv_num(0),
    m_has_pending_fetch(0) {
    AddRef();
}

TPInflow::~TPInflow() {
    _OnStop();
}

int TPInflow::Release() {
    int n = CAsyncClient::Release();
    if (0 == n) {
        DSTREAM_DEBUG("inflow[%p] delete", this);
        delete this;
    }
    return n;
}

ErrCode TPInflow::Start()  {
    VERIFY_OR_RETURN(NULL != m_owner, error::BAD_ARGUMENT);

    SetHost(m_owner);

    VERIFY_OR_RETURN(INVALID_ESPSTUB != m_stub,   error::BIND_STUB_FAIL);
    VERIFY_OR_RETURN(ESPE_OK == BindStub(m_stub), error::BIND_STUB_FAIL);

    m_queue = new (std::nothrow) TPAckQueue(m_id, 0, m_queue_name, m_cnt_grp.c_str());
    VERIFY_OR_RETURN(NULL != m_queue, error::BAD_MEM_ALLOC);
    ErrCode ret = m_queue->Init(m_queue_size, m_queue_soft);
    VERIFY_OR_RETURN(error::OK == ret, ret);

    _SetSourceChecker();

    m_running = true;
    DSTREAM_INFO("inflow[%p id=%lu, queue=%p] start run...", this, m_id, m_queue);
    return error::OK;
}

void TPInflow::Stop() {
    AsyncContext* ctx = new (std::nothrow ) AsyncContext();
    VERIFY_OR_RETURN(NULL != ctx, BLANK);
    QueueExec(error::OK, TPA_IF_STOP, this, ctx, false);

    DSTREAM_INFO("inflow[%p] stop...", this);
}

void TPInflow::_OnStop(AsyncContext* ctx) {
    if (NULL != ctx) {
        SAFE_DELETE(ctx);
    }

    m_running = false;
    UnbindStub();

    _DelSources();
    _DelSourceChecker();

    m_owner = NULL;
    if (NULL != m_queue) {
        m_queue->Destroy();
    }
    SAFE_DELETE(m_queue);
}

ErrCode TPInflow::set_data_strategy(int strategy) {
    VERIFY_OR_RETURN(IsValidDataStrategyType(strategy), error::BAD_ARGUMENT);
    m_data_strategy = strategy;
    return error::OK;
}

int TPInflow::data_strategy() const {
    return m_data_strategy;
}

ErrCode TPInflow::set_queues(const  std::string& name,
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

void TPInflow::set_batch_size(int batch_size) {
    m_batch_size = batch_size < kMinBatchSize ? kMinBatchSize : batch_size;
}

int TPInflow::batch_size() const {
    return m_batch_size;
}

void TPInflow::set_batch_num(int batch_num) {
    m_batch_num = batch_num < 1 ? 1 : batch_num;
}

int TPInflow::set_batch_num() {
    return m_batch_num;
}

const TPIfCounter& TPInflow::DumpCounter() {
    assert(m_counter.cnt_source_num.total() == m_sources.size());

    m_counter.UpdateQps();

    m_counter.cnt_sources.clear();
    if (0 != m_counter.cnt_source_num.total()) {
        BOOST_FOREACH(TPSourceMap::value_type & pair, m_sources) {
            TPSourceCounter& source_cnt = pair.second->counter; 
            source_cnt.UpdateQps();
            m_counter.cnt_sources.push_back(source_cnt);
        }
    }
    m_counter.cnt_queue = m_queue->DumpCounter();
    return m_counter;
}

//-----------------------------------------------
// source management interface
ErrCode TPInflow::AddSource(const std::vector<IfSourceMeta>& sources,
                            std::vector<ErrCode>* res) {
    VERIFY_OR_RETURN(true == m_running, error::NOT_RUNNING);
    for (size_t i = 0; i < sources.size(); ++i) {
        ErrCode ret = error::OK;
        TPSourceMap::const_iterator it = m_sources.find(sources[i].id);
        if (it == m_sources.end()) {
            TPSource* s = new (std::nothrow) TPSource(sources[i].id, sources[i].node);
            if (NULL == s) {
                ret = error::BAD_MEM_ALLOC;
                DSTREAM_WARN("fail to add source[id=%lu, owner=%lu, Addr="
                             S_DOTTED_ESPNODE"]: %s", sources[i].id, m_id,
                             DOTTED_ESPNODE(sources[i].node), error::ErrStr(ret));
            } else {
                uint64_t id = sources[i].id;
                m_sources[id] = s;
                DSTREAM_INFO("add source[%p id=%lu, owner=%lu Addr="
                             S_DOTTED_ESPNODE"]", s, sources[i].id, m_id,
                             DOTTED_ESPNODE(sources[i].node));
                _QueryStub(s);
                m_counter.cnt_source_num.Add(1);
                ret = error::OK;
            }
        } else {
            DSTREAM_WARN("try add already existent source[%p id=%lu, "
                         "owner=%lu," "node="S_DOTTED_ESPNODE"]",
                         it->second, it->second->id, m_id,
                         DOTTED_ESPNODE(it->second->node));
            ret = error::SOURCE_ALREADY_EXIST;
        }
        if (NULL != res) {
            (*res)[i] = ret;
        }
    }
    return error::OK;
}

ErrCode TPInflow::DelSource(const std::vector<IfSourceMeta>& sources,
                            std::vector<ErrCode>* res) {
    VERIFY_OR_RETURN(true == m_running, error::NOT_RUNNING);
    for (size_t i = 0; i < sources.size(); ++i) {
        TPSourceMap::iterator it = m_sources.find(sources[i].id);
        if (it == m_sources.end()) {
            DSTREAM_WARN("try del nonexistent source[id=%lu, owner=%lu,"
                         " Addr="S_DOTTED_ESPNODE"]", sources[i].id,
                         m_id, DOTTED_ESPNODE(sources[i].node));
            if (res) {
                (*res)[i] = error::SOURCE_NOT_EXIST;
            }
            continue;
        }
        TPSource* s = it->second;
        if (s->has_pending_recv) {
            m_pending_recv_num--;
        }
        if (res) {
            (*res)[i] = error::OK;
        }
        DSTREAM_INFO("del source[%p id=%lu, owner=%lu, start_time=%lu]",
                     s, s->id, m_id, s->start_time);
        m_counter.cnt_source_num.Sub(1);
        SAFE_DELETE(s);
        m_sources.erase(it);
    }
    return error::OK;
}

ErrCode TPInflow::UpdSource(const std::vector<IfSourceMeta>& sources,
                            std::vector<ErrCode>* res) {
    VERIFY_OR_RETURN(true == m_running, error::NOT_RUNNING);
    for (size_t i = 0; i < sources.size(); ++i) {
        ErrCode ret = error::OK;
        TPSourceMap::iterator it = m_sources.find(sources[i].id);
        if (it != m_sources.end()) {
            TPSource* s = it->second;
            DSTREAM_INFO("update: source[%p id=%lu, owner=%lu]: "
                         "addr"S_DOTTED_ESPNODE" => "S_DOTTED_ESPNODE
                         ", status: %s => INVALID", s, sources[i].id, m_id,
                         DOTTED_ESPNODE(s->node),
                         DOTTED_ESPNODE(sources[i].node),
                         SourceStatusStr(s->status));
            s->node   = sources[i].node;
            s->status = SOURCE_INVALID;
            _QueryStub(s);
            ret = error::OK;
        } else {
            DSTREAM_WARN("try upd nonexistent source[id=%lu, owner:%lu, "
                         "node="S_DOTTED_ESPNODE"]", sources[i].id, m_id,
                         DOTTED_ESPNODE(sources[i].node));
            ret = error::SOURCE_NOT_EXIST;
        }
        if (NULL != res) {
            (*res)[i] = ret;
        }
    }
    return error::OK;
}

//-----------------------------------------------
// fetch/cancel source
// #define CHECKER_DEBUG
#ifdef  CHECKER_DEBUG
#define _TP_MULTITIME 100
#else
#define _TP_MULTITIME 1
#endif

void TPInflow::_SetSourceChecker() {
    DLINK_INITIALIZE(&(m_check_source_ctx.link));
    g_pExecMan->DelayExec(TPA_IF_CHECK_SOURCE, this,
                          m_check_interval_ms * _TP_MULTITIME,
                          &m_check_source_ctx);
}

void TPInflow::_DelSourceChecker() {
    g_pExecMan->CancelExec(&m_check_source_ctx);
}

void TPInflow::_DelSources() {
    BOOST_FOREACH(TPSourceMap::value_type & pair, m_sources) {
        TPSource* s = pair.second;
        if (NULL != s) {
            if (s->has_pending_recv) {
                m_pending_recv_num--;
            }
            DSTREAM_INFO("del source[%p id=%lu, owner=%lu, start_time=%lu]",
                         s, s->id, m_id, s->start_time);
            SAFE_DELETE(s);
        }
    }
    m_sources.clear();
    m_counter.cnt_source_num.Set(0);
}

// donot delete ctx;
void TPInflow::_OnCheckSource(AsyncContext* ctx) {
    VERIFY_OR_RETURN(true == m_running, BLANK);
#ifdef CHECKER_DEBUG
    static uint64_t count = 0;
    DSTREAM_DEBUG("%luth triggled checkSource", count++);
#endif
    DSTREAM_UNUSED_VAL(ctx);

    if (LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark)) {
        m_counter.cnt_sync_source_num.Set(0);
        m_counter.cnt_invalid_source_num.Set(0);
        BOOST_FOREACH(TPSourceMap::value_type & pair, m_sources) {
            if (!CheckSource(pair.second)) {
                break;
            }
        }
    }

    _SetSourceChecker();
}

bool TPInflow::CheckSource(TPSource* source) {
    VERIFY_OR_RETURN(true == m_running, false);
    DSTREAM_DEBUG("check source: "S_TP_SOURCE_FMT, DOTTED_TP_SOURCE(*source));

    if (LIKELY(!m_queue->Empty())) {
        UNLIKELY_IF(m_has_pending_fetch) {
            _OnTriggerPendingFetch();
        }
    }

    UNLIKELY_IF(error::OK != m_queue->IsFull()) { // queue error, full, or softfull
        return true;
    }

    if (LIKELY(SOURCE_OK == source->status)) {
        UNLIKELY_IF(IsAfter(source->last_send, GetUint64TimeMs(),
                            m_fetch_data_timeout_ms)) {
            if (source->retry > m_max_retry) {
                DSTREAM_INFO(S_TP_SOURCE_FMT", OK => INVALID",
                             DOTTED_TP_SOURCE(*source));
                source->status = SOURCE_INVALID;
                source->retry = 0;
                _QueryStub(source);
            } else {
                _FetchSource(source);
            }
            source->retry++;
        }
    } else if (SOURCE_SYNC == source->status) {
        m_counter.cnt_sync_source_num.Add(1);
        UNLIKELY_IF(IsAfter(source->last_send, GetUint64TimeMs(),
                            m_fetch_data_timeout_ms)) {
            if (source->retry > m_max_retry) {
                DSTREAM_INFO(S_TP_SOURCE_FMT", SYNC => INVALID",
                             DOTTED_TP_SOURCE(*source));
                source->status = SOURCE_INVALID;
                source->retry = 0;
                _QueryStub(source);
            } else {
                _FetchSource(source);
            }
            source->retry++;
        }
    } else { // SOURCE_INVALID == source->status
        m_counter.cnt_invalid_source_num.Add(1);
        UNLIKELY_IF(IsAfter(source->last_send, GetUint64TimeMs(),
                            m_fetch_data_timeout_ms)) {
            _QueryStub(source);
            source->retry++;
        }
    }
    return true;
}

void TPInflow::_QueryStub(TPSource* source) {
    VERIFY_OR_RETURN(LIKELY(true == m_running), BLANK);
    VERIFY_OR_RETURN(LIKELY(INVALID_ESPNODE != source->node), BLANK);

    source->last_send = GetUint64TimeMs();
    VERIFY_OR_RETURN(LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark), BLANK);

    m_phdl               = AllocateHdl(true);
    m_phdl->nDataLen     = sizeof(TPStubMsg);
    m_pstub_msg = new(m_phdl->pBuf) TPStubMsg();
    m_pstub_msg->from_id = m_id;
    m_pstub_msg->to_id   = source->id;
    m_pstub_msg->err     = TPN_ACK_OK;
    m_pstub_msg->stub    = INVALID_ESPSTUB;
    int esp_ret = PostMsg(MakeESPADDR(source->node, kTPStubMgrStub),
                          TPE_SM_QUERY_OF, source->stub_msg_id++, m_phdl);
    UNLIKELY_IF(esp_ret != ESPE_OK) {
        DSTREAM_WARN("fail to post stub msg@"S_TP_SOURCE_FMT": err=%s, "
                     "addr="S_DOTTED_ESPADDR", lparam=%lu, "
                     S_TP_QUERY_STUB_MSG_FMT, DOTTED_TP_SOURCE(*source),
                     error::ErrStr(error::KYLIN_E_BEGIN + esp_ret),
                     DOTTED_ESPADDR(MakeESPADDR(source->node, kTPStubMgrStub)),
                     static_cast<uint64_t>(source->stub_msg_id - 1),
                     DOTTED_TP_QEURY_STUB_MSG(*m_pstub_msg));
    } else {
        DSTREAM_DEBUG("send stub query msg@"S_TP_SOURCE_FMT": addr="
                      S_DOTTED_ESPADDR", lparam=%lu, "S_TP_QUERY_STUB_MSG_FMT,
                      DOTTED_TP_SOURCE(*source),
                      DOTTED_ESPADDR(MakeESPADDR(source->node, kTPStubMgrStub)),
                      static_cast<uint64_t>(source->stub_msg_id - 1),
                      DOTTED_TP_QEURY_STUB_MSG(*m_pstub_msg));
    }
    ChainFreeHdl(m_phdl, NULL);
    m_phdl = NULL;
}

/**
* @brief    send data request to source
* @param    source source
* @author   konghui, konghui@baidu.com
* @date     2013-01-12
*/
void TPInflow::_PostMsgToSource(TPSource* s, int32_t size, int32_t batch) {
    VERIFY_OR_RETURN(LIKELY(true == m_running), BLANK);
    VERIFY_OR_RETURN(LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark), BLANK);

    m_phdl           = AllocateHdl(true);
    m_phdl->nDataLen = sizeof(TPDataMsg);
    m_pmsg           = new(m_phdl->pBuf) TPDataMsg();
    m_pmsg->from_id  = m_id;
    m_pmsg->to_id    = s->id;
    m_pmsg->size     = size;
    m_pmsg->batch    = batch;
    m_pmsg->ack      = s->ack;

    if (LIKELY(SOURCE_OK == s->status)) {
        m_pmsg->err  = TPN_ACK_OK;
    } else if (SOURCE_SYNC == s->status) {
        m_pmsg->err  = TPN_ACK_SYNC;
        s->counter.cnt_sync_num.Add(1);
    } else { // should not equal SOURCE_INVALID
        DSTREAM_FATAL("invalid post data msg with miss match status: "
                      S_TP_SOURCE_FMT": " S_TP_DATA_MSG_FMT,
                      DOTTED_TP_SOURCE(*s), DOTTED_TP_DATA_MSG(*m_pmsg));
    }

    int esp_ret = PostMsg(s->addr, TPE_IF_FETCH_SOURCE, s->data_msg_id++, m_phdl);
    UNLIKELY_IF(esp_ret != ESPE_OK) {
        DSTREAM_WARN("fail to post msg@"S_TP_SOURCE_FMT", err=%s: ",
                     DOTTED_TP_SOURCE(*s),
                     error::ErrStr(error::KYLIN_E_BEGIN + esp_ret));
    } else {
        DSTREAM_DEBUG("send fetch msg@source[%p status=%s, lparam=%lu]: "
                      S_TP_DATA_MSG_FMT, s, SourceStatusStr(s->status),
                      static_cast<uint64_t>(s->data_msg_id - 1),
                      DOTTED_TP_DATA_MSG(*m_pmsg));
    }
    ChainFreeHdl(m_phdl, NULL);
    m_phdl = NULL;
    s->last_send = GetUint64TimeMs();
}

ErrCode TPInflow::_ExtractStubMsg(ESPMessage* msg) {
    CBufReadStream brs(msg->req.pHdl, NULL);
    if (LIKELY(brs.GetObject<TPStubMsg>(&m_stub_msg))) {
        if (LIKELY((m_stub_msg.to_id == m_id))) {
            return error::OK;
        }
        DSTREAM_WARN("inflow miss match: this=%p, id=%lu, stub=%u: "
                     S_ESP_MSG_FMT": "S_TP_QUERY_STUB_MSG_FMT,
                     this, m_id, m_stub, DOTTED_ESP_MSG(*msg),
                     DOTTED_TP_QEURY_STUB_MSG(m_stub_msg));
        return error::DROP_MSG_DUE_ID_MISS_MATCH;
    }
    DSTREAM_WARN("fail to extract TPStubMsg head: this=%p, id=%lu "
                 S_ESP_MSG_FMT, this, m_id,
                 DOTTED_ESP_MSG(*msg));
    DSTREAM_FATAL("fail to extract TPStubMsg head: this=%p, id=%lu "
                  S_ESP_MSG_FMT, this, m_id,
                  DOTTED_ESP_MSG(*msg));
    return error::DATA_CORRUPTED;
}

void TPInflow::OnQueryStubAck(ESPMessage* msg) {
    VERIFY_OR_RETURN(true == m_running, BLANK);
    VERIFY_OR_RETURN(error::OK == _ExtractStubMsg(msg), BLANK);

    TPSourceMap::iterator it = m_sources.find(m_stub_msg.from_id);
    UNLIKELY_IF(it == m_sources.end()) {
        DSTREAM_WARN("%s: this=%p "S_TP_QUERY_STUB_MSG_FMT,
                     error::ErrStr(error::SOURCE_NOT_EXIST), this,
                     DOTTED_TP_QEURY_STUB_MSG(m_stub_msg));
        return;
    }
    TPSource* source = it->second;
    source->last_recv = GetUint64TimeMs();

    VERIFY_OR_RETURN(TPN_STUB_QUERY_OK == m_stub_msg.err, BLANK);

    if (LIKELY(SOURCE_INVALID == source->status)) {  // expect stub
        if (LIKELY(INVALID_ESPSTUB != m_stub_msg.stub)) {
            source->addr = MakeESPADDR(source->node, m_stub_msg.stub);
            source->status = SOURCE_SYNC;
            DSTREAM_INFO("source[%p id=%lu, owner=%lu addr="
                         S_DOTTED_ESPADDR"] status: INVALID => SYNC",
                         source, source->id, m_id,
                         DOTTED_ESPADDR(source->addr));
            _FetchSource(source);
            source->retry  = 0;
            return;
        } // else, drop due to invalid stub
        return;
    } // else, drop due to unmatch status
    DSTREAM_DEBUG("drop query ack due to status miss: "
                  S_TP_SOURCE_FMT": "S_TP_QUERY_STUB_MSG_FMT,
                  DOTTED_TP_SOURCE(*source),
                  DOTTED_TP_QEURY_STUB_MSG(m_stub_msg));
}

//-----------------------------------------------
// data interface
ErrCode TPInflow::_ExtractDataMsg(ESPMessage* msg, CBufReadStream* brs) {
    if (LIKELY(brs->GetObject<TPDataMsg>(&m_msg))) {
        if (LIKELY(m_msg.to_id == m_id)) {
            return error::OK;
        }
        return error::DROP_MSG_DUE_ID_MISS_MATCH;
    }
    DSTREAM_FATAL("corrupt msg: fail to extract Data");
    return error::DATA_CORRUPTED;
}

ErrCode TPInflow::_OnSourceRecv(TPSource* source,
                                int32_t   err,
                                uint64_t  ack,
                                CBufReadStream* brs,
                                int32_t   size,
                                int32_t   batch) {
    ErrCode ret = error::OK;
    if (LIKELY(SOURCE_OK == source->status)) {  // expect data
        if (LIKELY(TPN_ACK_OK == err)) {        // ack ok
            if (LIKELY(source->ack == ack)) { // expect ack
                ret = m_queue->PushBack(brs, size);
                if (LIKELY(error::OK <= ret)) { // ok, or soft watermark
                    source->ack++;
                    source->counter.cnt_in_num.Add(batch);
                    source->counter.cnt_in_size.Add(size);
                } else if (error::QUEUE_FULL == ret) { // pop aged data
                    int32_t drop_size  = size;
                    int32_t drop_batch = 0;
                    if (DST_WHEN_QUEUE_FULL_DROP_AGELONG == m_data_strategy &&
                        error::OK == (ret = _PopAgedTuples(&drop_size, &drop_batch)) &&
                        error::OK <= (ret = m_queue->PushBack(brs, size))) {
                        source->ack++;
                        source->counter.cnt_in_num.Add(batch);
                        source->counter.cnt_in_size.Add(size);
                        source->counter.cnt_drop_agelog_data_num.Add(drop_batch);
                        source->counter.cnt_drop_agelog_data_size.Add(drop_size);
                    }
                } // else, drop, wait pe refetch
                source->retry = 0;
                return ret;
            } else { // ack miss, drop
                source->counter.cnt_drop_ack_miss_num.Add(batch);
                source->counter.cnt_drop_ack_miss_size.Add(size);
                return error::DROP_MSG_DUE_ACK_MISS_MATCH;
            }
        } else { // other ack status, drop
            source->counter.cnt_drop_status_miss_num.Add(batch);
            source->counter.cnt_drop_status_miss_size.Add(size);
            return error::DROP_MSG_DUE_ACK_STATUS_MISS_MATCH;
        }
    } else if (SOURCE_SYNC == source->status) {
        if (LIKELY(TPN_ACK_SYNC == err)) { // sync ack
            if (LIKELY(source->ack == ack)) {
                source->retry  = 0;
                source->status = SOURCE_OK;
                DSTREAM_INFO("source[%p id=%lu, owner=%lu, ack=%lu] "
                             "status: SYNC => OK",
                             source, source->id, m_id, source->ack);
                return error::OK;
            } else { // ack miss
                source->counter.cnt_drop_ack_miss_num.Add(1);
                return error::DROP_MSG_DUE_ACK_MISS_MATCH;
            }
        } else { // other ack status, drop
            source->counter.cnt_drop_status_miss_num.Add(batch);
            source->counter.cnt_drop_status_miss_size.Add(size);
            return error::DROP_MSG_DUE_ACK_STATUS_MISS_MATCH;
        }
    } else { // SOURCE_INVALID == source->status
        source->counter.cnt_drop_status_miss_num.Add(batch);
        source->counter.cnt_drop_status_miss_size.Add(size);
        return error::DROP_MSG_DUE_INFLOW_STATUS_MISS_MATCH;
    }
}

void TPInflow::OnRecvData(ESPMessage* msg) {
    VERIFY_OR_RETURN(LIKELY(true == m_running), BLANK);

    CBufReadStream brs(msg->req.pHdl, NULL);
    int ret = _ExtractDataMsg(msg, &brs);
    UNLIKELY_IF(error::OK != ret) {
        DSTREAM_WARN("%s: "S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                     DOTTED_TP_DATA_MSG(m_msg));
        return;
    }

    DSTREAM_DEBUG("get msg: lparam=%lu, "S_TP_DATA_MSG_FMT,
                  static_cast<uint64_t>(msg->lParam), DOTTED_TP_DATA_MSG(m_msg));

    TPSourceMap::iterator it = m_sources.find(m_msg.from_id);
    if (LIKELY(it != m_sources.end())) {
        TPSource* source = it->second;
        source->last_recv = GetUint64TimeMs();
        if (LIKELY(!source->has_pending_recv)) {
            ret = _OnSourceRecv(source, m_msg.err, m_msg.ack,
                                &brs, m_msg.size, m_msg.batch);
            if (LIKELY(error::OK == ret)) {
                _FetchSource(source); // ack and fetch
            } else if (error::OK_SOFT_WATERMARK == ret) {
                DSTREAM_DEBUG("%s: "S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                              DOTTED_TP_DATA_MSG(m_msg));
                _AckSource(source);   // donot fetch, just ack
            } else if (error::QUEUE_FULL == ret) { // pending recv
                if (DST_WHEN_QUEUE_FULL_HOLD == m_data_strategy) {
                    UNLIKELY_IF(TPGetFreeTranBufPercent() < g_kKylinWriteWaterMark) {
                        DSTREAM_WARN("fail to pending msg due to %s: "
                                     S_TP_SOURCE_FMT" owner=%lu, queue=%p: "
                                     "lparam=%lu, "S_TP_DATA_MSG_FMT,
                                     error::ErrStr(error::ESP_TOUCH_WATERMARK),
                                     DOTTED_TP_SOURCE(*source),
                                     m_id, m_queue, static_cast<uint64_t>(msg->lParam),
                                     DOTTED_TP_DATA_MSG(m_msg));
                        source->counter.cnt_drop_tranbuf_full_num.Add(m_msg.batch);
                        source->counter.cnt_drop_tranbuf_full_size.Add(m_msg.size);
                        return;
                    }
                    PendingRecv& pr = source->pending_recv;
                    pr.hdl   = CloneHdl(msg->req.pHdl, NULL);
                    pr.ack   = m_msg.ack;
                    pr.size  = m_msg.size;
                    pr.batch = m_msg.batch;
                    m_pending_recv_num++;
                    source->has_pending_recv = true;
                    source->counter.cnt_pending_recv_num.Add(m_msg.batch);
                    source->counter.cnt_pending_recv_size.Add(m_msg.size);
                    DSTREAM_DEBUG("pending recv due to queue full: "
                                  S_TP_SOURCE_FMT" owner=%lu, queue=%p: "
                                  "lparam=%lu, "S_TP_DATA_MSG_FMT" => "
                                  S_TP_IFPR_FMT,
                                  DOTTED_TP_SOURCE(*source),
                                  m_id, m_queue, static_cast<uint64_t>(msg->lParam),
                                  DOTTED_TP_DATA_MSG(m_msg),
                                  DOTTED_TP_IFPR(pr));
                } else {
                    DSTREAM_FATAL("!!!should not happen: %s: this=%p, "
                                  "lparam=%lu, "S_TP_DATA_MSG_FMT,
                                  error::ErrStr(ret), this,
                                  static_cast<uint64_t>(msg->lParam),
                                  DOTTED_TP_DATA_MSG(m_msg));
                }
                return;
            } else { // donot postmsg
                DSTREAM_WARN("%s: "S_TP_SOURCE_FMT" owner=%lu, queue=[%p, %s]:"
                             " lparam=%lu, "S_TP_DATA_MSG_FMT,
                             error::ErrStr(ret), DOTTED_TP_SOURCE(*source),
                             m_id, m_queue, m_queue->DumpStatus().c_str(),
                             static_cast<uint64_t>(msg->lParam), DOTTED_TP_DATA_MSG(m_msg));
                return;
            }
            // trigger pending fetch
            UNLIKELY_IF(m_has_pending_fetch) { // trigger pending recv
                _OnTriggerPendingFetch();
            }
            return;
        } // else: unordered msg, drop
        DSTREAM_DEBUG("drop msg due to has pending recv: source=[%p ack=%lu]: "
                      "lparam=%lu, "S_TP_DATA_MSG_FMT", "S_TP_IFPR_FMT,
                      source, source->ack, static_cast<uint64_t>(msg->lParam),
                      DOTTED_TP_DATA_MSG(m_msg),
                      DOTTED_TP_IFPR(source->pending_recv));
    } else {
        // unexpect msg due to:
        //  1. this source has been deleted;
        //  2. this msg is out of order;
        DSTREAM_WARN("%s: this=%p, id=%lu: "S_TP_DATA_MSG_FMT,
                     error::ErrStr(error::SOURCE_NOT_EXIST),
                     this, m_id, DOTTED_TP_DATA_MSG(m_msg));
        return;
    }
}

ErrCode TPInflow::_FetchData(IN    uint64_t ack,
                             INOUT int32_t* size,
                             INOUT int32_t* batch,
                             OUT   CBufWriteStream* bws) {
    ErrCode ret = error::OK;
    if (LIKELY(ack == m_msg_ack + 1)) { // OK
        ret = m_queue->PopFront(m_queue_ack);
        if (LIKELY(error::OK <= ret)) {
            m_queue_ack = kAdvanceQueueAck; // advance ack
            ret = m_queue->Front(bws, size, batch, &m_queue_ack);
            if (LIKELY(error::OK == ret)) {
                m_msg_ack++;
                m_counter.cnt_out_num.Add(static_cast<uint64_t>(*batch));
                m_counter.cnt_out_size.Add(static_cast<uint64_t>(*size));
            }
        }
        return ret;
    } else if (ack == m_msg_ack) { // OK, refetch last data
        ret = m_queue->Front(bws, size, batch, &m_queue_ack);
        if (LIKELY(error::OK == ret)) {
            m_counter.cnt_out_replay_num.Add(static_cast<uint64_t>(*batch));
            m_counter.cnt_out_replay_size.Add(static_cast<uint64_t>(*size));
        }
        return ret;
    }
    m_counter.cnt_drop_ack_miss_num.Add(1);
    return error::DROP_MSG_DUE_ACK_MISS_MATCH;
}

ErrCode TPInflow::_OnFetchData(IN ESPADDRESS addr,
                               IN uint64     lparam,
                               IN int32_t    err,
                               IN uint64_t   ack,
                               IN int32_t    size,
                               IN int32_t    batch) {
    UNLIKELY_IF(m_queue->Empty()) {
        return error::QUEUE_EMPTY;
    }
    _DS_TIMER_PREFIX();
    _DS_TIMER_BEGIN();
    UNLIKELY_IF(TPGetFreeTranBufPercent() < g_kKylinWriteWaterMark) {
        _DS_TIMER_END(10);
        return error::ESP_TOUCH_WATERMARK;
    }
    _DS_TIMER_END(10);

    CBufWriteStream bws;
    UNLIKELY_IF(!bws.PutObject<TPDataMsg>(m_msg)) { // padding msg head
        DSTREAM_WARN("fail to serial data to buf stream: "S_TP_DATA_MSG_FMT,
                     DOTTED_TP_DATA_MSG(m_msg));
        return error::SERIAIL_DATA_FAIL;
    }

    ErrCode ret = _FetchData(ack, &size, &batch, &bws);
    UNLIKELY_IF(ret < error::OK) {
        DSTREAM_WARN("inflow[%p id=%lu] fail to read queue[%p]: %s: "
                     "lparam=%lu, fromId=%lu, toId=%lu, err=%d, "
                     "size=%d, batch=%d, ack=%lu, ack_=%lu]", this,
                     m_id, m_queue, error::ErrStr(ret), static_cast<uint64_t>(lparam),
                     m_id, m_id, err, size, batch, ack, m_msg_ack);
        goto ERR_RET;
    }

ERR_RET:
    // ack msg
    m_phdl          = bws.GetBegin();
    m_pmsg          = new(m_phdl->pBuf) TPDataMsg();
    m_pmsg->from_id = m_id;
    m_pmsg->to_id   = m_id;
    m_pmsg->err     = ret;
    m_pmsg->ack     = m_msg_ack;
    m_pmsg->size    = size;
    m_pmsg->batch   = batch;
    m_pmsg->chksum  = 0xFFFEEEEEEEEEEEEE; // CheckSum(hdl); // TODO
    int esp_ret = PostMsg(addr, TPE_IF_LOCALPE_FETCH_ACK, lparam, m_phdl);
    UNLIKELY_IF(ESPE_OK != esp_ret) {
        ret = (ErrCode)(error::KYLIN_E_BEGIN + esp_ret);
        DSTREAM_WARN("inflow[%p id=%lu] fail to post msg: %s: "
                     S_TP_DATA_MSG_FMT, this, m_id,
                     error::ErrStr(ret),
                     DOTTED_TP_DATA_MSG(*m_pmsg));
    }
    ChainFreeHdl(m_phdl, NULL);
    m_phdl = NULL;
    return ret;
}

void TPInflow::OnFetchData(ESPMessage* msg) {
    VERIFY_OR_RETURN(true == m_running, BLANK);
    CBufReadStream  brs(msg->req.pHdl, NULL);
    ErrCode ret = _ExtractDataMsg(msg, &brs);
    UNLIKELY_IF(error::OK != ret) {
        DSTREAM_WARN("%s: "S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                     DOTTED_TP_DATA_MSG(m_msg));
        return;
    }

    DSTREAM_DEBUG("get msg: lparam=%lu "S_TP_DATA_MSG_FMT,
                  static_cast<uint64_t>(msg->lParam), DOTTED_TP_DATA_MSG(m_msg));

    ret = _OnFetchData(msg->aFrom.addr, msg->lParam, m_msg.err,
                       m_msg.ack, m_msg.size, m_msg.batch);
    UNLIKELY_IF(error::QUEUE_EMPTY == ret ||
                error::ESP_TOUCH_WATERMARK == ret) { // pending
        DSTREAM_DEBUG("pending fetch due to %s: inflow"
                      "[%p id=%lu, queue=%p]: lparam=%lu: "
                      S_TP_DATA_MSG_FMT, error::ErrStr(ret),
                      this, m_id, m_queue, static_cast<uint64_t>(msg->lParam),
                      DOTTED_TP_DATA_MSG(m_msg));
        m_has_pending_fetch       = 1;
        m_pending_fetch.msg       = m_msg;
        m_pending_fetch.recv_time = GetUint64TimeMs();
        m_pending_fetch.lparam    = msg->lParam;
        m_pending_fetch.afrom     = msg->aFrom.addr;
        m_counter.cnt_pending_fetch_num.Add(1);
        // trigger pending recv
        _OnTriggerPendingRecv();
    } else { // OK or fail, clear pending fetch
        if (0 != m_has_pending_fetch) {
            DSTREAM_DEBUG("del pending fetch due to: %s: this=%p, "
                          S_TP_IFPF_FMT, error::ErrStr(ret), this,
                          DOTTED_TP_IFPF(m_pending_fetch));
        }
        m_has_pending_fetch = 0;
        if (LIKELY(error::OK == ret)) { // trigger pending recv
            _OnTriggerPendingRecv();
        }
    }
    return;
}

ErrCode TPInflow::_PopAgedTuples(int32_t* size, int32_t* batch) {
    CBufWriteStream droped_tuples;

    m_queue_ack = kAdvanceQueueAck; // reset queue ack
    ErrCode ret = m_queue->PopAgedTuples(size,
                                         batch,
                                         &droped_tuples);
    UNLIKELY_IF(error::OK != ret) {
        return ret;
    }
    m_counter.cnt_drop_agelog_data_num.Add(*batch);
    m_counter.cnt_drop_agelog_data_size.Add(*size);
    std::string s;
    BufHandle* hdl = droped_tuples.GetBegin();
    DumpTuplesFromBufHdl(hdl, *size, *batch, &s);
    ChainFreeHdl(hdl, NULL);
    hdl = NULL;
    DSTREAM_WARN("these tupels in inflow[%p id=%lu capacity=%lu] "
                 "may lost due to queue full: %s", this, m_id,
                 m_queue_size, s.c_str());
    return ret;
}

bool TPInflow::TriggerPendingRecv(TPSource* source) {
    if (error::OK == m_queue->IsFull()) {
        if (source->has_pending_recv) {
            PendingRecv& pr = source->pending_recv;
            CBufReadStream brs(pr.hdl, NULL);
            brs.Seek(sizeof(TPDataMsg));
            ErrCode ret = _OnSourceRecv(source, TPN_ACK_OK, pr.ack,
                                        &brs, pr.size, pr.batch);
            if (LIKELY(error::OK == ret)) {
                _FetchSource(source); // ack and fetch
            } else if (error::OK_SOFT_WATERMARK == ret) {
                _AckSource(source);   // donot fetch, just ack
            } else {
                DSTREAM_WARN("del pending recv due to:%s : "
                             "source[%p id=%lu, owner=%lu, status=%s, ack=%lu]: "
                             S_TP_IFPR_FMT, error::ErrStr(ret), source,
                             source->id, m_id, SourceStatusStr(source->status),
                             source->ack, DOTTED_TP_IFPR(pr));
            }
            // clear pendding
            DSTREAM_DEBUG("del pending recv due to: OK : "
                          "source[%p id=%lu, owner=%lu, status=%s, ack=%lu]: "
                          S_TP_IFPR_FMT, source, source->id, m_id,
                          SourceStatusStr(source->status), source->ack,
                          DOTTED_TP_IFPR(pr));
            pr.Reset();
            m_pending_recv_num--;
            source->has_pending_recv = false;
            return true;
        }
        return true;
    }
    return false;
}

void TPInflow::_OnTriggerPendingRecv() {
    // deal all pending recv
    BOOST_FOREACH(TPSourceMap::value_type & pair, m_sources) {
        if (!TriggerPendingRecv(pair.second)) {
            break;
        }
    }
}

void TPInflow::_OnTriggerPendingFetch() {
    // if (GetUint64TimeMs() - m_pending_fetch.recv_time >
    //     config_cosnt::kDataFetchTimeoutMs) {
    //     m_has_pending_fetch = 0;
    //     DSTREAM_DEBUG("del pending fetch due to timeout: inflow"
    //                   "[%p id=%lu]: [%p]: lparam=%lu: "S_TP_IFPF_FMT,
    //                   this, m_id, m_queue, m_pending_fetch.lparam,
    //                   DOTTED_TP_IFPF(m_pending_fetch));
    //     return;
    // }
    ErrCode ret = _OnFetchData(m_pending_fetch.afrom,
                               m_pending_fetch.lparam,
                               m_pending_fetch.msg.err,
                               m_pending_fetch.msg.ack,
                               m_pending_fetch.msg.size,
                               m_pending_fetch.msg.batch);
    if (LIKELY(error::QUEUE_EMPTY != ret &&
               error::ESP_TOUCH_WATERMARK != ret)) {
        m_has_pending_fetch = 0;
        DSTREAM_DEBUG("del pending fetch due to %s: inflow"
                      "[%p id=%lu]: [%p]: lparam=%lu: "
                      S_TP_IFPF_FMT, error::ErrStr(ret),
                      this, m_id, m_queue, m_pending_fetch.lparam,
                      DOTTED_TP_IFPF(m_pending_fetch));
    }
    return;
}

void TPInflow::OnCompletion(AsyncContext* ctx) {
    if (LIKELY(ctx->nAction < AA_USER_BEGIN)) {
        CESPObject::OnCompletion(ctx);
        return;
    }

    switch (ctx->nAction) {
    case TPA_IF_STOP:
        return _OnStop(ctx);
    case TPA_IF_CHECK_SOURCE:
        return _OnCheckSource(ctx);
    default:
        DSTREAM_WARN("unkown AsyncContext=[%p nErrCode=%d, nActon=%d, "
                     "pClient=%p, fProc = %p]", ctx, ctx->nErrCode,
                     ctx->nAction, ctx->pClient, ctx->fProc);
        return;
    }
}

} // namespace transporter
} // namespace dstream
