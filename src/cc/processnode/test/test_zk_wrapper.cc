#include "gtest/gtest.h"
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

using namespace std;
using namespace dstream;
using namespace dstream::rpc;
using namespace dstream::processnode;
using namespace dstream::application_tools;

namespace dstream {
namespace processnode {

class TestZKWrapper : public ::testing::Test
{
protected:
    void ConstructFakeApp(Application& app, int process_num, int paralism);
    void ConstructFakePE(const Application& app, PE_LIST& pe_list);

    void InitTest(const string& conf_file);
    void DestoryTest();

    void GetAppTest();
    void GetPEAddrTest();
    void GetUpstreamPEIDsTest();
    void GetPEDownStreamTagTest();

private:
    PN pn_;
    PM pm_;
};

void TestZKWrapper::ConstructFakeApp(Application& app, int process_num, int paralism)
{
    timeval now;
    gettimeofday(&now, NULL);
    app.mutable_id()->set_id(now.tv_usec);
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
        processor->set_exec_cmd("test_bin");
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

void TestZKWrapper::ConstructFakePE(const Application& app, PE_LIST& pe_list)
{
    ClusterConfig cluster_config;
    pe_list.clear();
    CreateAppPEInstance(app, cluster_config, NULL, &pe_list);
}


void TestZKWrapper::InitTest(const string& conf_file)
{
    // read config file
    if (g_pn_cfg_map.InitCfgMapFromFile(conf_file.c_str()) < error::OK) {
        DSTREAM_WARN("fail to read config from[%s]", conf_file.c_str());
        return ;
    }
    //debug
    // g_pn_cfg_map.Dump();

    // test zk
    g_zk = new ZK(&g_pn_cfg_map);

    PNID pnid;
    pnid.set_id("mock_pn_id");
    *pn_.mutable_pn_id() = pnid;
    pn_.set_host("mock_pn_host");
    pn_.set_sub_port(1000);
    pn_.set_debug_port(1001);
    pn_.set_pub_port(1002);
    pn_.set_rpc_server_port(1003);

    ClusterConfig cluster_config;
    cluster_config.set_hdfs_path("mock_hdfs_path");

    g_zk->GetMetaManager()->AddPN(pn_);
    g_zk->GetMetaManager()->UpdateClusterConfig(cluster_config);
}

void TestZKWrapper::DestoryTest()
{
    g_zk->GetMetaManager()->DeletePN(pn_);

    if (g_zk) {
        delete g_zk;
        g_zk = NULL;
    }
}

void TestZKWrapper::GetAppTest()
{
    AppID app_id;
    Application test_app;
    Application valid_app;

    // fail to get
    app_id.set_id(0xFF);
    ASSERT_NE(error::OK, g_zk->GetApp(app_id, &test_app));

    ConstructFakeApp(valid_app, 5, 3);
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->AddApplication(&valid_app));
    DSTREAM_DEBUG("construct fake app");

    ASSERT_EQ(error::OK, g_zk->GetApp(valid_app.id(), &test_app));

    // check app
    ASSERT_EQ(valid_app.id().id(), test_app.id().id());
    ASSERT_STREQ(valid_app.name().c_str(), test_app.name().c_str());

    ASSERT_EQ(valid_app.topology().processors_size(), test_app.topology().processors_size());

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->DeleteApplication(valid_app.id()));
}

void TestZKWrapper::GetPEAddrTest()
{
    PEID pe_id;
    PE_LIST pe_list;
    Application valid_app;
    ProcessorElement valid_pe;
    string test_pe_addr;
    list<ProcessorElement>::iterator iter;

    // construct fake env
    string pn_addr = rpc_const::GetUri(pn_.host(), pn_.sub_port());
    ConstructFakeApp(valid_app, 5, 3);
    DSTREAM_DEBUG("construct fake app & pe");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->AddApplication(&valid_app));
    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->GetAppProcessElements(valid_app.id(), &pe_list));
    for (iter = pe_list.begin(); iter != pe_list.end(); iter++) {
        // mock select primary pe
        BackupPE* backup_pe = iter->mutable_backups(0);
        BackupPEID bakcup_peid;
        bakcup_peid.set_id( iter->pe_id().id() );
        *(backup_pe->mutable_pn_id()) = pn_.pn_id();
        *(backup_pe->mutable_backup_pe_id()) = bakcup_peid;
        iter->mutable_primary(0)->set_id( iter->pe_id().id() );
        ASSERT_EQ(error::OK, g_zk->GetMetaManager()->UpdateProcessElement(valid_app.id(), *iter));
    }
    DSTREAM_DEBUG("add app & process_element");

    ASSERT_EQ(error::OK, g_zk->GetMetaManager()->DeleteApplication(valid_app.id()));
}

TEST_F(TestZKWrapper, test_get_app)
{
    InitTest("test_pn_reporter.conf.xml");
    GetAppTest();
    DestoryTest();
}

TEST_F(TestZKWrapper, test_get_pe_addr)
{
    InitTest("test_pn_reporter.conf.xml");
    GetPEAddrTest();
    DestoryTest();
}

}// end processnode
} // end dstream

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_zk_wrapper");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}



