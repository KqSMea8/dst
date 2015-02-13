#include <transporter/transporter.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "common/mem_mgr.h"
#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_stubmgr.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::DefaultValue;
using ::testing::MockFunction;
using ::testing::InSequence;

using namespace dstream;

typedef std::vector<transporter::IfSourceMeta> TPIfSourceVec;
typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::dstream::mem_mgr::MemMgr::MemMgrPtr inst = ::dstream::mem_mgr::MemMgr::Instance();
    inst->Init(10000000000);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}

class MockTPInflow : public transporter::TPInflow {
public:
    MockTPInflow(CESPObject* host, uint64_t id, ESPSTUB stub)
        : transporter::TPInflow(host, id, stub) {}

    MOCK_METHOD0(_SetSourceChecker, void()); //disable this function

    MOCK_METHOD2(AddSource,
                 transporter::ErrCode(const std::vector<transporter::IfSourceMeta>&,
                                      std::vector<transporter::ErrCode>*));
    MOCK_METHOD2(DelSource,
                 transporter::ErrCode(const std::vector<transporter::IfSourceMeta>&,
                                      std::vector<transporter::ErrCode>*));
    MOCK_METHOD2(UpdSource,
                 transporter::ErrCode(const std::vector<transporter::IfSourceMeta>&,
                                      std::vector<transporter::ErrCode>*));

    transporter::ErrCode MockAddSource(const std::vector<transporter::IfSourceMeta>& sources,
                                       std::vector<transporter::ErrCode>* res) {
        transporter::ErrCode ret = transporter::TPInflow::AddSource(sources, NULL);
        TPTestComplete();
        return ret;
    }
    transporter::ErrCode MockDelSource(const std::vector<transporter::IfSourceMeta>& sources,
                                       std::vector<transporter::ErrCode>* res) {
        transporter::ErrCode ret = transporter::TPInflow::DelSource(sources, NULL);
        TPTestComplete();
        return ret;
    }
    transporter::ErrCode MockUpdSource(const std::vector<transporter::IfSourceMeta>& sources,
                                       std::vector<transporter::ErrCode>* res) {
        transporter::ErrCode ret =  transporter::TPInflow::UpdSource(sources, NULL);
        TPTestComplete();
        return ret;
    }
};

class MockTransporter : public transporter::Transporter {
public:
    MockTransporter(uint64_t id, ESPSTUB stub, ESPSTUB if_stub = INVALID_ESPSTUB)
        : transporter::Transporter(id, stub, if_stub) {}

    MOCK_METHOD1(_OnAddOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnDelOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnUpdOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(OnOutflowRecv, void(ESPMessage* esp_msg));

    void MockOnAddOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnAddOutflow(ctx);
        TPTestComplete();
    }
    void MockOnDelOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnDelOutflow(ctx);
        TPTestComplete();
    }
    void MockOnUpdOutflow(transporter::OfMgrCtx* ctx) {
        transporter::Transporter::_OnUpdOutflow(ctx);
        TPTestComplete();
    }

    void MockOnOutflowRecv(ESPMessage* esp_msg) {
        transporter::Transporter::OnOutflowRecv(esp_msg);
        TPTestComplete();
    }
    //MockFunction<void(std::string)> check_;
};

class MockTPAckQueue : public transporter::TPAckQueue {
public:
    MockTPAckQueue(uint64_t owner_id, uint64_t vice_id,
                   const std::string& name, const std::string& cnt_grp)
        : transporter::TPAckQueue(owner_id, vice_id, name, cnt_grp) {}

    MOCK_METHOD1(Init, transporter::ErrCode(uint64_t capacity));
    MOCK_METHOD0(Destroy, transporter::ErrCode());

    MOCK_METHOD2(PushBack, transporter::ErrCode(CBufReadStream* brs, IN int32_t size));
    MOCK_METHOD4(Front, transporter::ErrCode(CBufWriteStream* bws,
                                             INOUT int32_t*  size,
                                             OUT   int32_t*  batch,
                                             INOUT uint64_t* ack));
    MOCK_METHOD1(PopFront, transporter::ErrCode(uint64_t ack));
    MOCK_METHOD4(PopFront, transporter::ErrCode(uint64_t ack,
                                                CBufWriteStream* bws,
                                                int32_t* size,
                                                int32_t* batch));
    MOCK_CONST_METHOD0(DumpStatus, std::string());
};

