#include "transporter/transporter_cli.h"
#include <string>
#include "common/id.h"
#include "common/utils.h"

namespace dstream {
namespace transporter {

TPCli::TPCli() : m_running(TC_NOT_RUNNING),
    m_node(INVALID_ESPNODE),
    m_addr(INVALID_ESPADDRESS) {
}

int TPCli::Init(ESPADDRESS server_addr, ESPSTUB local_stub) {
    AtomicSetValue(m_running, TC_RUNNING);
    int ret = CESPClient::Init(server_addr, local_stub);
    return ESPE_OK == ret ? error::OK : error::KYLIN_E_BEGIN + ret;
}

void TPCli::Destroy() {
    AtomicSetValue(m_running, TC_NOT_RUNNING);
    CESPClient::Destroy();
}

bool TPCli::OnMsg(ESPMessage* msg) {
    Request* req = _RemoveOutSet(msg->lParam);
    if ((NULL != req) && (TC_RUNNING == AtomicGetValue(m_running))) {
        req->cb(this, 0, msg->req.pHdl, req);
    } else if (TC_RUNNING == AtomicGetValue(m_running)) {
        DSTREAM_WARN("Warning: unknown succ ack[%p], discarded.\n", req);
    }
    return true;
}

TPProxyCli::TPProxyCli(TPType type, uint64_t id, uint64_t to_id)
    : m_type(type),
      m_id(id),
      m_to_id(to_id),
      m_to_stub(INVALID_ESPSTUB),
      m_cli(NULL),
      m_timeout(config_const::kDataFetchTimeoutMs) {
}

TPProxyCli::~TPProxyCli() {
    if (NULL != m_cli) {
        m_cli->Destroy();
        m_cli = NULL;
    }
}

int TPProxyCli::Init(const std::string& to_ipport, int timeout_ms) {
    m_timeout   = timeout_ms;
    m_to_ipport = to_ipport;
    return _QueryStub();
}

int TPProxyCli::SyncReq(uint32 nmsg, IN BufHandle* arg, OUT BufHandle** ack_hdl) {
    VERIFY_OR_RETURN(NULL != ack_hdl, error::BAD_ARGUMENT);

    int ret = error::OK;
    UNLIKELY_IF(INVALID_ESPSTUB == m_to_stub) {
        ret = _QueryStub();
        VERIFY_OR_RETURN(ret == error::OK, ret);
    }

    UNLIKELY_IF(NULL == m_cli) {
        m_cli = new (std::nothrow) TPCli();
        VERIFY_OR_RETURN(NULL != m_cli, error::BAD_MEM_ALLOC);
        ret = m_cli->Init(MakeESPADDR(m_to_node, m_to_stub));
        if (ret < error::OK) {
            goto ERR_RET;
        }
    }

    if (NULL != arg) {
        TPDataMsg* msg = reinterpret_cast<TPDataMsg*>(arg->pBuf);
        msg->from_id = m_id;
        msg->to_id   = m_to_id;
        DSTREAM_DEBUG("Send Msg:"S_TP_DATA_MSG_FMT, DOTTED_TP_DATA_MSG(*msg));
    }


    int esp_ret = m_cli->SyncReq(nmsg, arg, ack_hdl, m_timeout);
    if ((ESPE_OK == esp_ret) && (NULL != *ack_hdl)) {
        ret = error::OK;
    } else {
        ret = (ESPE_OK == esp_ret ? error::DATA_IS_NULL :
               error::KYLIN_E_BEGIN + esp_ret);
        // goto ERR_RET;
    }
    return ret;

ERR_RET:
    m_to_stub = INVALID_ESPSTUB;
    if (NULL != m_cli)     {
        m_cli->Destroy();
        m_cli = NULL;
    }
    return ret;
}

int TPProxyCli::_QueryStub() {
    int esp_ret = ESPE_OK;
    int ret  = error::OK;

    int nmsg = (m_type == TPT_INFLOW ? TPE_SM_QUERY_IF : TPE_SM_QUERY_OF);
    TPStubMsg stub_msg;
    stub_msg.from_id = m_id;
    stub_msg.to_id   = m_to_id;

    CBufWriteStream bws;
    BufHandle* hdl     = NULL;
    BufHandle* ack_hdl = NULL;

    TPCli* stub_cli = new (std::nothrow) TPCli();
    VERIFY_OR_RETURN(NULL != stub_cli, error::BAD_MEM_ALLOC);

    if ((m_to_node = ESPFindNode(m_to_ipport.c_str())) == INVALID_ESPNODE ||
        (ret = stub_cli->Init(MakeESPADDR(m_to_node, kTPStubMgrStub))) != error::OK) {
        ret = (error::OK == ret ? error::COMM_CONNECT_FAIL : ret);
        goto ERR_RET;
    }

    if (bws.PutObject<TPStubMsg>(stub_msg) != sizeof(stub_msg)) {
        ret = error::PUT_DATA_INTO_BUF_FAIL;
        goto ERR_RET;
    }

    hdl = bws.GetBegin();
    esp_ret = stub_cli->SyncReq(nmsg, hdl, &ack_hdl, m_timeout);
    if ((ESPE_OK == esp_ret) && (NULL != ack_hdl)) {
        CBufReadStream brs(ack_hdl, NULL);
        if (brs.GetObject<TPStubMsg>(&stub_msg)) {
            m_to_stub = stub_msg.stub;
            ret = (INVALID_ESPSTUB == m_to_stub ? error::NO_STUB_FOUND : error::OK);
            DSTREAM_DEBUG("pe[%lu] get pe[%lu]'s %s stub[%u]", m_id, m_to_id,
                          m_type == TPT_INFLOW ? "inflow" : "outflow", m_to_stub);
        } else {
            ret = error::DATA_CORRUPTED;
            DSTREAM_WARN("%s:"S_TP_QUERY_STUB_MSG_FMT, error::ErrStr(ret),
                         DOTTED_TP_QEURY_STUB_MSG(stub_msg));
        }
    } else {
        ret = (ESPE_OK == esp_ret ? error::DATA_IS_NULL :
               error::KYLIN_E_BEGIN + esp_ret);
        DSTREAM_WARN("fail to query stub:%s", error::ErrStr(ret));
    }

ERR_RET:
    if (NULL != stub_cli) {
        stub_cli->Destroy();
        stub_cli = NULL;
    }
    if (NULL != hdl)      {
        ChainFreeHdl(hdl, NULL);
        hdl = NULL;
    }
    if (NULL != ack_hdl)  {
        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
    }
    return ret;
}

} // namespace transporter
} // namespace dstream
