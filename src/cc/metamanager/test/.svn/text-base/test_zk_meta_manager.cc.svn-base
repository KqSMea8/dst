/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/25
* @filename:    test_zk_meta_manager.cc
* @author:      zhanggengxin
* @brief:
*/

#include <gtest/gtest.h>

#include "common/application_tools.h"
#include "common/dstream_type.h"
#include "common/config_const.h"
//#include "metamanager/memory_meta_manager.h"
#include "metamanager/meta_manager.h"
#include "metamanager/zk_meta_manager.h"
#include "common/auto_ptr.h"

namespace dstream {
namespace test_zk_meta_manager {

using namespace application_tools;
using namespace zk_client;
using namespace auto_ptr;
using namespace zk_meta_manager;

class MetaManagerTest : public ::testing::Test
{
protected:
    virtual bool InitMetaManager() {
        return false;
    }
    void TestAppMeta();
    void TestPMMeta();
    void TestPNMeta();
    void TestUser();
    void TestPNGroup();
    void TestSubPoint();
    AutoPtr<meta_manager::MetaManager> meta_manager_;
};

/*bool MetaManagerTest::InitMetaManager()
{
  meta_manager_ = AutoPtr<meta_manager::MetaManager>(new memory_meta_manager::MemoryMetaManager);
  return meta_manager_.PtrValid() && meta_manager_->InitMetaManager(NULL);
}*/

class ZKMetaManagerTest : public MetaManagerTest
{
protected:
    virtual bool InitMetaManager();
    void TearDown();
};

void ZKMetaManagerTest::TearDown()
{
    if (meta_manager_.PtrValid()) {
        zk_meta_manager::ZKMetaManager* zk_meta_manager_ = (zk_meta_manager::ZKMetaManager*)((
                    meta_manager::MetaManager*)meta_manager_);
        ZkClient* zk_client = zk_meta_manager_->zk_client();
        zk_client->DeleteNodeTree(zk_meta_manager_->app_root_path().c_str());
        zk_client->DeletePersistentNode(zk_meta_manager_->cluster_config_path().c_str());
        zk_client->DeleteEphemeralNode(zk_meta_manager_->pm_path().c_str());
        zk_client->DeleteNodeTree(zk_meta_manager_->pn_root_path().c_str());
        zk_client->DeleteNodeTree(zk_meta_manager_->pn_group_root_path().c_str());
        zk_client->DeleteNodeTree(zk_meta_manager_->sub_point_root_path().c_str());
    }
}

bool ZKMetaManagerTest::InitMetaManager()
{
    config::Config config_;
    if (config_.ReadConfig(config_const::kTestZKMetaConfigFile) != error::OK) {
        return false;
    }
    meta_manager_ = AutoPtr<meta_manager::MetaManager>(new zk_meta_manager::ZKMetaManager);
    return (meta_manager_.PtrValid() && meta_manager_->InitMetaManager(&config_));
}

static bool PMEqual(const PM& pm1, const PM& pm2)
{
    return pm1.host() == pm2.host() && pm1.report_port() == pm2.report_port() &&
           pm1.submit_port() == pm2.submit_port();
}

static bool AppEqual(const Application& app1, const Application& app2)
{
    if (app1.id().id() != app2.id().id()) {
        return false;
    }
    if (app1.name() != app2.name()) {
        return false;
    }
    if (app1.descr() != app2.descr()) {
        return false;
    }
    if (app1.user().username() != app2.user().username() ||
        app1.user().password() != app2.user().password()) {
        return false;
    }
    const Topology& topology1 = app1.topology();
    const Topology& topology2 = app2.topology();
    if (topology1.processor_num() != topology2.processor_num()) {
        return false;
    }
    for (unsigned int i = 0; i < topology1.processor_num(); ++i) {
        const Processor& processor1 = topology1.processors(i);
        const Processor& processor2 = topology2.processors(i);
        if (processor1.name() != processor2.name() ||
            processor1.descr() != processor2.descr()) {
            return false;
        }
        if (processor1.id().id() != processor2.id().id() ||
            processor1.role() != processor2.role()) {
            return false;
        }
        if (processor1.exec_cmd() != processor2.exec_cmd()) {
            return false;
        }
        if (processor1.parallism() != processor2.parallism() ||
            processor1.backup_num() != processor2.backup_num()) {
            return false;
        }
        if (processor1.has_subscribe() != processor2.has_subscribe()) {
            return false;
        }
        if (processor1.has_subscribe()) {
            const Subscribe& sub1 = processor1.subscribe();
            const Subscribe& sub2 = processor2.subscribe();
            if (sub1.processor_size() != sub2.processor_size()) {
                return false;
            }
            for (int j = 0; j < sub1.processor_size(); ++j) {
                const ProcessorSub& psub1 = sub1.processor(j);
                const ProcessorSub& psub2 = sub2.processor(j);
                if (psub1.name() != psub2.name() || psub1.tags() != psub2.tags() ||
                    psub1.username() != psub2.username() || psub1.password() != psub2.password()) {
                    return false;
                }
            }
        }
    }
    if (app1.status() != app2.status()) {
        return false;
    }
    return true;
}

static bool PEListEqual(const std::list<ProcessorElement>& pe_list1,
                        const std::list<ProcessorElement>& pe_list2)
{
    if (pe_list1.size() != pe_list2.size()) {
        return false;
    }
    typedef std::set<ProcessorElement, ComparePE> PE_SET;
    PE_SET pe_set;
    for (std::list<ProcessorElement>::const_iterator pe_iter = pe_list1.begin();
         pe_iter != pe_list1.end(); ++pe_iter) {
        pe_set.insert(*pe_iter);
    }
    for (std::list<ProcessorElement>::const_iterator pe_iter = pe_list2.begin();
         pe_iter != pe_list2.end(); ++pe_iter) {
        PE_SET::iterator pe_find = pe_set.find(*pe_iter);
        if (pe_find == pe_set.end()) {
            return false;
        }
        if (pe_find->backups_size() != pe_iter->backups_size()) {
            return false;
        }
        for (int i = 0; i < pe_iter->backups_size(); ++i) {
            const BackupPE& backup_pe1 = pe_iter->backups(i);
            const BackupPE& backup_pe2 = pe_find->backups(i);
            if (backup_pe1.backup_pe_id().id() != backup_pe2.backup_pe_id().id()) {
                return false;
            }
            if (backup_pe1.has_pn_id() != backup_pe2.has_pn_id()) {
                return false;
            }
            if (backup_pe1.has_pn_id() && backup_pe1.pn_id().id() != backup_pe2.pn_id().id()) {
                return false;
            }
        }
        if (pe_find->serial() != pe_iter->serial()) {
            return false;
        }
        if (pe_find->primary_size() != pe_iter->primary_size()) {
            return false;
        }
        for (int i = 0; i < pe_iter->primary_size(); ++i) {
            const BackupPEID& primary_id = pe_iter->primary(i);
            if (primary_id.has_id() != pe_find->primary(i).has_id()) {
                return false;
            }
            if (primary_id.has_id() && primary_id.id() != pe_find->primary(i).id()) {
                return false;
            }
        }
    }
    return true;
}

void MetaManagerTest::TestAppMeta()
{
    Application test_app;
    ConstructFakeApp(&test_app);
    ClusterConfig cluster_config;
    cluster_config.set_hdfs_path("test-inf-dstream.baidu.com:9090");
    ASSERT_EQ(error::OK, meta_manager_->UpdateClusterConfig(cluster_config));
    ASSERT_EQ(error::OK, meta_manager_->AddApplication(&test_app));
    std::list<AppID> app_id_list;
    ASSERT_EQ(error::OK, meta_manager_->GetAppIDList(&app_id_list));
    ASSERT_EQ(1u, app_id_list.size());
    ASSERT_EQ(test_app.id().id(), app_id_list.begin()->id());
    Application meta_app;
    ASSERT_EQ(error::OK, meta_manager_->GetApplication(*app_id_list.begin(), &meta_app));
    ASSERT_TRUE(AppEqual(test_app, meta_app));
    std::list<Application> app_list;
    ASSERT_EQ(error::OK, meta_manager_->GetApplicationList(&app_list));
    ASSERT_EQ(1u, app_list.size());
    ASSERT_TRUE(AppEqual(test_app, *app_list.begin()));
    std::list<ProcessorElement> pe_list, meta_pe_list;
    ASSERT_TRUE(CreateAppPEInstance(test_app, cluster_config, NULL, &pe_list));
    ASSERT_EQ(error::OK, meta_manager_->GetAppProcessElements(test_app.id(), &meta_pe_list));
    ASSERT_TRUE(PEListEqual(pe_list, meta_pe_list));
    for (int i = 0; i < test_app.topology().processors_size(); ++i) {
        const Processor& processor = test_app.topology().processors(i);
        pe_list.clear();
        ASSERT_EQ(error::OK, meta_manager_->GetProcessorPEs(test_app.id(), processor.id(), &pe_list));
        ASSERT_EQ(pe_list.size(), processor.parallism());
    }

}

static bool PNEqual(const PN& pn1, const PN& pn2)
{
    return (pn1.pn_id().id() == pn2.pn_id().id() && pn1.rpc_server_port() == pn2.rpc_server_port()
            && pn1.pub_port() == pn2.pub_port() && pn1.sub_port() == pn2.sub_port() &&
            pn1.host() == pn2.host());
}

void MetaManagerTest::TestPNMeta()
{
    PN pn, pn2, meta_pn;
    PNID pn_id;
    *(pn_id.mutable_id()) = "localhost_1";
    *(pn.mutable_pn_id()) = pn_id;
    *(pn.mutable_host()) = "127.0.0.1";
    pn.set_rpc_server_port(1200);
    pn.set_pub_port(1300);
    pn.set_sub_port(1400);
    ASSERT_EQ(error::OK, meta_manager_->AddPN(pn));
    ASSERT_EQ(error::OK, meta_manager_->GetPN(pn_id, &meta_pn));
    ASSERT_TRUE(PNEqual(pn, meta_pn));
    *(pn_id.mutable_id()) = "localhost_2";
    *(pn2.mutable_pn_id()) = pn_id;
    *(pn2.mutable_host()) = "127.0.0.2";
    pn2.set_rpc_server_port(1233);
    pn2.set_pub_port(1455);
    pn2.set_sub_port(1345);
    ASSERT_EQ(error::OK, meta_manager_->AddPN(pn2));
    ASSERT_EQ(error::OK, meta_manager_->GetPN(pn_id, &meta_pn));
    ASSERT_TRUE(PNEqual(pn2, meta_pn));
    list<PN> pn_list;
    ASSERT_EQ(error::OK, meta_manager_->GetPNList(&pn_list));
    ASSERT_EQ(2u, pn_list.size());
    for (list<PN>::iterator pn_iter = pn_list.begin(); pn_iter != pn_list.end(); ++pn_iter) {
        if (pn_iter->pn_id().id() == pn.pn_id().id()) {
            ASSERT_TRUE(PNEqual(pn, *pn_iter));
            continue;
        }
        if (pn_iter->pn_id().id() == pn2.pn_id().id()) {
            ASSERT_TRUE(PNEqual(pn2, *pn_iter));
        }
    }
    ASSERT_EQ(error::OK, meta_manager_->DeletePN(pn2));
    ASSERT_NE(error::OK, meta_manager_->GetPN(pn_id, &meta_pn));
}

void MetaManagerTest::TestPMMeta()
{
    PM pm;
    ASSERT_EQ(error::OK, meta_manager_->AddPM(pm));
    *(pm.mutable_host()) = "localhost";
    pm.set_report_port(1344);
    pm.set_submit_port(1345);
    ASSERT_EQ(error::OK, meta_manager_->UpdatePM(pm));
    PM meta_pm;
    ASSERT_EQ(error::OK, meta_manager_->GetPM(&meta_pm));
    ASSERT_TRUE(PMEqual(pm, meta_pm));
    ASSERT_EQ(error::OK, meta_manager_->DeletePM());
    ASSERT_NE(error::OK, meta_manager_->GetPM(&meta_pm));
}

void MetaManagerTest::TestUser()
{
    User test_user, test_user2;
    test_user.set_username("test");
    test_user.set_password("test");
    ASSERT_NE(error::OK, meta_manager_->CheckUser(test_user));
    ASSERT_EQ(error::OK, meta_manager_->UpdateUser(test_user));
    ASSERT_EQ(error::OK, meta_manager_->CheckUser(test_user));
    test_user2 = test_user;
    test_user2.set_password("test_test");
    ASSERT_NE(error::OK, meta_manager_->CheckUser(test_user2));
    ASSERT_EQ(error::OK, meta_manager_->UpdateUser(test_user2));
    ASSERT_NE(error::OK, meta_manager_->CheckUser(test_user));
    test_user.set_password(test_user2.password());
    ASSERT_EQ(error::OK, meta_manager_->CheckUser(test_user));
}

static bool PNGroupEqual(const GroupInfo& group1, const GroupInfo& group2)
{
    if (group1.group_name() != group2.group_name()) {
        return false;
    }
    /*  if (group1.type() != group2.type()) {
        return false;
      }*/
    if (group1.pn_list_size() != group2.pn_list_size()) {
        return false;
    }
    for ( int i = 0; i < group1.pn_list_size(); i++) {
        if (group1.pn_list(i) != group2.pn_list(i)) {
            return false;
        }
    }
    return true;
}

void MetaManagerTest::TestPNGroup()
{
    PNGroups pn_group;
    GroupInfo group1;
    group1.set_group_name("default");
    //  group1.set_type(SHARE);
    group1.add_pn_list("st01-dstream-0001.st01");
    group1.add_pn_list("st01-dstream-0002.st01");
    group1.add_pn_list("st01-dstream-0003.st01");
    GroupInfo* add = pn_group.add_group_list();
    add->CopyFrom(group1);

    ASSERT_EQ(error::OK, meta_manager_->UpdatePNGroups(pn_group));
    PNGroups zk_pn_group;
    ASSERT_EQ(error::OK, meta_manager_->GetPNGroups(&zk_pn_group));
    ASSERT_EQ(1, zk_pn_group.group_list_size());
    GroupInfo zk_group1;
    std::list<GroupInfo> zk_group_list;
    std::list<std::string> zk_group_name_list;
    ASSERT_EQ(error::OK, meta_manager_->GetGroupList(&zk_group_list));
    ASSERT_EQ(1u, zk_group_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupNameList(&zk_group_name_list));
    ASSERT_EQ(1u, zk_group_name_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupInfo("default", &zk_group1));
    ASSERT_TRUE(PNGroupEqual(group1, zk_group1));

    zk_group_list.clear();
    zk_group_name_list.clear();
    GroupInfo group2;
    group2.set_group_name("dstream");
    //  group2.set_type(MONOPOLY);
    group2.add_pn_list("st01-dstream-0004.st01");
    group2.add_pn_list("st01-dstream-0005.st01");
    ASSERT_EQ(error::OK, meta_manager_->AddGroupInfo(group2));
    ASSERT_EQ(error::OK, meta_manager_->GetGroupList(&zk_group_list));
    ASSERT_EQ(2u, zk_group_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupNameList(&zk_group_name_list));
    ASSERT_EQ(2u, zk_group_name_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupInfo("dstream", &zk_group1));
    ASSERT_TRUE(PNGroupEqual(group2, zk_group1));

    ASSERT_EQ(error::OK, meta_manager_->DeleteGroupInfo(group1));
    zk_group_list.clear();
    zk_group_name_list.clear();
    ASSERT_EQ(error::OK, meta_manager_->GetGroupList(&zk_group_list));
    ASSERT_EQ(1u, zk_group_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupNameList(&zk_group_name_list));
    ASSERT_EQ(1u, zk_group_name_list.size());
    ASSERT_EQ(error::OK, meta_manager_->GetGroupInfo("dstream", &zk_group1));
    ASSERT_TRUE(PNGroupEqual(group2, zk_group1));


    ASSERT_NE(error::OK, meta_manager_->AddGroupInfo(group2));
    ASSERT_NE(error::OK, meta_manager_->DeleteGroupInfo(group1));

    ASSERT_EQ(error::OK, meta_manager_->DeletePNGroups());
    zk_group_list.clear();
    ASSERT_NE(error::OK, meta_manager_->GetGroupList(&zk_group_list));
    ASSERT_EQ(0u, zk_group_list.size());
}

void MetaManagerTest::TestSubPoint() {
    SubPoint sub_point;
    std::string path("test-path");
    sub_point.mutable_id()->set_id(123);
    sub_point.set_msg_id(1);
    sub_point.set_seq_id(2);

    ASSERT_EQ(error::OK, meta_manager_->AddSubPoint(path, sub_point));

    SubPoint zk_sub_point;
    PEID pe_id;
    pe_id.set_id(123);
    ASSERT_EQ(error::OK, meta_manager_->GetSubPoint(path, &zk_sub_point));
    ASSERT_EQ(123u, zk_sub_point.id().id());
    ASSERT_EQ(1u, zk_sub_point.msg_id());
    ASSERT_EQ(2u, zk_sub_point.seq_id());

    sub_point.mutable_id()->set_id(456);
    sub_point.set_msg_id(2);
    sub_point.set_seq_id(3);
    ASSERT_EQ(error::OK, meta_manager_->UpdateSubPoint(path, sub_point));

    zk_sub_point.Clear();
    pe_id.set_id(456);
    ASSERT_EQ(error::OK, meta_manager_->GetSubPoint(path, &zk_sub_point));
    ASSERT_EQ(456u, zk_sub_point.id().id());
    ASSERT_EQ(2u, zk_sub_point.msg_id());
    ASSERT_EQ(3u, zk_sub_point.seq_id());

    ASSERT_EQ(error::OK, meta_manager_->DelSubPoint(path, sub_point));
    ASSERT_NE(error::OK, meta_manager_->GetSubPoint(path, &zk_sub_point));
}

#define TEST_META_F(test_class_name, test_fixture_name, test_method_name)  \
                    TEST_F(test_class_name, test_fixture_name)             \
                    {                                                      \
                      if(InitMetaManager()) {                              \
                        test_method_name();                                \
                      }  else{                                             \
                        DSTREAM_WARN("metamanager fail");                  \
                      }                                                    \
                    }                                                      \
 
TEST_META_F(MetaManagerTest, test_app_meta_operation, TestAppMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_app_meta_operation, TestAppMeta);
TEST_META_F(MetaManagerTest, test_pm_meta_op, TestPMMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_pm_meta_op, TestPMMeta);
TEST_META_F(MetaManagerTest, test_pn_op, TestPNMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_pn_op, TestPNMeta);
TEST_META_F(MetaManagerTest, test_user_op, TestUser);
TEST_META_F(ZKMetaManagerTest, test_zk_user_op, TestUser);
TEST_META_F(MetaManagerTest, test_pn_group_op, TestPNGroup);
TEST_META_F(ZKMetaManagerTest, test_zk_pn_group_op, TestPNGroup);
TEST_META_F(MetaManagerTest, test_sub_point_op, TestSubPoint);
TEST_META_F(ZKMetaManagerTest, test_zk_sub_point_op, TestSubPoint);

} //namespace test_zk_meta_manager
} //namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_metamanager");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

