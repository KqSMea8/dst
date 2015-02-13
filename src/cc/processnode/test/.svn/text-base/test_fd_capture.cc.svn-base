#include <stdio.h>
#include <unistd.h>
#include "gtest/gtest.h"

#include "common/error.h"
#include "common/logger.h"
#include "common/utils.h"
#include "common/file_meta.h"
#include "common/thread.h"

#include "processnode/fd_capture.h"

using namespace std;
using namespace dstream;
using namespace thread;
using namespace filemeta;
using namespace dstream::processnode;

class TestFdCapture : public ::testing::Test {
protected:
    void SetUp() {
        system("mkdir -p ./capture_data/out");
    }
    void TearDown() {
        system("rm ./capture_data/out/* -rf");
    }
};

class SleepExitThread: public Thread {
public:
    SleepExitThread(int usecond): usecond_(usecond) {
    }
private:
    void Run() {
        usleep(usecond_);
        FdCapture::GetInstance()->Stop();
        DSTREAM_DEBUG("Stopped....");
    }
    int usecond_;

};

class WriteThread: public Thread {
public:
    WriteThread(int rdfd, int wtfd, int waitus = 0): rdfd_(rdfd), wtfd_(wtfd), us_(waitus) {
    }
private:
    int rdfd_;
    int wtfd_;
    int us_;
    int buf[8192];
    void Run() {
        int len;
        while ((len = read(rdfd_, buf, 8192)) > 0) {
            DSTREAM_DEBUG("write thread read buf len: 8192");
            if (write(wtfd_, buf, len) <= 0 || m_stop) {
                break;
            }
            if (us_ > 0) {
                usleep(us_);
            }
        }
    }
};

TEST_F(TestFdCapture, CommonFileFDTest) {
    FdCapture* capture = FdCapture::GetInstance();
    int fd = open("./capture_data/in/1.data", O_RDONLY);
    int fd2 = open("./capture_data/in/2.data", O_RDONLY);
    capture->AddWatcher(FdWatcher::FdWatcherPtr(new FdWatcher("./capture_data/out" , "1.data", fd,
                                                -1)));
    capture->AddWatcher(FdWatcher::FdWatcherPtr(new FdWatcher("./capture_data/out", "2.data", fd2,
                                                -1)));
    SleepExitThread st(1300 * 1000); // about 1.2 second
    int pipe_1[2];
    int pipe_2[2];
    int fd_d1 = open("./capture_data/in/1.data", O_RDONLY);
    int fd_d2 = open("./capture_data/in/2.data", O_RDONLY);
    ASSERT_EQ(0, pipe(pipe_1));
    ASSERT_EQ(0, pipe(pipe_2));
    capture->AddWatcher(FdWatcher::FdWatcherPtr(new FdWatcher("./capture_data/out/", "3.data",
                                                pipe_1[0], -1)));
    capture->AddWatcher(FdWatcher::FdWatcherPtr(new FdWatcher("./capture_data/out/", "4.data",
                                                pipe_2[0], -1)));
    WriteThread wt1(fd_d1, pipe_1[1], 400000);
    WriteThread wt2(fd_d2, pipe_2[1]);
    st.Start();
    wt1.Start();
    wt2.Start();
    signal(SIGPIPE,
           SIG_IGN); // WriteThread wt1 will get a sigpipe....my afternoon is gone with the signal...
    capture->Start(1); // ...hang...until Stop is called
    std::map<string, string> meta;
    ASSERT_EQ(dstream::error::OK, GetMeta("./capture_data/out", &meta));
    meta.erase(".svn"); // sometimes we should deal something not really exists in the real situation
    ASSERT_EQ(4lu, meta.size());
    std::map<string, string> orig_meta;
    ASSERT_EQ(dstream::error::OK, GetMeta("./capture_data/in", &orig_meta));
    map<string, string>::iterator iter = meta.begin();
    string& content1 = (iter)->second;
    string& content2 = (++iter)->second;
    // string& content3 = (++iter)->second;
    iter++;
    string& content4 = (++iter)->second;
    ASSERT_EQ(orig_meta["1.data"], content1);
    ASSERT_EQ(orig_meta["2.data"], content2);
    //ASSERT_EQ(orig_meta["1.data"],content3);
    // _/\_
    //  ||
    //  =============================
    //               ||
    // because of the sleep..This ASSERT_EQ will fail.
    ASSERT_EQ(orig_meta["2.data"], content4);
    ClearMeta("./capture_data/out", meta);
}


int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_out_capture");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
