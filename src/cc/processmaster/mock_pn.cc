/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    mock pn
*********************************************************************/

#include "processmaster/mock_pn.h"

#include "common/rpc_client.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace mock_pn {

using namespace meta_manager;
using namespace rpc_const;
using namespace rpc_server;

int MockPN::serial_num = 0;

MockPN::MockPN(uint64_t mem_basic, float cpu_basic) :
    m_mem_basic(mem_basic), m_cpu_basic(cpu_basic), m_rpc_success(true) {
}

MockPN::~MockPN() {
    KillPN();
}

std::string MockPN::CREATEPE(const CreatePEMessage* request,
                             CreatePEResult* response) {
    if (!m_rpc_success) {
        return kFail;
    }
    if (m_fail_app_id.has_id() &&
        m_fail_app_id.id() == request->app_id().id()) {
        return kFail;
    }
    PNPEStatus pnpe_status;
    pnpe_status.m_app_id = request->app_id();
    pnpe_status.m_pe_id = request->pe_id();
    pnpe_status.m_backup_pe_id = request->backup_id();
    if (m_status.find(pnpe_status) == m_status.end()) {
        m_status.insert(pnpe_status);
    }
    *(response->mutable_id()) = m_pn.pn_id();
    *(response->mutable_app_id()) = request->app_id();
    *(response->mutable_pe_id()) = request->pe_id();
    *(response->mutable_backup_id()) = request->backup_id();
    response->set_res(0);
    return rpc_const::kOK;
}

std::string MockPN::PEOPERATION(const PEOperationMessage* request,
                                PEOperationResult* response) {
    if (!m_rpc_success) {
        return kFail;
    }
    *(response->mutable_id()) = m_pn.pn_id();
    *(response->mutable_app_id()) = request->app_id();
    *(response->mutable_pe_id()) = request->pe_id();
    *(response->mutable_backup_id()) = request->backup_pe_id();
    response->set_res(0);
    PNPEStatus pnpe_status;
    pnpe_status.m_app_id = request->app_id();
    pnpe_status.m_pe_id = request->pe_id();
    pnpe_status.m_backup_pe_id = request->backup_pe_id();
    if (m_status.find(pnpe_status) != m_status.end()) {
        if (request->type() == PEOperationMessage_OperationType_KILL) {
            KillPE(request->app_id(), request->pe_id(),
                   request->backup_pe_id(), false);
        }
    } else {
        if (request->type() == PEOperationMessage_OperationType_QUERY) {
            response->set_res(1);
        }
    }

    return rpc_const::kOK;
}

void MockPN::KillPN() {
    ClearStatus();
    MetaManager* meta = MetaManager::GetMetaManager();
    if (0 != meta) {
        meta->DeletePN(m_pn);
    }
    StopServer();
}

void MockPN::ClearStatus() {
    m_fail_pes.clear();
    m_status.clear();
}

bool MockPN::StartPN(const std::string pn_id_prefix) {
    if (RegisterProtocol(kCreatePEMethod, dynamic_cast<CreatePEProtocol*>(this)) && // NOLINT
        RegisterProtocol(kPEOperationMethod, dynamic_cast<PEOperationProtocol*>(this)) && // NOLINT
        StartServer()) {
        MetaManager* meta = MetaManager::GetMetaManager();
        char pn_id_name[100];
        m_pn.set_host(server_ip().c_str());
        m_pn.set_rpc_server_port(server_port());
        PNID* cur_pn_id = m_pn.mutable_pn_id();
        timeval time;
        gettimeofday(&time, NULL);
        if (pn_id_prefix == "") {
            snprintf(pn_id_name, sizeof(pn_id_name), "%s_%ld_%d",
                     server_ip().c_str(), time.tv_usec, serial_num++);
        } else {
            snprintf(pn_id_name, sizeof(pn_id_name), "%s_%ld_%d",
                     pn_id_prefix.c_str(), time.tv_usec, serial_num++);
        }
        cur_pn_id->set_id(pn_id_name);
        if (meta->AddPN(m_pn) == error::OK) {
            AddPNMachine();
        }
    }
    DSTREAM_WARN("register and start server fail");
    return false;
}

