#include <transporter/transporter.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_stubmgr.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::AtLeast;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::MockFunction;
using ::testing::InSequence;
//using ::testing::SetArgReferee;
//using ::testing::SetArgumentPointee;

using namespace dstream;

typedef std::vector<transporter::IfSourceMeta> TPIfSourceVec;
typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}
class MockTransporter : public transporter::Transporter {
public:
    MockTransporter(uint64_t id, ESPSTUB stub, ESPSTUB if_stub = INVALID_ESPSTUB)
        : transporter::Transporter(id, stub, if_stub) {}

    MOCK_METHOD1(_OnStop,       void(AsyncContext* ctx));
    MOCK_METHOD1(_OnAddOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnDelOutflow, void(transporter::OfMgrCtx* ctx));
    MOCK_METHOD1(_OnUpdOutflow, void(transporter::OfMgrCtx* ctx));

    void MockOnStop(AsyncContext* ctx) {
        transporter::Transporter::_OnStop(ctx);
        TPTestComplete();
    }
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

    //MockFunction<void(std::string)> check_;
};

class test_Transporter_AddRefAndRelease_suite : public ::testing::Test {
protected:
    test_Transporter_AddRefAndRelease_suite(): tp_(NULL) {};
    virtual ~test_Transporter_AddRefAndRelease_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
public:
    transporter::Transporter* tp_;
};

TEST_F(test_Transporter_AddRefAndRelease_suite, AddRefAndRelease) {
    tp_ = new transporter::Transporter(5712, 5812);
    ASSERT_TRUE(tp_ != NULL);
    ASSERT_EQ(tp_->m_nRef, 1);
    ASSERT_EQ(tp_->AddRef(), 2);
    ASSERT_EQ(tp_->Release(), 1);
    ASSERT_EQ(tp_->Release(), 0);
    tp_ = NULL;
}

class test_Transporter_StartAndStop_suite : public ::testing::Test {
protected:
    test_Transporter_StartAndStop_suite(): tp_(NULL), m_id(kLocalId) {};
    virtual ~test_Transporter_StartAndStop_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
public:
    static const uint64_t kLocalId = 5712;
public:
    MockTransporter* tp_;
    uint64_t         m_id;
};

/**
 * @brief
 * @begin_version 1.0.0.1
**/
TEST_F(test_Transporter_StartAndStop_suite, WithoutInflow) {
    ESPSTUB stub = g_pTPStubMgr->AllocOfStub(m_id);
    ASSERT_NE(stub, INVALID_ESPSTUB);
    tp_ = new MockTransporter(m_id, stub);
    ASSERT_TRUE(tp_ != NULL);

    ASSERT_EQ(tp_->Start(), error::OK);
    ASSERT_EQ(tp_->m_running, true);
    ASSERT_TRUE(NULL == tp_->m_inflow);

    EXPECT_CALL(*(tp_), _OnStop(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnStop));

    TPTestStart();
    tp_->Stop();
    TPTestWait();
    ASSERT_EQ(tp_->m_running, false);

    tp_->Release();
    tp_ = NULL;

    g_pTPStubMgr->FreeOfStub(m_id);
}

TEST_F(test_Transporter_StartAndStop_suite, WithInflow) {
    ESPSTUB stub    = g_pTPStubMgr->AllocOfStub(m_id);
    ESPSTUB if_stub = g_pTPStubMgr->AllocIfStub(m_id);
    ASSERT_NE(stub,    INVALID_ESPSTUB);
    ASSERT_NE(if_stub, INVALID_ESPSTUB);
    tp_ = new MockTransporter(m_id, stub, if_stub);
    ASSERT_TRUE(tp_ != NULL);

    ASSERT_EQ(tp_->Start(), error::OK);
    ASSERT_EQ(tp_->m_running, true);
    ASSERT_TRUE(NULL != tp_->m_inflow);

    EXPECT_CALL(*(tp_), _OnStop(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnStop));
    TPTestStart();
    tp_->Stop();
    TPTestWait();
    ASSERT_EQ(tp_->m_running, false);

    tp_->Release();
    tp_ = NULL;

    g_pTPStubMgr->FreeOfStub(m_id);
    g_pTPStubMgr->FreeIfStub(m_id);
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

class test_Transporter_IfSourceMgr_suite : public ::testing::Test {
protected:
    test_Transporter_IfSourceMgr_suite(): tp_(NULL), m_id(kLocalId) {};
    virtual ~test_Transporter_IfSourceMgr_suite() {};
    virtual void SetUp() {
        g_pTPStubMgr->FreeOfStub(m_id);
        g_pTPStubMgr->FreeIfStub(m_id);
        ESPSTUB stub    = g_pTPStubMgr->AllocOfStub(m_id);
        ESPSTUB if_stub = g_pTPStubMgr->AllocIfStub(m_id);
        ASSERT_NE(stub,    INVALID_ESPSTUB);
        ASSERT_NE(if_stub, INVALID_ESPSTUB);
        tp_ = new transporter::Transporter(m_id, stub, if_stub);
        ASSERT_TRUE(NULL != tp_);
        ASSERT_EQ(tp_->Start(), error::OK);

        ASSERT_EQ(tp_->m_inflow->m_sources.size(), 0UL);
        //replace inflow with mock inflow
        tp_->m_inflow->_OnStop(NULL);
        tp_->m_inflow->Release();

        mock_inflow_ = new MockTPInflow(tp_, tp_->m_id, tp_->m_if_stub);
        tp_->m_inflow = mock_inflow_;
        ASSERT_TRUE(NULL != tp_->m_inflow);
        ASSERT_EQ(mock_inflow_->Start(), error::OK);
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            tp_->Stop();
            tp_->Release();
            tp_ = NULL;
        }
        mock_inflow_ = NULL;
        g_pTPStubMgr->FreeOfStub(m_id);
        g_pTPStubMgr->FreeIfStub(m_id);
    };
public:
    static const uint64_t kLocalId = 5712;
public:
    transporter::Transporter* tp_;
    uint64_t                  m_id;
    MockTPInflow*             mock_inflow_;
};