//---------------------------------------------------------
// fetch outflow data
static const uint64_t kDataNum    = 5;
static       int32_t  kDataSize;
static const uint64_t kMaxDataLen = 1024;
std::vector<std::string> datas;

class test_Transporter_OnOutflowFetch_suite : public ::testing::Test {
protected:
    test_Transporter_OnOutflowFetch_suite(): tp_(NULL), id_(kLocalId),
        down_id_(kDownId), down_ack_(0),
        if_cli_(NULL) {
    }

    virtual ~test_Transporter_OnOutflowFetch_suite() {};
    virtual void SetUp() {
        stub_ = g_pTPStubMgr->AllocOfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);
        tp_ = new MockTransporter(id_, stub_);
        ASSERT_TRUE(NULL != tp_);
        ASSERT_EQ(tp_->Start(), error::OK);

        EXPECT_CALL(*(tp_), _OnAddOutflow(_))
        .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow));

        // fill data
        if (0 == datas.size()) {
            for (uint64_t i = 0; i < kDataNum; ++i) {
                std::string s = "Xfered Data: ";
                for (uint64_t j = 1; j < i * 2 + 2; ++j) {
                    StringAppendNumber(&s, j);
                }
                datas.push_back(s);
                kDataSize += sizeof(int); //int len
                kDataSize += (int32_t)s.size();
            }
        }
        printf("kDataSize=%d\n", kDataSize);

        //add outflow
        TPOfSourceVec* outflows = new TPOfSourceVec();
        ASSERT_TRUE(NULL != outflows);
        std::vector<std::string> tags;
        tags.push_back("tag");
        transporter::OfMeta of_meta = {EXTRACT_APPPRO_ID(down_id_), 1, tags};
        outflows->push_back(of_meta);
        TPTestStart();
        ASSERT_EQ(tp_->AddOutflow(outflows), error::OK);
        TPTestWait();
        ASSERT_EQ(tp_->m_outflow_map.size(), (size_t)1);

        //replace mock queue;
        transporter::Transporter::OutflowMap::iterator it;
        it = tp_->m_outflow_map.find(EXTRACT_APPPRO_ID(down_id_));
        ASSERT_NE(it, tp_->m_outflow_map.end());
        transporter::TPOutflow* outflow = it->second;
        ASSERT_TRUE(NULL != outflow);
        transporter::TPAckQueue* queue = outflow->m_branches[0].queue;
        ASSERT_TRUE(NULL != queue);
        mock_of_queue_ = new MockTPAckQueue(queue->m_owner_id,
                                            queue->m_vice_id,
                                            queue->m_name,
                                            queue->m_cnt_grp);
        ASSERT_TRUE(NULL != mock_of_queue_);
        DSTREAM_DEBUG("Replace TPAckQueue %p with MockTPAckQueue %p",
                      queue, mock_of_queue_);
        SAFE_DELETE(queue);
        outflow->m_branches[0].queue = mock_of_queue_;

        //create conn
        char ipport[64];
        snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
        ESPNODE node = ESPFindNode(ipport);
        ASSERT_NE(node, INVALID_ESPNODE);
        if_cli_ = new transporter::TPCli();
        ASSERT_EQ(if_cli_->Init(MakeESPADDR(node, stub_)), error::OK);

        ON_CALL(*mock_of_queue_, Destroy())
        .WillByDefault(Return(error::OK));
        EXPECT_CALL(*mock_of_queue_, Destroy())
        .Times(AtLeast(1));
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            tp_->Stop();
            tp_->Release();
            tp_ = NULL;
        }
        g_pTPStubMgr->FreeOfStub(id_);
    };
public:
    static const uint64_t kLocalId    = MAKE_PEID(1UL, 1UL, 3UL);
    static const uint64_t kDownId     = MAKE_PEID(1UL, 2UL, 0UL);

