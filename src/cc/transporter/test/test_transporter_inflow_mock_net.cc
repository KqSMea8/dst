#include <stdlib.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Random.h"
#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_inflow.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Field;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::NotNull;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::DefaultValue;
using ::testing::MockFunction;

using namespace dstream;
using namespace dstream::transporter;

typedef std::vector<transporter::IfSourceMeta> TPIfSourceVec;
typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

static const int kMaxSingleDataLen = 4096;
static const int kBufLen = 1 * 1024;

class DataGenerator {
    DECLARE_SINGLETON(DataGenerator)
public:
    ~DataGenerator() {
        Destroy();
    }
    void Destroy() {
        SAFE_DELETE(buf_);
        index_.clear();
    }

    ErrCode Init(uint64_t capacity = 1024) {
        //prepare data queue
        CRandom rand;
        rand.SetSeed(-1);
        buf_ = (char*)calloc(kBufLen, 1);
        if (NULL == buf_) {
            return error::BAD_MEM_ALLOC;
        }

        int n = 0;
        int size = (int)(rand.Next() * 94) + 1;
        cur_ = buf_;
        do {
            index_.push_back(cur_);
            *(int*)cur_ = size;
            cur_ += sizeof(int);
            for (int i = 32; i < 32 + size; ++i, cur_++) {
                *cur_ = (char)i;
            }
            n += sizeof(int) + size;
            size = (int)(rand.Next() * 94) + 1;
        } while (n + size < kBufLen);
        return error::OK;
    }

    ErrCode Get(CBufWriteStream* bws,
                INOUT int32_t* size,
                OUT int32_t* batch,
                INOUT uint64_t* ack) {
        size_t index = (size_t)(*ack % index_.size());
        int use = (int)(index_[index] - buf_);
        int total = *size < (kBufLen - use) ?
                    *size : (kBufLen - use);
        int n = 0;
        int to_read = *(int*)index_[index];
        *batch = 0;
        while ((n + to_read < total) && (index < index_.size())) {
            bws->PutBuffer(index_[index], sizeof(int) + to_read);
            n += to_read;
            (*batch)++;
            if (++index < index_.size()) {
                to_read = *(int*)index_[index];
            }
        }
        *size = n;
        return error::OK;
    }

    void CheckData(uint64_t ack,
                   CBufReadStream* brs,
                   int32_t size,
                   int32_t batch) {
        static uint64_t count = 0;

        size_t index = (size_t)(ack % index_.size());
        for (int i = 0; i < batch; ++i, index++) {
            int len = *(int*)index_[index];
            int real_len = 0;
            ASSERT_TRUE(brs->GetObject<int>(&real_len));
            ASSERT_EQ(real_len, len)
                    << "At " << count << "st:i=" << i << ", index=" << index << std::endl;
            static char buf[kMaxSingleDataLen];
            ASSERT_TRUE(brs->GetBuffer(buf, len));
            buf[len] = '\0';
            std::string real_str(index_[index] + sizeof(int), len);
            ASSERT_STREQ(real_str.c_str(), buf)
                    << "At " << count << "st:i=" << i << ", index=" << index << std::endl;
            printf("get data %d: = %d, %s\n", i, len, buf);
        }

        count++;
    }

protected:
    char*              buf_;
    char*              cur_;
    int                len_;
    std::vector<char*> index_;
};

IMPLEMENT_SINGLETON_WITHOUT_CTOR(DataGenerator)
#define g_DataGenerator  DataGenerator::Instance()

DataGenerator::DataGenerator() : buf_(NULL), cur_(NULL), len_(kBufLen) {
}

class MockTPAckQueue : public transporter::TPAckQueue {
public:
    MockTPAckQueue(uint64_t owner_id, uint64_t vice_id,
                   const std::string& name, const std::string& cnt_grp)
        : transporter::TPAckQueue(owner_id, vice_id, name, cnt_grp),
          ack_(0) {}

    ErrCode Init(uint64_t capacity) {
        return error::OK;
    }
    ErrCode Destroy() {
        return error::OK;
    }

    ErrCode IsFull() {
        return error::OK;
    }
    bool Empty() {
        return false;
    }

