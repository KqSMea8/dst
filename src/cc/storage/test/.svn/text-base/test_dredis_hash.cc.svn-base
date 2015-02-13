#include <string>
#include <boost/shared_ptr.hpp>
#include "storage/dredis_sa.h"
#include "storage/dredis_hash.h"
#include "redis_env.h"
#include "common/logger.h"
#include <gtest/gtest.h>

using namespace dredis;
using namespace dstream;

const std::string g_HashTableName = "__UnitTestHashTable";

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
    EnvTearDown();
    return ret;
}

/**
 * @brief
**/
class test_RedisHashTable_CreateTable_suite : public ::testing::Test {
protected:
    test_RedisHashTable_CreateTable_suite() {};
    virtual ~test_RedisHashTable_CreateTable_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RedisHashTable_CreateTable_suite, *)
        ASSERT_EQ(rsa_.Init(g_SentinelIpport, g_RedisMasterNameForSentinel,
                            3, 20000, g_RedisMasterPasswd), storage::OK);
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RedisHashTable_CreateTable_suite, *)
    };

    void CreateHashTable(const std::string& table_name, bool replace_old,
                         storage::RedisHashTablePtr* phash);
public:
    storage::DRedisSA rsa_;
};

void test_RedisHashTable_CreateTable_suite::CreateHashTable (
    const std::string& table_name, bool replace_old,
    storage::RedisHashTablePtr* phash) {
    ASSERT_TRUE(NULL != phash);
    storage::IStoreTablePtr pt;
    ASSERT_EQ(rsa_.CreateTable(table_name,
                               storage::REDIS_HASH_TABLE,
                               replace_old, &pt),
              storage::OK);
    *phash = boost::dynamic_pointer_cast<storage::RedisHashTable>(pt);
}

TEST_F(test_RedisHashTable_CreateTable_suite, CreateHashTable) {
    storage::RedisHashTablePtr phash;
    //boost::shared_ptr<storage::RedisHashTable> phash;
    CreateHashTable(g_HashTableName, true, &phash);
    ASSERT_TRUE(NULL != phash.get());

    uint64_t n;
    ASSERT_EQ(phash->Count(&n), storage::OK);
    printf("count of hash[%s] is [%zu]\n", g_HashTableName.c_str(), n);
}

TEST_F(test_RedisHashTable_CreateTable_suite, InsertAndErase) {
    storage::RedisHashTablePtr phash;
    //boost::shared_ptr<storage::RedisHashTable> phash;
    CreateHashTable(g_HashTableName, true, &phash);
    ASSERT_TRUE(NULL != phash.get());

    const ssize_t num = 1024;
    char a1[num];
    char a2[num * 2];
    srandom(time(NULL));
    for (ssize_t i = 0; i < num; i++) {
        a1[i] = random() % 127;
    }
    for (ssize_t i = 0; i < num * 2; i++) {
        a2[i] = random() % 127;
    }
    //erase
    std::string k = "__UnitTestHashTestKey";
    std::string v;
    ASSERT_EQ(phash->Erase(k), storage::OK);
    //insert
    v.assign((char*)a1, sizeof(a1));
    ASSERT_EQ(phash->Insert(k, v), storage::OK);
    //insert again
    ASSERT_EQ(phash->Insert(k, v), storage::KEY_ALREADY_EXISTS);
    //replace old value
    v.assign((char*)a2, sizeof(a2));
    ASSERT_GE(phash->Insert(k, v, true), storage::OK);
    v.clear();
    //query and check
    ASSERT_EQ(phash->Find(k, &v), storage::OK);
    ASSERT_EQ(sizeof(a2), v.size());
    for (ssize_t i = 0; i < num * 2; i++) {
        ASSERT_EQ(a2[i], v.at(i));
    }
}