public:
    MockTransporter*    tp_;
    uint64_t            id_;
    uint64_t            stub_;

    uint64_t            down_id_;
    uint64_t            down_batch_;
    int32_t             down_batch_size_;
    uint64_t            down_ack_;

    MockTPAckQueue*     mock_of_queue_;
    transporter::TPCli* if_cli_;
};

transporter::ErrCode FillDataToWriteStream(CBufWriteStream* bws,
                                           INOUT int32_t*   size,
                                           OUT   int32_t*   batch,
                                           INOUT uint64_t*  ack) {
    DSTREAM_UNUSED_VAL(ack);
    int32_t  min = *size < kDataSize ? *size : kDataSize;
    int32_t  n;
    uint64_t i;
    for (n = 0, i = 0; n + (int)sizeof(int) + (int)datas[i].size() < min; ++i) {
        std::string& s = datas[i];
        bws->PutObject<int>((int)s.size());
        bws->PutBuffer(s.data(), s.size());
        n += sizeof(int) + s.size();
        printf("fill data %lu: = %lu, %s\n", i, s.size(), s.c_str());
    }
    *size = n;
    *batch = i;
    return error::OK;
}

const size_t kFetchNum = 100;
ACTION_P(FillDataAction, ack) {
    *arg3 = ack % kFetchNum;
    return FillDataToWriteStream(arg0, arg1, arg2, arg3);
}
/**
* @brief    测试正常fetch
* @date     2013-01-30
*/
TEST_F(test_Transporter_OnOutflowFetch_suite, OnOutflowFetch_Start) {
    BufHandle* ack_hdl;
    transporter::TPDataMsg msg;
    char buf[4096] = {0};
    int  len = 0;

    transporter::ErrCode default_errcode = error::OK;
    DefaultValue<transporter::ErrCode>::Set(default_errcode);

    //first fetch, 会触发AckQueue的PopFront
    EXPECT_CALL(*mock_of_queue_, PopFront(transporter::kFlushQueueUnack, _, _, _))
    .Times(AtLeast(1))
    .WillRepeatedly(Return(error::OK));

    down_batch_size_ = kDataSize / 2;
    msg.from_id = down_id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_SYNC;
    msg.size    = down_batch_size_;
    msg.ack     = 0;
    CBufWriteStream bws;
    bws.PutObject<transporter::TPDataMsg>(msg);
    ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                               bws.GetBegin(), &ack_hdl, 5000),
              error::OK);

    CBufReadStream brs(ack_hdl, NULL);
    ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));
    ASSERT_EQ(msg.from_id, id_);
    ASSERT_EQ(msg.to_id, down_id_);
    ASSERT_EQ(msg.err, transporter::TPN_ACK_SYNC);
    ASSERT_EQ(msg.ack, 0UL);
    ASSERT_EQ(msg.size, 0);
    ASSERT_EQ(msg.batch, 0);

    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;

    //resync after short break,  不会触发AckQueue的PopFront和Front;
    for (size_t i = 1; i < kFetchNum; ++i) {
        down_batch_size_ = kDataSize / 2;
        msg.from_id = down_id_;
        msg.to_id   = id_;
        msg.err     = transporter::TPN_ACK_SYNC;
        msg.size    = down_batch_size_;
        msg.ack     = i;
        CBufWriteStream bws1;
        bws1.PutObject<transporter::TPDataMsg>(msg);
        ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                                   bws1.GetBegin(), &ack_hdl, 5000),
                  error::OK);

        CBufReadStream brs(ack_hdl, NULL);
        ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));
        ASSERT_EQ(msg.from_id, id_);
        ASSERT_EQ(msg.to_id, down_id_);
        ASSERT_EQ(msg.err, transporter::TPN_ACK_SYNC);
        ASSERT_EQ(msg.ack, i);
        ASSERT_EQ(msg.size, 0);
        ASSERT_EQ(msg.batch, 0);

        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
    } // end of resync

    // more fetch
    for (size_t i = kFetchNum/*从上次的sync开始*/; i < kFetchNum + 10; ++i) {
        EXPECT_CALL(*mock_of_queue_, PopFront(_))
        .WillOnce(Return(error::OK));
        EXPECT_CALL(*mock_of_queue_, Front(_, _, _, _))
        .WillOnce(FillDataAction(i));

        down_batch_size_ = kDataSize + i;
        msg.from_id = down_id_;
        msg.to_id   = id_;
        msg.err     = transporter::TPN_ACK_OK;
        msg.size    = down_batch_size_;
        msg.ack     = i;
        CBufWriteStream bws;
        bws.PutObject<transporter::TPDataMsg>(msg);


        ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                                   bws.GetBegin(), &ack_hdl, 5000),
                  error::OK);
        CBufReadStream brs(ack_hdl, NULL);
        ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));
        ASSERT_EQ(msg.from_id, id_) << "At " << i << "th";
        ASSERT_EQ(msg.to_id, down_id_) << "At " << i << "th";
        ASSERT_EQ(msg.err, transporter::TPN_ACK_OK) << "At " << i << "th";
        ASSERT_EQ(msg.ack, i) << "At " << i << "th";
        ASSERT_LE(msg.size, down_batch_size_) << "At " << i << "th";
        down_ack_ = msg.ack + 1; //update ack

        int n = 0;
        for (size_t j = 0; n != msg.size; ++j) {
            ASSERT_TRUE(brs.GetObject<int>(&len)) << "At i=" << i << ", j=" << j;
            ASSERT_EQ(len, (int)datas[j].size()) << "At i=" << i << ", j=" << j;
            ASSERT_EQ(brs.GetBuffer(buf, len), len) << "At i=" << i << ", j=" << j;
            buf[len] = '\0';
            ASSERT_STREQ(buf, datas[j].data()) << "At i=" << i << ", j=" << j;
            n += sizeof(int) + len;
            //printf("Get data: [%s]\n", buf);
        }

        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
    }// end of more fetch
} //end of OnOutflowFetch

