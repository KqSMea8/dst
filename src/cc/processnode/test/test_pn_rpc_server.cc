/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/7/17
* @filename:    test_pn_rpc_server.cc
* @author:      lanbijia
* @brief:       test pn rpc server
*/

#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/zk_client.h"
#include "common/rpc_server.h"
#include "common/application_tools.h"

#include "metamanager/zk_config.h"
#include "metamanager/zk_meta_manager.h"
#include "metamanager/meta_manager.h"

#include "processnode/config_map.h"
#include "processnode/zk_wrapper.h"
#include "processnode/pn_rpc_server.h"
#include "processnode/pn_reporter.h"
#include "processnode/process_node.h"

using namespace std;
using namespace dstream;
using namespace dstream::rpc_server;
using namespace dstream::processnode;
using namespace dstream::pn_rpc_server;
using namespace dstream::application_tools;



namespace dstream {
namespace processnode {

class TestPNRPCServer : public ::testing::Test
{
protected:
    void InitTest(const string& conf_file);
    void DestroyTest();

    void ConstructFakeApp(Application& app, int process_num, int paralism);

    void PNDebugRPCServerTest();
    int32_t MockDebugRPCCall(const std::string& debug_cmd);

    void RecvPMRPCServerTest();
    int32_t MockPMCreatePERPCCall(const PEID& peid, const BackupPE& backup_pe,
                                  const Processor& processor);
    int32_t MockPMKillPERPCCall(const PEID& peid, const BackupPE& backup_pe);

    void RecvPERPCServerTest();
    int32_t MockPEReadyRPCCall();
    int32_t MockPEHeartbeatRPCCall();

private:
    PN pn_meta;
    int32_t pm_work_thread_num;
    int32_t pe_work_thread_num;
    int32_t pn_work_thread_num;
    int32_t debug_work_thread_num;

    string pm_host_;
    int32_t pm_port_;

};

void TestPNRPCServer::ConstructFakeApp(Application& app, int process_num, int paralism)
{
    timeval now;
    gettimeofday(&now, NULL);
    app.mutable_id()->set_id(1);
    app.set_name("test");
    app.set_descr("used for test");
    User* app_user = app.mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app.mutable_topology();
    topology->set_processor_num(process_num);
    char process_name[1000];
    for (int i = 0; i < process_num; ++i) {
        Processor* processor = topology->add_processors();
        sprintf(process_name, "test_process_%d", i);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        processor->set_role(WORK_PROCESSOR);
        processor->set_exec_cmd("sleep 3");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(paralism);
        if (i > 0) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            sprintf(process_name, "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
    }
    app.set_status(RUN);
}


void TestPNRPCServer::InitTest(const string& conf_file) {
    // read config file
    if (g_pn_cfg_map.InitCfgMapFromFile(conf_file.c_str()) < error::OK) {
        DSTREAM_WARN("fail to read config from[%s]", conf_file.c_str());
        return ;
    }
    //debug
    g_pn_cfg_map.Dump();

    // create zk client
    g_zk = new ZK(&g_pn_cfg_map);

    if (error::OK != GetLocalhostIP(&pm_host_)) { /* get host ip */
        DSTREAM_WARN("fail to get local host ip");
        return ;
    }
    g_pn_cfg_map.GetIntValue(config_const::kPNDebugListenPortName, &pm_port_);

    PM pm_meta;
    pm_meta.set_host(pm_host_);
    pm_meta.set_report_port(pm_port_);
    g_zk->GetMetaManager()->AddPM(pm_meta);

    // gen pn id
    int32_t port;
    std::string host;
    g_pn_cfg_map.GetValue(config_const::kLocalHostIPName, &host);
    pn_meta.set_host(host);
    // set pn <-> pm port
    g_pn_cfg_map.GetIntValue(config_const::kPNPMListenThreadNumName, &pm_work_thread_num);
    g_pn_cfg_map.GetIntValue(config_const::kPNListenPortName, &port);
    pn_meta.mutable_pn_id()->set_id("mock_pn");
    pn_meta.set_rpc_server_port(port);
    // set pn <-> pn port
    g_pn_cfg_map.GetIntValue(config_const::kPNPNListenThreadNumName, &pn_work_thread_num);
    g_pn_cfg_map.GetIntValue(config_const::kPNPNListenPortName, &port);
    pn_meta.set_sub_port(port);
    // set pn <-> pe port
    g_pn_cfg_map.GetIntValue(config_const::kPNPEListenThreadNumName, &pe_work_thread_num);
    g_pn_cfg_map.GetIntValue(config_const::kPNPEListenPortName, &port);
    pn_meta.set_pub_port(port);
    // set pn debug port
    g_pn_cfg_map.GetIntValue(config_const::kPNDebugListenThreadNumName, &debug_work_thread_num);
    g_pn_cfg_map.GetIntValue(config_const::kPNDebugListenPortName, &port);
    pn_meta.set_debug_port(port);
    // set importer port
    g_pn_cfg_map.GetIntValue(config_const::kImporterPortName, &port);
    pn_meta.set_importer_port(port);

    ProcessNode::InitProcessNodeContext();
    ProcessNode::GetProcessNode()->SetPNMeta(pn_meta);
}

void TestPNRPCServer::DestroyTest() {
    // stop pn report service
    sleep(2);

    ProcessNode::DestoryProcessNode();

    g_zk->GetMetaManager()->DeletePM();

    if (g_zk) {
        delete g_zk;
        g_zk = NULL;
    }
}

int32_t TestPNRPCServer::MockDebugRPCCall(const std::string& debug_cmd) {
    rpc::Connection conn;
    std::string pn_uri = rpc_const::GetUri(pn_meta.host(), pn_meta.debug_port());
    DSTREAM_DEBUG("debug pn : [%s]\n", pn_uri.c_str());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_ERROR("connect to pn failed\n");
        return error::UNPOSSIBLE;
    }
    DSTREAM_DEBUG("connect to pn ...OK\n");

    proto_rpc::RPCRequest< DebugPNRequest > req;
    proto_rpc::RPCResponse< DebugPNACK > res;
    req.set_method(rpc_const::kDebugPNMethod);
    // set cmd & args
    req.set_cmd(debug_cmd);

    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_ERROR("rpc all to pn failed\n");
        return error::UNPOSSIBLE;
    }

