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
#include "metamanager/memory_meta_manager.h"
#include "metamanager/zk_meta_manager.h"

namespace dstream {
namespace test_meta_manager {

using namespace application_tools;
using namespace memory_meta_manager;
using namespace zk_client;
using namespace zk_meta_manager;

/************************************************************************/
/* MetaManager Test : abstract class                                    */
/************************************************************************/
class MetaManagerTest : public ::testing::Test
{
protected:
    virtual bool InitMetaManager() = 0;
    void TestAppMeta();
    void TestPMMeta();
    void TestPNMeta();
    void TestClusterConfig();
    AutoPtr<MetaManager> meta_manager_;
};

/************************************************************************/
/* Test MemoryMetaManager                                               */
/************************************************************************/
class MemoryMetaManagerTest : public MetaManagerTest
{
protected:
    virtual bool InitMetaManager();
};

bool MemoryMetaManagerTest::InitMetaManager()
{
    meta_manager_ = AutoPtr<MetaManager>(new MemoryMetaManager);
    return meta_manager_.PtrValid() && meta_manager_->InitMetaManager(NULL);
}

/************************************************************************/
/* Test ZKMetaManager                                                   */
/************************************************************************/
class ZKMetaManagerTest : public MetaManagerTest
{
protected:
    virtual bool InitMetaManager();
    void TearDown();
private:
    ClusterConfig old_cluster_config_;
    void ClearContext(ZKMetaManager* zk_meta);
};

void ZKMetaManagerTest::ClearContext(ZKMetaManager* zk_meta)
{
    if (NULL != zk_meta) {
        if (old_cluster_config_.users_size() != 0) {
            zk_meta->UpdateClusterConfig(old_cluster_config_);
        }
        ZkClient* zk_client = zk_meta->zk_client();
        zk_client->DeleteNodeTree(zk_meta->app_root_path().c_str());
        zk_client->DeleteEphemeralNode(zk_meta->pm_path().c_str());
        zk_client->DeleteNodeTree(zk_meta->pn_root_path().c_str());
    }
}

void ZKMetaManagerTest::TearDown()
{
    ClearContext((ZKMetaManager*)((MetaManager*)meta_manager_));
}

bool ZKMetaManagerTest::InitMetaManager()
{
    Config config_;
    if (config_.ReadConfig(kTestZKMetaConfigFile) != error::OK) {
        return false;
    }
    ZKMetaManager* zk_meta = new ZKMetaManager;
    meta_manager_ = AutoPtr<MetaManager>(zk_meta);
    if (meta_manager_.PtrValid() && meta_manager_->InitMetaManager(&config_)) {
        DSTREAM_INFO("init cluster config %s", zk_meta->cluster_config_path().c_str());
        zk_meta->zk_client()->InitSystemRoot(zk_meta->cluster_config_path().c_str());
    } else {
        return false;
    }
    ClearContext((ZKMetaManager*)((MetaManager*)meta_manager_));
    meta_manager_->GetClusterConfig(old_cluster_config_);
    return true;
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
                if (psub1.name() != psub2.name() || psub1.tag() != psub2.tag() ||
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
    typedef std::set<ProcessorElement, meta_manager::ComparePE> PE_SET;
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
    ConstructFakeApp(test_app);
    ClusterConfig cluster_config;
    cluster_config.set_hdfs_path("test-inf-dstream.baidu.com:9090");
    ASSERT_EQ(error::OK, meta_manager_->UpdateClusterConfig(cluster_config));
    ASSERT_EQ(error::OK, meta_manager_->AddApplication(test_app));
    std::list<AppID> app_id_list;
    ASSERT_EQ(error::OK, meta_manager_->GetAppIDList(app_id_list));
    ASSERT_EQ(1, app_id_list.size());
    ASSERT_EQ(test_app.id().id(), app_id_list.begin()->id());
    Application meta_app;
    ASSERT_EQ(error::OK, meta_manager_->GetApplication(*app_id_list.begin(), meta_app));
    ASSERT_TRUE(AppEqual(test_app, meta_app));
    std::list<Application> app_list;
    ASSERT_EQ(error::OK, meta_manager_->GetApplicationList(app_list));
    ASSERT_EQ(1, app_list.size());
    ASSERT_TRUE(AppEqual(test_app, *app_list.begin()));
    std::list<ProcessorElement> pe_list, meta_pe_list;
    ASSERT_TRUE(CreateAppPEInstance(test_app, pe_list, cluster_config, NULL));
    ASSERT_EQ(error::OK, meta_manager_->GetAppProcessElements(test_app.id(), meta_pe_list));
    ASSERT_TRUE(PEListEqual(pe_list, meta_pe_list));
    for (int i = 0; i < test_app.topology().processors_size(); ++i) {
        const Processor& processor = test_app.topology().processors(i);
        pe_list.clear();
        ASSERT_EQ(error::OK, meta_manager_->GetProcessorPEs(test_app.id(), processor.id(), pe_list));
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
    ASSERT_EQ(error::OK, meta_manager_->GetPN(pn_id, meta_pn));
    ASSERT_TRUE(PNEqual(pn, meta_pn));
    *(pn_id.mutable_id()) = "localhost_2";
    *(pn2.mutable_pn_id()) = pn_id;
    *(pn2.mutable_host()) = "127.0.0.2";
    pn2.set_rpc_server_port(1233);
    pn2.set_pub_port(1455);
    pn2.set_sub_port(1345);
    ASSERT_EQ(error::OK, meta_manager_->AddPN(pn2));
    ASSERT_EQ(error::OK, meta_manager_->GetPN(pn_id, meta_pn));
    ASSERT_TRUE(PNEqual(pn2, meta_pn));
    list<PN> pn_list;
    ASSERT_EQ(error::OK, meta_manager_->GetPNList(pn_list));
    ASSERT_EQ(2, pn_list.size());
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
    ASSERT_NE(error::OK, meta_manager_->GetPN(pn_id, meta_pn));
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
    ASSERT_EQ(error::OK, meta_manager_->GetPM(meta_pm));
    ASSERT_TRUE(PMEqual(pm, meta_pm));
    ASSERT_EQ(error::OK, meta_manager_->DeletePM());
    ASSERT_NE(error::OK, meta_manager_->GetPM(meta_pm));
}

static bool ClusterConfigEQ(const ClusterConfig& config1, const ClusterConfig& config2)
{
    if (config1.users_size() != config2.users_size()) {
        return false;
    }
    for (int i = 0; i < config1.users_size(); ++i) {
        const User& user1 = config1.users(i);
        const User& user2 = config2.users(i);
        if (user1.username() != user2.username() || user1.password() != user2.password()) {
            return false;
        }
    }
    return (config1.hdfs_path() == config2.hdfs_path());
}

void MetaManagerTest::TestClusterConfig()
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
    ClusterConfig cluster_config;
    ASSERT_EQ(error::OK, meta_manager_->GetClusterConfig(cluster_config));
    cluster_config.set_hdfs_path("hdfs://test-inf-dstream:8000");
    ASSERT_EQ(error::OK, meta_manager_->UpdateClusterConfig(cluster_config));
    ClusterConfig meta_cluster_config;
    ASSERT_EQ(error::OK, meta_manager_->GetClusterConfig(meta_cluster_config));
    ASSERT_TRUE(ClusterConfigEQ(cluster_config, meta_cluster_config));
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
 
TEST_META_F(ZKMetaManagerTest, test_zk_app_meta_op, TestAppMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_pm_meta_op, TestPMMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_pn_op, TestPNMeta);
TEST_META_F(ZKMetaManagerTest, test_zk_cluster_config_op, TestClusterConfig);

} //namespace test_zk_meta_manager
} //namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_metamanager");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