TEST_F(test_Transporter_IfSourceMgr_suite, AddIfSource) {
    EXPECT_CALL(*(mock_inflow_), AddSource(_, NULL))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockAddSource))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockAddSource));

    //add
    TPIfSourceVec* sources1 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources1);
    transporter::IfSourceMeta if_meta1 = {5771, 0xFFEEFFEE00010000};
    transporter::IfSourceMeta if_meta2 = {5772, 0xFFEEFFEE00020000};
    sources1->push_back(if_meta1);
    sources1->push_back(if_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->AddIfSource(sources1), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 2UL);

    //add again
    TPIfSourceVec* sources2 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources2);
    sources2->push_back(if_meta1);
    TPTestStart();
    ASSERT_EQ(tp_->AddIfSource(sources2), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 2UL);//duplicate removal
}

TEST_F(test_Transporter_IfSourceMgr_suite, DelIfSource) {
    EXPECT_CALL(*(mock_inflow_), AddSource(_, NULL))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockAddSource));
    EXPECT_CALL(*(mock_inflow_), DelSource(_, NULL))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockDelSource));

    //add
    TPIfSourceVec* sources1 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources1);
    transporter::IfSourceMeta if_meta1 = {5771, 0xFFEEFFEE00010000};
    transporter::IfSourceMeta if_meta2 = {5772, 0xFFEEFFEE00020000};
    sources1->push_back(if_meta1);
    sources1->push_back(if_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->AddIfSource(sources1), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 2UL);

    //del
    TPIfSourceVec* sources2 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources2);
    sources2->push_back(if_meta1);
    sources2->push_back(if_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->DelIfSource(sources2), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 0UL);
}

TEST_F(test_Transporter_IfSourceMgr_suite, UpdIfSource) {
    EXPECT_CALL(*(mock_inflow_), AddSource(_, NULL))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockAddSource));
    EXPECT_CALL(*(mock_inflow_), UpdSource(_, NULL))
    .WillOnce(Invoke(mock_inflow_, &MockTPInflow::MockUpdSource));

    //add
    TPIfSourceVec* sources1 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources1);
    transporter::IfSourceMeta if_meta1 = {5771, 0xFFEEFFEE00010000};
    transporter::IfSourceMeta if_meta2 = {5772, 0xFFEEFFEE00020000};
    sources1->push_back(if_meta1);
    sources1->push_back(if_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->AddIfSource(sources1), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 2UL);

    //upd
    TPIfSourceVec* sources2 = new TPIfSourceVec();
    ASSERT_TRUE(NULL != sources2);
    if_meta1.node = 0xFFEEFFEE00110000;
    if_meta2.node = 0xFFEEFFEE00220000;
    sources2->push_back(if_meta1);
    sources2->push_back(if_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->UpdIfSource(sources2), error::OK);
    TPTestWait();
    ASSERT_EQ(tp_->m_inflow->m_sources.size(), 2UL);
    transporter::TPInflow::TPSource* s;
    transporter::TPInflow::TPSourceMap::iterator it = tp_->m_inflow->m_sources.find(5771);
    ASSERT_TRUE(it != tp_->m_inflow->m_sources.end());
    s = it->second;
    ASSERT_TRUE(s != NULL);
    ASSERT_EQ(s->node, 0xFFEEFFEE00110000);
    it = tp_->m_inflow->m_sources.find(5772);
    ASSERT_TRUE(it != tp_->m_inflow->m_sources.end());
    s = it->second;
    ASSERT_TRUE(s != NULL);
    ASSERT_EQ(s->node, 0xFFEEFFEE00220000);
}

