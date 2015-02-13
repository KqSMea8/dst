#include "gtest/gtest.h"
#include "common/logger.h"

#include "common/time_cache_map.h"

namespace dstream {

class TestTimeCacheMap : public ::testing::Test
{
public:
    TestTimeCacheMap() : time_cache_map_(NULL) {};
    virtual ~TestTimeCacheMap() {};

protected:
    void SetUp();
    void TearDown();
    void GetPutTest(int test_num);
    void TimeOutTest(int test_num);
    void RemoveTest(int test_num);

private:
    TimeCacheMap<uint64_t, uint64_t>* time_cache_map_;
};

void test_call_back(const uint64_t& v) {
    DSTREAM_DEBUG("on call back with v = %lu", v);
}

void TestTimeCacheMap::SetUp() {
    time_cache_map_ = new TimeCacheMap<uint64_t, uint64_t>(10, 5, test_call_back);
}

void TestTimeCacheMap::TearDown() {
    delete time_cache_map_;
}

void TestTimeCacheMap::GetPutTest(int test_num) {
    // code below must exec in 2 seconds, for timeout = 10 / (5 - 1)
    ASSERT_EQ(time_cache_map_->size(), 0UL);

    for (uint64_t i = 1; i <= test_num; ++i) {
        ASSERT_EQ(time_cache_map_->contain_key(i), false);

        uint64_t tmp = 0;
        time_cache_map_->get(i, &tmp);
        ASSERT_EQ(tmp, 0UL);

        time_cache_map_->put(i, test_num - i);
        ASSERT_EQ(time_cache_map_->contain_key(i), true);

        tmp = 0;
        time_cache_map_->get(i, &tmp);
        ASSERT_EQ(tmp, test_num - i);
    }

    ASSERT_EQ(time_cache_map_->size(), (size_t)test_num);
}

void TestTimeCacheMap::RemoveTest(int test_num) {
    // code below must exec in 2 seconds, for timeout = 10 / (5 - 1)
    ASSERT_EQ(time_cache_map_->size(), 0UL);

    for (uint64_t i = 1; i <= test_num; ++i) {
        time_cache_map_->put(i, test_num - i);
        ASSERT_EQ(time_cache_map_->contain_key(i), true);

        uint64_t tmp = 0;
        time_cache_map_->remove(i, &tmp);
        ASSERT_EQ(tmp, test_num - i);
    }

    ASSERT_EQ(time_cache_map_->size(), 0UL);

    // remove the one which map is not contain
    uint64_t tmp = 0;
    time_cache_map_->remove(1000, &tmp);
    ASSERT_EQ(tmp, 0UL);

    ASSERT_EQ(time_cache_map_->size(), 0UL);

}


void TestTimeCacheMap::TimeOutTest(int test_num) {
    // code below must exec in 2 seconds, for timeout = 10 / (5 - 1)
    ASSERT_EQ(time_cache_map_->size(), 0UL);

    for (uint64_t i = 1; i <= test_num; ++i) {
        time_cache_map_->put(i, test_num - i);
        ASSERT_EQ(time_cache_map_->contain_key(i), true);

        if (0 == i % (test_num / 4)) {
            sleep(3); // wait for next bucket
        }
    }

    sleep(1);
    ASSERT_EQ(time_cache_map_->size(), (size_t)test_num / 4 * 3);

    sleep(3); // wait for timeout
    ASSERT_EQ(time_cache_map_->size(), (size_t)test_num / 4 * 2);

    sleep(3); // wait for timeout
    ASSERT_EQ(time_cache_map_->size(), (size_t)test_num / 4);

    sleep(3); // wait for timeout
    ASSERT_EQ(time_cache_map_->size(), 0UL);

}



TEST_F(TestTimeCacheMap, test_get_put)
{
    GetPutTest(100);
}


TEST_F(TestTimeCacheMap, test_remove)
{
    RemoveTest(100);
}

TEST_F(TestTimeCacheMap, test_timeout)
{
    TimeOutTest(100);
}

}

int main(int argc, char** argv) {
    ::dstream::logger::initialize("test_time_cache_map");
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

