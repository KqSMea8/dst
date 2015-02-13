/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_COMMON_COUNTERS_H__
#define __DSTREAM_CC_COMMON_COUNTERS_H__

#include <stddef.h>
#include <stdint.h>
#include <string>
#include "common/mutex.h"

namespace dstream {
namespace common {

struct MappedCounter;

//
// currently two types of counter are supported
// 1. rate counter:
//    rate counter is an increaming number, and could be used to calculate the increaming rate
//    to use rate counter, just increment it or add value to it;
//    the dumper tool will calculate the rate
//    rate counter could be used to monitor things like QPS
// 2. num counter:
//    num counter is just a number, and just set its value when used
//    the dumper tool will calculate the counter's statistics (e.g., its average value)
//    rate counter could be used to monitor things like latency
//
enum CounterType {
    kCounterTypeRate = 0,
    kCounterTypeNum = 1
};

//
// rate counter with underlying uint64 data
// the object could only be obtained with Counters::CreateUint64RateCounter()
// it's lock free
//
class Uint64RateCounter {
public:
    ~Uint64RateCounter();

    // increment the counter by 1
    void Increment();
    // add val to the counter
    void Add(uint64_t val);

    // get current value of the counter
    uint64_t GetValue();

private:
    explicit Uint64RateCounter(MappedCounter* mapped_counter);

    // pointer that points to the counter data,
    // which is actually in the shared memory mapped to the counter file
    volatile uint64_t* m_data_ptr;

    MappedCounter* m_mapped_counter;

    friend class CountersInstance;
};

//
// num counter with underlying uint64 data
// the object could only be obtained with Counters::CreateUint64NumCounter()
// it's lock free
//
class Uint64NumCounter {
public:
    ~Uint64NumCounter();

    // set counter value
    void Set(uint64_t val);

    // get current counter value
    uint64_t GetValue();

private:
    explicit Uint64NumCounter(MappedCounter* mapped_counter);

    // update statistics of the counter
    void UpdateStatistics(uint64_t val);

    // pointer that points to the counter data,
    // which is actually in the shared memory mapped to the counter file
    volatile uint64_t* m_data_ptr;

    // pointer that points to the data of sum of counters, which is also in the shared memory
    // sum is used to calculate average value, avg = sum/count
    volatile uint64_t* m_data_sum_ptr;

    // pointer that points to the data of counter total count, which is also in the shared memory
    // count is used to calculate average value, avg = sum/count
    volatile uint64_t* m_data_count_ptr;

    MappedCounter* m_mapped_counter;

    friend class CountersInstance;
};

//
// use Counters class to create counters in your process
// usage:
//    1. call Init() when the process starts, commonly in the main() function.
//       note that loggers need to be inited before counters are inited,
//       as counters use logger to log errors
//    2. call CreateXxxCounter() to create counter objects that you want to use
//       the counter object will not be recycled automatically, delete it by your own
//    3. call Deinit() when the process ends (not necessarily)
//
// it's thread safe
//
class Counters {
public:
    // default file path will be "./counter", default name will be "pid.counter"
    static bool Init(const char* mapping_file_path = NULL, const char* mapping_file_name = NULL);

    static bool Deinit();

    static Uint64RateCounter* CreateUint64RateCounter(const char* group_name,
            const char* name,
            uint64_t attr1 = 0,
            uint64_t attr2 = 0,
            uint64_t attr3 = 0,
            uint32_t collect_interval_in_sec = 10);

    static Uint64NumCounter* CreateUint64NumCounter(const char* group_name,
            const char* name,
            uint64_t attr1 = 0,
            uint64_t attr2 = 0,
            uint64_t attr3 = 0,
            uint32_t collect_interval_in_sec = 10);
};


//
// below are classes and structs used for counters implementation
//

// align with 64-bit to guarantee atomic set/get operation for uint64 values
#pragma pack(push, 8)

//
// structure of a single counter the is stored in the shared memory mapped to the counter file
//
struct MappedCounter {
    // the counter data
    uint64_t counter_data;

