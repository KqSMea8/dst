/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief  meta manager using zookeeper
 */

#ifndef __DSTREAM_CC_PROCESSMASTER_ZK_META_MANAGER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_ZK_META_MANAGER_H__ // NOLINT

#include <assert.h>
#include <cstddef>
#include <list>
#include <set>
#include <string>
#include <utility>

#include "common/dstream_type.h"
#include "common/zk_client.h"
#include "metamanager/meta_manager.h"

namespace dstream {
namespace zk_meta_manager {

/************************************************************************/
/* ZKData : Used for auto memory release                                */
/************************************************************************/
class MetaZKData : public zk_client::Data {
public:
    MetaZKData();
    MetaZKData(const MetaZKData& d);
    MetaZKData& operator =(const MetaZKData& d);
    bool AllocateBuf(int size = zk_client::ZkClient::kZkMaxSingleNodeDataLen);
    ~MetaZKData();
};

class MetaZKNode : public zk_client::ZkNode {
public:
    MetaZKNode();
    ~MetaZKNode();
    bool AllocateBuf(int path_size = zk_client::ZkClient::kZkMaxSingleNodePathLen,
                     int data_size = zk_client::ZkClient::kZkMaxSingleNodeDataLen);
};

/************************************************************************/
/* GetZK data of path, T is a protobuffer type                          */
/************************************************************************/
template <class T>
ERROR_CODE GetZKData(zk_client::ZkClient* zk_client, const std::string& path, T* obj) {
    assert(NULL != zk_client);
    assert(NULL != obj);
    MetaZKData zk_data;
    if (!zk_data.AllocateBuf()) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    error::Code res = error::OK;
    if ((res = zk_client->GetNodeData(path.c_str(), &zk_data)) != error::OK) {
        DSTREAM_WARN("get node [%s] fail", path.c_str());
        return res;
    }
    if (!obj->ParseFromArray(zk_data.m_buf, zk_data.m_len)) {
        DSTREAM_WARN("parse data error");
        return error::SERIAIL_DATA_FAIL;
    }
    return res;
}

template <class T>
ERROR_CODE GetZKChildData(zk_client::ZkClient* zk_client,
                          const std::string& parent_path,
                          std::list<T>* obj_list,
                          std::list<string>* path_list = 0) {
    assert(NULL != zk_client);
    assert(NULL != obj_list);
    error::Code res = error::OK;
    std::list<zk_client::ZkNode*> child_list;
    if ((res = zk_client->GetNodeChildren(parent_path.c_str(),
                                          &child_list)) != error::OK) {
        DSTREAM_WARN("Get [%s]'s Child fail", parent_path.c_str());
        return res;
    }
    std::list<zk_client::ZkNode*>::iterator child_end = child_list.end();
    for (std::list<zk_client::ZkNode*>::iterator child_iter = child_list.begin();
         child_iter != child_end; ++child_iter) {
        T obj;
        if (!obj.ParseFromArray((*child_iter)->data.m_buf,
                                (*child_iter)->data.m_len)) {
            DSTREAM_WARN("parse from array fail");
        } else {
            obj_list->push_back(obj);
        }
        if (0 != path_list) {
            path_list->push_back((*child_iter)->path.m_buf);
        }
        DeleteZkNode(*child_iter);
    }
    return error::OK;
}

/************************************************************************/
/* set zk data of path, T is protobuffer type                           */
/************************************************************************/
template <class T>
ERROR_CODE SetZKData(zk_client::ZkClient* zk_client, const std::string& path, const T& obj) {
    assert(NULL != zk_client);
    MetaZKData zk_data;
    if (!zk_data.AllocateBuf(obj.ByteSize())) {
        DSTREAM_WARN("allocate data node error");
        return error::ALLOC_BUFFER_FAIL;
    }
    if (!obj.SerializeToArray(zk_data.m_buf, zk_data.m_len)) {
        DSTREAM_WARN("serialize data fail");
        return error::SERIAIL_DATA_FAIL;
    }
    error::Code res;
    if ((res = zk_client->SetNodeData(path.c_str(), zk_data)) != error::OK) {
        DSTREAM_WARN("update zk data fail");
    }
    return res;
}

/************************************************************************/
/* MetaManager using zookeeper to store meta data                       */
/************************************************************************/
class ZKMetaManager : public meta_manager::MetaManager {
public:
    ZKMetaManager() {}
    virtual ~ZKMetaManager() {}
    bool InitMetaManager(const Config* config);

