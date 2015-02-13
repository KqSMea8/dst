/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief  meta manager using zookeeper
 */

#include "metamanager/zk_meta_manager.h"

#include <cstddef>

#include "common/logger.h"
#include "metamanager/zk_config.h"

namespace dstream {
namespace zk_meta_manager {
using namespace zk_config;
using namespace application_tools;

#define CHECK_ZK_CLIENT(zk_client)              \
    if (!zk_client.PtrValid()) {                   \
        DSTREAM_WARN("zk client is not init");       \
        return error::CONNECT_ZK_FAIL;               \
    }                                              \

MetaZKData::MetaZKData() {
    m_buf = NULL;
    m_len = 0;
}

MetaZKData::MetaZKData(const MetaZKData& d)
    : zk_client::Data(d) {
    if (this != &d && d.m_len) {
        RELEASEARRAY(m_buf);
        AllocateBuf(d.m_len);
        memcpy(m_buf, d.m_buf, d.m_len);
    }
}

MetaZKData& MetaZKData::operator =(const MetaZKData& d) {
    if (this != &d && d.m_len) {
        RELEASEARRAY(m_buf);
        AllocateBuf(d.m_len);
        memcpy(m_buf, d.m_buf, d.m_len);
    }
    return *this;
}

MetaZKData::~MetaZKData() {
    RELEASEARRAY(m_buf);
}

bool MetaZKData::AllocateBuf(int size) {
    m_len = size;
    return NULL != (m_buf = new (std::nothrow) char[size]);
}

MetaZKNode::MetaZKNode() {
    path.m_buf = data.m_buf = NULL;
    path.m_len = data.m_len = 0;
}

MetaZKNode::~MetaZKNode() {
    RELEASEARRAY(path.m_buf);
    RELEASEARRAY(data.m_buf);
}

bool MetaZKNode::AllocateBuf(int path_size, int data_size) {
    path.m_len = path_size;
    data.m_len = data_size;
    if (NULL != (path.m_buf = new (std::nothrow) char[path_size])) {
        data.m_buf = new (std::nothrow) char[data_size];
        if (NULL == data.m_buf) {
            RELEASEARRAY(path.m_buf);
            return false;
        }
    } else {
        return false;
    }
    return true;
}

bool ZKMetaManager::InitMetaManager(const Config* config) {
    if (zk_client_.PtrValid() && zk_client_->IsConnected()) {
        DSTREAM_WARN("zk client is connected");
        if (!InitMaxAppID()) {
            DSTREAM_WARN("Init Max AppID fail");
            return false;
        }
        return true;
    }
    ZKConfig zk_config(config);
    if (NULL != config) {
        m_root_path = zk_config.GetSysPath();
        m_app_root_path = zk_config.AppRootPath();
        m_cluster_config_path = zk_config.ClusterConfigPath();
        m_path = zk_config.PMPath();
        m_pn_root_path = zk_config.PNRootPath();
        m_zk_addr = zk_config.GetZKPath();
        m_backup_pm_root = zk_config.BackupPMRoot();
        m_pn_group_root_path = zk_config.PNGroupRootPath();
        m_sub_point_root_path = zk_config.SubPointRootPath();
        m_scribe_root_path = zk_config.ScribeRootPath();
        if (m_zk_addr == "") {
            DSTREAM_WARN("no zk address config");
            return false;
        }
        m_zk_timeout = zk_config.Timeout();
    } else {
        DSTREAM_WARN("config fail");
        return false;
    }
    zk_client_ = auto_ptr::AutoPtr<zk_client::ZkClient>(new (std::nothrow) zk_client::ZkClient(
                 m_zk_addr.c_str(), m_zk_timeout));
    if (!zk_client_.PtrValid()) {
        DSTREAM_WARN("create zk client fail");
        return false;
    }
    // To do
    if (zk_client_->Connect() == error::OK) {
        if (zk_client_->InitSystemRoot(m_backup_pm_root.c_str()) != error::OK) {
            DSTREAM_WARN("Init BackupPM Path %s fail", m_backup_pm_root.c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(m_app_root_path.c_str()) != error::OK) {
            DSTREAM_WARN("init app root path %s fail", m_app_root_path.c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(m_cluster_config_path.c_str()) != error::OK) {
            DSTREAM_WARN("init cluster config %s fail", m_cluster_config_path.c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(m_pn_root_path.c_str()) != error::OK) {
            DSTREAM_WARN("init pn root path %s fail", m_path.c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(zk_config.ClientRootPath().c_str()) != error::OK) {
            DSTREAM_WARN("init client root path %s fail", zk_config.ClientRootPath().c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(machine_root_path().c_str()) != error::OK) {
            DSTREAM_WARN("init machine root path [%s] fail", machine_root_path().c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(pn_group_root_path().c_str()) != error::OK) {
            DSTREAM_WARN("init pn group root path [%s] fail", pn_group_root_path().c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(sub_point_root_path().c_str()) != error::OK) {
            DSTREAM_WARN("init sub point root path [%s] fail", sub_point_root_path().c_str());
            return false;
        }
        if (zk_client_->InitSystemRoot(scribe_root_path().c_str()) != error::OK) {
            DSTREAM_WARN("init scribe root path [%s] fail", scribe_root_path().c_str());
            return false;
        }

        if (!InitMaxAppID()) {
            DSTREAM_WARN("InitMax AppID fail");
            return false;
        }
        m_config = config;
        return true;
    }
    return false;
}

bool ZKMetaManager::InitMaxAppID() {
    std::list<AppID> app_id_list;
    int32_t res = error::OK;
    if ((res = GetAppIDList(&app_id_list)) == error::OK) {
        for (std::list<AppID>::iterator app_id_iter = app_id_list.begin();
             app_id_iter != app_id_list.end();
             ++app_id_iter) {
            if (app_id_iter->id() > m_current_max_id.id()) {
                m_current_max_id.set_id(app_id_iter->id());
            }
        }
    } else {
        if (res == error::ZK_NO_NODE) {
            m_current_max_id.set_id(1);
            return true;
        }
        DSTREAM_WARN("get app list fail");
        return false;
    }
    return true;
}

ERROR_CODE ZKMetaManager::GetClusterConfig(ClusterConfig* config) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, m_cluster_config_path, config);
}

ERROR_CODE ZKMetaManager::UpdateClusterConfig(const ClusterConfig& config) {
    CHECK_ZK_CLIENT(zk_client_);
    return SetZKData(zk_client_, m_cluster_config_path, config);
}

ERROR_CODE ZKMetaManager::UpdateApplication(const Application& app) {
    CHECK_ZK_CLIENT(zk_client_);
    return SetZKData(zk_client_, GetAppPath(app.id()), app);
}

typedef const list<ProcessorElement> CONST_PE_LIST;
typedef CONST_PE_LIST::const_iterator CONST_PE_LIST_ITER;
ERROR_CODE ZKMetaManager::AddAppProcessElements(const AppID& app_id,
                                                const list<ProcessorElement>& pe_list) {
    CHECK_ZK_CLIENT(zk_client_);
    DSTREAM_DEBUG("App [%lu] pe size [%lu]", app_id.id(), pe_list.size());
    CONST_PE_LIST_ITER pe_end = pe_list.end();
    for (CONST_PE_LIST_ITER pe_iter = pe_list.begin();
         pe_iter != pe_end; ++pe_iter) {
        if (AddZKNode(GetPEPath(app_id, pe_iter->pe_id()),
                      &(*pe_iter), true) != error::OK) {
            DSTREAM_WARN("Add App[%lu] pe[%lu] fail", app_id.id(),
                         pe_iter->pe_id().id());
            zk_client_->DeleteNodeTree(GetAppPath(app_id).c_str());
            return error::ADD_PE_FAIL;
        }
    }
    return error::OK;
}

ERROR_CODE ZKMetaManager::UpdateProcessElement(const AppID& app_id,
                                               const ProcessorElement& pe) {
    CHECK_ZK_CLIENT(zk_client_);
    return SetZKData(zk_client_, GetPEPath(app_id, pe.pe_id()).c_str(), pe);
}

ERROR_CODE ZKMetaManager::GetApplicationList(list<Application>* app_list) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKChildData(zk_client_, m_app_root_path, app_list);
}

ERROR_CODE ZKMetaManager::GetAppIDList(list<AppID>* id_list) {
    CHECK_ZK_CLIENT(zk_client_);
    // To update
    list<Application> apps;
    ERROR_CODE res = GetApplicationList(&apps);
    if (res == error::OK) {
        list<Application>::iterator app_end = apps.end();
        for (list<Application>::iterator app_iter = apps.begin();
             app_iter != app_end; ++app_iter) {
            id_list->push_back(app_iter->id());
        }
    }
    return res;
}

ERROR_CODE ZKMetaManager::GetApplication(const AppID& app_id, Application* app) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetAppPath(app_id).c_str(), app);
}

ERROR_CODE ZKMetaManager::GetAppProcessElements(const AppID& app_id,
                                                list<ProcessorElement>* pe_list) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKChildData(zk_client_, GetAppPath(app_id), pe_list);
}

ERROR_CODE ZKMetaManager::GetAppFlowControl(const AppID& app_id,
        const std::string& processor, FlowControl* flow_control) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE res = error::OK;

    Application app;
    if ((res = GetApplication(app_id, &app)) < error::OK) {
        return res;
    }

    int idx = 0;
    for (; idx < app.topology().processors_size(); ++idx) {
        if (app.topology().processors(idx).name() == processor) {
            *flow_control = app.topology().processors(idx).flow_control();
            break;
        }
    }

    // not found
    if (idx == app.topology().processors_size()) {
        flow_control->Clear();
        return error::PROCESSOR_NOT_EXIST;
    }
    return error::OK;
}

ERROR_CODE ZKMetaManager::GetPNProcessElements(const PNID& pn_id,
                                               set<ProcessorElement,
                                               application_tools::ComparePE>* pe_set) {
    CHECK_ZK_CLIENT(zk_client_);
    typedef list<ProcessorElement> PE_LIST;
    typedef PE_LIST::iterator PE_LIST_ITER;
    list<AppID> app_id_list;
    ERROR_CODE res = error::OK;
    if ((res = GetAppIDList(&app_id_list)) != error::OK) {
        DSTREAM_WARN("get app list fail");
        return res;
    }
    list<AppID>::iterator app_id_end = app_id_list.end();
    list<ProcessorElement> pe_list;
    for (list<AppID>::iterator app_id_iter = app_id_list.begin();
         app_id_iter != app_id_end; ++app_id_iter) {
        if ((res = GetAppProcessElements(*app_id_iter, &pe_list)) != error::OK) {
            return res;
        }
        PE_LIST_ITER pe_end = pe_list.end();
        for (PE_LIST_ITER pe_iter = pe_list.begin(); pe_iter != pe_end; ++pe_iter) {
            for (int i = 0; i < pe_iter->backups_size(); ++i) {
                if (pe_iter->backups(i).pn_id().id() == pn_id.id()) {
                    pe_set->insert(*pe_iter);
                    break;
                }
            }
        }
        pe_list.clear();
    }
    return error::OK;
}

ERROR_CODE ZKMetaManager::GetPNProcessElementsMap(PnidPeSetMap* pn_pe_map) {
    CHECK_ZK_CLIENT(zk_client_);
    typedef list<ProcessorElement> PE_LIST;
    typedef PE_LIST::iterator PE_LIST_ITER;
    list<AppID> app_id_list;
    ERROR_CODE res = error::OK;
    if ((res = GetAppIDList(&app_id_list)) != error::OK) {
        DSTREAM_WARN("get app list fail");
        return res;
    }
    list<AppID>::iterator app_id_end = app_id_list.end();
    list<ProcessorElement> pe_list;
    for (list<AppID>::iterator app_id_iter = app_id_list.begin();
         app_id_iter != app_id_end; ++app_id_iter) {
        if ((res = GetAppProcessElements(*app_id_iter, &pe_list)) != error::OK) {
            return res;
        }
        PE_LIST_ITER pe_end = pe_list.end();
        for (PE_LIST_ITER pe_iter = pe_list.begin(); pe_iter != pe_end; ++pe_iter) {
            for (int i = 0; i < pe_iter->backups_size(); ++i) {
                PNID pn_id = pe_iter->backups(i).pn_id();
                PnidPeSetMapIter pn_pe_it;
                if ((pn_pe_it = pn_pe_map->find(pn_id)) == pn_pe_map->end()) {
                    PeSet pe_set;
                    pe_set.insert(*pe_iter);
                    PnidPeSetMapPair pn_pe_pair(pn_id, pe_set);
                    pn_pe_map->insert(pn_pe_pair);
                } else {
                    pn_pe_it->second.insert(*pe_iter);
                }
            }
        }
        pe_list.clear();
    }
    return error::OK;
}

ERROR_CODE ZKMetaManager::GetProcessorElement(const AppID& app_id,
                                              const PEID& pe_id,
                                              ProcessorElement* pe) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetPEPath(app_id, pe_id), pe);
}

