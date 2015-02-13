/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    client operation
*********************************************************************/
#ifndef __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_CLIENT_OP_H_
#define __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_CLIENT_OP_H_

#include "common/application_tools.h"
#include "common/hdfs_client.h"
#include "common/rpc_client.h"
#include "gtest/gtest.h"
#include "common/config_const.h"
#include "metamanager/zk_config.h"
#include "metamanager/zk_meta_manager.h"
#include "processmaster/mock_pn.h"
#include "processmaster/process_master.h"

namespace dstream {
namespace client_op {

using namespace mock_pn;
using namespace rpc_client;
using namespace zk_meta_manager;
using process_master::ProcessMaster;
typedef auto_ptr::AutoPtr<ProcessMaster> PM_PTR;
typedef UpdateTopology_Operation UTO;

extern const char* kBigpipePath;
extern const char* kHdfsPath;
extern const char* kUser;
extern const char* kWrongVersion;
extern const char* kNoVersion;
extern const char* kRightVersion;
extern const char* kLessEqualVersion;
extern const char* kGreatEqualVersion;
//Init Metamanager
void InitMeta(const char* /*meta_manager_name*/, ClusterConfig* old_config = NULL);

//Get PM Uri
std::string GetPMUri();

void LogIn(std::string* res, uint64_t* session_id, ZKMetaManager* zk_meta);
void CheckApp(const AppID& app);


//connect to pm and log in
void LogIn(const char* version, const char* user,
           const char* passwd, std::string& res, uint64_t& session_id,
           ZKMetaManager* zk_meta = NULL);

void AskForAppID(AppID& app_id, ZKMetaManager* zk_meta = NULL);
//void NotifyPM(uint64_t session_id, const std::string& status);

//Submit App
Application SubmitApp(bool success = true, int process_num = 2,
                      int base = 2, uint64_t memory = 1024 * 1024 * 1024);

//UpdateTopology request for app
UpdateTopology UpdateTopoRequestTest(const Application& app, UTO operation);
void UpdateTopoOverTest(UpdateTopology& ut, UTO op);

//Add SubTree
void AddSubTreeTest(const Application& app);
UpdateTopology AddSubTreeRequestTest(const Application& add_app);

//Del SubTree
void DelSubTreeTest(const Application& del_app);
UpdateTopology DelSubTreeRequestTest(const Application& del_app);

//Flow Control
void FlowControlApp(const AppID& app_id,
        FlowControlRequest::Operation op, const std::string& processor, int32_t limit);

void BlackListOperation(const BlackListOperationRequest* request,
                        BlackListOperationResponse* response);

//Update Para
void UpdateParaTest(const Application& update_app);
UpdateTopology UpdateParaRequestTest(const Application& update_app);

//kill app
void KillApp(const AppID& app_id, ZKMetaManager* zk_meta);

void AddApp(Application& app_id);

//clear app used for clean test context
void ClearApp();

//Add Processor for app, pp is processor's paralism
void AddProcessor(Application& app, int& pp, int& backup_num);
//Remove Processor for app, pp is processor's paralism
void RemoveProcessor(Application& app, int& pp, int& backup_num);

//wait for pm event over
void WaitForPMEventOver(ProcessMaster& pm);

} // namespace test_client_op
} // namespace dstream
#endif // __DSTREAM_CC_PROCESSMASTER_DSTREAM_TEST_TEST_CLIENT_OP_H_
