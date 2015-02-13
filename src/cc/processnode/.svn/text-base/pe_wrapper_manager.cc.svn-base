/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/07/09
* @filename:    pe_wrapper_manager.cc
* @author:      lanbijia
* @brief:       PEWrapper manager
*/
#include <utility>
#include "boost/algorithm/string/trim.hpp"
#include "common/application_tools.h"
#include "common/hdfs_client.h"

#include "processnode/pe_wrapper_manager.h"
#include "processnode/process_node.h"
#include "pb_to_json.h"

namespace dstream {
namespace processnode {

LogCaptureThread::LogCaptureThread(int max_roll_time): max_roll_time_(max_roll_time) {
    Start();
}

void LogCaptureThread::Run() {
    FdCapture::GetInstance()->Start(max_roll_time_);
}

LogCaptureThread::~LogCaptureThread() {
    FdCapture::GetInstance()->Stop();
}

const int kPELogMaxRollTime = 1 * 60 * 60;

PEWrapperManager::PEWrapperManager()
    : m_log_capture_thread(kPELogMaxRollTime),
      m_is_destory(false) {
}

PEWrapperManager::~PEWrapperManager() {
    DestoryAllPEWrapper();
}


string PEWrapperManager::DumpPEWrapperMapInfo() {
    string ret = "";
    PEWrapperMap::const_iterator map_iter;
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
        ret += "pe wrapper :\n";
        for (map_iter = m_pe_wrapper_map.begin();
             map_iter != m_pe_wrapper_map.end(); map_iter++) {
            ret += map_iter->second->DumpPEInfo() + "\n";
        }
    }
    return ret;
}

string PEWrapperManager::DumpPEUpDownStreamInfo(uint64_t peid) {
    string dump_info;
    PEWrapperMap::const_iterator map_iter;

    RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
    map_iter = m_pe_wrapper_map.find(peid);
    if (map_iter != m_pe_wrapper_map.end()) {
        dump_info += map_iter->second->DumpUpStreamInfo();
        dump_info += map_iter->second->DumpDownStreamInfo();
    }

    return dump_info;
}

string PEWrapperManager::DumpPEIDList() {
    string peid_list_info;
    PEWrapperMap::const_iterator map_iter;

    RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
    for (map_iter = m_pe_wrapper_map.begin(); map_iter != m_pe_wrapper_map.end(); map_iter++) {
        uint64_t peid = map_iter->second->peid().id();
        StringAppendNumber(&peid_list_info, peid);
        peid_list_info += " ";
    }

    return peid_list_info;
}
std::string PEWrapperManager::PrintStatus() {
    std::string s;
    return s;
}

PEWrapper::PEWrapperPtr PEWrapperManager::GetPEWrapper(uint64_t peid) {
    PEWrapperMap::const_iterator map_iter;
    PEWrapper::PEWrapperPtr wrapper;
    // find pe wrapper by peid
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
        map_iter = m_pe_wrapper_map.find(peid);
        if (map_iter != m_pe_wrapper_map.end()) {
            wrapper = map_iter->second;
        } else {
            DSTREAM_WARN("[%s] peid:[%lu] not found in the wrapper map",
                         __FUNCTION__, peid);
        }
    }
    return wrapper;
}

error::Code PEWrapperManager::CheckAndInsertPEWrapper(
    const PEID& peid,
    const AppID& app_id,
    const Processor& processor,
    const BackupPEID& bpeid,
    const uint64_t last_assign_time) {
    error::Code ret = error::OK;
    PEWrapperMap::const_iterator it;
    CompeletePEID cpeid;
    uint64_t pe_id = peid.id();
    if (m_is_destory) {
        DSTREAM_WARN("PEWrapperManager destoryed, cannot add PE wrapper(%lu)", pe_id);
        return error::FAILED_EXPECTATION;
    }

    PEWrapper::PEWrapperPtr wrapper(new (std::nothrow) PEWrapper());
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, true);
        if (wrapper == NULL) {
            ret = error::BAD_MEMORY_ALLOCATION;
            goto ERR_RET;
        }

        *(cpeid.mutable_app_id())    = app_id;
        *(cpeid.mutable_pe_id())     = peid;
        *(cpeid.mutable_processor()) = processor;
        *(cpeid.mutable_backup_id()) = bpeid;

        it = m_pe_wrapper_map.find(pe_id);
        if (it != m_pe_wrapper_map.end()) {
            DSTREAM_WARN("[%s] PE(%lu) already running on this pn, will not create it again!",
                         __FUNCTION__, pe_id);
            ret = error::PE_ALREADY_EXISTS;
            goto ERR_RET;
        }

        wrapper->set_peid(peid);
        wrapper->set_processor(processor);
        wrapper->set_cpeid(cpeid);
        wrapper->set_last_assign_time(last_assign_time);
        wrapper->set_status(PEWrapper::CREATING);

        std::pair<PEWrapperMap::iterator, bool> map_ret = 
            m_pe_wrapper_map.insert(std::pair<uint64_t, PEWrapper::PEWrapperPtr>(pe_id, wrapper));
        if (map_ret.second == false) {
            DSTREAM_WARN("PE(%lu) wrapper already exists, insert failed.", pe_id);
            ret = error::PEWRAPPER_ALREADY_EXIST;
            goto ERR_RET;
        }
    }
    // init transporter & router
    if ((ret = wrapper->InitLocalPE()) < error::OK) {
        // start to gc pe wrapper
        assert(wrapper->StartGCThread(peid.id()) == error::OK);
        goto ERR_RET;
    }

    DSTREAM_INFO("[%s] PE(%lu) wrapper insert success.",
                 __FUNCTION__, pe_id);
    return ret;