class test_Transporter_OutflowMgr_suite : public ::testing::Test {
protected:
    test_Transporter_OutflowMgr_suite(): tp_(NULL), m_id(kLocalId) {};
    virtual ~test_Transporter_OutflowMgr_suite() {};
    virtual void SetUp() {
        ESPSTUB stub = g_pTPStubMgr->AllocOfStub(m_id);
        ASSERT_NE(stub, INVALID_ESPSTUB);
        //tp_ = new transporter::Transporter(m_id, stub);
        tp_ = new MockTransporter(m_id, stub);
        ASSERT_TRUE(NULL != tp_);
        ASSERT_EQ(tp_->Start(), error::OK);
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            EXPECT_CALL(*(tp_), _OnStop(_))
            .WillOnce(Invoke(tp_, &MockTransporter::MockOnStop));
            TPTestStart();
            tp_->Stop();
            TPTestWait();
            tp_->Release();
            tp_ = NULL;
        }
        g_pTPStubMgr->FreeOfStub(m_id);
        g_pTPStubMgr->FreeIfStub(m_id);
    };
public:
    static const uint64_t kLocalId = 5712;
public:
    //transporter::Transporter* tp_;
    MockTransporter* tp_;
    uint64_t         m_id;
};

TEST_F(test_Transporter_OutflowMgr_suite, AddOutflow) {
    std::string s;
    transporter::TPTagMap::SubVecPtr subs;
    transporter::Transporter::OutflowMap::iterator it;

    ASSERT_EQ(tp_->m_outflow_map.size(), 0UL);

    //InSequence call_seq;
    EXPECT_CALL(*(tp_), _OnAddOutflow(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow));

    //add
    TPOfSourceVec* outflows1 = new TPOfSourceVec();
    ASSERT_TRUE(NULL != outflows1);
    std::vector<std::string> tags1;
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_1");
    tags1.push_back("tag_1_2");
    transporter::OfMeta of_meta1 = {5711, 10, tags1};
    std::vector<std::string> tags2;
    tags2.push_back("tag_0");
    tags2.push_back("tag_2_1");
    tags2.push_back("tag_2_2");
    transporter::OfMeta of_meta2 = {5712, 1, tags2};
    outflows1->push_back(of_meta1);
    outflows1->push_back(of_meta2);

    TPTestStart();
    ASSERT_EQ(tp_->AddOutflow(outflows1), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), (size_t)2);

    it = tp_->m_outflow_map.find(5711);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), (size_t)10);

    it = tp_->m_outflow_map.find(5712);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), (size_t)1);

    subs = tp_->m_tag_map.GetSubs("tag_0");
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), (size_t)2);
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());

    //add again, should not change
    TPOfSourceVec* outflows2 = new TPOfSourceVec();
    tags1.clear();
    tags2.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_2");
    tags2.push_back(config_const::kTPAllTag);
    transporter::OfMeta of_meta3 = {5711, 10, tags1};
    transporter::OfMeta of_meta4 = {5712, 5,  tags2};
    outflows2->push_back(of_meta3);
    outflows2->push_back(of_meta4);

    TPTestStart();
    ASSERT_EQ(tp_->AddOutflow(outflows2), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 2u);

    it = tp_->m_outflow_map.find(5711);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 10u);

    it = tp_->m_outflow_map.find(5712);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 1u);

    subs = tp_->m_tag_map.GetSubs("tag_1_2");
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), 1u);

    s.clear();
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());
}

