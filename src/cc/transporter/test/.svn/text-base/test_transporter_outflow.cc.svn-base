#include <gtest/gtest.h>

#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_outflow.h"

using namespace dstream;
using namespace dstream::transporter;

int main(int argc, char** argv) {
    logger::initialize("pn_test");
    common::Counters::Init();
    ::testing::InitGoogleTest(&argc, argv);
    //::testing::AddGlobalTestEnvironment(new TransporterEnv());
    int ret = RUN_ALL_TESTS();
    common::Counters::Deinit();
    return ret;
}

class test_PopAgedTuples_suite : public ::testing::Test {
protected:
    test_PopAgedTuples_suite(): id_(kId), owner_id_(kOwnerId), outflow_(NULL) {};
    virtual ~test_PopAgedTuples_suite() {};
    virtual void SetUp() {
        outflow_ = new TPOutflow(id_, owner_id_, 1UL);
        ASSERT_TRUE(NULL != outflow_);
        //set data stategy
        ASSERT_EQ(outflow_->set_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
                  error::OK);
        //set queue size and start outflow
        ASSERT_EQ(outflow_->set_queues("TestSendQueue", "Tests",
                                       kQueueSize, kQueueSoft), error::OK);
        ASSERT_EQ(outflow_->Start(), error::OK);
    };
    virtual void TearDown() {
        SAFE_DELETE(outflow_);
    };

public:
    static const uint64_t kId        = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kOwnerId   = MAKE_PEID(1UL, 2UL, 0UL);
    static const uint64_t kQueueSize = 1024;
    static const double   kQueueSoft = 0.9f;

public:
    uint64_t   id_;
    uint64_t   owner_id_;
    TPOutflow* outflow_;
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
    CBufReadStream brs1(hdl, NULL);

    TPAckQueue& queue = *outflow_->m_branches[0UL].queue;
    ASSERT_GE(queue.PushBack(&brs1, size), error::OK);

    ChainFreeHdl(hdl, NULL);
    hdl = NULL;

    //flush more tuples tuples to triggle flush aged tupled
    for (int i = 0; i < 2; ++i) {
        ASSERT_EQ(FillTuplesAtMostN(&hdl, &size, &batch), error::OK) << "i=" << i;
        CBufReadStream brs2(hdl, NULL);
        ASSERT_GE(outflow_->OnRecvData(transporter::TPN_ACK_OK, 0UL, size, batch, brs2),
                  error::OK) << "i=" << i;

        ChainFreeHdl(hdl, NULL);
        hdl = NULL;
    }
}

class test_OnUpdParall_suite : public ::testing::Test {
protected:
    test_OnUpdParall_suite(): id_(kId), owner_id_(kOwnerId), outflow_(NULL) {};
    virtual ~test_OnUpdParall_suite() {};
    virtual void SetUp() {
        outflow_ = new TPOutflow(id_, owner_id_, 1UL);
        ASSERT_TRUE(NULL != outflow_);
        //set data stategy
        ASSERT_EQ(outflow_->set_data_strategy(DST_WHEN_QUEUE_FULL_DROP_AGELONG),
                  error::OK);
        //set queue size and start outflow
        ASSERT_EQ(outflow_->set_queues("TestSendQueue", "Tests",
                                       kQueueSize, kQueueSoft), error::OK);
        ASSERT_EQ(outflow_->Start(), error::OK);
    };
    virtual void TearDown() {
        SAFE_DELETE(outflow_);
    };

public:
    static const uint64_t kId        = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kOwnerId   = MAKE_PEID(1UL, 2UL, 0UL);
    static const uint64_t kQueueSize = 1024;
    static const double   kQueueSoft = 0.9f;

public:
    uint64_t   id_;
    uint64_t   owner_id_;
    TPOutflow* outflow_;
}; 

TEST_F(test_OnUpdParall_suite, PopAgedTuples) {
    ASSERT_EQ(outflow_->m_parall, 1UL);

    outflow_->OnUpdParall(10);
    ASSERT_EQ(outflow_->m_parall, 10UL);

    outflow_->OnUpdParall(3);
    ASSERT_EQ(outflow_->m_parall, 3UL);
}
