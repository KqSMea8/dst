#include "common/logger.h"
#include <iostream>

using namespace std;
using namespace dstream;
using namespace logger;
using namespace log4cplus;

int main() {
    logger::initialize("processmaster");
    string a = "start";
    //LOG4CPLUS_DEBUG(logger::logger, "test");
    //logger::logger->log(log4cplus::INFO_LOG_LEVEL, "logger, test is ", __FILE__, __LINE__);
    DSTREAM_ERROR("masterapp// error, test 1 is start");
    DSTREAM_INFO("masterapp// test[%d] is %s", 1, a.c_str());
    printf("masterapp// print, test[%d] is %s\n", 1, a.c_str());

    logger::initialize("processnode");
    DSTREAM_ERROR("nodeapp// error, test 1 is start");
    DSTREAM_INFO("nodeapp// test[%d] is %s", 1, a.c_str());
    printf("nodeapp// print, test[%d] is %s\n", 1, a.c_str());
}
