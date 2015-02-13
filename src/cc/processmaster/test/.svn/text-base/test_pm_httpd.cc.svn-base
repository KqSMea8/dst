/** 
* @file     test_pm_httpd.cc
* @brief    test pm httpd function
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-22
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include "gtest/gtest.h"
#include "common/auto_ptr.h"
#include "common/error.h"
#include "common/config_const.h"
#include "processmaster/event_manager.h"
#include "processmaster/pm_httpd.h"
#include "scheduler/scheduler.h"
using namespace dstream::scheduler;
using namespace dstream::event_manager;

::dstream::process_master::ProcessMaster *pm_ptr = 0;
namespace dstream {
namespace test_pm_httpd {
class TestPMHttpd : public ::testing::Test {
public:
    enum {
        HTTPD_START,
        HTTPD_RUNNING,
        HTTPD_STOP
    };
    static void* HttpRoutine(void *args);
    void TestGetHttpdData();
protected:
    void SetUp();
    void TearDown();
private:
    volatile static int32_t thread_signal;
    ::dstream::process_master::ProcessMaster pm;
    static dstream::pm_httpd::PmHttpdResponder responder;
    static pthread_t httpd_thread;
    volatile static uint32_t pm_httpd_port;
};

volatile int32_t TestPMHttpd::thread_signal = HTTPD_START;
pthread_t TestPMHttpd::httpd_thread;
dstream::pm_httpd::PmHttpdResponder TestPMHttpd::responder;
volatile uint32_t TestPMHttpd::pm_httpd_port = 0;

void* TestPMHttpd::HttpRoutine(void* args) {
    std::cout << "Start thread httpd" << std::endl;
    void *context = NULL;
    std::string httpd_port;
    pm_httpd_port = dstream::config_const::kMinDynamicHttpdPort;
    for (; pm_httpd_port < dstream::config_const::kMaxDynamicHttpdPort; ++pm_httpd_port) {
        httpd_port = "";
        dstream::StringAppendNumber(&httpd_port, pm_httpd_port);
        if(dstream::httpd::HttpdStart(httpd_port, &context, &responder)
                < dstream::error::OK) {
            std::cout << "unable to start httpd port on " << pm_httpd_port<< endl;
        } else {
            break;
        }
        usleep(1000);
    }
    if (pm_httpd_port >= dstream::config_const::kMaxDynamicHttpdPort) {
        DSTREAM_WARN("unable to start pm httpd server");
        return NULL;
    }
    thread_signal = TestPMHttpd::HTTPD_RUNNING;
    while(thread_signal != TestPMHttpd::HTTPD_STOP) {
        usleep(1000);
    }
    dstream::httpd::HttpdStop(context);
    context = NULL;
    return NULL;
}

void TestPMHttpd::SetUp() {
    pm.SetHttpdPort(8000);
    pm.Start("./pm_test.xml");
    pm_ptr = &pm;
    thread_signal = HTTPD_START;
    int ret =  pthread_create (&httpd_thread, NULL, HttpRoutine, NULL); 
    if (ret) {
        std::cout << "can't create httpd thread, ret="<< ret << std::endl;
        exit(ret);
    }
}
void TestPMHttpd::TearDown() {
    thread_signal = HTTPD_STOP;
    pthread_join(httpd_thread, NULL);
}

void CmdSystem(const char* command, char *buf, int buf_len) {
    assert(command != NULL && buf != NULL && buf_len > 0);
    FILE *fpRead;
    fpRead = popen(command, "r");
    int ibuflen = 0;
    memset(buf,'\0',buf_len);
    int tr = 0;
    while((tr = fgetc(fpRead))!=EOF) {
        if(ibuflen < buf_len - 1) {
            buf[ibuflen++] = (char)tr;
        } else {
            std::cout << "out of memory buffer" << std::endl;
            break;
        }
    }
    buf[ibuflen] = '\0';
    if(fpRead!=NULL)
        pclose(fpRead);
}
void TestPMHttpd::TestGetHttpdData() {
    while (thread_signal != TestPMHttpd::HTTPD_RUNNING) {
        usleep(1000);
    }
    char host_name[255];
    char cmd_buf[100*1024];
    const char * http_getr = "lwp-request";
    std::string url_base("http://");
    std::string url;
    std::string cmd;
    std::string result;
    int ret = gethostname(host_name, 255);
    if(ret) {
        std::cout << "can't get host name, ret =" << ret << std::endl;
        exit(ret);
    }
    url_base += host_name;
    url_base += ":";
    dstream::StringAppendNumber(&url_base, pm_httpd_port);

    url = url_base;
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    std::cout << cmd <<std::endl;

    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    std::string expect_return = "/eventmanager,/resourcescheduler,/pnclientstatus";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

    url = url_base + "/eventmanager";
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{\"excute_event_map\":"
                    "{\"event_kv\":[]},\"all_event_map\":"
                    "{\"event_kv\":[]},\"delay_event_map\":{\"delay_event_kv\":[]}}" ;
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

    url = url_base + "/resourcescheduler";
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{\"pn_resource_statics\":{\"pn_statics_kv\":[]}}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

    url = url_base + "/pnclientstatus";
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{\"pn_reports\":{\"pnid_report_kv\":[]}}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

}

TEST_F(TestPMHttpd, TestPMHttpdService) { 
   TestGetHttpdData(); 
}


} // namespace test_pm_httpd
} // namespace dstream
