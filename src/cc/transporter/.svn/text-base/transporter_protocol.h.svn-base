/**
* @file transportor_protocol.h
* @brief
* @author Kong Hui
* @version 1.0.0
* @date 2012-12-21
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*/

#ifndef __DSTREAM_SRC_CC_TRANSPORTER_PROTOCAL_H__
#define __DSTREAM_SRC_CC_TRANSPORTER_PROTOCAL_H__

#include <string>
#include <vector>
#include "ESP.h"
#include "Kylin.h"

namespace dstream {
namespace transporter {

//-----------------------------------------------
// tags
// typedef std::string Tag;

//-----------------------------------------------
enum TPType {
    TPT_INFLOW,
    TPT_OUTFLOW
};
// nMsg/Action code
enum TPEspMsgAction { // [0, 2^16)
    TPE_BEGIN                = 0,
    // inflow
    TPE_IF_FETCH_SOURCE      = TPE_BEGIN + 0, // to fetch data from upstream source
    TPE_IF_FETCH_SOURCE_ACK  = TPE_BEGIN + 1,
    TPE_IF_LOCALPE_FETCH     = TPE_BEGIN + 2, // receiv fetch request from local pe
    TPE_IF_LOCALPE_FETCH_ACK = TPE_BEGIN + 3,

    // outflow
    TPE_OF_SUB_FETCH         = TPE_IF_FETCH_SOURCE, // receiv fetch request from downstream pe
    TPE_OF_SUB_FETCH_ACK     = TPE_IF_FETCH_SOURCE_ACK,
    TPE_OF_RECV_LOCALPE      = TPE_BEGIN + 102, // receiv data from local pe
    TPE_OF_RECV_LOCALPE_ACK  = TPE_BEGIN + 103,

    // stub query
    TPE_SM_QUERY_IF          = TPE_BEGIN + 200, // query localpe's inflow stub
    TPE_SM_QUERY_IF_ACK      = TPE_BEGIN + 201,
    TPE_SM_QUERY_OF          = TPE_BEGIN + 202, // query localpe's outflow stub
    TPE_SM_QUERY_OF_ACK      = TPE_BEGIN + 203,

    TPE_END                  = TPE_BEGIN + 5000,
};

enum TPAction { // positive
    TPA_BEGIN           = AA_USER_BEGIN,   // skip esp msg
    TPA_IF_SOURCE_ADD   = TPA_BEGIN + 100,
    TPA_IF_SOURCE_DEL   = TPA_BEGIN + 101,
    TPA_IF_SOURCE_UPD   = TPA_BEGIN + 102,

    TPA_OF_ADD          = TPA_BEGIN + 200,
    TPA_OF_DEL          = TPA_BEGIN + 201,
    TPA_OF_UPD          = TPA_BEGIN + 202,

    TPA_CNT_DUMP        = TPA_BEGIN + 300,

    TPA_USER_BEGIN      = AA_USER_BEGIN + 10000
};

//  >= 0: ÕýÈ·
//  < 0,  ErrCode
enum TPMsgErr {
    TPN_ACK_OK             = 0,
    TPN_ACK_SYNC           = 1,
    TPN_STUB_QUERY_OK      = 0,
    TPN_STUB_QUERY_NO_STUB = 1
};


//-----------------------------------------------
// esp message

const uint8_t  kTPMsgVerion   = 1;
const uint64_t kTPStubMgrStub = 1;
const int32_t  kMaxMsgSize    = 5 * 1024 * 1024; /* max signle msg len */
const int32_t  kMaxMsgBatch   = 1024;             /* max signle msg len, not work yet */
const int32_t  kMaxTagSize    = 1024;             /* max tag size*/
const int32_t  kMinBatchSize  = 1 * 1024 * 1024;  /* min batch size*/

struct TPMsgHead {
    uint8_t         ver;     // version
    uint64_t        from_id; // downstream peid
    uint64_t        to_id;   // upstream peid
    int32_t         err;     // err code
    uint64_t        chksum;  // check sum
    TPMsgHead(): ver(kTPMsgVerion), err(TPN_ACK_OK) {}
};

typedef struct TPStubMsg: TPMsgHead {
    ESPSTUB         stub;
};

typedef struct TPDataMsg: TPMsgHead {
    int32_t         batch;   // batch num
    int32_t         size;    // batch size
    uint64_t        ack;     // ack
    char            data[0]; // data, len | data | len | data, or get tuple.size()
};

const uint64_t kInvalidAck      = 0xFFFFFFFFFFFFFFFF; // invalid ack
const uint64_t kFlushQueueUnack = 0xFFFFFFFFFFFFFFFF; // flush unacked data
const uint64_t kAdvanceQueueAck = 0xFFFFFFFFFFFFFFFE; // advance

#define DSTREAM_DATA_WARN DSTREAM_WARN
#define S_ESP_MSG_FMT \
    "EspMsg[%p]=[aFrom="S_DOTTED_ESPADDR", aTo="S_DOTTED_ESPADDR \
    ", nMsg=%u, lParam=%llu, nLen=%d]"
#define DOTTED_ESP_MSG(msg) \
    &(msg), \
    DOTTED_ESPADDR((msg).aFrom.addr), \
    DOTTED_ESPADDR((msg).aTo.addr), \
    (msg).nMsg, (msg).lParam, (msg).nLen
#define S_TP_QUERY_STUB_MSG_FMT \
    "TPStubMsg[%p]=[ver=%u, fromId=%lu, toId=%lu, err=%d, stub=%u]"
#define DOTTED_TP_QEURY_STUB_MSG(msg) \
    &(msg), (msg).ver, (msg).from_id, (msg).to_id, (msg).err, (msg).stub
#define S_TP_DATA_MSG_FMT \
    "TPDataMsg[%p]=[ver=%u, fromId=%lu, toId=%lu, err=%d, size=%d, batch=%d, ack=%lu]"
#define DOTTED_TP_DATA_MSG(msg) \
    &(msg), (msg).ver, (msg).from_id, (msg).to_id, (msg).err, \
    (msg).size, (msg).batch, (msg).ack

//-----------------------------------------------
// transportor management interface
// kylin context

// @brief    for upstream pe
struct IfSourceMeta {
    uint64_t    id;   // source id(peid)
    ESPNODE     node; // source esp addr(ip:port)
};
// @brief    for downstream pe
struct OfMeta {
    uint64_t                 id;     // outflow id(processor id)
    uint64_t                 parall; // parallisiam
    std::vector<std::string> tags;   // tags
};

struct IfSourceMgrCtx: AsyncContext {
    const std::vector<IfSourceMeta>* sources;
};
struct OfMgrCtx: AsyncContext {
    const std::vector<OfMeta>* outflows;
};

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTER_PROTOCAL_H__
