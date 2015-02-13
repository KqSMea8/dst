#include "generic_operator/multi_join_table.h"
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

class MultiJoinTableTest : public ::testing::Test {
protected:
    void CheckResult(std::vector<std::string>& result, std::vector<std::string>& expected_result);

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

void MultiJoinTableTest::CheckResult(std::vector<std::string>& result,
                                     std::vector<std::string>& expected_result) {
    ASSERT_EQ(expected_result.size(), result.size());
    std::sort(result.begin(), result.end());
    std::sort(expected_result.begin(), expected_result.end());
    for (size_t i = 0; i < result.size(); ++i) {
        ASSERT_EQ(expected_result[i], result[i]);
    }
}

void MultiJoinTableTest::CheckKvVector(std::vector<std::pair<int, std::string> >& kv_vec,
                                       std::vector<std::pair<int, std::string> >& expected_kv_vec) {
    ASSERT_EQ(expected_kv_vec.size(), kv_vec.size());
    std::sort(kv_vec.begin(), kv_vec.end(), KvPairCmp());
    std::sort(expected_kv_vec.begin(), expected_kv_vec.end(), KvPairCmp());
    for (size_t i = 0; i < expected_kv_vec.size(); ++i) {
        ASSERT_EQ(expected_kv_vec[i].first, kv_vec[i].first);
        ASSERT_EQ(expected_kv_vec[i].second, kv_vec[i].second);
    }
}

TEST_F(MultiJoinTableTest, SimpleTest) {
    MultiJoinTable<int, std::string> join_table(500);

    join_table.Insert(1, "a");
    join_table.Insert(3, "b");
    join_table.Insert(5, "c");
    join_table.Insert(3, "a");
    join_table.Insert(3, "c");
    join_table.Insert(5, "a");

    // expected kv vector
    std::vector<std::pair<int, std::string> > expected_kv_vec;
    expected_kv_vec.push_back(std::make_pair(1, "a"));
    expected_kv_vec.push_back(std::make_pair(3, "b"));
    expected_kv_vec.push_back(std::make_pair(5, "c"));
    expected_kv_vec.push_back(std::make_pair(3, "a"));
    expected_kv_vec.push_back(std::make_pair(3, "c"));
    expected_kv_vec.push_back(std::make_pair(5, "a"));

    std::vector<std::string> result;
    std::vector<std::string> expected_result;

    int result_num;
    result.clear();
    result_num = join_table.Find(2, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());

    result.clear();
    result_num = join_table.Find(1, &result);
    ASSERT_EQ(1, result_num);
    ASSERT_EQ(1u, result.size());
    expected_result.clear();
    expected_result.push_back("a");
    CheckResult(result, expected_result);

    result.clear();
    result_num = join_table.Find(3, &result);
    ASSERT_EQ(3, result_num);
    ASSERT_EQ(3u, result.size());
    expected_result.clear();
    expected_result.push_back("a");
    expected_result.push_back("b");
    expected_result.push_back("c");
    CheckResult(result, expected_result);

    result.clear();
    result_num = join_table.Find(5, &result);
    ASSERT_EQ(2, result_num);
    ASSERT_EQ(2u, result.size());
    expected_result.clear();
    expected_result.push_back("a");
    expected_result.push_back("c");
    CheckResult(result, expected_result);

    // test for iterator
    std::vector<std::pair<int, std::string> > kv_vec;
    int key;
    std::string value;
    for (MultiJoinTableIterator<int, std::string> join_table_itr(join_table);
         join_table_itr.GetCurrent(&key, &value);
         join_table_itr.Next()) {
        kv_vec.push_back(std::make_pair(key, value));
    }
    CheckKvVector(expected_kv_vec, kv_vec);

    // test for remove
    result.clear();
    join_table.Remove(3);
    result_num = join_table.Find(3, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());

    result.clear();
    join_table.Remove(5);
    result_num = join_table.Find(5, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());
}

TEST_F(MultiJoinTableTest, StoreTest) {
    MultiJoinTable<int, std::string> tmp_join_table(500);

    boost::shared_ptr<storage::MockStorageListTable> mock_storage_list_table(
        new storage::MockStorageListTable());
    int ret = tmp_join_table.SetStore(mock_storage_list_table, true, true, true, 1, 100);
    ASSERT_EQ(0, ret);

    // should not restore any data from empty store table
    int key;
    std::string value;
    for (MultiJoinTableIterator<int, std::string> itr(tmp_join_table);
         itr.GetCurrent(&key, &value);
         itr.Next()) {
        ASSERT_TRUE(false);
    }

    ASSERT_EQ(0, tmp_join_table.Insert(1, "a"));
    ASSERT_EQ(0, tmp_join_table.Insert(3, "b"));
    ASSERT_EQ(0, tmp_join_table.Insert(5, "c"));
    ASSERT_EQ(0, tmp_join_table.Insert(3, "a"));
    ASSERT_EQ(0, tmp_join_table.Insert(3, "c"));
    ASSERT_EQ(0, tmp_join_table.Remove(3));
    // sleep to make sure batch interval is reached
    sleep(1);
    ASSERT_EQ(0, tmp_join_table.Insert(5, "a"));

    // expected kv vector
    std::vector<std::pair<int, std::string> > expected_kv_vec;
    expected_kv_vec.push_back(std::make_pair(1, "a"));
    expected_kv_vec.push_back(std::make_pair(5, "c"));
    expected_kv_vec.push_back(std::make_pair(5, "a"));

    // restore records from mock storage table and check resotred join table
    MultiJoinTable<int, std::string> join_table;
    ret = join_table.SetStore(mock_storage_list_table, true, true, true, 100, 100);
    ASSERT_EQ(0, ret);

    std::vector<std::string> result;
    std::vector<std::string> expected_result;

    int result_num;
    result.clear();
    result_num = join_table.Find(2, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());

    result.clear();
    result_num = join_table.Find(1, &result);
    ASSERT_EQ(1, result_num);
    ASSERT_EQ(1u, result.size());
    expected_result.clear();
    expected_result.push_back("a");
    CheckResult(result, expected_result);

    result.clear();
    result_num = join_table.Find(3, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());

    result.clear();
    result_num = join_table.Find(5, &result);
    ASSERT_EQ(2, result_num);
    ASSERT_EQ(2u, result.size());
    expected_result.clear();
    expected_result.push_back("a");
    expected_result.push_back("c");
    CheckResult(result, expected_result);

    // test for iterator
    std::vector<std::pair<int, std::string> > kv_vec;
    for (MultiJoinTableIterator<int, std::string> join_table_itr(join_table);
         join_table_itr.GetCurrent(&key, &value);
         join_table_itr.Next()) {
        kv_vec.push_back(std::make_pair(key, value));
    }
    CheckKvVector(expected_kv_vec, kv_vec);

    // test for remove
    result.clear();
    join_table.Remove(3);
    result_num = join_table.Find(3, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());

    result.clear();
    join_table.Remove(5);
    result_num = join_table.Find(5, &result);
    ASSERT_EQ(0, result_num);
    ASSERT_EQ(0u, result.size());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TestEnv());
    return RUN_ALL_TESTS();
}
