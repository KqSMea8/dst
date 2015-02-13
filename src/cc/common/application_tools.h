/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/12
* @filename:    application_tools.h
* @author:      zhanggengxin
* @brief:       application tools
*/

#ifndef __DSTREAM_CC_PROCESSMASTER_APPLICATION_TOOLS_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_APPLICATION_TOOLS_H__ // NOLINT

#include <list>
#include <map>
#include <set>
#include <vector>
#include "common/dstream_type.h"
#include "common/proto/application.pb.h"
#include "common/proto/cluster_config.pb.h"

namespace dstream {
namespace application_tools {

#define ERROR_CODE dstream::error::Code

inline bool cmp_backup_pe_id(const BackupPEID& backup_pe_id1,
                             const BackupPEID& backup_pe_id2) {
    return backup_pe_id1.id() < backup_pe_id2.id();
}

inline bool cmp_pn_id(const PNID& pn_id1, const PNID& pn_id2) {
    return pn_id1.id() < pn_id2.id();
}

inline bool cmp_user(const User& user1, const User& user2) {
    return user1.username() < user2.username();
}
inline bool pe_id_equal(const PEID& pe_id1, const PEID& pe_id2) {
    return pe_id1.id() == pe_id2.id();
}

/************************************************************************/
/* class ComprePE compare process, used to remove duplicated pe         */
/************************************************************************/
class ComparePE {
public:
    bool operator() (const ProcessorElement& pe1, const ProcessorElement& pe2) {
        return (pe1.app().id() < pe2.app().id() ||
                pe1.pe_id().id() < pe2.pe_id().id());
    }
};

typedef std::set<ProcessorElement, ComparePE> PeSet;
typedef PeSet::iterator PeSetIter;

class PEIDCompare {
public:
    bool operator()(const PEID& pe_id1, const PEID& pe_id2) const {
        return pe_id1.id() < pe_id2.id();
    }
};

class AppIDCompare {
public:
    bool operator()(const AppID& app_id1, const AppID& app_id2) {
        return app_id1.id() < app_id2.id();
    }
};

class ProcessorIDCompare {
public:
    bool operator()(const ProcessorID& processor_id1, const ProcessorID& processor_id2) {
        return processor_id1.id() < processor_id2.id();
    }
};

/************************************************************************/
/* compare two user                                                     */
/************************************************************************/
class CompareUser {
public:
    bool operator()(const User& user1, const User& user2) {
        return cmp_user(user1, user2);
    };
};

// type define
typedef std::map<PNID, PeSet, PNIDCompare> PnidPeSetMap;
typedef std::pair<PNID, PeSet> PnidPeSetMapPair;
typedef PnidPeSetMap::iterator PnidPeSetMapIter;

typedef std::set<PEID, PEIDCompare> PeidSet;
typedef PeidSet::iterator PeidSetIter;

// Get App ID from pe id
inline uint64_t app_id(const PEID& peid) {
    return peid.id() >> 40;
}
// construct uniq Processor id from pe id
ProcessorID ConstructProcessorID(const PEID& pe_id);
ProcessorID ConstructProcessorID(const AppID& app_id, const ProcessorID& id);

void AddPEAssignment(const BackupPEID& backup_pe_id,
                     const PNID& pn_id,
                     ProcessorElement* pe);

bool CheckPENormal(const ProcessorElement& pe,
                   const AppID& app_id,
                   const PEID& pe_id);

bool CreateAppPEInstance(const Application& app,
                         const ClusterConfig& cluster_config,
                         const config::Config* config,
                         std::list<ProcessorElement>* pe_list);

bool GetProcessor(const Application& app, const ProcessorElement& pe, Processor* processor);
bool HasPrimaryPE(const ProcessorElement& pe, BackupPEID* primary_id);
bool InitAppPath(const ClusterConfig& cluster_config, Application* app);
void RemovePEAssignment(const PNID& pn_id, ProcessorElement* pe);
void RemovePrimaryPE(const BackupPEID& primay_id, ProcessorElement* pe);
void GetPNPEMap(const std::list<ProcessorElement>& pe_list, PnidPeSetMap* pn_pe_map);
std::string GetAppRootPath(const ClusterConfig& cluster_config, const Application& app);
std::string LowerString(const std::string& str);
int32_t SplitString(const std::string& str, std::vector<std::string>* vec);
int32_t SplitStringEx(const std::string& str, const char token, std::vector<std::string>* vec);

// used for pn watch
uint64_t GetAppID(const char* str);
uint64_t GetProcessElementID(const char* str);

ERROR_CODE ParseStringToNumberList(const std::string& str, std::vector<int>* num_list);


// userd for user operation
ERROR_CODE GetUser(const ClusterConfig& cluster_config, std::set<User, CompareUser>* userinfo);
ERROR_CODE AddUser(ClusterConfig& cluster_config, const User& user);
ERROR_CODE UpdateUser(ClusterConfig& cluster_config, const User& user);
ERROR_CODE DeleteUser(ClusterConfig& cluster_config, const User& user);

void ConstructFakeAppClient(Application* app, int process_num = 3, int base = 2,
                            uint64_t memory = 1024 * 1024 * 1024, int paralism = 1);
void ConstructFakeSubTree(bool add, Application* app);
void ConstructFakeImporter(bool add, Application* app);
void ConstructFakeUpPara(Application* app);
void ConstructFakeUpdate(Application* app);
// used for test : construct an fake app
void ConstructFakeApp(Application* app, int process_num = 2, int base = 2,
                      uint64_t memory = 1024 * 1024 * 1024, int paralism = 1);
// Get CPU/Memrory information of all processor
void GetProcessInfo(std::map<pid_t, Resource>* pid_res_map, char* buffer = NULL);

// Get CPU/Memory information through cgroup proc system(/cgroup)
void GetCGProcessInfo(std::map<pid_t, Resource>* pid_res_map);

// return the app can schedule
bool AppCanScheduler(const Application& app);

} // namespace application_tools
} // namespace dstream

#endif // NOLINT
