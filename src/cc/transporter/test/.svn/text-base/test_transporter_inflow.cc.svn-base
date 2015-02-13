#include <gtest/gtest.h>

#include <stdlib.h>
#include "Random.h"
#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_inflow.h"

using namespace dstream;
using namespace dstream::transporter;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}

typedef std::vector<transporter::IfSourceMeta> TPIfSourceVec;
typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

class test_PopAgedTuples_suite : public ::testing::Test {
public:
    static const uint64_t kId        = MAKE_PEID(1UL, 2UL, 0UL);
    static const uint64_t kUpId      = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kAck       = 1;
    static const uint64_t kQueueSize = 1024;
    static const double   kQueueSoft = 0.9f;

protected:
    test_PopAgedTuples_suite(): id_(kId), upid_(kUpId), inflow_(NULL),
        source_(NULL), fake_owner_(NULL) {};
    virtual ~test_PopAgedTuples_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeIfStub(id_);
        stub_    = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);

        //start inflow
        fake_owner_ = new CESPObject();
        ASSERT_TRUE(NULL != fake_owner_);
        inflow_ = new (std::nothrow) transporter::TPInflow(fake_owner_, id_, stub_);
        ASSERT_TRUE(NULL != inflow_);

        //set data stategy
        ASSERT_EQ(inflow_->set_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
                  error::OK);
        //set queue size and start outflow
        ASSERT_EQ(inflow_->set_queues("TestSendQueue", "Tests",
                                      kQueueSize, kQueueSoft), error::OK);
        ASSERT_EQ(inflow_->Start(), error::OK);

        //add source
        TPIfSourceVec sources;
        transporter::IfSourceMeta if_meta = {upid_, INVALID_ESPNODE};
        sources.push_back(if_meta);
        ASSERT_EQ(inflow_->AddSource(sources, NULL), error::OK);
        //update source status
        TPInflow::TPSourceMap::iterator it = inflow_->m_sources.find(upid_);
        ASSERT_TRUE(it != inflow_->m_sources.end());
        source_ = it->second;
        ASSERT_TRUE(NULL != source_);
        source_->status = TPInflow::SOURCE_OK;
        source_->ack    = kAck;
    };
    virtual void TearDown() {
        if (NULL != inflow_) {
            inflow_->Stop();
            inflow_->Release();
        }
        if (NULL != fake_owner_) {
            delete fake_owner_;
        }
        inflow_ = NULL;
        fake_owner_ = NULL;
    };

public:
    uint64_t              id_;
    uint64_t              upid_;
    TPInflow*             inflow_;
    ESPSTUB               stub_;
    TPInflow::TPSource*   source_;
    CESPObject*           fake_owner_;
};

TEST_F(test_PopAgedTuples_suite, PopAgedTuples) {
    //make sure soft-watermark is appropriate
    ASSERT_GT(kQueueSize * kQueueSoft, 1);

    //fill tuples
    BufHandle* hdl = NULL;
    int size  = kQueueSize - 1;//to void get queue full
    int batch = 0;
    ASSERT_EQ(FillTuplesAtMostN(&hdl, &size, &batch), error::OK);

    //push tuples into queue
    CBufReadStream brs(hdl, NULL);

    TPAckQueue& queue = *inflow_->m_queue;
    ASSERT_GE(queue.PushBack(&brs, size), error::OK);

    ChainFreeHdl(hdl, NULL);
    hdl = NULL;

    //flush more tuples tuples to triggle flush aged tupled
    for (int i = 0; i < 2; ++i) {
        ASSERT_EQ(FillTuplesAtMostN(&hdl, &size, &batch), error::OK) << "i=" << i;
        CBufReadStream brs1(hdl, NULL);
        ASSERT_GE(inflow_->_OnSourceRecv(source_, TPN_ACK_OK, kAck + i,
                                         &brs1, size, batch),
                  error::OK) << "i=" << i;

        ChainFreeHdl(hdl, NULL);
        hdl = NULL;
    }
}

