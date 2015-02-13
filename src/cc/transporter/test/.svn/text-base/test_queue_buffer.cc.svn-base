#include <gtest/gtest.h>
#include <list>
#include <time.h>
//#include <google/profiler.h>

#include "transporter/queue_buffer.h"

using namespace dstream;
using namespace dstream::queue_buffer;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_queue_buffer");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_QueueBuffer_QueueBuffer_suite : public ::testing::Test {
protected:
    test_QueueBuffer_QueueBuffer_suite() {};
    virtual ~test_QueueBuffer_QueueBuffer_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_QueueBuffer_QueueBuffer_suite, *)
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(99, w_ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_QueueBuffer_QueueBuffer_suite, *)
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_QueueBuffer_suite, QueueStatistic)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    ASSERT_EQ(queue_buffer_->ReadSize(), 0u);
    ASSERT_EQ(queue_buffer_->WriteSize(), 0u);
    ASSERT_EQ(queue_buffer_->Status(), error::OK);

}

/**
 * @brief
**/
class test_QueueBuffer_Push_suite : public ::testing::Test {
protected:
    test_QueueBuffer_Push_suite() {};
    virtual ~test_QueueBuffer_Push_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_QueueBuffer_Push_suite, *)
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(99, w_ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_QueueBuffer_Push_suite, *)
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
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
TEST_F(test_QueueBuffer_Push_suite, PushOK)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    for (uint64_t i = 1; i <= queue_buffer_->m_buffer_size / data_len; ++i) {
        CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
        ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
        DeleteReadStream(d1);
    }

    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, queue_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_FALSE(queue_buffer_->IsEmpty());

    delete [] data;
}

TEST_F(test_QueueBuffer_Push_suite, Watermark)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_len = 10;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    for (uint64_t i = 1; i <= queue_buffer_->m_buffer_size / data_len - 1; ++i) {
        CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
        ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
        DeleteReadStream(d1);
    }

    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK_SOFT_WATERMARK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_FALSE(queue_buffer_->IsEmpty());

    delete [] data;
}


/**
 * @brief
**/
class test_QueueBuffer_BatchPending_suite : public ::testing::Test {
protected:
    test_QueueBuffer_BatchPending_suite() {};
    virtual ~test_QueueBuffer_BatchPending_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(57, w_ptr);
    };
    virtual void TearDown() {
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_BatchPending_suite, PendingOK)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 20
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 20
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    std::cout << queue_buffer_->DumpStatus() << std::endl;

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 23;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
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
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 10);
    ASSERT_EQ(batch, 1);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // set block 3, len = 20 buffer is too small to set data in
    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, queue_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    // get block 3, len = 10
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 10; // exactly fit size
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 10);
    ASSERT_EQ(batch, 1);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block %lu", ack);

    // check status
    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_TRUE(queue_buffer_->IsEmpty());

    delete [] data;
}


TEST_F(test_QueueBuffer_BatchPending_suite, Repending)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 20;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 20
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 20
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // get block 1, len = 30
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 33;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
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
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 30);
    ASSERT_EQ(batch, 3);
    ASSERT_TRUE(ack == old_ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("repending get data block %lu", ack);
}

TEST_F(test_QueueBuffer_BatchPending_suite, BadArguPending)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    // get block
    CBufWriteStream* wd = NULL;
    data_size = 23;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::BAD_ARGUMENT, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 23);
    ASSERT_EQ(batch, 0);

    wd = new (std::nothrow) CBufWriteStream();
    data_size = 0;
    // data size = 0, do nothing just return ok
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 0);
    ASSERT_EQ(batch, 0);
    SAFE_DELETE(wd);
}

