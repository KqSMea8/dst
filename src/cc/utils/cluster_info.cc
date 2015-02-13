/** 
* @file     cluster_info.cc
* @brief    
* @author   konghui, konghui@baidu.com
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <stdio.h>
#include <map>
#include <string>

#include "common/config_const.h"
#include "common/logger.h"
#include "metamanager/zk_meta_manager.h"

using namespace dstream::config_const;
using namespace dstream;
using namespace dstream::zk_meta_manager;
using namespace std;

const char* kMonitorName = "ClusterMonitor";

void GetPrimaryPM(ZKMetaManager* meta) {
    assert(NULL != meta);
    PM primary_pm;
    if (meta->GetPM(&primary_pm) != error::OK) {
        DSTREAM_ERROR("[%s][PrimaryPM] Get PM fail", kMonitorName);
    } else {
        DSTREAM_INFO("[%s][PrimaryPM] Primary PM is %s", kMonitorName,
                     primary_pm.host().c_str());
    }
}

void GetBackupPM(ZKMetaManager* zk_meta) {
    assert(NULL != zk_meta);
    list<PM> backup_pm_list;
    if (zk_meta->GetBackupPM(&backup_pm_list) != error::OK) {
        DSTREAM_ERROR("[%s][BackupPM] Get BackupPM list fail", kMonitorName);
    } else {
        DSTREAM_INFO("[%s][BackupPM] BackupPM number %lu", kMonitorName,
                     backup_pm_list.size());
        for (list<PM>::iterator it = backup_pm_list.begin();
             it != backup_pm_list.end(); ++it) {
            DSTREAM_INFO("[%s][BackupPM][BackupPM] %s", kMonitorName, it->id().c_str());
        }
    }
}

void GetPNList(ZKMetaManager* zk_meta) {
    assert(NULL != zk_meta);
    list<PN> pn_list;
    if (zk_meta->GetPNList(&pn_list) != error::OK) {
        DSTREAM_ERROR("[%s][PNLIST] Get PN list fail", kMonitorName);
    } else {
        DSTREAM_INFO("[%s][PNLIST] PN number %ld", kMonitorName, pn_list.size());
        for (list<PN>::iterator it = pn_list.begin();
             it != pn_list.end(); ++it) {
            DSTREAM_INFO("[%s][PNLIST][PN] %s", kMonitorName, it->pn_id().id().c_str());
        }
    }
}

void GetClusterConfig(ZKMetaManager* meta) {
    assert(meta != NULL);
    ClusterConfig cluster_config;
    if (meta->GetClusterConfig(&cluster_config) != error::OK) {
        DSTREAM_ERROR("[%s][ClusterConfig] GetClusterConfig fail", kMonitorName);
    } else {
        if (cluster_config.has_min_host_percentage()) {
            DSTREAM_INFO("[%s][ClusterConfig] Active PN percentage %f", kMonitorName,
                         cluster_config.min_host_percentage());
        } else {
            DSTREAM_INFO("[%s][ClusterConfig] Active PN percentage %f", kMonitorName,
                         kMinActivePNPercentage);
        }
    }
}

void GetPEList(ZKMetaManager* meta) {
    assert(meta != NULL);
    list<Application> app_list;
    if (meta->GetApplicationList(&app_list) != error::OK) {
        DSTREAM_ERROR("[%s] GetAppList fail", kMonitorName);
    } else {
        list<Application>::iterator app_it, app_end;
        for (app_it = app_list.begin(), app_end = app_list.end();
             app_it != app_end; ++app_it) {
            if (app_it->status() != RUN) {
                continue;
            }
            list<ProcessorElement> pe_list;
            if (meta->GetAppProcessElements(app_it->id(), &pe_list) == error::OK &&
                pe_list.size() > 0) {
                list<ProcessorElement>::iterator pe_it, pe_end;
                for (pe_it = pe_list.begin(), pe_end = pe_list.end();
                     pe_it != pe_end; ++pe_it) {
                    const BackupPE& bk_pe = pe_it->backups(0);
                    if (!bk_pe.has_pn_id()) {
                        DSTREAM_INFO("[%s][GetPEList] App [%zu] PE [%zu] has not pn,"
                                     " last assignment time [%zu]",
                                     kMonitorName, app_it->id().id(), pe_it->pe_id().id(),
                                     bk_pe.last_assignment_time());
                    }
                }
            }
        }
    }
}

typedef void (*Func) (ZKMetaManager* /*zk*/);   // NOLINT

struct visit_info {
    bool visit;
    Func f;
};

int main(int argc, char** argv) {
    logger::initialize("ClusterLogMonitor");
    if (argc < 2) {
        DSTREAM_WARN("Usage:cluster_info cluster_file\n");
        exit(1);
    }
    config::Config config;
    if (config.ReadConfig(argv[1]) != error::OK) {
        DSTREAM_WARN("[%s]read config file %s fail\n", kMonitorName, argv[1]);
        exit(1);
    }
    ZKMetaManager zk_meta;
    if (!zk_meta.InitMetaManager(&config)) {
        DSTREAM_ERROR("[%s]init zk meta manager fail", kMonitorName);
        exit(1);
    }
    if (argc == 2) {
        GetClusterConfig(&zk_meta);
        GetPrimaryPM(&zk_meta);
        GetBackupPM(&zk_meta);
        GetPNList(&zk_meta);
        GetPEList(&zk_meta);
        return 0;
    }
    map<string, visit_info> m;
    visit_info v;
    v.visit = false;
    v.f = &GetClusterConfig;
    m["cluster_config"] = v;
    v.f = &GetPrimaryPM;
    m["primary_pm"] = v;
    v.f = &GetBackupPM;
    m["backup_pm"] = v;
    v.f = &GetPNList;
    m["pn"] = v;
    m["pe_information"] = v;
    v.f = &GetPEList;
    map<string, visit_info>::iterator m_end = m.end();
    for (int i = 2; i < argc; ++i) {
        map<string, visit_info>::iterator m_find = m.find(argv[i]);
        if (m_find != m_end && !m_find->second.visit) {
            m_find->second.visit = true;
            m_find->second.f(&zk_meta);
        }
    }
}
