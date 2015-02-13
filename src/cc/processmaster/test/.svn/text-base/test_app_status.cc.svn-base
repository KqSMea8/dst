#include "gtest/gtest.h"
#include "pm_test_common.h"

#include "common/id.h"
#include "common/application_tools.h"
#include "processmaster/app_status.h"

namespace dstream {
namespace test_app_status {

using namespace common;
using namespace pm_test_common;
using namespace application_tools;
using namespace meta_manager;

class TestAppStatus : public PMTestBase {
protected:
    void SetUp();
    void TearDown();

    void TestGetDifference();
    void TestUpdateApplication();
    void TestUpdateProcessor();
protected:
    app_status::AppStatus* app_status_;
};

void TestAppStatus::SetUp() {
    PMTestBase::SetUp();
    Application app;
    ConstructFakeApp(&app, 3, 3);
    app.mutable_id()->set_id(1);
    ASSERT_EQ(error::OK, meta_->AddApplication(&app));
    app_status::AppStatus::InitAppStatus();
    app_status_ = app_status::AppStatus::GetAppStatus();
}

void TestAppStatus::TearDown() {
    app_status::AppStatus::DestroyAppStatus();
}

void TestAppStatus::TestGetDifference() {
    app_status::PidRevMap large_map, less_map, diff_map;
    // init 2 apps
    large_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 0), 0));
    large_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 1), 0));
    large_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 2), 0));

    // init less map
    less_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 0), 0));

    // assert pair to be add
    app_status_->GetDifference(large_map, less_map, diff_map);
    ASSERT_EQ(diff_map.size(), 2u);

    // assert pair to be delete
    diff_map.clear();
    less_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 1), 0));
    less_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 2), 0));
    less_map.insert(app_status::PidRevPair(MAKE_APP_PROCESSOR_ID(1, 3), 0));
    app_status_->GetDifference(less_map, large_map, diff_map);
    ASSERT_EQ(diff_map.size(), 1u);
}

void TestAppStatus::TestUpdateApplication() {
    Application app;
    ConstructFakeApp(&app);
    app.mutable_id()->set_id(1);
    Topology* tp = app.mutable_topology();
    ASSERT_EQ(tp->processors(0).id().id(), 0u);
    ASSERT_EQ(tp->processors(1).id().id(), 1u);

    tp->set_processor_num(3);
    Processor* processor = tp->add_processors();
    processor->mutable_id()->set_id(2);

    app_status_->UpdateApplication(app);
    ASSERT_EQ(app_status_->m_pid_rev_map.size(), 3u);
}

void TestAppStatus::TestUpdateProcessor() {
    ASSERT_EQ(app_status_->m_pid_rev_map.size(), 3u);
    ASSERT_EQ(error::ADD_APPSTATUS_FAIL, app_status_->AddProcessor(MAKE_APP_PROCESSOR_ID(1, 0), 0));
    ASSERT_EQ(error::DEL_APPSTATUS_FAIL, app_status_->DelProcessor(MAKE_APP_PROCESSOR_ID(1, 3)));

    ASSERT_EQ(error::OK, app_status_->AddProcessor(MAKE_APP_PROCESSOR_ID(1, 3), 0));
    ASSERT_EQ(app_status_->m_pid_rev_map.size(), 4u);
    ASSERT_EQ(error::OK, app_status_->DelProcessor(MAKE_APP_PROCESSOR_ID(1, 3)));
    ASSERT_EQ(error::OK, app_status_->DelProcessor(MAKE_APP_PROCESSOR_ID(1, 2)));
    ASSERT_EQ(app_status_->m_pid_rev_map.size(), 2u);
}

TEST_F(TestAppStatus, TestGetDifference) {
    TestGetDifference();
}

TEST_F(TestAppStatus, TestUpdateApplication) {
    TestUpdateApplication();
}

TEST_F(TestAppStatus, TestUpdateProcessor) {
    TestUpdateProcessor();
}
}//namespace test_app_status
}//namespace dstream
