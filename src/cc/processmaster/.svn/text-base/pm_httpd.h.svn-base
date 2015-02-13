/** 
* @file     pm_httpd.h
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-27
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#ifndef __DESTREAM_CC_PROCESSMASTER_PM_HTTPD_H__
#define __DESTREAM_CC_PROCESSMASTER_PM_HTTPD_H__
#include <string>
#include <map>
#include "common/error.h"
#include "common/dstream_httpd.h"
#include "processmaster/process_master.h"

extern ::dstream::process_master::ProcessMaster* pm_ptr;

namespace dstream {
namespace pm_httpd {
class PmHttpdResponder : public httpd::HttpdResponder {
public:
    PmHttpdResponder(); 
    virtual ~PmHttpdResponder() {}
    static dstream::error::Code GetEventManagerData(std::string* output);
    static dstream::error::Code GetSchedulerData(std::string* output);
    static dstream::error::Code GetPNClientStatusData(std::string* output);
    static dstream::error::Code GetRootData(std::string* output);
};
} // pm_httpd
} // namespace dstream

#endif
