/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file test_multi_key_joiner.cc
 * @author lanbijia
 * @date 2012/9/26
 * @brief test class MultiKeyJoiner
 */

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mock class
#include "mock_join_table.h"
#include "mock_timed_sliding_window.h"
#include "mock_config.h"
// test class
#include "generic_operator/multi_key_joiner.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::SetArgumentPointee;
using ::testing::HasSubstr;
using ::testing::Not;

using namespace dstream;
using namespace dstream::generic_operator;

template <class T>
std::string NumToString(const T& n) {
    std::ostringstream out;
    out << n;
    return out.str();
}


class TestMultiJoiner : public ::testing::Test
{
public:
    typedef MockMultiJoinTable<KeyType, DataType> MockKVTable;
    typedef boost::shared_ptr<MockKVTable> MockKVTablePtr;

protected:
    void SetUp();
    void TearDown();
    void InitJoiner(const std::string& config_file);
    void SingleRecord(uint32_t record_num,
                      uint32_t base_time);
    void MultiRecord(uint32_t record_num,
                     uint32_t base_time,
                     uint32_t dup_num);

private:
    MultiKeyJoiner* joiner_;
    // 保存时间窗口指针
    TimedSlidingWindows<MultiKeyJoiner::KVTable>* old_time_window_ptr;
};

void TestMultiJoiner::InitJoiner(const std::string& config_file) {
    dstream::config::Config conf;

    ASSERT_EQ(dstream::error::OK, conf.ReadConfig(config_file));
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Initialize(conf, "test_joiner_id",
              "test_joiner_name", NULL));
}

void TestMultiJoiner::SingleRecord(uint32_t record_num,
                                   uint32_t base_time) {

    for (uint32_t i = base_time; i < record_num + base_time; ++i) {
        // construct init data
        KeyType k = NumToString<uint32_t>(i + 1);
        DataType v = NumToString<uint32_t>(i + 1);

        ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(i + 1, true, k, v));
    }
}

void TestMultiJoiner::MultiRecord(uint32_t record_num,
                                  uint32_t base_time,
                                  uint32_t dup_num) {

    for (uint32_t i = base_time; i < record_num + base_time; ++i) {
        // construct init data
        KeyType k = NumToString<uint32_t>(i + 1);
        DataType v = NumToString<int>(i + 1);

        for (uint32_t d = 0; d < dup_num; ++d) {
            ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(i + 1, false, k, v));
        }
    }
}

void TestMultiJoiner::SetUp() {
    joiner_ = new MultiKeyJoiner();

    old_time_window_ptr = joiner_->m_t_table;
}

void TestMultiJoiner::TearDown() {
    if (joiner_) {
        joiner_->m_t_table = old_time_window_ptr;
    }
    delete joiner_;
}

// test init ok
TEST_F(TestMultiJoiner, NormalInitTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    // Set mock object
    MockTimedSlidingWindow<MultiKeyJoiner::KVTable> mock_window;
    EXPECT_CALL(mock_window, Init(_, _))
    .WillOnce(Return(0));
    EXPECT_CALL(mock_window, SetStore(_, _, _, _, _))
    .WillOnce(Return(0));
    EXPECT_CALL(mock_window, SetWindowCreatingCallback(_, _));
    EXPECT_CALL(mock_window, SetWindowSlidingOutCallback(_, _, _));

    int32_t ret_int_conf = 10;
    std::string ret_string_conf = "true";
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_int_conf), Return(dstream::error::OK)));

    storage::StoreAccessor* sa = reinterpret_cast<dstream::storage::StoreAccessor*>(0xDEADBEEF);

    // Test
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Init(mock_config, "test_joiner_id",
              "test_joiner_name", &mock_window, sa));
    ASSERT_STREQ(joiner_->m_id.c_str(), "test_joiner_id");
    ASSERT_TRUE(joiner_->m_inited);

    // test reinit
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Initialize(mock_config, "test_joiner_id",
              "test_joiner_name", sa));
}

