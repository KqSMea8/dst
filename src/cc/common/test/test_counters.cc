#include "gtest/gtest.h"
#include "common/counters.h"
#include "common/logger.h"
#include <sys/types.h>
#include <unistd.h>
#include <sstream>

using namespace std;
using namespace dstream;
using namespace dstream::common;

static const char* kCounterPath = "./counter";

class CountersEnv : public ::testing::Environment {
public:
    virtual void SetUp()
    {
        dstream::logger::initialize("counters_test");
        ASSERT_TRUE(Counters::Init());
    }

    virtual void TearDown() {
        Counters::Deinit();
    }
};

class CountersTest : public ::testing::Test {
public:
    ~CountersTest();

protected:
    void CreateAndPlayRateCounter(uint64_t id);
    static void* RateCounterThreadEntry(void* arg);
    void CreateAndPlayNumCounter(uint64_t id);
    static void* NumCounterThreadEntry(void* arg);
    void CheckCounters();
    void CheckCounter(MappedCounter* mapped_counter);
    void CheckCountersIterator(CountersIterator& counters_iterator);
    void CheckRateCounter(MappedCounter* mapped_counter, uint64_t id);
    void CheckNumCounter(MappedCounter* mapped_counter, uint64_t id);

    struct ThreadContext {
        CountersTest* p_this;
        uint64_t id;
    };

    static const char* kIteratorTestRateCounterGroup;
    static const char* kIteratorTestRateCounterPrefix;
    static const char* kIteratorTestNumCounterGroup;
    static const char* kIteratorTestNumCounterPrefix;
    static const size_t kIteratorTestThreadNum;
    static const size_t kIteratorTestCounterNum;
    static const size_t kIteratorTestLoopNum;

    std::vector<Uint64RateCounter*> rate_counters_;
    std::vector<Uint64NumCounter*> num_counters_;
    dstream::MutexLock vector_lock_;
};

const char* CountersTest::kIteratorTestRateCounterGroup = "itr_test_rate";
const char* CountersTest::kIteratorTestRateCounterPrefix = "rate_";
const char* CountersTest::kIteratorTestNumCounterGroup = "itr_test_num";
const char* CountersTest::kIteratorTestNumCounterPrefix = "num_";
const size_t CountersTest::kIteratorTestThreadNum = 1000;
const size_t CountersTest::kIteratorTestCounterNum = 10;
const size_t CountersTest::kIteratorTestLoopNum = 10000;

CountersTest::~CountersTest() {
    MutexLockGuard vec_lock(vector_lock_);
    for (size_t i = 0; i < rate_counters_.size(); ++i) {
        delete rate_counters_[i];
    }
    for (size_t i = 0; i < num_counters_.size(); ++i) {
        delete num_counters_[i];
    }
}

void CountersTest::CreateAndPlayRateCounter(uint64_t id) {
    stringstream name;
    name << kIteratorTestRateCounterPrefix << id;
    Uint64RateCounter* rate_counter = Counters::CreateUint64RateCounter(kIteratorTestRateCounterGroup, name.str().c_str(), id, id + 1, id + 2, id * 10);
    ASSERT_TRUE(rate_counter != NULL);
    for (size_t i = 0; i < id * kIteratorTestLoopNum; ++i) {
        rate_counter->Increment();
        rate_counter->Add(i);
    }
    MutexLockGuard vec_lock(vector_lock_);
    rate_counters_.push_back(rate_counter);
}

void* CountersTest::RateCounterThreadEntry(void* arg) {
    ThreadContext* context = (ThreadContext*)arg;
    uint64_t id = context->id % kIteratorTestCounterNum;
    context->p_this->CreateAndPlayRateCounter(id);
    delete context;
    return NULL;
}