/**
 * @brief
**/
class test_QueueBuffer_BatchPop_suite : public ::testing::Test {
protected:
    test_QueueBuffer_BatchPop_suite() {};
    virtual ~test_QueueBuffer_BatchPop_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(100, w_ptr);
    };
    virtual void TearDown() {
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_BatchPop_suite, PopOK)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));
    int32_t data_size = 0;
    int32_t batch = 0;
    uint64_t ack = 0;

    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    // get block 1, len = 30
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 39;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 30);
    ASSERT_EQ(batch, 3);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    // set block 3, len = 40 buffer is too small to set data in
    CBufReadStream* d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::QUEUE_FULL, queue_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    // check status
    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_FALSE(queue_buffer_->IsEmpty());

    // ack block 1
    ASSERT_EQ(error::OK, queue_buffer_->BatchPop(transporter::kFlushQueueUnack));

    // set block 3 again, len = 40
    d3 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK_SOFT_WATERMARK, queue_buffer_->BatchPush(d3, data_len));
    DeleteReadStream(d3);

    std::cout << queue_buffer_->DumpStatus() << std::endl;

    // get block 2
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_TRUE(ack != transporter::kAdvanceQueueAck);
    uint64_t old_ack = ack;
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack != transporter::kAdvanceQueueAck);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    // get block 2
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 20;
    batch = 0;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_TRUE(ack != transporter::kAdvanceQueueAck);
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack == old_ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) again ack(%lu)", ack);

    // ack block 2
    ASSERT_EQ(error::OK, queue_buffer_->BatchPop(ack));

    // check status
    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_FALSE(queue_buffer_->IsEmpty());

    // get block 3, len = 40
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 45;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 40);
    ASSERT_EQ(batch, 4);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(3) ack(%lu)", ack);

    // ack all pending data (block 3)
    ASSERT_EQ(error::OK, queue_buffer_->BatchPop(transporter::kFlushQueueUnack));

    // get block 4, len = 40
    wd = new (std::nothrow) CBufWriteStream();
    data_size = 45;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 30);
    ASSERT_EQ(batch, 3);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(4) ack(%lu)", ack);

    // check status
    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_TRUE(queue_buffer_->IsEmpty());

    // ack pending data (block 4)
    ASSERT_EQ(error::OK, queue_buffer_->BatchPop(transporter::kFlushQueueUnack));

    ASSERT_FALSE(queue_buffer_->IsFull());
    ASSERT_TRUE(queue_buffer_->IsEmpty());

    ASSERT_EQ(error::OK_NO_DATA_FLUSHED, queue_buffer_->BatchPop(transporter::kFlushQueueUnack));

    delete [] data;
}


TEST_F(test_QueueBuffer_BatchPop_suite, PopNoPendingData)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));
    ASSERT_EQ(error::OK_NO_DATA_FLUSHED, queue_buffer_->BatchPop(123/*any number*/));
}

/**
 * @brief
**/
class test_QueueBuffer_FlushPop_suite : public ::testing::Test {
protected:
    test_QueueBuffer_FlushPop_suite() {};
    virtual ~test_QueueBuffer_FlushPop_suite() {};
    virtual void SetUp() {
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(100, w_ptr);
    };
    virtual void TearDown() {
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

TEST_F(test_QueueBuffer_FlushPop_suite, NoFlushData)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));
    int32_t expect_size = 123;
    ASSERT_EQ(queue_buffer_->FlushPop(&expect_size, NULL, NULL), error::OK);
    ASSERT_EQ(expect_size, 0);

    ASSERT_EQ(queue_buffer_->m_w_count, 0u);
    ASSERT_EQ(queue_buffer_->m_r_count, 0u);
}

TEST_F(test_QueueBuffer_FlushPop_suite, FlushAllPending)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
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
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    int32_t expect_size = 35;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(queue_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 40);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    ASSERT_EQ(queue_buffer_->m_w_count, 40u);
    ASSERT_EQ(queue_buffer_->m_r_count, 40u);
}

TEST_F(test_QueueBuffer_FlushPop_suite, FlushAllPendingAndPartNoPending)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
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
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(2) ack(%lu)", ack);

    int32_t expect_size = 45;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(queue_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 50);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    ASSERT_EQ(queue_buffer_->m_w_count, 30u);
    ASSERT_EQ(queue_buffer_->m_r_count, queue_buffer_->m_w_count);
}


TEST_F(test_QueueBuffer_FlushPop_suite, NoPendingFlushAllData)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);

    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    int32_t expect_size = 90;
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(queue_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // there are only 2 block data
    ASSERT_EQ(expect_size, data_len * 2);
    ASSERT_EQ(batch, expect_size / 10);

    ASSERT_TRUE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    ASSERT_EQ(queue_buffer_->m_w_count, 0u);
    ASSERT_EQ(queue_buffer_->m_r_count, 0u);
}

