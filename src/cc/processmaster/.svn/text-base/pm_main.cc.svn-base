/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    pm_main.cc
* @author:      zhanggengxin@baidu.com
* @brief:       main functions used for pm
*/
#include <signal.h>
#include "common/error.h"
#include "common/dstream_optional.h"
#include "metamanager/meta_manager.h"
#include "processmaster/pm_httpd.h"
#include "processmaster/process_master.h"

using namespace dstream;
using namespace dstream::meta_manager;
::dstream::process_master::ProcessMaster* pm_ptr = 0;

void sigterm_handler(int signal) {
    MetaManager* meta = MetaManager::GetMetaManager();
    if (0 != meta && 0 != pm_ptr) {
        // if pm is working master, delete pm
        if (pm_ptr->working_master()) {
            meta->DeletePM();
        }
        DSTREAM_WARN("PM exit for signal [%d]", signal);
        // kill self
        kill(getpid(), SIGKILL);
    }
}

int main(int argc, char** argv) {
    AddOptional("-v", PrintPMVersion);
    AddOptional("--version", PrintPMVersion);
    if (argc > 1 && argv[1][0] == '-') {
        ParseOptional(argc - 1, argv + 1);
    }
    pm_ptr = 0;
    ::dstream::logger::initialize("processmaster");
    ::dstream::process_master::ProcessMaster pm;
    bool start_res = false;

    // for httpd server, get the port
    // start httpd server
    dstream::pm_httpd::PmHttpdResponder responder;
    void * httpd_context = NULL;
    uint32_t pm_httpd_port = config_const::kMinDynamicHttpdPort;
    for (; pm_httpd_port < config_const::kMaxDynamicHttpdPort; ++pm_httpd_port) {
        std::string httpd_port = "";
        StringAppendNumber(&httpd_port, pm_httpd_port);
        if(dstream::httpd::HttpdStart(httpd_port, &httpd_context, &responder) < 
                dstream::error::OK) {
            DSTREAM_WARN("could not start pm httpd server on port [%u]", pm_httpd_port);
        } else {
            break;
        }
        usleep(1000);
    }
    if (pm_httpd_port >= config_const::kMaxDynamicHttpdPort) {
        DSTREAM_WARN("unable to start pm httpd server");
        return -1;
    }
    pm.SetHttpdPort(pm_httpd_port);
    if (argc > 1) {
        start_res = pm.Start(argv[1]);
    } else {
        start_res = pm.Start(NULL);
    }
    if (start_res) {
        pm_ptr = &pm;
        signal(SIGTERM, sigterm_handler);
    }
    pm.Join();
    // when pm exit , stop the httpd server
    dstream::httpd::HttpdStop(httpd_context);
    DSTREAM_INFO("process master exit");
    ::dstream::meta_manager::MetaManager::DestoryMetaManager();
}