ERROR_CODE ZKMetaManager::GetPM(PM* pm) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, m_path, pm);
}

ERROR_CODE ZKMetaManager::AddPM(const PM& pm) {
    CHECK_ZK_CLIENT(zk_client_);
    return AddZKNode(m_path, &pm, false);
}

ERROR_CODE ZKMetaManager::AddBackupPM(PM* pm) {
    CHECK_ZK_CLIENT(zk_client_);
    std::string backup_pm_path;
    ERROR_CODE res = AddZKNode(GetBackupPMPath(*pm), pm, false, 1, &backup_pm_path);
    if (res == error::OK) {
        DSTREAM_DEBUG("[%s] AddBackupPM success, pm id [%s]", __FUNCTION__,
                      backup_pm_path.c_str());
        pm->set_id(backup_pm_path);
    } else {
        DSTREAM_WARN("[%s] AddBackupPM [%s] fail", __FUNCTION__,
                     pm->host().c_str());
    }
    return SetZKData(zk_client_, backup_pm_path, *pm);
}

ERROR_CODE ZKMetaManager::GetBackupPM(PM* backup_pm) {
    if (backup_pm->has_id()) {
        return GetZKData(zk_client_, backup_pm->id(), backup_pm);
    } else {
        DSTREAM_WARN("[%s] backup pm not set id", __FUNCTION__);
        return error::ZK_NO_NODE;
    }
    return error::OK;
}

