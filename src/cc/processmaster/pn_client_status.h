/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/01
* @filename:    pn_client_status.h
* @author:      zhanggengxin@baidu.com
* @brief:       pn staus
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_PN__CLIENTSTATUS_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_PN__CLIENTSTATUS_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include "common/application_tools.h"
#include "common/error.h"
#include "common/dstream_type.h"
#include "processmaster/event_manager.h"
#include "processmaster/gc.h"
#include "scheduler/scheduler.h"

namespace dstream {
namespace pn_client_status {

/************************************************************************/
/* client information                                                   */
/************************************************************************/
struct client_info {
    User user;
    AppID app_id;
};

// defind PNID->PNReport map
typedef std::map<PNID, PNReport, PNIDCompare> PnidReportMap;
typedef PnidReportMap::iterator PnidReportMapIter;
typedef std::pair<PNID, PNReport> PnidReportPair;

// defind client->client_info information
typedef std::map<uint64_t, client_info> ClientIdMap;
typedef ClientIdMap::iterator ClientIdMapIter;
typedef std::pair<uint64_t, client_info> ClientIdPair;

/************************************************************************/
/* PNStaus : used for manager pn/client status                           */
/************************************************************************/
class PNClientStatus {
public:
    static PNClientStatus* GetPNClientStatus();
    static int32_t InitPNStatus(event_manager::EventManager* event_manager, pm_gc::GC* gc,
                                scheduler::Scheduler* scheduler);
    static void DestroyPNStatus();
    int32_t UpdatePNStatus(const PNReport* report);
    int32_t RemovePNStatus(const PNID& pn_id);
    int32_t AddClient(const User* user, uint64_t session_id);
    int32_t FindClient(uint64_t session_id, client_info* ci);
    int32_t UpdateClient(uint64_t session_id, const AppID& app_id);
    int32_t RemoveClient(const std::string& node);
    int32_t RemoveClient(uint64_t session_id);
    int32_t InitMap();
    void EraseMap(const PEID& pe_id, const PNID& pn_id);
    void ClearAll();
    PnidReportMap pn_reports();
    
    /** 
    * @brief    Dump method for httpd server
    * 
    * @param    output, output string for request
    * 
    * @return   errorCode
    * @author   liuguodong,liuguodong01@baidu.com
    * @date     2013-08-21
    */
    dstream::error::Code Dump(std::string* output);

private:
    PNClientStatus(event_manager::EventManager* event_manager,
                   pm_gc::GC* gc, scheduler::Scheduler* scheduler) :
        m_event_manager(event_manager), m_gc(gc), m_scheduler(scheduler) {}
    bool AddPNReport(const PNReport* report);
    static PNClientStatus* pn_status_instace;
    PnidReportMap m_pn_reports; 
    ClientIdMap m_clients;
    event_manager::EventManager* m_event_manager;
    pm_gc::GC* m_gc;
    scheduler::Scheduler* m_scheduler;
    application_tools::PnidPeSetMap m_pn_pe_map;
    MutexLock m_lock;
};

} // namespace pn_client_status
} // namespace dstream

#endif // NOLINT