/**
* @brief    消费一次，重放N次
* @date     2013-01-30
*/
TEST_F(test_Transporter_OnOutflowFetch_suite, OnOutflowFetch_ReConsume) {
    BufHandle* ack_hdl;
    transporter::TPDataMsg msg;
    bool reply = false;
    int  reply_size = 0;

    char buf[4096] = {0};
    int  len = 0;
    int32_t n;
    size_t j;

    // 每隔10次，重复抓取2次
    int step = 1;
    for (int i = 0; i < 100; i += step) {
        step = 0 == step ? 1 : (int)(5 != i % 10);

        transporter::TPMsgErr err;
        if (0 == i) {
            err = transporter::TPN_ACK_SYNC;
            EXPECT_CALL(*mock_of_queue_, PopFront(transporter::kFlushQueueUnack, _, _, _))
            .WillOnce(Return(error::OK));
        } else {
            err = transporter::TPN_ACK_OK;
            if (reply) {
                EXPECT_CALL(*mock_of_queue_, PopFront(_))
                .Times(0)
                .WillRepeatedly(Return(error::OK));
            } else {
                EXPECT_CALL(*mock_of_queue_, PopFront(_))
                .Times(AnyNumber())
                .WillRepeatedly(Return(error::OK));
            }
            EXPECT_CALL(*mock_of_queue_, Front(_, _, _, _))
            .WillOnce(Invoke(FillDataToWriteStream));
        }

        down_batch_size_ = (kDataSize + i) % kDataSize + 100;
        msg.from_id = down_id_;
        msg.to_id   = id_;
        msg.err     = err;
        msg.size    = down_batch_size_;
        msg.ack     = down_ack_;
        CBufWriteStream bws;
        bws.PutObject<transporter::TPDataMsg>(msg);


        ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                                   bws.GetBegin(), &ack_hdl, 50000),
                  error::OK);
        CBufReadStream brs(ack_hdl, NULL);
        ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));
        ASSERT_EQ(msg.from_id, id_);
        ASSERT_EQ(msg.to_id, down_id_);
        ASSERT_EQ(msg.err, err);
        if (step) {
            ASSERT_EQ(msg.ack, down_ack_);
        }
        ASSERT_LE(msg.size, down_batch_size_);
        down_ack_ = 0 == step ? msg.ack : msg.ack + 1; //update ack

        if (i && (0 == step)) {
            reply_size = msg.size;
            reply = true;
        }
        if (reply) {//重放数据，应该相等，此处通过比对大小确认相等；
            ASSERT_EQ(reply_size, msg.size);
            reply = false;
        }

        for (j = 0, n = 0; n != msg.size; ++j) {
            ASSERT_TRUE(brs.GetObject<int>(&len));
            ASSERT_EQ(len, (int)datas[j].size());
            ASSERT_EQ(brs.GetBuffer(buf, len), len);
            buf[len] = '\0';
            ASSERT_STREQ(buf, datas[j].data());
            n += sizeof(int) + len;
            //printf("Get data: [%s]\n", buf);
        }

        ChainFreeHdl(ack_hdl, NULL);
        ack_hdl = NULL;
    }// end of more fetch
}

