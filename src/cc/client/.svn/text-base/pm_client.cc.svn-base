/**
* @file   pm_client.cc
* @brief    pm client class
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <boost/algorithm/string.hpp>
#include "client/application.h"
#include "client/client_config.h"
#include "client/dstream_client.h"
#include "client/pm_client.h"
#include "client/zk.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/hdfs_client.h"
#include "common/logger.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_client_rpc.pb.h"
#include "common/proto_rpc.h"
#include "common/rpc.h"
#include "common/rpc_const.h"
#include "common/zk_client.h"
#include "metamanager/zk_config.h"

namespace dstream {
namespace client {
using namespace hdfs_client;

#define CHECK_LOGIN() \
    do { \
        if (!m_login) { \
            DSTREAM_INFO("not loggin yet"); \
            return error::CLIENT_NOT_LOGIN_YET; \
        } \
    } while (0)

int32_t PMClient::Init() {
    int32_t hdfs_retry;
    g_cfg_map.GetIntValue(config_const::kHDFSRetryName, &hdfs_retry);
    m_hdfs_client = new (std::nothrow) HdfsClient(
                                        g_cfg_map.GetValue(config_const::kHDFSClientDirName),
                                        hdfs_retry);
    if (NULL == m_hdfs_client) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    return error::OK;
}

int32_t PMClient::Connect(const std::string& zk_svr_list, User* dstream_user) {
    if (m_login) {
        DSTREAM_INFO("already loggin, will not loggin again");
        return error::OK;
    }

    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* get pm add from zookeeper */
    ret = g_zk->GetPM(m_reconnection_times, m_reconnection_interval_seconds, &m_pm);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to get pm from zookeeper[%s]", zk_svr_list.c_str());
        return ret;
    }

    std::string pm_uri = rpc_const::GetUri(m_pm.host(), m_pm.submit_port());

    rpc::Context::init();
    if (m_connection.Connect(pm_uri.c_str()) < 0) {
        DSTREAM_WARN("fail to connect pm:[%s]", pm_uri.c_str());
        return error::COMM_CONNECT_FAIL;
    }

    /* loging */
    proto_rpc::RPCRequest<User> req;
    proto_rpc::RPCResponse<LoginAck> res;
    req.set_method(rpc_const::kLoginMethod);
    req.set_username(dstream_user->username());
    req.set_password(dstream_user->password());
    req.set_hostname(dstream_user->hostname());
    req.set_hostip(dstream_user->hostip());
    req.set_version(dstream_user->version());
    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        res.user().session_id() <= 0) {
        if (LoginAck_ErrorCode_VERSION_MISSMATCH == res.code()) {
            DSTREAM_WARN("unsupported client version, client vesion:[%s], pm version:[%s], "
                         "please get corresponding Dstream product(%s) form scm.baidu.com",
                         dstream_user->version().c_str(),
                         res.user().version().c_str(),
                         res.user().version().c_str());
        } else if (LoginAck_ErrorCode_WRONG_USER == res.code()) {
            DSTREAM_WARN("loggin into pm:[%s] with wrong username or passwd, session_id:[%ld], "
                         "client vesion:[%s], pm version:[%s]",
                         pm_uri.c_str(), res.user().session_id(),
                         dstream_user->version().c_str(),
                         res.user().has_version() ?
                         res.user().version().c_str() : config_const::kUnknownPMVersion);
        }
        return error::AUTH_FAIL;
    }

    m_user.CopyFrom(*dstream_user);
    m_user.set_session_id(res.user().session_id());
    m_login = true;

    DSTREAM_DEBUG("loggin into pm:[%s], session_id:[%ld], "
                  "client version:[%s], pm version:[%s] ...OK",
                  pm_uri.c_str(), m_user.session_id(),
                  dstream_user->version().c_str(),
                  res.user().version().c_str());

    return error::OK;
}

int32_t PMClient::LoginPM(std::string zk_svr_list, User* dstream_user) {
    if (dstream_user == NULL) {
        DSTREAM_WARN("Bad argument, PMClient::LoginPM does not take null value pointer");
        return error::BAD_ARGUMENT;
    }
    /* connect to pm */
    int32_t ret = Connect(zk_svr_list, dstream_user);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to connect to pm");
        return ret;
    }
    return error::OK;
}