    ErrCode PushBack(CBufReadStream* brs,
                     IN int32_t size) {
        return error::OK;
    }
    ErrCode Front(CBufWriteStream* bws,
                  INOUT int32_t* size,
                  OUT int32_t* batch,
                  INOUT uint64_t* ack) {
        if (transporter::kAdvanceQueueAck == *ack) {
            *ack = ack_;
        }
        return g_DataGenerator->Get(bws, size, batch, ack);
    }
    ErrCode PopFront(uint64_t ack) {
        if (transporter::kFlushQueueUnack == ack) {
            ack_ = 0;
        } else {
            ack_++;
        }
        return error::OK;
    }
    ErrCode PopFront(uint64_t ack,
                     OUT CBufWriteStream* bws,
                     OUT int32_t* size,
                     OUT int32_t* batch) {
        if (transporter::kFlushQueueUnack == ack) {
            ack_ = 0;
        } else {
            ack_++;
        }
        if (NULL != bws && NULL != size && NULL != batch) {
        }
        return error::OK;
    }
    std::string DumpStatus() const {
        return "";
    }

    void CheckData(uint64_t ack,
                   CBufReadStream* brs,
                   int32_t size,
                   int32_t batch) {
        g_DataGenerator->CheckData(ack, brs, size, batch);
    }

public:
    uint64_t ack_;
};

class MockTPInflow : public transporter::TPInflow {
public:
    MockTPInflow(CESPObject* host, uint64_t id, ESPSTUB stub)
        : transporter::TPInflow(host, id, stub) {}

    MOCK_METHOD1(_OnStop, void(AsyncContext*));
    MOCK_METHOD1(OnCompletion, void(AsyncContext*));

    MOCK_METHOD1(_QueryStub,    void(TPInflow::TPSource* source));
    MOCK_METHOD1(_FetchSource , void(TPInflow::TPSource* source));

    MOCK_METHOD6(_OnSourceRecv, ErrCode(TPInflow::TPSource* source,
                                        int32_t err,
                                        uint64_t ack,
                                        CBufReadStream* brs,
                                        int32_t size,
                                        int32_t batch));

    void MockOnStop(AsyncContext* ctx) {
        transporter::TPInflow::_OnStop(ctx);
    }

    void MockOnCompletion(AsyncContext* ctx) {
        transporter::TPInflow::OnCompletion(ctx);
    }

    void MockQueryStub(TPInflow::TPSource* source) {
        transporter::TPInflow::_QueryStub(source);
    }

    void MockFetchSource(TPInflow::TPSource* source) {
        transporter::TPInflow::_FetchSource(source);
    }

    ErrCode MockOnSourceRecv(TPInflow::TPSource* source,
                             int32_t err,
                             uint64_t ack,
                             CBufReadStream* brs,
                             int32_t size,
                             int32_t batch) {
        return transporter::TPInflow::_OnSourceRecv(source, err, ack, brs, size, batch);
    }
};

class MockTransporter : public transporter::Transporter {
public:
    MockTransporter(uint64_t id, ESPSTUB stub, ESPSTUB if_stub = INVALID_ESPSTUB)
        : transporter::Transporter(id, stub, if_stub) {}

    MOCK_METHOD1(_OnAddOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnDelOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnUpdOutflow, void(transporter::OfMgrCtx* ctx));

    void MockOnAddOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnAddOutflow(ctx);
    }
    void MockOnDelOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnDelOutflow(ctx);
    }
    void MockOnUpdOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnUpdOutflow(ctx);
    }
};

class test_TPInflow_StartAndStop_suite : public ::testing::Test {
protected:
    test_TPInflow_StartAndStop_suite(): inflow_(NULL), stub_(INVALID_ESPSTUB),
        id_(kLocalId), fake_owner_(NULL) {};
    virtual ~test_TPInflow_StartAndStop_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeIfStub(id_);
        stub_ = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);
        fake_owner_ = new CESPObject();
        ASSERT_TRUE(NULL != fake_owner_);
        inflow_ = new (std::nothrow) MockTPInflow(fake_owner_, id_, stub_);
        ASSERT_TRUE(NULL != inflow_);
    };
    virtual void TearDown() {
        if (NULL != inflow_) {
            inflow_->Release();
        }
        SAFE_DELETE(fake_owner_);
    };
public:
    static const uint64_t kLocalId = 5712;
public:
    MockTPInflow* inflow_;
    ESPSTUB       stub_;
    uint64_t      id_;
    CESPObject*   fake_owner_;
};

TEST_F(test_TPInflow_StartAndStop_suite, Start_And_Stop) {
    EXPECT_CALL(*(inflow_), _OnStop(NotNull()))
    .WillOnce(DoAll(Invoke(inflow_, &MockTPInflow::MockOnStop),
                    InvokeWithoutArgs(TPTestComplete)));
    EXPECT_CALL(*(inflow_), OnCompletion(_))
    .WillRepeatedly(Invoke(inflow_, &MockTPInflow::MockOnCompletion));

    ASSERT_EQ(inflow_->Start(), error::OK);
    ASSERT_EQ(fake_owner_->m_nHostId, inflow_->m_nHostId);
    TPTestStart();
    inflow_->Stop();
    TPTestWait();
}

