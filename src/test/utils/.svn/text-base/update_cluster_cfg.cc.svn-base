#include "metamanager/meta_manager.h"
#include "metamanager/zk_meta_manager.h"
#include "metamanager/zk_config.h"

using namespace dstream::zk_meta_manager;
using namespace dstream::config;
using namespace dstream::zk_config;
using namespace dstream::meta_manager;
using namespace dstream;

int main(int argc, char** argv)
{
    logger::initialize("update meta manager");
    if (argc != 3) {
        DSTREAM_WARN("usage : update_cluster_config config_file username");
        return 1;
    }
    Config config;
    if (config.ReadConfig(argv[1])) {
        DSTREAM_WARN("read config %s fail", argv[1]);
        return 1;
    }
    if (!meta_manager::MetaManager::InitMetaManagerContext(&config)) {
        DSTREAM_WARN("init manager fail");
        return 1;
    }
    meta_manager::MetaManager* meta = meta_manager::MetaManager::GetMetaManager();
    ClusterConfig cluster_config;
    User* add_user = cluster_config.add_users();
    add_user->set_username(argv[2]);
    add_user->set_password(argv[2]);
    if (meta->UpdateClusterConfig(cluster_config) != error::OK) {
        DSTREAM_WARN("update cluster config fail");
    } else {
        DSTREAM_INFO("update cluster config success");
    }
    meta_manager::MetaManager::DestoryMetaManager();
}