void CountersTest::CreateAndPlayNumCounter(uint64_t id) {
    stringstream name;
    name << kIteratorTestNumCounterPrefix << id;
    Uint64NumCounter* num_counter = Counters::CreateUint64NumCounter(kIteratorTestNumCounterGroup, name.str().c_str(), id, id + 1, id + 2, id * 10);
    ASSERT_TRUE(num_counter != NULL);
    for (size_t i = 0; i < id * kIteratorTestLoopNum; ++i) {
        num_counter->Set(i * id);
    }
    MutexLockGuard vec_lock(vector_lock_);
    num_counters_.push_back(num_counter);
}

void* CountersTest::NumCounterThreadEntry(void* arg) {
    ThreadContext* context = (ThreadContext*)arg;
    uint64_t id = context->id % kIteratorTestCounterNum;
    context->p_this->CreateAndPlayNumCounter(id);
    delete context;
    return NULL;
}

void CountersTest::CheckCounters() {
    stringstream counter_file_name;
    counter_file_name << kCounterPath << "/" << getpid() << ".counter";
    CountersIterator counters_iterator;
    bool init_ret = counters_iterator.Init(counter_file_name.str().c_str());
    ASSERT_TRUE(init_ret);
    CheckCountersIterator(counters_iterator);
    // reset and check again
    counters_iterator.Reset();
    CheckCountersIterator(counters_iterator);
}

void CountersTest::CheckCountersIterator(CountersIterator& counters_iterator) {
    vector<bool> rate_counter_created_array(kIteratorTestCounterNum, false);
    vector<bool> num_counter_created_array(kIteratorTestCounterNum, false);
    MappedCounter* mapped_counter;
    while ((mapped_counter = counters_iterator.Next()) != NULL) {
        string name(mapped_counter->GetCounterName());
        if (name.find(kIteratorTestRateCounterGroup) == 0) {
            uint64_t id = mapped_counter->attr1;
            CheckRateCounter(mapped_counter, id);
            rate_counter_created_array[id] = true;
        } else if (name.find(kIteratorTestNumCounterGroup) == 0) {
            uint64_t id = mapped_counter->attr1;
            CheckNumCounter(mapped_counter, id);
            num_counter_created_array[id] = true;
        }
    }
    // check all counters are created
    for (size_t i = 0; i < kIteratorTestCounterNum; ++i) {
        ASSERT_TRUE(rate_counter_created_array[i]);
    }
    for (size_t i = 0; i < kIteratorTestCounterNum; ++i) {
        ASSERT_TRUE(num_counter_created_array[i]);
    }
}

void CountersTest::CheckRateCounter(MappedCounter* mapped_counter, uint64_t id) {
    ASSERT_TRUE(mapped_counter != NULL);
    stringstream expected_name;
    expected_name << kIteratorTestRateCounterGroup << "." << kIteratorTestRateCounterPrefix << id;
    ASSERT_EQ(expected_name.str(), mapped_counter->GetCounterName());
    ASSERT_EQ(id, mapped_counter->attr1);
    ASSERT_EQ(id + 1, mapped_counter->attr2);
    ASSERT_EQ(id + 2, mapped_counter->attr3);
    ASSERT_EQ(id * 10, mapped_counter->collect_interval_in_sec);
    ASSERT_EQ(kCounterTypeRate, mapped_counter->counter_type);
    uint64_t expected_value = 0;
    for (size_t i = 0; i < id * kIteratorTestLoopNum; ++i) {
        ++expected_value;
        expected_value += i;
    }
    expected_value *= kIteratorTestThreadNum / kIteratorTestCounterNum;
    ASSERT_EQ(expected_value, mapped_counter->counter_data);
    ASSERT_EQ((uint64_t)0, mapped_counter->prev_data);
    ASSERT_EQ((uint64_t)0, mapped_counter->last_reset_time);
}

