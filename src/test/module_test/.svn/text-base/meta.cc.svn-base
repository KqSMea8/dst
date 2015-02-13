#include "logger.h"
#include "memory_meta_manager.h"

using namespace dstream::logger;
using namespace dstream::memory_meta_manager;
using namespace dstream;

int main()
{
    initialize("meta");
    ClusterConfig config;
    User* add_user = config.add_users();
    *(add_user->mutable_name()) = "test";
    *(add_user->mutable_password()) = "test";
    MemoryMetaManager meta_manager;
    if (meta_manager.UpdateClusterConfig(config) != error::OK) {
        DSTREAM_WARN("update cluster config fail");
        return -1;
    }
    DSTREAM_INFO("update cluster config success, add user test");
    PN pn1;
    PNID pn_id;
    *(pn_id.mutable_id()) = "127.0.0.1_1";
    *(pn1.mutable_host()) = "127.0.0.1";
    *(pn1.mutable_pn_id()) = pn_id;
    pn1.set_pub_port(12334);
    pn1.set_rpc_server_port(12345);
    if (meta_manager.AddPN(pn1) != error::OK) {
        DSTREAM_WARN("add pn %s fail", pn1.pn_id().id().c_str());
        return -1;
    }
    DSTREAM_INFO("add pn %s success", pn1.pn_id().id().c_str());
    PN pn2 = pn1;
    *(pn_id.mutable_id()) = "127.0.0.1_2";
    *(pn2.mutable_pn_id()) = pn_id;
    pn2.set_rpc_server_port(17890);
    pn2.set_pub_port(34566);
    if (meta_manager.AddPN(pn2) != error::OK) {
        DSTREAM_WARN("add pn %s fail", pn2.pn_id().id().c_str());
        return -1;
    }
    DSTREAM_INFO("add pn %s success", pn2.pn_id().id().c_str());
    list<PN> pn_list;
    if (meta_manager.GetPNList(pn_list) != error::OK) {
        DSTREAM_WARN("get pn list error");
        return -1;
    }
    DSTREAM_INFO("Get pn list success, pn is:");
    for (list<PN>::iterator iter = pn_list.begin(); iter != pn_list.end(); ++iter) {
        DSTREAM_INFO("pn id:%s host_name:%s rpc_server_port %d", iter->pn_id().id().c_str(),
                     iter->host().c_str(), iter->rpc_server_port());
    }
}
