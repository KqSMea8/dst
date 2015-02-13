/** 
* @file     pn_httpd.h
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-27
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#ifndef __DESTREAM_CC_PROCESSNODE_PN_HTTPD_H__
#define __DESTREAM_CC_PROCESSNODE_PN_HTTPD_H__
#include <string>
#include <map>
#include "common/error.h"
#include "common/dstream_httpd.h"


namespace dstream {
namespace pn_httpd {
class PnHttpdResponder : public httpd::HttpdResponder {
public:
    PnHttpdResponder(); 
    virtual ~PnHttpdResponder() {}
    static dstream::error::Code GetRootData(std::string* output);
    static dstream::error::Code GetTransporterData(std::string* output);
    static dstream::error::Code GetPEWrapperMgrData(std::string* output);
};
} // pm_httpd
} // namespace dstream

#endif