TEST_F(test_TPInflow_StartAndStop_suite, EnableSourceChecker) {
    EXPECT_CALL(*(inflow_), _OnStop(NotNull()))
    .WillOnce(DoAll(Invoke(inflow_, &MockTPInflow::MockOnStop),
                    InvokeWithoutArgs(TPTestComplete)));
    //检查5次定期check，并触发signal，其余忽略
    EXPECT_CALL(*(inflow_), OnCompletion(_))
    .WillRepeatedly(Invoke(inflow_, &MockTPInflow::MockOnCompletion));
    EXPECT_CALL(*(inflow_),
                OnCompletion(
                    Field(&AsyncContext::nAction,
                          Eq((int)transporter::TPInflow::TPA_IF_CHECK_SOURCE))))
    .Times(5)
    .WillRepeatedly(DoAll(Invoke(inflow_, &MockTPInflow::MockOnCompletion),
                          InvokeWithoutArgs(TPTestComplete)));

    TPTestStart(5); //wait triggled 5 times
    ASSERT_EQ(inflow_->Start(), error::OK);
    ASSERT_EQ(fake_owner_->m_nHostId, inflow_->m_nHostId);
    TPTestWait(); //wait(), check interval

    TPTestStart();
    inflow_->Stop();
    TPTestWait();
}

class test_TPInflow_FetchOutflow_suite : public ::testing::Test {
protected:
    test_TPInflow_FetchOutflow_suite(): inflow_(NULL), stub_(INVALID_ESPSTUB),
        id_(kLocalId), fake_owner_(NULL),
        up_tp_(NULL), up_id_(kUpId),
        up_node_(INVALID_ESPNODE),
        up_stub_(INVALID_ESPSTUB) {};
    virtual ~test_TPInflow_FetchOutflow_suite() {};
    virtual void SetUp() {
        //alloc stub
        g_pTPStubMgr->FreeIfStub(id_);
        g_pTPStubMgr->FreeOfStub(up_id_);
        stub_    = g_pTPStubMgr->AllocIfStub(id_);
        up_stub_ = g_pTPStubMgr->AllocOfStub(up_id_);
        ASSERT_NE(stub_,    INVALID_ESPSTUB);
        ASSERT_NE(up_stub_, INVALID_ESPSTUB);
        //start up transnporter
        up_tp_ = new (std::nothrow) MockTransporter(up_id_, up_stub_);
        ASSERT_TRUE(NULL != up_tp_);
        ASSERT_EQ(up_tp_->Start(), error::OK);

        EXPECT_CALL(*(up_tp_), _OnAddOutflow(_))
        .WillOnce(DoAll(Invoke(up_tp_, &MockTransporter::MockOnAddOutflow),
                        InvokeWithoutArgs(TPTestComplete)));

        //add outflow
        TPOfSourceVec* outflows = new (std::nothrow)TPOfSourceVec();
        ASSERT_TRUE(NULL != outflows);
        std::vector<std::string> tags;
        tags.push_back("tag");
        transporter::OfMeta of_meta = {EXTRACT_APPPRO_ID(id_), 1, tags};
        outflows->push_back(of_meta);
        TPTestStart();
        ASSERT_EQ(up_tp_->AddOutflow(outflows), error::OK);
        TPTestWait();
        ASSERT_EQ(up_tp_->m_outflow_map.size(), (size_t)1);

        //replace send queue with mock queue
        transporter::Transporter::OutflowMap::iterator it;
        it = up_tp_->m_outflow_map.find(EXTRACT_APPPRO_ID(id_));
        ASSERT_NE(it, up_tp_->m_outflow_map.end());
        transporter::TPOutflow* outflow = it->second;
        ASSERT_TRUE(NULL != outflow);
        TPAckQueue* queue = outflow->m_branches[0].queue;
        ASSERT_TRUE(NULL != queue);
        mock_of_queue_ = new MockTPAckQueue(queue->m_owner_id,
                                            queue->m_vice_id,
                                            queue->m_name,
                                            queue->m_cnt_grp);
        ASSERT_TRUE(NULL != mock_of_queue_);
        ASSERT_EQ(mock_of_queue_->Init(10000), error::OK);
        DSTREAM_DEBUG("Replace TPAckQueue %p with MockTPAckQueue %p",
                      queue, mock_of_queue_);
        delete(queue);
        outflow->m_branches[0].queue = mock_of_queue_;

        //make address
        char ipport[64];
        snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
        up_node_ = ESPFindNode(ipport);
        ASSERT_NE(up_node_, INVALID_ESPNODE);
    };
    virtual void TearDown() {
        if (NULL != inflow_) {
            EXPECT_CALL(*(inflow_), _OnStop(NotNull()))
            .WillOnce(DoAll(Invoke(inflow_, &MockTPInflow::MockOnStop),
                            InvokeWithoutArgs(TPTestComplete)));
            TPTestStart();
            inflow_->Stop();
            TPTestWait();
            inflow_->Release();
        }
        if (NULL != up_tp_) {
            up_tp_->Stop();
            up_tp_->Release();
        }
        if (NULL != fake_owner_) {
            delete fake_owner_;
        }
        inflow_ = NULL;
        up_tp_  = NULL;
        fake_owner_ = NULL;
    };
public:
    static const uint64_t kLocalId = MAKE_PEID(1UL, 1UL, 0UL);
    static const uint64_t kUpId    = MAKE_PEID(1UL, 2UL, 0UL);
public:
    MockTPInflow*          inflow_;
    ESPSTUB                stub_;
    uint64_t               id_;
    CESPObject*            fake_owner_;
    MockTPAckQueue*        mock_if_queue_;

