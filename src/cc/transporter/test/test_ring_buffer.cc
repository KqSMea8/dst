#include <gtest/gtest.h>
#include <list>
#include <time.h>
//#include <google/profiler.h>

#include "transporter/ring_buffer.h"

using namespace dstream;
using namespace dstream::ring_buffer;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_ring_buffer");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief
**/
class test_DataItem_DataItem_suite : public ::testing::Test {
protected:
    test_DataItem_DataItem_suite() {};
    virtual ~test_DataItem_DataItem_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DataItem_DataItem_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DataItem_DataItem_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DataItem_DataItem_suite, case_name1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_DataItem_DataItem_suite, case_name2)
{
    //TODO
}

/**
 * @brief
**/
class test_RingBuffer_RingBuffer_suite : public ::testing::Test {
protected:
    test_RingBuffer_RingBuffer_suite() {};
    virtual ~test_RingBuffer_RingBuffer_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RingBuffer_RingBuffer_suite, *)
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(99, ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RingBuffer_RingBuffer_suite, *)
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_RingBuffer_suite, QueueStatistic)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    ASSERT_EQ(ring_buffer_->ReadSize(), 0u);
    ASSERT_EQ(ring_buffer_->WriteSize(), 0u);
    ASSERT_EQ(ring_buffer_->Status(), error::OK);

}

/**
 * @brief
**/
class test_RingBuffer_Push_suite : public ::testing::Test {
protected:
    test_RingBuffer_Push_suite() {};
    virtual ~test_RingBuffer_Push_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RingBuffer_Push_suite, *)
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(99, ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RingBuffer_Push_suite, *)
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

CBufReadStream* NewReadStream(char* data, int len) {
    BufHandle h;
    InitBufHandle(&h);
    h.pBuf = data;
    h.nBufLen = len;
    h.nDataLen = len;
    return (new (std::nothrow) CBufReadStream(&h, NULL));
}

void DeleteReadStream(CBufReadStream* d) {
    if (d) {
        delete d;
        d = NULL;
    }
}

int FillData(uint8_t* data, int32_t len) {
    // each data 4 + 6
    assert(len % 10 == 0);
    int d_len = 6;
    int i = 0;
    for (; i < len / 10; ++i) {
        memcpy((char*)(data + i * 10), &d_len, sizeof(d_len));
        memset((char*)(data + i * 10 + sizeof(d_len)), i + 1, d_len);
    }
    return i;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_Push_suite, PushOK)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    for (int i = 1; i <= ring_buffer_->m_buffer_size / data_len; ++i) {
        CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
        ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
        DeleteReadStream(d1);
    }

    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, ring_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_TRUE(ring_buffer_->m_push_pos == data_len * (ring_buffer_->m_buffer_size / data_len));

    delete [] data;
}

TEST_F(test_RingBuffer_Push_suite, Watermark)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_len = 10;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    for (int i = 1; i <= ring_buffer_->m_buffer_size / data_len - 1; ++i) {
        CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
        ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
        DeleteReadStream(d1);
    }

    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK_SOFT_WATERMARK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_TRUE(ring_buffer_->m_push_pos == data_len * (ring_buffer_->m_buffer_size / data_len));

    delete [] data;
}


/**
 * @brief
**/
class test_RingBuffer_BatchPending_suite : public ::testing::Test {
protected:
    test_RingBuffer_BatchPending_suite() {};
    virtual ~test_RingBuffer_BatchPending_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(57, ptr);
    };
    virtual void TearDown() {
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_BatchPending_suite, PendingOK)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 20
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 20
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 23;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // get block 2, len = 10
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 17;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 10);
    ASSERT_EQ(batch, 1);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // set block 3, len = 20 buffer is too small to set data in
    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, ring_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    // get block 3, len = 10
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 10; // exactly fit size
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 10);
    ASSERT_EQ(batch, 1);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // check status
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_TRUE(ring_buffer_->IsEmpty());
    ASSERT_TRUE(ring_buffer_->m_push_pos == data_len * 2);

    delete [] data;
}


TEST_F(test_RingBuffer_BatchPending_suite, Repending)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 20
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 20
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // get block 1, len = 30
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 33;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 30);
    ASSERT_EQ(batch, 3);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // re-pending
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 33;
    batch = 0;
    uint64_t old_ack = ack;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 30);
    ASSERT_EQ(batch, 3);
    ASSERT_TRUE(ack == old_ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("repending get data block %lu", ack);
}

