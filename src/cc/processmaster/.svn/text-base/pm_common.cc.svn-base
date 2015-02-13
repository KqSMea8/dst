/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    pm_common.cc
* @author:      zhanggengxin@baidu.com
* @brief:       common functions used for pm
*/

#include "processmaster/pm_common.h"

#include "common/application_tools.h"
#include "common/config_const.h"
#include "common/hdfs_client.h"
#include "metamanager/meta_manager.h"
#include "metamanager/zk_config.h"
#include "processmaster/dstream_event.h"

namespace dstream {
namespace pm_common {
using namespace application_tools;
using namespace hdfs_client;
using namespace config_const;
using namespace dstream_event;
using namespace meta_manager;
using namespace event_manager;
using namespace zk_config;
using namespace zk_client;
using namespace auto_ptr;
using namespace scheduler;

Config* g_config = NULL;

static int32_t RemoveAppDir(const std::string& path) {
    if (g_config == NULL) {
        DSTREAM_WARN("config is not init");
        return error::REMOVE_FILE_FAIL;
    }
    std::string hdfs_client_path;
    if (g_config->GetValue(kHDFSClientDirName, &hdfs_client_path) != error::OK) {
        hdfs_client_path = kHDFSClientDir;
    }
    int32_t res = error::OK;
    HdfsClient hdfs_client(hdfs_client_path, kDefaultHDFSRetry);
    if (LowerString(path.substr(0, 4)) == "hdfs") {
        bool exist;
        if ((res = hdfs_client.TestFile(path, exist)) == error::OK) {
            if (!exist) {
                DSTREAM_INFO("path [%s] does not exist", path.c_str());
                return error::OK;
            }
            if (exist && (res = hdfs_client.RemoveFile(path)) != error::OK) {
                DSTREAM_WARN("remove path [%s] fail", path.c_str());
                return error::REMOVE_FILE_FAIL;
            }
        }
    } else {
        std::string rm_cmd = "rm -rf " + path;
        HdfsClient::HPCmdExec(rm_cmd);
    }
    DSTREAM_INFO("Remove path [%s] ok", path.c_str());
    return error::OK;
}

bool AddCreatePEEvent(const Application& app,
                      EventManager& event_manager,
                      bool check_replicate /*= false*/) {
    MetaManager* meta = MetaManager::GetMetaManager();
    typedef std::list<ProcessorElement> PeList;
    typedef PeList::iterator PeListIter;
    PeList pe_list;
    if (meta->GetAppProcessElements(app.id(), &pe_list) != error::OK) {
        DSTREAM_WARN("get pe list fail");
        return false;
    }
    PeListIter pe_end = pe_list.end();
    for (PeListIter pe_iter = pe_list.begin(); pe_iter != pe_end; ++pe_iter) {
        // check replicate number
        if (check_replicate) {
            Processor processor;
            GetProcessor(app, *pe_iter, &processor);
            if (static_cast<uint32_t>(pe_iter->backups_size()) > processor.backup_num()) {
                BackupPE* remove_be = pe_iter->mutable_backups()->ReleaseLast();
                if (remove_be->backup_pe_id().id() == pe_iter->primary(0).id()) {
                    BackupPE* tmp = pe_iter->mutable_backups()->ReleaseLast();
                    *(pe_iter->add_backups()) = *remove_be;
                    delete remove_be;
                    remove_be = tmp;
                }
                delete remove_be;
                if (meta->UpdateProcessElement(app.id(), *pe_iter) != error::OK) {
                    DSTREAM_WARN("update process element fail");
                    return false;
                }
            }
        }
        // check pe assignment
        if (!CheckPENormal(*pe_iter, app.id(), pe_iter->pe_id())) {
            if (!event_manager.AddEvent(EventPtr(new CreatePEEvent(app.id(),
                                                 pe_iter->pe_id(),
                                                 pe_iter->resource())))) {
                DSTREAM_WARN("add crate pe event fail ");
                return false;
            }
            DSTREAM_DEBUG("add create pe event");
        }
        // check assignmented pes' pn
        for (int i = 0; i < pe_iter->backups_size(); ++i) {
            const BackupPE& bk_pe = pe_iter->backups(i);
            if (bk_pe.has_pn_id()) {
                PN pn;
                // need to do : check pn exist, zk_client does not return code
                if (meta->GetPN(bk_pe.pn_id(), &pn) != error::OK) {
                    if (!AddPEFailEvent(&event_manager, *pe_iter,
                                        bk_pe.backup_pe_id(), bk_pe.pn_id())) {
                        DSTREAM_WARN("add pe fail event fail");
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

bool AddPEFailEvent(EventManager* event_manager,
                    const ProcessorElement& pe,
                    const BackupPEID& bk_pe_id,
                    const PNID& pn_id) {
    return event_manager->AddEvent(EventPtr(new PEFailEvent(pe.app(),
                                            pe.pe_id(),
                                            pe.resource(),
                                            bk_pe_id,
                                            pn_id)));
}

int32_t CreateZK(const Config* config, AutoPtr<ZkClient>& zk_client) {
    ZKConfig zk_config(config);
    zk_client = AutoPtr<ZkClient>(new ZkClient(zk_config.GetZKPath().c_str(),
                                               zk_config.Timeout()));
    if (!zk_client.PtrValid()) {
        DSTREAM_ERROR("create zk fail");
        return error::CREATE_ZK_FAIL;
    }
    return zk_client->Connect();
}

void AddKillPEEvent(EventManager* event_manager, const AppID& app_id,
                    const PEID& pe_id, const Resource& resource,
                    const BackupPEID& bk_pe_id, const PNID& pn_id,
                    bool kill_app /*= false*/) {
    KillPEEvent* kill_pe_event = new KillPEEvent(app_id, pe_id, bk_pe_id, pn_id, kill_app);
    kill_pe_event->set_resource(resource);
    event_manager->AddEvent(EventPtr(kill_pe_event));
}

typedef list<ProcessorElement> PeList;
int32_t KillApp(EventManager* event_manager, const Application& app) {
    DSTREAM_INFO("kill app [%lu]", app.id().id());
    int res = error::OK;
    MetaManager* meta = MetaManager::GetMetaManager();
    if (NULL == meta || NULL == g_config) {
        DSTREAM_WARN("meta is not init");
        return error::META_INVALID;
    }
    ClusterConfig cluster_config;
    if ((res = meta->GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("Get ClusterConfig fail");
        return res;
    }
    if (app.status() == KILL) {
        PeList app_pe_list;
        if (meta->GetAppProcessElements(app.id(), &app_pe_list) == error::OK) {
            /* PeList::iterator pe_end = app_pe_list.end();
            for (PeList::iterator pe_it = app_pe_list.begin();
              pe_it != pe_end; ++pe_it) {
                for (int i = 0; i < pe_it->backups_size(); ++i) {
                  BackupPE* bk_pe = pe_it->mutable_backups(i);
                  if (bk_pe->has_pn_id()) {
                    AddKillPEEvent(event_manager, app.id(), pe_it->pe_id(),
                                   pe_it->resource(), bk_pe->backup_pe_id(),
                                   bk_pe->pn_id(), true);
                  }
                }
            }*/
            if ((res = ClearPE(meta, app.id(), app_pe_list, event_manager)) != error::OK) {
                DSTREAM_WARN("Clear PE fail");
                return res;
            }
        }
        std::string root_path = GetAppRootPath(cluster_config, app);
        if ((res = RemoveAppDir(root_path)) != error::OK) {
            DSTREAM_WARN("Remove App path [%s] fail", root_path.c_str());
            return res;
        }
        if ((res = meta->DelScribeImporter(app.name())) != error::OK) {
            DSTREAM_WARN("Remove App [%s] Scribe dir fail", app.name().c_str());
            // return res;
        }
        DSTREAM_INFO("delete app [%lu]", app.id().id());
        return meta->DeleteApplication(app.id());
    }
    return res;
}

int32_t AddSubTree(const Application& app, EventManager* event_manager,
                   bool b_start /*= false*/) {
    int32_t res = error::OK;
    ClusterConfig cluster_config;
    MetaManager* meta = MetaManager::GetMetaManager();
    if ((res = meta->GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("get cluster config fail");
        return res;
    }
    PeList app_pe_list, meta_pe_list, diff_pe_list;
    if (!CreateAppPEInstance(app, cluster_config, g_config, &app_pe_list)) {
        DSTREAM_WARN("create app [%lu] pe instance fail", app.id().id());
        return error::ADD_APP_FAIL;
    }
    if ((res = meta->GetAppProcessElements(app.id(), &meta_pe_list)) != error::OK) {
        DSTREAM_WARN("get app [%lu] pe list fail", app.id().id());
        return res;
    }
    GetPEDifference(app_pe_list, meta_pe_list, &diff_pe_list);
    if (diff_pe_list.empty()) {
        return res;
    }
    if ((res = meta->AddAppProcessElements(app.id(), diff_pe_list)) != error::OK) {
        DSTREAM_WARN("add app [%lu] pe list fail", app.id().id());
        return res;
    }
    event_manager->AddEvent(EventPtr(new AddAppEvent(app.id(), !b_start)));
    return res;
}

int32_t DelSubTree(const Application& app, EventManager* event_manager,
                   Scheduler* scheduler /*= NULL*/) {
    DSTREAM_DEBUG("del app [%lu] sub tree", app.id().id());
    int res = error::OK;
    ClusterConfig cluster_config;
    MetaManager* meta = MetaManager::GetMetaManager();
    if ((res = meta->GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("get cluster config fail");
        return res;
    }
    PeList app_pe_list, meta_pe_list, diff_pe_list;
    if (!CreateAppPEInstance(app, cluster_config, g_config, &app_pe_list)) {
        DSTREAM_WARN("create app [%lu] pe instance fail", app.id().id());
        return error::ADD_APP_FAIL;
    }
    if ((res = meta->GetAppProcessElements(app.id(), &meta_pe_list)) != error::OK) {
        DSTREAM_WARN("get app [%lu] pe list fail", app.id().id());
        return res;
    }
    GetPEDifference(meta_pe_list, app_pe_list, &diff_pe_list);
    DSTREAM_INFO("app [%lu] delete pe num [%zd]", app.id().id(), diff_pe_list.size());
    if (diff_pe_list.empty()) {
        return res;
    }
    PidList pid_list;
    // remove files
    if (diff_pe_list.size() > 0) {
        PeList::iterator pe_end = diff_pe_list.end();
        std::set<std::string> rm_processor_set;
        for (PeList::iterator it = diff_pe_list.begin();
             it != diff_pe_list.end(); ++it) {
            rm_processor_set.insert(it->processor_name());
            pid_list.push_back(ConstructProcessorID(it->pe_id()));
        }
        /*    PeList::iterator pe_end = diff_pe_list.end();
        std::set<std::string> rm_processor_set;
        for (PeList::iterator it = diff_pe_list.begin();
                               it != diff_pe_list.end(); ++it) {
          rm_processor_set.insert(it->processor_name());
          pid_list.push_back(ConstructProcessorID(it->pe_id()));
          for (int i = 0; i < it->backups_size(); ++i) {
            const BackupPE& bk_pe = it->backups(i);
            if (bk_pe.has_pn_id() && NULL != event_manager) {
              AddKillPEEvent(event_manager, app.id(), it->pe_id(), it->resource(),
                             bk_pe.backup_pe_id(), bk_pe.pn_id(), true);
            }
          }
        }*/
        if ((res = ClearPE(meta, app.id(), diff_pe_list, event_manager)) != error::OK) {
            DSTREAM_WARN("Clear PE fail");
        }
        std::string app_path = GetAppRootPath(cluster_config, app) + "app/";
        res = error::OK;
        for (std::set<std::string>::iterator it = rm_processor_set.begin();
             it != rm_processor_set.end(); ++it) {
            if (RemoveAppDir(app_path + *it + "*") != error::OK && res == error::OK) {
                res = error::REMOVE_FILE_FAIL;
            }
        }
        if (res != error::OK) {
            DSTREAM_WARN("Remove processor files fail");
            return res;
        }
    }
    if ((res = meta->DeletePEList(app.id(), diff_pe_list)) != error::OK) {
        DSTREAM_WARN("delete app [%lu] pe list fail", app.id().id());
        return res;
    }
    if (NULL != scheduler) {
        scheduler->RemoveResRequest(pid_list);
    }
    return error::OK;
}

int32_t UpdatePara(const Application& app, EventManager* event_manager,
                   Scheduler* scheduler /*= NULL*/) {
    int32_t ret = error::OK;
    ClusterConfig cluster_config;
    MetaManager* meta = MetaManager::GetMetaManager();
    if ((ret = meta->GetClusterConfig(&cluster_config)) != error::OK) {
        DSTREAM_WARN("get cluster config fail");
        return ret;
    }
    PeList app_pe_list, meta_pe_list, add_list, delete_list;
    if (!CreateAppPEInstance(app, cluster_config, g_config, &app_pe_list)) {
        DSTREAM_WARN("create app [%lu] pe instance fail", app.id().id());
        return error::ADD_APP_FAIL;
    }
    if ((ret = meta->GetAppProcessElements(app.id(), &meta_pe_list)) != error::OK) {
        DSTREAM_WARN("get app [%lu] pe list fail", app.id().id());
        return ret;
    }

    GetPEDifference(app_pe_list, meta_pe_list, &add_list);
    DSTREAM_INFO("app [%lu] add pe num [%zd]", app.id().id(), add_list.size());
    GetPEDifference(meta_pe_list, app_pe_list, &delete_list);
    DSTREAM_INFO("app [%lu] delete pe num [%zd]", app.id().id(), delete_list.size());

    // add new pe
    if (add_list.size() > 0) {
        if ((ret = meta->AddAppProcessElements(app.id(), add_list)) != error::OK) {
            DSTREAM_WARN("add app [%lu] pe list fail", app.id().id());
            return ret;
        }
        event_manager->AddEvent(EventPtr(new AddAppEvent(app.id(), 0)));
    }

    // delete old pe
    PidList pid_list;
    if (delete_list.size() > 0) {
        PeList::iterator pe_end = delete_list.end();
        for (PeList::iterator it = delete_list.begin();
             it != delete_list.end(); ++it) {
            pid_list.push_back(ConstructProcessorID(it->pe_id()));
        }
        /*    PeList::iterator pe_end = delete_list.end();
        for (PeList::iterator it = delete_list.begin();
                               it != delete_list.end(); ++it) {
          pid_list.push_back(ConstructProcessorID(it->pe_id()));
          for (int i = 0; i < it->backups_size(); ++i) {
            const BackupPE& bk_pe = it->backups(i);
            if (bk_pe.has_pn_id() && NULL != event_manager) {
              AddKillPEEvent(event_manager, app.id(), it->pe_id(), it->resource(),
                             bk_pe.backup_pe_id(), bk_pe.pn_id(), true);
            }
          }
        }*/
        if ((ret = ClearPE(meta, app.id(), delete_list, event_manager)) != error::OK) {
            DSTREAM_WARN("Clear PE fail");
        }
    }
    if ((ret = meta->DeletePEList(app.id(), delete_list)) != error::OK) {
        DSTREAM_WARN("delete app [%lu] pe list fail", app.id().id());
        return ret;
    }
    if (NULL != scheduler) {
        scheduler->RemoveResRequest(pid_list);
    }

    return ret;
}

int32_t ClearPE(MetaManager* meta, const AppID appid,
                PeList clear_pe, EventManager* event_manager) {
    int32_t ret = error::OK;

    PeList::iterator pe_end = clear_pe.end();
    for (PeList::iterator it = clear_pe.begin();
         it != clear_pe.end(); ++it) {
        for (int i = 0; i < it->backups_size(); ++i) {
            const BackupPE& bk_pe = it->backups(i);
            if (bk_pe.has_pn_id()) {
                PNID backup_pn_id = bk_pe.pn_id();
                it->mutable_backups(i)->clear_pn_id();
                if ((ret = meta->UpdateProcessElement(appid, *it)) != error::OK) {
                    DSTREAM_WARN("Update ProcessElement [%lu] fail", it->pe_id().id());
                    return ret;
                }
                if (NULL != event_manager) {
                    AddKillPEEvent(event_manager, appid, it->pe_id(), it->resource(),
                                   bk_pe.backup_pe_id(), backup_pn_id, true);
                }
            }
        }
    }
    return ret;
}

void GetPEDifference(const PeList& large_list, const PeList& less_list, PeList* diff_list) {
    PeSet less_pe_set;
    PeList::const_iterator less_pe_end = less_list.end();
    for (PeList::const_iterator it = less_list.begin(); it != less_pe_end; ++it) {
        less_pe_set.insert(*it);
    }
    PeList::const_iterator large_pe_end = large_list.end();
    PeSet::const_iterator pe_end = less_pe_set.end();
    for (PeList::const_iterator it = large_list.begin(); it != large_pe_end; ++it) {
        if (less_pe_set.find(*it) == pe_end) {
            diff_list->push_back(*it);
        }
    }
}

int32_t GetAppByName(MetaManager* meta,
                     const std::string& app_name, Application& app) {
    list<Application> app_list;
    if (meta->GetApplicationList(&app_list) == error::OK) {
        typedef list<Application>::iterator AppListIter;
        AppListIter app_end = app_list.end();
        for (AppListIter app_it = app_list.begin();
             app_it != app_end; ++app_it) {
            if (app_it->name() == app_name) {
                app = *app_it;
                return error::OK;
            }
        }
        DSTREAM_WARN("cannot get app [%s]", app_name.c_str());
        return error::APP_NOT_EXIST;
    }
    return error::GET_ZK_DATA_FAIL;
}

} // namespace pm_common
} // namespace dstream
