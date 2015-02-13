/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <google/malloc_extension.h>

#include "common/config.h"
#include "common/config_const.h"
#include "common/counters.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/mem_mgr.h"
#include "common/rpc_server.h"
#include "common/zk_client.h"

#include "metamanager/meta_manager.h"
#include "metamanager/zk_config.h"
#include "metamanager/zk_meta_manager.h"

#include "processnode/config_map.h"
#include "processnode/pn_httpd.h"
#include "processnode/pn_reporter.h"
#include "processnode/pn_rpc_server.h"
#include "processnode/process_node.h"
#include "processnode/router_mgr.h"
#include "processnode/zk_wrapper.h"

#include "transporter/transporter_common.h"
#include "transporter/transporter_counters.h"

using dstream::processnode::g_zk;
using dstream::processnode::g_pn_cfg_map;
using dstream::processnode::g_pn_reporter;
using dstream::processnode::g_quit;

static const uint64_t g_PrintStatusInteval = 10;
static const double g_MemoryRate = 0.7;

bool ExtractPortFromURI(const std::string& uri, int32_t* port) {
    std::string::size_type idx = uri.rfind(':');
    if (idx == std::string::npos) {
        return false;
    }
    *port = strtol(uri.c_str() + idx + 1, NULL, 10);
    return true;
}

void PrintUsage(const char* prj) {
    printf("Usage:\n");
    printf("  %s config_file\n", prj);
    printf("  %s --version\n", prj);
}

void sigterm_handler(int signal) {
    if (g_zk) {
        g_zk->UnRegister();
    }
    DSTREAM_WARN("PN exit for signal [%d]", signal);
    // kill self
    kill(getpid(), SIGKILL);
}

void GenPNID(std::string ip, dstream::PNID* id) {
    std::string str = ip + "_";
    time_t timer = time(NULL);
    dstream::StringAppendNumber(&str, timer);
    id->set_id(str);
}

static void PrintStat(int intv) {
    dstream::transporter::TPPrintStatus(intv);
    // tcmalloc status
    printf("%s\n", dstream::mem_mgr::MemMgr::Instance()->DumpMemStat().c_str());
}