    return error::OK;
}

void TestPNRPCServer::PNDebugRPCServerTest() {
    // create pn debugger server
    PNDebugRPCServer pn_debuger_server(ProcessNode::GetProcessNode());
    ASSERT_EQ(true, pn_debuger_server.StartServer(pn_meta.debug_port(), pn_meta.host(),
                                                  debug_work_thread_num));

    ASSERT_EQ(error::OK, MockDebugRPCCall("allpe"));
    ASSERT_EQ(error::OK, MockDebugRPCCall("conn"));
    ASSERT_EQ(error::OK, MockDebugRPCCall("queue"));
    ASSERT_EQ(error::OK, MockDebugRPCCall("pe_id_list"));
    ASSERT_EQ(error::OK, MockDebugRPCCall("exception_queue"));

    pn_debuger_server.StopServer();
}

int32_t TestPNRPCServer::MockPMCreatePERPCCall(const PEID& peid, const BackupPE& backup_pe,
                                               const Processor& processor) {
    rpc::Connection conn;
    std::string pn_uri = rpc_const::GetUri(pn_meta.host(), pn_meta.rpc_server_port());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_ERROR("connect to pn failed\n");
        return error::UNPOSSIBLE;
    }
    DSTREAM_DEBUG("connect to pn ...OK\n");

    proto_rpc::RPCRequest<CreatePEMessage> req;
    proto_rpc::RPCResponse< CreatePEResult > res;
    AppID app_id;
    app_id.set_id(application_tools::app_id(peid));
    *(req.mutable_app_id()) = app_id;
    *(req.mutable_pe_id()) = peid;
    *(req.mutable_backup_id()) = backup_pe.backup_pe_id();
    *(req.mutable_processor()) = processor;
    req.set_method(rpc_const::kCreatePEMethod);

    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_ERROR("CreatePE rpc call to pn failed\n");
        return error::UNPOSSIBLE;
    }

    conn.Close();
    return error::OK;
}

int32_t TestPNRPCServer::MockPMKillPERPCCall(const PEID& peid, const BackupPE& backup_pe) {
    rpc::Connection conn;
    std::string pn_uri = rpc_const::GetUri(pn_meta.host(), pn_meta.rpc_server_port());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_ERROR("connect to pn failed\n");
        return error::UNPOSSIBLE;
    }
    DSTREAM_DEBUG("connect to pn ...OK\n");

    proto_rpc::RPCRequest<PEOperationMessage> req;
    proto_rpc::RPCResponse< PEOperationResult > res;
    PEOperationMessage_OperationType type = PEOperationMessage_OperationType_KILL;
    req.set_type(type);
    AppID app_id;
    app_id.set_id(application_tools::app_id(peid));
    *(req.mutable_app_id()) = app_id;
    *(req.mutable_pe_id()) = peid;
    *(req.mutable_backup_pe_id()) = backup_pe.backup_pe_id();
    req.set_method(rpc_const::kPEOperationMethod);

    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_ERROR("KillPE rpc call to pn failed\n");
        return error::UNPOSSIBLE;
    }

    conn.Close();
    return error::OK;
}