TEST_F(test_QueueBuffer_FlushPop_suite, FlushPartPending)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int32_t data_size = 0;
    uint64_t ack = 0;
    int32_t batch = 0;
    int32_t data_len = 40;
    uint8_t* data = new (std::nothrow) uint8_t[data_len];
    FillData(data, data_len);
    // set block 1, len = 40
    CBufReadStream* d1 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d1, data_len));
    DeleteReadStream(d1);
    // set block 2, len = 40
    CBufReadStream* d2 = NewReadStream((char*)data, (int)data_len);
    ASSERT_EQ(error::OK, queue_buffer_->BatchPush(d2, data_len));
    DeleteReadStream(d2);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());

    // get block 1, len = 20
    CBufWriteStream* wd = new (std::nothrow) CBufWriteStream();
    data_size = 29;
    batch = 0;
    ack = transporter::kAdvanceQueueAck;
    ASSERT_EQ(error::OK, queue_buffer_->BatchPending(wd, &data_size, &batch, &ack));
    ASSERT_EQ(data_size, 20);
    ASSERT_EQ(batch, 2);
    ASSERT_TRUE(ack);
    SAFE_DELETE(wd);
    DSTREAM_INFO("get data block(1) ack(%lu)", ack);

    int32_t expect_size = 15;
    wd = new (std::nothrow) CBufWriteStream();
    batch = 0;
    ASSERT_EQ(queue_buffer_->FlushPop(&expect_size, wd, &batch), error::OK);
    SAFE_DELETE(wd);
    // flush 3 block data(2 pending, 1 no pending)
    ASSERT_EQ(expect_size, 20);
    ASSERT_EQ(batch, expect_size / 10);

    // check status
    ASSERT_FALSE(queue_buffer_->IsEmpty());
    ASSERT_FALSE(queue_buffer_->IsFull());
}

struct DataItemAck : public DataItem {
    int64_t ack;
};

/**
 * @brief
**/
class test_QueueBuffer_Complex_suite : public ::testing::Test {
protected:
    test_QueueBuffer_Complex_suite() {};
    virtual ~test_QueueBuffer_Complex_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_QueueBuffer_Size_suite, *)
        srand(time(NULL));
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(1007, w_ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_QueueBuffer_Size_suite, *)
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

struct InputData {
    CBufReadStream* brs;
    int32_t size;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_Complex_suite, ComplexTest)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int data_num = 1000;
    uint8_t* data = new (std::nothrow) uint8_t[100];
    FillData(data, 100);

    std::list<InputData> pushQ;
    std::list<DataItemAck> pendingQ;
    std::list<DataItemAck> popQ;

    DSTREAM_INFO("queue_buffer status : %s", queue_buffer_->DumpStatus().c_str());

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
                error::Code ret = queue_buffer_->BatchPush(d.brs, d.size);
                if (ret == error::OK || ret == error::OK_SOFT_WATERMARK) {
                    // DSTREAM_DEBUG("push data len(%lu) into queue_buffer", d.size);
                    // push ok
                    pushQ.pop_front();
                    DeleteReadStream(d.brs);
                    // random add some pending request
                    int rest = d.size / 10;
                    while (rest > 0) {
                        int p = rand() % rest + 1;
                        DataItemAck pd;
                        pd.d_len = 10 * p;
                        pendingQ.push_back(pd);
                        rest -= p;
                    }
                }
            }
            break;
        case 1:
            // pending data
            if (pendingQ.size() > 0 && !queue_buffer_->HasPending()) {
                DataItemAck expect_d = pendingQ.front();
                int32_t len = expect_d.d_len;
                int32_t batch = 0;
                uint64_t ack = transporter::kAdvanceQueueAck;
                CBufWriteStream bws;
                if (error::OK == queue_buffer_->BatchPending(&bws, &len, &batch, &ack)) {
                    // DSTREAM_DEBUG("pending data len(%lu) of queue_buffer", len);
                    // pending ok
                    pendingQ.pop_front();
                    expect_d.ack = ack;
                    total_batch += batch;
                    ASSERT_EQ(len, expect_d.d_len);
                    popQ.push_back(expect_d);
                }
            }
            break;
        case 2:
            // pop data
            if (popQ.size() > 0 && queue_buffer_->HasPending()) {
                DataItemAck d = popQ.front();
                if (popQ.size() % 2 == 0) {
                    ASSERT_EQ(error::OK, queue_buffer_->BatchPop(d.ack));
                    // DSTREAM_DEBUG("ack data len(%lu) of queue_buffer", d.d_len);
                } else {
                    int32_t expect_size = d.d_len - 1;
                    ASSERT_EQ(error::OK, queue_buffer_->FlushPop(&expect_size, NULL, NULL));
                }
                popQ.pop_front();
            }
            break;
        }
    } // end of while
    DSTREAM_INFO("total data len(%lu), total batch(%d) num(%d)",
                 total_data_len, total_batch, data_num);
    delete [] data;

    DSTREAM_INFO("queue_buffer status : %s", queue_buffer_->DumpStatus().c_str());
}