ERR_RET:
    DSTREAM_ERROR("[%s] PE(%lu) wrapper create fail, errno(%d).",
                  __FUNCTION__, pe_id, ret);
    return ret;
}

error::Code PEWrapperManager::RemovePEWrapper(uint64_t peid) {
    error::Code ret = error::OK;
    PEWrapperMap::iterator it;
    // kill pe, delete its enviroment, erase map item
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, true);
        // look up pe in pe map
        it = m_pe_wrapper_map.find(peid);
        if (it != m_pe_wrapper_map.end()) {
            if (g_pn_reporter) {
                // clear pn reporter pe status info
                g_pn_reporter->DeletePEStatus(peid);
            }
            // insert failed pe into failed map
            if (m_pe_failed_map.find(peid) == m_pe_failed_map.end()) {
                m_pe_failed_map.insert(
                        std::pair<uint64_t, PEWrapper::PEWrapperPtr>(peid, it->second));
                DSTREAM_DEBUG("insert failed pe[%lu] into pe_failed_map", peid);
            } else {
                // failed pe exist, but HAVE NOT report to PM yet
                DSTREAM_WARN("pe[%lu] exist in pe_failed_map", peid);
            }
            // erase pe wrapper
            m_pe_wrapper_map.erase(it);
        }
    }
    return ret;
}

error::Code PEWrapperManager::GCPEWrapper(uint64_t peid) {
    error::Code ret = error::OK;
    PEWrapperMap::const_iterator it;
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
        // look up pe in pe map
        it = m_pe_wrapper_map.find(peid);
        if (it != m_pe_wrapper_map.end()) {
            // start to gc pe wrapper
            assert(it->second->StartGCThread(peid) == error::OK);
        }
    }
    return ret;
}

int32_t PEWrapperManager::GetPEInfo(proto_rpc::RPCRequest<PNReport>* req,
                                    std::vector<PEID>* fail_peid_vector) {
    // get processor resource information
    std::string run_pe_str;
    PEWrapperMap::iterator map_iter, map_failed_iter;

    std::string failed_pe_str = "";
    fail_peid_vector->clear();
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, false);
        for (map_iter = m_pe_wrapper_map.begin(); map_iter != m_pe_wrapper_map.end(); map_iter++) {
            uint64_t peid = map_iter->second->cpeid().pe_id().id();
            BackupPEStatus* pe = req->add_status();
            if (g_pn_reporter) {
                *pe = g_pn_reporter->GetPEStatus(peid);
            }
            pe->mutable_pe_id()->set_id(peid);
            *(pe->mutable_app_id()) = map_iter->second->cpeid().app_id();
            *(pe->mutable_backup_pd_id()) = map_iter->second->cpeid().backup_id();
            pe->set_revision(map_iter->second->revision());

            // contruct pelist string
            StringAppendNumber(&run_pe_str, peid);
            run_pe_str.append(", ");
        }
        DSTREAM_INFO("PN Reporter WORKING_PE_LIST: %s (%d PEs)",
                     run_pe_str.c_str(), req->status_size());

        for (map_failed_iter = m_pe_failed_map.begin(); map_failed_iter != m_pe_failed_map.end();
             map_failed_iter++) {
            uint64_t peid = map_failed_iter->second->cpeid().pe_id().id();
            FailPE* failed_pe = req->add_fails();
            *(failed_pe->mutable_app_id()) = map_failed_iter->second->cpeid().app_id();
            *(failed_pe->mutable_pe_id()) = map_failed_iter->second->cpeid().pe_id();
            *(failed_pe->mutable_backup_pe_id()) = map_failed_iter->second->cpeid().backup_id();
            failed_pe->set_revision(map_failed_iter->second->revision());

            // contruct fail pelist string
            StringAppendNumber(&failed_pe_str, peid);
            failed_pe_str.append(", ");

            fail_peid_vector->push_back(map_failed_iter->second->cpeid().pe_id());
        }
        DSTREAM_INFO("PN Reporter FAILED_PE_LIST: %s (%d PEs)",
                failed_pe_str.c_str(), req->fails_size());
    }

    return error::OK;
}

