/**
* @brief  mock pm for client unit test
* @author fangjun,fangjun02@baidu.com
* @date   2013-01-29
*/
#ifndef _DSTREAM_CC_CLIENT_TEST_MOCK_PM_H__
#define _DSTREAM_CC_CLIENT_TEST_MOCK_PM_H__

#include "common/dstream_type.h"
#include "common/rpc_const.h"
#include "common/rpc_server.h"

namespace dstream {
namespace client {

enum RPC_FAIL_FLAG {
    ASKFORAPPID_FAIL = 0,
    SUBMITAPP_FAIL,
    FILEUPLOADDONE_FAIL,
    KILLAPP_FAIL,
    USEROP_FAIL,
    UPDATETOPOLOGY_FAIL,
    FLOWCONTROL_FAIL,
    ADD_SUB_GET_FAIL,
    UPDATE_PRO_FAIL,
    UPDATE_PRO_DONE_FAIL,
    INVAILD_TOPO_FAIL,
    UPDATETOPOLOGY_ACK_FAIL
};

class MockPM : public rpc_server::RPCServer,
    public rpc_const::LoginProtocol,
    public rpc_const::ApplyAppIDProtocol,
    public rpc_const::SubmitAppProtocol,
    public rpc_const::NotifyPMProtocol,
    public rpc_const::KillAppProtocol,
    public rpc_const::UpdateTopologyProtocol,
    public rpc_const::UpdateProcessorProtocol,
    public rpc_const::UpdateProDoneProtocol,
/*               public rpc_const::AddNewTopoProtocol,*/
    public rpc_const::FlowControlProtocol,
    public rpc_const::UserOpProtocol {
public:
    MockPM();
    virtual ~MockPM();

    bool StartPM();
    void StopPM();

    std::string LOGIN(const User* request_user, LoginAck* response);
    std::string ASKFORAPPID(const AppID* app_id, AppID* response_app_id);
    std::string SUBMITAPP(const Application* app, FileConfig* file_response);
    std::string FILEUPLOADDONE(const NotifyPM* notify, rpc_const::NotifyAck* response);
    std::string KILLAPP(const Application* app, AppID* response);
    std::string USEROP(const UserOperation* userop, UserOperationReply* response);
    std::string UPDATETOPOLOGY(const UpdateTopology* update, UpdateTopology* res_app);
    std::string FLOWCONTROL(const FlowControlRequest* request, FlowControlResponse* response);
    std::string UPDATEPROCESSOR(const UpdateProRequest* request, UpdateProResponse* response);
    std::string UPDATEPRODONE(const UpdateProDone* request, rpc_const::UpdateProAck* response);

    void set_host(const std::string& ip) {
        m_pm.set_host(ip);
    }
    void set_report_port(const uint32_t& port) {
        m_pm.set_report_port(port);
    }
    void set_submit_port(const uint32_t& port) {
        m_pm.set_submit_port(port);
    }
    void set_rpc_fail(RPC_FAIL_FLAG flag) {
        rpc_flag[flag] = -1;
    }
    void set_rpc_succ(RPC_FAIL_FLAG flag) {
        rpc_flag[flag] = 0;
    }

    void DumpRPCFail();
private:
    static const int rpc_flag_num = 10;
    int rpc_flag[rpc_flag_num];
    PM m_pm;
};

}//namespace client
}//namespace dstream
#endif