ERROR_CODE ZKMetaManager::GetBackupPM(list<PM>* backup_pm_list) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKChildData(zk_client_, m_backup_pm_root, backup_pm_list);
}

ERROR_CODE ZKMetaManager::DeleteBackupPM(const PM& pm) {
    if (pm.has_id()) {
        CHECK_ZK_CLIENT(zk_client_);
        return zk_client_->DeleteEphemeralNode(pm.id().c_str());
    }
    return error::DEL_BACKUP_PM_FAIL;
}

ERROR_CODE ZKMetaManager::UpdatePM(const PM& pm) {
    // DSTREAM_DEBUG("update pm %d", pm.submit_port());
    return SetZKData(zk_client_, m_path, pm);
}

ERROR_CODE ZKMetaManager::UpdatePMMachines(const PMMachines& pm_machines) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE res = SetZKData(zk_client_, GetPMMachinesPath(), pm_machines);
    if (res == error::ZK_NO_NODE) {
        return AddZKNode(GetPMMachinesPath(), &pm_machines, true);
    }
    return SetZKData(zk_client_, GetPMMachinesPath(), pm_machines);
}

ERROR_CODE ZKMetaManager::GetPMMachines(PMMachines* pm_machines) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetPMMachinesPath(), pm_machines);
}

ERROR_CODE ZKMetaManager::DeletePMMachines() {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeletePersistentNode(GetPMMachinesPath().c_str());
}