// test init read all conf fail
TEST_F(TestMultiJoiner, InitReadConfFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    // Set mock object
    MockConfig mock_config;
    // read join id fail
    EXPECT_CALL(mock_config, GetValue(_, _))
    .WillRepeatedly(Return(dstream::error::FAILED_EXPECTATION));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(Return(dstream::error::FAILED_EXPECTATION));

    storage::StoreAccessor* sa = reinterpret_cast<storage::StoreAccessor*>(0xDEADBEEF);

    // Test
    ASSERT_EQ(dstream::generic_operator::error::JOINER_ID_NOT_EXIST, joiner_->Init(mock_config,
              "test_joiner_id", "test_joiner_name", NULL, sa));

    ASSERT_EQ(joiner_->m_step_in_sec, dstream::generic_operator::MultiKeyJoiner::kDefaultWindowStep);
    ASSERT_EQ(joiner_->m_window_size_by_step,
              dstream::generic_operator::MultiKeyJoiner::kDefaultWindowSize);
    ASSERT_EQ(joiner_->m_restore_from_store,
              dstream::generic_operator::MultiKeyJoiner::kDefaultJoinWithStore);
    ASSERT_STRNE(joiner_->m_id.c_str(), "test_joiner_id");
    ASSERT_TRUE(!joiner_->m_inited);
}