int32_t PMClient::SubmitApp(Application* app ) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    app->set_session_id(m_user.session_id());

    /* apply app id from pm */
    if ( -1 == g_app_id ) {
        proto_rpc::RPCRequest<AppID>  req_id;
        proto_rpc::RPCResponse<AppID> res_id;
        req_id.set_method(rpc_const::kApplyAppIDMethod);
        req_id.set_session_id(m_user.session_id());
        if (m_connection.Call(&req_id, &res_id, pm_timeout_sec) < 0 ||
            !rpc_const::IsReturnValueOK(res_id.return_value()) ) {
            DSTREAM_WARN("fail to apply app id from pm, ret value:[%s]",
                         res_id.return_value().c_str());
            return error::CLIENT_APPLY_APP_ID_FAIL;
        }
        app->mutable_id()->set_id(res_id.id());
        DSTREAM_DEBUG("apply app id:[%ld], session_id:[%ld] from pm ...OK",
                      app->id().id(), app->session_id());
        g_app_id = app->id().id();
    }

    /* submit topology and config to pm */
    proto_rpc::RPCRequest<Application>  req_app;
    proto_rpc::RPCResponse<FileConfig>  res_app;
    req_app.CopyFrom(*app);
    req_app.set_method(rpc_const::kSubmitAppMethod);
    if (m_connection.Call(&req_app, &res_app, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res_app.return_value()) ) {
        DSTREAM_WARN("fail to submit app to pm");
        return error::CLIENT_SUBMIT_APP_FAIL;
    }
    /* wait pm return hdfs env for app data, and tranfer app data to hdfs */
    DSTREAM_DEBUG("Receive path:[%s] from PM, prepare to upload to ftp server",
                  res_app.path().c_str());

    /* upload app to filesystem */
    std::string fs_path_prefix = res_app.path();
    UNLIKELY_IF(0 != fs_path_prefix.find("hdfs://")) { /* local fs */
        // local fs
    } else {/* hdfs */
        ret = m_hdfs_client->TestAndRemoveDir(res_app.path());
        if (ret < error::OK) {
            DSTREAM_WARN("fail to remove dir from hdfs[%s]",
                         res_app.path().c_str());
            return ret;
        }

        // list all file in app_dir
        // for each file in app_dir, upload
        VectorString to_be_upload;
        VectorStringIterator it;
        if ((ret = GenerateUploadFile(g_cmd_line_args.app_dir, &to_be_upload)) < error::OK) {
            DSTREAM_WARN("empty directory");
            return ret;
        }
        for (it = to_be_upload.begin(); it != to_be_upload.end(); it++) {
            std::string local_file = g_cmd_line_args.app_dir + "/" + *it;
            std::string remote_file = res_app.path();
            remote_file.append("/");
            remote_file.append(*it);
            if (IsDir(local_file.c_str())) {
                local_file.append("/*");
                remote_file.append("_v0");
            }

            DSTREAM_INFO("upload file %s to %s", local_file.c_str(), remote_file.c_str());
            ret = m_hdfs_client->UploadFile(local_file, remote_file, true);
            if (ret < error::OK) {
                DSTREAM_WARN("fail to upload app dir:[%s] to hdfs[%s]",
                             it->c_str(), res_app.path().c_str());
                return ret;
            }
        }
    }

    /* notify pm that we are ok */
    proto_rpc::RPCRequest<NotifyPM>     req_notify;
    proto_rpc::RPCResponse<rpc_const::NotifyAck>   res_notify;
    req_notify.set_session_id(m_user.session_id());
    req_notify.set_status(rpc_const::kOK);
    req_notify.set_method(rpc_const::kNotifyPMMethod);
    if (m_connection.Call(&req_notify, &res_notify, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res_notify.return_value()) ) {
        DSTREAM_WARN("fail to notify pm");
        return error::CLIENT_NOTIFY_PM_FAIL;
    }

    return error::OK;
}

int32_t PMClient::DecommissionApp(const std::string name) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    proto_rpc::RPCRequest<Application>  req;
    proto_rpc::RPCResponse<AppID>       res;
    req.set_method(rpc_const::kKillAppMethod);
    req.set_session_id(m_user.session_id());
    req.set_name(name);
    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to decommission app:[%s] from pm", name.c_str());
        g_app_id = res.id();
        return error::CLIENT_DECOMMISSION_APP_FAIL;
    }
    g_app_id = res.id();
    DSTREAM_INFO("decommission app[%s], id[%lu] ...OK", name.c_str(), res.id());

    return ret;
}
int32_t PMClient::DecommissionApp(const int64_t id) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    proto_rpc::RPCRequest<Application>  req;
    proto_rpc::RPCResponse<AppID>       res;
    req.set_method(rpc_const::kKillAppMethod);
    req.set_session_id(m_user.session_id());
    req.mutable_id()->set_id(id);
    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to decommission app:[%lu] to pm", id);
        g_app_id = res.id();
        return error::CLIENT_DECOMMISSION_APP_FAIL;
    }
    g_app_id = res.id();
    DSTREAM_INFO("decommission app id[%lu] ...OK", res.id());

    return ret;
}

