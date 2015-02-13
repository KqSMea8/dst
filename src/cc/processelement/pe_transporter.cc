/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


#include "processelement/pe_transporter.h"

namespace dstream {
namespace processelement {

PEFetchCli::PEFetchCli(uint64_t id)
    : transporter::TPProxyCli(transporter::TPT_INFLOW, id, id) {}

int PEFetchCli::FetchData(BufHandle** data_hdl,
                          int*        batch,
                          int*        size,
                          uint64_t*   ack) {
    m_hdl           = AllocateHdl(true);
    m_hdl->nDataLen = sizeof(transporter::TPDataMsg);
    m_pmsg          = new(m_hdl->pBuf) transporter::TPDataMsg();
    m_pmsg->err     = transporter::TPN_ACK_OK;
    m_pmsg->batch   = *batch;
    m_pmsg->size    = *size;
    m_pmsg->ack     = *ack;

    BufHandle* ack_hdl = NULL;
    int ret = SyncReq(transporter::TPE_IF_LOCALPE_FETCH, m_hdl, &ack_hdl);
    ChainFreeHdl(m_hdl, NULL);
    m_hdl = NULL;
    UNLIKELY_IF(ret < error::OK || NULL == ack_hdl) {
        return ret < error::OK ? ret : error::FAILED_EXPECTATION;
    }

    CBufReadStream brs(ack_hdl, NULL);
    UNLIKELY_IF(!brs.GetObject<transporter::TPDataMsg>(&m_msg)) {
        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
        return error::DATA_CORRUPTED;
    }

    // DSTREAM_DEBUG("%s get msg :"S_TP_DATA_MSG_FMT,
    //                __FUNCTION__, DOTTED_TP_DATA_MSG(m_msg));

    ret = m_msg.err;
    if (LIKELY(transporter::TPN_ACK_OK == m_msg.err)) {
        if (LIKELY(*ack == m_msg.ack)) {
            *batch    = m_msg.batch;
            *size     = m_msg.size;
            *data_hdl = ack_hdl;
            return error::OK;
        } else {
            ret = error::DROP_MSG_DUE_ACK_MISS_MATCH;
        }
    }
    *ack = m_msg.ack;
    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
    return ret;
}

PEForwardCli::PEForwardCli(uint64_t id)
    : transporter::TPProxyCli(transporter::TPT_OUTFLOW, id, id) {}

int PEForwardCli::SendData(BufHandle* data_hdl,
                           int        batch,
                           int        size,
                           uint64_t*  ack) {
    m_hdl           = AllocateHdl(true);
    m_hdl->nDataLen = sizeof(transporter::TPDataMsg);
    m_hdl->_next    = data_hdl;
    m_pmsg          = new(m_hdl->pBuf) transporter::TPDataMsg();
    m_pmsg->err     = transporter::TPN_ACK_OK;
    m_pmsg->batch   = batch;
    m_pmsg->size    = size;
    m_pmsg->ack     = *ack;

    BufHandle* ack_hdl = NULL;
    int ret = SyncReq(transporter::TPE_OF_RECV_LOCALPE, m_hdl, &ack_hdl);
    m_hdl->_next = NULL;
    ChainFreeHdl(m_hdl, NULL);
    m_hdl = NULL;
    UNLIKELY_IF(ret < error::OK || NULL == ack_hdl) {
        return ret < error::OK ? ret : error::FAILED_EXPECTATION;
    }

    CBufReadStream brs(ack_hdl, NULL);
    if (LIKELY(brs.GetObject<transporter::TPDataMsg>(&m_msg))) {
        // DSTREAM_DEBUG("%s get msg :"S_TP_DATA_MSG_FMT,
        //               __FUNCTION__, DOTTED_TP_DATA_MSG(m_msg));
        *ack = m_msg.ack;
        if (LIKELY(transporter::TPN_ACK_OK == m_msg.err)) {
            ret = error::OK;
        } else {
            ret = m_msg.err;
        }
    } else {
        ret = error::DATA_CORRUPTED;
    }
    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
    return ret;
}

} // namespace processelement
} // namespace dstream