/**
 * @brief
**/
class test_QueueBuffer_Performance_suite : public ::testing::Test {
protected:
    test_QueueBuffer_Performance_suite() {};
    virtual ~test_QueueBuffer_Performance_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_QueueBuffer_Size_suite, *)
        srand(time(NULL));
        mem_mgr::MemMgr::MemMgrPtr w_ptr;
        queue_buffer_ = new (std::nothrow) QueueBuffer(8192, w_ptr);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_QueueBuffer_Size_suite, *)
        delete queue_buffer_;
    };
private:
    QueueBuffer* queue_buffer_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_Performance_suite, Performance)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    int data_unit = 10;
    int data_num = 1000;
    uint8_t* data = new (std::nothrow) uint8_t[data_unit * 10];
    FillData(data, data_unit * 10);

    std::list<InputData> pushQ;
    std::list<DataItemAck> pendingQ;
    std::list<DataItemAck> popQ;

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
    clock_t push_total_time = 0;
    clock_t pop_total_time = 0;
    clock_t pending_total_time = 0;
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
                error::Code ret = queue_buffer_->BatchPush(d.brs, d.size);
                if (ret == error::OK || ret == error::OK_SOFT_WATERMARK) {
                    push_total_time += clock() - start_t;
                    // push ok
                    pushQ.pop_front();
                    DeleteReadStream(d.brs);
                    // add only one pending request
                    int rest = d.size / 10;
                    while (rest > 0) {
                        int p = rand() % rest + 1;
                        DataItemAck pd;
                        pd.d_len = 10 * p;
                        pendingQ.push_back(pd);
                        rest -= p;
                    }
                }
            }
            break;
        case 1:
            // pending data
            if (pendingQ.size() > 0 && !queue_buffer_->HasPending()) {
                DataItemAck expect_d = pendingQ.front();
                int32_t len = expect_d.d_len;
                int32_t batch = 0;
                uint64_t ack = transporter::kAdvanceQueueAck;
                CBufWriteStream bws;
                start_t = clock();
                error::Code ret = queue_buffer_->BatchPending(&bws, &len, &batch, &ack);
                if (error::OK == ret) {
                    pending_total_time += clock() - start_t;
                    // pending ok
                    pendingQ.pop_front();
                    expect_d.ack = ack;
                    total_batch += batch;
                    popQ.push_back(expect_d);
                }
            }
            break;
        case 2:
            // pop data
            if (popQ.size() > 0 && queue_buffer_->HasPending()) {
                DataItemAck d = popQ.front();
                popQ.pop_front();
                start_t = clock();
                queue_buffer_->BatchPop(d.ack);
                pop_total_time += clock() - start_t;
            }
            break;
        }
    } // end of while
    total_time = pop_total_time + push_total_time + pending_total_time;
    DSTREAM_INFO("total data len(%lu), total batch(%d) num(%d), elapse time(%fs), pusht(%fs), popt(%fs), pendingt(%fs), QPS(%f)",
                 total_data_len, total_batch, data_num,
                 total_time * 1.0 / CLOCKS_PER_SEC,
                 push_total_time * 1.0 / CLOCKS_PER_SEC,
                 pop_total_time * 1.0 / CLOCKS_PER_SEC,
                 pending_total_time * 1.0 / CLOCKS_PER_SEC,
                 data_num / (total_time * 1.0 / CLOCKS_PER_SEC));
    delete [] data;

}