ERROR_CODE ZKMetaManager::DeletePM() {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeleteEphemeralNode(m_path.c_str());
}

ERROR_CODE ZKMetaManager::AddPN(const PN& pn) {
    CHECK_ZK_CLIENT(zk_client_);
    return AddZKNode(GetPNPath(pn.pn_id()), &pn, false);
}

ERROR_CODE ZKMetaManager::UpdatePN(const PN& pn) {
    CHECK_ZK_CLIENT(zk_client_);
    return SetZKData(zk_client_, GetPNPath(pn.pn_id()), pn);
}

ERROR_CODE ZKMetaManager::DeletePN(const PN& pn) {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeleteEphemeralNode(GetPNPath(pn.pn_id()).c_str());
}

ERROR_CODE ZKMetaManager::GetPNList(list<PN>* pn_list) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKChildData(zk_client_, m_pn_root_path, pn_list);
}

ERROR_CODE ZKMetaManager::UpdatePNMachines(const PNMachines& pn_machines) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE res = SetZKData(zk_client_, GetPNMachinesPath(), pn_machines);
    if (res == error::ZK_NO_NODE) {
        DSTREAM_WARN("[%s] zk node [%s] not exist", __FUNCTION__, GetPNMachinesPath().c_str());
        return AddZKNode(GetPNMachinesPath(), &pn_machines, true);
    }
    return SetZKData(zk_client_, GetPNMachinesPath(), pn_machines);
}