void CountersTest::CheckNumCounter(MappedCounter* mapped_counter, uint64_t id) {
    stringstream expected_name;
    expected_name << kIteratorTestNumCounterGroup << "." << kIteratorTestNumCounterPrefix << id;
    ASSERT_EQ(expected_name.str(), mapped_counter->GetCounterName());
    ASSERT_EQ(id, mapped_counter->attr1);
    ASSERT_EQ(id + 1, mapped_counter->attr2);
    ASSERT_EQ(id + 2, mapped_counter->attr3);
    ASSERT_EQ(id * 10, mapped_counter->collect_interval_in_sec);
    ASSERT_EQ(kCounterTypeNum, mapped_counter->counter_type);
    ASSERT_EQ((uint64_t)((kIteratorTestLoopNum * id - 1) * id), mapped_counter->counter_data);
    ASSERT_EQ((uint64_t)(kIteratorTestLoopNum * id * kIteratorTestThreadNum / kIteratorTestCounterNum), mapped_counter->data_count);
    ASSERT_EQ((uint64_t)0, mapped_counter->last_reset_time);
    uint64_t expected_sum = 0;
    for (uint64_t i = 0; i < kIteratorTestLoopNum * id; ++i) {
        expected_sum += i * id;
    }
    expected_sum *= kIteratorTestThreadNum / kIteratorTestCounterNum;
    ASSERT_EQ(expected_sum, mapped_counter->data_sum);
}


TEST_F(CountersTest, TestSingleRateCounter) {
    Uint64RateCounter* rate_counter = Counters::CreateUint64RateCounter("test", "rate1");
    ASSERT_TRUE(rate_counter != NULL);

    // test increment
    uint64_t expected_value = 0;
    for (uint64_t i = 0; i < 10000; ++i) {
        rate_counter->Increment();
        ++expected_value;
    }
    ASSERT_EQ(expected_value, rate_counter->GetValue());

    // test add
    for (uint64_t i = 0; i < 100; ++i) {
        rate_counter->Add(i);
        expected_value += i;
    }
    ASSERT_EQ(expected_value, rate_counter->GetValue());

    delete rate_counter;
}

TEST_F(CountersTest, TestSingleNumCounter) {
    Uint64NumCounter* num_counter = Counters::CreateUint64NumCounter("test", "num1");
    ASSERT_TRUE(num_counter != NULL);

    // test set
    uint64_t set_value = 12345789;
    num_counter->Set(set_value);
    ASSERT_EQ(set_value, num_counter->GetValue());

    delete num_counter;
}

TEST_F(CountersTest, TestCountersIterator) {
    ASSERT_EQ((size_t)0, kIteratorTestThreadNum % kIteratorTestCounterNum);
    vector<pthread_t> pid_array;
    for (uint64_t i = 0; i < kIteratorTestThreadNum; ++i) {
        pthread_t pid;
        ThreadContext *context = new ThreadContext;
        context->p_this = this;
        context->id = i;
        int ret = pthread_create(&pid, NULL, RateCounterThreadEntry, (void*)context);
        ASSERT_EQ(0, ret);
        pid_array.push_back(pid);
    }
    for (uint64_t i = 0; i < kIteratorTestThreadNum; ++i) {
        pthread_t pid;
        ThreadContext *context = new ThreadContext;
        context->p_this = this;
        context->id = i;
        int ret = pthread_create(&pid, NULL, NumCounterThreadEntry, (void*)context);
        ASSERT_EQ(0, ret);
        pid_array.push_back(pid);
    }
    for (uint64_t i = 0; i < pid_array.size(); ++i) {
        int ret = pthread_join(pid_array[i], NULL);
        ASSERT_EQ(0, ret);
    }
    CheckCounters();
}

TEST_F(CountersTest, TestReuse) {
    for (int i = 0; i < 100000; ++i) {
        stringstream name;
        name << i;
        Uint64RateCounter* rate_counter = Counters::CreateUint64RateCounter("test", name.str().c_str());
        ASSERT_TRUE(rate_counter != NULL);
        Uint64NumCounter* num_counter = Counters::CreateUint64NumCounter("test", name.str().c_str());
        ASSERT_TRUE(num_counter != NULL);
        delete rate_counter;
        delete num_counter;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    AddGlobalTestEnvironment(new CountersEnv());
    return RUN_ALL_TESTS();
}
