/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/14
* @filename:    pm_rpc_server.h
* @author:      zhanggengxin
* @brief:       pm rpc server
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_PM_RPC_SERVER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_PM_RPC_SERVER_H__ // NOLINT

#include <string>
#include "common/application_tools.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/rpc_client.h"
#include "common/rpc_const.h"
#include "common/rpc_server.h"
#include "processmaster/event_manager.h"
#include "processmaster/gc.h"
#include "processmaster/zk_watcher.h"

#define ERROR_CODE dstream::error::Code
namespace dstream {
namespace pm_rpc_server {

/************************************************************************/
/* pm rpc server, handle pn report and client request                   */
/************************************************************************/
class PMRPCServer : public rpc_const::PNReportProtocol,
    public rpc_const::LoginProtocol,
    public rpc_const::ApplyAppIDProtocol,
    public rpc_const::SubmitAppProtocol,
    public rpc_const::UpdateTopologyProtocol,
    public rpc_const::AddNewTopoProtocol,
    public rpc_const::NotifyPMProtocol,
    public rpc_const::KillAppProtocol,
    public rpc_const::KillPEProtocol,
    public rpc_const::BlackListOperationProtocol,
    public rpc_const::GetMetricProtocol,
    public rpc_const::KillPNProtocol,
    public rpc_const::FlowControlProtocol,
    public rpc_const::UpdateProcessorProtocol,
    public rpc_const::UpdateProDoneProtocol,
    public rpc_const::UserOpProtocol,
    public rpc_server::RPCServer {
public:
    PMRPCServer() : m_max_session_id(0) {}
    bool Init(auto_ptr::AutoPtr<event_manager::EventManager> event_manager,
              auto_ptr::AutoPtr<zk_watcher::DStreamWatcher> watcher,
              auto_ptr::AutoPtr<pm_gc::GC>gc,
              auto_ptr::AutoPtr<scheduler::Scheduler> scheduler);
    std::string PNREPORT(const PNReport* pn_report, ReportResponse* response);
    std::string LOGIN(const User* request_user, LoginAck* response);
    std::string ASKFORAPPID(const AppID* app_id, AppID* response_app_id);
    std::string SUBMITAPP(const Application* app, FileConfig* file_response);
    std::string UPDATETOPOLOGY(const UpdateTopology* update, UpdateTopology* res_app);
    std::string ADDNEWTOPO(const Application* add_new_pe, rpc_const::NotifyAck* add_new_notify);
    std::string FILEUPLOADDONE(const NotifyPM* notify, rpc_const::NotifyAck* response);
    std::string KILLAPP(const Application* app, AppID* response);
    std::string KILLPE(const KillPE* kill_pe, rpc_const::KillPEAck* response);
    std::string BLACKLISTOPERATION(const BlackListOperationRequest* request, BlackListOperationResponse *response);
    std::string GETMETRIC(const StatusRequest* request, StatusResponse* response);
    std::string KILLPN(const PNID* kill_pn, rpc_const::KillPNAck* response);
    std::string USEROP(const UserOperation* userop, UserOperationReply* response);
    std::string FLOWCONTROL(const FlowControlRequest* request, FlowControlResponse* response);
    std::string UPDATEPROCESSOR(const UpdateProRequest* request, UpdateProResponse* response);
    std::string UPDATEPRODONE(const UpdateProDone* request, rpc_const::UpdateProAck* response);

private:
    auto_ptr::AutoPtr<event_manager::EventManager> m_event_manager;
    auto_ptr::AutoPtr<zk_watcher::DStreamWatcher> m_watcher;
    auto_ptr::AutoPtr<scheduler::Scheduler> m_scheduler;
    auto_ptr::AutoPtr<pm_gc::GC> m_gc;
    MutexLock m_lock;
    uint64_t m_max_session_id;
};

} // namespace pm_rpc_server
} // namespace dstream

#endif // NOLINT
