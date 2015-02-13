/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test for dstream event
*********************************************************************/

#include <vector>
#include <unistd.h>

#include "common/application_tools.h"
#include "common/config_const.h"
#include "common/logger.h"
#include "gtest/gtest.h"
#include "metamanager/meta_manager.h"
#include "processmaster/dstream_event.h"
#include "processmaster/dstream_event_manager.h"
#include "processmaster/mock_pn.h"
#include "processmaster/plain_scheduler.h"
#include "processmaster/pm_auto_ptr.h"
#include "processmaster/pm_rpc_server.h"
#include "processmaster/zk_watcher.h"

namespace dstream {
namespace test_dstream_event {
using namespace dstream_event_manager;
using pm_auto_ptr::AutoPtr;
using namespace application_tools;
using namespace config_const;
using namespace dstream_event;
using namespace mock_pn;
using namespace meta_manager;
using namespace plain_scheduler;
using namespace pm_auto_ptr;
using namespace pm_rpc_server;
using namespace zk_watcher;
typedef AutoPtr<DStreamEventManager> DEventMPtr;
typedef AutoPtr<MockPN> MockPtr;

static void sleep_event(EventManager& event_manager)
{
    DSTREAM_DEBUG("wait for event");
    for (int i = 0; i < 100; ++i) {
        if (event_manager.wait_event() && event_manager.EventSize() == 0) {
            return;
        }
        usleep(100 * 1000);
    }
    DSTREAM_DEBUG("event not over");
    ASSERT_TRUE(0);
}

/************************************************************************/
/* test for dstream event and dstream event manager                     */
/************************************************************************/
class TestDStreamEvent : public ::testing::Test
{
protected:
    void SetUp();
    void AddPN();
    void TearDown() {
        MetaManager::DestoryMetaManager();
    }
    void TestAddEvent();
    config::Config config;
private:
    std::vector<MockPtr> pns_;
    DEventMPtr event_manager_;
    AutoPtr<PlainScheduler> scheduler_;
    AutoPtr<DStreamWatcher> watcher_;
    PMRPCServer pm_server_;
    bool init_;
};

void TestDStreamEvent::AddPN()
{
    pns_.clear();
    for (int i = 0; i < 4; ++i) {
        MockPtr mock_pn(new mock_pn::MockPN());
        std::vector<MockPtr>::iterator insert_iter = pns_.insert(pns_.end(), mock_pn);
        if (!(*insert_iter)->StartPN()) {
            DSTREAM_WARN("start pn fail");
            pns_.erase(insert_iter);
            continue;
        }
        DSTREAM_INFO("start pn success");
        usleep(100 * 1000);
    }
    DSTREAM_INFO("start %d mock pn", pns_.size());
}

void TestDStreamEvent::SetUp()
{
    init_ = false;
    ASSERT_EQ(config.ReadConfig(config_const::kTestPMConfigFile), error::OK);
    ASSERT_TRUE(MetaManager::InitMetaManagerContext(&config));
    scheduler_ = AutoPtr<PlainScheduler>(new PlainScheduler(pm_server_));
    event_manager_ = DEventMPtr(new DStreamEventManager(&config, scheduler_));
    ASSERT_TRUE(event_manager_->Start());
    watcher_ = AutoPtr<DStreamWatcher>(new DStreamWatcher(&config));
    ASSERT_TRUE(pm_server_.Init(event_manager_, watcher_)
                && pm_server_.StartServer());
    init_ = true;
}

void TestDStreamEvent::TestAddEvent()
{
    if (init_) {
        ClusterConfig cluster_config;
        cluster_config.set_big_pipe_path("test-inf-dstream");
        cluster_config.set_hdfs_path("test-inf-dstream");
        Application app;
        ConstructFakeApp(app, 3, 3);
        MetaManager* meta = MetaManager::GetMetaManager();
        ASSERT_EQ(error::OK, meta->UpdateClusterConfig(cluster_config));
        ASSERT_EQ(error::OK, meta->AddApplication(app));
        app.set_status(RUN);
        ASSERT_EQ(error::OK, meta->UpdateApplication(app));
        PM pm;
        pm.set_host(pm_server_.server_ip());
        pm.set_report_port(pm_server_.server_port());
        ASSERT_TRUE(meta->AddPM(pm) == error::OK);
        std::list<ProcessorElement> pe_list;
        ASSERT_EQ(error::OK, meta->GetAppProcessElements(app.id(), pe_list));
        ASSERT_NE(0, pe_list.size());
        typedef std::list<ProcessorElement>::iterator PE_LIST_ITER;
        PE_LIST_ITER pe_iter = pe_list.begin();
        PE_LIST_ITER pe_end = pe_list.end();
        int pe_size = pe_list.size() / 2;
        for (pe_iter = pe_list.begin(); pe_iter != pe_end; ++pe_iter) {
            ASSERT_FALSE(CheckPENormal(*pe_iter, app.id(), pe_iter->pe_id()));
        }
        int i = 0;
        for (pe_iter = pe_list.begin(); i < pe_size; ++pe_iter, ++i) {
            event_manager_->AddEvent(EventPtr(new CreatePEEvent(app.id(),
                                              pe_iter->pe_id())));
        }
        sleep_event(*event_manager_);
        ASSERT_EQ(0, event_manager_->EventSize());
        pe_size = pe_list.size();
        pe_list.clear();
        ASSERT_EQ(error::OK, meta->GetAppProcessElements(app.id(), pe_list));
        ASSERT_EQ(pe_list.size(), pe_size);
        pe_end = pe_list.end();
        pe_size = pe_list.size() / 2;
        for (pe_iter = pe_list.begin(), i = 0; i < pe_size; ++pe_iter, ++i) {
            ASSERT_TRUE(CheckPENormal(*pe_iter, app.id(), pe_iter->pe_id()));
        }
        do {
            ASSERT_FALSE(CheckPENormal(*pe_iter, app.id(), pe_iter->pe_id()));
        } while (++pe_iter != pe_end);
        for (int i = 0; i < pns_.size(); ++i) {
            pns_[i]->ReportStatus();
        }
        pns_[0]->KillPN();
        ASSERT_TRUE(pm_server_.RemovePNReport(pns_[0]->pn_id()));
        sleep_event(*event_manager_);
        const STATUS_SET status_set = pns_[0]->status();
        PNID pn_id = pns_[0]->pn_id();
        ProcessorElement pe;
        ASSERT_NE(0, status_set.size());
        for (STATUS_SET::const_iterator status_iter = status_set.begin();
             status_iter != status_set.end();
             ++status_iter) {
            ASSERT_TRUE(meta->GetProcessorElement(status_iter->app_id_,
                                                  status_iter->pe_id_,
                                                  pe) == error::OK);
            bool find = false;
            for (int i = 0; i < pe.backups_size(); ++i) {
                const BackupPE& backup_pe = pe.backups(i);
                if (backup_pe.backup_pe_id().id() == status_iter->backup_pe_id_.id()) {
                    ASSERT_TRUE(backup_pe.has_pn_id());
                    ASSERT_STRNE(backup_pe.pn_id().id().c_str(), pn_id.id().c_str());
                    find = true;
                    break;
                }
            }
            ASSERT_TRUE(find);
        }
    } else {
        DSTREAM_WARN("init fail");
        ASSERT_TRUE(false);
    }
}

TEST_F(TestDStreamEvent, test_event)
{
    AddPN();
    TestAddEvent();
}

TEST_F(TestDStreamEvent, test_zk_event)
{
    MetaManager::DestoryMetaManager();
    ASSERT_EQ(error::OK, config.SetValue(kMetaManagerName, kZKMetaManagerName));
    ASSERT_TRUE(MetaManager::InitMetaManagerContext(&config));
    AddPN();
    TestAddEvent();
}

}//namespace test_dstream_event
}//namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_dstream_event");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
