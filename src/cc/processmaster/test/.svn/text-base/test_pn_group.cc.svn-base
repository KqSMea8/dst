/************************************************************************/
/* Test PN Group                                                        */
/************************************************************************/

#include "gtest/gtest.h"

#include "processmaster/process_master.h"
#include "processmaster/test/pm_test_common.h"
#include "scheduler/pn_group.h"
#include "processmaster/mock_pn.h"
#include "client_op.h"

namespace dstream {
namespace test_pngroup {

using namespace auto_ptr;
using namespace config_const;
using namespace process_master;
using namespace pm_test_common;
using namespace client_op;
using namespace meta_manager;

typedef struct PN_INFO {
    std::string host_name;
    std::string ip;
} PN_INFO_NODE;

static bool GenerateMockPN(std::vector<PN_INFO_NODE>& pn, const unsigned int pn_num) {
    for (int i = 1; i < 10; ++i) {
        if ( pn.size() == pn_num ) {
            return true;
        }
        char pn_hostname[64];
        sprintf(pn_hostname, "st01-dstream-000%d.st01", i + 1);
        std::string pn_ip;
        if (error::OK != GetRemotehostIP(pn_hostname, pn_ip)) {
            DSTREAM_WARN("fail to get ip of %s\n", pn_hostname);
            continue;
        }

        PN_INFO_NODE add;
        add.host_name = pn_hostname;
        add.ip = pn_ip;
        pn.push_back(add);
    }
    return false;
}

static void GeneratePNGroup(PNGroups& pn_group, const std::vector<PN_INFO_NODE>& pn) {
    GroupInfo* group1 = pn_group.add_group_list();
    group1->set_group_name("dstream");
    //  group1->set_type(SHARE);
    group1->add_pn_list(pn[0].host_name);
}

static void ModifyPNGroup(PNGroups& pn_group, const std::vector<PN_INFO_NODE>& pn) {
    GroupInfo* group1 = pn_group.add_group_list();
    group1->set_group_name("dstream");
    //  group1->set_type(SHARE);
    group1->add_pn_list(pn[0].host_name);
    group1->add_pn_list(pn[1].host_name);
}

static void ModifyPNGroup2(PNGroups& pn_group, const std::vector<PN_INFO_NODE>& pn) {
    GroupInfo* group1 = pn_group.add_group_list();
    group1->set_group_name("dstream");
    //  group1->set_type(SHARE);
    group1->add_pn_list(pn[0].host_name);
}

static void AddPNGroup(PNGroups& pn_group, const std::vector<PN_INFO_NODE>& pn) {
    GroupInfo* group2 = pn_group.add_group_list();
    group2->set_group_name("dstream");
    //  group2->set_type(MONOPOLY);
    group2->add_pn_list(pn[0].host_name);

    GroupInfo* group3 = pn_group.add_group_list();
    group3->set_group_name("uap");
    //  group3->set_type(MONOPOLY);
    group3->add_pn_list(pn[1].host_name);
}

static void DelPNGroup(PNGroups& pn_group) {
    pn_group.Clear();
}

//Test PN Group Class
class TestPNGroup : public ::testing::Test {
protected:
    void SetUp();
    void TearDown();
    void ClearContext();
    void TestGetGroupInfo();
protected:
    ZKMetaManager* meta_;
    ClusterConfig old_cluster_config;
};

void TestPNGroup::SetUp() {
    InitMeta(kZKMetaManagerName, &old_cluster_config);
    meta_ = (ZKMetaManager*)MetaManager::GetMetaManager();
    ClearContext();
}

void TestPNGroup::TearDown() {
    if (old_cluster_config.users_size() > 0) {
        meta_->UpdateClusterConfig(old_cluster_config);
    }
    ClearContext();
    meta_->DeletePNGroups();
    MetaManager::DestoryMetaManager();
}

void TestPNGroup::ClearContext() {
    ClearApp();
    meta_->DeletePM();
    meta_->DeletePMMachines();
    meta_->DeletePNMachines();
    list<PN> pn_list;
    meta_->GetPNList(&pn_list);
    for (list<PN>::iterator pn_it = pn_list.begin();
         pn_it != pn_list.end(); ++pn_it) {
        meta_->DeletePN(*pn_it);
    }
}

void TestPNGroup::TestGetGroupInfo() {
    ProcessMaster pm;
    pm.Start(kTestPMConfigFile);
    pm.gc()->Stop();

    std::vector<PN_INFO_NODE> pn;
    ASSERT_EQ(true, GenerateMockPN(pn, 4));
    ASSERT_EQ(4u, pn.size());

    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->UpdatePNGroup());
    PNGroups before;
    pm.scheduler()->pn_group()->GetPNGroup(&before);
    ASSERT_EQ(0, before.group_list_size());