/**
* @brief    test of posibilities of OnFetchData fail
*
* @param    test_Transporter_OnOutflowFetch_suite
* @param    OnOutflowFetch_Fail
* @author   fangjun,fangjun02@baidu.com
* @date     2013-03-09
*/
TEST_F(test_Transporter_OnOutflowFetch_suite, OnOutflowFetch_Fail) {
    BufHandle* ack_hdl;
    transporter::TPDataMsg msg;

    EXPECT_CALL(*mock_of_queue_, PopFront(transporter::kFlushQueueUnack, _, _, _))
    .WillOnce(Return(error::BAD_ARGUMENT))
    .WillOnce(Return(error::OK));

    EXPECT_CALL(*mock_of_queue_, PopFront(_))
    .WillOnce(Return(error::BAD_ARGUMENT))
    .WillRepeatedly(Return(error::OK));

    //EXPECT_CALL(*mock_of_queue_, Front(_, _, _, _))
    //            .WillOnce(Return(error::BAD_ARGUMENT));
    //            .WillOnce(Return(error::OK));

    // test of popfront fail(err == TPN_ACK_SYNC)
    down_batch_size_ = kDataSize;
    msg.from_id = down_id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_SYNC;
    msg.size    = down_batch_size_;
    msg.ack     = 0;
    CBufWriteStream bws;
    bws.PutObject<transporter::TPDataMsg>(msg);
    ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                               bws.GetBegin(), &ack_hdl, 50),
              APFE_TIMEDOUT);

    CBufReadStream brs(ack_hdl, NULL);
    ASSERT_FALSE(brs.GetObject<transporter::TPDataMsg>(&msg));

    // make sync OK
    down_batch_size_ = kDataSize;
    msg.from_id = down_id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_SYNC;
    msg.size    = down_batch_size_;
    msg.ack     = 0;
    CBufWriteStream bws1;
    bws1.PutObject<transporter::TPDataMsg>(msg);

    ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                               bws1.GetBegin(), &ack_hdl, 5000),
              error::OK);

    // test of popfront fail(err == TPN_ACK_OK)
    down_batch_size_ = kDataSize;
    msg.from_id = down_id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_OK;
    msg.size    = down_batch_size_;
    msg.ack     = 1;
    CBufWriteStream bws2;
    bws2.PutObject<transporter::TPDataMsg>(msg);
    ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                               bws2.GetBegin(), &ack_hdl, 50),
              APFE_TIMEDOUT);

    CBufReadStream brs2(ack_hdl, NULL);
    ASSERT_FALSE(brs2.GetObject<transporter::TPDataMsg>(&msg));

    // test of fetch size is 0
    down_batch_size_ = 0;
    msg.from_id = down_id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_OK;
    msg.size    = down_batch_size_;
    msg.ack     = 1;
    CBufWriteStream bws3;
    bws3.PutObject<transporter::TPDataMsg>(msg);
    ASSERT_EQ(if_cli_->SyncReq(transporter::TPE_IF_FETCH_SOURCE,
                               bws3.GetBegin(), &ack_hdl, 50),
              APFE_TIMEDOUT);

    CBufReadStream brs3(ack_hdl, NULL);
    ASSERT_FALSE(brs3.GetObject<transporter::TPDataMsg>(&msg));

    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
} //end of OnOutflowFetch



class test_Transporter_OnOutflowRecv_suite : public ::testing::Test {
protected:
    test_Transporter_OnOutflowRecv_suite(): tp_(NULL), id_(kLocalId),
        stub_(INVALID_ESPSTUB),
        down_id_(kDownId), pe_cli_(NULL) {
    }