TEST_F(test_Transporter_OutflowMgr_suite, DelOutflow) {
    std::string s;
    transporter::TPTagMap::SubVecPtr subs;
    transporter::Transporter::OutflowMap::iterator it;

    ASSERT_EQ(tp_->m_outflow_map.size(), 0u);

    std::vector<std::string> tags1;
    std::vector<std::string> tags2;

    EXPECT_CALL(*(tp_), _OnAddOutflow(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow));
    EXPECT_CALL(*(tp_), _OnDelOutflow(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnDelOutflow))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnDelOutflow));

    //del, should fail
    TPOfSourceVec* outflows0 = new TPOfSourceVec();
    tags1.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_2");
    transporter::OfMeta of_meta0 = {5711, 10, tags1};
    outflows0->push_back(of_meta0);

    TPTestStart();
    ASSERT_EQ(tp_->DelOutflow(outflows0), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 0u);

    ASSERT_EQ(tp_->m_tag_map.m_sub_map.size(), 0u);
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());

    //add
    TPOfSourceVec* outflows1 = new TPOfSourceVec();
    ASSERT_TRUE(NULL != outflows1);
    tags1.clear();
    tags2.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_1");
    tags1.push_back("tag_1_2");
    transporter::OfMeta of_meta1 = {5711, 10, tags1};
    tags2.push_back("tag_0");
    tags2.push_back("tag_2_1");
    tags2.push_back("tag_2_2");
    transporter::OfMeta of_meta2 = {5712, 1, tags2};
    outflows1->push_back(of_meta1);
    outflows1->push_back(of_meta2);

    TPTestStart();
    ASSERT_EQ(tp_->AddOutflow(outflows1), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 2u);

    it = tp_->m_outflow_map.find(5711);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 10u);

    it = tp_->m_outflow_map.find(5712);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 1u);

    subs = tp_->m_tag_map.GetSubs("tag_0");
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), 2u);
    s.clear();
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());

    //del, should OK
    TPOfSourceVec* outflows2 = new TPOfSourceVec();
    tags1.clear();
    tags2.clear();
    transporter::OfMeta of_meta3 = {5711, 2, tags1};
    transporter::OfMeta of_meta4 = {5712, 1,  tags2};
    outflows2->push_back(of_meta3);
    outflows2->push_back(of_meta4);
    TPTestStart();
    ASSERT_EQ(tp_->DelOutflow(outflows2), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 0u);
    ASSERT_EQ(tp_->m_tag_map.m_sub_map.size(), 0u);
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());
}

TEST_F(test_Transporter_OutflowMgr_suite, UpdOutflow) {
    std::string s;
    transporter::TPTagMap::SubVecPtr subs;
    transporter::Transporter::OutflowMap::iterator it;

    ASSERT_EQ(tp_->m_outflow_map.size(), 0u);

    std::vector<std::string> tags1;
    std::vector<std::string> tags2;

    EXPECT_CALL(*(tp_), _OnAddOutflow(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnAddOutflow));
    EXPECT_CALL(*(tp_), _OnUpdOutflow(_))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnUpdOutflow))
    .WillOnce(Invoke(tp_, &MockTransporter::MockOnUpdOutflow));

    //update, should fail
    TPOfSourceVec* outflows0 = new TPOfSourceVec();
    tags1.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_2");
    transporter::OfMeta of_meta0 = {5711, 10, tags1};
    outflows0->push_back(of_meta0);

    TPTestStart();
    ASSERT_EQ(tp_->UpdOutflow(outflows0), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 0u);

    ASSERT_EQ(tp_->m_tag_map.m_sub_map.size(), 0u);
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());

    //add
    TPOfSourceVec* outflows1 = new TPOfSourceVec();
    ASSERT_TRUE(NULL != outflows1);
    tags1.clear();
    tags2.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_1");
    tags1.push_back("tag_1_2");
    transporter::OfMeta of_meta1 = {5711, 10, tags1};
    tags2.push_back("tag_0");
    tags2.push_back("tag_2_1");
    tags2.push_back("tag_2_2");
    transporter::OfMeta of_meta2 = {5712, 1, tags2};
    outflows1->push_back(of_meta1);
    outflows1->push_back(of_meta2);
    TPTestStart();
    ASSERT_EQ(tp_->AddOutflow(outflows1), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 2u);

    it = tp_->m_outflow_map.find(5711);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 10u);

    it = tp_->m_outflow_map.find(5712);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 1u);

    subs = tp_->m_tag_map.GetSubs("tag_0");
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), 2u);
    s.clear();
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());

    //update, should change
    TPOfSourceVec* outflows2 = new TPOfSourceVec();
    tags1.clear();
    tags2.clear();
    tags1.push_back("tag_0");
    tags1.push_back("tag_1_2");
    tags2.push_back(config_const::kTPAllTag);
    transporter::OfMeta of_meta3 = {5711, 10, tags1};
    transporter::OfMeta of_meta4 = {5712, 5,  tags2};
    outflows2->push_back(of_meta3);
    outflows2->push_back(of_meta4);
    TPTestStart();
    ASSERT_EQ(tp_->UpdOutflow(outflows2), error::OK);
    TPTestWait();

    ASSERT_EQ(tp_->m_outflow_map.size(), 2u);

    it = tp_->m_outflow_map.find(5711);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 10u);

    it = tp_->m_outflow_map.find(5712);
    ASSERT_NE(it, tp_->m_outflow_map.end());
    ASSERT_TRUE(NULL != it->second);
    ASSERT_EQ(it->second->m_branches.size(), 5u);

    subs = tp_->m_tag_map.GetSubs("tag_1_2");
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), 2u);

    s.clear();
    ASSERT_EQ(tp_->m_tag_map.Dump(&s), error::OK);
    printf("TagMap: %s\n", s.c_str());
}
