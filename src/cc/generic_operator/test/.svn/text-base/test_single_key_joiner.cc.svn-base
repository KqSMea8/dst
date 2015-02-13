/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file test_single_key_joiner.cc
 * @author lanbijia
 * @date 2012/9/26
 * @brief test class SingleKeyJoiner
 */

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mock class
#include "mock_join_table.h"
#include "mock_timed_sliding_window.h"
#include "mock_config.h"
// test class
#include "generic_operator/single_key_joiner.h"

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


class TestSingleJoiner : public ::testing::Test
{
public:
    typedef MockSingleJoinTable<KeyType, DataType> MockKVTable;
    typedef boost::shared_ptr<MockKVTable> MockKVTablePtr;

protected:
    void SetUp();
    void TearDown();
    void InitJoiner(const std::string& config_file);
    void SingleRecord(uint32_t record_num,
                      uint32_t base_time);

private:
    SingleKeyJoiner* joiner_;
    // 保存时间窗口指针
    TimedSlidingWindows<SingleKeyJoiner::KVTable>* old_time_window_ptr;
};

void TestSingleJoiner::InitJoiner(const std::string& config_file) {
    dstream::config::Config conf;

    ASSERT_EQ(dstream::error::OK, conf.ReadConfig(config_file));
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Initialize(conf, "test_joiner_id",
              "test_joiner_name", NULL));
}

void TestSingleJoiner::SingleRecord(uint32_t record_num,
                                    uint32_t base_time) {

    for (uint32_t i = base_time; i < record_num + base_time; ++i) {
        // construct init data
        KeyType k = NumToString<uint32_t>(i + 1);
        DataType v = NumToString<uint32_t>(i + 1);

        ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(i + 1, true, k, v));
    }
}

void TestSingleJoiner::SetUp() {
    joiner_ = new SingleKeyJoiner();

    old_time_window_ptr = joiner_->m_t_table;
}

void TestSingleJoiner::TearDown() {
    if (joiner_) {
        joiner_->m_t_table = old_time_window_ptr;
    }
    delete joiner_;
}