void MockPN::KillPE(const AppID& app_id, const PEID& pe_id,
                    const BackupPEID& backup_pe_id, bool add_fail /*= true*/) {
    PNPEStatus pnpe_status;
    pnpe_status.m_app_id = app_id;
    pnpe_status.m_pe_id = pe_id;
    pnpe_status.m_backup_pe_id = backup_pe_id;
    StatusSetIter status_find = m_status.find(pnpe_status);
    if (status_find != m_status.end()) {
        if (!add_fail) {
            m_status.erase(status_find);
            return;
        }
        FailPE fail_pe;
        FailPeListIter fail_pe_iter = m_fail_pes.insert(m_fail_pes.end(), fail_pe);
        *(fail_pe_iter->mutable_app_id()) = app_id;
        *(fail_pe_iter->mutable_pe_id()) = pe_id;
        *(fail_pe_iter->mutable_backup_pe_id()) = backup_pe_id;
        m_status.erase(status_find);
    }
}

void MockPN::ReportStatus() {
    PNReport report;
    *(report.mutable_id()) = m_pn.pn_id();
    PNResourceStatus* resource_status = report.mutable_resource();
    Resource* total_resource = resource_status->mutable_total_resource();
    uint64_t memory_value = m_mem_basic;
    float cpu_value = m_cpu_basic;
    for (int i = 0; i < 3; ++i) {
        memory_value *= static_cast<uint64_t>(1024);
    }
    total_resource->set_memory(memory_value);
    total_resource->set_cpu(cpu_value);
    for (StatusSetIter status_iter = m_status.begin();
         status_iter != m_status.end();
         ++status_iter) {
        BackupPEStatus* add_status = report.add_status();
        *(add_status->mutable_app_id()) = status_iter->m_app_id;
        *(add_status->mutable_pe_id()) = status_iter->m_pe_id;
        *(add_status->mutable_backup_pd_id()) = status_iter->m_backup_pe_id;
    }
    for (FailPeListIter fail_iter = m_fail_pes.begin();
         fail_iter != m_fail_pes.end();
         ++fail_iter) {
        FailPE* fail_pe = report.add_fails();
        *(fail_pe->mutable_app_id()) = fail_iter->app_id();
        *(fail_pe->mutable_pe_id()) = fail_iter->pe_id();
        *(fail_pe->mutable_backup_pe_id()) = fail_iter->backup_pe_id();
    }
    MetaManager* meta = MetaManager::GetMetaManager();
    PM pm;
    if (meta->GetPM(&pm) == error::OK) {
        ReportResponse response;
        DSTREAM_INFO("pn [%s] report status total memory [%lu] total cpu [%lf]",
                     m_pn.pn_id().id().c_str(),
                     m_mem_basic * 1024 * 1024 * 1024, m_cpu_basic);
        rpc_client::rpc_call(rpc_const::GetUri(pm.host(), pm.report_port()).c_str(),
                             rpc_const::kPNReport, report, response, NULL, 1000 * 60 * 10);
    } else {
        DSTREAM_WARN("get pm fail");
    }
    m_fail_pes.clear();
}

void MockPN::AddBackupPE(const AppID& app_id,
                         const PEID& pe_id,
                         const BackupPEID& be_id) {
    PNPEStatus pnpe_status;
    pnpe_status.m_app_id = app_id;
    pnpe_status.m_pe_id = pe_id;
    pnpe_status.m_backup_pe_id = be_id;
    m_status.insert(pnpe_status);
}

void MockPN::AddPNMachine() {
    MetaManager* meta = MetaManager::GetMetaManager();
    assert(NULL != meta);
    PNMachines pn_machines;
    if (meta->GetPNMachines(&pn_machines) == error::OK) {
        DSTREAM_DEBUG("AddPNMachine");
        pn_machines.add_pn_list(m_pn.pn_id().id());
        meta->UpdatePNMachines(pn_machines);
    } else {
        DSTREAM_DEBUG("AddPNMachine");
        pn_machines.add_pn_list(m_pn.pn_id().id());
        meta->UpdatePNMachines(pn_machines);
    }
}

} // namespace mock_pn
} // namespace dstream
