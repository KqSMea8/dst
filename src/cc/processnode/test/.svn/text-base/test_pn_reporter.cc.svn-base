#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/utils.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/rpc_server.h"

#include "processnode/config_map.h"
#include "processnode/pn_reporter.h"
#include "processnode/zk_wrapper.h"
#include "processnode/pe_wrapper_manager.h"
#include "processnode/process_node.h"

using namespace std;
using namespace dstream;
using namespace dstream::rpc_const;
using namespace dstream::rpc_server;
using namespace dstream::application_tools;
using namespace dstream::processnode;

namespace dstream {
namespace processnode {

class PNReportRPCServer;


class TestPNReporter : public ::testing::Test
{
public:
    void SetUp();
    void TearDown();

    void SetFailSize(uint32_t num) {
        fail_pe_size_ = num;
    }
    void SetStatusSize(uint32_t num) {
        status_size_ = num;
    }
    void SetQueueSize(uint32_t num) {
        queue_size_ = num;
    }
    uint32_t GetFailSize() {
        return fail_pe_size_;
    }
    uint32_t GetStatusSize() {
        return status_size_;
    }
    uint32_t GetQueueSize() {
        return queue_size_;
    }

    void IncReportNum() {
        report_num_++;
    }
    uint32_t ReportNum() {
        return report_num_;
    }
    void ClearReportNum() {
        report_num_ = 0;
    }

protected:
    uint32_t InitTest(const string& conf_file);
    void PNReporterTest();
    void PNReporterTestFast();
    void MockWorkingPE(const uint32_t& num);
    void MockFailPE(const uint32_t& num);
    uint32_t ClearWorkingPE();
    uint32_t ClearFailPE();
    void DestoryTest();

private:
    string pm_host_;
    int32_t pm_port_;
    PNReportRPCServer* pn_report_server_ptr_;

    // validate
    volatile uint32_t fail_pe_size_;
    volatile uint32_t status_size_;
    volatile uint32_t queue_size_;
    volatile uint32_t report_num_;
};

// mock service
class PNReportRPCServer : public RPCServer,
    public PNReportProtocol
{
public:
    PNReportRPCServer(TestPNReporter* test_pn_ptr)
        : test_pn_ptr_(test_pn_ptr)
    {
        RegisterProtocol(kPNReport, (PNReportProtocol*)this);
    }
    std::string PNREPORT(const PNReport* req, ReportResponse* ack);

private:
    TestPNReporter* test_pn_ptr_;
};

std::string PNReportRPCServer::PNREPORT(const PNReport* req, ReportResponse* /*ack*/) {
    test_pn_ptr_->SetFailSize(req->fails_size());
    test_pn_ptr_->SetStatusSize(req->status_size());
    test_pn_ptr_->SetQueueSize(req->queues_size());
    test_pn_ptr_->IncReportNum();

    return "OK";
}

void TestPNReporter::SetUp() {
    if (0 != InitTest("test_pn_reporter.conf.xml")) {
        exit(1);
    }
}

void TestPNReporter::TearDown() {
    DestoryTest();
}

uint32_t TestPNReporter::InitTest(const string& conf_file)
{
    // read config file
    if (g_pn_cfg_map.InitCfgMapFromFile(conf_file.c_str()) < error::OK) {
        DSTREAM_WARN("fail to read config from[%s]", conf_file.c_str());
        return 1;
    }
    //debug
    g_pn_cfg_map.Dump();

    if (error::OK != GetLocalhostIP(&pm_host_)) { /* get host ip */
        DSTREAM_WARN("fail to get local host ip");
        return 1;
    }
    g_pn_cfg_map.GetIntValue(config_const::kPNDebugListenPortName, &pm_port_);

    // create zk client

    g_zk = new ZK(&g_pn_cfg_map);

    g_zk->GetMetaManager()->DeletePM();

    PM pm_meta;
    pm_meta.set_host(pm_host_);
    pm_meta.set_report_port(pm_port_);
    g_zk->GetMetaManager()->AddPM(pm_meta);

    // create pn
    PN pn_meta;
    ProcessNode::InitProcessNodeContext();
    ProcessNode::GetProcessNode()->SetPNMeta(pn_meta);

    // start mock service
    pn_report_server_ptr_ = new PNReportRPCServer(this);
    if (!pn_report_server_ptr_->StartServer(pm_port_, pm_host_, 1)) {
        DSTREAM_ERROR("pn_report_server StartServer fail.");
        return 1;
    }

    status_size_ = 0;
    fail_pe_size_ = 0;
    queue_size_ = 0;
    ClearReportNum();

    return 0;
}

void TestPNReporter::DestoryTest() {
    // stop pn report service
    sleep(2);
    pn_report_server_ptr_->StopServer();
    if (pn_report_server_ptr_) {
        delete pn_report_server_ptr_;
        pn_report_server_ptr_ = NULL;
    }

    ProcessNode::DestoryProcessNode();

    g_zk->GetMetaManager()->DeletePM();

    if (g_zk) {
        delete g_zk;
        g_zk = NULL;
    }

}


void TestPNReporter::MockWorkingPE(const uint32_t& num) {
    CompeletePEID cpeid;
    AppID app_id;
    PEID peid;
    Processor processor;
    BackupPEID bk_peid;

    *(cpeid.mutable_app_id())    = app_id;
    *(cpeid.mutable_processor()) = processor;
    *(cpeid.mutable_backup_id()) = bk_peid;

    for (uint32_t i = 0; i < num; ++i) {
        PEWrapper* wrapper = new PEWrapper();
        wrapper->set_cpeid(cpeid);
        peid.set_id(i + 1);
        wrapper->set_peid(peid);
        *(cpeid.mutable_pe_id()) = peid;

        PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().insert(std::pair<uint64_t, PEWrapper*>
                (peid.id(), wrapper));
    }
}

void TestPNReporter::MockFailPE(const uint32_t& num) {
    CompeletePEID cpeid;
    AppID app_id;
    PEID peid;
    Processor processor;
    BackupPEID bk_peid;

    *(cpeid.mutable_app_id())    = app_id;
    *(cpeid.mutable_processor()) = processor;
    *(cpeid.mutable_backup_id()) = bk_peid;

    for (uint32_t i = 0; i < num; ++i) {
        PEWrapper* wrapper = new PEWrapper();
        wrapper->set_cpeid(cpeid);
        peid.set_id(i + 1);
        wrapper->set_peid(peid);
        *(cpeid.mutable_pe_id()) = peid;

        PEWrapperManager::GetPEWrapperManager()->GetFailPEWrapperMap().insert(
            std::pair<uint64_t, PEWrapper*>(peid.id(), wrapper));
    }
}

uint32_t TestPNReporter::ClearWorkingPE() {
    uint32_t num = PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().size();
    PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().clear();
    return num;
}

uint32_t TestPNReporter::ClearFailPE() {
    uint32_t num = PEWrapperManager::GetPEWrapperManager()->GetFailPEWrapperMap().size();
    PEWrapperManager::GetPEWrapperManager()->GetFailPEWrapperMap().clear();

    return num;
}

void TestPNReporter::PNReporterTest()
{
    // do testing
    PNReporter pn_reporter_inst(&g_pn_cfg_map, 1024 * 10, 16);

    MockWorkingPE(2);
    MockFailPE(1);

    if (pn_reporter_inst.Start()) {
        DSTREAM_DEBUG("start pn reporter over");
    }

    // wait for one report
    while (ReportNum() < 1) {
        usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 100));
    }
    // sleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 1.5));
    ASSERT_EQ(ReportNum(), 1u);