    // sum of all counters since last reset, used to calculate average
    // average = data_sum/data_count
    // used by num counter
    uint64_t data_sum;

    // count of all counters since last reset, used to calculate average
    // average = data_sum/data_count
    // used by num counter
    uint64_t data_count;

    // previous counter value when last reset, used to calculate rate
    // rate = (counter_data - prev_data) / (current_time - last_reset_time)
    // used by rate counter
    uint64_t prev_data;

    // last reset time, in microsecond
    // used by rate counter to calculate rate
    uint64_t last_reset_time;

    // next counter's offset in the shared memory
    // 0 means no more counters
    uint32_t next_counter_offset;

    // attrs defined for the counter
    uint64_t attr1;
    uint64_t attr2;
    uint64_t attr3;

    // counter type
    CounterType counter_type;

    // counter collect interval in second
    uint32_t collect_interval_in_sec;

    // indicate whether the counter is in use
    volatile int64_t ref_count;

    // get counter name address
    // counter name will be stored right after this object
    char* GetCounterNameAddress();
    const char* GetCounterName();

    void ResetData();

    bool HasAttribute(const char* name_in,
            CounterType type_in, uint64_t attr1_in, uint64_t attr2_in, uint64_t attr3_in);

    // for rate counter, calculate rate from last reset, and reset
    // rate = (counter_data - prev_data) / (current_time - last_reset_time), in unit of num/second
    uint64_t GetRateAndReset();

    // for num counter, calculate statistics from last reset, and reset
    // avg = data_sum/data_count
    void GetStatisticsAndReset(uint64_t* avg);
};

// structure of the mapping file header
//
struct MappedHeader {
    // magic number to indicate whether it's a valid counter file
    uint32_t header_magic_num;

    // the first counter's offset in the shared memory
    // 0 means there's no counter
    uint32_t first_counter_offset;
};

#pragma pack(pop)

//
// there will be a global object of CountersInstance,
// which will actually maintain counters mapping for the process
//
class CountersInstance {
public:
    CountersInstance();
    ~CountersInstance();

    bool Init(const char* mapping_file_path, const char* mapping_file_name);
    bool IsInitialized();

    bool Deinit();

    Uint64RateCounter* CreateUint64RateCounter(const char* group_name,
            const char* name,
            uint64_t attr1,
            uint64_t attr2,
            uint64_t attr3,
            uint32_t collect_interval_in_sec);

    Uint64NumCounter* CreateUint64NumCounter(const char* group_name,
            const char* name,
            uint64_t attr1,
            uint64_t attr2,
            uint64_t attr3,
            uint32_t collect_interval_in_sec);

private:
    MappedCounter* AllocCounter(const char* group_name,
            const char* name,
            uint64_t attr1,
            uint64_t attr2,
            uint64_t attr3,
            uint64_t collect_interval_in_sec,
            CounterType counter_type);

    // the file name with full path where the counters are mapped into
    std::string m_mapped_file;

    bool m_initialized;

    // pointer that points to the mapped header
    // which is also the start of shared memory mapped to the counter file
    MappedHeader* m_mapped_header;

    // next available offset in the shared memory
    uint64_t m_next_avail_offset;

    // mutex lock to guarantee safety when allocating counters in multi threads
    dstream::MutexLock m_alloc_lock;
};

//
// iterator to iterate all counters in the mapping file
//
class CountersIterator {
public:
    CountersIterator();
    ~CountersIterator();

    bool Init(const char* filename);
    bool Deinit();

    // return current counter and move to next
    MappedCounter* Next();

    void Reset();

private:
    // points to the mapped header
    // which is also the start of shared memory mapped to the counter file
    MappedHeader* m_mapped_header;

    // pointer that points to the current counter
    MappedCounter* m_current_counter;
};

} // namespace common
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_COUNTERS_H__