    virtual ~test_Transporter_OnOutflowRecv_suite() {};
    virtual void SetUp() {
        //init data
        InitData();

        //start transporter
        stub_ = g_pTPStubMgr->AllocOfStub(id_);
        ASSERT_NE(stub_, INVALID_ESPSTUB);
        tp_ = new MockTransporter(id_, stub_);
        ASSERT_TRUE(NULL != tp_);
        ASSERT_EQ(tp_->Start(), error::OK);

        EXPECT_CALL(*(tp_), _OnAddOutflow(_))
        .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow));

        //add outflow
        TPOfSourceVec* outflows = new TPOfSourceVec();
        ASSERT_TRUE(NULL != outflows);
        std::vector<std::string> tags;
        tags.push_back(kTestTag);
        transporter::OfMeta of_meta = {EXTRACT_PROCESSOR_ID(down_id_), kParall, tags};
        outflows->push_back(of_meta);
        TPTestStart();
        ASSERT_EQ(tp_->AddOutflow(outflows), error::OK);
        TPTestWait();
        ASSERT_EQ(tp_->m_outflow_map.size(), (size_t)1);

        //create conn
        char ipport[64];
        snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
        ESPNODE node = ESPFindNode(ipport);
        ASSERT_NE(node, INVALID_ESPNODE);
        pe_cli_ = new transporter::TPProxyCli(transporter::TPT_OUTFLOW, id_, id_);
        ASSERT_TRUE(NULL != pe_cli_);
        ASSERT_EQ(pe_cli_->Init(ipport, -1), error::OK);
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            tp_->Stop();
            tp_->Release();
            tp_ = NULL;
        }
        g_pTPStubMgr->FreeOfStub(id_);
    };

    void InitData() {
        data_.clear();
        CRandom rand;
        rand.SetSeed(-1);
        data_size = 0;
        for (uint64_t i = 0; i < kTestDataNum; ++i) {
            int size = (int)(rand.Next() * 94) + 1;
            std::string s = "Test Data: ";
            for (int j = 32; j < 32 + size; ++j) {
                s += (char)j;
            }
            data_.push_back(s);
            data_size += s.size();
            //printf("Gen data: %lu: %s\n", s.size(), s.c_str());
        }
    }
    void FillData(size_t index,
                  CBufWriteStream* bws) {
        //ASSERT_LT(index, data_.size());
        //ASSERT_TRUE(bws->PutObject<int>((int)data_[index].size()));
        ASSERT_TRUE(bws->PutBuffer(data_[index].data(), data_[index].size()));
        //printf("fill data %lu: = %lu, %s\n", index, data_[index].size(),
        //        data_[index].c_str());
    }
    int GetDataLen(size_t index) {
        return (int)data_[index].size();
    }
    void CheckData(CBufReadStream* brs, int size) {
        static size_t index = 0;
        ASSERT_LT(index, data_.size());
        int len;
        ASSERT_TRUE(brs->GetObject<int>(&len));
        ASSERT_EQ((int)data_[index].size(), len);
        ASSERT_EQ(size, len);
        char buf[4096];
        ASSERT_GT((int)sizeof(buf), len);
        ASSERT_EQ(brs->GetBuffer(buf, len), len);
        ASSERT_STREQ(data_[index].c_str(), buf);
        index ++;
    }

public:
    static const uint64_t kParall     = 2UL;
    static const uint64_t kLocalId    = MAKE_PEID(1UL, 1UL, 3UL);
    static const uint64_t kDownId     = MAKE_PEID(1UL, 2UL, 0UL);
    static const uint64_t kTestDataNum = 50;
public:
    MockTransporter*         tp_;
    uint64_t                 id_;
    ESPSTUB                  stub_;
    uint64_t                 down_id_;
    transporter::TPProxyCli* pe_cli_;
    std::vector<std::string> data_;
    size_t                   data_size;
};

