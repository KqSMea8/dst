/********************************************************************
    created:    26:12:2011
    author:     zhanggengxin@baidu.com

    purpose:    test for rpc server
*********************************************************************/

#include <gtest/gtest.h>
#include <set>

#include "common/proto_rpc.h"
#include "common/rpc_client.h"
#include "common/rpc_const.h"
#include "common/rpc_server.h"

namespace dstream {
namespace test_rpc_server {
using namespace proto_rpc;

class PNReportCompare
{
public:
    bool operator()(const PNReport& pn_report1, const PNReport& pn_report2)
    {
        return (pn_report1.id().id() < pn_report2.id().id());
    }
};

class AsyncPNReportHandler : public rpc_client::ProtoAsyncRPCHandler<ReportResponse>
{
public:
    void set_expect_res(int res) {
        expect_res_ = res;
    }
    void HandleResponse(int call_res, const ReportResponse* response, const std::string& return_val);
    void HandleResponse(int call_res, const rpc::Request*, const ReportResponse* response,
                        const std::string& return_val) {}
private:
    int expect_res_;
};

void AsyncPNReportHandler::HandleResponse(int call_res,
                                          const ReportResponse* /*response*/,
                                          const std::string& return_val)
{
    if (expect_res_ == 0) {
        ASSERT_EQ(0, call_res);
        ASSERT_STREQ(return_val.c_str(), "ok");
    } else {
        ASSERT_NE(0, call_res);
    }
}

typedef std::set<PNReport, PNReportCompare> PN_REPORT_SET;
typedef PN_REPORT_SET::iterator PN_REPORT_SET_ITER;

class PNReportRPCServer : public rpc_server::RPCServer, public rpc_const::PNReportProtocol
{
public:
    virtual std::string handleRequest(const PNReport* req, ReportResponse* res);
    bool HasPNReport(const PNID& pn_id);
private:
    PN_REPORT_SET pn_reports_;
};

std::string PNReportRPCServer::handleRequest(const PNReport* req,
                                             ReportResponse* res)
{
    DSTREAM_INFO("pn report id %s", req->id().id().c_str());
    DSTREAM_DEBUG("pn report is %s", req->DebugString().c_str());
    pn_reports_.insert(*req);
    res->set_next_report_interval(10);
    return "ok";
}

bool PNReportRPCServer::HasPNReport(const PNID& pn_id)
{
    PNReport report;
    *(report.mutable_id()) = pn_id;
    return pn_reports_.find(report) != pn_reports_.end();
}

class TestRPCServer : public ::testing::Test
{
protected:
    void TearDown();
    void TestStartServer();
    void TestRPCReport();
    void TestAsyncRPCClient();
    PNReportRPCServer server_;
};

void TestRPCServer::TestStartServer()
{
    ASSERT_TRUE(server_.StartServer());
    ASSERT_TRUE(server_.running());
    PNReportRPCServer other_server;
    ASSERT_FALSE(other_server.StartServer(server_.server_port(), "*"));
    ASSERT_FALSE(other_server.running());
    ASSERT_TRUE(other_server.StartServer());
    ASSERT_TRUE(other_server.running());
    server_.StopServer();
    other_server.StopServer();
    ASSERT_FALSE(server_.running());
    ASSERT_FALSE(other_server.running());
}

void TestRPCServer::TearDown()
{
    server_.StopServer();
}

void TestRPCServer::TestRPCReport()
{
    ASSERT_TRUE(server_.StartServer());
    server_.RegisterProtocol(rpc_const::kPNReport, (rpc_const::PNReportProtocol*)&server_);
    std::string uri = rpc_const::GetUri(server_.server_ip(), server_.server_port());
    PNReport report;
    PNID pn_id;
    *(pn_id.mutable_id()) = "localhost";
    *(report.mutable_id()) = pn_id;
    ReportResponse response;
    std::string res;
    ASSERT_EQ(0, rpc_client::rpc_call(uri.c_str(), rpc_const::kPNReport, report, response, &res));
    ASSERT_EQ(10u, response.next_report_interval());
    ASSERT_STREQ(res.c_str(), "ok");
    ASSERT_TRUE(server_.HasPNReport(pn_id));
}

void TestRPCServer::TestAsyncRPCClient()
{
    ASSERT_TRUE(server_.StartServer());
    server_.RegisterProtocol(rpc_const::kPNReport, (rpc_const::PNReportProtocol*)&server_);
    std::string uri = rpc_const::GetUri(server_.server_ip(), server_.server_port());
    PNReportRPCServer other_server;
    ASSERT_TRUE(other_server.StartServer());
    other_server.RegisterProtocol(rpc_const::kPNReport, (rpc_const::PNReportProtocol*)&other_server);
    rpc_client::AsyncRPCClient async_client;
    char id[100];
    for (int i = 0; i < 10; ++i) {
        RPCRequest<PNReport>* rpc_report = new RPCRequest<PNReport>;
        PNID pn_id;
        sprintf(id, "test_%d", i);
        *(pn_id.mutable_id()) = id;
        *(rpc_report->mutable_id()) = pn_id;
        AsyncPNReportHandler* handler = new AsyncPNReportHandler;
        handler->set_expect_res(0);
        ASSERT_TRUE(async_client.call(uri.c_str(), rpc_const::kPNReport, rpc_report, handler));
    }
    uri = rpc_const::GetUri(other_server.server_ip(), other_server.server_port());
    for (int i = 20; i < 40; ++i) {
        RPCRequest<PNReport>* rpc_report = new RPCRequest<PNReport>;
        PNID pn_id;
        sprintf(id, "localtest_%d", i);
        *(pn_id.mutable_id()) = id;
        *(rpc_report->mutable_id()) = pn_id;
        AsyncPNReportHandler* handler = new AsyncPNReportHandler;
        handler->set_expect_res(0);
        BackupPEStatus* status = rpc_report->add_status();
        status->add_metric_name("test");
        status->add_metric_name("test");
        status->clear_metric_name();
        status->clear_metric_value();
        status->add_metric_name("new test");
        status->add_metric_value("new value");
        status->add_metric_name("test data");
        status->add_metric_value("test data vaule");
        status = rpc_report->add_status();
        status->add_metric_name("second name");
        ASSERT_TRUE(async_client.call(uri.c_str(), rpc_const::kPNReport, rpc_report, handler));
    }
    async_client.wait();
    RPCRequest<PNReport>* rpc_report = new RPCRequest<PNReport>;
    PNID pn_id;
    sprintf(id, "localtest_%d", 90);
    *(pn_id.mutable_id()) = id;
    *(rpc_report->mutable_id()) = pn_id;
    AsyncPNReportHandler* handler = new AsyncPNReportHandler;
    handler->set_expect_res(-1);
    ASSERT_TRUE(async_client.call(uri.c_str(), rpc_const::kPNReport, rpc_report, handler));
    other_server.StopServer();
    async_client.wait();
}

TEST_F(TestRPCServer, test_server_start)
{
    TestStartServer();
}

TEST_F(TestRPCServer, test_rpc_server_report)
{
    TestRPCReport();
}

TEST_F(TestRPCServer, test_async_client)
{
    TestAsyncRPCClient();
}

}//namespace test_rpc_server
}//namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_rpc_server");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

