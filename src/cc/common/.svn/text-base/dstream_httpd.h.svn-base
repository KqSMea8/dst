/** 
* @file     dstream_httpd.h
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-20
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#ifndef __DSTREAM_CC_COMMON_DSTREAM_HTTPD_H__
#define __DSTREAM_CC_COMMON_DSTREAM_HTTPD_H__
#include <map>
#include <string>
#include "common/error.h"
#include "mongoose/mongoose.h"
namespace dstream {
namespace httpd {

typedef dstream::error::Code (*DataRequestHandler)(std::string *);
typedef std::map<std::string, DataRequestHandler> RequestMap;
typedef std::map<std::string, DataRequestHandler>::iterator RequestMapIter;

class HttpdResponder {
public:
    HttpdResponder() {}
    virtual ~HttpdResponder() {}
    virtual RequestMap GetRequestMap() const{
        return m_request_map;
    }
protected:
    RequestMap m_request_map;
};

/** 
* @brief    start the httpd server
* 
* @param    listen_port, http serve port to bind
* 
* @return   context of mongoose httpd server
*           if error return NULL;
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-08-20
*/
int32_t  HttpdStart(const std::string& listen_port, void **context, HttpdResponder *responder);

/** 
* @brief    stop the httpd server
* 
* @param    httpd_context, httpd_context has been 
*           return by HttpdStart
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-08-20
*/
void HttpdStop(void * httpd_context);

} // namespace process_master
} // namespace dstream
#endif