    // check normal pe & fail pe number
    ASSERT_EQ(GetStatusSize(), 2u);
    ASSERT_EQ(GetFailSize(), 1u);

    while (ReportNum() < 2) {
        usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 100));
    }
    // sleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 1.5));
    ASSERT_EQ(ReportNum(), 2u);

    // check normal pe & fail pe number
    ASSERT_EQ(GetStatusSize(), 2u);
    ASSERT_EQ(GetFailSize(), 1u); // auto clear fail pe !!!

    ASSERT_EQ(ClearWorkingPE(), 2u);
    ASSERT_EQ(ClearFailPE(), 1u);

    while (ReportNum() < 3) {
        usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 100));
    }
    // sleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 1.5));
    ASSERT_EQ(ReportNum(), 3u);

    ASSERT_EQ(GetStatusSize(), 0u);
    ASSERT_EQ(GetFailSize(), 0u);

    pn_reporter_inst.set_pm_reconnect();

    while (ReportNum() < 4) {
        usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 100));
    }
    // sleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 1.5));
    ASSERT_EQ(ReportNum(), 4u);

    // pn_reporter_inst.GetPEStatus();

    pn_reporter_inst.Stop();
}

void TestPNReporter::PNReporterTestFast()
{
    // do testing
    PNReporter pn_reporter_inst(&g_pn_cfg_map, 1024 * 10, 16);

    MockWorkingPE(2);
    MockFailPE(1);

    if (pn_reporter_inst.Start()) {
        DSTREAM_DEBUG("start pn reporter over");
    }

    // wait for one report
    while (ReportNum() < 1) {
        usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 100));
    }
    ASSERT_EQ(ReportNum(), 1u);

    // check normal pe & fail pe number
    ASSERT_EQ(GetStatusSize(), 2u);
    ASSERT_EQ(GetFailSize(), 1u);

    ASSERT_EQ(ClearWorkingPE(), 2u);
    ASSERT_EQ(ClearFailPE(), 1u);
    pn_reporter_inst.SignalCond();

    usleep(static_cast<uint32_t>(config_const::kDefaultReportIntervalSec * 1000 * 10));
    ASSERT_EQ(ReportNum(), 2u);

    ASSERT_EQ(GetStatusSize(), 0u);
    ASSERT_EQ(GetFailSize(), 0u);

    pn_reporter_inst.Stop();
}

TEST_F(TestPNReporter, test_pn_reporter)
{
    PNReporterTest();
}

TEST_F(TestPNReporter, test_pn_reporter_fast)
{
    PNReporterTestFast();
}


}// end processnode
} // end dstream

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_pn_reporter");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


