/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/06
* @filename:    memory_meta_manager.h
* @author:      zhanggengxin
* @brief:       memory meta data manager
*/

#ifndef __DSTREAM_CC_METAMANAGER_MEMORY_META_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_METAMANAGER_MEMORY_META_MANAGER_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <utility>

#include "common/dstream_type.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace memory_meta_manager {

class CompareApp {
public:
    bool operator() (const Application& app1, const Application& app2) {
        return app1.id().id() < app2.id().id();
    }
};

class CompareAppID {
public:
    bool operator()(const AppID& app_id1, const AppID& app_id2) {
        return app_id1.id() < app_id2.id();
    }
};

class ComparePN {
public:
    bool operator()(const PN& pn1, const PN& pn2) {
        return pn1.pn_id().id() < pn2.pn_id().id();
    }
};

class MemoryMetaManager : public meta_manager::MetaManager {
public:
    MemoryMetaManager();
    ~MemoryMetaManager();
    bool InitMetaManager(const Config* /*config*/) {
        return true;
    }

    ERROR_CODE GetClusterConfig(ClusterConfig* config);
    ERROR_CODE UpdateClusterConfig(const ClusterConfig& config);

    ERROR_CODE AddAppProcessElements(const AppID& app, const list<ProcessorElement>& pe_list);
    ERROR_CODE UpdateApplication(const Application& app);
    ERROR_CODE UpdateProcessElement(const AppID& app_id, const ProcessorElement& pe);
    ERROR_CODE DeleteApplication(const AppID& app);
    ERROR_CODE GetApplicationList(std::list<Application>* app_list);
    ERROR_CODE GetAppIDList(std::list<AppID>* id_list);
    ERROR_CODE GetApplication(const AppID& app_id, Application* app);
    ERROR_CODE GetAppProcessElements(const AppID& app_id, std::list<ProcessorElement>* pe_list);
    ERROR_CODE GetAppFlowControl(const AppID& app_id,
            const std::string& processor, FlowControl* flow_control);
    ERROR_CODE GetPNProcessElements(const PNID& pn_id,
                                    set<ProcessorElement, application_tools::ComparePE>* pe_list);
    ERROR_CODE GetPNProcessElementsMap(PNID_PESET_MAP* pn_pe_map);
    ERROR_CODE GetProcessorElement(const AppID& app_id, const PEID& pe_id, ProcessorElement* pe);
    ERROR_CODE DeletePEList(const AppID& app_id, const PE_LIST& pe_list) {
        DSTREAM_UNUSED_VAL(app_id);
        DSTREAM_UNUSED_VAL(pe_list);
        return error::OK;
    }

    ERROR_CODE GetPM(PM* pm);
    ERROR_CODE AddPM(const PM& pm);
    ERROR_CODE AddBackupPM(PM* pm);
    ERROR_CODE GetBackupPM(list<PM>* backup_pm_list);
    ERROR_CODE DeleteBackupPM(const PM& pm);
    ERROR_CODE UpdatePM(const PM& pm);
    ERROR_CODE DeletePM();

    ERROR_CODE GetPN(const PNID& pn_id, PN& pn);
    ERROR_CODE GetPNList(list<PN>* pn_list);
    ERROR_CODE AddPN(const PN& pn);
    ERROR_CODE UpdatePN(const PN& pn);
    ERROR_CODE DeletePN(const PN& pn);

protected:
    ERROR_CODE SaveApplication(const Application& app);

private:
    // App set type
    typedef std::set<Application, CompareApp> APP_SET;
    typedef APP_SET::iterator APP_SET_ITER;
    typedef std::pair<APP_SET_ITER, bool> APP_INSERT_RES;
    // PN set type
    typedef std::set<PN, ComparePN> PN_SET;
    typedef PN_SET::iterator PN_SET_ITER;
    typedef std::pair<PN_SET_ITER, bool> PN_INSERT_RES;
    // PEAssignment type
    typedef std::set<ProcessorElement, application_tools::ComparePE> PE_SET;
    typedef PE_SET::iterator PE_SET_ITER;
    typedef std::pair<PE_SET_ITER, bool> PE_INSERT_RES;
    typedef std::map<AppID, PE_SET, CompareAppID> APP_PE_MAP;
    typedef APP_PE_MAP::iterator APP_PE_MAP_ITER;
    typedef std::pair<AppID, PE_SET> APP_PE_PAIR;
    typedef std::pair<APP_PE_MAP_ITER, bool> APP_PE_INSERT_RES;

    APP_SET m_app_set;
    auto_ptr::AutoPtr<ClusterConfig> m_cluster_config;
    PN_SET m_pn_set;
    auto_ptr::AutoPtr<PM> m_pm;
    APP_PE_MAP m_app_pe_map;
    list<PM> m_backup_pm_list;
    int m_backup_pm_serial;
};
} // namespace memory_meta_manager
} // namespace dstream

#endif // NOLINT