TEST_F(test_RingBuffer_BatchPending_suite, BadArguPending)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    // get block
    CBufWriteStream* wd = NULL;
    data_size = 23;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::BAD_ARGUMENT, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 23);
    ASSERT_EQ(batch, 0);

    wd = new (std::nothrow) CBufWriteStream();
    data_size = 0;
    // data size = 0, do nothing just return ok
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 0);
    ASSERT_EQ(batch, 0);
    SAFE_DELETE(wd);
}

/**
 * @brief
**/
class test_RingBuffer_BatchPop_suite : public ::testing::Test {
protected:
    test_RingBuffer_BatchPop_suite() {};
    virtual ~test_RingBuffer_BatchPop_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(100, ptr);
    };
    virtual void TearDown() {
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_BatchPop_suite, PopOK)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));
    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    // get block 2, len = 20
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    // set block 3, len = 40 buffer is too small to set data in
    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, ring_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    // check status
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 20 * 2);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 2u);

    // ack block 1
    ASSERT_EQ(error::OK, ring_buffer_->BatchPop(0/*not used*/));

    // set block 3 again, len = 40
    d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK_SOFT_WATERMARK, ring_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    // get block 2 again
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    uint64_t old_ack = ack;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack == old_ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) again ack(%lu)", ack);

    ASSERT_EQ(ring_buffer_->m_pending_pos, 20 * 2);

    // ack block 2
    ASSERT_EQ(error::OK, ring_buffer_->BatchPop(0/*not used*/));

    // check status
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_EQ(ring_buffer_->m_push_pos, 20);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 20 * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, ring_buffer_->m_pop_pos);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    // get block 3, len = 40
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 45;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 40);
    ASSERT_EQ(batch, 4);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(3) ack(%lu)", ack);

    // get block 4, len = 40
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 45;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 40);
    ASSERT_EQ(batch, 4);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(4) ack(%lu)", ack);

    // check status
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_TRUE(ring_buffer_->IsEmpty());
    ASSERT_EQ(ring_buffer_->m_push_pos, 20);
    ASSERT_EQ(ring_buffer_->m_pending_pos, ring_buffer_->m_push_pos);
    ASSERT_EQ(ring_buffer_->m_pop_pos, data_len);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 2u);

    // ack all pending data (block 3 & 4)
    ASSERT_EQ(error::OK, ring_buffer_->BatchPop(transporter::kFlushQueueUnack));

    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_TRUE(ring_buffer_->IsEmpty());
    ASSERT_EQ(ring_buffer_->m_push_pos, 20);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);
    ASSERT_EQ(ring_buffer_->m_pending_pos, ring_buffer_->m_push_pos);
    ASSERT_EQ(ring_buffer_->m_pop_pos, ring_buffer_->m_pending_pos);

    ASSERT_EQ(error::OK_NO_DATA_FLUSHED, ring_buffer_->BatchPop(transporter::kFlushQueueUnack));

    delete [] data;
}


TEST_F(test_RingBuffer_BatchPop_suite, PopNoPendingData)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));
    ASSERT_EQ(error::OK_NO_DATA_FLUSHED, ring_buffer_->BatchPop(123/*any number*/));
}

/**
 * @brief
**/
class test_RingBuffer_FlushPop_suite : public ::testing::Test {
protected:
    test_RingBuffer_FlushPop_suite() {};
    virtual ~test_RingBuffer_FlushPop_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(100, ptr);
    };
    virtual void TearDown() {
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

TEST_F(test_RingBuffer_FlushPop_suite, NoFlushData)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));
    int32_t expect_size = 123;
    ASSERT_EQ(ring_buffer_->FlushPop(&expect_size, NULL, NULL), error::OK);
    ASSERT_EQ(expect_size, 0);

    ASSERT_EQ(ring_buffer_->m_w_count, 0);
    ASSERT_EQ(ring_buffer_->m_r_count, 0);
}