// test init ok
TEST_F(TestSingleJoiner, NormalInitTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    // Set mock object
    MockTimedSlidingWindow<SingleKeyJoiner::KVTable> mock_window;
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
TEST_F(TestSingleJoiner, InitReadConfFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

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

    ASSERT_EQ(joiner_->m_step_in_sec, dstream::generic_operator::SingleKeyJoiner::kDefaultWindowStep);
    ASSERT_EQ(joiner_->m_window_size_by_step,
              dstream::generic_operator::SingleKeyJoiner::kDefaultWindowSize);
    ASSERT_EQ(joiner_->m_restore_from_store,
              dstream::generic_operator::SingleKeyJoiner::kDefaultJoinWithStore);
    ASSERT_STRNE(joiner_->m_id.c_str(), "test_joiner_id");
    ASSERT_TRUE(!joiner_->m_inited);
}

// test init read string conf fail
TEST_F(TestSingleJoiner, ReadConfigFileTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));
    std::string joiner_id = "test_joiner_id";
    std::string ret_string_conf = "true";

    // read kJoinWithStore conf fail
    DSTREAM_DEBUG("read kJoinWithStore conf fail");
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(HasSubstr(SingleKeyJoiner::kJoinWithStore), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kJoinWithStore
    EXPECT_CALL(mock_config, GetValue(Not(HasSubstr(SingleKeyJoiner::kJoinWithStore)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_EQ(joiner_->m_join_with_store,
              dstream::generic_operator::SingleKeyJoiner::kDefaultJoinWithStore);

    // read kWindowStep conf fail
    DSTREAM_DEBUG("read kWindowStep conf fail");
    MockConfig mock_config_1;
    EXPECT_CALL(mock_config_1, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_1, GetIntValue(HasSubstr(SingleKeyJoiner::kWindowStep), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kWindowStep
    EXPECT_CALL(mock_config_1, GetIntValue(Not(HasSubstr(SingleKeyJoiner::kWindowStep)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK))); // read kWindowSize
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_1, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, dstream::generic_operator::SingleKeyJoiner::kDefaultWindowStep);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kWindowSize conf fail
    DSTREAM_DEBUG("read kWindowSize conf fail");
    MockConfig mock_config_2;
    EXPECT_CALL(mock_config_2, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_2, GetIntValue(HasSubstr(SingleKeyJoiner::kWindowSize), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kWindowSize
    EXPECT_CALL(mock_config_2, GetIntValue(Not(HasSubstr(SingleKeyJoiner::kWindowSize)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK))); // read others
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_2, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step,
              dstream::generic_operator::SingleKeyJoiner::kDefaultWindowSize);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kRestoreFromStore conf fail
    DSTREAM_DEBUG("read kRestoreFromStore conf fail");
    MockConfig mock_config_3;
    EXPECT_CALL(mock_config_3, GetValue(HasSubstr(SingleKeyJoiner::kRestoreFromStore), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kRestoreFromStore
    EXPECT_CALL(mock_config_3, GetValue(Not(HasSubstr(SingleKeyJoiner::kRestoreFromStore)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_3, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_3, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_EQ(joiner_->m_restore_from_store,
              dstream::generic_operator::SingleKeyJoiner::kDefaultRestoreFromStore);
    ASSERT_TRUE(joiner_->m_ignore_store_error);

    // read kIgnoreStoreError conf fail
    DSTREAM_DEBUG("read kIgnoreStoreError conf fail");
    MockConfig mock_config_4;
    EXPECT_CALL(mock_config_4, GetValue(HasSubstr(SingleKeyJoiner::kIgnoreStoreError), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kIgnoreStoreError
    EXPECT_CALL(mock_config_4, GetValue(Not(HasSubstr(SingleKeyJoiner::kIgnoreStoreError)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_4, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_4, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_EQ(joiner_->m_ignore_store_error,
              dstream::generic_operator::SingleKeyJoiner::kDefaultIgnoreStoreError);

    // read kStoreBatchInsert conf fail
    DSTREAM_DEBUG("read kStoreBatchInsert conf fail");
    MockConfig mock_config_5;
    EXPECT_CALL(mock_config_5, GetValue(HasSubstr(SingleKeyJoiner::kStoreBatchInsert), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchInsert
    EXPECT_CALL(mock_config_5, GetValue(Not(HasSubstr(SingleKeyJoiner::kStoreBatchInsert)), _))
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
              dstream::generic_operator::SingleKeyJoiner::kDefaultStoreBatchInsert);

    // read kStoreBatchCount conf fail
    DSTREAM_DEBUG("read kStoreBatchCount conf fail");
    MockConfig mock_config_6;
    EXPECT_CALL(mock_config_6, GetIntValue(HasSubstr(SingleKeyJoiner::kStoreBatchCount), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchCount
    EXPECT_CALL(mock_config_6, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_6, GetIntValue(Not(HasSubstr(SingleKeyJoiner::kStoreBatchCount)), _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(829), Return(dstream::error::OK)));
    ASSERT_TRUE(joiner_->CheckAndReadConfig(mock_config_6, joiner_id));
    ASSERT_EQ(joiner_->m_step_in_sec, 829);
    ASSERT_EQ(joiner_->m_window_size_by_step, 829);
    ASSERT_TRUE(joiner_->m_join_with_store);
    ASSERT_TRUE(joiner_->m_restore_from_store);
    ASSERT_TRUE(joiner_->m_ignore_store_error);
    ASSERT_TRUE(joiner_->m_batch_insert);
    ASSERT_EQ(joiner_->m_batch_count,
              dstream::generic_operator::SingleKeyJoiner::kDefaultStoreBatchCount);

    // read kStoreBatchIntervalMs conf fail
    DSTREAM_DEBUG("read kStoreBatchIntervalMs conf fail");
    MockConfig mock_config_7;
    EXPECT_CALL(mock_config_7, GetIntValue(HasSubstr(SingleKeyJoiner::kStoreBatchIntervalMs), _))
    .WillOnce(Return(dstream::error::FAILED_EXPECTATION)); // read kStoreBatchIntervalMs
    EXPECT_CALL(mock_config_7, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config_7, GetIntValue(Not(HasSubstr(SingleKeyJoiner::kStoreBatchIntervalMs)), _))
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
              dstream::generic_operator::SingleKeyJoiner::kDefaultStoreBatchIntervalMs);

}

// test init timed sliding window init fail
TEST_F(TestSingleJoiner, InitTimedSlidingWindowInitFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    // Set mock object
    int32_t ret_int_conf = 10;
    std::string ret_string_conf = "false";
    MockConfig mock_config;
    EXPECT_CALL(mock_config, GetValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_string_conf), Return(dstream::error::OK)));
    EXPECT_CALL(mock_config, GetIntValue(_, _))
    .WillRepeatedly(DoAll(SetArgPointee<1>(ret_int_conf), Return(dstream::error::OK)));

    MockTimedSlidingWindow<SingleKeyJoiner::KVTable> mock_window;
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
TEST_F(TestSingleJoiner, NormalDestroyTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    // Set mock object
    MockTimedSlidingWindow<SingleKeyJoiner::KVTable> mock_window;
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
TEST_F(TestSingleJoiner, NormalRecordTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    KeyType keys;
    DataType value;

    TestSingleJoiner::MockKVTable* mock_join_table_ptr = new TestSingleJoiner::MockKVTable;
    EXPECT_CALL(*mock_join_table_ptr, Insert(_, _, _))
    .WillOnce(Return(0));

    TestSingleJoiner::MockKVTablePtr mock_window_ptr(mock_join_table_ptr);
    MockTimedSlidingWindow<SingleKeyJoiner::KVTable>  mock_window;
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
TEST_F(TestSingleJoiner, ReplaceAndDuplicateRecordTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    uint32_t tuple_time = 123;
    KeyType keys = "test_key";
    DataType value = "test_value";
    DataType value1 = "test_value1";
    DataType join_value;

    // test replace
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, true, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, true, keys, value1));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, false, &join_value));
    ASSERT_STREQ(value1.c_str(), join_value.c_str());
    join_value.clear();

    // test duplicate
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Record(tuple_time, false, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::JOINER_RECORD_DATA_FAIL,
              joiner_->Record(tuple_time, false, keys, value));
    ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, false, &join_value));
    ASSERT_STREQ(value.c_str(), join_value.c_str());
    join_value.clear();

}


// test joiner record get window fail
TEST_F(TestSingleJoiner, JoinerRecordGetWindowFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    std::string keys = "test_key";
    std::string value = "test_value";

    joiner_->m_inited = true;

    MockTimedSlidingWindow<SingleKeyJoiner::KVTable> mock_window;
    TestSingleJoiner::MockKVTablePtr mock_window_ptr;
    mock_window_ptr.reset();
    EXPECT_CALL(mock_window, GetWindow(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(mock_window_ptr), Return(0)));
    joiner_->m_t_table = &mock_window;

    ASSERT_EQ(dstream::generic_operator::error::JOINER_GET_WINDOW_FAIL, joiner_->Record(tuple_time,
              true, keys, value));
}

// test joiner record insert data fail
TEST_F(TestSingleJoiner, JoinerRecordInsertDataFailTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    uint32_t tuple_time = 123456;
    KeyType keys;
    DataType value;

    joiner_->m_inited = true;

    // mock insert fail
    MockKVTable* mock_join_table_ptr = new MockKVTable;
    EXPECT_CALL(*mock_join_table_ptr, Insert(_, _, _))
    .WillOnce(Return(-1));

    MockKVTablePtr mock_window_ptr(mock_join_table_ptr);

    MockTimedSlidingWindow<SingleKeyJoiner::KVTable> mock_window;
    EXPECT_CALL(mock_window, GetWindow(_, _))
    .WillOnce(DoAll(SetArgumentPointee<1>(mock_window_ptr), Return(0)));
    joiner_->m_t_table = &mock_window;

    ASSERT_EQ(dstream::generic_operator::error::JOINER_RECORD_DATA_FAIL, joiner_->Record(tuple_time,
              false, keys, value));
}

// test joiner join ok with single record value
TEST_F(TestSingleJoiner, JoinerNormalSingleValueJoinTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    uint32_t record_num = 100;
    uint32_t base_time = 100;
    SingleRecord(record_num, base_time);

    KeyType keys;
    DataType join_value;
    std::vector<DataType>::iterator value_iter;

    // mock join key
    for (uint32_t i = base_time + 10; i < record_num + base_time; ++i) {
        keys = NumToString<uint32_t>(i + 1);
        ASSERT_EQ(dstream::generic_operator::error::OK, joiner_->Join(keys, true, &join_value));

        ASSERT_STREQ(join_value.c_str(), NumToString<uint32_t>(i + 1).c_str());
    }
}

// test joiner join fail
TEST_F(TestSingleJoiner, JoinerJoinKeyNotFoundTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    KeyType keys;
    DataType join_value;

    InitJoiner("test_joiner.cfg.xml");

    uint32_t record_num = 1;
    uint32_t base_time = 100;
    SingleRecord(record_num, base_time);

    // test uninit
    joiner_->m_inited = false;
    ASSERT_EQ(dstream::generic_operator::error::JOINER_UNINIT,
              joiner_->Join(keys, true, &join_value));
    joiner_->m_inited = true;
    join_value.clear();

    // mock join key ok
    keys = NumToString<uint32_t>(base_time + 1);
    ASSERT_EQ(dstream::generic_operator::error::OK,
              joiner_->Join(keys, true, &join_value));
    ASSERT_STRNE("", join_value.c_str());

    // mock join key fail
    keys = NumToString<uint32_t>(record_num + base_time + 1);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_value));

    keys = NumToString<uint32_t>(base_time);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_value));

    keys = NumToString<uint32_t>(0);
    ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
              joiner_->Join(keys, true, &join_value));

}

static uint32_t data_count = 0;
void user_call_back(const KeyType& key, const DataType& data, void* context) {
    printf("key(%s) data(%s) context(%p).\n", key.c_str(), data.c_str(), context);
    data_count++;
}

// test joiner join with user callback
TEST_F(TestSingleJoiner, JoinerJoinWithUserCallbackTest) {
    ASSERT_NE(joiner_, reinterpret_cast<SingleKeyJoiner*>(NULL));

    InitJoiner("test_joiner.cfg.xml");

    joiner_->RegisterDataTimeOutCallBack(user_call_back, (void*)0x12345);

    uint32_t record_num = 100;
    uint32_t base_time = 100;
    SingleRecord(record_num * 2, base_time);

    KeyType keys;
    DataType join_value;

    // mock join key
    uint32_t expect_count = 0;
    for (uint32_t i = base_time; i < record_num + base_time + joiner_->m_step_in_sec - 1; ++i) {
        keys = NumToString<uint32_t>(i + 1);
        printf("join key(%s).\n", keys.c_str());
        ASSERT_EQ(dstream::generic_operator::error::JOINER_KEY_NOT_FOUND,
                  joiner_->Join(keys, true, &join_value));
        expect_count += 1;
    }
    ASSERT_EQ(expect_count, data_count);
}

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_joiner");
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}
