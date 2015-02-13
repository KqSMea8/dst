#include <gtest/gtest.h>

#include "common/logger.h"
#include "common/utils.h"
#include "common/rpc.h"
#include "common/rpc_const.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/zk_client.h"
#include "common/application_tools.h"

#include "metamanager/zk_config.h"
#include "metamanager/meta_manager.h"

#include "processnode/config_map.h"
#include "processnode/zk_wrapper.h"
#include "processnode/pe_wrapper.h"
#include "processnode/process_node.h"
#include "processnode/pe_wrapper_manager.h"
#include "processnode/router_mgr.h"

#include "test_process_node_env.h"

using namespace std;
using namespace dstream;
using namespace dstream::rpc;
using namespace dstream::processnode;

namespace dstream {
namespace processnode {

class MockPEWrapperUser : public thread::Thread
{
public:
    MockPEWrapperUser(std::vector<uint64_t>& peid_list) : peid_list_(peid_list) {};
    ~MockPEWrapperUser() {};

    void Run();

private:
    std::vector<uint64_t> peid_list_;
};

void MockPEWrapperUser::Run() {
    int size = peid_list_.size();
    while (!m_stop) {
        // mock use pewrapper
        uint64_t peid = peid_list_[rand() % size];
        PEWrapper::PEWrapperPtr wrapper = PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(peid);
        if (wrapper != NULL) {
            usleep(10000); // sleep for a while
        }
    }
}


class TestPEWrapperManager : public ::testing::Test
{
protected:
    void InitTest();
    void DestoryTest();
    void PEWrapperInsertAndRemoveTest();
    void MultiThreadPEWrapperManagerTest();

    void SetUp() {
        InitTest();
    }
    void TearDown() {
        DestoryTest();
    }

private:
    void ConstructFakeApp(Application& app, int process_num, int paralism);

    PN pn_meta;
};

void TestPEWrapperManager::ConstructFakeApp(Application& app, int process_num, int paralism)
{
    // timeval now;
    // gettimeofday(&now, NULL);
    // app.mutable_id()->set_id(now.tv_usec);
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
        if (i == 0) {
            processor->set_role(IMPORTER);
        } else if (i == process_num - 1) {
            processor->set_role(EXPORTER);
        } else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("sleep 3");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(paralism);
        processor->set_backup_num(1);
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


void TestPEWrapperManager::MultiThreadPEWrapperManagerTest() {
    int paralism = 2;
    int process_num = 3;

    PE_LIST pe_list;
    Application test_app;
    list<ProcessorElement>::iterator iter;

    string pn_addr = rpc_const::GetUri(pn_meta.host(), pn_meta.sub_port());
    ConstructFakeApp(test_app, process_num, paralism);
    g_zk->GetMetaManager()->DeleteApplication(test_app.id());
    DSTREAM_DEBUG("construct fake app & pe");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->AddApplication(&test_app));
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(test_app.id(), &pe_list));
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // mock select primary pe
        BackupPE* backup_pe = iter->mutable_backups(0);
        BackupPEID bakcup_peid;
        bakcup_peid.set_id( iter->pe_id().id() );
        *(backup_pe->mutable_pn_id()) = pn_meta.pn_id();
        *(backup_pe->mutable_backup_pe_id()) = bakcup_peid;
        iter->mutable_primary(0)->set_id( iter->pe_id().id() );
        ASSERT_EQ(error::OK, g_zk->GetMetaManager()->UpdateProcessElement(test_app.id(), *iter));
    }
    DSTREAM_DEBUG("add app & process_element");

    pe_list.clear();
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(test_app.id(), &pe_list));
    ASSERT_EQ(paralism * process_num, (int)pe_list.size());
    std::vector<uint64_t> peid_list;
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // put pe to create queue
        Processor processor;
        ASSERT_EQ(true, application_tools::GetProcessor(test_app, *iter, &processor));
        ASSERT_EQ(error::OK, PEWrapperManager::GetPEWrapperManager()->CheckAndInsertPEWrapper(
                      iter->pe_id(), iter->app(), processor, iter->primary(0), 12345));
        peid_list.push_back(iter->pe_id().id());
    }

    while ( (int)PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().size() != paralism *
            process_num ) {
        DSTREAM_DEBUG("waiting create thread to create all pe wrapper, map size(%zu)",
                      PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().size());
        usleep(100000); // sleep for a while
    }

    MockPEWrapperUser mock1(peid_list);
    MockPEWrapperUser mock2(peid_list);
    ASSERT_EQ(mock1.Start() || mock2.Start(), true);

    usleep(1000000); // sleep 1 second
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // put pe to gc queue
        PEWrapperManager::GetPEWrapperManager()->GCPEWrapper(iter->pe_id().id());
        PEWrapperManager::GetPEWrapperManager()->RemovePEWrapper(iter->pe_id().id());
    }

    mock1.Stop();
    mock2.Stop();

    while ( PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().size() != 0 ) {
        DSTREAM_DEBUG("waiting gc thread to destroy all pe wrapper, map size(%zu)",
                      PEWrapperManager::GetPEWrapperManager()->GetPEWrapperMap().size());
    }
    DSTREAM_DEBUG("finish test remove pe wrapper");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->DeleteApplication(test_app.id()));
}