int32_t PMClient::FlowControlApp(const std::string name,
        const std::string &processor, const std::string& cmd_string) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    proto_rpc::RPCRequest<FlowControlRequest>     req;
    proto_rpc::RPCResponse<FlowControlResponse>   res;
    req.set_method(rpc_const::kFlowControlMethod);
    req.set_name(name);
    req.set_session_id(m_user.session_id());
    // set operation
    if (SetFlowControlRequest(cmd_string, processor, &req) < error::OK) {
        DSTREAM_WARN("fail to set flow control request of app:[%s] to pm", name.c_str());
        return error::CLIENT_FLOW_CONTROL_FAIL;
    }

    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to flow control app:[%s] to pm", name.c_str());
        return error::CLIENT_FLOW_CONTROL_FAIL;
    }

    SetFlowControlResult(&res, cmd_string, name, processor);
    return ret;
}

int32_t PMClient::FlowControlApp(const int64_t id,
        const std::string &processor, const std::string& cmd_string) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    proto_rpc::RPCRequest<FlowControlRequest>     req;
    proto_rpc::RPCResponse<FlowControlResponse>   res;
    req.set_method(rpc_const::kFlowControlMethod);
    req.mutable_app_id()->set_id(id);
    req.set_session_id(m_user.session_id());
    // set operation
    if (SetFlowControlRequest(cmd_string, processor, &req) < error::OK) {
        DSTREAM_WARN("fail to set flow control request of app:[%lu] to pm", id);
        return error::CLIENT_FLOW_CONTROL_FAIL;
    }

    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to flow control app:[%lu] to pm", id);
        return error::CLIENT_FLOW_CONTROL_FAIL;
    }

    std::string app_string;
    StringAppendNumber(&app_string, id);
    SetFlowControlResult(&res, cmd_string, app_string, processor);
    return ret;
}

int32_t PMClient::GetAppTopology(const std::string& app_name, Application& app) {
    int32_t ret = g_zk->GetApp(app_name, m_reconnection_times,
                               m_reconnection_interval_seconds,
                               &app);
    if (ret < error::OK) {
        DSTREAM_DEBUG("fail to get app topo");
        return ret;
    }
    return error::OK;
}