void PEWrapperManager::ClearFailPEInfo(std::vector<PEID>* fail_peid_vector) {
    PEWrapperMap::iterator map_failed_iter;
    std::vector<PEID>::iterator iter;
    // reset failed PE list
    RWLockGuard lock_guard(m_pe_wrapper_map_lock, true);
    for (iter = fail_peid_vector->begin(); iter != fail_peid_vector->end(); iter++) {
        // pe failed map erase
        // delete PEWrapper::PEWrapperPtr in pe map
        map_failed_iter = m_pe_failed_map.find(iter->id());
        if (map_failed_iter != m_pe_failed_map.end()) {
            // pe failed map erase
            // delete PEWrapper::PEWrapperPtr in pe map
            m_pe_failed_map.erase(map_failed_iter);
        }
    }
}

void PEWrapperManager::DestoryAllPEWrapper() {
    // mark manager to prevent from creating new pe
    m_is_destory = true;
    {
        RWLockGuard lock_guard(m_pe_wrapper_map_lock, true);
        PEWrapperMap::const_iterator it;
        for (it = m_pe_wrapper_map.begin(); it != m_pe_wrapper_map.end(); ++it) {
            assert(it->second->StartGCThread(it->second->peid().id()) == error::OK);
        }
        m_pe_wrapper_map.clear();
        m_pe_failed_map.clear();
    }
}

PEWrapperManager::wPEWrapperManagerPtr PEWrapperManager::m_instance;

PEWrapperManager::PEWrapperManagerPtr PEWrapperManager::GetPEWrapperManager() {
    PEWrapperManagerPtr ins = m_instance.lock();
    if (!ins) {
        ins.reset(new (std::nothrow) PEWrapperManager);
        m_instance = ins;
    }
    return ins;
}

error::Code PEWrapperManager::Dump(std::string* output) {
    PEWrapperMap pe_wrapper_map = GetPEWrapperMap();
    PEWrapperMap pe_failed_map = GetFailPEWrapperMap();
    PEWrapperMapPB pe_wrapper_map_pb;
    PEWrapperMap::const_iterator it;
    for (it = pe_wrapper_map.begin(); it != pe_wrapper_map.end(); ++it) {
        PEWrapperKV* pe_wrapper_kv = pe_wrapper_map_pb.add_normal_pe_kv();
        pe_wrapper_kv->set_pe_id(it->second->peid().id());
        pe_wrapper_kv->set_type(it->second->status());
        pe_wrapper_kv->set_assign_time(it->second->last_assign_time());
        pe_wrapper_kv->set_app_name(it->second->app_name());
        pe_wrapper_kv->set_revision(it->second->revision());
    }
    for (it = pe_failed_map.begin(); it != pe_failed_map.end(); ++it) {
        PEWrapperKV* pe_wrapper_kv = pe_wrapper_map_pb.add_fail_pe_kv();
        pe_wrapper_kv->set_pe_id(it->second->peid().id());
        pe_wrapper_kv->set_type(it->second->status());
        pe_wrapper_kv->set_assign_time(it->second->last_assign_time());
        pe_wrapper_kv->set_app_name(it->second->app_name());
        pe_wrapper_kv->set_revision(it->second->revision());
    }
    // dump pb to json
    std::string json_str = "{\"pe_wrapper_mgr\":";
    std::string tmp;
    ProtoMessageToJson(pe_wrapper_map_pb, &tmp, NULL);
    boost::algorithm::trim_if(tmp, boost::is_any_of("\n\t "));
    json_str += tmp;
    json_str += "}";
    output->assign(json_str); 
    return dstream::error::OK;
}

} // namespace processnode
} // namespace dstream

