/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/7/3
* @filename:    pn_reporter.h
* @author:      lanbijia
* @brief:       process node report to process master
*/

#include "processnode/pn_reporter.h"

#include <string>
#include <vector>

#include "common/counters.h"
#include "processnode/process_node.h"


namespace dstream {
namespace processnode {

PNReporter* g_pn_reporter = NULL;

PNReporter::~PNReporter() {
    Stop();
}

void PNReporter::OnNodeDelete(const char* /*path*/) {
    WatchPM(); // rewatch
    // set reconnect pm
    set_pm_reconnect();
}

void PNReporter::OnDataChange(const char* /*path*/) {
    // set reconnect pm
    set_pm_reconnect();
}

void PNReporter::OnSessionExpire(const char* /*path*/) {
    DSTREAM_ERROR("zookeeper session expired, pn going to quit.");
    // set g_quit flag
    AtomicSetValue(g_quit, 1);
    // once zk session expired, suicide maybe a good choice
    Stop(false); // main() will clear the environment
}


int32_t PNReporter::WatchPM() {
    int ret = error::OK;
    do {
        ret = StartWatchPM();
        if (ret < error::OK) {
            DSTREAM_DEBUG("pn reporter : fail to watch pm");
        }
        sleep(config_const::kReconnectToPMIntervalSec);
    }
    while (ret != error::OK && !m_stop);
    return error::OK;
}

int32_t PNReporter::ConnectPM() {
    assert(g_zk);
    // get pm info from zookeeper
    PM pm;
    int32_t ret = g_zk->GetPM(&pm);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to get pm from zookeeper, errno(%d)", ret);
        return ret;
    }
    // get pm addr
    std::string pm_addr = rpc_const::GetUri(pm.host(), pm.report_port());
    DSTREAM_DEBUG("get pm addr(%s) from zookeeper", pm_addr.c_str());

    DisconnectPM();

    // create reporter connection
    DSTREAM_INFO("create reporter connection to PM(%s) for reporting", pm_addr.c_str());
    m_report_pm_conn = new rpc::Connection();
    assert(m_report_pm_conn);
    if (m_report_pm_conn->Connect(pm_addr.c_str()) < 0) {
        DSTREAM_WARN("reporter connect PM(%s) failed", pm_addr.c_str());
        DisconnectPM();
        return error::MASTER_CONNECT_FAIL;
    }
    m_pm_reconnect = false;
    return error::OK;
}

void PNReporter::DisconnectPM() {
    if (NULL != m_report_pm_conn) {
        delete m_report_pm_conn;
        m_report_pm_conn = NULL;
    }
}

void PNReporter::Run() {
    int rpc_timeout;
    g_pn_cfg_map.GetIntValue(config_const::kPNToPMTimeoutName, &rpc_timeout);

    WatchPM();

    while (!m_stop) {
        if (m_pm_reconnect) {
            if (ConnectPM() < error::OK) {
                sleep(config_const::kReconnectToPMIntervalSec);
                continue;
            }
        }

        // send report request to PM
        DSTREAM_DEBUG("PN send report request to PM...");
        proto_rpc::RPCRequest< PNReport > req;
        proto_rpc::RPCResponse< ReportResponse > res;
        req.Clear();
        res.Clear();
        req.set_method(rpc_const::kPNReport);

        std::vector<PEID> peid_vector;

        // set pn id
        *(req.mutable_id()) = ProcessNode::GetProcessNode()->GetPNMeta().pn_id();

        // set resource
        PNResourceStatus* pn_resources = req.mutable_resource();
        Resource* total_resource = pn_resources->mutable_total_resource();
        total_resource->set_memory(m_total_mem);
        total_resource->set_cpu(m_total_cpu);

        // set pe info & fail pe info
        PEWrapperManager::GetPEWrapperManager()->GetPEInfo(&req, &peid_vector);

        if (m_report_pm_conn->Call(&req, &res, rpc_timeout) < 0) {
            DSTREAM_WARN("PN send_pm_thread_function, connection call(%s) failed",
                         rpc_const::kPNReport);
            DisconnectPM();

            sleep(config_const::kReconnectToPMIntervalSec);
            m_pm_reconnect = true;
            continue;
        }

        // clear fail pe info
        PEWrapperManager::GetPEWrapperManager()->ClearFailPEInfo(&peid_vector);

        MutexLockGuard CondLock(m_lock);
        m_time_cond.timed_wait(config_const::kDefaultReportIntervalSec * 1000);
    }

    DisconnectPM();
    // unwatch PM
    if (UnWatchPM() < error::OK) {
        DSTREAM_ERROR("PN fail to unwatch PM");
    }
}

void PNReporter::SignalCond() {
    MutexLockGuard CondLock(m_lock);
    m_time_cond.notify();
}

} // namespace processnode
} // namespace dstream