void TestPEWrapperManager::PEWrapperInsertAndRemoveTest() {
    int paralism = 2;
    int process_num = 3;

    PE_LIST pe_list;
    Application test_app;
    list<ProcessorElement>::iterator iter;

    string pn_addr = rpc_const::GetUri(pn_meta.host(), pn_meta.sub_port());
    ConstructFakeApp(test_app, process_num, paralism);
    g_zk->GetMetaManager()->DeleteApplication(test_app.id());
    DSTREAM_DEBUG("construct fake app & pe");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->AddApplication(&test_app));
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(test_app.id(), &pe_list));
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // mock select primary pe
        BackupPE* backup_pe = iter->mutable_backups(0);
        BackupPEID bakcup_peid;
        bakcup_peid.set_id( iter->pe_id().id() );
        *(backup_pe->mutable_pn_id()) = pn_meta.pn_id();
        *(backup_pe->mutable_backup_pe_id()) = bakcup_peid;
        iter->mutable_primary(0)->set_id( iter->pe_id().id() );
        ASSERT_EQ(error::OK, g_zk->GetMetaManager()->UpdateProcessElement(test_app.id(), *iter));
    }
    DSTREAM_DEBUG("add app & process_element");

    pe_list.clear();
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(test_app.id(), &pe_list));
    ASSERT_EQ(paralism * process_num, (int)pe_list.size());
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        uint64_t cur_peid = iter->pe_id().id();
        // test CheckAndInsertPEWrapper
        Processor processor;
        ASSERT_EQ(true, application_tools::GetProcessor(test_app, *iter, &processor));
        ASSERT_EQ(error::OK, PEWrapperManager::GetPEWrapperManager()->CheckAndInsertPEWrapper(
                      iter->pe_id(), iter->app(), processor, iter->primary(0), 12345));
        PEWrapper::PEWrapperPtr wrapper = PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(cur_peid);
        ASSERT_TRUE(wrapper);
        wrapper->CheckAndSetStatus(PEWrapper::INVALID, PEWrapper::CREATING);
        ASSERT_EQ(wrapper->StartCreateThread(cur_peid), error::OK);

        PEWrapper::PEWrapperPtr valid_wrapper = PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(
                                                    cur_peid);

        PEWrapperManager::GetPEWrapperManager()->DumpPEUpDownStreamInfo(cur_peid);

        DSTREAM_DEBUG("create pe wrapper [%lu]", iter->pe_id().id());

    }
    DSTREAM_DEBUG("finish test create pe wrapper");

    PEWrapperManager::GetPEWrapperManager()->DumpPEWrapperMapInfo();

    PEWrapperManager::GetPEWrapperManager()->DumpPEIDList();

    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        uint64_t cur_peid = iter->pe_id().id();
        ASSERT_EQ(error::OK, PEWrapperManager::GetPEWrapperManager()->GCPEWrapper(cur_peid));
        ASSERT_EQ(error::OK, PEWrapperManager::GetPEWrapperManager()->RemovePEWrapper(cur_peid));
        PEWrapper::PEWrapperPtr valid_wrapper = PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(
                                                    cur_peid);
        ASSERT_TRUE(valid_wrapper == NULL);
    }
    DSTREAM_DEBUG("finish test remove pe wrapper");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->DeleteApplication(test_app.id()));
    sleep(3);
}



void TestPEWrapperManager::InitTest() {
    int32_t ret = error::OK;
    string config_file = "./test_pn_reporter.conf.xml";
    // read conf file
    ret = g_pn_cfg_map.InitCfgMapFromFile(config_file);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to read config from[%s]", config_file.c_str());
        return ;
    }

    // gen pn id
    int32_t port;
    int32_t pm_work_thread_num;
    int32_t pe_work_thread_num;
    int32_t pn_work_thread_num;
    int32_t debug_work_thread_num;
    std::string host;
    g_pn_cfg_map.GetValue(config_const::kLocalHostIPName, &host);
    pn_meta.set_host(host);
    // set pn <-> pm port
    g_pn_cfg_map.GetIntValue(config_const::kPNPMListenThreadNumName, &pm_work_thread_num);
    g_pn_cfg_map.GetIntValue(config_const::kPNListenPortName, &port);
    *(pn_meta.mutable_pn_id()->mutable_id()) = "mock_pn";
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

    // Init AppRouter Mgr
    router::RouterMgr* pAppMgr = router::RouterMgr::Instance();
    if (NULL == pAppMgr) {
        DSTREAM_ERROR("fail to get app router manager");
        return ;
    }
    pAppMgr->SetConfigFile(config_file);

    // create zk wrapper
    g_zk = new ZK(&g_pn_cfg_map);

    // create pn
    if (!ProcessNode::InitProcessNodeContext()) {
        DSTREAM_ERROR("fail to process node");
        return ;
    }
    ProcessNode::GetProcessNode()->SetPNMeta(pn_meta);

    ClusterConfig cluster_config;
    cluster_config.set_hdfs_path("mock_hdfs_path");

    g_zk->GetMetaManager()->AddPN(pn_meta);
    g_zk->GetMetaManager()->UpdateClusterConfig(cluster_config);

}

void TestPEWrapperManager::DestoryTest() {
    ProcessNode::DestoryProcessNode();

    g_zk->GetMetaManager()->DeletePN(pn_meta);

    if (g_zk) {
        delete g_zk;
        g_zk = NULL;
    }
}


TEST_F(TestPEWrapperManager, test_wrapper_manager)
{
    PEWrapperInsertAndRemoveTest();
}

TEST_F(TestPEWrapperManager, test_multithread_wrapper_manager)
{
    MultiThreadPEWrapperManagerTest();
}


} // end processnode
} // end dstream

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_pe_wrapper_manager");
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new ProcessNodeEnv());
    return RUN_ALL_TESTS();
}


