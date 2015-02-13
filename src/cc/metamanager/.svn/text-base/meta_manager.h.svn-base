/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief MetaManager for dstream
 * @Usage:if(MetaManager::InitMetaManager(config)){meta = MetaManager.GetMetaManager();}
 */

#ifndef __DSTREAM_CC_METAMANAGER_META_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_METAMANAGER_META_MANAGER_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <string>

#include "common/application_tools.h"
#include "common/config.h"
#include "common/error.h"
#include "common/mutex.h"
#include "common/proto/application.pb.h"
#include "common/proto/cluster_config.pb.h"
#include "common/proto/pm.pb.h"

using std::list;
using std::string;
using std::set;
using dstream::config::Config;

typedef list<dstream::PN> PN_LIST;
typedef PN_LIST::iterator PN_LIST_ITER;
typedef list<dstream::ProcessorElement> PE_LIST;

struct GroupInfoCompare {
    inline bool operator() (const dstream::GroupInfo& left, const dstream::GroupInfo& right) const {
        return left.group_name() < right.group_name();
    }
};

#define ERROR_CODE dstream::error::Code
namespace dstream {
namespace meta_manager {
extern const uint64_t kMaxAppNum;

/************************************************************************/
/* MetaManager interface : define meta operation                        */
/************************************************************************/
class MetaManager {
public:
    MetaManager();
    virtual ~MetaManager() {}
    virtual bool InitMetaManager(const Config* config) = 0;

    // ClusterConfig interface
    virtual ERROR_CODE GetClusterConfig(ClusterConfig* config) = 0;
    virtual ERROR_CODE UpdateClusterConfig(const ClusterConfig& config) = 0;
    ERROR_CODE CheckUser(const User& add_user);
    // update users : add user if user not exist
    // modify user password if user exist and password does not equal
    ERROR_CODE UpdateUser(const User& user);

    // Application interface
    virtual ERROR_CODE AddApplication(Application* app);
    virtual ERROR_CODE InsertProcessElement(const Application& update_app);
    virtual ERROR_CODE UpdateApplication(const Application& app) = 0;
    virtual ERROR_CODE AddAppProcessElements(const AppID& app,
                                             const PE_LIST& pe_list) = 0;
    virtual ERROR_CODE UpdateProcessElement(const AppID& app_id,
                                            const ProcessorElement& pe) = 0;
    virtual ERROR_CODE DeletePEList(const AppID& app_id, const PE_LIST& pe_list) = 0;
    virtual ERROR_CODE DeleteApplication(const AppID& app_id) = 0;
    virtual ERROR_CODE GetApplicationList(list<Application>* app_list) = 0;
    virtual ERROR_CODE GetAppIDList(list<AppID>* id_list) = 0;
    virtual ERROR_CODE GetApplication(const AppID& app_id, Application* app) = 0;
    virtual ERROR_CODE GetAppProcessElements(const AppID& app_id, PE_LIST* pe_list) = 0;
    virtual ERROR_CODE GetProcessorPEs(const AppID& app_id,
                                       const ProcessorID& processor_id,
                                       list<ProcessorElement>* pe_list);
    virtual ERROR_CODE GetAppFlowControl(const AppID& app_id,
            const std::string& processor, FlowControl* flow_control) = 0;
    /************************************************************************/
    /* Get processments on pn including under constuct pe                   */
    /************************************************************************/
    virtual ERROR_CODE GetPNProcessElementsMap(application_tools::PnidPeSetMap* pn_pe_map) = 0;
    virtual ERROR_CODE GetPNProcessElements(const PNID& pn_id,
                                            set<ProcessorElement,
                                            application_tools::ComparePE>* pe_list) = 0;
    virtual ERROR_CODE GetProcessorElement(const AppID& app_id, const PEID& pe_id,
                                           ProcessorElement* pe) = 0;
    // Get All Running pe list
    virtual ERROR_CODE GetAllProcessElements(list<ProcessorElement>* all_pes);

