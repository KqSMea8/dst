#include "pm_test_common.h"

#include "client_op.h"

namespace dstream {
namespace pm_test_common {
using namespace meta_manager;
using namespace client_op;
using namespace config_const;

//common function used for pm test
int GetPrimaryPMIndex(ProcessMaster* pm, int pm_count) {
    for (int i = 0; i < pm_count; ++i) {
        if (pm[i].working_master()) {
            return i;
        }
    }
    return -1;
}

int GetAppPENum(const Application& app) {
    int pe_num = 0;
    const Topology& topology = app.topology();
    for (int i = 0; i < topology.processors_size(); ++i) {
        const Processor& processor = topology.processors(i);
        pe_num += (processor.backup_num() * processor.parallism());
    }
    return pe_num;
}

//end of common functions

void PMTestBase::SetUp() {
    InitMeta(kZKMetaManagerName, &old_cluster_config_);
    meta_ = (ZKMetaManager*)MetaManager::GetMetaManager();
    ClearContext();
}

void PMTestBase::TearDown() {
    if (old_cluster_config_.users_size() > 0) {
        meta_->UpdateClusterConfig(old_cluster_config_);
    }
    ClearContext();
    meta_->DeletePNGroups();
    MetaManager::DestoryMetaManager();
    meta_ = 0;
}

void PMTestBase::ClearContext() {
    ClearApp();
    meta_->DeletePM();
    meta_->DeletePMMachines();
    meta_->DeletePNMachines();
    list<PN> pn_list;
    meta_->GetPNList(&pn_list);
    for (list<PN>::iterator pn_it = pn_list.begin();
         pn_it != pn_list.end(); ++pn_it) {
        meta_->DeletePN(*pn_it);
    }
}

} //namespace pm_test_common
} //namespace dstream