    MockTransporter* up_tp_;
    uint64_t         up_id_;
    ESPNODE          up_node_;
    ESPSTUB          up_stub_;
    MockTPAckQueue*  mock_of_queue_;
};

ACTION_P(CheckRecvData, queue) {
    queue->CheckData(arg2, arg3, arg4, arg5);
}

TEST_F(test_TPInflow_FetchOutflow_suite, FetchOutflow) {
    ErrCode default_errcode = error::OK;
    DefaultValue<ErrCode>::Set(default_errcode);

    //start inflow
    fake_owner_ = new CESPObject();
    ASSERT_TRUE(NULL != fake_owner_);
    inflow_ = new (std::nothrow) MockTPInflow(fake_owner_, id_, stub_);
    ASSERT_TRUE(NULL != inflow_);
    inflow_->m_batch_size = 1024; //limit fetch size

    EXPECT_CALL(*(inflow_), OnCompletion(_))
    .WillRepeatedly(Invoke(inflow_, &MockTPInflow::MockOnCompletion));

    ASSERT_EQ(inflow_->Start(), error::OK);

    //replace recv queue with mock queue
    ASSERT_TRUE(NULL != inflow_->m_queue);
    DSTREAM_DEBUG("Replace TPAckQueue %p with MockTPAckQueue %p",
                  inflow_->m_queue, mock_if_queue_);
    mock_if_queue_ = new MockTPAckQueue(inflow_->m_queue->m_owner_id,
                                        inflow_->m_queue->m_vice_id,
                                        inflow_->m_queue->m_name,
                                        inflow_->m_queue->m_cnt_grp);
    delete inflow_->m_queue;
    inflow_->m_queue = mock_if_queue_;

    //expect _OnSourceRecv triggled
    TPTestStart(5);

    EXPECT_CALL(*inflow_, _OnSourceRecv(_, _, _, _, _, _))
    .WillRepeatedly(DoAll(CheckRecvData(mock_of_queue_),
                          Invoke(inflow_, &MockTPInflow::MockOnSourceRecv)));

    //set source
    TPIfSourceVec sources;
    transporter::IfSourceMeta if_meta = {kUpId, up_node_};
    sources.push_back(if_meta);
    ASSERT_EQ(inflow_->AddSource(sources, NULL), error::OK);
    ASSERT_EQ(inflow_->m_sources.size(), 1UL);

    //find source
    TPInflow::TPSourceMap::iterator it = inflow_->m_sources.find(up_id_);
    ASSERT_TRUE(it != inflow_->m_sources.end());
    TPInflow::TPSource* source = it->second;
    ASSERT_TRUE(NULL != source);

    //check fetch
    // _QueryStub atleast once
    EXPECT_CALL(*inflow_, _QueryStub(_))
    .Times(AtLeast(1))
    .WillRepeatedly(DoAll(Invoke(inflow_, &MockTPInflow::MockQueryStub),
                          InvokeWithoutArgs(TPTestComplete)));
    EXPECT_CALL(*inflow_, _FetchSource(_))
    .WillRepeatedly(DoAll(Invoke(inflow_, &MockTPInflow::MockFetchSource),
                          InvokeWithoutArgs(TPTestComplete)));

    TPTestWait();
}

