/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTOR_COUNTERS_H__
#define __DSTREAM_SRC_CC_TRANSPORTOR_COUNTERS_H__

#include <stdint.h>
#include <string>
#include "common/error.h"
#include "common/utils.h"
#include "transporter/transporter_common.h"

namespace dstream {
namespace transporter {

class TPUint64Counter {
public:
    TPUint64Counter(): m_total(0), m_last_total(0) {
        m_last_time = GetUint64TimeMs();
    }

    TPUint64Counter(uint64_t total): m_total(total), m_last_total(0) {
        m_last_time = GetUint64TimeMs();
    }

    void Reset(){
        m_total      = 0;
        m_last_total = 0;
        m_last_time  = GetUint64TimeMs();
    }

    uint64_t total() const {
        return m_total;
    }

    uint64_t qps() {
        return m_qps;
    }

    void UpdateQps() {
        uint64_t cur           = GetUint64TimeMs();
        uint64_t counter_lap   = m_total - m_last_total;
        uint64_t time_lap_snds = (cur - m_last_time) / 1000;

        m_last_total = m_total;
        m_last_time  = cur;

        m_qps = 0 == time_lap_snds ?
                0 : counter_lap / time_lap_snds;
    }

    void Set(uint64_t t) { m_total = m_last_total = t; }    // for "total" only
    void Add(uint64_t t) { m_total += t; }
    void Sub(uint64_t t) { m_total -= t; }

private:
    uint64_t    m_total;
    uint64_t    m_qps;
    uint64_t    m_last_total;
    uint64_t    m_last_time;
};

struct TPQueueCounter {
    TPUint64Counter    cnt_capability;
    double             cnt_watermark;
    TPUint64Counter    cnt_data_size;
    TPUint64Counter    cnt_unacked_size;

    TPQueueCounter() {} 
    TPQueueCounter(uint64_t cap, double wmk, uint64_t size, uint64_t unacked)
            : cnt_capability(cap),
              cnt_watermark(wmk),
              cnt_data_size(size),
              cnt_unacked_size(unacked) {} 

    void DumpJson(std::string* out);
};

struct TPSourceCounter {
    uint64_t                id;
    TPUint64Counter         cnt_in_num;
    TPUint64Counter         cnt_in_size;
    TPUint64Counter         cnt_sync_num;
    TPUint64Counter         cnt_drop_ack_miss_num;
    TPUint64Counter         cnt_drop_ack_miss_size;
    TPUint64Counter         cnt_drop_status_miss_num;
    TPUint64Counter         cnt_drop_status_miss_size;
    TPUint64Counter         cnt_drop_agelog_data_num;
    TPUint64Counter         cnt_drop_agelog_data_size;
    TPUint64Counter         cnt_drop_tranbuf_full_num;
    TPUint64Counter         cnt_drop_tranbuf_full_size;
    TPUint64Counter         cnt_pending_recv_num;
    TPUint64Counter         cnt_pending_recv_size;

    void DumpJson(std::string* out);
    void UpdateQps();
};

struct TPBranchCounter {
    uint64_t                id;
    TPUint64Counter         cnt_in_num;
    TPUint64Counter         cnt_in_size;
    TPUint64Counter         cnt_out_num;
    TPUint64Counter         cnt_out_size;
    TPUint64Counter         cnt_replay_num;
    TPUint64Counter         cnt_replay_size;
    TPUint64Counter         cnt_queue_full_num;
    TPUint64Counter         cnt_other_resync_num;
    TPUint64Counter         cnt_jitter_resync_num;
    TPUint64Counter         cnt_ack_miss_fetch_num;
    TPUint64Counter         cnt_ack_without_fetch_num;
    TPUint64Counter         cnt_may_drop_resync_num;
    TPUint64Counter         cnt_may_drop_resync_size;
    TPUint64Counter         cnt_drop_agelog_data_num;
    TPUint64Counter         cnt_drop_agelog_data_size;
    TPQueueCounter          cnt_queue;

    void DumpJson(std::string* out);
    void UpdateQps();
};

struct TPIfCounter {
    TPUint64Counter                 cnt_source_num;         // num of all sources
    TPUint64Counter                 cnt_sync_source_num;    // num of invalid sources
    TPUint64Counter                 cnt_invalid_source_num; // num of invalid sources
    TPUint64Counter                 cnt_out_num;
    TPUint64Counter                 cnt_out_size;
    TPUint64Counter                 cnt_out_replay_num;
    TPUint64Counter                 cnt_out_replay_size;
    TPUint64Counter                 cnt_drop_agelog_data_num;
    TPUint64Counter                 cnt_drop_agelog_data_size;
    TPUint64Counter                 cnt_drop_ack_miss_num;
    TPUint64Counter                 cnt_pending_fetch_num;
    TPQueueCounter                  cnt_queue;
    std::vector<TPSourceCounter>    cnt_sources;

    void DumpJson(std::string* out);
    void UpdateQps();
};

struct TPOfCounter {
    uint64_t                       id;
    TPUint64Counter                cnt_branch_num;
    std::vector<TPBranchCounter>   cnt_branches;

    void DumpJson(std::string* out);
};

struct TPAllOfsCounter {
    TPUint64Counter             cnt_of_num;        // num of outflows
    TPUint64Counter             cnt_in_num;
    TPUint64Counter             cnt_in_size;
    TPUint64Counter             cnt_out_num;
    TPUint64Counter             cnt_out_size;
    TPUint64Counter             cnt_drop_ack_miss_num;
    TPUint64Counter             cnt_drop_ack_miss_size;
    TPUint64Counter             cnt_drop_nosubs_num;
    TPUint64Counter             cnt_drop_nosubs_size;
    TPUint64Counter             cnt_drop_queue_full_num;
    TPUint64Counter             cnt_drop_queue_full_size;
    TPUint64Counter             cnt_drop_tranbuf_full_num;
    TPUint64Counter             cnt_drop_tranbuf_full_size;
    TPUint64Counter             cnt_may_drop_resync_num;
    TPUint64Counter             cnt_may_drop_resync_size;
    TPUint64Counter             cnt_pending_fetch_num;
    TPUint64Counter             cnt_pending_recv_num;
    TPUint64Counter             cnt_pending_recv_size;
    std::vector<TPOfCounter>    cnt_ofs;

    void DumpJson(std::string* out);
    void UpdateQps();
};

struct TPCounterDumpCtx : AsyncContext {
    uint64_t                    id;
    TPAllOfsCounter             ofs_cnt;
    bool                        has_if_cnt;
    TPIfCounter                 if_cnt;

    TPCounterDumpCtx(): has_if_cnt(false) {}
    TPCounterDumpCtx(uint64_t id_, TPAllOfsCounter ofs_cnt_)
            : id(id_), ofs_cnt(ofs_cnt_), has_if_cnt(false) {}
    TPCounterDumpCtx(uint64_t id_, TPAllOfsCounter ofs_cnt_, TPIfCounter if_cnt_)
            : id(id_), ofs_cnt(ofs_cnt_), if_cnt(if_cnt_) {}
};

enum TPCounterAction {
};

class TPCounterDumper :  public CAsyncClient {
public:
    static const char* kCounterDir;

    TPCounterDumper();
    ~TPCounterDumper();

    ErrCode Start();
    void    Stop();

    void OnCompletion(AsyncContext* ctx);

private:
    void _OnRecvCounter(AsyncContext* ctx);
    void _DumpJsonToFile(const std::string& json_str, const std::string& file);
private:
    bool m_running;
};

extern TPCounterDumper* g_pCounterDumper;

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTOR_COUNTERS_H__