ERROR_CODE ZKMetaManager::GetPNMachines(PNMachines* pn_machines) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetPNMachinesPath(), pn_machines);
}

ERROR_CODE ZKMetaManager::DeletePNMachines() {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeletePersistentNode(GetPNMachinesPath().c_str());
}

ERROR_CODE ZKMetaManager::UpdatePNGroups(const PNGroups& pn_group) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE ret = error::OK;
    std::set<GroupInfo, GroupInfoCompare> group_set;
    std::set<GroupInfo, GroupInfoCompare>::iterator set_it, set_end;

    for (int i = 0; i < pn_group.group_list_size(); i++) {
        if ((ret = UpdateGroupInfo(pn_group.group_list(i))) < error::OK) {
            DSTREAM_WARN("[%s] update group info [%s] fail", __FUNCTION__,
                         pn_group.group_list(i).group_name().c_str());
            return ret;
        }
        group_set.insert(pn_group.group_list(i));
    }
    std::list<GroupInfo> zk_group_list;
    std::list<GroupInfo>::iterator it;
    if ((ret = GetGroupList(&zk_group_list)) < error::OK) {
        DSTREAM_WARN("[%s] get group list fail", __FUNCTION__);
        return ret;
    }

    set_end = group_set.end();
    for (it = zk_group_list.begin(); it != zk_group_list.end(); it++) {
        if ((set_it = group_set.find(*it)) == set_end) {
            if ((ret = DeleteGroupInfo(*it)) < error::OK) {
                DSTREAM_WARN("[%s] delete group info [%s] fail",
                             __FUNCTION__, it->group_name().c_str());
                return ret;
            }
        }
    }

    return ret;
}

ERROR_CODE ZKMetaManager::GetPNGroups(PNGroups* pn_group) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE ret = error::OK;
    std::list<GroupInfo> group_list;
    if ((ret = GetGroupList(&group_list)) < error::OK) {
        DSTREAM_WARN("[%s] get group list fail", __FUNCTION__);
        return ret;
    }
    list<GroupInfo>::iterator group_end = group_list.end();
    for (list<GroupInfo>::iterator group_iter = group_list.begin();
         group_iter != group_end; ++group_iter) {
        GroupInfo* add = pn_group->add_group_list();
        add->CopyFrom(*group_iter);
    }
    return ret;
}

ERROR_CODE ZKMetaManager::UpdateGroupInfo(const GroupInfo& group_info) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE res = SetZKData(zk_client_, GetPNGroupPath(group_info.group_name()), group_info);
    if (res == error::ZK_NO_NODE) {
        DSTREAM_WARN("[%s] zk node [%s] not exist", __FUNCTION__,
                     GetPNGroupPath(group_info.group_name()).c_str());
        return AddZKNode(GetPNGroupPath(group_info.group_name()), &group_info, true);
    }
    // TODO(fangjun02) this call seems like useless???
    return SetZKData(zk_client_, GetPNGroupPath(group_info.group_name()), group_info);
}

ERROR_CODE ZKMetaManager::GetGroupInfo(const std::string& group_name, GroupInfo* group_info) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetPNGroupPath(group_name), group_info);
}

