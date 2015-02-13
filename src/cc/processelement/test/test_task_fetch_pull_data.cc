/**
* @file     test_task_fetch_pull_data.cc
* @brief    PE tranportor test
* @author   konghui, konghui@baidu.com
* @version  1.5.0
* @date     2013-02-18
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_inflow.h"
#include "transporter/transporter.h"
#include "ESP.h"
#include "ESPObject.h"
#include "common/config_const.h"
#include "processelement/task.h"
#include "echo.pb.h"
#include "SyncObj.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::MockFunction;
using ::testing::InSequence;

using namespace dstream;
using namespace dstream::processelement;

void InitData();
typedef ProtoTuple< EchoRequest > InTuple;
typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;

CCond g_tp_test_cond;
static const uint64_t kParall     = 2UL;
static const uint64_t kLocalId    = MAKE_PEID(1UL, 1UL, 0UL);
static const uint64_t kDownPEId   = MAKE_PEID(1UL, 2UL, 0UL);

static const uint64_t kDataNum    = 50;
static       int32_t  kDataSize;
static const uint64_t kMaxDataLen = 1024;
static const char*    kTestTag    = "TestTag";
std::vector<std::string> datas;

typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

void InitData() {
    if (0 == datas.size()) {
        for (uint64_t i = 0; i < kDataNum; i++) {
            std::string s;

            uint64 hash = 0;
            int    tag_len = strlen(kTestTag) + 1;
            s.append((const char*)&hash, sizeof(hash)); //hash
            s.append((const char*)&tag_len, sizeof(tag_len)); //tag len
            s.append(kTestTag, tag_len); //tag

            std::string tuple_str = "tuple#";
            StringAppendNumber(&tuple_str, i);

            InTuple tuple;
            tuple.set_hash_code(hash);
            tuple.set_tag(kTestTag);
            tuple.set_field(tuple_str);
            std::string raw_tuple;
            STLStringByteArray byte_array(&raw_tuple);
            tuple.Serialize(&byte_array);

            s += raw_tuple;

            datas.push_back(s);
            kDataSize += sizeof(int); //int len
            kDataSize += (int32_t)s.size();
        }
    }
    printf("kDataSize=%d\n", kDataSize);
}

void FillDataToWriteStream(CBufWriteStream* bws,
                           INOUT int32_t*   size,
                           OUT   int32_t*   batch) {
    int32_t  min = *size < kDataSize ? *size : kDataSize;
    int32_t  n;
    uint64_t i;
    for (n = 0, i = 0; n + (int)sizeof(int) + (int)datas[i].size() < min; i++) {
        bws->PutObject<int>((int)datas[i].size());
        bws->PutBuffer(datas[i].data(), datas[i].size());
        n += sizeof(int) + datas[i].size();
    }
    *size  = n;
    *batch = i;
}
void CheckData(Tuples& tuples) {
    for (size_t i = 0; i < tuples.size(); i++) {
        InTuple* tuple = (InTuple*)tuples[i];
        ASSERT_EQ(tuple->hash_code(), 0UL);
        ASSERT_STREQ(tuple->tag().c_str(), kTestTag);

        size_t offset = sizeof(uint64) + sizeof(int) + strlen(kTestTag) + 1;
        std::string raw_tuple(datas[i].substr(offset));
        STLStringByteArray byte_array(&raw_tuple);
        InTuple source_tuple;
        ASSERT_TRUE(source_tuple.Deserialize(&byte_array));

        ASSERT_TRUE(tuple->field() == source_tuple.field());
        //printf("Get Tuple: hash=%lu, tag=%s, tuple.field=%s\n",
        //       tuple->hash_code(), tuple->tag().c_str(),
        //       tuple->field().c_str());
    }
}
void CheckData(int index, std::string& s) {
    ASSERT_EQ(datas[index], s);
}

class MockTransporter : public CESPObject {
    ESP_DECLARE_OBJECT(MockTransporter);
public:
    MockTransporter(uint64_t id, ESPSTUB of_stub, ESPSTUB if_stub) :
        m_id(id), if_stub_(if_stub), of_stub_(of_stub) {}
    ~MockTransporter() {
        Stop();
    }
    int Start() {
        VERIFY_OR_RETURN(ESPE_OK == BindStub(if_stub_), error::BIND_STUB_FAIL);
        VERIFY_OR_RETURN(ESPE_OK == BindStub(of_stub_), error::BIND_STUB_FAIL);
        return error::OK;
    }
    void Stop() {
        UnbindStub();
    }
    void OnFetchData(ESPMessage* req_msg) {
        CBufReadStream  brs(req_msg->req.pHdl, NULL);
        transporter::TPDataMsg msg;
        ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));

        CBufWriteStream bws;
        bws.PutObject<transporter::TPDataMsg>(msg);

        FillDataToWriteStream(&bws, &msg.size, &msg.batch);
        BufHandle* phdl  = bws.GetBegin();
        transporter::TPDataMsg* ack_msg = (transporter::TPDataMsg*)(phdl->pBuf);
        ack_msg->from_id = m_id;
        ack_msg->to_id   = msg.from_id;
        ack_msg->err     = transporter::TPN_ACK_OK;
        ack_msg->ack     = msg.ack;//not used
        ack_msg->size    = msg.size;
        ack_msg->batch   = msg.batch;
        ack_msg->chksum  = 0xFFFEEEEEEEEEEEEE;//CheckSum(phdl);//TOOD
        ASSERT_EQ(PostMsg(req_msg->aFrom.addr,
                          transporter::TPE_IF_LOCALPE_FETCH_ACK,
                          req_msg->lParam, phdl),
                  error::OK);
    }
    void OnOutflowRecv(ESPMessage* req_msg) {
        CBufReadStream  brs(req_msg->req.pHdl, NULL);
        transporter::TPDataMsg msg;
        ASSERT_TRUE(brs.GetObject<transporter::TPDataMsg>(&msg));
        for (int i = 0; i < msg.batch; i++) {
            int len = 0;
            ASSERT_TRUE(brs.GetObject<int>(&len));
            std::string s(len, '\0');
            ASSERT_EQ(brs.GetBuffer((char*)s.data(), len), len);
            CheckData(i, s);
        }
        msg.err     = transporter::TPN_ACK_OK;
        msg.chksum  = 0xFFFEEEEEEEEEEEEE;//CheckSum(hdl);//TOOD
        CBufWriteStream bws;
        ASSERT_TRUE(bws.PutObject<transporter::TPDataMsg>(msg));
        ASSERT_EQ(PostMsg(req_msg->aFrom.addr,
                          transporter::TPE_OF_RECV_LOCALPE,
                          req_msg->lParam, bws.GetBegin()),
                  error::OK);
    }

private:
    uint64_t m_id;
    ESPSTUB  if_stub_;
    ESPSTUB  of_stub_;
};

ESP_BEGIN_MAP_MESSAGE(MockTransporter, CESPObject)
ESP_MAP_MESSAGE_RAW(transporter::TPE_OF_RECV_LOCALPE,  OnOutflowRecv)
ESP_MAP_MESSAGE_RAW(transporter::TPE_IF_LOCALPE_FETCH, OnFetchData)
ESP_END_MAP_MESSAGE

class EchoTask : public Task {
public:
    EchoTask() {
        m_id = kLocalId;
        //set task env
        m_pe_type = internal::Context::PE_TYPE_NORMAL;
        m_tuples_batch_size = 1024 * 1024 * 1024; //1G, unlimited
        char ipport[64];
        snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
        m_pn_ipport = ipport;
        //register type
        registerTupleTypeInfo(kTestTag, new InTupleInfo());
    }
    void FakeInit() {
        ASSERT_TRUE(m_counters.Init(m_id));
    }
    virtual void onTuples(const Tuples& tuples) {
        for (size_t i = 0; i < tuples.size(); i++) {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
            InTuple* out_tuple = dynamic_cast<InTuple*>(emit(kTestTag));
            *out_tuple = *in_tuple;
        }
    }
};

EchoTask* task = NULL;

int main(int argc, char** argv)
{
    logger::initialize("pe_test");
    transporter::TPInitKylin();
    KLSetLog(stdout, 0, NULL);
    g_pTPStubMgr->Start();
    common::Counters::Init();

    InitData();
    task = new EchoTask();
    task->FakeInit();
    int ret = error::OK;

    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    g_pTPStubMgr->Stop();
    transporter::TPStopKylin();

    delete task;
    task = NULL;

    common::Counters::Deinit();

    return ret;
}

class test_Task_Data_suite : public ::testing::Test {
protected:
    test_Task_Data_suite(): m_id(kLocalId), down_id_(kDownPEId) {};
    virtual ~test_Task_Data_suite() {};
    virtual void SetUp() {
        //start transporter
        g_pTPStubMgr->FreeIfStub(m_id);
        g_pTPStubMgr->FreeOfStub(m_id);
        if_stub_ = g_pTPStubMgr->AllocIfStub(m_id);
        of_stub_ = g_pTPStubMgr->AllocOfStub(m_id);
        ASSERT_NE(if_stub_, INVALID_ESPSTUB);
        ASSERT_NE(of_stub_, INVALID_ESPSTUB);

        tp_ = new MockTransporter(m_id, of_stub_, if_stub_);
        ASSERT_TRUE(NULL != tp_);
        ASSERT_EQ(tp_->Start(), error::OK);
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            tp_->Stop();
            tp_->Release();
            tp_ = NULL;
        }
        g_pTPStubMgr->FreeIfStub(m_id);
        g_pTPStubMgr->FreeOfStub(m_id);
    };
public:
    uint64_t         m_id;
    uint64_t         down_id_;

    ESPSTUB          if_stub_;
    ESPSTUB          of_stub_;
    MockTransporter* tp_;
};

TEST_F(test_Task_Data_suite, fetch_and_forward) {
    ASSERT_EQ(task->StartTransporter(), error::OK);
    //fetch data
    ASSERT_GE(task->NormalWorkflow(), error::OK);
    std::string s;
    DumpTuples(task->m_send_tuples, &s);
    //printf("send tuples: %s\n", s.c_str());

    //check send tuples
    CheckData(task->m_send_tuples);
    //forward data
    ASSERT_GE(task->ForwardLog(), error::OK);
}