// test init read string conf fail
TEST_F(TestMultiJoiner, ReadConfigFileTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));
    std::string joiner_id = "test_joiner_id";
    std::string ret_string_conf = "true";

    // read kJoinWithStore conf fail
    DSTREAM_DEBUG("read kJoinWithStore conf fail");
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(HasSubstr(MultiKeyJoiner::kJoinWithStore), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kJoinWithStore
    EXPECT_CALL(mock_config, GetValue(Not(HasSubstr(MultiKeyJoiner::kJoinWithStore)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_EQ(joiner_->m_join_with_store,
              dstream::generic_operator::MultiKeyJoiner::kDefaultJoinWithStore);

    // read kWindowStep conf fail
    DSTREAM_DEBUG("read kWindowStep conf fail");
    MockConfig mock_config_1;
    EXPECT_CALL(mock_config_1, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_1, GetIntValue(HasSubstr(MultiKeyJoiner::kWindowStep), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kWindowStep
    EXPECT_CALL(mock_config_1, GetIntValue(Not(HasSubstr(MultiKeyJoiner::kWindowStep)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK))); // read kWindowSize
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_1, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, dstream::generic_operator::MultiKeyJoiner::kDefaultWindowStep);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kWindowSize conf fail
    DSTREAM_DEBUG("read kWindowSize conf fail");
    MockConfig mock_config_2;
    EXPECT_CALL(mock_config_2, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_2, GetIntValue(HasSubstr(MultiKeyJoiner::kWindowSize), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kWindowSize
    EXPECT_CALL(mock_config_2, GetIntValue(Not(HasSubstr(MultiKeyJoiner::kWindowSize)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK))); // read others
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_2, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step,
              dstream::generic_operator::MultiKeyJoiner::kDefaultWindowSize);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kRestoreFromStore conf fail
    DSTREAM_DEBUG("read kRestoreFromStore conf fail");
    MockConfig mock_config_3;
    EXPECT_CALL(mock_config_3, GetValue(HasSubstr(MultiKeyJoiner::kRestoreFromStore), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kRestoreFromStore
    EXPECT_CALL(mock_config_3, GetValue(Not(HasSubstr(MultiKeyJoiner::kRestoreFromStore)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_3, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_3, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_EQ(joiner_->m_restore_from_store,
              dstream::generic_operator::MultiKeyJoiner::kDefaultRestoreFromStore);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kIgnoreStoreError conf fail
    DSTREAM_DEBUG("read kIgnoreStoreError conf fail");
    MockConfig mock_config_4;
    EXPECT_CALL(mock_config_4, GetValue(HasSubstr(MultiKeyJoiner::kIgnoreStoreError), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kIgnoreStoreError
    EXPECT_CALL(mock_config_4, GetValue(Not(HasSubstr(MultiKeyJoiner::kIgnoreStoreError)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_4, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_4, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_EQ(joiner_->m_ignore_store_error,
              dstream::generic_operator::MultiKeyJoiner::kDefaultIgnoreStoreError);

    // read kStoreBatchInsert conf fail
    DSTREAM_DEBUG("read kStoreBatchInsert conf fail");
    MockConfig mock_config_5;
    EXPECT_CALL(mock_config_5, GetValue(HasSubstr(MultiKeyJoiner::kStoreBatchInsert), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchInsert
    EXPECT_CALL(mock_config_5, GetValue(Not(HasSubstr(MultiKeyJoiner::kStoreBatchInsert)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_5, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_5, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);
    ASSERT_EQ(joiner_->m_batch_insert,
              dstream::generic_operator::MultiKeyJoiner::kDefaultStoreBatchInsert);

    // read kStoreBatchCount conf fail
    DSTREAM_DEBUG("read kStoreBatchCount conf fail");
    MockConfig mock_config_6;
    EXPECT_CALL(mock_config_6, GetIntValue(HasSubstr(MultiKeyJoiner::kStoreBatchCount), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchCount
    EXPECT_CALL(mock_config_6, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_6, GetIntValue(Not(HasSubstr(MultiKeyJoiner::kStoreBatchCount)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_6, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);
    ASSERT_TRUE(joiner_->m_batch_insert);
    ASSERT_EQ(joiner_->m_batch_count,
              dstream::generic_operator::MultiKeyJoiner::kDefaultStoreBatchCount);

    // read kStoreBatchIntervalMs conf fail
    DSTREAM_DEBUG("read kStoreBatchIntervalMs conf fail");
    MockConfig mock_config_7;
    EXPECT_CALL(mock_config_7, GetIntValue(HasSubstr(MultiKeyJoiner::kStoreBatchIntervalMs), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchIntervalMs
    EXPECT_CALL(mock_config_7, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_7, GetIntValue(Not(HasSubstr(MultiKeyJoiner::kStoreBatchIntervalMs)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_7, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);
    ASSERT_TRUE(joiner_->m_batch_insert);
    ASSERT_EQ(joiner_->m_batch_count, 829);
    ASSERT_EQ(joiner_->m_batch_interval_in_ms,
              dstream::generic_operator::MultiKeyJoiner::kDefaultStoreBatchIntervalMs);

}

// test init timed sliding window init fail
TEST_F(TestMultiJoiner, InitTimedSlidingWindowInitFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    // Set mock object
    int32_t ret_int_conf = 10;
    std::string ret_string_conf = "false";
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_int_conf), Return(dstream::error::OK)));

    MockTimedSlidingWindow<MultiKeyJoiner::KVTable> mock_window;
    EXPECT_CALL(mock_window, Init(_, _))
    .WillOnce(Return(-1)); // mock init fail
    EXPECT_CALL(mock_window, SetWindowCreatingCallback(_, _));
    EXPECT_CALL(mock_window, SetWindowSlidingOutCallback(_, _, _));

    storage::StoreAccessor* sa = reinterpret_cast<storage::StoreAccessor*>(0xDEADBEEF);

    // Test
    ASSERT_EQ(dstream::generic_operator::error::JOINER_WINDOW_INIT_ERROR, joiner_->Init(mock_config,
              "test_joiner_id", "test_joiner_name", &mock_window, sa));

    ASSERT_EQ(joiner_->m_step_in_sec, ret_int_conf);
    ASSERT_EQ(joiner_->m_window_size_by_step, ret_int_conf);
    ASSERT_FALSE(joiner_->m_restore_from_store);
    ASSERT_STRNE(joiner_->m_id.c_str(), "test_joiner_id");
    ASSERT_TRUE(!joiner_->m_inited);
}

// test destroy
TEST_F(TestMultiJoiner, NormalDestroyTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    // Set mock object
    MockTimedSlidingWindow<MultiKeyJoiner::KVTable> mock_window;
    // delete at joiner_->Destroy
    EXPECT_CALL(mock_window, Init(_, _))
    .WillOnce(Return(0));
    EXPECT_CALL(mock_window, SetStore(_, _, _, _, _))
    .WillOnce(Return(0));
    EXPECT_CALL(mock_window, SetWindowCreatingCallback(_, _));
    EXPECT_CALL(mock_window, SetWindowSlidingOutCallback(_, _, _));

    int32_t ret_int_conf = 10;
    std::string ret_string_conf = "true";
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_int_conf), Return(dstream::error::OK)));

    storage::StoreAccessor* sa = reinterpret_cast<storage::StoreAccessor*>(0xDEADBEEF);
    // init joiner first
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Init(mock_config, "test_joiner_id",
              "test_joiner_name", &mock_window, sa));

    // Test
    joiner_->Destroy();
    ASSERT_TRUE(!joiner_->m_inited);
}

// test joiner normal record
TEST_F(TestMultiJoiner, NormalRecordTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    KeyType keys;
    DataType value;

    MockKVTable* mock_join_table_ptr = new MockKVTable;
    EXPECT_CALL(*mock_join_table_ptr, Insert(_, _))
    .WillOnce(Return(0));

    MockKVTablePtr mock_window_ptr(mock_join_table_ptr);
    MockTimedSlidingWindow<MultiKeyJoiner::KVTable>  mock_window;
    EXPECT_CALL(mock_window, GetWindow(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(mock_window_ptr), Return(0)));
    joiner_->m_t_table = &mock_window;

    // Test uninit
    ASSERT_EQ(dstream::generic_operator::error::JOINER_UNINIT, joiner_->Record(tuple_time, true, keys,
              value));

    // Test normal
    joiner_->m_inited = true;

    // Test record
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, false, keys, value));

}

// test joiner normal record replace & duplicate
TEST_F(TestMultiJoiner, ReplaceAndDuplicateRecordTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    uint32_t tuple_time = 123;
    KeyType keys = "test_key";
    DataType value = "test_value";
    std::vector<DataType> join_values;

    // test replace
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, true, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, true, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, false, &join_values));
    ASSERT_EQ(1u, join_values.size());
    join_values.clear();

    // test duplicate
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, false, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, false, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, false, &join_values));
    ASSERT_EQ(2u, join_values.size());
    join_values.clear();

}


// test joiner record get window fail
TEST_F(TestMultiJoiner, JoinerRecordGetWindowFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    std::string keys = "test_key";
    std::string value = "test_value";

    joiner_->m_inited = true;

    MockTimedSlidingWindow<MultiKeyJoiner::KVTable> mock_window;
    MockKVTablePtr mock_window_ptr;
    mock_window_ptr.reset();
    EXPECT_CALL(mock_window, GetWindow(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(mock_window_ptr), Return(0)));
    joiner_->m_t_table = &mock_window;

    ASSERT_EQ(dstream::generic_operator::error::JOINER_GET_WINDOW_FAIL, joiner_->Record(tuple_time,
              true, keys, value));
}

// test joiner record insert data fail
TEST_F(TestMultiJoiner, JoinerRecordInsertDataFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    KeyType keys;
    DataType value;

    joiner_->m_inited = true;

    // mock insert fail
    MockKVTable* mock_join_table_ptr = new MockKVTable;
    EXPECT_CALL(*mock_join_table_ptr, Insert(_, _))
    .WillOnce(Return(-1));

    MockKVTablePtr mock_window_ptr(mock_join_table_ptr);

    MockTimedSlidingWindow<MultiKeyJoiner::KVTable> mock_window;
    EXPECT_CALL(mock_window, GetWindow(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(mock_window_ptr), Return(0)));
    joiner_->m_t_table = &mock_window;

    ASSERT_EQ(dstream::generic_operator::error::JOINER_RECORD_DATA_FAIL, joiner_->Record(tuple_time,
              false, keys, value));
}

// test joiner join ok with single record value
TEST_F(TestMultiJoiner, JoinerNormalSingleValueJoinTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    uint32_t record_num = 100;
    uint32_t base_time = 100;
    SingleRecord(record_num, base_time);

    KeyType keys;
    std::vector<DataType> join_values;
    std::vector<DataType>::iterator value_iter;

    // mock join key
    for (uint32_t i = base_time + 10; i < record_num + base_time; ++i) {
        keys = NumToString<uint32_t>(i + 1);
        ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, true, &join_values));

        ASSERT_EQ(1u, join_values.size());
        value_iter = join_values.begin();
        ASSERT_STREQ(value_iter->c_str(), NumToString<uint32_t>(i + 1).c_str());
    }
}

// test joiner join ok with multi record value
TEST_F(TestMultiJoiner, JoinerNormalMultiValueJoinTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    uint32_t record_num = 100;
    uint32_t base_time = 100;
    uint32_t dup_num = 5;
    MultiRecord(record_num, base_time, dup_num);

    KeyType keys;
    std::vector<DataType> join_values;
    std::vector<DataType>::iterator value_iter;

    // mock join key
    for (uint32_t i = base_time + 10; i < record_num + base_time; ++i) {
        keys = NumToString<uint32_t>(i + 1);

        ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, true, &join_values));
        ASSERT_EQ(dup_num, join_values.size());
        for (value_iter = join_values.begin(); value_iter != join_values.end(); ++value_iter) {
            ASSERT_STREQ(value_iter->c_str(), NumToString<uint32_t>(i + 1).c_str());
        }
    }
}

// test joiner join fail
TEST_F(TestMultiJoiner, JoinerJoinKeyNotFoundTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    KeyType keys;
    std::vector<DataType> join_values;

    InitJoiner("test_joiner.cfg.xml");

    uint32_t record_num = 1;
    uint32_t base_time = 100;
    uint32_t dup_num = 3;
    MultiRecord(record_num, base_time, dup_num);

    // test uninit
    joiner_->m_inited = false;
    ASSERT_EQ(dstream::generic_operator::error::JOINER_UNINIT,
              joiner_->Join(keys, true, &join_values));
    joiner_->m_inited = true;

    // mock join key ok
    keys = NumToString<uint32_t>(base_time + 1);
    ASSERT_EQ(dstream::generic_operator::error::OK,
              joiner_->Join(keys, true, &join_values));
    ASSERT_EQ(3u, join_values.size());

    // mock join key fail
    keys = NumToString<uint32_t>(record_num + base_time + 1);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_values));

    keys = NumToString<uint32_t>(base_time);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_values));

    keys = NumToString<uint32_t>(0);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_values));

}

static uint32_t data_count = 0;
void user_call_back(const KeyType& key, const DataType& data, void* context) {
    printf("key(%s) data(%s) context(%p).\n", key.c_str(), data.c_str(), context);
    data_count++;
}

// test joiner join with user callback
TEST_F(TestMultiJoiner, JoinerJoinWithUserCallbackTest) {
    ASSERT_NE(joiner_, reinterpret_cast<MultiKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    joiner_->RegisterDataTimeOutCallBack(user_call_back, (void*)0x12345);

    uint32_t record_num = 100;
    uint32_t base_time = 100;
    uint32_t dup_num = 3;
    MultiRecord(record_num * 2, base_time, dup_num);

    KeyType keys;
    std::vector<DataType> join_values;
    std::vector<DataType>::iterator value_iter;

    // mock join key
    uint32_t expect_count = 0;
    for (uint32_t i = base_time; i < record_num + base_time + joiner_->m_step_in_sec - 1; ++i) {
        keys = NumToString<uint32_t>(i + 1);
        printf("join key(%s).\n", keys.c_str());
        ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
                  joiner_->Join(keys, true, &join_values));
        expect_count += dup_num;
    }
    ASSERT_EQ(expect_count, data_count);
}


int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_joiner");
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}
