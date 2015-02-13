/**
* @file   pm_client.h
* @brief    pm client class
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/


#ifndef __DSTREAM_CC_CLIENT_PM_CLIENT_H__ //NOLINT
#define __DSTREAM_CC_CLIENT_PM_CLIENT_H__ //NOLINT

#include <set>
#include <string>
#include "common/application_tools.h"
#include "common/hdfs_client.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/rpc.h"

namespace dstream {
namespace client {

/**
* @brief    pm client
* @author   konghui, konghui@baidu.com
* @date   2011-12-15
*/
class PMClient {
public:
    static const uint32_t kPMReconnectIntervalSecs = 1;
    static const uint32_t kPMReconnectTimes        = static_cast<uint32_t>(-1);
    static const uint32_t kMaxFlowControlInfoLen   = 256;
    /**< retry time, -1 means loop forever */

    PMClient(): m_login(false),
        m_reconnection_interval_seconds(kPMReconnectIntervalSecs),
        m_reconnection_times(kPMReconnectTimes),
        m_hdfs_client(NULL)
    {}
    virtual ~PMClient() {
        if (NULL != m_hdfs_client) {
            delete m_hdfs_client;
            m_hdfs_client = NULL;
        }
    }
    void Reset() {
        m_pm.Clear();
        m_connection.Close();
        m_user.Clear();
        m_login = false;
    }

    int32_t Init();
    void set_reconnection_interval_seconds(uint32_t s) {
        m_reconnection_interval_seconds = s;
    }
    void set_reconnection_times(uint32_t t) {
        m_reconnection_times = t;
    }
    int32_t Connect(const std::string& zk_svr_list, User* user);
    int32_t LoginPM(std::string zk_svr_list, User* user);
    virtual int32_t SubmitApp(Application* app);
    virtual int32_t AddSubTree(const Application& sub_tree);
    virtual int32_t DelSubTree(const Application& sub_tree);
    virtual int32_t AddImporter(const Application& importer);
    virtual int32_t DelImporter(const Application& importer);
    virtual int32_t UpdateParallelism(const Application& update);
    virtual int32_t DecommissionApp(const std::string name);
    virtual int32_t DecommissionApp(const int64_t app_id);
    virtual int32_t FlowControlApp(const int64_t app_id,
            const std::string &processor, const std::string& cmd);
    virtual int32_t FlowControlApp(const std::string name,
            const std::string &processor, const std::string& cmd);

    int32_t SetFlowControlRequest(const std::string& cmd,
            const std::string& processor, FlowControlRequest* request);
    void    SetFlowControlResult(const FlowControlResponse *res,
            const std::string& cmd, const std::string& name, const std::string& processor);
    std::string FlowControlInfo();
    virtual int32_t UpdateProcessor(const Application& update);

    // deprecated
    virtual int32_t GetAppTopology(const std::string& app_name, Application& app);
    // int32_t MigratePE(const std::string& pe, const std::string& target_pn);
    // int32_t KillPElement(const AppID app_id, const PEID pe_id,
    //                      const BackupPEID backup_pe_id, const PNID pn_id);
    // int32_t DecommissionPn(const PNID pn_id);

    // User Operation
    virtual std::set<User, application_tools::CompareUser> GetUser();
    virtual int32_t UpdateUser(const std::string username, const std::string password);
    virtual int32_t AddUser(const std::string username, const std::string password);
    virtual int32_t DeleteUser(const std::string username);

    int32_t UserOp(const UserOperation userop);

private:
    int32_t GetPmAddrFromZK(const char* zk_svr_list);

    PM                m_pm;
    rpc::Connection   m_connection;
    User              m_user;
    bool              m_login;
    uint32_t          m_reconnection_interval_seconds;
    uint32_t          m_reconnection_times;
    hdfs_client::HdfsClient*       m_hdfs_client;

    std::set<User, application_tools::CompareUser> user;
    char              flow_control_info[kMaxFlowControlInfoLen];
};

} // namespace client
} // namespace dstream
#endif // __DSTREAM_CC_CLIENT_PM_CLIENT_H__ NOLINT
