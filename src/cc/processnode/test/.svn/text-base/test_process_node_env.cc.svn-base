#include "test_process_node_env.h"
#include "common/counters.h"
#include "processnode/router_mgr.h"

#include "Kylin.h"
#include "ESP.h"

using namespace dstream;

void ProcessNodeEnv::SetUp() {
    common::Counters::Init();
    transporter::g_pCounterDumper = new dstream::transporter::TPCounterDumper();
    if (transporter::g_pCounterDumper->Start() < error::OK) {
        DSTREAM_ERROR("Failed to init counters");
        return ;
    }
    transporter::TPInitKylin();
    KLSetLog(stdout, 0, NULL);
    g_pTPStubMgr->Start();
    router::RouterMgr::Instance();
}

void ProcessNodeEnv::TearDown() {
    delete router::RouterMgr::Instance();
    g_pTPStubMgr->Stop();
    transporter::TPStopKylin();
    transporter::g_pCounterDumper->Stop();
    common::Counters::Deinit();
}