    // ClusterConfig interface
    virtual ERROR_CODE GetClusterConfig(ClusterConfig* config);
    virtual ERROR_CODE UpdateClusterConfig(const ClusterConfig& config);

    // Application interface
    virtual ERROR_CODE UpdateApplication(const Application& app);
    virtual ERROR_CODE DeleteApplication(const AppID& app);
    virtual ERROR_CODE AddAppProcessElements(const AppID& app_id,
                                             const list<ProcessorElement>& pe_list);
    virtual ERROR_CODE UpdateProcessElement(const AppID& app_id,
                                            const ProcessorElement& pe);
    virtual ERROR_CODE GetApplicationList(list<Application>* app_list);
    virtual ERROR_CODE GetAppIDList(list<AppID>* id_list);
    virtual ERROR_CODE GetApplication(const AppID& app_id, Application* app);
    virtual ERROR_CODE GetAppProcessElements(const AppID& app_id,
                                             list<ProcessorElement>* pe_list);
    virtual ERROR_CODE DeletePEList(const AppID& app_id, const PE_LIST& pe_list);
    virtual ERROR_CODE GetAppFlowControl(const AppID& app_id,
            const std::string& processor, FlowControl* flow_control);
    /************************************************************************/
    /* Get processments on pn including under constuct pe                   */
    /************************************************************************/
    virtual ERROR_CODE GetPNProcessElements(const PNID& pn_id,
                                            set<ProcessorElement,
                                            application_tools::ComparePE>* pe_list);
    virtual ERROR_CODE GetPNProcessElementsMap(application_tools::PnidPeSetMap* pn_pe_map);
    virtual ERROR_CODE GetProcessorElement(const AppID& app_id,
                                           const PEID& pe_id,
                                           ProcessorElement* pe);

    // PM interface
    virtual ERROR_CODE AddPM(const PM& pm);
    virtual ERROR_CODE AddBackupPM(PM* pm);
    virtual ERROR_CODE GetBackupPM(PM* pm);
    virtual ERROR_CODE GetBackupPM(list<PM>* backup_pm_list);
    virtual ERROR_CODE DeleteBackupPM(const PM& pm);
    virtual ERROR_CODE DeletePM();
    virtual ERROR_CODE GetPM(PM* pm);
    virtual ERROR_CODE UpdatePM(const PM& pm);
    virtual ERROR_CODE UpdatePMMachines(const PMMachines& pm_machines);
    virtual ERROR_CODE GetPMMachines(PMMachines* pm_machines);
    virtual ERROR_CODE DeletePMMachines();

