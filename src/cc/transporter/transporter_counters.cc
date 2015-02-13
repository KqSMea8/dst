#include "transporter/transporter_counters.h"
#include "common/logger.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace transporter {

TPCounterDumper* g_pCounterDumper = NULL;
const char*      TPCounterDumper::kCounterDir = "./counter";

TPCounterDumper::TPCounterDumper()
    : m_running(false) {
}

TPCounterDumper::~TPCounterDumper() {
    Stop();
}

ErrCode TPCounterDumper::Start() {
    if (MakeDir(kCounterDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0 &&
        EEXIST != errno) {
        DSTREAM_WARN("fail to mkir for %s: %s", kCounterDir, strerror(errno));
        return error::FILE_OPERATION_ERROR;
    }
    m_running = true;
    return error::OK;
}

void TPCounterDumper::Stop() {
}

void TPSourceCounter::UpdateQps() {
    cnt_in_num.UpdateQps();
    cnt_in_size.UpdateQps();
}

void TPBranchCounter::UpdateQps() {
    cnt_in_num.UpdateQps();
    cnt_in_size.UpdateQps();
    cnt_out_num.UpdateQps();
    cnt_out_size.UpdateQps();
}

void TPIfCounter::UpdateQps() {
    cnt_out_num.UpdateQps();
    cnt_out_size.UpdateQps();
}

void TPAllOfsCounter::UpdateQps() {
    cnt_in_num.UpdateQps();
    cnt_in_size.UpdateQps();
    cnt_out_num.UpdateQps();
    cnt_out_size.UpdateQps();
}

#define _CounterTotalToJson(field) \
    if (!comma) {       \
        comma = true;   \
    } else {            \
        *json_str += ",";\
    }                   \
    *json_str += "\"";    \
    *json_str += #field; \
    *json_str += "\":";    \
    StringAppendNumber(json_str, field.total());

#define _CounterTotalAndQpsToJson(field) \
    if (!comma) {       \
        comma = true;   \
    } else {            \
        *json_str += ",";\
    }                   \
    *json_str += "\"";    \
    *json_str += #field; \
    *json_str += "_total"; \
    *json_str += "\":";    \
    StringAppendNumber(json_str, field.total()); \
    *json_str += ", \"";    \
    *json_str += #field; \
    *json_str += "_qps"; \
    *json_str += "\":";    \
    StringAppendNumber(json_str, field.qps());

void TPQueueCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);

    // "Queue": {"":"", "":""}
    *json_str += "\"Queue\": {";
    bool comma = false;

    _CounterTotalToJson(cnt_capability);
    char wmk[64];
    snprintf(wmk, sizeof(wmk), "%f", cnt_watermark);
    *json_str += ",\"cnt_watermark\":";
    *json_str += wmk;
    _CounterTotalToJson(cnt_data_size);
    _CounterTotalToJson(cnt_unacked_size);
    
    *json_str += "}";
};

void TPSourceCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);

    // "id": {"":"", "":""}

    *json_str += "\"";
    StringAppendNumber(json_str, id);
    *json_str += "\":{";

    bool comma = false;
    _CounterTotalAndQpsToJson(cnt_in_num);
    _CounterTotalAndQpsToJson(cnt_in_size);
    _CounterTotalToJson(cnt_sync_num);
    _CounterTotalToJson(cnt_drop_ack_miss_num);
    _CounterTotalToJson(cnt_drop_ack_miss_size);
    _CounterTotalToJson(cnt_drop_status_miss_num);
    _CounterTotalToJson(cnt_drop_status_miss_size);
    _CounterTotalToJson(cnt_drop_agelog_data_num);
    _CounterTotalToJson(cnt_drop_agelog_data_size);
    _CounterTotalToJson(cnt_drop_tranbuf_full_num);
    _CounterTotalToJson(cnt_drop_tranbuf_full_size);
    _CounterTotalToJson(cnt_pending_recv_num);
    _CounterTotalToJson(cnt_pending_recv_size);

    *json_str += "}";
}

void TPIfCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);

    // "inflow" : {blablabla}
    *json_str += "\"Inflow\": {";
    bool comma = false;

    _CounterTotalToJson(cnt_source_num);
    _CounterTotalToJson(cnt_sync_source_num);
    _CounterTotalToJson(cnt_invalid_source_num);
    _CounterTotalAndQpsToJson(cnt_out_num);
    _CounterTotalAndQpsToJson(cnt_out_size);
    _CounterTotalToJson(cnt_out_replay_num);
    _CounterTotalToJson(cnt_out_replay_size);
    _CounterTotalToJson(cnt_drop_agelog_data_num);
    _CounterTotalToJson(cnt_drop_agelog_data_size);
    _CounterTotalToJson(cnt_drop_ack_miss_num);
    _CounterTotalToJson(cnt_pending_fetch_num);

    *json_str += ",";
    cnt_queue.DumpJson(json_str);

    // update TPSourceCounter in TPIfCounter
    *json_str += ",\"Source\":{";

    for (uint64_t i = 0; i < cnt_sources.size(); i ++) {
        if (0 != i) {
            *json_str += ",";
        }
        cnt_sources[i].DumpJson(json_str);
    } // end of for

    *json_str += "}}";   // inflow and sources
};

void TPBranchCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);

    // "id" : {blablabla}
    *json_str += "\"";
    StringAppendNumber(json_str, id);
    *json_str += "\":{";

    bool comma = false;
    _CounterTotalAndQpsToJson(cnt_in_num);
    _CounterTotalAndQpsToJson(cnt_in_size);
    _CounterTotalAndQpsToJson(cnt_out_num);
    _CounterTotalAndQpsToJson(cnt_out_size);
    _CounterTotalToJson(cnt_replay_num);
    _CounterTotalToJson(cnt_replay_size);
    _CounterTotalToJson(cnt_queue_full_num);
    _CounterTotalToJson(cnt_other_resync_num);
    _CounterTotalToJson(cnt_jitter_resync_num);
    _CounterTotalToJson(cnt_ack_miss_fetch_num);
    _CounterTotalToJson(cnt_ack_without_fetch_num);
    _CounterTotalToJson(cnt_may_drop_resync_num);
    _CounterTotalToJson(cnt_may_drop_resync_size);
    _CounterTotalToJson(cnt_drop_agelog_data_num);
    _CounterTotalToJson(cnt_drop_agelog_data_size);

    *json_str += ",";
    cnt_queue.DumpJson(json_str);

    *json_str += "}";
}

void TPOfCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);
    // "id" : {blablabla}
    *json_str += "\"";
    StringAppendNumber(json_str, id);
    *json_str += "\":{";
    bool comma = false;
    _CounterTotalToJson(cnt_branch_num);
    *json_str += "}, \"Branch\":{";

    // update TPBranchCounter in TPOfCounter
    for (uint64_t i = 0; i < cnt_branches.size(); ++i) {
        if (0 != i) {
            *json_str += ",";
        }
        cnt_branches[i].DumpJson(json_str);
    }
    *json_str += "}";
}

void TPAllOfsCounter::DumpJson(std::string* json_str) {
    assert(NULL != json_str);

    // "AllOutflows": {"":"", "":""}
    *json_str += "\"AllOutflows\": {";
    bool comma = false;

    _CounterTotalToJson(cnt_of_num);
    _CounterTotalAndQpsToJson(cnt_in_num);
    _CounterTotalAndQpsToJson(cnt_in_size);
    _CounterTotalAndQpsToJson(cnt_out_num);
    _CounterTotalAndQpsToJson(cnt_out_size);
    _CounterTotalToJson(cnt_drop_ack_miss_num);
    _CounterTotalToJson(cnt_drop_ack_miss_size);
    _CounterTotalToJson(cnt_drop_nosubs_num);
    _CounterTotalToJson(cnt_drop_nosubs_size);
    _CounterTotalToJson(cnt_drop_queue_full_num);
    _CounterTotalToJson(cnt_drop_queue_full_size);
    _CounterTotalToJson(cnt_drop_tranbuf_full_num);
    _CounterTotalToJson(cnt_drop_tranbuf_full_size);
    _CounterTotalToJson(cnt_may_drop_resync_num);
    _CounterTotalToJson(cnt_may_drop_resync_size);
    _CounterTotalToJson(cnt_pending_fetch_num);
    _CounterTotalToJson(cnt_pending_recv_num);
    _CounterTotalToJson(cnt_pending_recv_size);

    // update TPOfCounter in TPAllOfsCounter
    *json_str += ",\"Outflow\":{";

    for (uint64_t i = 0; i < cnt_ofs.size(); ++i) { // updsate TPOfCounter in TPAllOfsCounter
        if (0 != i) {
            *json_str += ",";
        }
        cnt_ofs[i].DumpJson(json_str);
    }

    *json_str += "}}";
}

void TPCounterDumper::_OnRecvCounter(AsyncContext* ctx) {
    TPCounterDumpCtx* cnt_ctx = reinterpret_cast<TPCounterDumpCtx*>(ctx);

    std::string json_str;
    json_str.resize(4096);
    json_str  = "{\"";
    StringAppendNumber(&json_str, cnt_ctx->id);
    json_str += "\":{";

    // update inflows
    if (cnt_ctx->has_if_cnt) {
        cnt_ctx->if_cnt.DumpJson(&json_str);
        json_str += ",";
    }
    // update outflows
    cnt_ctx->ofs_cnt.DumpJson(&json_str);
    json_str += "},\"Timestamp\":";
    StringAppendNumber(&json_str, GetUint64TimeMs());
    json_str += "}";

    std::string file(kCounterDir);
    file += "/";
    StringAppendNumber(&file, cnt_ctx->id);
    file += ".cnt.json";
    _DumpJsonToFile(json_str, file);

    SAFE_DELETE(cnt_ctx);
}
#undef  _CounterTotalToJson
#undef  _CounterTotalAndQpsToJson

void TPCounterDumper::_DumpJsonToFile(const std::string& json_str, const std::string& file) {
    std::string tmp_file = file + ".tmp.cnt~";
    FILE* fp = fopen(tmp_file.c_str(), "w");
    if (NULL == fp) {
        DSTREAM_WARN("fail to open file[%s]: errno=%d",
                     tmp_file.c_str(), errno);
        return;
    }
    size_t size = json_str.size();
    size_t nwrite = fwrite(json_str.c_str(), size, 1, fp);
    if (1 != nwrite) {
        DSTREAM_WARN("fail to write json into file[%s]: %lu : errno=%d, %s",
                     file.c_str(), nwrite, errno, json_str.c_str());
    }
    fclose(fp);
    if (rename(tmp_file.c_str(), file.c_str()) < 0) {
        DSTREAM_WARN("fail to mv file [%s] => [%s], erro=%d",
                     tmp_file.c_str(), file.c_str(), errno);
        return;
    }
    return;
}

void TPCounterDumper::OnCompletion(AsyncContext* ctx) {
    switch (ctx->nAction) {
    case TPA_CNT_DUMP:
        return _OnRecvCounter(ctx);
    default:
        DSTREAM_FATAL("unkown AsyncContext=[%p nErrCode=%d, nActon=%d, "
                      "pClient=%p, fProc = %p]", ctx, ctx->nErrCode,
                      ctx->nAction, ctx->pClient, ctx->fProc);
    }
}

} // namespace transporter
} // namespace dstream
