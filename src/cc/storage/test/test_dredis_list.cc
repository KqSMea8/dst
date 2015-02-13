#include <string>
#include <vector>
#include <deque>
#include <boost/shared_ptr.hpp>
#include "storage/dredis_sa.h"
#include "storage/dredis_list.h"
#include "redis_env.h"
#include "common/logger.h"
#include <gtest/gtest.h>

using namespace dredis;
using namespace dstream;

const std::string g_ListTableName = "__UnitTestListTable";

int main(int argc, char** argv) {
    logger::initialize("SA");//dstream log init
    //run redis sentinel, master, slave
    EnvTearDown();
    EnvSetUp();
    int ret = 0;
    if ((StartRedisMasterServer(g_RedisMasterCfgWithAuth) == 0) &&
        (StartRedisSlaveServer(g_RedisSlaveCfgWithAuth) == 0) &&
        (StartSentinelServer(g_SentinelCfgWithAuth) == 0)) {
        sleep(1); //wait redis start up
        testing::InitGoogleTest(&argc, argv);
        ret = RUN_ALL_TESTS();
    }
    //EnvTearDown();
    return ret;
}

/**
 * @brief
**/
class test_RedisListTable_CreateTable_suite : public ::testing::Test {
protected:
    test_RedisListTable_CreateTable_suite() {};
    virtual ~test_RedisListTable_CreateTable_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RedisListTable_CreateTable_suite, *)
        ASSERT_EQ(rsa_.Init(g_SentinelIpport, g_RedisMasterNameForSentinel,
                            3, 20000, g_RedisMasterPasswd), storage::OK);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RedisListTable_CreateTable_suite, *)
    };

    void CreateListTable(const std::string& table_name, bool replace_old,
                         storage::RedisListTablePtr* plist);
public:
    storage::DRedisSA rsa_;
};

void test_RedisListTable_CreateTable_suite::CreateListTable (
    const std::string& table_name, bool replace_old,
    storage::RedisListTablePtr* plist) {
    ASSERT_TRUE(NULL != plist);
    storage::IStoreTablePtr pt;
    ASSERT_EQ(rsa_.CreateTable(table_name,
                               storage::REDIS_LIST_TABLE,
                               replace_old, &pt),
              storage::OK);
    *plist = boost::dynamic_pointer_cast<storage::RedisListTable>(pt);
}

TEST_F(test_RedisListTable_CreateTable_suite, CreateListTable) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    uint64_t n;
    ASSERT_EQ(plist->Count(&n), storage::OK);
    printf("count of list[%s] is [%lu]\n", g_ListTableName.c_str(), n);
}

TEST_F(test_RedisListTable_CreateTable_suite, PushAndPop) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    //pop front
    std::string s;
    ASSERT_EQ(plist->PopFront(&s), storage::OK);
    //push
    const size_t num = 1024;
    char array[num];
    srandom(time(NULL));
    for (size_t i = 0; i < num; i++) {
        array[i] = random() % 127;
    }
    s.assign((char*)array, sizeof(array));
    ASSERT_EQ(plist->PushFront(s), storage::OK);
    //pop back
    s.clear();
    ASSERT_EQ(plist->PopFront(&s), storage::OK);
    ASSERT_EQ(sizeof(array), s.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(array[i], s.at(i));
    }

    for (size_t i = 0; i < num; i++) {
        array[i] = random() % 127;
    }
    //push back
    s.clear();
    s.assign((char*)array, sizeof(array));
    ASSERT_EQ(plist->PushBack(s), storage::OK);
    //pop back
    s.clear();
    ASSERT_EQ(plist->PopBack(&s), storage::OK);
    ASSERT_EQ(sizeof(array), s.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(array[i], s.at(i));
    }
}

TEST_F(test_RedisListTable_CreateTable_suite, GetFrontAnd_Back) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    std::string s;
    //get
    ASSERT_EQ(plist->GetFront(&s), storage::OK);
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(plist->GetBack(&s), storage::OK);
    ASSERT_TRUE(s.empty());
    //push
    const size_t num = 1024;
    char array[num];
    srandom(time(NULL));
    for (size_t i = 0; i < num; i++) {
        array[i] = random() % 127;
    }
    s.assign((char*)array, sizeof(array));
    ASSERT_EQ(plist->PushFront(s), storage::OK);

    //get front
    s.clear();
    ASSERT_EQ(plist->GetFront(&s), storage::OK);
    ASSERT_EQ(s.size(), sizeof(array));
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(s[i], array[i]);
    }

    //get back
    s.clear();
    ASSERT_EQ(plist->GetBack(&s), storage::OK);
    ASSERT_EQ(s.size(), sizeof(array));
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(s[i], array[i]);
    }
}

TEST_F(test_RedisListTable_CreateTable_suite, BatchPushAndPop_Front) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_values;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //push
    org_values.resize(num);
    FillRandomKeys(&org_values);
    ASSERT_EQ(plist->PushFront(org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }

    //pop
    ASSERT_EQ(plist->PopFront(num, &ret_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), ret_values.size());
    ASSERT_EQ(ret_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(org_values[i], ret_values[num - i - 1]);
    }
}

TEST_F(test_RedisListTable_CreateTable_suite, BatchPushAndPop_Back) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_values;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //push
    org_values.resize(num);
    FillRandomKeys(&org_values);
    ASSERT_EQ(plist->PushBack(org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }

    //pop
    ASSERT_EQ(plist->PopBack(num, &ret_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), ret_values.size());
    ASSERT_EQ(ret_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(org_values[i], ret_values[num - i - 1]);
    }
}

int GetAllCb(void* ctx, int err, const std::string& data) {
    if (0 != err) {
        printf("get error reply: [%d][%s]\n", err, data.c_str());
        return -1;
    }
    std::deque<std::string>* ret_values = (std::deque<std::string>*)ctx;
    ret_values->push_front(data);//revert the order
    return 0;
}

TEST_F(test_RedisListTable_CreateTable_suite, GetAll) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());

    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_values;
    std::deque<std::string> ret_values;
    std::vector<int> results;
    //push
    org_values.resize(num);
    FillRandomKeys(&org_values);
    ASSERT_EQ(plist->PushFront(org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }

    //get all
    ASSERT_EQ(plist->GetAll(GetAllCb, &ret_values), storage::OK);
    ASSERT_EQ(ret_values.size(), org_values.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_TRUE(org_values[i] == ret_values[i]);
    }
}

TEST_F(test_RedisListTable_CreateTable_suite, Clear) {
    storage::RedisListTablePtr plist;
    CreateListTable(g_ListTableName, true, &plist);
    ASSERT_TRUE(NULL != plist.get());
    //clear
    ASSERT_EQ(plist->Clear(), storage::OK);

    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_values;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //insert
    org_values.resize(num);
    FillRandomKeys(&org_values);
    ASSERT_EQ(plist->PushFront(org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }

    //clear
    uint64_t n;
    ASSERT_EQ(plist->Count(&n), storage::OK);
    printf("before clear: count of list[%s] is [%zu]\n", g_ListTableName.c_str(), n);
    ASSERT_EQ(plist->Clear(), storage::OK);
    ASSERT_EQ(plist->Count(&n), storage::OK);
    //check again
    printf("after clear: count of list[%s] is [%zu]\n", g_ListTableName.c_str(), n);
}
