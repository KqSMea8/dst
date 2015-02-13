#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/zk_client.h"
#include "common/proto_serializer.h"
#include "common/proto/pub_sub.pb.h"
#include "common/proto/application.pb.h"

#include "metamanager/zk_config.h"

#include "processnode/config_map.h"
#include "processnode/zk_wrapper.h"
#include "processnode/process_node.h"
#include "processnode/pe_wrapper.h"
#include "processnode/queue_manager.h"

using namespace std;
using namespace dstream;
using namespace dstream::processnode;
using namespace dstream::proto_serializer;

namespace dstream {

class TestPNReplay : public ::testing::Test
{
private:
    std::string pipe_name_;
    PEWrapper* pe;
    QueueManager* queue_manager_;

public:
    PEWrapper* GetPEWrapper() {
        return pe;
    }

    TestPNReplay();
    virtual ~TestPNReplay();

protected:
    void InitTest(const std::string& pipe_name);
    void PublishTest(const uint64_t& peid, const uint32_t& pub_count);
    void SubscribeTest(const uint64_t& peid, const uint64_t& begin_pipeid, uint32_t& log_count);
    void DeinitTest();
};

TestPNReplay::TestPNReplay()
    : pe(NULL), queue_manager_(NULL)
{
    queue_manager_ = new QueueManager(100);
    pe = new PEWrapper(queue_manager_);

}

TestPNReplay::~TestPNReplay() {
    if (pe) {
        delete pe;
    }
    if (queue_manager_) {
        delete queue_manager_;
    }

}

void TestPNReplay::InitTest(const std::string& pipe_name) {
    pipe_name_ = pipe_name;
}

void TestPNReplay::DeinitTest() {
    std::string cmd;
    cmd = "rm -f ";
    cmd.append(pipe_name_);
    system(cmd.c_str());
}


void TestPNReplay::SubscribeTest(const uint64_t& peid, const uint64_t& begin_pipeid,
                                 uint32_t& log_count) {
    vector<Log*> replay_logs;
    if (error::OK != pe->GetReplayLogs(peid, pipe_name_, begin_pipeid, replay_logs)) {
        DSTREAM_ERROR("replay log(%s) open failed, begin_pipeid(%llu)", pipe_name_.c_str(), begin_pipeid);
    }
    uint32_t count = 0;
    DSTREAM_DEBUG("from log begin_pipeid(%llu) to latest log", begin_pipeid);
    for (vector<Log*>::iterator it = replay_logs.begin(); it != replay_logs.end(); ++it) {
        DSTREAM_DEBUG("log debug_info : %s", (*it)->debug_info().c_str());
        if (count == replay_logs.size() - 1) {
            ASSERT_STREQ("END_REPLAY", (*it)->debug_info().c_str());
        } else {
            ASSERT_STREQ("REPLAYING", (*it)->debug_info().c_str());
        }
        delete *it;
        count++;
    }
    log_count = count;
}

void TestPNReplay::PublishTest(const uint64_t& peid, const uint32_t& pub_count) {
    for (uint32_t count = 0; count < pub_count; ++count) {
        uint64_t forward_pipe_id_ = 0;
        Log* logptr = new Log();
        logptr->set_debug_info( NumberToString(static_cast<ssize_t>(count)) );

        // write replay log
        if (error::OK != pe->PutReplayLogs(peid, pipe_name_, logptr, forward_pipe_id_)) {
            DSTREAM_ERROR("put replay log(%s) failed, forward_pipe_id(%llu)", pipe_name_.c_str(),
                          forward_pipe_id_);
        }

        delete logptr;
    }

}

TEST_F(TestPNReplay, test_get_replay_logs)
{
    uint64_t peid = 850829;
    vector<dstream::Log*> replay_logs;
    vector<dstream::Log*>::iterator replay_log_iter;

    std::string test_pipe_name("test_pipe");
    InitTest(test_pipe_name);

    // pipe is not exist
    ASSERT_EQ(error::OK, GetPEWrapper()->GetReplayLogs(peid, test_pipe_name, 0, replay_logs));
    ASSERT_EQ(1, replay_logs.size());
    replay_log_iter = replay_logs.begin();
    ASSERT_STREQ("END_REPLAY", (*replay_log_iter)->debug_info().c_str());
    delete (*replay_log_iter);
    replay_logs.clear();

    // pipe exist, but is empty
    int fd = ::open(test_pipe_name.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666);
    ASSERT_NE(-1, fd);
    ::close(fd);
    ASSERT_EQ(error::OK, GetPEWrapper()->GetReplayLogs(peid, test_pipe_name, 0, replay_logs));
    ASSERT_EQ(1, replay_logs.size());
    replay_log_iter = replay_logs.begin();
    ASSERT_STREQ("END_REPLAY", (*replay_log_iter)->debug_info().c_str());
    delete (*replay_log_iter);
    replay_logs.clear();

    DeinitTest();
}

TEST_F(TestPNReplay, test_pub_sub)
{
    uint64_t peid = 850829;

    InitTest("test_pub_sub_pipe");

    PublishTest(peid, 100);
    uint32_t count = 0;
    SubscribeTest(peid, 0, count);
    ASSERT_EQ(100, count);

    DeinitTest();
}

} // end dstream

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_replay");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
