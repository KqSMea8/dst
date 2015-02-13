#include "transporter/transporter_stubmgr.h"
#include <boost/foreach.hpp>
#include "common/utils.h"

namespace dstream {
namespace transporter {

ESP_BEGIN_MAP_MESSAGE(TPStubMgr, CESPObject)
ESP_MAP_MESSAGE_RAW(TPE_SM_QUERY_IF, OnQueryInflowStub)
ESP_MAP_MESSAGE_RAW(TPE_SM_QUERY_OF, OnQueryOutflowStub)
ESP_END_MAP_MESSAGE

IMPLEMENT_SINGLETON_WITHOUT_CTOR(TPStubMgr)

TPStubMgr::TPStubMgr(): phdl_(NULL), m_running(SM_NOT_RUNNING), m_stub(kTPStubMgrStub) {}
TPStubMgr::~TPStubMgr() {
    _OnStop();
}

ErrCode TPStubMgr::Start() {
    VERIFY_OR_RETURN(AtomicGetValue(m_running) != SM_RUNNING, error::OK);

    AtomicSetValue(m_stub, kTPStubMgrStub);
    AtomicInc64(m_stub);
    m_stub_set.insert(kTPStubMgrStub);
    if (ESPE_OK != BindStub(kTPStubMgrStub, ESP_ANYNODE)) {
        return error::BIND_STUB_FAIL;
    }
    AtomicSetValue(m_running, SM_RUNNING);
    return error::OK;
}

void TPStubMgr::Stop() {
    AtomicSetValue(m_running, SM_NOT_RUNNING);
    AsyncContext* ctx = new AsyncContext(); // ²»Ê¹ÓÃnothrow
    QueueExec(error::OK, SM_STOP, this, ctx, false);
}

void TPStubMgr::_OnStop() {
    UnbindStub();
}

//-----------------------------------------------
// stub alloc/free
ESPSTUB TPStubMgr::AllocIfStub(uint64_t id) {
    return _AllocStub(id, &m_if_stub_map, "If");
}

ESPSTUB TPStubMgr::AllocOfStub(uint64_t id) {
    return _AllocStub(id, &m_of_stub_map, "Of");
}

ESPSTUB TPStubMgr::_AllocStub(uint64_t id,
                              StubMap* stub_map,
                              const char* name) {
    VERIFY_OR_RETURN(AtomicGetValue(m_running) == SM_RUNNING, INVALID_ESPSTUB);

    DS_SCOPE_LOCK_BEGIN(m_mutex);
    StubMap::const_iterator it = stub_map->find(id);
    UNLIKELY_IF(it != stub_map->end()) {
        return INVALID_ESPSTUB;
    }
    DS_SCOPE_LOCK_END();

    uint64_t retry = 0;
    ESPSTUB stub = INVALID_ESPSTUB;
    do {
        stub = _ConvertStub(AtomicInc64(m_stub));
    } while (!_TestFreeAndInsertStub(stub) && retry < kMaxRetry && ++retry);

    VERIFY_OR_RETURN(retry < kMaxRetry, INVALID_ESPSTUB);

    DS_SCOPE_LOCK_BEGIN(m_mutex);
    (*stub_map)[id] = stub;
    DS_SCOPE_LOCK_END();
    DSTREAM_INFO("assign %s_stub=%u to %lu", name, stub, id);
    return stub;
}

void TPStubMgr::FreeIfStub(uint64_t id) {
    _FreeStub(id, &m_if_stub_map, "If");
}
void TPStubMgr::FreeOfStub(uint64_t id) {
    _FreeStub(id, &m_of_stub_map, "Of");
}
void TPStubMgr::_FreeStub(uint64_t id, StubMap* stub_map, const char* name) {
    DS_SCOPE_LOCK_BEGIN(m_mutex);
    StubMap::const_iterator it = stub_map->find(id);
    if (it != stub_map->end()) {
        DSTREAM_INFO("free %s_stub=%u of %lu", name, it->second, id);
        m_stub_set.erase(it->second);
        stub_map->erase(it);
    }
    DS_SCOPE_LOCK_END();
}

//-----------------------------------------------
// stub query
void TPStubMgr::_OnQueryStub(ESPMessage* msg,
                             const StubMap& stub_map,
                             TPEspMsgAction action) {
    VERIFY_OR_RETURN(AtomicGetValue(m_running) == SM_RUNNING, BLANK);
    VERIFY_OR_RETURN(LIKELY(TPGetFreeTranBufPercent() > g_kKylinWriteWaterMark), BLANK);

    phdl_ = AllocateHdl(true);
    phdl_->nDataLen = sizeof(TPStubMsg);
    pmsg_ = reinterpret_cast<TPStubMsg*>(phdl_->pBuf);
    CBufReadStream brs(msg->req.pHdl, NULL);
    UNLIKELY_IF(!brs.GetObject<TPStubMsg>(pmsg_)) {
        DSTREAM_WARN("fail to extract TPStubMsg head:"S_ESP_MSG_FMT,
                     DOTTED_ESP_MSG(*msg));
        ChainFreeHdl(phdl_, NULL);
        phdl_ = NULL;
        return;
    }
    DSTREAM_DEBUG("get stub msg: "S_ESP_MSG_FMT": "S_TP_QUERY_STUB_MSG_FMT,
                  DOTTED_ESP_MSG(*msg), DOTTED_TP_QEURY_STUB_MSG(*pmsg_));

    uint64_t to_id = pmsg_->to_id;
    pmsg_->to_id   = pmsg_->from_id;
    pmsg_->from_id = to_id;

    DS_SCOPE_LOCK_BEGIN(m_mutex);
    StubMap::const_iterator it = stub_map.find(to_id);
    if (it != stub_map.end()) {
        pmsg_->stub = it->second;
        pmsg_->err  = TPN_STUB_QUERY_OK;
    } else {
        pmsg_->stub = INVALID_ESPSTUB;
        pmsg_->err  = TPN_STUB_QUERY_NO_STUB;
    }
    DS_SCOPE_LOCK_END();

    int esp_ret = PostMsg(msg->aFrom.addr, action, msg->lParam, phdl_);
    UNLIKELY_IF(ESPE_OK != esp_ret) {
        DSTREAM_WARN("fail to ack stub msg: err=%s: addr="S_DOTTED_ESPADDR
                     ", lparam=%lu: "S_TP_QUERY_STUB_MSG_FMT,
                     error::ErrStr(error::KYLIN_E_BEGIN + esp_ret),
                     DOTTED_ESPADDR(msg->aFrom.addr),
                     static_cast<uint64_t>(msg->lParam),
                     DOTTED_TP_QEURY_STUB_MSG(*pmsg_));
    } else {
        DSTREAM_DEBUG("ack stub query msg: OK: addr="S_DOTTED_ESPADDR
                      ", lparam=%lu: "S_TP_QUERY_STUB_MSG_FMT,
                      DOTTED_ESPADDR(msg->aFrom.addr),
                      static_cast<uint64_t>(msg->lParam),
                      DOTTED_TP_QEURY_STUB_MSG(*pmsg_));
    }

    ChainFreeHdl(phdl_, NULL);
    phdl_ = NULL;
}

bool TPStubMgr::_TestFreeAndInsertStub(ESPSTUB stub)  {
    DS_SCOPE_LOCK_BEGIN(m_mutex);
    if (m_stub_set.find(stub) == m_stub_set.end()) {
        return m_stub_set.insert(stub).second;
    }
    return false;
    DS_SCOPE_LOCK_END();
}

void TPStubMgr::OnCompletion(AsyncContext* ctx) {
    if (SM_STOP == ctx->nAction) {
        return _OnStop();
    }

    if (ctx->nAction < AA_USER_BEGIN) {
        CESPObject::OnCompletion(ctx);
    }
}

} // namespace transporter
} // namespace dstream