TEST_F(test_TPInflow_FetchOutflow_suite, Refetch) {
    //TODO
}

class test_TPInflow_OnPEFetch_suite : public ::testing::Test {
protected:
    test_TPInflow_OnPEFetch_suite(): inflow_(NULL), stub_(INVALID_ESPSTUB),
        node_(INVALID_ESPNODE), id_(kLocalId),
        fake_owner_(NULL) {}
    virtual ~test_TPInflow_OnPEFetch_suite() {};
    virtual void SetUp() {
        //alloc stub
        g_pTPStubMgr->FreeIfStub(id_);
        stub_ = g_pTPStubMgr->AllocIfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);

        //start inflow
        fake_owner_ = new CESPObject();
        ASSERT_TRUE(NULL != fake_owner_);
        inflow_ = new (std::nothrow) MockTPInflow(fake_owner_, id_, stub_);
        ASSERT_TRUE(NULL != inflow_);
        inflow_->m_batch_size = 1024; //limit fetch size

        EXPECT_CALL(*(inflow_), OnCompletion(_))
        .WillRepeatedly(Invoke(inflow_, &MockTPInflow::MockOnCompletion));

        ASSERT_EQ(inflow_->Start(), error::OK);

        //replace recv queue with mock queue
        ASSERT_TRUE(NULL != inflow_->m_queue);
        DSTREAM_DEBUG("Replace TPAckQueue %p with MockTPAckQueue %p",
                      inflow_->m_queue, mock_if_queue_);
        mock_if_queue_ = new MockTPAckQueue(inflow_->m_queue->m_owner_id,
                                            inflow_->m_queue->m_vice_id,
                                            inflow_->m_queue->m_name,
                                            inflow_->m_queue->m_cnt_grp);
        delete inflow_->m_queue;
        inflow_->m_queue = mock_if_queue_;
    };
    virtual void TearDown() {
        if (NULL != inflow_) {
            EXPECT_CALL(*(inflow_), _OnStop(NotNull()))
            .WillOnce(DoAll(Invoke(inflow_, &MockTPInflow::MockOnStop),
                            InvokeWithoutArgs(TPTestComplete)));
            TPTestStart();
            inflow_->Stop();
            TPTestWait();
            inflow_->Release();
        }
        if (NULL != fake_owner_) {
            delete fake_owner_;
        }
        inflow_ = NULL;
        fake_owner_ = NULL;
    };
public:
    static const uint64_t kLocalId = MAKE_PEID(1UL, 1UL, 0UL);
public:
    MockTPInflow*          inflow_;
    ESPSTUB                stub_;
    ESPNODE                node_;
    uint64_t               id_;
    CESPObject*            fake_owner_;
    MockTPAckQueue*        mock_if_queue_;
};

TEST_F(test_TPInflow_OnPEFetch_suite, OnPEFetch) {
    char ipport[64];
    snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);

    transporter::TPDataMsg msg;
    BufHandle* ack_hdl = NULL;

    transporter::TPProxyCli cli(transporter::TPT_INFLOW, id_, id_);
    ASSERT_EQ(cli.Init(ipport), error::OK);
    for (int i = 0; i < 10; ++i) {
        msg.from_id = id_;
        msg.to_id   = id_;
        msg.err     = transporter::TPN_ACK_OK;
        msg.batch   = 100;
        msg.size    = 1024;
        msg.ack     = i; //for test
        CBufWriteStream bws;
        bws.PutObject<transporter::TPDataMsg>(msg);
        ASSERT_EQ(cli.SyncReq(transporter::TPE_IF_LOCALPE_FETCH,
                              bws.GetBegin(), &ack_hdl), error::OK);
        ASSERT_TRUE(ack_hdl != NULL);

        //check data
        CBufReadStream brs(ack_hdl, NULL);
        brs.GetObject<transporter::TPDataMsg>(&msg);
        ASSERT_EQ(msg.from_id, id_);
        ASSERT_EQ(msg.to_id,   id_);
        ASSERT_LT(msg.size,    1024);
        ASSERT_GT(msg.batch,   0);

        DSTREAM_DEBUG("Get Msg:"S_TP_DATA_MSG_FMT, DOTTED_TP_DATA_MSG(msg));

        mock_if_queue_->CheckData(msg.ack, &brs, msg.size, msg.batch);

        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
    }
}

int main(int argc, char** argv) {
    DSTREAM_UNUSED_VAL(kMaxSingleLen);
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    if (g_DataGenerator->Init() < error::OK) {
        return -1;
    }
    int ret = RUN_ALL_TESTS();
    g_DataGenerator->Destroy();
    return ret;
}
