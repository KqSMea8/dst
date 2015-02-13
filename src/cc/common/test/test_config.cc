#include <cstdio>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include "gtest/gtest.h"
#include "common/error.h"
#include "common/config.h"

using namespace std;
using namespace dstream;
using namespace config;
struct Param {
    Param(string name, string value, bool OK): key_(name), value_(value), OK_(OK) {
    }
    string key_;
    string value_;
    bool OK_;
};

class TestConfigParamTestOK: public ::testing::TestWithParam<Param> {
public:
    void SetUp() {
        config = new Config();
        config->ReadConfig("./test_config_xmls/test_config_1.xml");
    }
    void TearDown() {
        delete config;
    }

protected:
    static Config* config;
};

Config* TestConfigParamTestOK::config = NULL;

TEST_P(TestConfigParamTestOK, test_get_value) {
    const Param& param = GetParam();
    std::string value;
    if (param.OK_) {
        ASSERT_EQ(error::OK, config->GetValue(param.key_, &value));
    } else {
        ASSERT_NE(error::OK, config->GetValue(param.key_, &value));
    }
    ASSERT_EQ(param.value_, value);

}
INSTANTIATE_TEST_CASE_P(GetValueTest, TestConfigParamTestOK ,
                        testing::Values(
                            Param("Client.User.UserName", "demo", true),
                            Param("Client.PM.TimeoutSec", "-1"  , true),
                            Param("sss", "", false),
                            Param("HDFS", "", true), //TODO: Check this..
                            Param("Client.PM.TimeoutSec", "-1", true),
                            Param("Client.PM", "", true)
                        )
                       );
TEST_P(TestConfigParamTestOK, test_get_set) {
    const Param& param = GetParam();
    string value;
    if (error::OK == config->SetValue(param.key_, param.value_, true)) {
        ASSERT_EQ(error::OK, config->GetValue(param.key_, &value));
        ASSERT_EQ(param.value_, value);
    }
}
class TestConfig : public testing::Test {

};


TEST_F(TestConfig, test_open_failed) {
    Config config;
    ASSERT_NE(error::OK, config.ReadConfig(""));
    ASSERT_NE(error::OK, config.ReadConfig("./xxx/xxx/xxx.xml"));
    ASSERT_NE(error::OK, config.ReadConfig("./test_config_xmls/bad_config_1.xml"));
    ASSERT_NE(error::OK, config.ReadConfig("./test_config_xmls/bad_config_2.xml"));
    ASSERT_EQ(error::OK, config.ReadConfig("./test_config_xmls/test_config_1.xml"));
}

TEST_F(TestConfig, test_get_children) {
    Config config;
    config::ConfigNode::NodeList node_list;
    ASSERT_EQ(error::OK, config.ReadConfig("./test_config_xmls/test_config_2.xml"));
    ConfigNode node = config.GetNode("Client");
    ASSERT_EQ(false, node.IsErrorNode());
    node_list = node.GetChildrenList("User");
    ASSERT_EQ(4u, node_list.size());
}

int main(int argc, char** argv) {
    ::logger::initialize("test_config");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
