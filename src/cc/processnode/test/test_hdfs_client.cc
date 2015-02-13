#include "gtest/gtest.h"
#include "common/logger.h"
#include "common/utils.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/zk_client.h"

#include "processnode/config_map.h"
#include "common/hdfs_client.h"

using namespace std;
using namespace dstream;
using namespace dstream::processnode;
using namespace dstream::hdfs_client;

namespace dstream {
namespace processnode {

string hdfs_unit_test_dir("/test/test_hdfs_unitest");
string remote_file_path("/test/test_hdfs_unitest");
std::string hdfs_client_dir;
int32_t hdfs_retry;

class TestHDFSClient : public ::testing::Test
{
    static const uint32_t kFileCount = 4;
protected:
    int32_t InitTest(string& sample_file);
    int32_t DeinitTest(const string& sample_file, const string& remote_file);
    int32_t TestFileTest();
    int32_t UploadFileTest(const string& local_file, const string& remote_file);
    int32_t UploadFileOverwriteTest(const string& local_file, const string& remote_file);
    int32_t ListFileTest(const string& remote_file);
};

int32_t TestHDFSClient::InitTest(string& sample_file) {
    /* create sample directory & structs */
    string hdfs_test_dir("local_hdfs_test");
    string create_dir_cmd("mkdir -p ");
    create_dir_cmd.append(hdfs_test_dir);
    DSTREAM_DEBUG("create sample directory, exec cmd : [%s]", create_dir_cmd.c_str());
    if (system(create_dir_cmd.c_str()) == -1) {
        return error::FAILED_EXPECTATION;
    }

    uint32_t count;
    string create_item_cmd("touch ");
    for (count = 0; count < kFileCount; ++count) {
        time_t timer = time(NULL);
        string file_prefix("sample_file_");
        StringAppendNumber(&file_prefix, timer);
        string create_item_cmd_t = create_item_cmd + hdfs_test_dir + "/" + file_prefix;
        DSTREAM_DEBUG("create sample file item, exec cmd : [%s]", create_item_cmd_t.c_str());
        if (system(create_item_cmd_t.c_str()) == -1) {
            break;
        }
        sleep(1); // wait 1 second
    }

    if (count != kFileCount) {
        return error::FAILED_EXPECTATION;
    }

    sample_file = hdfs_test_dir;
    return error::OK;
}

int32_t TestHDFSClient::DeinitTest(const string& sample_file, const string& remote_file) {
    int32_t ret = error::OK;

    /* remove remote files */
    HdfsClient hp(hdfs_client_dir, hdfs_retry);
    ret = hp.RemoveFile(remote_file);

    /* remove local files */
    string rm_dir_cmd("rm -rf ");
    rm_dir_cmd.append(sample_file);
    DSTREAM_DEBUG("deinit test, exec cmd : [%s]", rm_dir_cmd.c_str());
    if (system(rm_dir_cmd.c_str()) == -1) {
        ret = error::FAILED_EXPECTATION;
    }

    return ret;
}

int32_t TestHDFSClient::TestFileTest() {
    int32_t ret = error::OK;

    HdfsClient hp(hdfs_client_dir, hdfs_retry);

    hp.RemoveFile(remote_file_path);

    bool exist = true;
    ret = hp.TestFile(hdfs_unit_test_dir, exist);
    if (ret != error::OK || exist) {
        ret = error::FAILED_EXPECTATION;
    }
    return ret;
}


int32_t TestHDFSClient::UploadFileTest(const string& local_file, const string& remote_file) {
    int32_t ret;

    HdfsClient hp(hdfs_client_dir, hdfs_retry);

    hp.RemoveFile(hdfs_unit_test_dir);

    string hdfs_test_dir(remote_file);
    ret = hp.UploadFile(local_file, hdfs_test_dir, 0);
    if (ret != error::OK) {
        return ret;
    }
    bool exist = false;
    ret = hp.TestFile(hdfs_test_dir, exist);
    if (ret != error::OK || !exist) {
        ret = error::FAILED_EXPECTATION;
    }
    return ret;
}

int32_t TestHDFSClient::UploadFileOverwriteTest(const string& local_file,
                                                const string& remote_file) {
    int32_t ret;

    HdfsClient hp(hdfs_client_dir, hdfs_retry);

    string hdfs_test_dir(remote_file);
    ret = hp.UploadFile(local_file, hdfs_test_dir, true);
    if (ret != error::OK) {
        return ret;
    }
    bool exist = false;
    ret = hp.TestFile(hdfs_test_dir, exist);
    if (ret != error::OK || !exist) {
        ret = error::FAILED_EXPECTATION;
    }
    return ret;
}


int32_t TestHDFSClient::ListFileTest(const string& remote_file) {
    int32_t ret = error::OK;

    HdfsClient hp(hdfs_client_dir, hdfs_retry);

    std::vector<std::string> file_list;
    ret = hp.ListFile(remote_file, &file_list);
    if (file_list.size() != kFileCount) {
        ret = error::FAILED_EXPECTATION;
    }

    for (vector<string>::iterator it = file_list.begin(); it != file_list.end(); ++it) {
        DSTREAM_DEBUG("hdfs file : %s", (*it).c_str());
    }

    return ret;
}


TEST_F(TestHDFSClient, test_testfile)
{
    // file is not exist
    ASSERT_EQ(error::OK, TestFileTest());
}

TEST_F(TestHDFSClient, test_uploadfile)
{
    // create local file struct
    string file_path;
    ASSERT_EQ(error::OK, InitTest(file_path));
    // upload file to hdfs
    ASSERT_EQ(error::OK, UploadFileTest(file_path, remote_file_path));
    // upload overwrite file to hdfs
    ASSERT_EQ(error::OK, UploadFileOverwriteTest(file_path, remote_file_path));
    // remove local file struct
    ASSERT_EQ(error::OK, DeinitTest(file_path, remote_file_path));
}

TEST_F(TestHDFSClient, test_listfile)
{
    // create local file struct
    string file_path;
    ASSERT_EQ(error::OK, InitTest(file_path));
    // upload file to hdfs
    ASSERT_EQ(error::OK, UploadFileTest(file_path, remote_file_path));
    // list files on hdfs
    ASSERT_EQ(error::OK, ListFileTest(remote_file_path));
    // remove local file struct
    ASSERT_EQ(error::OK, DeinitTest(file_path, remote_file_path));
}


} // end processnode
} // end dstream

int main(int argc, char** argv) {
    int32_t ret = error::OK;

    ::dstream::logger::initialize("test_hdfs_client");
    ::testing::InitGoogleTest(&argc, argv);

    // read conf file
    ret = g_pn_cfg_map.InitCfgMapFromFile("./test_hdfs_client.conf.xml");
    if (ret < error::OK) {
        DSTREAM_WARN("fail to read config from['./test_hdfs_client.conf.xml']");
        return 1;
    }

    g_pn_cfg_map.GetValue(config_const::kHDFSClientDirName, &hdfs_client_dir);
    g_pn_cfg_map.GetIntValue(config_const::kHDFSRetryName, &hdfs_retry);

    return RUN_ALL_TESTS();
}
