/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_PROCESSNODE_PE_STATUS_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PE_STATUS_H__ // NOLINT

#include <boost/foreach.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "common/mutex.h"
#include "common/proto/pm_pn_rpc.pb.h"

#include "processelement/metric.h"

namespace dstream {
namespace processnode {

class PEStatus {
public:
    typedef std::map<std::string, std::string> PEMetricsMap;
    typedef std::pair<std::string, std::string> PEMetricsPair;
    typedef std::map<uint64_t, BackupPEStatus*> PEStatusMap;
    typedef std::pair<uint64_t, BackupPEStatus*> PEStatusPair;

    PEStatus() {}
    virtual ~PEStatus() {
        // need lock here?
        MutexLockGuard lock_guard(m_status_lock);
        BOOST_FOREACH(const PEStatusPair & item, m_status) {
            delete item.second;
        }
        m_status.clear();
    }

    BackupPEStatus GetPEStatus(uint64_t peid) const {
        BackupPEStatus* status = NULL;
        MutexLockGuard lock_guard(m_status_lock);
        PEStatusMap::const_iterator it = m_status.find(peid);
        if (it == m_status.end()) {
            status = new (std::nothrow) BackupPEStatus();
            if (NULL != status) {
                status->mutable_pe_id()->set_id(peid);
                m_status[peid] = status;
            }
        } else {
            status = it->second;
        }
        if (NULL == status) {
            BackupPEStatus s;
            return s;
        }
        return *status;
    }

    void DeletePEStatus(uint64_t peid) {
        MutexLockGuard lock_guard(m_status_lock);
        PEStatusMap::iterator it = m_status.find(peid);
        if (it != m_status.end()) {
            BackupPEStatus* tmp = it->second;
            m_status.erase(it);
            delete tmp;
        }
    }

    void SetMetricInfo(uint64_t peid,
            const PEMetricsMap& metrics) {
        MutexLockGuard lock_guard(m_status_lock);
        BackupPEStatus* status = get_pe_status_(peid);
        if (status) {
            status->clear_metric_name();
            status->clear_metric_value();
            BOOST_FOREACH(const PEMetricsPair & item, metrics) {
                if (item.first == processelement::internal::Metric::kCpuUsageCounter) {
                    status->set_cpu_used(strtol(item.second.c_str(), NULL, 10));
                } else if (item.first == processelement::internal::Metric::kMemUsageCounter) {
                    status->set_memory_used(strtol(item.second.c_str(), NULL, 10)>>10); // to KB
                    DSTREAM_INFO("PE[%lu] Memory: %ld MB",
                            peid, strtol(item.second.c_str(), NULL, 10)>>20);
                } else {
                    status->add_metric_name(item.first);
                    status->add_metric_value(item.second);
                    if (item.first == processelement::internal::Metric::kRecvTimeTuplesCounter) {
                        DSTREAM_INFO("PE[%lu] RECV_QPS: %s", peid, item.second.c_str());
                    }                     
                    if (item.first == processelement::internal::Metric::kSendTimeTuplesCounter) {
                        DSTREAM_INFO("PE[%lu] SEND_QPS: %s", peid, item.second.c_str());
                    }       
                }
                //DSTREAM_INFO("PE[%lu] %s: %s", peid, item.first.c_str(), item.second.c_str());
            }
        } else {
            DSTREAM_ERROR("pe(%lu) fail to set metric status, may out of memory", peid);
        }
    }

protected:
    BackupPEStatus* get_pe_status_(uint64_t peid) {
        BackupPEStatus* status = NULL;
        PEStatusMap::const_iterator it = m_status.find(peid);
        if (it == m_status.end()) {
            status = new (std::nothrow) BackupPEStatus();
            if (NULL != status) {
                status->mutable_pe_id()->set_id(peid);
                m_status[peid] = status;
            }
        } else {
            status = it->second;
        }
        return status;
    }

private:
    mutable MutexLock   m_status_lock;
    mutable PEStatusMap m_status;
}; // class PEStatus

} // namespace processnode
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSNODE_PE_STATUS_H__ NOLINT