ERROR_CODE ZKMetaManager::GetGroupNameList(std::list<std::string>* group_name_list) {
    CHECK_ZK_CLIENT(zk_client_);
    // To update
    std::list <GroupInfo> group_list;
    ERROR_CODE res = GetGroupList(&group_list);
    if (res == error::OK) {
        list<GroupInfo>::iterator group_end = group_list.end();
        for (list<GroupInfo>::iterator group_iter = group_list.begin();
             group_iter != group_end; ++group_iter) {
            group_name_list->push_back(group_iter->group_name());
        }
    } else {
        DSTREAM_WARN("[%s] get group list fail", __FUNCTION__);
    }
    return res;
}

ERROR_CODE ZKMetaManager::GetGroupList(std::list<GroupInfo>* group_list) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKChildData(zk_client_, m_pn_group_root_path, group_list);
}

ERROR_CODE ZKMetaManager::AddGroupInfo(const GroupInfo& group_info) {
    CHECK_ZK_CLIENT(zk_client_);
    return AddZKNode(GetPNGroupPath(group_info.group_name()), &group_info, true);
}

ERROR_CODE ZKMetaManager::DeleteGroupInfo(const GroupInfo& group_info) {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeletePersistentNode(GetPNGroupPath(group_info.group_name()).c_str());
}

ERROR_CODE ZKMetaManager::DeletePNGroups() {
    CHECK_ZK_CLIENT(zk_client_);
    std::list <GroupInfo> group_list;
    ERROR_CODE res = GetGroupList(&group_list);
    if (res == error::OK) {
        list<GroupInfo>::iterator group_end = group_list.end();
        for (list<GroupInfo>::iterator group_iter = group_list.begin();
             group_iter != group_end; ++group_iter) {
            if ((res = DeleteGroupInfo(*group_iter)) < error::OK) {
                DSTREAM_WARN("[%s] delete group info fail", __FUNCTION__);
                return res;
            }
        }
    }
    return zk_client_->DeletePersistentNode(m_pn_group_root_path.c_str());
}

ERROR_CODE ZKMetaManager::AddSubPoint(const std::string& path, const SubPoint& sub_point) {
    CHECK_ZK_CLIENT(zk_client_);
    return AddZKNode(GetSubPointPath(path), &sub_point, true);
}

ERROR_CODE ZKMetaManager::DelSubPoint(const std::string& path, const SubPoint& sub_point) {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeletePersistentNode(GetSubPointPath(path).c_str());
}

ERROR_CODE ZKMetaManager::DelAppSubPoint(const std::string& path) {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeleteNodeTree(GetSubPointPath(path).c_str());
}

ERROR_CODE ZKMetaManager::GetSubPoint(const std::string& path, SubPoint* sub_point) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetSubPointPath(path), sub_point);
}

ERROR_CODE ZKMetaManager::UpdateSubPoint(const std::string& path, const SubPoint& sub_point) {
    CHECK_ZK_CLIENT(zk_client_);
    ERROR_CODE res = SetZKData(zk_client_, GetSubPointPath(path), sub_point);
    if (res == error::ZK_NO_NODE) {
        DSTREAM_WARN("[%s] zk node [%s] not exist", __FUNCTION__, GetSubPointPath(path).c_str());
        return AddZKNode(GetSubPointPath(path), &sub_point, true);
    }
    return res;
}

ERROR_CODE ZKMetaManager::AddScribeAppNode(const std::string& path, const std::string& ip_port) {
    CHECK_ZK_CLIENT(zk_client_);
    bool is_persist = true;
    int m_len = ip_port.size() + 1;
    MetaZKData zk_data;
    if (!zk_data.AllocateBuf(m_len)) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    strncpy(zk_data.m_buf, ip_port.data(), m_len);
    zk_data.m_len = m_len;
    ERROR_CODE res = zk_client_->SetNodeData(GetScribePath(path).c_str(), zk_data);
    DSTREAM_DEBUG("Add scribe importer:path(%s), scribepath(%s), retcode(%d)",
                            path.c_str(), GetScribePath(path).c_str(), res);
    if (res == error::ZK_NO_NODE) {
        DSTREAM_WARN("Add scribe importer:path(%s), scribepath(%s) storetype(%d)",
                path.c_str(), GetScribePath(path).c_str(), is_persist);
        return AddZKNode(GetScribePath(path), ip_port, is_persist);
    }
    return res;
}

