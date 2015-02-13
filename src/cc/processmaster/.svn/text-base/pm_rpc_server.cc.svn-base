/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/14
* @filename:    pm_rpc_server.cc
* @author:      zhanggengxin
* @brief:       pm rpc server
*/

#include "processmaster/pm_rpc_server.h"

#include <assert.h>
#include <sys/time.h>
#include <set>
#include <string>

#include "common/config_const.h"
#include "common/error.h"
#include "common/id.h"
#include "metamanager/meta_manager.h"
#include "processmaster/app_status.h"
#include "processmaster/dstream_event.h"
#include "processmaster/kill_app_gc_task.h"
#include "processmaster/pm_common.h"
#include "processmaster/pn_client_status.h"
#include "processmaster/update_app_event.h"

namespace dstream {
namespace pm_rpc_server {
using namespace application_tools;
using namespace auto_ptr;
using namespace config_const;
using namespace rpc_const;
using namespace rpc_server;
using namespace dstream_event;
using namespace gc_task;
using namespace kill_app_gc_task;
using namespace meta_manager;
using namespace event_manager;
using namespace pn_client_status;
using namespace pm_common;
using namespace pm_gc;
using namespace update_app_event;
using namespace scheduler;
using namespace zk_watcher;
using std::set;

#define CHECK_META_STATUS()                               \
    MetaManager* meta = MetaManager::GetMetaManager();      \
    PNClientStatus* status = PNClientStatus::GetPNClientStatus(); \
    if (NULL == meta || NULL == status) {                    \
        DSTREAM_WARN("meta or staus not init");                \
        return rpc_const::kFail;                                \
    }                                                         \

#define CHECK_CLIENT_USER(session_id)                                    \
    CHECK_META_STATUS();                                                   \
    client_info ci;                                                        \
    if (status->FindClient(session_id, &ci) != error::OK) {                  \
        DSTREAM_WARN("cannot find client [%lu]", session_id);                \
        return rpc_const::kFail;                                             \
    }                                                                      \
    if (ci.user.username() == kRootName) {                                 \
        DSTREAM_INFO("root user [%s] cannot ask for app id", kRootName);     \
        return rpc_const::kFail;                                              \
    }                                                                        \

bool PMRPCServer::Init(AutoPtr<EventManager> event_manager,
                       AutoPtr<DStreamWatcher> watcher,
                       AutoPtr<GC>gc, AutoPtr<Scheduler> scheduler) {
    m_event_manager = event_manager;
    m_watcher = watcher;
    m_gc = gc;
    m_scheduler = scheduler;
    return (RegisterProtocol(kPNReport, dynamic_cast<PNReportProtocol*>(this)) && // NOLINT
            RegisterProtocol(kLoginMethod, dynamic_cast<LoginProtocol*>(this)) && // NOLINT
            RegisterProtocol(kApplyAppIDMethod, dynamic_cast<ApplyAppIDProtocol*>(this)) && // NOLINT
            RegisterProtocol(kSubmitAppMethod, dynamic_cast<SubmitAppProtocol*>(this)) && // NOLINT
            RegisterProtocol(kNotifyPMMethod, dynamic_cast<NotifyPMProtocol*>(this)) && // NOLINT
            RegisterProtocol(kKillAppMethod, dynamic_cast<KillAppProtocol*>(this)) && // NOLINT
            RegisterProtocol(kKillPEMethod, dynamic_cast<KillPEProtocol*>(this)) && // NOLINT
            RegisterProtocol(kKillPNMethod, dynamic_cast<KillPNProtocol*>(this)) && // NOLINT
            RegisterProtocol(kUserOpMethod, dynamic_cast<UserOpProtocol*>(this)) && // NOLINT
            RegisterProtocol(kFlowControlMethod, dynamic_cast<FlowControlProtocol*>(this)) && // NOLINT
            RegisterProtocol(kUpdateProcessorMethod, dynamic_cast<UpdateProcessorProtocol*>(this)) && // NOLINT
            RegisterProtocol(kUpdateProDoneMethod, dynamic_cast<UpdateProDoneProtocol*>(this)) && // NOLINT
            RegisterProtocol(kUpdateTopologyMethod, dynamic_cast<UpdateTopologyProtocol*>(this)) && // NOLINT
            RegisterProtocol(kAddNewTopoMethod, dynamic_cast<AddNewTopoProtocol*>(this)) && // NOLINT
            RegisterProtocol(kGetMetricMethod, dynamic_cast<GetMetricProtocol*>(this)) &&  // NOLINT
            RegisterProtocol(kBlackListMethod, dynamic_cast<BlackListOperationProtocol*>(this))); // NOLINT
}

std::string PMRPCServer::PNREPORT(const PNReport* report,
                                  ReportResponse* response) {
    DSTREAM_DEBUG("pn [%s] report", report->id().id().c_str());
    response->set_next_report_interval(kPNIntervalSecs);
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    if (NULL == status || status->UpdatePNStatus(report) != error::OK) {
        DSTREAM_WARN("pn client status is null");
        return rpc_const::kFail;
    }
    return rpc_const::kOK;
}

std::string PMRPCServer::LOGIN(const User* request_user, LoginAck* response) {
#ifdef VERSION
    response->mutable_user()->set_version(VERSION);
#else
    response->mutable_user()->set_version(config_const::kUnknownPMVersion);
#endif
    if (!request_user->has_version()) {
        DSTREAM_WARN("client login declined, version mismatched:"
                     "client version:[%s], pm version:[%s], "
                     "minimum client version required:[%s]",
                     config_const::kUnknownClientVersion,
                     response->user().version().c_str(),
                     config_const::kPMMinRequiredClientVersion);
        response->set_code(LoginAck_ErrorCode_VERSION_MISSMATCH);
        return rpc_const::kFail;
    } else {
        if (strncmp(request_user->version().c_str(), VERSION, config_const::kVersionLen) > 0 ||
            strncmp(request_user->version().c_str(),
                    config_const::kPMMinRequiredClientVersion, config_const::kVersionLen) < 0) {
            DSTREAM_WARN("Client login declined, version mismatched:"
                         "client version:[%s], pm version:[%s], "
                         "minimum client version required:[%s]",
                         request_user->version().c_str(),
                         response->user().version().c_str(),
                         config_const::kPMMinRequiredClientVersion);
            response->set_code(LoginAck_ErrorCode_VERSION_MISSMATCH);
            return rpc_const::kFail;
        }
    }

    CHECK_META_STATUS();
    MutexLockGuard mutext_guard(m_lock);
    if (meta->CheckUser(*request_user) != error::OK &&
        !(request_user->username() == kRootName &&
          request_user->password() == kRootPassWord)) {
        DSTREAM_WARN("check user %s %s fail", request_user->username().c_str(),
                     request_user->password().c_str());
        response->set_code(LoginAck_ErrorCode_WRONG_USER);
        return rpc_const::kFail;
    }
    if (m_watcher->StartWatch(request_user->hostname()) != error::OK) {
        DSTREAM_WARN("cannot watch client %s",
                     request_user->hostname().c_str());
        response->set_code(LoginAck_ErrorCode_OTHER);
        return rpc_const::kFail;
    }
    timeval now;
    gettimeofday(&now, NULL);
    response->mutable_user()->CopyFrom(*request_user);
    if (static_cast<uint64_t>(now.tv_usec) > m_max_session_id) {
        m_max_session_id = now.tv_usec;
    } else {
        ++m_max_session_id;
    }
    response->mutable_user()->set_session_id(m_max_session_id);
    if (status->AddClient(request_user, response->user().session_id()) == error::OK) {
        return rpc_const::kOK;
    }
    response->set_code(LoginAck_ErrorCode_OTHER);
    return rpc_const::kFail;
}

std::string PMRPCServer::ASKFORAPPID(const AppID* app_id,
                                     AppID* response_app_id) {
    CHECK_CLIENT_USER(app_id->session_id());
    if (meta->GetNewAppID(response_app_id) == error::OK) {
        response_app_id->set_session_id(app_id->session_id());
        if (status->UpdateClient(app_id->session_id(),
                                 *response_app_id) != error::OK) {
            DSTREAM_WARN("update client fail");
            return kFail;
        }
        DSTREAM_INFO("ask for app id [%lu]", response_app_id->id());
        return kOK;
    }
    DSTREAM_INFO("cannot get session [%lu]", app_id->session_id());
    return kFail;
}

std::string PMRPCServer::SUBMITAPP(const Application* app, FileConfig* file_response) {
    CHECK_CLIENT_USER(app->session_id());
    ClusterConfig cluster_config;
    if (meta->GetClusterConfig(&cluster_config) != error::OK) {
        DSTREAM_WARN("Get Cluster config fail");
        return kFail;
    }
    Application add_app = *app;
    *(add_app.mutable_user()) = ci.user;
    *(add_app.mutable_id()) = ci.app_id;
    if (!InitAppPath(cluster_config, &add_app)) {
        DSTREAM_WARN("init app path fail");
        return kFail;
    }
    *(add_app.mutable_id()) = ci.app_id;
    if (meta->AddApplication(&add_app) != error::OK) {
        DSTREAM_WARN("Add application [%lu] fail", ci.app_id.id());
        return kFail;
    }
    *file_response = add_app.app_path();
    return kOK;
}

static inline std::string UpdateAppStatus(Application* app,
                                          AppStatus new_status,
                                          MetaManager* meta,
                                          UpdateTopology* res_app,
                                          UpdateTopology_Operation succ_op,
                                          UpdateTopology_Operation fail_op) {
    app->set_status(new_status);
    if (meta->UpdateApplication(*app) == error::OK) {
        *(res_app->mutable_app()) = *app;
        res_app->set_type(succ_op);
        DSTREAM_INFO("Application [%lu] is updateing subtree [%d]", app->id().id(),
                     new_status);
        return kOK;
    }
    DSTREAM_WARN("upate application [%lu] fail", app->id().id());
    res_app->set_type(fail_op);
    return kFail;
}

std::string PMRPCServer::UPDATETOPOLOGY(const UpdateTopology* update,
                                        UpdateTopology* res_app) {
    if (!update->has_app()) {
        DSTREAM_WARN("update has no app");
        res_app->set_error_num(error::APP_NOT_EXIST);
        return kFail;
    }
    if (!update->app().has_id() && !update->app().has_name()) {
        DSTREAM_WARN("update has neigther app id nor app name");
        res_app->set_error_num(error::APP_NOT_EXIST);
        return kFail;
    }
    CHECK_CLIENT_USER(update->session_id());
    MutexLockGuard lock_guard(m_lock);
    Application app;
    if (update->app().has_id()) {
        if (meta->GetApplication(update->app().id(), &app) != error::OK) {
            DSTREAM_WARN("Get Application [%lu] fail", update->app().id().id());
            res_app->set_error_num(error::APP_NOT_EXIST);
            return kFail;
        }
    } else {
        if (GetAppByName(meta, update->app().name(), app) != error::OK) {
            DSTREAM_WARN("get app by name [%s] fail", update->app().name().c_str());
            res_app->set_error_num(error::APP_NOT_EXIST);
            return kFail;
        }
    }
    DSTREAM_INFO("update topology App [%lu] status [%d]", app.id().id(), app.status());
    switch (app.status()) {
    case RUN:
        if (update->type() == UpdateTopology_Operation_ADD) {
            status->UpdateClient(update->session_id(), app.id());
            return UpdateAppStatus(&app, ADDTOPO, meta, res_app,
                                   UpdateTopology_Operation_ADD_SUB_OK,
                                   UpdateTopology_Operation_ADD_SUB_FAIL);
        }
        if (update->type() == UpdateTopology_Operation_DEL) {
            status->UpdateClient(update->session_id(), app.id());
            return UpdateAppStatus(&app, DELTOPO, meta, res_app,
                                   UpdateTopology_Operation_DEL_SUB_OK,
                                   UpdateTopology_Operation_DEL_SUB_FAIL);
        }
        if (update->type() == UpdateTopology_Operation_UPDATE_PARA) {
            status->UpdateClient(update->session_id(), app.id());
            return UpdateAppStatus(&app, UPPARA, meta, res_app,
                                   UpdateTopology_Operation_DEL_SUB_OK,
                                   UpdateTopology_Operation_DEL_SUB_FAIL);
        }

        DSTREAM_WARN("request stauts [%d] is wrong", update->type());
        return kFail;
        break;
    case ADDTOPO:
        if (update->type() == UpdateTopology_Operation_ADD_SUB_OK) {
            app = update->app();
            // app.set_status(ADDSUB);
            if (meta->UpdateApplication(app) != error::OK) {
                DSTREAM_WARN("update application [%lu] fail", app.id().id());
                res_app->set_error_num(error::SERIAIL_DATA_FAIL);
                return kFail;
            }
            app_status::AppStatus::GetAppStatus()->UpdateApplication(app);
            m_event_manager->AddEvent(EventPtr(new AddSubtreeEvent(app,
                                               m_event_manager)));
            return kOK;
        }
        if (update->type() == UpdateTopology_Operation_ADD_SUB_FAIL) {
            app.set_status(RUN);
            m_event_manager->AddEvent(EventPtr(new UpdateAppStatusEvent(app,
                                               m_event_manager)));
            return kOK;
        }
        break;
    case DELTOPO:
        if (update->type() == UpdateTopology_Operation_DEL_SUB_OK) {
            app = update->app();
            // app.set_status(DELSUB);
            if (meta->UpdateApplication(app) != error::OK) {
                DSTREAM_WARN("update application [%lu] fail", app.id().id());
                res_app->set_error_num(error::SERIAIL_DATA_FAIL);
                return kFail;
            }
            app_status::AppStatus::GetAppStatus()->UpdateApplication(app);
            EventPtr del_event_ptr = EventPtr(new DelSubtreeEvent(app, m_event_manager,
                                              m_scheduler));
            del_event_ptr->HandleEvent();
            m_event_manager->AddEvent(del_event_ptr);
            return kOK;
        }
        if (update->type() == UpdateTopology_Operation_DEL_SUB_FAIL) {
            app.set_status(RUN);
            m_event_manager->AddEvent(EventPtr(new UpdateAppStatusEvent(app,
                                               m_event_manager)));
            return kOK;
        }
        break;
    case UPPARA:
        if (update->type() == UpdateTopology_Operation_UPDATE_PARA_OK) {
            app = update->app();
            // app.set_status(UPPARA);
            if (meta->UpdateApplication(app) != error::OK) {
                DSTREAM_WARN("update application [%lu] fail", app.id().id());
                res_app->set_error_num(error::SERIAIL_DATA_FAIL);
                return kFail;
            }
            app_status::AppStatus::GetAppStatus()->UpdateApplication(app);
            EventPtr up_event_ptr = EventPtr(new UpdateParaEvent(app, m_event_manager,
                                             m_scheduler));
            up_event_ptr->HandleEvent();
            m_event_manager->AddEvent(up_event_ptr);
            return kOK;
        }
        if (update->type() == UpdateTopology_Operation_UPDATE_PARA_FAIL) {
            app.set_status(RUN);
            m_event_manager->AddEvent(EventPtr(new UpdateAppStatusEvent(app,
                                               m_event_manager)));
            return kOK;
        }
        break;
    default:
        DSTREAM_INFO("Application [%lu] staus [%d] is wrong", app.id().id(),
                     app.status());
        return kFail;
    }
    return kFail;
}

std::string PMRPCServer::ADDNEWTOPO(const Application* /*add_new_pe*/,
                                    NotifyAck* /*add_new_notify*/) {
    return rpc_const::kFail;
}

std::string PMRPCServer::FILEUPLOADDONE(const NotifyPM* notify,
                                        NotifyAck* /*response*/) {
    CHECK_CLIENT_USER(notify->session_id());
    if (notify->status() == kOK) {
        Application app;
        if (meta->GetApplication(ci.app_id, &app) == error::OK) {
            app.set_status(RUN);
            app.set_submit_time(uint64_t(GetTimeMS()));
            if (meta->UpdateApplication(app) == error::OK) {
                app_status::AppStatus::GetAppStatus()->UpdateApplication(app);
                m_event_manager->AddEvent(EventPtr(new AddAppEvent(app.id())));
                m_watcher->UnWatch(ci.user.hostname());
            } else {
                DSTREAM_WARN("update app or create pe fail");
                return kFail;
            }
        } else {
            DSTREAM_WARN("Get Application [%lu] fail", ci.app_id.id());
            return kFail;
        }
    }
    // status->RemoveClient(notify->session_id());
    return kOK;
}

std::string PMRPCServer::KILLAPP(const Application* app, AppID* response) {
    CHECK_CLIENT_USER(app->session_id());
    if (!app->has_id() && !app->has_name()) {
        DSTREAM_WARN("no app id neither app name");
        return kFail;
    }

    Application kill_app;
    error::Code res = error::OK;
    if (app->has_id()) {
        if ((res = meta->GetApplication(app->id(), &kill_app)) != error::OK) {
            /* if (res == error::ZK_NO_NODE) {
              return kOK;
            }*/
            DSTREAM_WARN("get application [%lu] fail", app->id().id());
            return kFail;
        }
    } else {
        std::string app_name = app->name();
        res = static_cast<error::Code>(GetAppByName(meta, app_name, kill_app));
        if (res != error::OK) {
            /* if (res == error::APP_NOT_EXIST) {
              return kOK;
            }*/
            DSTREAM_WARN("get application[%s] fail", app_name.c_str());
            return kFail;
        }
    }

    response->set_id(kill_app.id().id());
    kill_app.set_status(KILL);
    if (meta->UpdateApplication(kill_app) != error::OK) {
        DSTREAM_WARN("update application [%s][%lu] fail",
                     kill_app.name().c_str(), kill_app.id().id());
        return kFail;
    }
    // remove resource
    if (NULL != m_scheduler) {
        const Topology& topology = kill_app.topology();
        PidList pid_list;
        for (int i = 0; i < topology.processors_size(); ++i) {
            const Processor& processor = topology.processors(i);
            pid_list.push_back(ConstructProcessorID(kill_app.id(), processor.id()));
        }
        m_scheduler->RemoveResRequest(pid_list);
    }
    DSTREAM_INFO("Kill App [%s][%lu]", kill_app.name().c_str(),
                 kill_app.id().id());
    GcTaskAutoPtr gc_task_ptr = GcTaskAutoPtr(new KillAppGCTask(kill_app.id(), m_event_manager));
    gc_task_ptr->RunTask(m_scheduler);
    m_gc->AddGCTask(gc_task_ptr);
    return kOK;
}

std::string PMRPCServer::KILLPE(const KillPE* /*kill_pe*/, KillPEAck* /*response*/) {
    // check pe is exist
    /* AppID app_id = kill_pe->app_id();
    PEID pe_id = kill_pe->pe_id();
    BackupPEID backup_pe_id = kill_pe->backup_pe_id();
    PNID pn_id = kill_pe->pn_id();
    m_event_manager->AddEvent(EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id, pn_id)) );*/
    return rpc_const::kOK;
}

std::string PMRPCServer::KILLPN(const PNID* /*pn_id*/, KillPNAck* /*response*/) {
    // TODO(fangjun02)
    // MetaManager* meta_manager_ = MetaManager::GetMetaManager();
    return rpc_const::kOK;
}

typedef set<User, CompareUser>::iterator UserSetIter;
std::string PMRPCServer::USEROP(const UserOperation* userop, UserOperationReply* response) {
    MetaManager* meta_manager_ = MetaManager::GetMetaManager();
    ClusterConfig cluster_config;
    if (meta_manager_->GetClusterConfig(&cluster_config) != error::OK) {
        return rpc_const::kFail;
    }
    MutexLockGuard mutex_guard(m_lock);
    std::set<User, CompareUser> userinfo;
    UserSetIter user_index;

    switch (userop->type()) {
    case UserOperation::GET:
        response->set_replytype(UserOperationReply::GET);
        if (GetUser(cluster_config, &userinfo) != error::OK) {
            return rpc_const::kFail;
        }
        for (user_index = userinfo.begin(); user_index != userinfo.end(); user_index++) {
            User* current_user = response->add_user();
            current_user->CopyFrom(*user_index);
        }
        break;
    case UserOperation::UPDATE:
        response->set_replytype(UserOperationReply::UPDATE);
        if (UpdateUser(cluster_config, userop->user()) != error::OK) {
            return rpc_const::kFail;
        }
        break;
    case UserOperation::ADD:
        response->set_replytype(UserOperationReply::ADD);
        if (AddUser(cluster_config, userop->user()) != error::OK) {
            return rpc_const::kFail;
        }
        break;
    case UserOperation::DELETE:
        response->set_replytype(UserOperationReply::DELETE);
        if (DeleteUser(cluster_config, userop->user()) != error::OK) {
            return rpc_const::kFail;
        }
        break;
    }

    if (meta_manager_->UpdateClusterConfig(cluster_config) == error::OK) {
        return rpc_const::kOK;
    }
    return rpc_const::kFail;
}
std::string PMRPCServer::BLACKLISTOPERATION(const BlackListOperationRequest* request,
                                            BlackListOperationResponse* response) {
    int32_t req_pe_num = request->pe_id().size();
    int32_t code;
    switch(request->operation()) {
        case BlackListOperationRequest::ERASE: {
            for (int i = 0; i< req_pe_num; i++) {
                code = m_scheduler->ErasePEBlackList(request->pe_id(i));
                if(code < error::OK) {
                    DSTREAM_WARN("failed to erase pe[%zu] scheduler blacklist:%s",
                                  request->pe_id(i).id(), dstream::error::ErrStr(code));
                    return rpc_const::kFail;
                } else {
                    DSTREAM_INFO("pe[%zu] blacklist has been erased", request->pe_id(i).id());
                }
            }
            break;
        }
        case BlackListOperationRequest::CLEAR: {
            code = m_scheduler->ClearBlackList();
            if(code < error::OK) {
                DSTREAM_WARN("failed to  clear all pe scheduler blacklist:%s",
                              dstream::error::ErrStr(code));
                return rpc_const::kFail;
            } else {
                DSTREAM_INFO("all pe scheduler blacklist are cleared");
            }
            break;
        }
        case BlackListOperationRequest::GET: {
            std::set<PNID, PNIDCompare> black_set;
            std::set<PNID, PNIDCompare>::iterator black_set_iter;
            Application app;
            MetaManager* meta = MetaManager::GetMetaManager();
            app.mutable_id()->set_id(0);
            for (int i = 0; i < req_pe_num; ++i) {
                black_set.clear();
                PEID pe_id = request->pe_id(i);
                uint64_t app_id = application_tools::app_id(pe_id);
                if(app_id == 0) {
                    DSTREAM_WARN("pe id [%lu] is not exist", pe_id.id());
                    continue;
                } else if(app_id != app.id().id()) {
                    AppID appid;
                    appid.set_id(app_id);
                    if(meta->GetApplication(appid, &app) != error::OK) {
                        DSTREAM_WARN("get app [%lu] failed, pe id [%lu] is not exist",
                                      appid.id(), pe_id.id());
                        continue;
                    }
                }
                // check pe id is exist
                Topology tp = app.topology();
                int32_t pro_id = static_cast<int32_t>(EXTRACT_PROCESSOR_ID(pe_id.id()));
                if(pro_id >= tp.processors_size() ||
                   EXTRACT_SEQUENCE_ID(pe_id.id()) >= tp.processors(pro_id).parallism()) {
                    DSTREAM_WARN("pe id [%lu] is not exist", pe_id.id());
                    continue;
                }

                code = m_scheduler->GetPEBlackList(request->pe_id(i), &black_set);
                if (code < error::OK) {
                    DSTREAM_WARN("failed to get pe[%zu] scheduler blacklist:%s",
                                 request->pe_id(i).id(), dstream::error::ErrStr(code));
                } else {
                    PEPNBlackList pe_pn_blacklist;
                    pe_pn_blacklist.mutable_pe_id()->set_id(request->pe_id(i).id());
                    for(black_set_iter = black_set.begin(); black_set_iter != black_set.end(); ++black_set_iter) {
                        pe_pn_blacklist.add_pn_id()->CopyFrom(*black_set_iter);
                    }
                    response->add_pe_pn_blacklist()->CopyFrom(pe_pn_blacklist);
                }
            }
            break;
        }
        default: {
            DSTREAM_WARN("unkown blacklist operation type:%d", request->operation());
            return rpc_const::kFail;
        }
    }
    return rpc_const::kOK;
}
std::string PMRPCServer::GETMETRIC(const StatusRequest* request,
                                   StatusResponse* response) {
    PNClientStatus* status = PNClientStatus::GetPNClientStatus();
    if (NULL == status) {
        DSTREAM_WARN("staus not init");
        return rpc_const::kFail;
    }
    PnidReportMap pn_reports_ = status->pn_reports();
    int req_pn_num = request->pn_id_size();
    PnidReportMapIter report_end = pn_reports_.end();
    for (int i = 0; i < req_pn_num; ++i) {
        PnidReportMapIter report_find = pn_reports_.find(request->pn_id(i));
        if (report_find != report_end) {
            PNReport* pn_report = response->add_pn_report_list();
            *pn_report = report_find->second;
        }
    }
    int req_pe_num = request->pe_id_size();
    if (req_pe_num > 0 && pn_reports_.size() > 0) {
        typedef set<PEID, PEIDCompare> PeidSet;
        typedef PeidSet::iterator PeidSetIter;
        PeidSet pe_id_set;
        for (int i = 0; i < req_pe_num; ++i) {
            pe_id_set.insert(request->pe_id(i));
        }
        PeidSetIter pe_set_end = pe_id_set.end();
        for (PnidReportMapIter report_iter = pn_reports_.begin();
             report_iter != report_end; ++report_iter) {
            int bk_pe_size = report_iter->second.status_size();
            for (int i = 0; i < bk_pe_size; ++i) {
                const BackupPEStatus& bk_pe_status = report_iter->second.status(i);
                if (pe_id_set.find(bk_pe_status.pe_id()) != pe_set_end) {
                    *(response->add_pe_status_list()) = bk_pe_status;
                }
            }
        }
    }
    return kOK;
}

std::string PMRPCServer::FLOWCONTROL(const FlowControlRequest* request,
                                     FlowControlResponse* response) {
    CHECK_CLIENT_USER(request->session_id());
    Application current_app;
    error::Code res = error::OK;

    do {
        if (request->has_app_id()) {
            if (meta->GetApplication(request->app_id(), &current_app) != error::OK) {
                DSTREAM_WARN("Get Application fail");
                res = error::APP_NOT_EXIST;
                break;
            }
        } else {
            std::string app_name = request->name();
            res = static_cast<error::Code>(GetAppByName(meta, app_name, current_app));
            if (res != error::OK) {
                DSTREAM_WARN("get application[%s] fail", app_name.c_str());
                res = error::APP_NOT_EXIST;
                break;
            }
        }

        if (!request->has_processor()) {
            DSTREAM_WARN("wrong parameters: flow control command need processor name.");
            res = error::BAD_ARGUMENT;
            break;
        }

        // find processors
        Processor *processor_to_limit = NULL;
        bool processor_find = false;
        for (int idx = 0; idx < current_app.topology().processors_size(); ++idx) {
            processor_to_limit = current_app.mutable_topology()->mutable_processors(idx);
            if (processor_to_limit->name() == request->processor() ||
                config_const::kAllProcessors == request->processor()) {
                // skip non-importers
                //if (processor_to_limit->role() != IMPORTER) {
                //    continue;
                //}
                processor_find = true;
                // handle commands
                if (request->cmd() == FlowControlRequest_Operation_SET) {
                    processor_to_limit->mutable_flow_control()->set_qps_limit(request->limit());
                    res = meta->UpdateApplication(current_app);
                    if (res != error::OK) {
                        DSTREAM_WARN("Update Application fail");
                        break;
                    }
                    DSTREAM_INFO("update processor(%s:%s) qps limit to %d OK.",
                            request->processor().c_str(), processor_to_limit->name().c_str(),
                            request->limit());
                    response->set_limit(request->limit());
                } else {
                    response->set_limit(processor_to_limit->flow_control().qps_limit());
                }
            }
        }

        if (!processor_find) {
            DSTREAM_WARN("cannot find processor(%s)", request->processor().c_str());
            res = error::PROCESSOR_NOT_EXIST;
            break;
        }

        // one or more update failed.
        if (res != error::OK) {
            break;
        }

        response->set_name(request->name());
        response->set_processor(request->processor());
    } while (0);

    response->set_result(res);
    return kOK;
}

std::string PMRPCServer::UPDATEPROCESSOR(const UpdateProRequest* request,
                                         UpdateProResponse* response) {
    CHECK_CLIENT_USER(request->session_id());
    Application app;
    if (GetAppByName(meta, request->app().name(), app) != error::OK) {
        DSTREAM_WARN("get app by name [%s] fail", request->app().name().c_str());
        response->set_error_num(error::APP_NOT_EXIST);
        return kFail;
    }

    // check revision of processor_name
    unsigned int cur_revision, max_revision;
    bool found = false;
    Topology* tp = app.mutable_topology();
    for (int i = 0; i < tp->processors_size(); i++) {
        if (request->processor_name() == tp->processors(i).name()) {
            found = true;
            cur_revision = tp->processors(i).cur_revision();
            max_revision = tp->processors(i).max_revision();
            break;
        }
    }

    if (!found) {
        DSTREAM_WARN("get processor [%s] fail", request->processor_name().c_str());
        response->set_error_num(error::PROCESSOR_NOT_EXIST);
        return kFail;
    }

    // rollback
    if (request->has_revision()) {
        if (request->revision() > max_revision) {
            DSTREAM_WARN("get processor [%s] revision [%u] fail, current is [%u], max is [%u]",
                         request->processor_name().c_str(), request->revision(),
                         cur_revision, max_revision);
            response->set_error_num(error::REVISION_NOT_EXIST);
            return kFail;
        } else {
            response->set_revision(request->revision());
        }
        // update
    } else {
        response->set_revision(max_revision + 1);
    }

    return kOK;
}

std::string PMRPCServer::UPDATEPRODONE(const UpdateProDone* request,
                                       rpc_const::UpdateProAck* response) {
    CHECK_CLIENT_USER(request->session_id());
    if (request->status() == kOK) {
        Application app;
        if (GetAppByName(meta, request->app().name(), app) != error::OK) {
            DSTREAM_WARN("get app by name [%s] fail", request->app().name().c_str());
            return kFail;
        }
        Topology* tp = app.mutable_topology();
        uint64_t pid;
        unsigned int revision;
        EventPtr update_pro_ptr;
        for (int i = 0; i < tp->processors_size(); i++) {
            if (request->processor_name() == tp->processors(i).name()) {
                unsigned int max_revision = tp->processors(i).max_revision();
                pid = tp->processors(i).id().id();

                if (request->revision() <= max_revision) {
                    DSTREAM_INFO("rollback processor [%s] revision into [%u]",
                                 tp->processors(i).name().c_str(), request->revision());
                    tp->mutable_processors(i)->set_cur_revision(request->revision());
                    revision = request->revision();
                } else {
                    DSTREAM_INFO("update processor [%s] revision into [%u]",
                                 tp->processors(i).name().c_str(), max_revision + 1);
                    max_revision++;
                    revision = max_revision;
                    tp->mutable_processors(i)->set_cur_revision(max_revision);
                    tp->mutable_processors(i)->set_max_revision(max_revision);
                }

                update_pro_ptr = EventPtr(new UpdateProcessorEvent(app.id(),
                                          tp->processors(i).name(), revision));
                break;
            }
        }

        if (meta->UpdateApplication(app) != error::OK) {
            DSTREAM_WARN("update revision failed");
            return kFail;
        }
        app_status::AppStatus::GetAppStatus()->UpdateProcessor(
            MAKE_APP_PROCESSOR_ID(app.id().id(), pid), revision);
        if (NULL != update_pro_ptr) {
            update_pro_ptr->HandleEvent();
            m_event_manager->AddEvent(update_pro_ptr);
        }
    }
    return kOK;
}

} // namespace pm_rpc_server
} // namespace dstream
