/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <cassert>
#include "processelement/loopback.h"
#include "processelement/context.h"

namespace dstream {
namespace processelement {

using namespace dstream::common;
using namespace transporter;

static const char* kTaskCountersGroup   = "pe";
// static uint64_t    g_PrintStatusInteval = 1 * 1000;

TaskCounters::TaskCounters()
    : recv_tuples_counter(NULL),
      recv_bytes_counter(NULL),
      send_tuples_counter(NULL),
      send_bytes_counter(NULL),
      cpu_usage_counter(NULL),
      mem_usage_counter(NULL),
      importer_queue_counter(NULL),
      importer_latency_counter(NULL),
      last_cpu_stat(0, 0) {
}

TaskCounters::~TaskCounters() {
    delete recv_tuples_counter;
    delete recv_bytes_counter;
    delete send_tuples_counter;
    delete send_bytes_counter;
    delete cpu_usage_counter;
    delete mem_usage_counter;
    delete importer_queue_counter;
    delete importer_latency_counter;
}

bool TaskCounters::Init(uint64_t pe_id) {
    return Init(pe_id, false);
}

bool TaskCounters::Init(uint64_t pe_id, bool as_importer) {
    uint64_t app_id = pe_id >> 40;
    uint64_t processor_id = pe_id >> 32;
    // attr1=pe_id, attr2=app_id, attr3=processor_id
    recv_tuples_counter = Counters::CreateUint64RateCounter(kTaskCountersGroup, "RecvTuples", pe_id,
                          app_id, processor_id);
    recv_bytes_counter = Counters::CreateUint64RateCounter(kTaskCountersGroup, "RecvBytes", pe_id,
                         app_id, processor_id);
    send_tuples_counter = Counters::CreateUint64RateCounter(kTaskCountersGroup, "SendTuples", pe_id,
                          app_id, processor_id);
    send_bytes_counter = Counters::CreateUint64RateCounter(kTaskCountersGroup, "SendBytes", pe_id,
                         app_id, processor_id);
    cpu_usage_counter = Counters::CreateUint64NumCounter(kTaskCountersGroup, "CPU", pe_id, app_id,
                        processor_id);
    mem_usage_counter = Counters::CreateUint64NumCounter(kTaskCountersGroup, "Mem", pe_id, app_id,
                        processor_id);

    // importer counters
    if (as_importer) {
        importer_queue_counter = Counters::CreateUint64NumCounter(kTaskCountersGroup,
                                                                  "ImpQSize", pe_id,
                                                                  app_id, processor_id);
        importer_latency_counter = Counters::CreateUint64NumCounter(kTaskCountersGroup,
                                                                    "ImpLatency", pe_id,
                                                                    app_id, processor_id);
        if (importer_latency_counter == NULL || importer_queue_counter == NULL) {
            return false;
        }
    }

    return recv_tuples_counter != NULL
           && recv_bytes_counter != NULL
           && send_tuples_counter != NULL
           && send_bytes_counter != NULL
           && cpu_usage_counter != NULL
           && mem_usage_counter != NULL;
}

// mock process node services.
// ----------------------------------------
LoopBackHeartbeatService::LoopBackHeartbeatService(LoopBack* loop_back):
    loop_back_(loop_back) {
    set_service_name(rpc_const::kPubSubHeartbeatMethod);
}
std::string LoopBackHeartbeatService::PUB_SUB_HEARTBEAT(const HeartbeatRequest* req,
        HeartbeatACK* /*ack*/) {
    assert(req->metric_name().size() == req->metric_value().size());
    std::map< std::string, std::string > metrics;
    for (int i = 0; i < req->metric_name().size(); i++) {
        const std::string& k = req->metric_name(i);
        const std::string& v = req->metric_value(i);
        metrics[k] = v;
    }
    loop_back_->OnHeartbeat(req->peid().id(), metrics);
    return rpc_const::kOK;
}

// ------------------------------------------------------------
// LoopBack Implementation
ESP_BEGIN_MAP_MESSAGE(LoopBack, CESPObject)
ESP_MAP_MESSAGE_RAW(TPE_IF_LOCALPE_FETCH, OnFetchData)
ESP_MAP_MESSAGE_RAW(TPE_OF_RECV_LOCALPE, OnForwardData)
ESP_END_MAP_MESSAGE

LoopBack::LoopBack():
    m_srv_heartbeat(this),
    m_buf(NULL),
    m_buf_len(transporter::kMaxMsgSize),
    m_if_cli(NULL),
    m_of_cli(NULL),
    m_if_ack(0),
    m_of_ack(0) {
    m_loopback_id = 2011;
    m_services[0] = &m_srv_heartbeat;
}

int32_t LoopBack::Start() {
    int32_t ret;
    if (!transporter::TPInitKylin()) {
        DSTREAM_DEBUG("Init Kylin fail");
        return -1;
    }
    KLSetLog(stdout, 0, NULL);
    ret = g_pTPStubMgr->Start();
    if (ret < error::OK) {
        DSTREAM_DEBUG("start stub manager fail");
        return ret;
    }

    g_pTPStubMgr->FreeIfStub(m_loopback_id);
    g_pTPStubMgr->FreeOfStub(m_loopback_id);
    m_if_stub = g_pTPStubMgr->AllocIfStub(m_loopback_id);
    m_of_stub = g_pTPStubMgr->AllocOfStub(m_loopback_id);

    VERIFY_OR_RETURN(ESPE_OK == BindStub(m_if_stub), error::BIND_STUB_FAIL);
    VERIFY_OR_RETURN(ESPE_OK == BindStub(m_of_stub), error::BIND_STUB_FAIL);
    return error::OK;
}

void LoopBack::Stop() {
    UnbindStub();

    g_pTPStubMgr->FreeIfStub(m_loopback_id);
    g_pTPStubMgr->FreeOfStub(m_loopback_id);

    g_pTPStubMgr->Stop();
    transporter::TPStopKylin();
}

void LoopBack::OnReady(ESPMessage* req_msg) {
}

void LoopBack::OnFetchData(ESPMessage* req_msg) {
    DSTREAM_DEBUG("OnFetchData...");

    CBufReadStream brs(req_msg->req.pHdl, NULL);
    transporter::TPDataMsg msg;
    brs.GetObject<transporter::TPDataMsg>(&msg);

    CBufWriteStream bws;
    bws.PutObject<transporter::TPDataMsg>(msg);

    Tuples tuples;
    // to generate tuples
    LoopSetUp(&tuples);

    int batch = 0, ret = 0;
    size_t size = config_const::kPNTuplesBatchSizeByte;
    ret = SerialTuples(tuples, &bws, &batch, &size);
    for (size_t i = 0; i < tuples.size(); ++i) {
        DeallocateTuple(const_cast<Tuple*>(tuples[i]));
    }
    if (ret != error::OK) {
        DSTREAM_WARN("SerialTuples failed, ret=%d", ret);
        return;
    }

    // FillDataToWriteStream(&bws, &msg.size, &msg.batch);
    BufHandle* phdl     = bws.GetBegin();
    transporter::TPDataMsg* ack_msg = (transporter::TPDataMsg*)(phdl->pBuf);
    ack_msg->from_id    = m_loopback_id;
    ack_msg->to_id      = msg.from_id;
    ack_msg->err        = transporter::TPN_ACK_OK;
    ack_msg->batch      = batch;
    ack_msg->size       = size;
    ack_msg->chksum     = 0xFFFEEEEEEEEEEEEE;

    if (error::OK != PostMsg(req_msg->aFrom.addr,
                             transporter::TPE_IF_LOCALPE_FETCH_ACK,
                             req_msg->lParam, phdl)) {
        DSTREAM_DEBUG("fail to post msg");
    }
}

void LoopBack::OnForwardData(ESPMessage* req_msg) {
    DSTREAM_DEBUG("OnForwardData...");

    CBufReadStream brs(req_msg->req.pHdl, NULL);
    transporter::TPDataMsg msg;
    brs.GetObject<transporter::TPDataMsg>(&msg);

    Tuples tuples;
    int ret = DeserialTuples(req_msg->req.pHdl, msg.batch, msg.size, &tuples);
    if (ret != error::OK) {
        DSTREAM_WARN("deserialize tuples failed, ret=%d", ret);
        return;
    }
    // call user specified post handlers
    LoopTearDown(tuples);
    for (size_t i = 0; i < tuples.size(); ++i) {
        DeallocateTuple(const_cast<Tuple*>(tuples[i]));
    }

    msg.err = transporter::TPN_ACK_OK;
    msg.chksum = 0xFFFEEEEEEEEEEEEE;
    CBufWriteStream bws;
    bws.PutObject<transporter::TPDataMsg>(msg);
    if (error::OK != PostMsg(req_msg->aFrom.addr,
                             transporter::TPE_OF_RECV_LOCALPE,
                             req_msg->lParam, bws.GetBegin())) {
        DSTREAM_DEBUG("fail to post msg");
    }
}

void LoopBack::OnHeartbeat(uint64_t peid, const std::map< std::string, std::string >& metrics) {
    DSTREAM_DEBUG("OnHeartbeat(%zu)...", static_cast<size_t>(peid));
}

void LoopBack::RegisterTupleTypeInfo(const std::string& tag, TupleTypeInfo* info) {
    m_type_map_lock.LockForWrite();
    m_tuple_type_info_map[tag] = info;
    m_type_map_lock.Unlock();
}

TupleTypeInfo* LoopBack::FindTupleTypeInfoByTag(const std::string& tag) {
    m_type_map_lock.LockForRead();
    TupleTypeInfoMap::iterator it = m_tuple_type_info_map.find(tag);
    if (it != m_tuple_type_info_map.end()) {
        // DSTREAM_DEBUG("get tag [%s] in map", tag.c_str());
        TupleTypeInfo* res = it->second;
        m_type_map_lock.Unlock();
        return res;
    }
    m_type_map_lock.Unlock();
    m_type_map_lock.LockForWrite();
    it = m_tuple_type_info_map.find(tag);
    if (it != m_tuple_type_info_map.end()) {
        DSTREAM_DEBUG("get tag [%s] in map", tag.c_str());
        TupleTypeInfo* res = it->second;
        m_type_map_lock.Unlock();
        return res;
    }
    std::string file, type_name;
    if (!dynamic_type::GetDynamicTypeInfo(tag, &file, &type_name)) {
        DSTREAM_DEBUG("parse tag [%s] fail", tag.c_str());
        m_type_map_lock.Unlock();
        return NULL;
    }
    DynamicProtoType* add_proto_type = new DynamicProtoType();
    if (!add_proto_type->Init(file, type_name) &&
        !add_proto_type->Init(std::string("dynamic/proto/") + file, type_name)) {
        m_type_map_lock.Unlock();
        delete add_proto_type;
        return NULL;
    }
    m_tuple_type_info_map[tag] = add_proto_type;
    m_type_map_lock.Unlock();
    return add_proto_type;
}

Tuple* LoopBack::AllocateTuple(const std::string& tag) {
    TupleTypeInfo* info = FindTupleTypeInfoByTag(tag);
    if (!info) {
        return NULL;
    }
    Tuple* tuple = info->allocate();
    if (!tuple) {
        return NULL;
    }
    Tuple::increase_instance_counter();
    tuple->set_tag(tag);
    return tuple;
}

void LoopBack::DeallocateTuple(Tuple* tuple) {
    TupleTypeInfo* info = FindTupleTypeInfoByTag(tuple->tag());
    assert(info);
    info->deallocate(tuple);
}

int LoopBack::SerialTuples(const Tuples& tuples,
                       CBufWriteStream* bws,
                       int*    batch,
                       size_t* size) {
    size_t n = 0;
    size_t i = 0;
    for (; i < tuples.size(); i++) {
        // write tuple meta
        Tuple* tuple = const_cast<Tuple*>(tuples[i]);

        // write "hash | tag_len | tag" to m_buf
        Byte* cur = m_buf + sizeof(int);
        *(reinterpret_cast<uint64*>(cur)) = tuple->hash_code();
        cur += sizeof(tuple->hash_code());
        const std::string& tag = tuple->tag();
        int tag_len = tag.size() + 1; // char
        *(reinterpret_cast<int*>(cur)) = tag_len;
        cur += sizeof(tag_len);
        memcpy(cur, tag.data(), tag_len);
        cur += tag_len;
        int meta_len = cur - m_buf;

        // write tuple to m_buf
        FixBufByteArray byte_array(cur, m_buf_len - meta_len);
        UNLIKELY_IF(!tuple->Serialize(&byte_array)) {
            return error::SERIAIL_TUPLE_FAIL;
        }

        // write len m_buf
        int len = meta_len + byte_array.DataSize();
        *(reinterpret_cast<int*>(m_buf)) = len - sizeof(len);

        if (static_cast<int>(n) < transporter::kMaxMsgSize - len) {
            if (n + len > *size) {
                break;
            }
            // serial m_buf to buf stream
            UNLIKELY_IF(bws->PutBuffer(reinterpret_cast<char*>(m_buf), len) != len) {
                return error::SERIAIL_DATA_FAIL;
            }
            n += len;
            continue;
        }
        break;
    }
    *size  = n;
    *batch = static_cast<int>(i);

    DSTREAM_DEBUG("tuple size: %zd, serialized size: %zd", tuples.size(), i);
    return (0 == i) && (0 != tuples.size()) ? error::TOO_LARGE_DATA : error::OK;
}

int LoopBack::DeserialTuples(BufHandle* hdl,
                         int        batch,
                         size_t     size,
                         Tuples*    tuples) {
    int ret = error::OK;
    CBufReadStream brs(hdl, NULL);
    // jump msg head
    UNLIKELY_IF(!brs.Seek(static_cast<int>(sizeof(transporter::TPDataMsg)))) {
        return error::DATA_CORRUPTED;
    }

    // deserial tuples
    for (int i = 0; i < batch; i++) {
        int total_len;
        UNLIKELY_IF(!brs.GetObject<int>(&total_len) ||
                    (total_len >= transporter::kMaxMsgSize - static_cast<int>(sizeof(total_len))) ||
                    (total_len != brs.GetBuffer(reinterpret_cast<char*>(m_buf), total_len))) {
            DSTREAM_WARN("Corrupt Msg: fail to extract Data: brs=%p, batch=%d, "
                         "size=%lu", &brs, batch, size);
            ret = error::DATA_CORRUPTED;
            goto ERR_RET;
        }
        uint64 hash = *reinterpret_cast<uint64*>(m_buf);
        int tag_len = *reinterpret_cast<int*>(m_buf + sizeof(hash));
        int tuple_len = total_len - static_cast<int>(sizeof(tag_len) - sizeof(total_len)) - tag_len;
        char* tag = reinterpret_cast<char*>(m_buf + sizeof(hash) + sizeof(tag_len));
        Byte* raw_tuple = reinterpret_cast<Byte*>(tag + tag_len);
        FixBufByteArray byte_array(raw_tuple, tuple_len);

        // parse in_tuples.
        Tuple* in_tuple = 0;
        if (internal::Context::PE_TYPE_NORMAL == m_pe_type) {
            in_tuple = AllocateTuple(tag);
        } else if (internal::Context::PE_TYPE_STREAMING == m_pe_type) {
            in_tuple = AllocateTuple(internal::Context::kStreamingKVTagName);
            if (in_tuple) {
                in_tuple->set_tag(tag);
            }
        } else if (internal::Context::PE_TYPE_BIGPIPE_EXPORTER == m_pe_type) {
            // bigpipe exporter
            in_tuple = AllocateTuple(tag);
        } else {
            DSTREAM_ERROR("unknown pe type!!, pe(id=%lu, type=%lu), tag=%s",
                          m_id, m_pe_type, tag);
            ret = error::UNKNOWN_PE_TYPE;
            goto ERR_RET;
        }

        UNLIKELY_IF(NULL == in_tuple) {
            DSTREAM_WARN("allocate tuple failed, tuple lost!!!: "
                         "tuple(hash=%lu, tag=%s)", static_cast<uint64_t>(hash), tag);
            ret = error::UNKNOWN_TUPLE_TYPE;
            goto ERR_RET;
        } else {
            // DSTREAM_DEBUG("allocateInTuple '%s'", tag);
            tuples->push_back(in_tuple);
        }

        // set hash code,
        // note!!! tuple itself dosn't carry the hash code, but log does.
        in_tuple->set_hash_code(hash);
        if (!in_tuple->Deserialize(&byte_array)) {
            DSTREAM_WARN("tuple(%p, hash=%lu, tag=%s) deserialize failed, "
                         " tuple lost!!!", in_tuple, static_cast<uint64_t>(hash), tag);
            ret = error::PARSE_DATA_FAIL;
            goto ERR_RET;
        }
    }

    // update recv bytes counter
    m_counters.recv_bytes_counter->Add(size);
    m_counters.recv_tuples_counter->Add(batch);
    AtomicAdd64(&m_recv_tuples_counter, batch);
    AtomicAdd64(&m_recv_time_tuples_counter, batch);

    return error::OK;

ERR_RET:
    for (size_t i = 0; i < tuples->size(); i++) {
        DeallocateTuple(const_cast<Tuple*>((*tuples)[i]));
    }
    return ret;
}


} // namespace processelement
} // namespace dstream