TEST_F(test_Transporter_OnOutflowRecv_suite, OnOutflowRecv) {
    //prepare data
    transporter::TPDataMsg msg;

    EXPECT_CALL(*(tp_), OnOutflowRecv(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnOutflowRecv));

    CBufWriteStream bws;
    //send data to outflow
    msg.from_id = id_;
    msg.to_id   = id_;
    msg.err     = transporter::TPN_ACK_OK;
    msg.size    = 0;
    msg.ack     = 0;
    bws.PutObject<transporter::TPDataMsg>(msg);
    uint64_t send_size = 0;
    for (uint64_t i = 0; i < kDataNum; ++i) {
        //write tuple meta
        int tag_len   = (int)(strlen(kTestTag) + 1);
        int data_len  = GetDataLen((size_t)i);
        int total_len = (int)(sizeof(uint64) + sizeof(int)) + tag_len + data_len;
        bws.PutObject<int>(total_len);//total len
        bws.PutObject<uint64>((uint64)(i % kParall));//hash code
        bws.PutObject<int>(tag_len);//tag len
        bws.PutBuffer(kTestTag, tag_len);//tag
        //write data
        FillData((size_t)i, &bws);
        //printf("Gen Data: hash=%lu, tag_len=%d, tag=%s, data_len=%d, data=%s\n",
        //        i % kParall, (int)strlen(kTestTag), kTestTag,
        //        (int)data_[i].size(),
        //        data_[i].data());
        send_size += sizeof(int) + total_len;
    }
    BufHandle* hdl = bws.GetBegin();
    ASSERT_TRUE(hdl != NULL);

    transporter::TPDataMsg* pmsg = (transporter::TPDataMsg*)(hdl->pBuf);
    pmsg->size  = send_size;
    pmsg->batch = kDataNum;

    BufHandle* ack_hdl = NULL;
    ASSERT_EQ(pe_cli_->SyncReq(transporter::TPE_OF_RECV_LOCALPE,
                               hdl, &ack_hdl),
              error::OK);
    ASSERT_TRUE(ack_hdl != NULL);
    ChainFreeHdl(ack_hdl, NULL);
    ChainFreeHdl(hdl, NULL);
    ack_hdl = NULL;
    hdl = NULL;

    //wait
    TPTestWait();

    //check
    transporter::Transporter::OutflowMap::iterator it;
    it = tp_->m_outflow_map.find(EXTRACT_PROCESSOR_ID(down_id_));
    ASSERT_NE(it, tp_->m_outflow_map.end());
    transporter::TPOutflow* outflow = it->second;
    ASSERT_TRUE(NULL != outflow);
    for (uint64_t i = 0; i < kDataNum; ++i) {
        size_t queue_index = i % kParall;
        transporter::TPAckQueue* queue = outflow->m_branches[queue_index].queue;
        ASSERT_TRUE(NULL != queue);
        int tag_len   = (int)(strlen(kTestTag) + 1);
        int data_len  = (int)data_[i].size();
        int expect_size = (int)(sizeof(int) + sizeof(uint64) + sizeof(int))
                          + tag_len + data_len;
        int size  = expect_size;
        int batch = 1;
        CBufWriteStream w;
        uint64_t ack = transporter::kAdvanceQueueAck;
        ASSERT_EQ(queue->Front(&w, &size, &batch, &ack), error::OK) << "i=" << i;
        queue->PopFront(ack);
        ASSERT_EQ(size, expect_size) << "i=" << i;
        ASSERT_EQ(batch, 1) << "i=" << i;
        CBufReadStream r(w.GetBegin(), NULL);
        ASSERT_TRUE(r.GetObject<int>(&size)) << "i=" << i;
        ASSERT_EQ(size, expect_size - (int)sizeof(int)) << "i=" << i;
        uint64 hash;
        ASSERT_TRUE(r.GetObject<uint64>(&hash)) << "i=" << i;
        ASSERT_TRUE(r.GetObject<int>(&size)) << "i=" << i;
        ASSERT_EQ(size, tag_len) << "i=" << i;
        char buf[transporter::kMaxTagSize];
        ASSERT_EQ(r.GetBuffer(buf, tag_len), tag_len) << "i=" << i;
        ASSERT_STREQ(buf, kTestTag) << "i=" << i;
        ASSERT_EQ(r.GetBuffer(buf, data_len), data_len) << "i=" << i;
        buf[data_len] = '\0';
        ASSERT_STREQ(buf, data_[i].c_str()) << "i=" << i;
    }
}
