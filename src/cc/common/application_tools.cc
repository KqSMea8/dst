/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/12
* @filename:    application_tools.cc
* @author:      zhanggengxin
* @brief:       application tools
*/

#include "common/application_tools.h"
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "common/hdfs_client.h"

namespace dstream {
namespace application_tools {
using namespace hdfs_client;

// Get ProcessorID from peid
ProcessorID ConstructProcessorID(const PEID& pe_id) {
    ProcessorID processor_id;
    if (pe_id.has_id()) {
        uint64_t u_pe_id = pe_id.id() >> 32;
        processor_id.set_id((u_pe_id << 32));
    }
    return processor_id;
}

ProcessorID ConstructProcessorID(const AppID& app_id, const ProcessorID& pid) {
    ProcessorID processor_id;
    processor_id.set_id(((app_id.id() << 40) | (pid.id() << 32)));
    return processor_id;
}

void AddPEAssignment(const BackupPEID& backup_pe_id,
                     const PNID& pn_id,
                     ProcessorElement* pe) {
    int backup_size = pe->backups_size();
    for (int i = 0; i < backup_size; ++i) {
        BackupPE* backup_pe = pe->mutable_backups(i);
        if (backup_pe->has_backup_pe_id() &&
            backup_pe->backup_pe_id().id() == backup_pe_id.id()) {
            *(backup_pe->mutable_pn_id()) = pn_id;
            return;
        }
    }
}

bool CheckPENormal(const ProcessorElement& pe,
                   const AppID& /*app_id*/,
                   const PEID& /*pe_id*/) {
    if (pe.primary_size() == 0 || !pe.primary(0).has_id()) {
        return false;
    }
    // only check the first pe for resource scheduler
    for (int i = 0; i < pe.backups_size(); ++i) {
        if (!pe.backups(i).has_pn_id()) {
            return false;
        }
    }
    return true;
}

typedef std::list<ProcessorElement> PE_LIST;
bool CreateAppPEInstance(const Application& app,
                         const ClusterConfig& cluster_config,
                         const config::Config* /*config*/,
                         PE_LIST* pe_list) {
    DSTREAM_UNUSED_VAL(cluster_config);

    const Topology& topology = app.topology();
    uint64_t processor_size = static_cast<uint64_t>(topology.processors_size());
    BackupPEID* backup_pe_id;
    logConfig* log_config;
    BackupPE* backup_pe;
    uint64_t app_id_int = (app.id().id() << 40);
    char bigpipe_name[2000];
    for (uint64_t i = 0; i < processor_size; ++i) {
        const Processor& processor = topology.processors(i);
        uint64_t backup_num = processor.backup_num();
        uint64_t paralism = processor.parallism();
        for (uint64_t j = 0; j < paralism; ++j) {
            ProcessorElement pe;
            PE_LIST::iterator add_pe = pe_list->insert(pe_list->end(), pe);
            *(add_pe->mutable_app()) = app.id();
            *(add_pe->mutable_processor()) = processor.id();
            *(add_pe->mutable_resource()) = processor.resource_require();
            add_pe->set_processor_name(processor.name());
            uint64_t create_id = (app_id_int | (processor.id().id() << 32) | (j << 8));
            add_pe->mutable_pe_id()->set_id(create_id);
            add_pe->set_serial(j);
            log_config = add_pe->mutable_log();
            *(log_config->mutable_bigpipe()) = app.bigpipe();
            snprintf(bigpipe_name, sizeof(bigpipe_name), "%zu_%s_%zu",
                    static_cast<size_t>(app.id().id()),
                    processor.name().c_str(),
                    static_cast<size_t>(j));
            log_config->set_pipe_name(bigpipe_name);
            log_config->set_pipelet(0);
            add_pe->add_primary()->clear_id();
            for (uint64_t k = 0; k < backup_num; ++k) {
                backup_pe = add_pe->add_backups();
                backup_pe_id = backup_pe->mutable_backup_pe_id();
                backup_pe_id->set_id(add_pe->pe_id().id() + k);
                // set to 0 at first time
                backup_pe->set_last_assignment_time(0);
            }
        }
    }
    return true;
}

bool GetProcessor(const Application& app,
                  const ProcessorElement& pe,
                  Processor* processor) {
    const Topology& topology = app.topology();
    for (int i = 0; i < topology.processors_size(); ++i) {
        // const Processor& i_processor = topology.processors(i);
        if (topology.processors(i).id().id() == pe.processor().id()) {
            *processor = topology.processors(i);
            return true;
        }
    }
    return false;
}

bool HasPrimaryPE(const ProcessorElement& pe, BackupPEID* /*primary_id*/) {
    return (pe.primary_size() > 0 && pe.primary(0).has_id());
}

bool InitAppPath(const ClusterConfig& cluster_config, Application* app) {
    if (cluster_config.has_hdfs_path()) {
        FileConfig* app_file_path = app->mutable_app_path();
        app_file_path->set_username(app->user().username());
        app_file_path->set_password(app->user().password());
        std::string root_path = GetAppRootPath(cluster_config, *app);
        app_file_path->set_path(root_path + "app");

        FileConfig* check_point_path = app->mutable_checkpoint_path();
        *check_point_path = *app_file_path;
        check_point_path->set_path(root_path + "check_point");

        FileConfig* log_path = app->mutable_log_path();
        *log_path = * app_file_path;
        log_path->set_path(root_path + "log");
        return true;
    }
    return false;
}

std::string GetAppRootPath(const ClusterConfig& cluster_config,
                           const Application& app) {
    char path[4096];
    snprintf(path, sizeof(path), "%s/%s/%zu/",
            cluster_config.hdfs_path().c_str(),
            app.user().username().c_str(),
            static_cast<size_t>(app.id().id()));
    return path;
}

void RemovePEAssignment(const PNID& pn_id, ProcessorElement* pe) {
    int backup_size = pe->backups_size();
    for (int i = 0; i < backup_size; ++i) {
        BackupPE* backup_pe = pe->mutable_backups(i);
        if (backup_pe->has_pn_id() && backup_pe->pn_id().id() == pn_id.id()) {
            backup_pe->clear_pn_id();
            return;
        }
    }
}

void RemovePrimaryPE(const BackupPEID& primay_id, ProcessorElement* pe) {
    if (pe->primary_size() > 0) {
        BackupPEID* primay_pe_id = pe->mutable_primary(0);
        if (primay_pe_id->has_id() &&
            primay_id.has_id() &&
            primay_pe_id->id() == primay_id.id()) {
            primay_pe_id->clear_id();
        }
    }
}

void ConstructFakeAppClient(Application* app, int process_num, int base,
                            uint64_t memory, int paralism) {
    app->mutable_id()->set_id(256);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    topology->set_processor_num(process_num);
    char process_name[1000];
    for (int i = 0; i < process_num; ++i) {
        Processor* processor = topology->add_processors();
        snprintf(process_name, sizeof(process_name), "test_process_%d", i);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        if (i < 2) {
            processor->set_role(IMPORTER);
        } else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(base * (i + 1));
        processor->set_backup_num(paralism);
        if (i > 1) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            snprintf(process_name, sizeof(process_name), "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
        processor->set_pn_group("default");
        processor->set_cur_revision(0);
    }
    app->set_status(RUN);
}

void ConstructFakeSubTree(bool add, Application* app) {
    app->mutable_id()->set_id(256);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    topology->set_processor_num(1);
    char process_name[1000];
    Processor* processor = topology->add_processors();
    if (add) {
        timeval time;
        gettimeofday(&time, NULL);
        snprintf(process_name, sizeof(process_name), "test_process_3_%ld", time.tv_usec);
    } else {
        snprintf(process_name, sizeof(process_name), "test_process_2");
    }
    processor->set_name(process_name);
    processor->set_descr(process_name);
    processor->mutable_id()->set_id(3);
    processor->set_role(WORK_PROCESSOR);
    processor->set_exec_cmd("test_bin");
    Resource* resource = processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    processor->set_parallism(1);
    processor->set_backup_num(1);
    Subscribe* sub = processor->mutable_subscribe();
    ProcessorSub* process_sub = sub->add_processor();
    snprintf(process_name, sizeof(process_name), "test_process_1");
    process_sub->set_name(process_name);
    process_sub->set_tags("all");
    process_sub->set_username("test");
    process_sub->set_password("test");
    processor->set_pn_group("default");
    processor->set_cur_revision(0);
    app->set_status(RUN);
}

void ConstructFakeImporter(bool add, Application* app) {
    app->mutable_id()->set_id(256);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    for (int i = 0; i < 2; i++) {
        topology->set_processor_num(2);
        char process_name[1000];
        Processor* processor = topology->add_processors();
        if ( i == 0 ) {
            if (add) {
                snprintf(process_name, sizeof(process_name), "test_process_3");
            } else {
                snprintf(process_name, sizeof(process_name), "test_process_0");
            }
        } else {
            snprintf(process_name, sizeof(process_name), "test_process_2");
        }
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(3);
        if ( i == 0 ) {
            processor->set_role(IMPORTER);
        } else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(1);
        processor->set_backup_num(1);
        if ( i != 0 ) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            snprintf(process_name, sizeof(process_name), "test_process_3");
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
        processor->set_pn_group("default");
        processor->set_cur_revision(0);
    }
    app->set_status(RUN);
}

void ConstructFakeUpdate(Application* app) {
    app->mutable_id()->set_id(256);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    topology->set_processor_num(1);
    char process_name[1000];
    Processor* processor = topology->add_processors();
    snprintf(process_name, sizeof(process_name), "test_process_2");
    processor->set_name(process_name);
    processor->set_descr(process_name);
    processor->mutable_id()->set_id(2);
    processor->set_role(WORK_PROCESSOR);
    processor->set_exec_cmd("test_bin");
    Resource* resource = processor->mutable_resource_require();
    resource->set_cpu(0.5);
    resource->set_memory(1024 * 1024 * 1024);
    resource->set_disk(100000);
    resource->set_network(10000);
    processor->set_parallism(1);
    processor->set_backup_num(1);
    Subscribe* sub = processor->mutable_subscribe();
    ProcessorSub* process_sub = sub->add_processor();
    snprintf(process_name, sizeof(process_name), "test_process_1");
    process_sub->set_name(process_name);
    process_sub->set_tags("all");
    process_sub->set_username("test");
    process_sub->set_password("test");
    processor->set_pn_group("default");
    processor->set_cur_revision(0);
    app->set_status(RUN);
}

void ConstructFakeUpPara(Application* app) {
    app->mutable_id()->set_id(256);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    topology->set_processor_num(3);
    char process_name[1000];
    for (int i = 0; i < 3; ++i) {
        Processor* processor = topology->add_processors();
        snprintf(process_name, sizeof(process_name), "test_process_%d", i);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        if (i < 2) {
            processor->set_role(IMPORTER);
        } else {
            processor->set_role(WORK_PROCESSOR);
        }
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(3 * (i + 1));
        processor->set_backup_num(1);
        if (i > 1) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            snprintf(process_name, sizeof(process_name), "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
        processor->set_pn_group("default");
    }
    app->set_status(RUN);
}

// do not modify this method : because test case use this function
void ConstructFakeApp(Application* app,
                      int process_num /*=2*/,
                      int base /*= 2*/,
                      uint64_t /* memory */ /*= 1024 * 1024 * 1024*/,
                      int paralism /* =1 */) {
    timeval now;
    gettimeofday(&now, NULL);
    app->mutable_id()->set_id(now.tv_usec);
    app->set_name("test");
    app->set_descr("used for test");
    User* app_user = app->mutable_user();
    app_user->set_username("test");
    app_user->set_password("test");
    Topology* topology = app->mutable_topology();
    topology->set_processor_num(process_num);
    char process_name[1000];
    for (int i = 0; i < process_num; ++i) {
        Processor* processor = topology->add_processors();
        timeval time;
        gettimeofday(&time, NULL);
        snprintf(process_name, sizeof(process_name), "test_process_%d_%ld", i, time.tv_usec);
        processor->set_name(process_name);
        processor->set_descr(process_name);
        processor->mutable_id()->set_id(i);
        processor->set_role(WORK_PROCESSOR);
        processor->set_exec_cmd("test_bin");
        Resource* resource = processor->mutable_resource_require();
        resource->set_cpu(0.5);
        resource->set_memory(1024 * 1024 * 1024);
        resource->set_disk(100000);
        resource->set_network(10000);
        processor->set_parallism(base * (i + 1));
        processor->set_backup_num(paralism);
        if (i > 0) {
            Subscribe* sub = processor->mutable_subscribe();
            ProcessorSub* process_sub = sub->add_processor();
            snprintf(process_name, sizeof(process_name), "test_process_%d", i - 1);
            process_sub->set_name(process_name);
            process_sub->set_tags("all");
            process_sub->set_username("test");
            process_sub->set_password("test");
        }
        processor->set_pn_group("default");
        processor->set_cur_revision(0);
    }
    app->set_status(RUN);
}

void GetPNPEMap(const std::list<ProcessorElement>& pe_list,
                PnidPeSetMap* pn_pe_map) {
    typedef std::list<ProcessorElement>::const_iterator PE_PELIST_CONST_ITER;
    PE_PELIST_CONST_ITER pe_end = pe_list.end();
    for (PE_PELIST_CONST_ITER pe_iter = pe_list.begin();
         pe_iter != pe_end; ++pe_iter) {
        for (int i = 0; i < pe_iter->backups_size(); ++i) {
            const BackupPE& back_pe = pe_iter->backups(i);
            if (back_pe.has_pn_id()) {
                const PNID& pn_id = back_pe.pn_id();
                PnidPeSetMapIter pn_pe_find = pn_pe_map->find(pn_id);
                if (pn_pe_find == pn_pe_map->end()) {
                    PeSet pe_set;
                    pe_set.insert(*pe_iter);
                    (*pn_pe_map)[pn_id] = pe_set;
                } else {
                    pn_pe_find->second.insert(*pe_iter);
                }
            }
        }
    }
}

ERROR_CODE GetUser(const ClusterConfig& cluster_config, std::set<User, CompareUser>* userinfo) {
    int user_num = cluster_config.users_size();
    for ( int i = 0; i < user_num; ++i ) {
        userinfo->insert(cluster_config.users(i));
    }
    return error::OK;
}

ERROR_CODE UpdateUser(ClusterConfig& cluster_config, const User& add_user) {
    int user_num = cluster_config.users_size();
    for (int i = 0; i < user_num; ++i) {
        if (cluster_config.users(i).username() == add_user.username()) {
            if (cluster_config.users(i).password() == add_user.password()) {
                return error::CLIENT_USER_OP_FAIL;
            } else {
                User* user = cluster_config.mutable_users(i);
                user->set_password(add_user.password());
            }
        }
    }
    User* user = cluster_config.add_users();
    user->set_username(add_user.username());
    user->set_password(add_user.password());
    return error::OK;
}

ERROR_CODE AddUser(ClusterConfig& cluster_config, const User& add_user) {
    int user_num = cluster_config.users_size();
    for ( int i = 0; i < user_num; ++i ) {
        if (cluster_config.users(i).username() == add_user.username()) {
            return error::CLIENT_USER_OP_FAIL;
        }
    }
    User* user = cluster_config.add_users();
    user->set_username(add_user.username());
    user->set_password(add_user.password());
    return error::OK;
}

ERROR_CODE DeleteUser(ClusterConfig& cluster_config, const User& delete_user) {
    ClusterConfig backup_cluster_config;
    bool flag = false;
    int user_num = cluster_config.users_size();
    for ( int i = 0; i < user_num; ++i ) {
        if (cluster_config.users(i).username() == delete_user.username()) {
            flag = true;
            break;
        }
    }
    if (flag) {
        backup_cluster_config.CopyFrom(cluster_config);
        cluster_config.clear_users();
        int backup_user_num = backup_cluster_config.users_size();
        for ( int i = 0; i < backup_user_num; ++i ) {
            if (backup_cluster_config.users(i).username() != delete_user.username()) {
                cluster_config.add_users()->CopyFrom(backup_cluster_config.users(i));
            }
        }
        return error::OK;
    }
    return error::OK;
}

std::string LowerString(const std::string& str) {
    std::string res = str;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

int32_t SplitString(const std::string& str, std::vector<std::string>* vec) {
    return SplitStringEx(str, '/', vec);
}

int32_t SplitStringEx(const std::string& str, const char token, std::vector<std::string>* vec) {
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    int32_t i;
    for (i = 0; ; i++) {
        while (beg != std::string::npos && str[beg] == token) {
            beg++;
        }
        end = str.find_first_of(token, beg);
        if (end == std::string::npos) {
            break;
        }
        if (end != beg) {
            if (!str.substr(beg, end - beg).empty()) {
                vec->push_back(str.substr(beg, end - beg));
            }
        }
        beg = end + 1;
    }

    if (!str.substr(beg, end - beg).empty()) {
        vec->push_back(str.substr(beg));
        i++;
    }

    return i;
}

uint64_t GetAppID(const char* str) {
    std::string str_path(str);
    std::vector<std::string> split_path;
    if (SplitString(str_path, &split_path) <= 2) {
        return 0;
    }
    return strtoul(split_path[2].c_str(), NULL, 10);
}

uint64_t GetProcessElementID(const char* str) {
    std::string str_path(str);
    std::vector<std::string> split_path;
    if (SplitString(str_path, &split_path) <= 3) {
        return 0;
    }
    return strtoul(split_path[3].c_str(), NULL, 10);
}


ERROR_CODE ParseStringToNumberList(const std::string& str, std::vector<int>* num_list) {
    ERROR_CODE ret = error::OK;
    std::string prase_str = str;
    std::vector<std::string> str_list;

    // parse ',' first
    if (SplitStringEx(prase_str, ',', &str_list) <= 0) {
        ret = error::BAD_ARGUMENT;
        goto ERR_RET;
    }
    num_list->clear();
    for (std::vector<std::string>::iterator it = str_list.begin();
         it != str_list.end(); it++) {
        std::vector<std::string> sub_str_list;
        int parse_res = SplitStringEx(*it, '-', &sub_str_list);
        if (parse_res != 2 && parse_res != 1) {
            ret = error::BAD_ARGUMENT;
            goto ERR_RET;
        }

        int begin_idx = strtoull(sub_str_list[0].c_str(), NULL, 10);
        if (parse_res == 1) { // single number
            num_list->push_back(begin_idx);
        } else { // multi-number
            int end_idx = strtoull(sub_str_list[1].c_str(), NULL, 10);
            for (int i = begin_idx; i <= end_idx; ++i) {
                num_list->push_back(i);
            }
        }
    }

ERR_RET:
    return ret;
}

void GetProcessInfo(std::map<pid_t, Resource>* pid_res_map, char* buffer /*= NULL*/) {
    std::string cmd = "ps ux | awk '{if(NF>1)print $2,$3,$6}'";
    char* buf = buffer;
    bool allocate_buf = false;
    if (NULL == buf) {
        allocate_buf = true;
        buf = new char[kMaxReturnBufferLen];
    }
    HdfsClient::HPCmdExec(cmd, 1, buf);
    std::vector<std::string> lines;
    if (SplitStringEx(buf, '\n', &lines) != 0) {
        for (int i = 1, line_num = lines.size(); i < line_num; ++i) {
            std::vector<std::string> items;
            if (SplitStringEx(lines[i], ' ', &items) != 0) {
                Resource res;
                res.set_cpu(atof(items[1].c_str()));
                res.set_memory(atol(items[2].c_str()));
                (*pid_res_map)[atoi(items[0].c_str())] = res;
            }
        }
    }
    if (allocate_buf) {
        delete [] buf;
    }
}

// Get CPU/Memory information through cgroup proc system(/cgroup)
void GetCGProcessInfo(std::map<pid_t, Resource>* pid_res_map) {
}

// return the app can schedule
bool AppCanScheduler(const Application& app) {
    if (app.status() == SUBMIT || app.status() == KILL) {
        return false;
    }
    return true;
}

} // namespace application_tools
} // namespace dstream