TEST_F(test_RingBuffer_FlushPop_suite, FlushAllPending)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    // get block 2, len = 20
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    int32_t expect_size = 35;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(ring_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 40);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 40);
    ASSERT_EQ(ring_buffer_->m_pop_pos, ring_buffer_->m_pending_pos);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    ASSERT_EQ(ring_buffer_->m_w_count, 40);
    ASSERT_EQ(ring_buffer_->m_r_count, 40);
}

TEST_F(test_RingBuffer_FlushPop_suite, FlushAllPendingAndPartNoPending)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    // get block 2, len = 20
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    int32_t expect_size = 45;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(ring_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 50);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 50);
    ASSERT_EQ(ring_buffer_->m_pop_pos, ring_buffer_->m_pending_pos);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    ASSERT_EQ(ring_buffer_->m_w_count, 30);
    ASSERT_EQ(ring_buffer_->m_r_count, ring_buffer_->m_w_count);
}


TEST_F(test_RingBuffer_FlushPop_suite, NoPendingFlushAllData)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    int32_t expect_size = 90;
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(ring_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // there are only 2 block data
    ASSERT_EQ(expect_size, data_len * 2);
    ASSERT_EQ(batch, expect_size / 10);

    ASSERT_TRUE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, ring_buffer_->m_push_pos);
    ASSERT_EQ(ring_buffer_->m_pop_pos, ring_buffer_->m_push_pos);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    ASSERT_EQ(ring_buffer_->m_w_count, 0);
    ASSERT_EQ(ring_buffer_->m_r_count, 0);
}

TEST_F(test_RingBuffer_FlushPop_suite, FlushPartPending)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, ring_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 0);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 0u);

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    // get block 2, len = 20
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, ring_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    int32_t expect_size = 15;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(ring_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 20);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(ring_buffer_->IsEmpty());
    ASSERT_FALSE(ring_buffer_->IsFull());
    ASSERT_EQ(ring_buffer_->m_push_pos, data_len * 2);
    ASSERT_EQ(ring_buffer_->m_pending_pos, 40);
    ASSERT_EQ(ring_buffer_->m_pop_pos, 20);
    ASSERT_EQ(ring_buffer_->m_pending.size(), 1u);
}



/**
 * @brief
**/
class test_RingBuffer_Complex_suite : public ::testing::Test {
protected:
    test_RingBuffer_Complex_suite() {};
    virtual ~test_RingBuffer_Complex_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RingBuffer_Size_suite, *)
        srand(time(NULL));
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(1007, ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RingBuffer_Size_suite, *)
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

struct InputData {
    CBufReadStream* brs;
    int32_t size;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_Complex_suite, ComplexTest)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int data_num = 1000;
    uint8_t* data = new (std::nothrow) uint8_t[100];
    FillData(data, 100);

    std::list<InputData> pushQ;
    std::list<DataItem> pendingQ;
    std::list<DataItem> popQ;

    DSTREAM_INFO("ring_buffer status : %s", ring_buffer_->DumpStatus().c_str());

    int32_t total_batch = 0;
    uint64_t total_data_len = 0;
    for (int i = 0; i < data_num; ++i) {
        InputData d;
        d.size = 10 * (1 + rand() % 10);
        d.brs = NewReadStream((char*)data, d.size);
        total_data_len += d.size;
        pushQ.push_back(d);
    }
    DSTREAM_INFO("total data len(%lu)", total_data_len);

    while (pushQ.size() != 0
           || pendingQ.size() != 0
           || popQ.size() != 0) {
        int action = rand() % 3;
        switch (action) {
        case 0:
            // push data
            if (pushQ.size() > 0) {
                InputData d = pushQ.front();
                error::Code ret = ring_buffer_->BatchPush(d.brs, d.size);
                if (ret == error::OK || ret == error::OK_SOFT_WATERMARK) {
                    // DSTREAM_DEBUG("push data len(%lu) into ring_buffer", d.size);
                    // push ok
                    pushQ.pop_front();
                    DeleteReadStream(d.brs);
                    // random add some pending request
                    int rest = d.size / 10;
                    while (rest > 0) {
                        int p = rand() % rest + 1;
                        DataItem pd;
                        pd.d_len = 10 * p;
                        pendingQ.push_back(pd);
                        rest -= p;
                    }
                }
            }
            break;
        case 1:
            // pending data
            if (pendingQ.size() > 0) {
                DataItem expect_d = pendingQ.front();
                int32_t len = expect_d.d_len;
                int32_t batch = 0;
                uint64_t ack = transporter::kAdvanceQueueAck;
                CBufWriteStream bws;
                if (error::OK == ring_buffer_->BatchPending(&bws, &len, &batch, &ack)) {
                    // DSTREAM_DEBUG("pending data len(%lu) of ring_buffer", len);
                    // pending ok
                    pendingQ.pop_front();
                    expect_d.d_ptr = (uint8_t*)ack;
                    total_batch += batch;
                    ASSERT_EQ(len, expect_d.d_len);
                    popQ.push_back(expect_d);
                }
            }
            break;
        case 2:
            // pop data
            if (popQ.size() > 0) {
                DataItem d = popQ.front();
                if (popQ.size() % 2 == 0) {
                    ASSERT_EQ(error::OK, ring_buffer_->BatchPop((uint64_t)d.d_ptr));
                    // DSTREAM_DEBUG("ack data len(%lu) of ring_buffer", d.d_len);
                } else {
                    int32_t expect_size = d.d_len - 1;
                    ASSERT_EQ(error::OK, ring_buffer_->FlushPop(&expect_size, NULL, NULL));
                }
                popQ.pop_front();
            }
            break;
        }
    } // end of while
    DSTREAM_INFO("total data len(%lu), total batch(%d) num(%d)",
                 total_data_len, total_batch, data_num);
    delete [] data;

    DSTREAM_INFO("ring_buffer status : %s", ring_buffer_->DumpStatus().c_str());
}