ERROR_CODE ZKMetaManager::AddScribePENode(const std::string& path, const std::string& ip_port) {
    CHECK_ZK_CLIENT(zk_client_);
    bool is_persist = false;
    int m_len = ip_port.size() + 1;
    MetaZKData zk_data;
    if (!zk_data.AllocateBuf(m_len)) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    ERROR_CODE res = zk_client_->DeleteEphemeralNode(GetScribePath(path).c_str());
    DSTREAM_WARN("Delete scribe node path(%s), ret(%d)",
            GetScribePath(path).c_str(), res);
    strncpy(zk_data.m_buf, ip_port.data(), m_len);
    zk_data.m_len = m_len;
    res = AddZKNode(GetScribePath(path), ip_port, is_persist);
    DSTREAM_INFO("Add scribe importer:path(%s), scribepath(%s), retcode(%d)",
            path.c_str(), GetScribePath(path).c_str(), res);
    return res;
}

ERROR_CODE ZKMetaManager::DelScribeImporter(const std::string& path) {
    CHECK_ZK_CLIENT(zk_client_);
    // NOTICE:
    // we do not delete root path here because scribe server which use galieo forbid this
    return zk_client_->DeleteNodeTree(GetScribePath(path).c_str(), false);
}

ERROR_CODE ZKMetaManager::GetPN(const PNID& pn_id, PN* pn) {
    CHECK_ZK_CLIENT(zk_client_);
    return GetZKData(zk_client_, GetPNPath(pn_id), pn);
}

ERROR_CODE ZKMetaManager::SaveApplication(const Application& app) {
    CHECK_ZK_CLIENT(zk_client_);
    return AddZKNode(GetAppPath(app.id()), &app, true);
}

ERROR_CODE ZKMetaManager::DeleteApplication(const AppID& app_id) {
    CHECK_ZK_CLIENT(zk_client_);
    return zk_client_->DeleteNodeTree(GetAppPath(app_id).c_str());
}

ERROR_CODE ZKMetaManager::DeletePEList(const AppID& app_id,
                                       const PE_LIST& pe_list) {
    CHECK_ZK_CLIENT(zk_client_);
    m_lock.lock();
    PE_LIST::const_iterator pe_end = pe_list.end();
    ERROR_CODE res = error::OK;
    bool delete_succ = true;
    for (PE_LIST::const_iterator pe_it = pe_list.begin(); pe_it != pe_end; ++pe_it) {
        if ((res = zk_client_->DeletePersistentNode(GetPEPath(app_id,
                                                    pe_it->pe_id()).c_str())) != error::OK) {
            if (res != error::ZK_NO_NODE) {
                delete_succ = false;
            }
        }
    }
    if (!delete_succ) {
        res = error::ZK_DELETE_NODE_FAIL;
    }
    m_lock.unlock();
    return res;
}

ERROR_CODE ZKMetaManager::AddZKNode(const std::string& path,
                                    const ::google::protobuf::Message* msg,
                                    bool persist_node,
                                    int enable_sequence /* = 0*/,
                                    std::string* node_path /*= 0*/) {
    CHECK_ZK_CLIENT(zk_client_);
    MetaZKNode zk_node;
    if (!zk_node.AllocateBuf(zk_client::ZkClient::kZkMaxSingleNodePathLen,
                             msg->ByteSize())) {
        DSTREAM_WARN("allocate buffer fail");
        return error::ALLOC_BUFFER_FAIL;
    }
    if (static_cast<int>(path.length()) > zk_node.path.m_len - 1) {
        DSTREAM_WARN("[%s] Add ZKNode Fail path [%s] length [%zd] is large than [%d]",
                     __FUNCTION__, path.c_str(),
                     path.length(), zk_node.path.m_len);
        return error::SERIAIL_DATA_FAIL;
    }
    if (!strcpy(zk_node.path.m_buf, path.c_str())) {
        DSTREAM_WARN("serialize message fail");
        return error::SERIAIL_DATA_FAIL;
    }
    if (NULL != msg) {
        if (!msg->SerializeToArray(zk_node.data.m_buf, zk_node.data.m_len)) {
            DSTREAM_WARN("serialize message fail");
            return error::SERIAIL_DATA_FAIL;
        }
    } else {
        zk_node.data.m_len = 0;
    }
    if (persist_node) {
        return zk_client_->CreatePersistentNode(path.c_str(), &zk_node);
    }
    ERROR_CODE res = zk_client_->CreateEphemeralNode(path.c_str(), &zk_node,
                                                     enable_sequence);
    if (res == error::OK && NULL != node_path) {
        *node_path = zk_node.path.m_buf;
        DSTREAM_INFO("[%s] AddZKNode [%s] success", __FUNCTION__, zk_node.path.m_buf);
    }
    return res;
}