class test_HoldFetch_suite : public ::testing::Test {
protected:
    test_HoldFetch_suite(): id_(kId), upid_(kUpId), inflow_(NULL),
        source_(NULL), fake_owner_(NULL) {};
    virtual ~test_HoldFetch_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeIfStub(id_);
        stub_    = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);

        //start inflow
        fake_owner_ = new CESPObject();
        ASSERT_TRUE(NULL != fake_owner_);
        inflow_ = new (std::nothrow) TPInflow(fake_owner_, id_, stub_);
        ASSERT_TRUE(NULL != inflow_);

        ////set data stategy
        //ASSERT_EQ(inflow_->SetDataStrategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
        //          error::OK);
        //set queue size and start outflow
        ASSERT_EQ(inflow_->set_queues("TestSendQueue", "Tests",
                                      kQueueSize, kQueueSoft), error::OK);
        ASSERT_EQ(inflow_->Start(), error::OK);

        //add source
        TPIfSourceVec sources;
        transporter::IfSourceMeta if_meta = {upid_, INVALID_ESPNODE};
        sources.push_back(if_meta);
        ASSERT_EQ(inflow_->AddSource(sources, NULL), error::OK);
        //update source status
        TPInflow::TPSourceMap::iterator it = inflow_->m_sources.find(upid_);
        ASSERT_TRUE(it != inflow_->m_sources.end());
        source_ = it->second;
        ASSERT_TRUE(NULL != source_);
        source_->status = TPInflow::SOURCE_OK;
        source_->ack    = kAck;
    };
    virtual void TearDown() {
        if (NULL != inflow_) {
            inflow_->Stop();
            inflow_->Release();
        }
        if (NULL != fake_owner_) {
            delete fake_owner_;
        }
        inflow_ = NULL;
        fake_owner_ = NULL;
    };

public:
    static const uint64_t kId        = MAKE_PEID(1UL, 2UL, 0UL);
    static const uint64_t kUpId      = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kAck       = 1;
    static const uint64_t kQueueSize = 1024;
    static const double   kQueueSoft = 0.9f;

public:
    uint64_t              id_;
    uint64_t              upid_;
    TPInflow*             inflow_;
    ESPSTUB               stub_;
    TPInflow::TPSource*   source_;
    CESPObject*           fake_owner_;
};

TEST_F(test_HoldFetch_suite, HoldFetch) {
    TPAckQueue& queue = *inflow_->m_queue;
    ASSERT_TRUE(queue.Empty());
    //mock fetch
    ESPMessage esp_msg;
    TPDataMsg* data_msg;
    esp_msg.req.pHdl = AllocateHdl(true);
    esp_msg.req.pHdl->nDataLen = sizeof(TPDataMsg);
    data_msg           = new(esp_msg.req.pHdl->pBuf) TPDataMsg();
    data_msg->from_id  = kId;
    data_msg->to_id    = kId;
    data_msg->size     = kQueueSize / 2;
    data_msg->batch    = 0;
    data_msg->ack      = 0;
    data_msg->err      = TPN_ACK_OK;
    //expect hold, without posgmsg
    inflow_->OnFetchData(&esp_msg);
    ASSERT_EQ(inflow_->m_has_pending_fetch, 1);

    //fill tuples into queue;
    int32_t size  = (int32_t)kQueueSize / 2;
    int32_t batch = 0;
    BufHandle* hdl = NULL;
    ASSERT_EQ(FillTuplesAtMostN(&hdl, &size, &batch), error::OK);
    CBufReadStream brs(hdl, NULL);
    ASSERT_GE(queue.PushBack(&brs, size), error::OK);
    ChainFreeHdl(hdl, NULL);
    hdl = NULL;

    //fetch, expect triggle OK, postmsg
    inflow_->_OnTriggerPendingFetch();

    ASSERT_EQ(inflow_->m_has_pending_fetch, 0);
    ASSERT_TRUE(queue.Empty());
}
