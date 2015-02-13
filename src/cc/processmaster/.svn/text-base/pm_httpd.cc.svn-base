/** 
* @file     pm_httpd.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-27
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include "processmaster/pm_httpd.h"
#include "processmaster/pn_client_status.h"
namespace dstream {
namespace pm_httpd {
dstream::error::Code PmHttpdResponder::GetEventManagerData(std::string* output) {
    assert(NULL != output);
    if(pm_ptr == 0) {
        DSTREAM_WARN("ProcessMaster is not started, can't handle http request");
        return dstream::error::REQUEST_ERROR;
    } 
    DSTREAM_DEBUG("httpd handle a request for getting pm event manager info");
    if(pm_ptr->event_manager() != NULL) {
        return pm_ptr->event_manager()->Dump(output);
    } else {
        return dstream::error::PM_RESOURCE_NOT_AVAILABLE;
    }
}
dstream::error::Code PmHttpdResponder::GetSchedulerData(std::string* output) {
    assert(NULL != output);
    if(pm_ptr == 0) {
        DSTREAM_WARN("ProcessMaster is not started, can't handle http request");
        return dstream::error::REQUEST_ERROR;
    } 
    DSTREAM_DEBUG("httpd handle a request for getting pm scheduler info");
    if(pm_ptr->scheduler() != NULL) {
        return pm_ptr->scheduler()->Dump(output);
    } else {
        return dstream::error::PM_RESOURCE_NOT_AVAILABLE;
    }   

}

dstream::error::Code PmHttpdResponder::GetPNClientStatusData(std::string* output) {
    assert(NULL != output);
    if(pm_ptr == 0) {
        DSTREAM_WARN("ProcessMaster is not started, can't handle http request");
        return dstream::error::REQUEST_ERROR;
    } 
    DSTREAM_DEBUG("httpd handle a request for getting pn client status info");
    dstream::pn_client_status::PNClientStatus* pn_status_instance = 
        dstream::pn_client_status::PNClientStatus::GetPNClientStatus();
    if(NULL == pn_status_instance) {
        return dstream::error::PM_RESOURCE_NOT_AVAILABLE;
    }   
    return pn_status_instance->Dump(output);
}

dstream::error::Code PmHttpdResponder::GetRootData(std::string* output) {
    assert(NULL != output);
    std::string ret_buf;
    ret_buf = "/eventmanager,/resourcescheduler,/pnclientstatus";
    output->assign(ret_buf);
    return dstream::error::OK;
}

PmHttpdResponder::PmHttpdResponder() {
    m_request_map.clear();
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/eventmanager",
                                            PmHttpdResponder::GetEventManagerData));
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/resourcescheduler",
                                            PmHttpdResponder::GetSchedulerData));
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/pnclientstatus",
                                            PmHttpdResponder::GetPNClientStatusData));
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/",
                                            PmHttpdResponder::GetRootData));
}

} // namespace pm_httpd
} // namespace dstream
