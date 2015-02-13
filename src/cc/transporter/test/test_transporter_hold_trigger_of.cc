#include <gtest/gtest.h>

#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_outflow.h"

using namespace dstream;
using namespace dstream::transporter;

typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

int main(int argc, char** argv) {
    logger::initialize("pn_test");
    common::Counters::Init();
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    int ret = RUN_ALL_TESTS();
    common::Counters::Deinit();
    return ret;
}

class test_HoldAndTriggerFetch_suite : public ::testing::Test {
protected:
    test_HoldAndTriggerFetch_suite(): id_(kId), stub_(INVALID_ESPSTUB),
        tp_(NULL), down_pro_id_(kDownProId_),
        down_parall_(kParall) {};
    virtual ~test_HoldAndTriggerFetch_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeIfStub(id_);
        stub_    = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);

        tp_ = new Transporter(id_, stub_, INVALID_ESPSTUB);
        //set queue size and start outflow
        ASSERT_EQ(tp_->set_queues("TestSendQueue", "Tests",
                                  kQueueSize,
                                  kQueueSize,
                                  kQueueSoft,
                                  kQueueSoft), error::OK);
        ASSERT_EQ(tp_->Start(), error::OK);
        //add outflow

        TPOfSourceVec* outflows = new TPOfSourceVec();
        std::vector<std::string> tags;
        transporter::OfMeta of_meta = {down_pro_id_, down_parall_, tags};
        outflows->push_back(of_meta);
        OfMgrCtx* ctx = new OfMgrCtx();
        ctx->outflows = outflows;
        tp_->_OnAddOutflow(ctx);
        ASSERT_EQ(tp_->m_outflow_map.size(), 1UL);
    };
    virtual void TearDown() {
        SAFE_DELETE(tp_);
    };

public:
    static const uint64_t kParall     = 2UL;
    static const uint64_t kId         = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kDownProId_ = EXTRACT_APPPRO_ID(MAKE_PEID(1UL, 2UL, 0UL));
    static const uint64_t kQueueSize  = 2 * 1024;
    static const double   kQueueSoft  = 0.9f;

public:
    uint64_t     id_;
    ESPSTUB      stub_;
    Transporter* tp_;
    uint64_t     down_pro_id_;
    uint64_t     down_parall_;
};

