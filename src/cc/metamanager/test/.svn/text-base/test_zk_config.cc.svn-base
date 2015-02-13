#include "metamanager/zk_config.h"
#include "common/config_const.h"

using namespace dstream::zk_config;
using namespace dstream::config_const;

int main()
{
    ZKConfig zk_config;
    zk_config.ReadConfig("test_zk_meta_config.xml");
    zk_config.SetValue(kPMRootPathName, "/DStream/test/pm");
    zk_config.SetValue(kPNRootPathName, "/DStream/test/pn");
    zk_config.SetValue(kZKPathName, "10.224.118.14:9345,10.224.18.11:9345,10.224.16.30:9345");
    zk_config.SetValue(kConfigRootPathName, "/DStream/test/config");
    zk_config.SetValue(kAppRootPathName, "/DStream/test/app");
    zk_config.SetValue(kZKTimeoutName, "10");
}
