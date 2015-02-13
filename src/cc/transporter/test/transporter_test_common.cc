#include "transporter_test_common.h"

namespace dstream {

//-----------------------------------------------
// Condition Sync
CCond g_tp_test_cond;
// TODO:
// static uint64_t g_tp_test_cond_cnt = 0;

void TPTestStart(int count) {
    //DSTREAM_DEBUG("%lu:g_tp_test_cond start with count=%d", g_tp_test_cond_cnt, count);
    g_tp_test_cond.Init(count);
}

void TPTestWait() {
    //DSTREAM_DEBUG("%lu: g_tp_test_cond wait...", g_tp_test_cond_cnt);
    g_tp_test_cond.Wait();
}
void TPTestComplete() {
    //DSTREAM_DEBUG("%lu: g_tp_test_cond signal", g_tp_test_cond_cnt++);
    g_tp_test_cond.Signal();
}

//-----------------------------------------------
// TestTransptorterMgr
IMPLEMENT_SINGLETON(TestTransptorterMgr)

transporter::ErrCode TestTransptorterMgr::SetTransporter(uint64_t id,
        transporter::Transporter* tp) {
    if (tp == NULL) {
        return error::BAD_ARGUMENT;
    }
    MutexLockGuard lock_guard(mutex_);
    TestTransporterMap::iterator it = map_.find(id);
    if (it != map_.end()) {
        return error::TP_ALREADY_EXIST;
    }
    tp->AddRef();
    map_[id] = tp;
    return error::OK;
}

transporter::Transporter* TestTransptorterMgr::GetTransporter(uint64_t id) {
    MutexLockGuard lock_guard(mutex_);
    TestTransporterMap::iterator it = map_.find(id);
    if (it != map_.end()) {
        it->second->AddRef();
        return it->second;
    }
    return NULL;
}

void TestTransptorterMgr::DelTransporter(uint64_t id) {
    MutexLockGuard lock_guard(mutex_);
    TestTransporterMap::iterator it = map_.find(id);
    if (it != map_.end()) {
        it->second->Release();
        it->second = NULL;
        map_.erase(it);
    }
}

//-----------------------------------------------
// TestTransptorterMgr
const char* kTestTag = "TestTag";
int         kMaxSingleLen = 100;

enum FillTuplesType {
    _FillAtMostN  = 0,
    _FillAtLeastN = 1
};

static transporter::ErrCode _FillTuples(BufHandle** hdl,
                                        int* size,
                                        int* batch,
                                        FillTuplesType type) {
    CBufWriteStream bws;
    CRandom rand;
    rand.SetSeed(-1);

    *batch = 0;
    int n = *size;
    for (int i = 0; n > 0; ++i) {
        int data_len  = (int)(rand.Next() * kMaxSingleLen) + 1;
        int tag_len   = (int)(strlen(kTestTag) + 1);
        int total_len = (int)(sizeof(uint64) + sizeof(int)) + tag_len + data_len;
        if (_FillAtMostN == type) {
            if (n - total_len - (int)sizeof(int) < 0) {
                break;
            }
        }

        std::string s;
        //ascii[32, 126]
        for (int j = 0; j < data_len; ++j) {
            s += (char)((int)(rand.Next() * 95) + 32);
        }

        if (!bws.PutObject<int>(total_len) ||//total len
            !bws.PutObject<uint64>((uint64)(i)) ||//hash code
            !bws.PutObject<int>(tag_len)    ||//tag len
            bws.PutBuffer(kTestTag, tag_len) != tag_len ||//tag
            bws.PutBuffer(s.data(), (int)s.size()) != (int)s.size()) {
            return error::SERIAIL_TUPLE_FAIL;
        }
        (*batch)++;
        n -= (total_len + (int)sizeof(int));
    }
    *size -= n;
    *hdl = bws.GetBegin();
    return error::OK;
}

transporter::ErrCode FillTuplesAtMostN(BufHandle** hdl, int* size, int* batch) {
    return _FillTuples(hdl, size, batch, _FillAtMostN);
}

transporter::ErrCode FillTuplesAtLeastN(BufHandle** hdl, int* size, int* batch) {
    return _FillTuples(hdl, size, batch, _FillAtLeastN);
}


} //namespace dstream
