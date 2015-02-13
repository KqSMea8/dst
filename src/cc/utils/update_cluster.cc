/** 
* @file     update_cluster.cc
* @brief    
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include <google/protobuf/descriptor.h>
#include <google/protobuf/text_format.h>
#include <sys/stat.h>
#include "common/application_tools.h"
#include "common/dstream_optional.h"
#include "common/dstream_type.h"
#include "common/proto/pm.pb.h"
#include "common/utils.h"
#include "metamanager/meta_manager.h"
#include "metamanager/zk_config.h"
/*
using namespace dstream;
using namespace dstream::logger;
using namespace dstream::config;
using namespace google::protobuf;
using namespace dstream::meta_manager;
using namespace dstream::zk_config;
*/
typedef std::list<dstream::config::ConfigNode> NodeList;
bool ConstructMessage(google::protobuf::Message* message, dstream::config::ConfigNode* node) {
    assert(NULL != node);
    if (node->IsErrorNode()) {
        std::cout << "node is error node" << std::endl;
        return false;
    }
    std::string node_name = node->GetName();
    if (node_name != "") {
        const google::protobuf::Descriptor* des = message->GetDescriptor();
        const google::protobuf::FieldDescriptor* field_des = des->FindFieldByName(node_name);
        const google::protobuf::Reflection* ref = message->GetReflection();
        if (des == 0 || field_des == 0 || ref == 0) {
            std::cout << "cannot find field " << node_name << std::endl;
            return false;
        }
        std::string value;
        NodeList node_list;
        if (field_des->type() == google::protobuf::FieldDescriptor::TYPE_MESSAGE) {
            if (node->HasNoChildren()) {
                return false;
            }
            node_list = node->GetChildrenList();
            if (node_list.size() == 0) {
                return false;
            }
            if (node_list.size() > 1 && !field_des->is_repeated()) {
                return false;
            }
        } else {
            if (node->GetValue(&value) != dstream::error::OK) {
                std::cout << "cannot get node " << node_name << std::endl;
                return false;
            }
        }
        float float_val;
        uint32_t int_val;
        switch (field_des->type()) {
        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            float_val = dstream::Convert<float>(value);
            if (field_des->is_repeated()) {
                ref->AddFloat(message, field_des, float_val);
            } else {
                ref->SetFloat(message, field_des, float_val);
            }
            break;
        case google::protobuf::FieldDescriptor::TYPE_STRING:
            if (field_des->is_repeated()) {
                ref->AddString(message, field_des, value);
            } else {
                ref->SetString(message, field_des, value);
            }
            break;
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            int_val = dstream::Convert<uint32_t>(value);
            if (field_des->is_repeated()) {
                ref->AddUInt32(message, field_des, int_val);
            } else {
                ref->SetUInt32(message, field_des, int_val);
            }
            break;
        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
            if (field_des->is_repeated()) {
                google::protobuf::Message* add_msg = ref->AddMessage(message, field_des);
                for (NodeList::iterator it = node_list.begin();
                     it != node_list.end(); ++it) {
                    if (!ConstructMessage(add_msg, &(*it))) {
                        ref->RemoveLast(message, field_des);
                        break;
                    }
                }
            } else {
                google::protobuf::Message* msg = ref->MutableMessage(message, field_des);
                if (!ConstructMessage(msg, &(*node_list.begin()))) {
                    return false;
                }
            }
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}

static std::string config_file;
static std::string pm_list_file;
static std::string pn_list_file;
static std::string pn_group_file;
static const char* kPMTestFileOpt  = "--pm_file";
static const char* kPNTestFileOpt  = "--pn_file";
static const char* kPNGroupFileOpt = "--pn_group_file";
static const char* kClearMetaOpt   = "--clear_meta";
static bool clear_meta = false;

static int32_t ClearMetaOpt(const char* /*opt*/, const char* /*value*/) {
    clear_meta = true;
    return 0;
}

static int32_t ParseMachineList(const char* file_name, std::vector<std::string>* res) {
    assert(NULL != res);
    if (file_name == NULL) {
        DSTREAM_WARN("[%s] file name is empty", __FUNCTION__);
        return -1;
    }
    int filesize = -1;
    struct stat statbuff;
    if (stat(file_name, &statbuff) == 0) {
        filesize = statbuff.st_size;
    }
    if (filesize <= 0) {
        DSTREAM_WARN("[%s] file [%s] size is less than 0", __FUNCTION__, file_name);
        return -2;
    }
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        DSTREAM_WARN("[%s] opean file [%s] faile", __FUNCTION__, file_name);
        return -3;
    }
    char* buf = new char[filesize + 2];
    memset(buf, 0, filesize + 2);
    int read_count = fread(buf, sizeof(char), filesize, file); // NOLINT
    for (int i = 0; i < read_count;) {
        int end = i;
        while (end < filesize && buf[end] != '\r' && buf[end] != ' '
               && buf[end] != '\n' && buf[end] != '\t' && buf[end] != '\0') {
            ++end;
        }
        if (end != i) {
            buf[end] = '\0';
            res->push_back(buf + i);
            i = end + 1;
        }
        while (i < filesize && (buf[i] == '\r' || buf[i] == '\n' || buf[i] == ' '
                                || buf[i] == '\t' || buf[i] == '\0')) {
            ++i;
        }
    }
    delete []buf;
    fclose(file);
    return 0;
}

static int32_t GetPENum(const std::string& pn_name, std::list<dstream::PN>& pn_list,
                        std::map<dstream::PNID, int, dstream::PNIDCompare>& pn_pe_map) {
    std::string ip;
    if (dstream::error::OK != dstream::GetRemotehostIP(pn_name, ip)) {
        DSTREAM_WARN("get remote host [%s] ip fail", pn_name.c_str());
        return -1;
    }

    std::map<dstream::PNID, int, dstream::PNIDCompare>::iterator pn_pe_iter;
    std::list<dstream::PN>::const_iterator it = pn_list.begin();
    for (it = pn_list.begin(); it != pn_list.end(); it++) {
        size_t found = it->pn_id().id().find(ip);
        if (found != std::string::npos) {
            pn_pe_iter = pn_pe_map.find(it->pn_id());
            if (pn_pe_iter != pn_pe_map.end()) {
                return pn_pe_iter->second;
            } else {
                return 0;
            }
        }
    }
    return 0;
}

static int32_t StrictCheck(const std::map<std::string, std::string>& host_group_map) {
    int32_t ret = dstream::error::OK;
    dstream::meta_manager::MetaManager* meta = dstream::meta_manager::MetaManager::GetMetaManager();
    std::list<dstream::PN> zk_pn_list;
    if ((ret = meta->GetPNList(&zk_pn_list)) < dstream::error::OK) {
        DSTREAM_WARN("there is still no pn registers on zk");
        return dstream::error::OK;
    }

    std::list<dstream::AppID> app_id_list;
    std::list<dstream::AppID>::iterator app_id_iter;
    if ((ret = meta->GetAppIDList(&app_id_list)) < dstream::error::OK) {
        DSTREAM_WARN("there is still no app running");
        return dstream::error::OK;
    }
    std::map<dstream::PNID, int, dstream::PNIDCompare> pn_pe_map;
    std::map<dstream::PNID, int, dstream::PNIDCompare>::iterator pn_pe_iter;
    for (app_id_iter = app_id_list.begin();
         app_id_iter != app_id_list.end();
         app_id_iter++) {
        std::list<dstream::ProcessorElement> pe_list;
        std::list<dstream::ProcessorElement>::iterator pe_list_iter;
        if ((ret = meta->GetAppProcessElements(*app_id_iter, &pe_list)) < dstream::error::OK) {
            DSTREAM_WARN("there is still no app running");
            return dstream::error::OK;
        }
        for (pe_list_iter = pe_list.begin();
             pe_list_iter != pe_list.end();
             pe_list_iter++) {
            for (int i = 0; i < pe_list_iter->backups_size(); i++) {
                if ((pn_pe_iter = pn_pe_map.find(pe_list_iter->backups(i).pn_id()))
                    != pn_pe_map.end()) {
                    ++pn_pe_iter->second;
                } else {
                    pn_pe_map[pe_list_iter->backups(i).pn_id()] = 1;
                }
            }
        }
    }

    dstream::PNGroups zk_pn_group;
    if ((ret = meta->GetPNGroups(&zk_pn_group)) < dstream::error::OK) {
        DSTREAM_WARN("there is still no pn group registers on zk");
    }
    std::map<std::string, std::string> zk_host_group_map;
    for (int i = 0; i < zk_pn_group.group_list_size(); i++) {
        for (int j = 0; j < zk_pn_group.group_list(i).pn_list_size(); j++) {
            std::pair<std::string, std::string> zk_host_group_pair;
            zk_host_group_pair.first = zk_pn_group.group_list(i).pn_list(j);
            zk_host_group_pair.second = zk_pn_group.group_list(i).group_name();
            zk_host_group_map.insert(zk_host_group_pair);
        }
    }

    std::map<std::string, std::string>::const_iterator host_group_iter, host_group_find;
    for (host_group_iter = host_group_map.begin();
         host_group_iter != host_group_map.end();
         host_group_iter++) {
        if ((host_group_find = zk_host_group_map.find(host_group_iter->first))
            != zk_host_group_map.end()) {
            if (host_group_find->second != host_group_iter->second) {
                if (GetPENum(host_group_iter->first, zk_pn_list, pn_pe_map) > 0) {
                    return -2;
                }
            }
        } else {
            if (GetPENum(host_group_iter->first, zk_pn_list, pn_pe_map) > 0) {
                return -1;
            }
        }
    }
    return dstream::error::OK;
}

static int32_t CheckPNGroup(const dstream::PNGroups& pn_group) {
    std::set<std::string> pn_list, group_name_set;
    unsigned int pn_num = 0;
    std::map<std::string, std::string> host_group_map;
    /* 1. forbidden user to define default group
     * 2. check if there are groups with the same name
     * 3. check if there are pn overlaps in all groups
     * 4. check if there are pn with running pe changes its group
     */
    for (int i = 0; i < pn_group.group_list_size(); i++) {
        if (pn_group.group_list(i).group_name() == "default") {
            DSTREAM_WARN("[%s] default group is forbidden", __FUNCTION__);
            return -1;
        }
        group_name_set.insert(pn_group.group_list(i).group_name());
        pn_num += pn_group.group_list(i).pn_list_size();
        for (int j = 0; j < pn_group.group_list(i).pn_list_size(); j++) {
            pn_list.insert(pn_group.group_list(i).pn_list(j));
            std::pair<std::string, std::string> host_group_pair;
            host_group_pair.first = pn_group.group_list(i).pn_list(j);
            host_group_pair.second = pn_group.group_list(i).group_name();
            host_group_map.insert(host_group_pair);
        }
        if (pn_num != pn_list.size()) {
            DSTREAM_WARN("[%s] there are pns overlap", __FUNCTION__);
            return -2;
        }
    }
    if (pn_group.group_list_size() != static_cast<int>(group_name_set.size())) {
        DSTREAM_WARN("[%s] there are group names overlap", __FUNCTION__);
        return -3;
    }
    if (StrictCheck(host_group_map) < dstream::error::OK) {
        DSTREAM_WARN("[%s] strick check fail", __FUNCTION__);
        return -4;
    }
    return 0;
}

static int32_t ParsePNGroupList(const char* file_name, dstream::PNGroups* pn_group) {
    assert(NULL != pn_group);
    if (file_name == NULL) {
        DSTREAM_WARN("[%s] file name is empty", __FUNCTION__);
        return -1;
    }
    int filesize = -1;
    struct stat statbuff;
    if (stat(file_name, &statbuff) == 0) {
        filesize = statbuff.st_size;
    }
    if (filesize <= 0) {
        DSTREAM_WARN("[%s] file [%s] size is less than 0", __FUNCTION__, file_name);
        return -2;
    }
    FILE* file = fopen(file_name, "r");
    if (file == NULL) {
        DSTREAM_WARN("[%s] open file [%s] fail", __FUNCTION__, file_name);
        return -3;
    }
    char* buf = new char[filesize + 2];
    memset(buf, 0, filesize + 2);
    int read_count = fread(buf, sizeof(char), filesize, file); // NOLINT
    if (read_count < 0) {
        DSTREAM_WARN("[%s] read file [%s] fail", __FUNCTION__, file_name);
        delete []buf;
        fclose(file);
        return -4;
    }
    std::string s(buf);
    if (!google::protobuf::TextFormat::ParseFromString(s, pn_group)) {
        DSTREAM_WARN("[%s] parse from string fail", __FUNCTION__);
        delete []buf;
        fclose(file);
        return -5;
    }

    if (CheckPNGroup(*pn_group) < 0) {
        DSTREAM_WARN("[%s] check pn group file [%s] fail", __FUNCTION__, file_name);
        delete []buf;
        fclose(file);
        return -6;
    }

    delete []buf;
    fclose(file);
    return 0;
}

// Add pm/pn machines to zk
static int32_t AddMachines(const char* opt, const char* value) {
    if (opt == NULL || value == NULL) {
        DSTREAM_WARN("argument is invalid");
        return -1;
    }
    if (strcmp(opt, kPMTestFileOpt) == 0) {
        pm_list_file = value;
    }
    if (strcmp(opt, kPNTestFileOpt) == 0) {
        pn_list_file = value;
    }
    DSTREAM_INFO("[%s] opt is [%s] value is [%s]", __FUNCTION__, opt, value);
    if (strcmp(opt, kPMTestFileOpt) == 0 || strcmp(opt, kPNTestFileOpt) == 0) {
        std::vector<std::string> mac_list;
        if (ParseMachineList(value, &mac_list) == 0) {
            dstream::meta_manager::MetaManager* meta =
                            dstream::meta_manager::MetaManager::GetMetaManager();
            if (NULL == meta) {
                return 0;
            }
            dstream::PMMachines pm_machines;
            dstream::PNMachines pn_machines;
            if (strcmp(opt, kPMTestFileOpt) == 0) {
                for (unsigned int i = 0; i < mac_list.size(); ++i) {
                    pm_machines.add_pm_list(mac_list[i]);
                }
            } else {
                for (unsigned int i = 0; i < mac_list.size(); ++i) {
                    pn_machines.add_pn_list(mac_list[i]);
                }
            }
            if (strcmp(opt, kPMTestFileOpt) == 0 && meta->UpdatePMMachines(pm_machines)
                                                                        != dstream::error::OK) {
                DSTREAM_WARN("[%s] update pm machines fail", __FUNCTION__);
                return -4;
            }
            if (strcmp(opt, kPNTestFileOpt)  == 0 && meta->UpdatePNMachines(pn_machines)
                                                                        != dstream::error::OK) {
                DSTREAM_WARN("[%s] update pn machines fail", __FUNCTION__);
                return -4;
            }
            DSTREAM_INFO("[%s] update machines success", __FUNCTION__);
            return 0;
        }
        DSTREAM_WARN("[%s] parse file [%s] fail", __FUNCTION__, value);
        return -3;
    }
    return -2;
}

// update pn group list to zk
static int32_t UpdatePNGroup(const char* opt, const char* value) {
    if (opt == NULL || value == NULL) {
        DSTREAM_WARN("argument is invalid");
        return -1;
    }
    if (strcmp(opt, kPNGroupFileOpt) == 0) {
        pn_group_file = value;
    }
    DSTREAM_INFO("[%s] opt is [%s] value is [%s]", __FUNCTION__, opt, value);
    if (strcmp(opt, kPNGroupFileOpt) == 0) {
        dstream::PNGroups pn_group;
        if (ParsePNGroupList(value, &pn_group) == 0) {
            dstream::meta_manager::MetaManager* meta =
                    dstream::meta_manager::MetaManager::GetMetaManager();
            if (NULL == meta) {
                return -1;
            }
            if (strcmp(opt, kPNGroupFileOpt) == 0 && meta->UpdatePNGroups(pn_group)
                                                            != dstream::error::OK) {
                DSTREAM_WARN("[%s] update pn groups fail", __FUNCTION__);
                return -4;
            }
            DSTREAM_INFO("[%s] update pn group list success", __FUNCTION__);
            return 0;
        }
        DSTREAM_WARN("[%s] parse file [%s] fail", __FUNCTION__, value);
        return -3;
    }
    return -2;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s config_file %s pm_machines_file %s"
               " pn_machines_file %s pn group file %s\n",
               argv[0], kPMTestFileOpt, kPNTestFileOpt, kPNGroupFileOpt, kClearMetaOpt);
        exit(1);
    }
    ::dstream::logger::initialize("update_cluster");
    dstream::AddOptional(kPMTestFileOpt, AddMachines);
    dstream::AddOptional(kPNTestFileOpt, AddMachines);
    dstream::AddOptional(kPNGroupFileOpt, UpdatePNGroup);
    dstream::AddOptional(kClearMetaOpt, ClearMetaOpt);
    //  ParseOptional(argc - 2, argv + 2);
    dstream::config::Config config;
    dstream::ClusterConfig cluster_config;
    config.ReadConfig(argv[1]);
    dstream::config::ConfigNode node = config.GetRootNode();
    if (!dstream::meta_manager::MetaManager::InitMetaManagerContext(&config)) {
        std::cout << "init metamanager fail" << std::endl;
        exit(1);
    }
    dstream::meta_manager::MetaManager* meta = dstream::meta_manager::MetaManager::GetMetaManager();
    ERROR_CODE res = dstream::error::OK;
    if (!clear_meta && (res = meta->GetClusterConfig(&cluster_config)) != dstream::error::OK &&
        res != dstream::error::ZK_NO_NODE) {
        std::cout << "get cluster config fail" << std::endl;
        exit(1);
    }
    NodeList child_list = node.GetChildrenList();
    for (NodeList::iterator it = child_list.begin(); it != child_list.end(); ++it) {
        ConstructMessage(&cluster_config, &(*it));
    }
    // reconstruct users
    typedef std::map<std::string, std::string> NamePasswordMap;
    NamePasswordMap name_pwd_map;
    for (int i = 0; i < cluster_config.users_size(); ++i) {
        const dstream::User& user = cluster_config.users(i);
        if (user.has_username() && user.has_password()) {
            NamePasswordMap::iterator user_find = name_pwd_map.find(user.username());
            if (user_find == name_pwd_map.end()) {
                name_pwd_map[user.username()] = user.password();
            } else {
                user_find->second = user.password();
            }
        }
    }
    cluster_config.clear_users();
    for (NamePasswordMap::iterator it = name_pwd_map.begin();
         it != name_pwd_map.end(); ++it) {
        dstream::User* add_user = cluster_config.add_users();
        add_user->set_username(it->first);
        add_user->set_password(it->second);
    }

    if (meta->UpdateClusterConfig(cluster_config) != dstream::error::OK) {
        std::cout << "update cluster config fail" << std::endl;
        exit(1);
    }
    std::cout << "update cluster config success, content is:" << std::endl;
    dstream::zk_config::ZKConfig zk_cf(&config);
    std::cout << "config path:" << zk_cf.GetSysPath() << " content is:" << std::endl;
    std::cout << cluster_config.DebugString();
    dstream::ParseOptional(argc - 2, argv + 2);
}