/**
 * @brief
**/
class test_RingBuffer_Performance_suite : public ::testing::Test {
protected:
    test_RingBuffer_Performance_suite() {};
    virtual ~test_RingBuffer_Performance_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RingBuffer_Size_suite, *)
        srand(time(NULL));
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(8192, ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RingBuffer_Size_suite, *)
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_Performance_suite, Performance)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    int data_unit = 10;
    int data_num = 10000;
    uint8_t* data = new (std::nothrow) uint8_t[data_unit * 10];
    FillData(data, data_unit * 10);

    std::list<InputData> pushQ;
    std::list<DataItem> pendingQ;
    std::list<DataItem> popQ;

    int32_t total_batch = 0;
    uint64_t total_data_len = 0;
    for (int i = 0; i < data_num; ++i) {
        InputData d;
        d.size = 10 * (1 + rand() % data_unit);
        d.brs = NewReadStream((char*)data, d.size);
        total_data_len += d.size;
        pushQ.push_back(d);
    }
    DSTREAM_INFO("total data len(%lu)", total_data_len);

    clock_t total_time = 0;
    clock_t start_t = 0;
    while (pushQ.size() != 0
           || pendingQ.size() != 0
           || popQ.size() != 0) {
        int action = rand() % 3;
        switch (action) {
        case 0:
            // push data
            if (pushQ.size() > 0) {
                InputData d = pushQ.front();
                start_t = clock();
                error::Code ret = ring_buffer_->BatchPush(d.brs, d.size);
                if (ret == error::OK || ret == error::OK_SOFT_WATERMARK) {
                    total_time += clock() - start_t;
                    // push ok
                    pushQ.pop_front();
                    DeleteReadStream(d.brs);
                    // random add some pending request
                    int rest = d.size / 10;
                    while (rest > 0) {
                        int p = rand() % rest + 1;
                        DataItem pd;
                        pd.d_len = 10 * p;
                        pendingQ.push_back(pd);
                        rest -= p;
                    }
                }
            }
            break;
        case 1:
            // pending data
            if (pendingQ.size() > 0) {
                DataItem expect_d = pendingQ.front();
                int32_t len = expect_d.d_len;
                int32_t batch = 0;
                uint64_t ack = transporter::kAdvanceQueueAck;
                CBufWriteStream bws;
                start_t = clock();
                if (error::OK == ring_buffer_->BatchPending(&bws, &len, &batch, &ack)) {
                    total_time += clock() - start_t;
                    // pending ok
                    pendingQ.pop_front();
                    expect_d.d_ptr = (uint8_t*)ack;
                    total_batch += batch;
                    popQ.push_back(expect_d);
                }
            }
            break;
        case 2:
            // pop data
            if (popQ.size() > 0) {
                DataItem d = popQ.front();
                popQ.pop_front();
                start_t = clock();
                ring_buffer_->BatchPop((uint64_t)d.d_ptr);
                total_time += clock() - start_t;
            }
            break;
        }
    } // end of while
    DSTREAM_INFO("total data len(%lu), total batch(%d) num(%d), elapse time(%fs), QPS(%f)",
                 total_data_len, total_batch, data_num,
                 total_time * 1.0 / CLOCKS_PER_SEC,
                 data_num / (total_time * 1.0 / CLOCKS_PER_SEC));
    delete [] data;

}