    PNGroups pn_group;
    GeneratePNGroup(pn_group, pn);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    std::list<GroupInfo> zk_group;
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(1u, zk_group.size());

    sleep(1);
    ASSERT_EQ(1u, pm.scheduler()->pn_group()->GetGroupSize());
    MockPN mock_pns[4];
    for (int i = 0; i < 4; ++i) {
        mock_pns[i].set_mem_basic(2);
        mock_pns[i].set_cpu_basic(16);
        mock_pns[i].StartPN(pn[i].ip);
        mock_pns[i].ReportStatus();
    }

    sleep(1);
    PNIDSet pn_list;
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(3u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(1u, pn_list.size());

    pn_group.Clear();
    zk_group.clear();
    pn_list.clear();
    ModifyPNGroup(pn_group, pn);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(1u, zk_group.size());
    sleep(1);
    ASSERT_EQ(1u, pm.scheduler()->pn_group()->GetGroupSize());
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(2u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(2u, pn_list.size());

    pn_group.Clear();
    zk_group.clear();
    pn_list.clear();
    ModifyPNGroup2(pn_group, pn);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(1u, zk_group.size());
    sleep(1);
    ASSERT_EQ(1u, pm.scheduler()->pn_group()->GetGroupSize());
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(3u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(1u, pn_list.size());

    pn_group.Clear();
    zk_group.clear();
    pn_list.clear();
    ModifyPNGroup(pn_group, pn);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(1u, zk_group.size());
    sleep(1);
    ASSERT_EQ(1u, pm.scheduler()->pn_group()->GetGroupSize());
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(2u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(2u, pn_list.size());

    pn_group.Clear();
    zk_group.clear();
    pn_list.clear();
    AddPNGroup(pn_group, pn);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(2u, zk_group.size());
    sleep(1);
    ASSERT_EQ(2u, pm.scheduler()->pn_group()->GetGroupSize());
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(2u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(1u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("uap", pn_list));
    ASSERT_EQ(1u, pn_list.size());

    pn_group.Clear();
    zk_group.clear();
    pn_list.clear();
    DelPNGroup(pn_group);
    ASSERT_EQ(error::OK, meta_->UpdatePNGroups(pn_group));
    ASSERT_EQ(error::OK, meta_->GetGroupList(&zk_group));
    ASSERT_EQ(0u, zk_group.size());
    sleep(1);
    ASSERT_EQ(0u, pm.scheduler()->pn_group()->GetGroupSize());
    PNGroups temp;
    pm.scheduler()->pn_group()->GetPNGroup(&temp);
    for ( int i = 0; i < temp.group_list_size(); i++ ) {
        DSTREAM_DEBUG("%s", temp.group_list(i).group_name().c_str());
    }
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("default", pn_list));
    ASSERT_EQ(4u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("dstream", pn_list));
    ASSERT_EQ(0u, pn_list.size());
    pn_list.clear();
    ASSERT_EQ(error::OK, pm.scheduler()->pn_group()->GetPreferPNList("uap", pn_list));
    ASSERT_EQ(0u, pn_list.size());

}

//Test PN Group
TEST_F(TestPNGroup, TestGetGroupInfo)
{
    TestGetGroupInfo();
}

} //namespace test_pngroup
} //namespace dstream