/**
 * @brief
**/
class test_QueueBuffer_Profile_suite : public ::testing::Test {
protected:
    test_QueueBuffer_Profile_suite() {};
    virtual ~test_QueueBuffer_Profile_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_QueueBuffer_Size_suite, *)
        srand(time(NULL));
        ptr_ = mem_mgr::MemMgr::Instance();
        ASSERT_EQ(ptr_->Init(10000000000), error::OK);
        queue_buffer_ = new (std::nothrow) QueueBuffer(8192000, ptr_);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_QueueBuffer_Size_suite, *)
        delete queue_buffer_;
    };
    int FillLargeData(uint8_t* data, int32_t len, int32_t unit_size);
    clock_t RunTestLoop(uint8_t* data_blk, int data_num, int unit_batch, int unit_size,
                        uint64_t& pop_batch, uint64_t& pop_len);
private:
    QueueBuffer* queue_buffer_;
    mem_mgr::MemMgr::MemMgrPtr ptr_;
};

int test_QueueBuffer_Profile_suite::FillLargeData(uint8_t* data, int32_t len, int32_t unit_size) {
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

clock_t test_QueueBuffer_Profile_suite::RunTestLoop(uint8_t* data,
        int data_num,
        int unit_batch,
        int unit_size,
        uint64_t& pop_batch,
        uint64_t& pop_len) {
    error::Code ret = error::OK;
    uint64_t total_data_len = 0;
    uint64_t data_count = 0;
    clock_t start_t = clock();
    // push data
    for (int i = 0; i < data_num; ++i) {
        InputData d;
        d.size = (unit_size + sizeof(int)) * (unit_batch - 100 + rand() % 100);
        d.brs = NewReadStream((char*)data, d.size);
        total_data_len += d.size;
        ret = queue_buffer_->BatchPush(d.brs, d.size);
        // DSTREAM_INFO("push(size:%d) ret = %d\n", d.size, ret);
    }
    DSTREAM_INFO("%s total data len(%lu)", queue_buffer_->DumpStatus().c_str(), total_data_len);

    int pop_total_len = 0;
    while (!queue_buffer_->IsEmpty() && (data_count < pop_batch) ) {
        int32_t len = (unit_size + sizeof(int)) * (unit_batch - 100 + rand() % 100);
        int32_t batch = 0;
        uint64_t ack = transporter::kAdvanceQueueAck;
        CBufWriteStream bws;
        ret = queue_buffer_->BatchPending(&bws, &len, &batch, &ack);
        // DSTREAM_INFO("pending(%d,%d) ret = %d\n", len, batch, ret);
        ret = queue_buffer_->BatchPop(ack);
        // DSTREAM_INFO("pop ret = %d\n", ret);
        data_count += batch;
        pop_total_len += len;
        // DSTREAM_INFO("%s pop len: %d\n",
        //             queue_buffer_->DumpStatus().c_str(), pop_total_len);
    }
    pop_len = pop_total_len;
    pop_batch = data_count;
    return clock() - start_t;
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_QueueBuffer_Profile_suite, Profiling)
{
    ASSERT_TRUE(queue_buffer_ != static_cast<QueueBuffer*>(NULL));

    const int loop_count = 100;
    const int unit_batch = 500;
    const int unit_size = 1000;
    const uint64_t data_num = 10000;
    uint64_t total_data_len = 0;
    uint64_t total_data_batch = 0;
    uint8_t* data = new (std::nothrow) uint8_t[unit_batch * (unit_size + sizeof(int))];
    FillLargeData(data, unit_batch * (unit_size + sizeof(int)), unit_size);

    //  ProfilerStart("queue_pf");
    clock_t total_time = 0;
    uint64_t loop_batch = 0;
    uint64_t loop_len = 0;
    for (int i = 0; i < loop_count - 1; ++i) {
        loop_batch = 50000;
        total_time += RunTestLoop(data, data_num, unit_batch, unit_size, loop_batch, loop_len);
        total_data_batch += loop_batch;
        total_data_len += loop_len;
    }
    loop_batch = (uint64_t) - 1;
    total_time += RunTestLoop(data, data_num, unit_batch, unit_size, loop_batch, loop_len);
    total_data_batch += loop_batch;
    total_data_len += loop_len;
    DSTREAM_INFO("total data len(%lu), total count(%lu), elapse time(%fs), QPS(%f), DPS(%f)",
                 total_data_len, total_data_batch, total_time * 1.0 / CLOCKS_PER_SEC,
                 total_data_batch / (total_time * 1.0 / CLOCKS_PER_SEC),
                 total_data_len / (total_time * 1.0 / CLOCKS_PER_SEC));

    //  ProfilerFlush();
    //  ProfilerStop();

}