int main(int argc, char** argv) {
    int ret = dstream::error::OK;
    dstream::logger::initialize("processnode");
    if (!dstream::common::Counters::Init()) {
        DSTREAM_ERROR("Failed to init counters, exit");
        return 1;
    }

    dstream::transporter::g_pCounterDumper = new dstream::transporter::TPCounterDumper();
    ret = dstream::transporter::g_pCounterDumper->Start();
    if (ret < dstream::error::OK) {
        DSTREAM_ERROR("Failed to init counters");
        return 1;
    }

    if (argc < 2) {
        PrintUsage(argv[0]);
        return 0;
    }

    std::string cmd_string(argv[1]);
    std::string conf_file;
    if ("--version" == cmd_string || "-v" == cmd_string) {
        printf("[%s]: --version [%s], --build-time [%s]\n",
                argv[0], VERSION, BUILD_TIME);
        return 0;
    } else {
        conf_file = cmd_string;
    }

    // read conf file
    ret = dstream::processnode::g_pn_cfg_map.InitCfgMapFromFile(conf_file.c_str());
    if (ret < dstream::error::OK) {
        DSTREAM_WARN("fail to read config from[%s]", conf_file.c_str());
        return 1;
    }
    // dump config info
    dstream::processnode::g_pn_cfg_map.Dump();

    // gen pn id
    dstream::PN pn_meta;
    int32_t port;
    int32_t pm_work_thread_num;
    int32_t pe_work_thread_num;
    int32_t debug_work_thread_num;
    std::string str_val;
    // set pn <-> pm port
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNPMListenThreadNumName, &pm_work_thread_num);
    // set pn <-> pe port
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNPEListenThreadNumName, &pe_work_thread_num);
    // set pn debug port
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNDebugListenThreadNumName, &debug_work_thread_num);
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNDebugListenPortName, &port);
    pn_meta.set_debug_port(port);
    // set pn version
    pn_meta.set_version(VERSION);

    // default tranbuffer config
    int small_tranbuf_num = dstream::config_const::kDefaultPNSmallTranBufNum;
    int small_tranbuf_size = dstream::config_const::kDefaultPNSmallTranBufSize;
    int big_tranbuf_num = dstream::config_const::kDefaultPNBigTranBufNum;
    int write_water_mark = dstream::config_const::kDefaultPNWriteWaterMark;
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNSmallTranBufNum, &small_tranbuf_num);
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNSmallTranBufSize, &small_tranbuf_size);
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNBigTranBufNum, &big_tranbuf_num);
    dstream::processnode::g_pn_cfg_map.GetIntValue(
            dstream::config_const::kPNWriteWaterMark, &write_water_mark);

    dstream::transporter::TPSetTranBuf(small_tranbuf_num,
            small_tranbuf_size, big_tranbuf_num, write_water_mark);

    // Init kylin/Esp
    if (!dstream::transporter::TPInitKylin()) {
        DSTREAM_WARN("fail to init kylin/esp");
        return 1;
    }
    KLSetLog(stdout, 0, NULL);
    ret = g_pTPStubMgr->Start();
    if (ret < dstream::error::OK) {
        DSTREAM_WARN("fail to start TPStub manager, err(%d)", ret);
        return 1;
    }
    pn_meta.set_sub_port(dstream::transporter::g_esp_port);

    // Init AppRouter Mgr
    dstream::router::RouterMgr* pAppMgr = dstream::router::RouterMgr::Instance();
    if (NULL == pAppMgr) {
        DSTREAM_ERROR("fail to get app router manager");
        return 1;
    }
    pAppMgr->SetConfigFile(conf_file);

    // create zk wrapper
    g_zk = new dstream::processnode::ZK(&dstream::processnode::g_pn_cfg_map);

    // create pn
    dstream::processnode::ProcessNode::InitProcessNodeContext();

    // create recv_pe_server
    dstream::pn_rpc_server::PNPERPCServer recv_pe_server(
            dstream::processnode::ProcessNode::GetProcessNode());
    if (!recv_pe_server.StartServer(pe_work_thread_num)) {
        DSTREAM_ERROR("recv_pe_server StartServer fail.");
        return 1;
    }
    pn_meta.set_host(recv_pe_server.server_ip());
    GenPNID(pn_meta.host(), pn_meta.mutable_pn_id());
    pn_meta.set_pub_port(recv_pe_server.server_port());
    str_val = dstream::rpc_const::GetUri(pn_meta.host(), pn_meta.pub_port());
    dstream::processnode::g_pn_cfg_map.SetValue(
            dstream::config_const::kPNPEListenUriName, str_val);

    // create recv_pm_server
    dstream::pn_rpc_server::PNPMRPCServer recv_pm_server(
            dstream::processnode::ProcessNode::GetProcessNode());
    if (!recv_pm_server.StartServer(pm_work_thread_num)) {
        DSTREAM_ERROR("recv_pm_server StartServer fail.");
        return 1;
    }
    pn_meta.set_rpc_server_port(recv_pm_server.server_port());
    str_val = dstream::rpc_const::GetUri(pn_meta.host(), pn_meta.rpc_server_port());
    dstream::processnode::g_pn_cfg_map.SetValue(
            dstream::config_const::kPNListenUriName, str_val);

    // create pn debugger server
    dstream::pn_rpc_server::PNDebugRPCServer pn_debuger_server(
            dstream::processnode::ProcessNode::GetProcessNode());
    if (!pn_debuger_server.StartServer(
                pn_meta.debug_port(), pn_meta.host(), debug_work_thread_num)) {
        DSTREAM_ERROR("pn_debuger_server StartServer fail.");
        return 1;
    }

    // for httpd server, get the port
    // start httpd server
    dstream::pn_httpd::PnHttpdResponder responder;
    void * httpd_context = NULL;
    uint32_t pn_httpd_port = dstream::config_const::kMinDynamicHttpdPort;
    for(; pn_httpd_port < dstream::config_const::kMaxDynamicHttpdPort; ++pn_httpd_port) {
        std::string httpd_port = "";
        dstream::StringAppendNumber(&httpd_port, pn_httpd_port);
        if(dstream::httpd::HttpdStart(httpd_port, &httpd_context, &responder) < 
                                                dstream::error::OK) {
            DSTREAM_WARN("could not start pn httpd server on port [%u]", pn_httpd_port);
        } else {
            break;
        }
        usleep(1000);
    }
    if (pn_httpd_port >= dstream::config_const::kMaxDynamicHttpdPort) {
        DSTREAM_WARN("unable start pn httpd server");
        return -1;
    }
    pn_meta.set_httpd_port(pn_httpd_port);
    // set pn meta
    dstream::processnode::ProcessNode::GetProcessNode()->SetPNMeta(pn_meta);

    // trick : sleep for service port bind
    usleep(1000000);
    if (!recv_pm_server.running() ||
        !pn_debuger_server.running() ||
        !recv_pe_server.running()) {
        DSTREAM_ERROR("fail to start PN service");
        return 1;
    }

    // register pn on zookeeper after service started(port bind)
    DSTREAM_INFO("register pn to zookeeper...");
    ret  = g_zk->Register(pn_meta);
    if (ret < dstream::error::OK) {
        DSTREAM_ERROR("fail to register pn to zookeeper");
        return 2;
    }

    // register signal terms
    signal(SIGTERM, sigterm_handler);

    // get total memory config
    uint64_t machine_free_memory = 0;
    uint64_t machine_total_memory = 0;
    int processor_num = 0;
    dstream::GetMachineInfo(&processor_num, &machine_total_memory, &machine_free_memory);

    uint64_t total_pe_memory = static_cast<uint64_t>(-1);
    std::string memory_config;
    if (g_pn_cfg_map.GetValue(
                dstream::config_const::kPNTotalMemory, &memory_config) == dstream::error::OK) {
        total_pe_memory = dstream::GetConfigMemory(&memory_config);
        DSTREAM_DEBUG("Get Config Memory [%s] [%lu]", memory_config.c_str(), total_pe_memory);
    }
    if (total_pe_memory == static_cast<uint64_t>(-1)) {
        // get config fail, use default setting
        total_pe_memory = static_cast<uint64_t>(machine_free_memory * g_MemoryRate);
    }
    DSTREAM_INFO("PE-use Memory:[%lu], CPU:[%u]", total_pe_memory, processor_num);
    uint64_t buffer_memory = static_cast<uint64_t>(-1);
    memory_config.clear();
    if (g_pn_cfg_map.GetValue(
                dstream::config_const::kPNBufferMemory, &memory_config) == dstream::error::OK) {
        buffer_memory = dstream::GetConfigMemory(&memory_config);
        DSTREAM_DEBUG("Get Config Buffer Memory [%s] [%lu]", memory_config.c_str(), buffer_memory);
    }
    if (buffer_memory == static_cast<uint64_t>(-1)) {
        // get config fail, use default setting
        buffer_memory = static_cast<uint64_t>(machine_free_memory * (1.0 - g_MemoryRate));
    }
    DSTREAM_INFO("PN-buffer Memory:[%lu]", buffer_memory);

    uint64_t total_cpu_core = static_cast<uint64_t>(processor_num); 
    std::string cpu_config;
    if (g_pn_cfg_map.GetValue(
                dstream::config_const::kPNTotalCPU, &cpu_config) == dstream::error::OK) {
        total_cpu_core = atoi(cpu_config.c_str());
        DSTREAM_DEBUG("Get Config CPU [%s] [%lu]", cpu_config.c_str(), total_cpu_core);
    }
    DSTREAM_INFO("PN-use CPU:[%lu]", total_cpu_core);
 
    dstream::mem_mgr::MemMgr::MemMgrPtr inst = dstream::mem_mgr::MemMgr::Instance();
    if (inst) {
        DSTREAM_INFO("init pn mem mgr...");
        inst->Init(buffer_memory/*mem that PN use internally*/);
    }

    // create report sender thread
    DSTREAM_INFO("start pn reporter...");
    g_pn_reporter = NULL;
    dstream::processnode::PNReporter pn_reporter(&g_pn_cfg_map, total_pe_memory/*mem PE can use*/, total_cpu_core);
    if (!pn_reporter.Start()) {
        DSTREAM_ERROR("fail to start pn reporter");
        return 1;
    } else {
        g_pn_reporter = &pn_reporter;
    }

    // ALL start work end!!!

    // print status
    uint64_t n = 0;
    while (!AtomicGetValue(g_quit)) {
        sleep(1);
        if (g_PrintStatusInteval && 0 == (n++ % g_PrintStatusInteval)) {
            PrintStat(g_PrintStatusInteval);
        }
    }
    dstream::httpd::HttpdStop(httpd_context);
    // wait for thread exiting
    g_pn_reporter->Join();

    recv_pe_server.StopServer();
    DSTREAM_WARN("recv_pe_server stop.");

    recv_pm_server.StopServer();
    DSTREAM_WARN("recv_pm_server stop.");

    pn_debuger_server.StopServer();
    DSTREAM_WARN("pn_debuger_server stop.");

    // destory PE process first
    dstream::processnode::ProcessManager::GetInstance()->DestroyAllProcesses();

    dstream::processnode::ProcessNode::DestoryProcessNode();

    delete dstream::router::RouterMgr::Instance();

    g_pTPStubMgr->Stop();
    dstream::transporter::TPStopKylin();

    delete dstream::processnode::ProcessManager::GetInstance();

    if (g_zk) {
        g_zk->UnRegister();

        delete g_zk;
        g_zk = NULL;
    }

    dstream::transporter::g_pCounterDumper->Stop();

    if (!dstream::common::Counters::Deinit()) {
        DSTREAM_WARN("failed to deinit counters");
    }
    DSTREAM_INFO("Exiting ProcessNode.");

    // we simply sleep here for cleanup
    // actually we should join the gc thread of each pe wrapper
    // but we didn't expose a interface
    sleep(5);

    dstream::logger::close();

    return 0;
}