    // PN interface
    virtual ERROR_CODE AddPN(const PN& pn);
    virtual ERROR_CODE UpdatePN(const PN& pn);
    virtual ERROR_CODE DeletePN(const PN& pn);
    virtual ERROR_CODE GetPN(const PNID& pn_id, PN* pn);
    virtual ERROR_CODE GetPNList(list<PN>* pn_list);
    virtual ERROR_CODE UpdatePNMachines(const PNMachines& pn_machines);
    virtual ERROR_CODE GetPNMachines(PNMachines* pn_machines);
    virtual ERROR_CODE DeletePNMachines();
    // PNGroups
    virtual ERROR_CODE UpdatePNGroups(const PNGroups& pn_group);
    virtual ERROR_CODE UpdateGroupInfo(const GroupInfo& group_info);
    virtual ERROR_CODE GetPNGroups(PNGroups* pn_group);
    virtual ERROR_CODE GetGroupInfo(const std::string& group_name, GroupInfo* group_info);
    virtual ERROR_CODE GetGroupNameList(std::list<std::string>* group_name_list);
    virtual ERROR_CODE GetGroupList(std::list<GroupInfo>* group_list);
    virtual ERROR_CODE AddGroupInfo(const GroupInfo& group_info);
    virtual ERROR_CODE DeleteGroupInfo(const GroupInfo& group_info);
    virtual ERROR_CODE DeletePNGroups();
    // SubPoint
    virtual ERROR_CODE AddSubPoint(const std::string& path, const SubPoint& sub_point);
    virtual ERROR_CODE DelSubPoint(const std::string& path, const SubPoint& sub_point);
    virtual ERROR_CODE GetSubPoint(const std::string& path, SubPoint* sub_point);
    virtual ERROR_CODE UpdateSubPoint(const std::string& path, const SubPoint& sub_point);
    virtual ERROR_CODE DelAppSubPoint(const std::string& path);
    // Scribe
    virtual ERROR_CODE AddScribeAppNode(const std::string& path,
            const std::string& ip_port);
    virtual ERROR_CODE AddScribePENode(const std::string& path,
            const std::string& ip_port);
    virtual ERROR_CODE DelScribeImporter(const std::string& path);

    ERROR_CODE AddZKNode(const std::string& path,
                         const ::google::protobuf::Message* msg,
                         bool persist_node, int enable_sequence = 0,
                         std::string* node_path = 0);
    ERROR_CODE AddZKNode(const std::string& path,
                         const std::string& msg,
                         bool persist_node, int enable_sequence = 0,
                         std::string* node_path = 0);
    /************************************************************************/
    /*the interface used for test                                           */
    /************************************************************************/
    zk_client::ZkClient* zk_client() {
        return zk_client_;
    }
    const std::string& app_root_path() const {
        return m_app_root_path;
    }
    const std::string& cluster_config_path() const {
        return m_cluster_config_path;
    }
    const std::string& pm_path() const {
        return m_path;
    }
    const std::string& pn_root_path() const {
        return m_pn_root_path;
    }
    const std::string m_backup_pm_rootpath() {
        return m_backup_pm_root;
    }
    const std::string machine_root_path() {
        return m_root_path + "/machines";
    }
    const std::string pn_group_root_path() {
        return m_pn_group_root_path;
    }
    const std::string sub_point_root_path() {
        return m_sub_point_root_path;
    }
    const std::string scribe_root_path() {
        return m_scribe_root_path;
    }

protected:
    virtual ERROR_CODE SaveApplication(const Application& app);

private:
    bool InitMaxAppID();
    std::string GetAppPath(const AppID& app_id);
    std::string GetPNPath(const PNID& pn_id);
    std::string GetPEPath(const AppID& app_id, const PEID& pe_id);
    std::string GetBackupPMPath(const PM& pm);
    std::string GetPMMachinesPath() {
        return machine_root_path() + "/pm_machines";
    }
    std::string GetPNMachinesPath() {
        return machine_root_path() + "/pn_machines";
    }
    std::string GetPNGroupPath(const std::string& group_name);
    std::string GetSubPointPath(const std::string& path);
    std::string GetScribePath(const std::string& path);
    auto_ptr::AutoPtr<zk_client::ZkClient> zk_client_;
    std::string m_app_root_path;
    std::string m_cluster_config_path;
    std::string m_path;
    std::string m_backup_pm_root;
    std::string m_pn_root_path;
    std::string m_zk_addr;
    std::string m_pn_group_root_path;
    std::string m_sub_point_root_path;
    std::string m_scribe_root_path;
    std::string m_root_path;
    int m_zk_timeout;
};

} // namespace zk_meta_manager
} // namespace dstream
#endif // NOLINT