    // PM interface
    virtual ERROR_CODE GetPM(PM* pm) = 0;
    virtual ERROR_CODE AddPM(const PM& pm) = 0;
    virtual ERROR_CODE AddBackupPM(PM* pm) = 0;
    virtual ERROR_CODE GetBackupPM(PM* /*pm*/) {
        return error::OK;
    }
    virtual ERROR_CODE GetBackupPM(list<PM>* backup_pm_list) = 0;
    virtual ERROR_CODE DeletePM() = 0;
    virtual ERROR_CODE DeleteBackupPM(const PM& pm) = 0;
    virtual ERROR_CODE UpdatePM(const PM& pm) = 0;
    virtual ERROR_CODE UpdatePMMachines(const PMMachines& /*pm_machines*/) {
        return error::OK;
    }
    virtual ERROR_CODE GetPMMachines(PMMachines* /*pm_machines*/) {
        return error::OK;
    }
    virtual ERROR_CODE DeletePMMachines() {
        return error::OK;
    }

    // PN interface
    virtual ERROR_CODE GetPN(const PNID& pn_id, PN* pn) = 0;
    virtual ERROR_CODE GetPNList(list<PN>* pn_list) = 0;
    virtual ERROR_CODE AddPN(const PN& pn) = 0;
    virtual ERROR_CODE UpdatePN(const PN& pn) = 0;
    virtual ERROR_CODE DeletePN(const PN& pn) = 0;
    virtual ERROR_CODE GetNewAppID(AppID* app_id);
    virtual ERROR_CODE UpdatePNMachines(const PNMachines& /*pn_machines*/) {
        return error::OK;
    }
    virtual ERROR_CODE GetPNMachines(PNMachines* /*pn_machines*/) {
        return error::OK;
    }
    virtual ERROR_CODE DeletePNMachines() {
        return error::OK;
    }
    // PN Groups
    virtual ERROR_CODE UpdatePNGroups(const PNGroups& pn_group) {
        return error::OK;
    }
    virtual ERROR_CODE GetPNGroups(PNGroups* pn_group) {
        return error::OK;
    }
    virtual ERROR_CODE UpdateGroupInfo(const GroupInfo& group_info) {
        return error::OK;
    }
    virtual ERROR_CODE GetGroupInfo(const std::string& group_name, GroupInfo* group_info) {
        return error::OK;
    }
    virtual ERROR_CODE GetGroupNameList(std::list<std::string>* group_name_list) {
        return error::OK;
    }
    virtual ERROR_CODE GetGroupList(std::list<GroupInfo>* group_list) {
        return error::OK;
    }
    virtual ERROR_CODE AddGroupInfo(const GroupInfo& group_info) {
        return error::OK;
    }
    virtual ERROR_CODE DeleteGroupInfo(const GroupInfo& group_info) {
        return error::OK;
    }
    virtual ERROR_CODE DeletePNGroups() {
        return error::OK;
    }
    // SubPoint
    virtual ERROR_CODE AddSubPoint(const std::string& path, const SubPoint& sub_point) {
        return error::OK;
    }
    virtual ERROR_CODE DelSubPoint(const std::string& path, const SubPoint& sub_point) {
        return error::OK;
    }
    virtual ERROR_CODE GetSubPoint(const std::string& path, SubPoint* sub_point) {
        return error::OK;
    }
    virtual ERROR_CODE UpdateSubPoint(const std::string& path, const SubPoint& sub_point) {
        return error::OK;
    }
    virtual ERROR_CODE DelAppSubPoint(const std::string& path) {
        return error::OK;
    }
    virtual ERROR_CODE AddScribeAppNode(const std::string& path,
        const std::string& ip_port) {
        return error::OK;
    }
    virtual ERROR_CODE AddScribePENode(const std::string& path,
            const std::string& ip_port) {
        return error::OK;
    }
    virtual ERROR_CODE DelScribeImporter(const std::string& path) {
        return error::OK;
    }

    static MetaManager* GetMetaManager();
    // init matamanager context : create default metamanager
    static bool InitMetaManagerContext(const Config* config);
    static void DestoryMetaManager();

protected:
    bool CheckPN(const PN& pn);
    bool CheckPM(const PM& pm);
    virtual ERROR_CODE SaveApplication(const Application& app) = 0;
    MutexLock m_lock;
    AppID m_current_max_id;
    const Config* m_config;

private:
    static MetaManager* instance;
};

} // namespace meta_manager
} // namespace dstream
#endif // NOLINT
