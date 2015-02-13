#include "generic_operator/join_table.h"
#include <gtest/gtest.h>
#include "generic_operator/test/mock_storage.h"

using namespace dstream;
using namespace dstream::generic_operator;

class TestEnv : public ::testing::Environment {
public:
    virtual void SetUp() {
        logger::initialize("join_table_test");
    }

    virtual void TearDown() {
    }
};

class JoinTableTest : public ::testing::Test {
protected:
    void CheckKvVector(std::vector<std::pair<int, std::string> >& result,
                       std::vector<std::pair<int, std::string> >& expected_result);
};

class KvPairCmp {
public:
    bool operator()(std::pair<int, std::string> left, std::pair<int, std::string> right) {
        if (left.first == right.first) {
            return left.second < right.second;
        } else {
            return left.first < right.first;
        }
    }
};

void JoinTableTest::CheckKvVector(std::vector<std::pair<int, std::string> >& kv_vec,
                                  std::vector<std::pair<int, std::string> >& expected_kv_vec) {
    ASSERT_EQ(expected_kv_vec.size(), kv_vec.size());
    std::sort(kv_vec.begin(), kv_vec.end(), KvPairCmp());
    std::sort(expected_kv_vec.begin(), expected_kv_vec.end(), KvPairCmp());
    for (size_t i = 0; i < expected_kv_vec.size(); ++i) {
        ASSERT_EQ(expected_kv_vec[i].first, kv_vec[i].first);
        ASSERT_EQ(expected_kv_vec[i].second, kv_vec[i].second);
    }
}

TEST_F(JoinTableTest, SimpleTest) {
    JoinTable<int, std::string> join_table(500);

    ASSERT_EQ(0, join_table.Insert(1, "a"));
    ASSERT_EQ(0, join_table.Insert(3, "b", false));
    ASSERT_EQ(0, join_table.Insert(5, "c"));
    ASSERT_EQ(0, join_table.Insert(3, "c"));
    ASSERT_EQ(-1, join_table.Insert(3, "a", false));
    ASSERT_EQ(-1, join_table.Insert(5, "a", false));

    // expected kv vector
    std::vector<std::pair<int, std::string> > expected_kv_vec;
    expected_kv_vec.push_back(std::make_pair(1, "a"));
    expected_kv_vec.push_back(std::make_pair(3, "c"));
    expected_kv_vec.push_back(std::make_pair(5, "c"));

    std::string result;
    ASSERT_EQ(0, join_table.Find(2, &result));

    ASSERT_EQ(1, join_table.Find(1, &result));
    ASSERT_EQ(result, std::string("a"));

    ASSERT_EQ(1, join_table.Find(3, &result));
    ASSERT_EQ(result, std::string("c"));

    ASSERT_EQ(1, join_table.Find(5, &result));
    ASSERT_EQ(result, std::string("c"));

    // test for iterator
    std::vector<std::pair<int, std::string> > kv_vec;
    int key;
    std::string value;
    for (JoinTableIterator<int, std::string> join_table_itr(join_table);
         join_table_itr.GetCurrent(&key, &value);
         join_table_itr.Next()) {
        kv_vec.push_back(std::make_pair(key, value));
    }
    CheckKvVector(expected_kv_vec, kv_vec);

    // test for remove
    join_table.Remove(3);
    ASSERT_EQ(0, join_table.Find(3, &result));

    join_table.Remove(5);
    ASSERT_EQ(0, join_table.Find(5, &result));
}

TEST_F(JoinTableTest, StoreTest) {
    JoinTable<int, std::string> tmp_join_table(500);

    boost::shared_ptr<storage::MockStorageHashTable> mock_storage_hash_table(
        new storage::MockStorageHashTable());
    int ret = tmp_join_table.SetStore(mock_storage_hash_table, true, true, true, 1, 100);
    ASSERT_EQ(0, ret);

    // should not restore any data from empty store table
    int key;
    std::string value;
    for (JoinTableIterator<int, std::string> itr(tmp_join_table);
         itr.GetCurrent(&key, &value);
         itr.Next()) {
        ASSERT_TRUE(false);
    }

    ASSERT_EQ(0, tmp_join_table.Insert(1, "a"));
    ASSERT_EQ(0, tmp_join_table.Insert(3, "b", false));
    ASSERT_EQ(0, tmp_join_table.Insert(5, "c"));
    ASSERT_EQ(0, tmp_join_table.Remove(5));
    // sleep to make sure batch interval is reached
    sleep(1);
    ASSERT_EQ(0, tmp_join_table.Insert(3, "c"));
    ASSERT_EQ(-1, tmp_join_table.Insert(3, "a", false));
    //  ASSERT_EQ(0, tmp_join_table.Insert(5, "a", false));

    // expected kv vector
    std::vector<std::pair<int, std::string> > expected_kv_vec;
    expected_kv_vec.push_back(std::make_pair(1, "a"));
    expected_kv_vec.push_back(std::make_pair(3, "c"));

    // restore records from mock storage table and check resotred join table
    JoinTable<int, std::string> join_table;
    ret = join_table.SetStore(mock_storage_hash_table, true, true, true, 100, 100);
    ASSERT_EQ(0, ret);

    std::string result;
    ASSERT_EQ(0, join_table.Find(2, &result));

    ASSERT_EQ(1, join_table.Find(1, &result));
    ASSERT_EQ(result, std::string("a"));

    ASSERT_EQ(1, join_table.Find(3, &result));
    ASSERT_EQ(result, std::string("c"));

    ASSERT_EQ(0, join_table.Find(5, &result));

    // test for iterator
    std::vector<std::pair<int, std::string> > kv_vec;
    for (JoinTableIterator<int, std::string> join_table_itr(join_table);
         join_table_itr.GetCurrent(&key, &value);
         join_table_itr.Next()) {
        kv_vec.push_back(std::make_pair(key, value));
    }
    CheckKvVector(expected_kv_vec, kv_vec);

    // test for remove
    join_table.Remove(3);
    ASSERT_EQ(0, join_table.Find(3, &result));

    join_table.Remove(5);
    ASSERT_EQ(0, join_table.Find(5, &result));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TestEnv());
    return RUN_ALL_TESTS();
}
