/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    pm_common.h
* @author:      zhanggengxin@baidu.com
* @brief:       common functions used for pm
*/

#ifndef __DSTREAM_PROCESSMASTER_PM_COMMON_H__ // NOLINT
#define __DSTREAM_PROCESSMASTER_PM_COMMON_H__ // NOLINT

#include <list>
#include <set>
#include <string>
#include "common/proto/application.pb.h"
#include "common/proto/pn.pb.h"
#include "common/zk_client.h"
#include "metamanager/meta_manager.h"
#include "processmaster/event_manager.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace pm_common {

// Add Create PE Event
// if check_replicate is true it will check all replicate
bool AddCreatePEEvent(const Application& app,
                      event_manager::EventManager& event_manager,
                      bool check_replicate = false);

// Add PE Fail event
bool AddPEFailEvent(event_manager::EventManager* event_manager,
                    const ProcessorElement& pe,
                    const BackupPEID& bk_pe_id,
                    const PNID& pn_id);

// Add Kill PE Event
void AddKillPEEvent(event_manager::EventManager* event_manager, const AppID& app_id,
                    const PEID& pe_id, const Resource& resource,
                    const BackupPEID& bk_pe_id, const PNID& pn_id,
                    bool kill_app = false);

// kill app
int32_t KillApp(event_manager::EventManager* event_manager, const Application& app);

// Create ZKClient
int32_t CreateZK(const config::Config* config, auto_ptr::AutoPtr<zk_client::ZkClient>* zk_client);

int32_t AddSubTree(const Application& app, event_manager::EventManager* event_manager,
                   bool b_start = false);
// delete sub tree and remove processor files
int32_t DelSubTree(const Application& app, event_manager::EventManager* event_manager,
                   scheduler::Scheduler* scheduler = NULL);
int32_t UpdatePara(const Application& app, event_manager::EventManager* event_manager,
                   scheduler::Scheduler* scheduler = NULL);

// common function for clear pe
int32_t ClearPE(meta_manager::MetaManager* meta,
                const AppID appid, PE_LIST clear_pe,
                event_manager::EventManager* event_manager);

typedef list<ProcessorElement> PeList;
typedef PeList::iterator PeListIter;
void GetPEDifference(const PeList& large_list, const PeList& less_list, PeList* diff_list);

int32_t GetAppByName(meta_manager::MetaManager* meta, const string& app_name, Application& app);

// global config
extern config::Config* g_config;

} // namespace pm_common
} // namespace dstream

#endif // NOLINT
