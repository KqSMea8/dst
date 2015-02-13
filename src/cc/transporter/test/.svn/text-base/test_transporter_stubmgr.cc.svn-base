#include <boost/foreach.hpp>
#include <gtest/gtest.h>

#include "Kylin.h"
#include "ESP.h"
#include "BufStream.h"

#include "transporter_env.h"
#include "common/logger.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_stubmgr.h"


using namespace dstream;
//using namespace dstream::transporter;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}

class test_TPStubMgr_suite : public ::testing::Test {
protected:
    test_TPStubMgr_suite() {};
    virtual ~test_TPStubMgr_suite() {};
    virtual void SetUp() {
        logger::set_level(Logging::WARN_LOG_LEVEL);
    };
    virtual void TearDown() {
    };
};

TEST_F(test_TPStubMgr_suite, StubAllocAndFree) {
    const uint64_t kNum = 30000;

    std::vector<ESPSTUB>  stubs;

    stubs.resize(kNum);
    //alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        stubs[i] = g_pTPStubMgr->AllocIfStub(i);
        ASSERT_NE(stubs[i], INVALID_ESPSTUB);
    }

    //alloc again, should fail
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(g_pTPStubMgr->AllocIfStub(i), INVALID_ESPSTUB);
    }

    //free and alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        g_pTPStubMgr->FreeIfStub(i);
        ESPSTUB stub = g_pTPStubMgr->AllocIfStub(i);
        ASSERT_NE(stub, INVALID_ESPSTUB);
    }

    //alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        stubs[i] = g_pTPStubMgr->AllocOfStub(i);
        ASSERT_NE(stubs[i], INVALID_ESPSTUB);
    }

    //alloc again, should fail
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(g_pTPStubMgr->AllocOfStub(i), INVALID_ESPSTUB);
    }

    //free and alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        g_pTPStubMgr->FreeOfStub(i);
        ESPSTUB stub = g_pTPStubMgr->AllocOfStub(i);
        ASSERT_NE(stub, INVALID_ESPSTUB);
    }

    //free and alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        g_pTPStubMgr->FreeIfStub(i);
        ESPSTUB stub = g_pTPStubMgr->AllocIfStub(i);
        ASSERT_NE(stub, INVALID_ESPSTUB);
    }

    //free and alloc, should ok
    for (uint64_t i = 0; i < kNum; ++i) {
        g_pTPStubMgr->FreeOfStub(i);
        ESPSTUB stub = g_pTPStubMgr->AllocOfStub(i);
        ASSERT_NE(stub, INVALID_ESPSTUB);
    }

    //free all
    for (uint64_t i = 0; i < kNum; ++i) {
        g_pTPStubMgr->FreeOfStub(i);
        g_pTPStubMgr->FreeIfStub(i);
    }
}

TEST_F(test_TPStubMgr_suite, QueryStubIfAndOF) {
    //get addr
    transporter::TPStubMsg stub_msg;
    char ipport[64];
    snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
    ESPNODE node = ESPFindNode(ipport);
    ASSERT_NE(node, INVALID_ESPNODE);

    const uint64_t of_id = 5712;
    const uint64_t if_id = 5713;

    //alloc of stub
    g_pTPStubMgr->FreeOfStub(of_id);
    ESPSTUB of_stub = g_pTPStubMgr->AllocOfStub(of_id);

    //query of stub
    stub_msg.from_id = if_id;
    stub_msg.to_id   = of_id;
    BufHandle* ack_hdl = NULL;
    transporter::TPStubMsg* pmsg;
    CBufWriteStream bws_if;
    bws_if.PutObject<transporter::TPStubMsg>(stub_msg);

    int ret;
    transporter::TPCli* if_cli = new transporter::TPCli();
    ret = if_cli->Init(MakeESPADDR(node, transporter::kTPStubMgrStub));
    ASSERT_EQ(ret, error::OK) << error::ErrStr(ret);
    ASSERT_EQ(if_cli->SyncReq(transporter::TPE_SM_QUERY_OF, bws_if.GetBegin(), &ack_hdl), error::OK);
    pmsg = (transporter::TPStubMsg*)(ack_hdl->pBuf);
    ASSERT_EQ(pmsg->stub, of_stub);
    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
    if_cli->Destroy();
    if_cli = NULL;

    //alloc if stub
    g_pTPStubMgr->FreeIfStub(if_id);
    ESPSTUB if_stub = g_pTPStubMgr->AllocIfStub(if_id);

    //query if stub
    const uint64_t pe_id = 5714;
    stub_msg.from_id = pe_id;
    stub_msg.to_id   = if_id;
    CBufWriteStream bws_pe;
    bws_pe.PutObject<transporter::TPStubMsg>(stub_msg);

    transporter::TPCli* pe_cli = new transporter::TPCli();
    ret = pe_cli->Init(MakeESPADDR(node, transporter::kTPStubMgrStub));
    ASSERT_EQ(ret, error::OK) << error::ErrStr(ret);
    ASSERT_EQ(pe_cli->SyncReq(transporter::TPE_SM_QUERY_IF, bws_pe.GetBegin(), &ack_hdl), error::OK);
    pmsg = (transporter::TPStubMsg*)(ack_hdl->pBuf);
    ASSERT_EQ(pmsg->stub, if_stub);
    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
    pe_cli->Destroy();
    pe_cli = NULL;
}

/**
* @brief    test of QueryStub fail
*
* @param    test_TPStubMgr_suite
* @param    QueryStubFail
* @author   fangjun,fangjun02@baidu.com
* @date     2013-03-09
*/
TEST_F(test_TPStubMgr_suite, QueryStubFail) {
    //get addr
    transporter::TPStubMsg stub_msg;
    char ipport[64];
    snprintf(ipport, sizeof(ipport), "%s:%d", "127.0.0.1", transporter::g_esp_port);
    ESPNODE node = ESPFindNode(ipport);
    ASSERT_NE(node, INVALID_ESPNODE);

    const uint64_t of_id = 5715;
    const uint64_t if_id = 5713;

    // we do not alloc of stub here
    g_pTPStubMgr->FreeOfStub(of_id);
    // ESPSTUB of_stub = g_pTPStubMgr->AllocOfStub(of_id);

    //query of stub
    stub_msg.from_id = if_id;
    stub_msg.to_id   = of_id;
    BufHandle* ack_hdl = NULL;
    transporter::TPStubMsg* pmsg;
    CBufWriteStream bws_if;
    bws_if.PutObject<transporter::TPStubMsg>(stub_msg);

    transporter::TPCli* if_cli = new transporter::TPCli();
    ASSERT_EQ(if_cli->Init(MakeESPADDR(node, transporter::kTPStubMgrStub)), error::OK);
    ASSERT_EQ(if_cli->SyncReq(transporter::TPE_SM_QUERY_OF, bws_if.GetBegin(), &ack_hdl), error::OK);
    pmsg = (transporter::TPStubMsg*)(ack_hdl->pBuf);
    ASSERT_EQ(pmsg->stub, INVALID_ESPSTUB);
    ASSERT_EQ(pmsg->err, transporter::TPN_STUB_QUERY_NO_STUB);
    ChainFreeHdl(ack_hdl, NULL);
    ack_hdl = NULL;
    if_cli->Destroy();
    if_cli = NULL;
}
