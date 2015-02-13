#include <string>
#include <boost/shared_ptr.hpp>
#include "storage/dredis_sa.h"
#include "redis_env.h"
#include "common/logger.h"
#include <gtest/gtest.h>

using namespace dredis;
using namespace dstream;

const std::string g_WindowsListTableName = "__UnitTestWindowsListTable";
const std::string g_SliceListTableName = "__UnitTestSliceListTable";
const std::string g_SliceHashTableName = "__UnitTestSliceHashTable";

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
class test_DRedisSA_Init_suite : public ::testing::Test {
protected:
    test_DRedisSA_Init_suite() {};
    virtual ~test_DRedisSA_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DRedisSA_Init_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DRedisSA_Init_suite, *)
        //ASSERT_EQ(StopRedisMasterServer(), 0);
    };
public:
};

TEST_F(test_DRedisSA_Init_suite, Init) {
}

/**
 * @brief
**/
class test_DRedisSA_CreateTable_suite : public ::testing::Test {
protected:
    test_DRedisSA_CreateTable_suite() {};
    virtual ~test_DRedisSA_CreateTable_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_DRedisSA_CreateTable_suite, *)
        ASSERT_EQ(rsa_.Init(g_SentinelIpport, g_RedisMasterNameForSentinel,
                            3, 20000, g_RedisMasterPasswd), storage::OK);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_DRedisSA_CreateTable_suite, *)
    };
public:
    storage::DRedisSA rsa_;
};

TEST_F(test_DRedisSA_CreateTable_suite, CreateListTable) {
    storage::IStoreTablePtr plist;
    //boost::shared_ptr<storage::RedisListTable> plist;
    ASSERT_EQ(rsa_.CreateTable(g_WindowsListTableName,
                               storage::REDIS_LIST_TABLE, true, &plist),
              storage::OK);
    ASSERT_TRUE(NULL != plist.get());
}

TEST_F(test_DRedisSA_CreateTable_suite, CreateHashTable) {
    storage::IStoreTablePtr phash;
    //boost::shared_ptr<storage::RedisHashTable> phash;
    ASSERT_EQ(rsa_.CreateTable(g_SliceHashTableName,
                               storage::REDIS_HASH_TABLE, true, &phash),
              storage::OK);
    ASSERT_TRUE(NULL != phash.get());
    //test get and get
}