int32_t PMClient::AddSubTree(const Application& sub_tree) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    bool failed = false;
    std::string local_file, remote_file;
    Application* new_tree;
    VectorString to_be_upload;
    VectorStringIterator it;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* send add sub tree request, and get the app topology */
    proto_rpc::RPCRequest<UpdateTopology>     add_sub_tree_req;
    proto_rpc::RPCResponse<UpdateTopology>    add_sub_tree_res;
    add_sub_tree_req.set_session_id(m_user.session_id());
    add_sub_tree_req.mutable_app()->set_name(sub_tree.name());
    add_sub_tree_req.set_type(UpdateTopology::ADD);
    add_sub_tree_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&add_sub_tree_req, &add_sub_tree_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(add_sub_tree_res.return_value()) ) {
        DSTREAM_WARN("fail to send/recv ADD_SUB_TREE rpc to/from pm");
        if (add_sub_tree_res.error_num() == error::APP_NOT_EXIST) {
            DSTREAM_WARN("application [%s] not exists", sub_tree.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    if (!add_sub_tree_res.has_app()) {
        DSTREAM_WARN("fail to get app from pm");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    new_tree = add_sub_tree_res.mutable_app();
    /* check validity for adding sub tree into the app topology,
     * and merge sub tree into app
     */
    ret = MergeSubTreeIntoApp(sub_tree, new_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to merge sub tree into app, errstr:[%s]",
                     error::get_text(ret));
        failed = true;
        goto NOTIFY_PM_FAIL;
    }

    // list all file in app_dir
    // for each file in app_dir, upload
    if ((ret = GenerateUploadFile(g_cmd_line_args.app_dir, &to_be_upload)) < error::OK) {
        DSTREAM_WARN("empty directory");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    for (it = to_be_upload.begin(); it != to_be_upload.end(); it++) {
        local_file = g_cmd_line_args.app_dir + "/" + *it;
        remote_file = new_tree->app_path().path();
        remote_file.append("/");
        remote_file.append(*it);
        if (IsDir(local_file.c_str())) {
            local_file.append("/*");
            remote_file.append("_v0");
        }

        DSTREAM_INFO("upload file %s to %s", local_file.c_str(), remote_file.c_str());
        ret = m_hdfs_client->UploadFile(local_file, remote_file, true);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to upload app dir:[%s] to hdfs[%s]",
                         it->c_str(), new_tree->app_path().path().c_str());
            failed = true;
            goto NOTIFY_PM_FAIL;
        }
    }

NOTIFY_PM_FAIL:
    /* notify pm */
    proto_rpc::RPCRequest<UpdateTopology>     add_sub_tree_ack_req;
    proto_rpc::RPCResponse<UpdateTopology>    add_sub_tree_ack_res;
    add_sub_tree_ack_req.set_session_id(m_user.session_id());
    Application* submit_app = NULL;
    if (failed) {
        add_sub_tree_ack_req.set_type(UpdateTopology::ADD_SUB_FAIL);
    } else {
        add_sub_tree_ack_req.set_type(UpdateTopology::ADD_SUB_OK);
        submit_app = add_sub_tree_ack_req.mutable_app();
        if (NULL == submit_app) {
            DSTREAM_WARN("fail to allocate memory");
            return error::BAD_MEMORY_ALLOCATION;
        }
        *submit_app = *new_tree;
    }
    add_sub_tree_ack_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&add_sub_tree_ack_req, &add_sub_tree_ack_res,
                          pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(add_sub_tree_ack_res.return_value()) ) {
        DSTREAM_WARN("fail to ack [%s] to pm", failed ?
                     "ADD_SUB_FAIL" : "ADD_SUB_OK");
        if (add_sub_tree_ack_res.error_num() == error::SERIAIL_DATA_FAIL) {
            DSTREAM_WARN("fail to update application [%s] on zk", sub_tree.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    return failed ? ret : error::OK;
}

int32_t PMClient::DelSubTree(const Application& sub_tree) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    bool failed = false;
    std::string local_files, remote_files;
    Application* new_tree;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* send delete sub tree request, and get the app topology */
    proto_rpc::RPCRequest<UpdateTopology>     del_sub_tree_req;
    proto_rpc::RPCResponse<UpdateTopology>    del_sub_tree_res;
    del_sub_tree_req.set_session_id(m_user.session_id());
    del_sub_tree_req.mutable_app()->set_name(sub_tree.name());
    del_sub_tree_req.set_type(UpdateTopology::DEL);
    del_sub_tree_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&del_sub_tree_req, &del_sub_tree_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(del_sub_tree_res.return_value()) ) {
        DSTREAM_WARN("fail to send/recv DEL_SUB_TREE rpc to/from pm");
        if (del_sub_tree_res.error_num() == error::APP_NOT_EXIST) {
            DSTREAM_WARN("application [%s] not exists", sub_tree.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    if (!del_sub_tree_res.has_app()) {
        DSTREAM_WARN("fail to get app from pm");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    new_tree = del_sub_tree_res.mutable_app();
    /* check validity for deleting sub tree from the app topology,
     * and delete sub tree from app
     */
    ret = DelSubTreeFromApp(sub_tree, new_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to delete sub tree from app, errstr:[%s]",
                     error::get_text(ret));
        failed = true;
        goto NOTIFY_PM_FAIL;
    }

    /* will not delete sub tree from hdfs, pm will handle this */

NOTIFY_PM_FAIL:
    /* notify pm */
    proto_rpc::RPCRequest<UpdateTopology>     del_sub_tree_ack_req;
    proto_rpc::RPCResponse<UpdateTopology>    del_sub_tree_ack_res;
    del_sub_tree_ack_req.set_session_id(m_user.session_id());
    Application* submit_app = NULL;
    if (failed) {
        del_sub_tree_ack_req.set_type(UpdateTopology::DEL_SUB_FAIL);
    } else {
        del_sub_tree_ack_req.set_type(UpdateTopology::DEL_SUB_OK);
        submit_app = del_sub_tree_ack_req.mutable_app();
        if (NULL == submit_app) {
            DSTREAM_WARN("fail to allocate memory");
            return error::BAD_MEMORY_ALLOCATION;
        }
        *submit_app = *new_tree;
    }
    del_sub_tree_ack_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&del_sub_tree_ack_req, &del_sub_tree_ack_res,
                          pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(del_sub_tree_ack_res.return_value()) ) {
        DSTREAM_WARN("fail to ack [%s] to pm", failed ?
                     "DEL_SUB_FAIL" : "DEL_SUB_OK");
        if (del_sub_tree_ack_res.error_num() == error::SERIAIL_DATA_FAIL) {
            DSTREAM_WARN("fail to update application [%s] on zk", sub_tree.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    return failed ? ret : error::OK;
}

int32_t PMClient::AddImporter(const Application& importer) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    bool failed = false;
    std::string local_file, remote_file;
    VectorString to_be_upload;
    VectorStringIterator it;
    Application* new_tree;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* send add importer request, and get the app topology */
    proto_rpc::RPCRequest<UpdateTopology> add_importer_req;
    proto_rpc::RPCResponse<UpdateTopology> add_importer_res;
    add_importer_req.set_session_id(m_user.session_id());
    add_importer_req.mutable_app()->set_name(importer.name());
    add_importer_req.set_type(UpdateTopology::ADD);
    add_importer_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&add_importer_req, &add_importer_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(add_importer_res.return_value()) ) {
        DSTREAM_WARN("fail to send/recv ADD_IMPORTER rpc to/from pm");
        if (add_importer_res.error_num() == error::APP_NOT_EXIST) {
            DSTREAM_WARN("application [%s] not exists", importer.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    if (!add_importer_res.has_app()) {
        DSTREAM_WARN("fail to get app from pm");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    new_tree = add_importer_res.mutable_app();
    /* check validity for adding sub tree into the app topology,
     * and merge importer into app
     */
    ret = MergeImporterIntoApp(importer, new_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to merge importer into app, errstr:[%s]",
                     error::get_text(ret));
        failed = true;
        goto NOTIFY_PM_FAIL;
    }

    // list all file in app_dir
    // for each file in app_dir, upload
    if ((ret = GenerateUploadFile(g_cmd_line_args.app_dir, &to_be_upload)) < error::OK) {
        DSTREAM_WARN("empty directory");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    for (it = to_be_upload.begin(); it != to_be_upload.end(); it++) {
        local_file = g_cmd_line_args.app_dir + "/" + *it;
        remote_file = new_tree->app_path().path();
        remote_file.append("/");
        remote_file.append(*it);
        if (IsDir(local_file.c_str())) {
            local_file.append("/*");
            remote_file.append("_v0");
        }

        DSTREAM_INFO("upload file %s to %s", local_file.c_str(), remote_file.c_str());
        ret = m_hdfs_client->UploadFile(local_file, remote_file, true);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to upload app dir:[%s] to hdfs[%s]",
                         it->c_str(), new_tree->app_path().path().c_str());
            failed = true;
            goto NOTIFY_PM_FAIL;
        }
    }


NOTIFY_PM_FAIL:
    /* notify pm */
    proto_rpc::RPCRequest<UpdateTopology> add_importer_ack_req;
    proto_rpc::RPCResponse<UpdateTopology> add_importer_ack_res;
    add_importer_ack_req.set_session_id(m_user.session_id());
    Application* submit_app = NULL;
    if (failed) {
        add_importer_ack_req.set_type(UpdateTopology::ADD_SUB_FAIL);
    } else {
        add_importer_ack_req.set_type(UpdateTopology::ADD_SUB_OK);
        submit_app = add_importer_ack_req.mutable_app();
        if (NULL == submit_app) {
            DSTREAM_WARN("fail to allocate memory");
            return error::BAD_MEMORY_ALLOCATION;
        }
        *submit_app = *new_tree;
    }
    add_importer_ack_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&add_importer_ack_req, &add_importer_ack_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(add_importer_ack_res.return_value()) ) {
        DSTREAM_WARN("fail to ack [%s] to pm", failed ?
                     "ADD_IMPORTER_FAIL" : "ADD_IMPORTER_OK");
        if (add_importer_ack_res.error_num() == error::SERIAIL_DATA_FAIL) {
            DSTREAM_WARN("fail to update application [%s] on zk", importer.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    return failed ? ret : error::OK;
}

int32_t PMClient::DelImporter(const Application& importer) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    bool failed = false;
    std::string local_files, remote_files;
    Application* new_tree;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* send delete importer request, and get the app topology */
    proto_rpc::RPCRequest<UpdateTopology> del_importer_req;
    proto_rpc::RPCResponse<UpdateTopology> del_importer_res;
    del_importer_req.set_session_id(m_user.session_id());
    del_importer_req.mutable_app()->set_name(importer.name());
    del_importer_req.set_type(UpdateTopology::DEL);
    del_importer_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&del_importer_req, &del_importer_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(del_importer_res.return_value()) ) {
        DSTREAM_WARN("fail to send/recv DEL_IMP rpc to/from pm");
        if (del_importer_res.error_num() == error::APP_NOT_EXIST) {
            DSTREAM_WARN("application [%s] not exists", importer.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    if (!del_importer_res.has_app()) {
        DSTREAM_WARN("fail to get app from pm");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    new_tree = del_importer_res.mutable_app();
    /* check validity for deleting importer from the app topology,
     * and delete importer from app
     */
    ret = DelImporterFromApp(importer, new_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to delete sub tree from app, errstr:[%s]",
                     error::get_text(ret));
        failed = true;
        goto NOTIFY_PM_FAIL;
    }

    /* will not delete importer from hdfs, pm will handle this*/
NOTIFY_PM_FAIL:
    /* notifu pm */
    proto_rpc::RPCRequest<UpdateTopology> del_importer_ack_req;
    proto_rpc::RPCResponse<UpdateTopology> del_importer_ack_res;
    del_importer_ack_req.set_session_id(m_user.session_id());
    Application* submit_app = NULL;
    if (failed) {
        del_importer_ack_req.set_type(UpdateTopology::DEL_SUB_FAIL);
    } else {
        del_importer_ack_req.set_type(UpdateTopology::DEL_SUB_OK);
        submit_app = del_importer_ack_req.mutable_app();
        if (NULL == submit_app) {
            DSTREAM_WARN("fail to allocate memory");
            return error::BAD_MEMORY_ALLOCATION;
        }
        *submit_app = *new_tree;
    }
    del_importer_ack_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&del_importer_ack_req, &del_importer_ack_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(del_importer_ack_res.return_value()) ) {
        DSTREAM_WARN("fail to ack [%s] to pm", failed ?
                     "DEL_IMPORTER_FAIL" : "DEL_IMPORTER_OK");
        if (del_importer_ack_res.error_num() == error::SERIAIL_DATA_FAIL) {
            DSTREAM_WARN("fail to update application [%s] on zk", importer.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    return failed ? ret : error::OK;
}

int32_t PMClient::UpdateParallelism(const Application& update) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    bool failed = false;
    Application* new_tree;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    /* send update parallelism request, and get the app topology */
    proto_rpc::RPCRequest<UpdateTopology> update_para_req;
    proto_rpc::RPCResponse<UpdateTopology> update_para_res;
    update_para_req.set_session_id(m_user.session_id());
    update_para_req.mutable_app()->set_name(update.name());
    update_para_req.set_type(UpdateTopology::UPDATE_PARA);
    update_para_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&update_para_req, &update_para_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(update_para_res.return_value()) ) {
        DSTREAM_WARN("fail to send.recv UPDATE_PARA rpc to/from pm");
        if (update_para_res.error_num() == error::APP_NOT_EXIST) {
            DSTREAM_WARN("application [%s] not exists", update.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    if (!update_para_res.has_app()) {
        DSTREAM_WARN("fail to get app from pm");
        failed = true;
        goto NOTIFY_PM_FAIL;
    }
    new_tree = update_para_res.mutable_app();
    /* check validity for update parallelism from the app topology,
     * and update parallelism from app
     */
    ret = UpdateParallelismFromApp(update, new_tree);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to update parallelism from app, errstr:[%s]",
                     error::get_text(ret));
        failed = true;
        goto NOTIFY_PM_FAIL;
    }

NOTIFY_PM_FAIL:
    /* notify pm */
    proto_rpc::RPCRequest<UpdateTopology> update_para_ack_req;
    proto_rpc::RPCResponse<UpdateTopology> update_para_ack_res;
    update_para_ack_req.set_session_id(m_user.session_id());
    Application* submit_app = NULL;
    if (failed) {
        update_para_ack_req.set_type(UpdateTopology::UPDATE_PARA_FAIL);
    } else {
        update_para_ack_req.set_type(UpdateTopology::UPDATE_PARA_OK);
        submit_app = update_para_ack_req.mutable_app();
        if (NULL == submit_app) {
            DSTREAM_WARN("fail to allocate memory");
            return error::BAD_MEMORY_ALLOCATION;
        }
        *submit_app = *new_tree;
    }
    update_para_ack_req.set_method(rpc_const::kUpdateTopologyMethod);
    if (m_connection.Call(&update_para_ack_req, &update_para_ack_res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(update_para_ack_res.return_value()) ) {
        DSTREAM_WARN("fail to ack [%s] to pm", failed ?
                     "UPDATE_PARA_FAIL" : "UPDATE_PARAPOK");
        if (update_para_ack_res.error_num() == error::SERIAIL_DATA_FAIL) {
            DSTREAM_WARN("fail to update application [%s] on zk", update.name().c_str());
        }
        return error::RPC_CALL_FAIL;
    }
    return failed ? ret : error::OK;
}

std::set<User, application_tools::CompareUser> PMClient::GetUser() {
    std::set<User, application_tools::CompareUser>userinfo;

    UserOperation userop;
    userop.set_type(UserOperation::GET);

    UserOp(userop);

    return user;
}

int32_t PMClient::UpdateUser(const std::string username, const std::string password) {
    UserOperation userop;
    userop.set_type(UserOperation::UPDATE);
    userop.mutable_user()->set_username(username);
    userop.mutable_user()->set_password(password);

    return UserOp(userop);
}

int32_t PMClient::AddUser(const std::string username, const std::string password) {
    UserOperation userop;
    userop.set_type(UserOperation::ADD);
    userop.mutable_user()->set_username(username);
    userop.mutable_user()->set_password(password);

    return UserOp(userop);
}

int32_t PMClient::DeleteUser(const std::string username) {
    UserOperation userop;
    userop.set_type(UserOperation::DELETE);
    userop.mutable_user()->set_username(username);

    return UserOp(userop);
}

int32_t PMClient::UserOp(const UserOperation userop) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    proto_rpc::RPCRequest<UserOperation>   req;
    proto_rpc::RPCResponse<UserOperationReply> res;

    req.CopyFrom(userop);
    req.set_method(rpc_const::kUserOpMethod);

    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to operate user");
        return error::CLIENT_USER_OP_FAIL;
    }

    if (res.replytype() == UserOperationReply::GET) {
        for (int i = 0; i < res.user_size(); i++) {
            user.insert(res.user(i));
        }
    }
    return ret;
}

int32_t PMClient::SetFlowControlRequest(const std::string& cmd_string,
        const std::string &processor, FlowControlRequest* request) {
    if (request == NULL) {
        DSTREAM_WARN("Bad argument, PMClient::SetFlowControlRequest"
                     "does not take null value pointer");
        return error::BAD_ARGUMENT;
    }

    DSTREAM_INFO("processname: %s, cmd: %s", processor.c_str(), cmd_string.c_str());
    request->set_processor(processor);
    request->set_cmd(FlowControlRequest_Operation_SET);

    std::string cmd(cmd_string);
    boost::to_upper(cmd);
    if (cmd == "START") {
        request->set_limit(-1); // no limit
        return error::OK;
    } else if (cmd == "STOP") {
        request->set_limit(0);
        return error::OK;
    } else if (cmd == "STATUS") {
        request->set_cmd(FlowControlRequest_Operation_GET);
        return error::OK;
    } else {
        char *endptr = NULL;
        int64_t qps_limit = strtol(cmd_string.c_str(), &endptr, 10);
        if (*endptr == '\0') {
            qps_limit = (qps_limit < 0 || qps_limit > INT32_MAX) ? -1 : qps_limit;
            request->set_limit((int32_t)qps_limit);
        } else {
            DSTREAM_WARN("unknown command: %s(%s)", cmd_string.c_str(), endptr);
            return error::CLIENT_FLOW_CONTROL_FAIL;
        }
    }
    return error::OK;
}

void PMClient::SetFlowControlResult(const FlowControlResponse *res,
        const std::string& cmd_string, const std::string& name, const std::string& processor) {
    std::string cmd(cmd_string);
    boost::to_upper(cmd);

    if (res->result() == error::OK) {
        if (cmd == "STATUS") {
            snprintf(flow_control_info, kMaxFlowControlInfoLen, 
                    "App[%s:%s] is running at spout limit: %d.",
                    name.c_str(), res->processor().c_str(), res->limit());
        } else {
            snprintf(flow_control_info, kMaxFlowControlInfoLen, 
                    "App[%s:%s] spout limit has set to: %d.",
                    name.c_str(), res->processor().c_str(), res->limit());
        }
    } else {
        snprintf(flow_control_info, kMaxFlowControlInfoLen,
                "FlowControl command failed, result: %s", error::get_text(res->result()));
    }
}

std::string PMClient::FlowControlInfo() {
    return std::string(flow_control_info);
}

int32_t PMClient::UpdateProcessor(const Application& update) {
    CHECK_LOGIN();
    int32_t ret = error::OK;
    int32_t pm_timeout_sec;
    g_cfg_map.GetIntValue(config_const::kClientToPMTimeoutName, &pm_timeout_sec);

    // strong check if no app exists!
    Application old;
    ret = GetAppTopology(update.name(), old);
    if (ret < error::OK) {
        DSTREAM_WARN("app [%s] does not exists", update.name().c_str());
        return ret;
    }

    // you can only update one processor once
    const Topology& tp = update.topology();
    if (tp.processor_num() > 1 || tp.processors_size() > 1) {
        DSTREAM_WARN("you could only update one processor once");
        return error::CLIENT_UPDATE_PRO_FAIL;
    }
    std::string processor_name = tp.processors(0).name();

    // judge whether update or rollback
    bool rollback = false;
    if (g_cmd_line_args.revision != -1) {
        DSTREAM_INFO("rollback");
        rollback = true;
    } else {
        DSTREAM_INFO("update");
    }

    // get revision on zk
    proto_rpc::RPCRequest<UpdateProRequest>  req;
    proto_rpc::RPCResponse<UpdateProResponse> res;
    req.set_method(rpc_const::kUpdateProcessorMethod);
    req.set_session_id(m_user.session_id());
    req.mutable_app()->CopyFrom(update);
    req.set_processor_name(processor_name);
    if (rollback) {
        req.set_revision(g_cmd_line_args.revision);
    }
    if (m_connection.Call(&req, &res, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res.return_value()) ) {
        DSTREAM_WARN("fail to update processor from pm, ret value:[%s]",
                     res.return_value().c_str());
        return error::CLIENT_UPDATE_PRO_FAIL;
    }

    // check response
    int revision = res.revision();

    /* upload app to filesystem */
    std::string fs_path = old.app_path().path();
    if (!rollback) {
        std::string local_file = g_cmd_line_args.app_dir + "/" + processor_name;
        std::string remote_file = fs_path;
        remote_file.append("/");
        remote_file.append(processor_name);
        if (IsDir(local_file.c_str())) {
            local_file.append("/*");
            remote_file.append("_v");
            StringAppendNumber(&remote_file, revision);
        }

        DSTREAM_INFO("upload file %s to %s", local_file.c_str(), remote_file.c_str());
        ret = m_hdfs_client->UploadFile(local_file, remote_file, true);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to upload app dir:[%s] to hdfs[%s]",
                         processor_name.c_str(), fs_path.c_str());
            return ret;
        }
    } else {
        // check this revision on hdfs or not
        std::string check_path = fs_path + "/" + processor_name + "_v";
        StringAppendNumber(&check_path, revision);

        DSTREAM_INFO("check file %s on hdfs", check_path.c_str());
        bool exist = false;
        ret = m_hdfs_client->TestFile(check_path, exist);
        if (ret < error::OK) {
            DSTREAM_WARN("fail to check path [%s] on hdfs", check_path.c_str());
            return ret;
        } else {
            if (!exist) {
                DSTREAM_WARN("path [%s] does not exists on hdfs", check_path.c_str());
                return error::FILE_OPERATION_ERROR;
            }
        }
    }

    // notify pm to modify revision on zk
    proto_rpc::RPCRequest<UpdateProDone>  req_done;
    proto_rpc::RPCResponse<rpc_const::UpdateProAck> res_done;
    req_done.set_method(rpc_const::kUpdateProDoneMethod);
    req_done.set_session_id(m_user.session_id());
    req_done.set_status(rpc_const::kOK);
    req_done.mutable_app()->CopyFrom(old);
    req_done.set_processor_name(processor_name);
    req_done.set_revision(revision);
    if (m_connection.Call(&req_done, &res_done, pm_timeout_sec) < 0 ||
        !rpc_const::IsReturnValueOK(res_done.return_value()) ) {
        DSTREAM_WARN("fail to update processor done from pm, ret value:[%s]",
                     res_done.return_value().c_str());
        return error::CLIENT_UPDATE_PRO_FAIL;
    }

    return ret;
}

} // namespace client
} // namespace dstream