TEST_F(test_RedisHashTable_CreateTable_suite, Batch_InsertAndErase) {
    storage::RedisHashTablePtr phash;
    CreateHashTable(g_HashTableName, true, &phash);
    ASSERT_TRUE(NULL != phash.get());

    uint64_t n;
    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_keys;
    std::vector<std::string> org_values;
    std::vector<std::string> ret_keys;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //insert
    org_keys.resize(num);
    org_values.resize(num);
    FillRandomKeys(&org_keys);
    FillRandomKeys(&org_values);
    ASSERT_EQ(phash->Insert(org_keys, org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }
    //check count
    ASSERT_EQ(phash->Count(&n), storage::OK);
    printf("before erase: count of hash[%s] is [%zu]\n", g_HashTableName.c_str(), n);
    //erase
    ASSERT_EQ(phash->Erase(org_keys, &results), storage::OK);
    ASSERT_EQ(results.size(), org_keys.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }
    //check count
    ASSERT_EQ(phash->Count(&n), storage::OK);
    printf("after erase: count of hash[%s] is [%zu]\n", g_HashTableName.c_str(), n);
}

typedef struct {
    void* k;
    void* v;
} _kv_vectors;

int GetAllCb(void* ctx, int err, const std::string& k,
             const std::string& v) {
    if (0 != err) {
        printf("get error reply: [%d][%s][%s]\n", err, k.c_str(), v.c_str());
        return -1;
    }
    _kv_vectors* kv_ctx = (_kv_vectors*)ctx;
    std::vector<std::string>* keys = (std::vector<std::string>*)kv_ctx->k;
    std::vector<std::string>* values = (std::vector<std::string>*)kv_ctx->v;
    keys->push_back(k);
    values->push_back(v);
    return 0;
}

TEST_F(test_RedisHashTable_CreateTable_suite, GetAll) {
    storage::RedisHashTablePtr phash;
    CreateHashTable(g_HashTableName, true, &phash);
    ASSERT_TRUE(NULL != phash.get());

    const size_t num = 10;
    srandom(time(NULL));
    std::vector<std::string> org_keys;
    std::vector<std::string> org_values;
    std::vector<std::string> ret_keys;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //push
    org_keys.resize(num);
    org_values.resize(num);
    FillRandomKeys(&org_keys);
    FillRandomKeys(&org_values);
    ASSERT_EQ(phash->Insert(org_keys, org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }

    //get all
    _kv_vectors ctx;
    ctx.k = static_cast<void*>(&ret_keys);
    ctx.v = static_cast<void*>(&ret_values);
    ASSERT_EQ(phash->GetAll(GetAllCb, &ctx), storage::OK);
    ASSERT_EQ(org_keys.size(), ret_keys.size());
    ASSERT_EQ(org_values.size(), ret_values.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_TRUE(org_keys[i] == ret_keys[i]);
        ASSERT_TRUE(org_values[i] == ret_values[i]);
    }
}

TEST_F(test_RedisHashTable_CreateTable_suite, Clear) {
    storage::RedisHashTablePtr phash;
    CreateHashTable(g_HashTableName, true, &phash);
    ASSERT_TRUE(NULL != phash.get());
    //clear
    ASSERT_EQ(phash->Clear(), storage::OK);

    uint64_t n;
    const size_t num = 1024;
    srandom(time(NULL));
    std::vector<std::string> org_keys;
    std::vector<std::string> org_values;
    std::vector<std::string> ret_keys;
    std::vector<std::string> ret_values;
    std::vector<int> results;
    //insert
    org_keys.resize(num);
    org_values.resize(num);
    FillRandomKeys(&org_keys);
    FillRandomKeys(&org_values);
    ASSERT_EQ(phash->Insert(org_keys, org_values, &results), storage::OK);
    ASSERT_EQ(org_values.size(), results.size());
    for (size_t i = 0; i < num; i++) {
        ASSERT_EQ(results[i], storage::OK);
    }
    //clear
    ASSERT_EQ(phash->Count(&n), storage::OK);
    printf("before clear: count of hash[%s] is [%zu]\n", g_HashTableName.c_str(), n);
    ASSERT_EQ(phash->Clear(), storage::OK);
    ASSERT_EQ(phash->Count(&n), storage::OK);
    //check again
    printf("after clear: count of hash[%s] is [%zu]\n", g_HashTableName.c_str(), n);
}