TEST_F(test_HoldAndTriggerFetch_suite, HoldAndTriggerFetch) {
    transporter::Transporter::OutflowMap::iterator it;
    it = tp_->m_outflow_map.find(down_pro_id_);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    transporter::TPOutflow* outflow = it->second;
    ASSERT_TRUE(NULL != outflow);
    ASSERT_EQ(outflow->m_branches.size(), down_parall_);
    for (uint64_t seq = 0; seq < down_parall_; seq++) {
        TPBranch& branch = outflow->m_branches[seq];
        ASSERT_TRUE(branch.queue != NULL);
        ASSERT_TRUE(branch.queue->Empty());
    }

    const int kSize = 1024;

    //mock fetch
    for (uint64_t seq = 0; seq < down_parall_; seq++) {
        ESPMessage esp_msg;
        TPDataMsg* data_msg;
        esp_msg.req.pHdl   = AllocateHdl(true);
        esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
        data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
        data_msg->from_id  = MAKE_PROCESSOR_PE_ID(down_pro_id_, seq);
        data_msg->to_id    = id_;
        data_msg->size     = kSize;
        data_msg->batch    = 0;
        data_msg->ack      = 0;
        data_msg->err      = TPN_ACK_OK;
        //expect hold
        size_t peding_num = tp_->m_pending_fetch_map.size();
        tp_->OnOutflowFetch(&esp_msg);
        ASSERT_EQ(tp_->m_pending_fetch_map.size(), peding_num + 1);
        ChainFreeHdl(esp_msg.req.pHdl, NULL);
        esp_msg.req.pHdl = NULL;
    }
    //fetch again, expect update pending time, lparam
    for (uint64_t seq = 0; seq < down_parall_; seq++) {
        ESPMessage esp_msg;
        TPDataMsg* data_msg;
        esp_msg.req.pHdl   = AllocateHdl(true);
        esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
        data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
        data_msg->from_id  = MAKE_PROCESSOR_PE_ID(down_pro_id_, seq);
        data_msg->to_id    = id_;
        data_msg->size     = kSize;
        data_msg->batch    = 0;
        data_msg->ack      = 0;
        data_msg->err      = TPN_ACK_OK;
        //expect hold
        tp_->OnOutflowFetch(&esp_msg);
        ASSERT_EQ(tp_->m_pending_fetch_map.size(), down_parall_);
        ChainFreeHdl(esp_msg.req.pHdl, NULL);
        esp_msg.req.pHdl = NULL;
    }

    //feed data, expect trigger pending fetch
    BufHandle* hdl = NULL;
    int size  = kSize;
    int batch = 0;
    ASSERT_EQ(FillTuplesAtMostN(&hdl, &size, &batch), error::OK);
    ASSERT_GE(batch, (int)down_parall_);//to feed hash parallisms

    ESPMessage esp_msg;
    TPDataMsg* data_msg;
    esp_msg.req.pHdl   = AllocateHdl(true);
    esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
    data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
    data_msg->from_id  = id_;
    data_msg->to_id    = id_;
    data_msg->size     = size;
    data_msg->batch    = batch;
    data_msg->ack      = 0;
    data_msg->err      = TPN_ACK_OK;
    esp_msg.req.pHdl->_next = hdl;

    ASSERT_EQ(tp_->m_pending_fetch_map.size(), down_parall_);
    tp_->OnOutflowRecv(&esp_msg);
    ASSERT_EQ(tp_->m_pending_fetch_map.size(), 0UL);

    ChainFreeHdl(hdl, NULL);
    hdl = NULL;
}

class test_HoldAndTriggerRecv_suite : public ::testing::Test {
protected:
    test_HoldAndTriggerRecv_suite(): stub_(INVALID_ESPSTUB),
        tp_(NULL) {
        id_ = MAKE_PEID(kAppId, 0UL, 0UL);
    }
    virtual ~test_HoldAndTriggerRecv_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeIfStub(id_);
        stub_    = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);

        tp_ = new Transporter(id_, stub_, INVALID_ESPSTUB);
        //set queue size and start outflow
        ASSERT_EQ(tp_->set_data_strategy(DST_WHEN_QUEUE_FULL_HOLD), error::OK);
        ASSERT_EQ(tp_->set_queues("TestSendQueue", "Tests",
                                  kQueueSize,
                                  kQueueSize,
                                  kQueueSoft,
                                  kQueueSoft), error::OK);
        ASSERT_EQ(tp_->Start(), error::OK);

        //add outflow, sub all
        for (uint64_t pro_id = kDownProIdBegin;
             pro_id < kDownProIdBegin + kDonwOutflowNum; pro_id++) {
            uint64_t app_pro_id = MAKE_APP_PROCESSOR_ID(kAppId, pro_id);
            TPOfSourceVec* outflows = new TPOfSourceVec();
            std::vector<std::string> tags;
            transporter::OfMeta of_meta = {app_pro_id, 1UL, tags};
            outflows->push_back(of_meta);
            OfMgrCtx* ctx = new OfMgrCtx();
            ctx->outflows = outflows;
            tp_->_OnAddOutflow(ctx);
        }

    };
    virtual void TearDown() {
        SAFE_DELETE(tp_);
    };

public:
    static const uint64_t kAppId           = 1UL;
    static const uint64_t kDownProIdBegin  = 1UL;
    static const uint64_t kDonwOutflowNum;
    static const int      kQueueSize  = 2 * 1024;//2KB
    static const double   kQueueSoft  = 0.5f;

public:
    uint64_t     id_;
    ESPSTUB      stub_;
    Transporter* tp_;
};
const uint64_t test_HoldAndTriggerRecv_suite::kDonwOutflowNum = 2UL;

