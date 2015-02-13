#include "Trace.h"
#include "transporter_env.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_counters.h"

using namespace dstream;

void TransporterEnv::SetUp() {
    logger::initialize("transporter_test");
    common::Counters::Init();
    transporter::g_pCounterDumper = new dstream::transporter::TPCounterDumper();
    if (transporter::g_pCounterDumper->Start() < error::OK) {
        DSTREAM_ERROR("Failed to init counters");
        return ;
    }

    transporter::TPInitKylin();
    KLSetLog(stdout, 0, NULL);
    g_pTPStubMgr->Start();
}

void TransporterEnv::TearDown() {
    g_pTPStubMgr->Stop();
    transporter::TPStopKylin();
    transporter::g_pCounterDumper->Stop();
    common::Counters::Deinit();
}