/**
 * @brief
**/
class test_RingBuffer_Profile_suite : public ::testing::Test {
protected:
    test_RingBuffer_Profile_suite() {};
    virtual ~test_RingBuffer_Profile_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RingBuffer_Size_suite, *)
        srand(time(NULL));
        mem_mgr::MemMgr::MemMgrPtr ptr;
        ring_buffer_ = new (std::nothrow) RingBuffer(8192 * 81920, ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RingBuffer_Size_suite, *)
        delete ring_buffer_;
    };
private:
    RingBuffer* ring_buffer_;
};

int FillLargeData(uint8_t* data, int32_t len, int32_t unit_size) {
    // each data 4 + unit_size
    assert(len % (unit_size + 4) == 0);
    int d_len = unit_size;
    int i = 0;
    for (; i < len / (unit_size + 4); ++i) {
        memcpy((char*)(data + i * (unit_size + sizeof(d_len))), &d_len, sizeof(d_len));
        memset((char*)(data + i * (unit_size + sizeof(d_len)) + sizeof(d_len)), '#', d_len);
    }
    return i;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RingBuffer_Profile_suite, Profiling)
{
    ASSERT_TRUE(ring_buffer_ != static_cast<RingBuffer*>(NULL));

    error::Code ret = error::OK;
    const int unit_batch = 500;
    const int unit_size = 1000;
    const int data_num = 10000;
    uint8_t* data = new (std::nothrow) uint8_t[unit_batch * (unit_size + sizeof(int))];
    FillLargeData(data, unit_batch * (unit_size + sizeof(int)), unit_size);

    uint64_t total_data_len = 0;
    uint64_t data_count = 0;
    //  ProfilerStart("ring_pf");
    clock_t start_t = clock();
    // push data
    for (int i = 0; i < data_num; ++i) {
        InputData d;
        d.size = (unit_size + sizeof(int)) * (unit_batch - 100 + rand() % 100);
        d.brs = NewReadStream((char*)data, d.size);
        total_data_len += d.size;
        ret = ring_buffer_->BatchPush(d.brs, d.size);
        // DSTREAM_INFO("push(size:%d) ret = %d\n", d.size, ret);
    }
    DSTREAM_INFO("%s total data len(%lu)", ring_buffer_->DumpStatus().c_str(), total_data_len);

    int pop_total_len = 0;
    while (!ring_buffer_->IsEmpty()) {
        int32_t len = (unit_size + sizeof(int)) * (unit_batch - 100 + rand() % 100);
        int32_t batch = 0;
        uint64_t ack = transporter::kAdvanceQueueAck;
        CBufWriteStream bws;
        ret = ring_buffer_->BatchPending(&bws, &len, &batch, &ack);
        // DSTREAM_INFO("pending(%d,%d) ret = %d\n", len, batch, ret);
        ret = ring_buffer_->BatchPop(ack);
        // DSTREAM_INFO("pop ret = %d\n", ret);
        data_count += batch;
        pop_total_len += len;
        // DSTREAM_INFO("%s pop len: %d\n",
        //             ring_buffer_->DumpStatus().c_str(), pop_total_len);
    }
    clock_t total_time = clock() - start_t;
    DSTREAM_INFO("total data len(%lu), total count(%lu), elapse time(%fs), QPS(%f), DPS(%f)",
                 total_data_len, data_count, total_time * 1.0 / CLOCKS_PER_SEC,
                 data_count / (total_time * 1.0 / CLOCKS_PER_SEC),
                 total_data_len / (total_time * 1.0 / CLOCKS_PER_SEC));

    //  ProfilerFlush();
    //  ProfilerStop();

}
