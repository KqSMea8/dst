/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    mock pn
*********************************************************************/

#ifndef __DSTREAM_PROCESSMASTER_MOCK_PN_H__ // NOLINT
#define __DSTREAM_PROCESSMASTER_MOCK_PN_H__ // NOLINT

#include <list>
#include <set>
#include <string>
#include "common/dstream_type.h"
#include "common/rpc_const.h"
#include "common/rpc_server.h"

namespace dstream {
namespace mock_pn {

/************************************************************************/
/* pn status used to report to pm                                       */
/************************************************************************/
struct PNPEStatus {
    AppID m_app_id;
    PEID m_pe_id;
    BackupPEID m_backup_pe_id;
};

class ComparePNPEStatus {
public:
    bool operator()(const PNPEStatus& status1, const PNPEStatus& status2) {
        return (status1.m_app_id.id() < status2.m_app_id.id()
                || status1.m_pe_id.id() < status2.m_pe_id.id()
                || status1.m_backup_pe_id.id() < status2.m_backup_pe_id.id());
    }
};

typedef std::set<PNPEStatus, ComparePNPEStatus> StatusSet;
typedef StatusSet::iterator StatusSetIter;
typedef std::list<FailPE> FailPeList;
typedef FailPeList::iterator FailPeListIter;
/************************************************************************/
/* mock pn class : receive pn request                                   */
/************************************************************************/
class MockPN : public rpc_const::CreatePEProtocol,
    public rpc_const::PEOperationProtocol,
    public rpc_server::RPCServer {
public:
    MockPN(uint64_t mem_basic = 32, float cpu_basic = 16);
    ~MockPN();
    std::string CREATEPE(const CreatePEMessage* request, CreatePEResult* response);
    std::string PEOPERATION(const PEOperationMessage* request,
                            PEOperationResult* response);
    void KillPN();
    void ReportStatus();
    void ClearStatus();
    bool StartPN(std::string pn_id_prefix = "");
    PNID pn_id() {
        return m_pn.pn_id();
    }
    PN pn() {
        return m_pn;
    }
    void KillPE(const AppID& app_id, const PEID& pe_id,
                const BackupPEID& backup_pe_id, bool add_fail = true);
    const StatusSet& status() const {
        return m_status;
    }
    void AddBackupPE(const AppID& app_id, const PEID& pe_id, const BackupPEID& be_id);
    void set_mem_basic(uint64_t mem_basic) {
        m_mem_basic = mem_basic;
    }
    void set_cpu_basic(float cpu_basic) {
        m_cpu_basic = cpu_basic;
    }
    void AddPNMachine();
    void set_rpc_success(bool rpc_success) {
        m_rpc_success = rpc_success;
    }
    void SetFailAppID(const AppID& app_id) {
        m_fail_app_id = app_id;
    }
private:
    StatusSet m_status;
    FailPeList m_fail_pes;
    PN m_pn;
    // PNResourceStatus m_resource_status;
    static int serial_num;
    uint64_t m_mem_basic;
    float m_cpu_basic;
    bool m_rpc_success;
    AppID m_fail_app_id;
};

} // namespace mock_pn
} // namespace dstream

#endif // NOLINT 