ERROR_CODE ZKMetaManager::AddZKNode(const std::string& path,
                                    const std::string& msg,
                                    bool persist_node,
                                    int enable_sequence /* = 0*/,
                                    std::string* node_path /*= 0*/) {
    CHECK_ZK_CLIENT(zk_client_);
    MetaZKNode zk_node;
    if (!zk_node.AllocateBuf(zk_client::ZkClient::kZkMaxSingleNodePathLen,
                             msg.size())) {
        DSTREAM_WARN("allocate buffer fail");
        return error::ALLOC_BUFFER_FAIL;
    }
    if (static_cast<int>(path.length()) > zk_node.path.m_len - 1) {
        DSTREAM_WARN("[%s] Add ZKNode Fail path [%s] length [%zd] is large than [%d]",
                     __FUNCTION__, path.c_str(),
                     path.length(), zk_node.path.m_len);
        return error::SERIAIL_DATA_FAIL;
    }
    if (!strcpy(zk_node.path.m_buf, path.c_str())) {
        DSTREAM_WARN("serialize message fail");
        return error::SERIAIL_DATA_FAIL;
    }
    if (0 != msg.size()) {
        if (!strcpy(zk_node.data.m_buf, msg.c_str())) {
            DSTREAM_WARN("serialize message [%s] into zk node fail", msg.c_str());
            return error::SERIAIL_DATA_FAIL;
        }
        zk_node.data.m_len = msg.size();
    } else {
        zk_node.data.m_len = 0;
    }
    if (persist_node) {
        return zk_client_->CreatePersistentNode(path.c_str(), &zk_node);
    }
    ERROR_CODE res = zk_client_->CreateEphemeralNode(path.c_str(), &zk_node,
                                                     enable_sequence);
    if (res == error::OK && NULL != node_path) {
        *node_path = zk_node.path.m_buf;
        DSTREAM_INFO("[%s] AddZKNode [%s] success", __FUNCTION__, zk_node.path.m_buf);
    }
    return res;
}

std::string ZKMetaManager::GetAppPath(const AppID& app_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%ld", m_app_root_path.c_str(), app_id.id());
    return path;
}

std::string ZKMetaManager::GetPNPath(const PNID& pn_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%s", m_pn_root_path.c_str(), pn_id.id().c_str());
    return path;
}

std::string ZKMetaManager::GetPEPath(const AppID& app_id, const PEID& pe_id) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%ld/%ld", m_app_root_path.c_str(), app_id.id(), pe_id.id());
    return path;
}

std::string ZKMetaManager::GetBackupPMPath(const PM& pm) {
    std::string backup_pm_path = m_backup_pm_root + "/";
    return backup_pm_path + pm.host() + "_";
}

std::string ZKMetaManager::GetPNGroupPath(const std::string& group_name) {
    char path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%s", pn_group_root_path().c_str(), group_name.c_str());
    return path;
}

std::string ZKMetaManager::GetSubPointPath(const std::string& path) {
    char abs_path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(abs_path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%s", sub_point_root_path().c_str(), path.c_str());
    return abs_path;
}

std::string ZKMetaManager::GetScribePath(const std::string& path) {
    char abs_path[zk_client::ZkClient::kZkMaxSingleNodePathLen];
    snprintf(abs_path, zk_client::ZkClient::kZkMaxSingleNodePathLen,
             "%s/%s", scribe_root_path().c_str(), path.c_str());
    return abs_path;
}

} // namespace zk_meta_manager
} // namespace dstream