TEST_F(test_HoldAndTriggerRecv_suite, HoldTriggerRecv) {
    //feed data, expect pending recv
    BufHandle* hdl = NULL;
    int size  = (int)(kQueueSize * kQueueSoft);
    int batch = 0;
    ASSERT_EQ(FillTuplesAtLeastN(&hdl, &size, &batch), error::OK);
    ASSERT_GE(batch, (int)kDonwOutflowNum);//to feed hash parallisms

    ESPMessage esp_msg;
    TPDataMsg* data_msg;
    esp_msg.req.pHdl   = AllocateHdl(true);
    esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
    data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
    data_msg->from_id  = id_;
    data_msg->to_id    = id_;
    data_msg->size     = size;
    data_msg->batch    = batch;
    data_msg->ack      = 0;
    data_msg->err      = TPN_ACK_OK;
    esp_msg.req.pHdl->_next = hdl;

    ASSERT_EQ(tp_->m_full_queue_set.size(), 0UL);
    tp_->OnOutflowRecv(&esp_msg);
    ASSERT_EQ(tp_->m_full_queue_set.size(), kDonwOutflowNum);

    //feed data with last data, expect update pending time, lparam
    tp_->OnOutflowRecv(&esp_msg);
    ASSERT_EQ(tp_->m_full_queue_set.size(), kDonwOutflowNum);

    ChainFreeHdl(esp_msg.req.pHdl, NULL);
    esp_msg.req.pHdl = NULL;

    //fetch the first outflow's data, expect keep pending recv
    esp_msg.req.pHdl   = AllocateHdl(true);
    esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
    data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
    data_msg->from_id  = MAKE_PROCESSOR_PE_ID(MAKE_APP_PROCESSOR_ID(kAppId, kDownProIdBegin), 0UL);
    data_msg->to_id    = id_;
    data_msg->size     = kQueueSize < kMaxMsgSize ? kQueueSize : kMaxMsgSize;
    data_msg->batch    = 0;
    data_msg->ack      = 0;
    data_msg->err      = TPN_ACK_OK;
    //expect hold
    tp_->OnOutflowFetch(&esp_msg);

    //ack to flush data
    data_msg->ack      = 1;
    tp_->OnOutflowFetch(&esp_msg);
    ASSERT_EQ(tp_->m_full_queue_set.size(), kDonwOutflowNum - 1);

    ChainFreeHdl(esp_msg.req.pHdl, NULL);
    esp_msg.req.pHdl = NULL;

    //fetch data to trigger pending recv
    for (uint64_t pro_id = kDownProIdBegin + 1;//skip the first one
         pro_id < kDownProIdBegin + kDonwOutflowNum; pro_id++) {
        uint64_t app_pro_id = MAKE_APP_PROCESSOR_ID(kAppId, pro_id);
        uint64_t down_id    = MAKE_PROCESSOR_PE_ID(app_pro_id, 0UL);

        esp_msg.req.pHdl   = AllocateHdl(true);
        esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
        data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
        data_msg->from_id  = down_id;
        data_msg->to_id    = id_;
        data_msg->size     = kQueueSize < kMaxMsgSize ? kQueueSize : kMaxMsgSize;
        data_msg->batch    = 0;
        data_msg->ack      = 0;
        data_msg->err      = TPN_ACK_OK;
        //expect hold
        size_t full_queue = tp_->m_full_queue_set.size();
        tp_->OnOutflowFetch(&esp_msg);
        //ack to flush data
        data_msg->ack      = 1;
        tp_->OnOutflowFetch(&esp_msg);

        ASSERT_EQ(tp_->m_full_queue_set.size(), full_queue - 1);
        ChainFreeHdl(esp_msg.req.pHdl, NULL);
        esp_msg.req.pHdl = NULL;
    }
    ASSERT_EQ(tp_->m_full_queue_set.size(), 0UL);
    ASSERT_EQ(tp_->m_has_pending_recv, false);
}