void TestPNRPCServer::RecvPMRPCServerTest() {
    PNPMRPCServer recv_pm_server(ProcessNode::GetProcessNode());
    ASSERT_EQ(true, recv_pm_server.StartServer(pn_meta.rpc_server_port(), pn_meta.host(),
                                               pm_work_thread_num));

    Application valid_app;
    PE_LIST::iterator iter;
    PE_LIST pe_list;
    ConstructFakeApp(valid_app, 3, 1);
    g_zk->GetMetaManager()->DeleteApplication(valid_app.id());

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->AddApplication(&valid_app));
    DSTREAM_DEBUG("construct fake app");
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(valid_app.id(), &pe_list));
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // mock select primary pe
        BackupPE* backup_pe = iter->mutable_backups(0);
        BackupPEID bakcup_peid;
        bakcup_peid.set_id( iter->pe_id().id() );
        *(backup_pe->mutable_pn_id()) = pn_meta.pn_id();
        *(backup_pe->mutable_backup_pe_id()) = bakcup_peid;
        iter->mutable_primary(0)->set_id( iter->pe_id().id() );
        ASSERT_EQ(error::OK, g_zk->GetMetaManager()->UpdateProcessElement(valid_app.id(), *iter));
    }
    DSTREAM_DEBUG("add app & process_element");

    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        Processor processor;
        ASSERT_EQ(true, application_tools::GetProcessor(valid_app, *iter, &processor));
        ASSERT_EQ(error::OK, MockPMCreatePERPCCall(iter->pe_id(), iter->backups(0), processor));
    }

    sleep(5);

    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        ASSERT_EQ(error::OK, MockPMKillPERPCCall(iter->pe_id(), iter->backups(0)));
    }

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->DeleteApplication(valid_app.id()));

    recv_pm_server.StopServer();
}

int32_t TestPNRPCServer::MockPEReadyRPCCall() {
    rpc::Connection conn;
    std::string pn_uri = rpc_const::GetUri(pn_meta.host(), pn_meta.pub_port());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_WARN("connect(%s) failed", pn_uri.c_str());
        return error::UNPOSSIBLE;
    }
    proto_rpc::RPCRequest< ReadyRequest> req;
    proto_rpc::RPCResponse< ReadyACK> res;
    req.set_method(rpc_const::kPubSubReadyMethod);
    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_WARN("PE Ready RPC call(%s) failed", rpc_const::kPubSubReadyMethod);
        return error::UNPOSSIBLE;
    }
    conn.Close();

    return error::OK;
}

int32_t TestPNRPCServer::MockPEHeartbeatRPCCall() {

    PNReporter pn_reporter_inst(&g_pn_cfg_map, 1024 * 10, 16);
    g_pn_reporter = &pn_reporter_inst;

    if (!pn_reporter_inst.Start()) {
        DSTREAM_DEBUG("start pn reporter fail");
        return error::UNPOSSIBLE;
    }

    rpc::Connection conn;
    std::string pn_uri = rpc_const::GetUri(pn_meta.host(), pn_meta.pub_port());

    if (conn.Connect(pn_uri.c_str()) < 0) {
        DSTREAM_WARN("connect(%s) failed", pn_uri.c_str());
        return error::UNPOSSIBLE;
    }
    proto_rpc::RPCRequest< HeartbeatRequest> req;
    proto_rpc::RPCResponse< HeartbeatACK> res;
    req.set_method(rpc_const::kPubSubHeartbeatMethod);
    req.mutable_peid()->set_id(1);
    if (conn.Call(&req, &res, 3000) < 0) {
        DSTREAM_WARN("PE Heartbeat RPC call(%s) failed", rpc_const::kPubSubHeartbeatMethod);
        return error::UNPOSSIBLE;
    }
    conn.Close();

    pn_reporter_inst.Stop();
    g_pn_reporter = NULL;

    return error::OK;
}

void TestPNRPCServer::RecvPERPCServerTest() {
    PNPERPCServer recv_pe_server(ProcessNode::GetProcessNode());
    ASSERT_EQ(true, recv_pe_server.StartServer(pn_meta.pub_port(), pn_meta.host(), pe_work_thread_num));

    ASSERT_EQ(error::OK, MockPEReadyRPCCall());
    ASSERT_EQ(error::OK, MockPEHeartbeatRPCCall());

    recv_pe_server.StopServer();
}



TEST_F(TestPNRPCServer, test_debug_rpc_server)
{
    InitTest("test_pn_rpc_server.conf.xml");

    PNDebugRPCServerTest();

    DestroyTest();
}

TEST_F(TestPNRPCServer, test_recv_pm_rpc_server)
{
    InitTest("test_pn_rpc_server.conf.xml");

    RecvPMRPCServerTest();

    DestroyTest();
}

TEST_F(TestPNRPCServer, test_recv_pe_rpc_server)
{
    InitTest("test_pn_rpc_server.conf.xml");

    RecvPERPCServerTest();

    DestroyTest();
}



}
}


int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_pn_rpc_server");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}


