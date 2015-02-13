/** 
* @file     test_pn_httpd.cc
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
#include "processnode/pn_httpd.h"
#include "processnode/process_node.h"
namespace dstream {
namespace test_pn_httpd {
class TestPNHttpd : public ::testing::Test {
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
    static dstream::pn_httpd::PnHttpdResponder responder;
    static pthread_t httpd_thread;
    volatile static uint32_t pn_httpd_port;
};

volatile int32_t TestPNHttpd::thread_signal = HTTPD_START;
pthread_t TestPNHttpd::httpd_thread;
dstream::pn_httpd::PnHttpdResponder TestPNHttpd::responder;
volatile uint32_t TestPNHttpd::pn_httpd_port;

void* TestPNHttpd::HttpRoutine(void* args) {
    std::cout << "Start thread httpd" << std::endl;
    void *context = NULL;
    std::string httpd_port;
    pn_httpd_port = dstream::config_const::kMinDynamicHttpdPort;
    for (; pn_httpd_port < dstream::config_const::kMaxDynamicHttpdPort; ++pn_httpd_port) {
        httpd_port = "";
        dstream::StringAppendNumber(&httpd_port, pn_httpd_port);
        if(dstream::httpd::HttpdStart(httpd_port, &context, &responder)
                < dstream::error::OK) {
            std::cout << "unable to start httpd port on " << pn_httpd_port<< endl;
        } else {
            break;
        }
        usleep(1000);
    }
    if (pn_httpd_port >= dstream::config_const::kMaxDynamicHttpdPort) {
        DSTREAM_WARN("unable to start pn httpd server");
        return NULL;
    }
    thread_signal = TestPNHttpd::HTTPD_RUNNING;
    while(thread_signal != TestPNHttpd::HTTPD_STOP) {
        usleep(1000);
    }
    dstream::httpd::HttpdStop(context);
    context = NULL;
    return NULL;
}

void TestPNHttpd::SetUp() {
    DSTREAM_INFO("wait 35 seconds for counter file to cold down");
    sleep(35);
    dstream::processnode::ProcessNode::InitProcessNodeContext();
    thread_signal = HTTPD_START;
    int ret =  pthread_create (&httpd_thread, NULL, HttpRoutine, NULL); 
    if (ret) {
        std::cout << "can't create httpd thread, ret="<< ret << std::endl;
        exit(ret);
    }
}
void TestPNHttpd::TearDown() {
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
void TestPNHttpd::TestGetHttpdData() {
    while(thread_signal != TestPNHttpd::HTTPD_RUNNING) {
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
    dstream::StringAppendNumber(&url_base, pn_httpd_port);

    url = url_base;
    cmd = http_getr;
    cmd += " ";
    cmd += url;

    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    std::string expect_return = "/transporter,/pewrapper";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

    url = url_base + "/transporter";
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());
    CmdSystem("touch ./counter/123.cnt.json", cmd_buf, 100*1024);
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{\"123\":test,test,test}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());
    std::cout << "wait for 35 seconds for invaild of counter file" << std::endl;
    sleep(35);
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());
    
    

    url = url_base + "/pewrapper";
    cmd = http_getr;
    cmd += " ";
    cmd += url;
    CmdSystem(cmd.c_str(), cmd_buf, 100*1024);
    result = cmd_buf;
    expect_return = "{\"pe_wrapper_mgr\":{\"normal_pe_kv\":[],\"fail_pe_kv\":[]}}";
    ASSERT_STREQ(result.c_str(), expect_return.c_str());

}

TEST_F(TestPNHttpd, TestPNHttpdService) { 
   TestGetHttpdData(); 
}


} // namespace test_pn_httpd
} // namespace dstream

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_pn_httpd");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
