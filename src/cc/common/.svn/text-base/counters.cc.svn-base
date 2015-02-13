/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "common/counters.h"
#include "common/logger.h"
#include "common/utils.h"

using namespace std;
using namespace dstream;
using namespace dstream::common;

static const uint32_t kHeaderMagicNum = 0xABCD;
static const size_t kMappedFileSize = getpagesize() * 512;
static const uint64_t kMicrosecondsPerSecond = 1000000;


// implementations of Uint64RateCounter
Uint64RateCounter::Uint64RateCounter(MappedCounter* mapped_counter)
        : m_data_ptr(&mapped_counter->counter_data),
        m_mapped_counter(mapped_counter) {
}

Uint64RateCounter::~Uint64RateCounter() {
    if (m_mapped_counter != NULL) {
        AtomicAdd64(&m_mapped_counter->ref_count, -1);
    }
}

void Uint64RateCounter::Increment() {
    AtomicAdd64((volatile int64_t*)m_data_ptr, 1);
}

void Uint64RateCounter::Add(uint64_t val) {
    AtomicAdd64((volatile int64_t*)m_data_ptr, val);
}

uint64_t Uint64RateCounter::GetValue() {
    // for 64-bit aligned data, get is atomic
    return *m_data_ptr;
}


// implementations of Uint64NumCounter
Uint64NumCounter::Uint64NumCounter(MappedCounter* mapped_counter)
        : m_data_ptr(&mapped_counter->counter_data),
        m_data_sum_ptr(&mapped_counter->data_sum),
        m_data_count_ptr(&mapped_counter->data_count),
        m_mapped_counter(mapped_counter) {
}

Uint64NumCounter::~Uint64NumCounter() {
    if (m_mapped_counter != NULL) {
        AtomicAdd64(&m_mapped_counter->ref_count, -1);
    }
}

void Uint64NumCounter::Set(uint64_t val) {
    // for 64-bit aligned data, set is atomic
    *m_data_ptr = val;
    UpdateStatistics(val);
}

void Uint64NumCounter::UpdateStatistics(uint64_t val) {
    AtomicAdd64((volatile int64_t*)m_data_sum_ptr, val);
    AtomicAdd64((volatile int64_t*)m_data_count_ptr, 1);
}

uint64_t Uint64NumCounter::GetValue() {
    // for 64-bit aligned data, set is atomic
    return *m_data_ptr;
}


// global counters instance which will actually maintain all counters mapping for the process
static CountersInstance* g_counters = NULL;
// use this lock to guarantee global counters instance is initialized only once
static MutexLock g_counters_init_lock;

// implementation of Counters
bool Counters::Init(const char* mapping_file_path, const char* mapping_file_name) {
    // use default file path and name if not provided
    if (mapping_file_path == NULL) {
        // default path
        mapping_file_path = "./counter";
    }
    char default_file_name[255];
    if (mapping_file_name == NULL) {
        snprintf(default_file_name, sizeof(default_file_name), "%d.counter", getpid());
        mapping_file_name = default_file_name;
    }

    MutexLockGuard init_lock(g_counters_init_lock);
    if (g_counters != NULL) {
        return g_counters->IsInitialized();
    }
    g_counters = new CountersInstance();
    return g_counters->Init(mapping_file_path, mapping_file_name);
}

bool Counters::Deinit() {
    delete g_counters;
    g_counters = NULL;
    return true;
}

Uint64RateCounter* Counters::CreateUint64RateCounter(const char* group_name,
        const char* name,
        uint64_t attr1,
        uint64_t attr2,
        uint64_t attr3,
        uint32_t collect_interval_in_sec) {
    if (g_counters == NULL) {
        return NULL;
    }
    return g_counters->CreateUint64RateCounter(group_name,
            name, attr1, attr2, attr3, collect_interval_in_sec);
}

Uint64NumCounter* Counters::CreateUint64NumCounter(const char* group_name,
        const char* name,
        uint64_t attr1,
        uint64_t attr2,
        uint64_t attr3,
        uint32_t collect_interval_in_sec) {
    if (g_counters == NULL) {
        return NULL;
    }
    return g_counters->CreateUint64NumCounter(group_name,
            name, attr1, attr2, attr3, collect_interval_in_sec);
}

// implementation of CountersInstance
CountersInstance::CountersInstance()
        : m_initialized(false) {
}

CountersInstance::~CountersInstance() {
    Deinit();
}

bool CountersInstance::Init(const char* mapping_file_path, const char* mapping_file_name) {
    // try mkdir
    mkdir(mapping_file_path, 0775);
    m_mapped_file = mapping_file_path;
    m_mapped_file += "/";
    m_mapped_file += mapping_file_name;
    int mapped_fd = open(m_mapped_file.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0664);
    if (mapped_fd == -1) {
        DSTREAM_ERROR("failed to open %s", m_mapped_file.c_str());
        return false;
    }
    // extend the counter file size to hold mapped data
    if (ftruncate(mapped_fd, kMappedFileSize) != 0) {
        DSTREAM_ERROR("failed to truncate for %s", m_mapped_file.c_str());
        close(mapped_fd);
        return false;
    }

    void* mapped_data = mmap(NULL,
            kMappedFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, mapped_fd, 0);
    // close the fd after mmap is called
    if (close(mapped_fd) != 0) {
        DSTREAM_ERROR("failed to close %s", m_mapped_file.c_str());
    }
    if (mapped_data == MAP_FAILED) {
        DSTREAM_ERROR("mmap failed");
        return false;
    }
    memset(mapped_data, 0, kMappedFileSize);
    m_mapped_header = reinterpret_cast<MappedHeader*>(mapped_data);
    m_mapped_header->header_magic_num = kHeaderMagicNum;
    m_mapped_header->first_counter_offset = 0;

    m_next_avail_offset = sizeof(MappedHeader);

    m_initialized = true;
    return true;
}

bool CountersInstance::Deinit() {
    bool ret = true;
    if (m_initialized) {
        if (munmap(m_mapped_header, kMappedFileSize) != 0) {
            DSTREAM_ERROR("munmap failed");
            ret = false;
        }
        if (unlink(m_mapped_file.c_str()) != 0) {
            DSTREAM_ERROR("removing mapping file failed");
            ret = false;
        }
    }
    m_initialized = false;
    return ret;
}

bool CountersInstance::IsInitialized() {
    return m_initialized;
}

Uint64RateCounter* CountersInstance::CreateUint64RateCounter(const char* group_name,
        const char* name,
        uint64_t attr1,
        uint64_t attr2,
        uint64_t attr3,
        uint32_t collect_interval_in_sec) {
    MappedCounter* mapped_counter = AllocCounter(group_name,
            name, attr1, attr2, attr3, collect_interval_in_sec, kCounterTypeRate);
    if (mapped_counter == NULL) {
        DSTREAM_ERROR("failed to allocate mapped counter for %s.%s", group_name, name);
        return NULL;
    }
    return new Uint64RateCounter(mapped_counter);
}


Uint64NumCounter* CountersInstance::CreateUint64NumCounter(const char* group_name,
        const char* name,
        uint64_t attr1,
        uint64_t attr2,
        uint64_t attr3,
        uint32_t collect_interval_in_sec) {
    MappedCounter* mapped_counter = AllocCounter(group_name,
            name, attr1, attr2, attr3, collect_interval_in_sec, kCounterTypeNum);
    if (mapped_counter == NULL) {
        DSTREAM_ERROR("failed to allocate mapped counter for %s.%s", group_name, name);
        return NULL;
    }
    return new Uint64NumCounter(mapped_counter);
}

MappedCounter* CountersInstance::AllocCounter(const char* group_name,
        const char* name,
        uint64_t attr1,
        uint64_t attr2,
        uint64_t attr3,
        uint64_t collect_interval_in_sec,
        CounterType counter_type) {
    assert(m_initialized);

    // counter_name = group_name.name
    string counter_name(group_name);
    counter_name += ".";
    counter_name += name;

    MutexLockGuard alloc_lock(m_alloc_lock);

    // the counter that can be reused
    MappedCounter* reused_counter = NULL;

    // find the last counter in the counters list
    MappedCounter* last_counter = NULL;
    size_t offset = m_mapped_header->first_counter_offset;
    while (offset != 0) {
        last_counter = reinterpret_cast<MappedCounter*>(
                reinterpret_cast<uint8_t*>(m_mapped_header) + offset);
        if (last_counter->ref_count == 0
                && strlen(last_counter->GetCounterName()) >= counter_name.length()) {
            // the counter can be reused when its ref_count is zero and has enough size for name
            // counters allocation is exclusive so it's safe
            reused_counter = last_counter;
            break;
        }
        if (last_counter->HasAttribute(counter_name.c_str(), counter_type, attr1, attr2, attr3)) {
            DSTREAM_INFO("counters %s has already been allocated", counter_name.c_str());
            AtomicAdd64(&last_counter->ref_count, 1);
            return last_counter;
        }
        offset = last_counter->next_counter_offset;
    }

    MappedCounter* new_counter = NULL;
    size_t alloc_size = 0;
    if (reused_counter != NULL) {
        new_counter = reused_counter;
        new_counter->ResetData();
    } else {
        alloc_size = sizeof(MappedCounter) + counter_name.length() + 1;
        // round up to align with 64-bit
        alloc_size = (alloc_size + 7) & ~(size_t)7;
        // also round up m_next_avail_offset to align with 64-bit
        m_next_avail_offset = (m_next_avail_offset + 7) & ~(size_t)7;
        if (m_next_avail_offset + alloc_size > kMappedFileSize) {
            // there's no enough space for the new counter
            DSTREAM_ERROR("no enough space for new counters %s", counter_name.c_str());
            return NULL;
        }
        new_counter = reinterpret_cast<MappedCounter*>(
                reinterpret_cast<uint8_t*>(m_mapped_header) + m_next_avail_offset);
        memset(new_counter, 0, alloc_size);
    }
    snprintf(new_counter->GetCounterNameAddress(),
            counter_name.size() + 1, "%s", counter_name.c_str());
    new_counter->counter_type = counter_type;
    new_counter->attr1 = attr1;
    new_counter->attr2 = attr2;
    new_counter->attr3 = attr3;
    new_counter->collect_interval_in_sec = collect_interval_in_sec;
    AtomicAdd64(&new_counter->ref_count, 1);

    // link the new counter to the counters list, if it's newly allocated
    if (reused_counter == NULL) {
        if (last_counter != NULL) {
            last_counter->next_counter_offset = m_next_avail_offset;
        } else {
            m_mapped_header->first_counter_offset = m_next_avail_offset;
        }
        m_next_avail_offset += alloc_size;
    }

    return new_counter;
}


// implementation of MappedCounter
char* MappedCounter::GetCounterNameAddress() {
    return reinterpret_cast<char*>(this) + sizeof(*this);
}

const char* MappedCounter::GetCounterName() {
    return GetCounterNameAddress();
}

void MappedCounter::ResetData() {
    counter_data = 0;
    data_sum = 0;
    data_count = 0;
    prev_data = 0;
    last_reset_time = 0;
}

bool MappedCounter::HasAttribute(const char* name_in,
        CounterType type_in, uint64_t attr1_in, uint64_t attr2_in, uint64_t attr3_in) {
    return strcmp(GetCounterName(), name_in) == 0
        && counter_type == type_in
        && attr1 == attr1_in
        && attr2 == attr2_in
        && attr3 == attr3_in;
}

uint64_t MappedCounter::GetRateAndReset() {
    timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t cur_time = tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec;
    uint64_t rate = 0;
    uint64_t last_time_local = last_reset_time;
    uint64_t counter_data_local = counter_data;
    uint64_t prev_data_local = prev_data;

    if (cur_time > last_time_local && counter_data_local > prev_data_local) {
        rate = (counter_data_local - prev_data_local) * kMicrosecondsPerSecond /
            (cur_time - last_time_local);
    }

    // reset
    prev_data = counter_data;
    last_reset_time = cur_time;

    return rate;
}

void MappedCounter::GetStatisticsAndReset(uint64_t* avg) {
    uint64_t data_count_local = data_count;
    uint64_t data_sum_local = data_sum;
    if (data_count_local != 0) {
        *avg = data_sum_local / data_count_local;
    } else {
        *avg = 0;
    }

    data_sum = 0;
    data_count = 0;
}


// implementation of CountersIterator
CountersIterator::CountersIterator()
        : m_mapped_header(NULL),
        m_current_counter(NULL) {
}

CountersIterator::~CountersIterator() {
    Deinit();
}

bool CountersIterator::Init(const char* filename) {
    int mapped_fd = open(filename, O_RDWR);
    if (mapped_fd == -1) {
        DSTREAM_ERROR("failed to open %s", filename);
        return false;
    }
    // check the file size
    struct stat file_stat;
    if (fstat(mapped_fd, &file_stat) != 0) {
        DSTREAM_ERROR("failed to stat %s", filename);
        return false;
    }
    if (static_cast<unsigned int>(file_stat.st_size) < sizeof(kHeaderMagicNum)) {
        DSTREAM_ERROR("file %s is too small, it could not be a counter file", filename);
        return false;
    }
    // mmap the file
    void* mapped_data = mmap(NULL,
            kMappedFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, mapped_fd, 0);
    // close the fd after mmap is called
    if (close(mapped_fd) != 0) {
        DSTREAM_ERROR("failed to close %s", filename);
    }
    if (mapped_data == MAP_FAILED) {
        DSTREAM_ERROR("mmap failed");
        return false;
    }
    m_mapped_header = reinterpret_cast<MappedHeader*>(mapped_data);
    if (m_mapped_header->header_magic_num != kHeaderMagicNum) {
        DSTREAM_ERROR("wrong header in file %s", filename);
        return false;
    }
    if (m_mapped_header->first_counter_offset != 0) {
        m_current_counter = reinterpret_cast<MappedCounter*>(
                reinterpret_cast<uint8_t*>(m_mapped_header) +
                                           m_mapped_header->first_counter_offset);
    } else {
        m_current_counter = NULL;
    }
    return true;
}

bool CountersIterator::Deinit() {
    if (m_mapped_header != NULL) {
        if (munmap(m_mapped_header, kMappedFileSize) != 0) {
            DSTREAM_ERROR("munmap failed");
            return false;
        }
    }
    m_mapped_header = NULL;
    m_current_counter = NULL;
    return true;
}

MappedCounter* CountersIterator::Next() {
    // find the first counter that is in use
    while (m_current_counter != NULL && m_current_counter->ref_count == 0) {
        if (m_current_counter->next_counter_offset != 0) {
            m_current_counter = reinterpret_cast<MappedCounter*>(
                    reinterpret_cast<uint8_t*>(m_mapped_header)
                    + m_current_counter->next_counter_offset);
        } else {
            m_current_counter = NULL;
        }
    }
    MappedCounter* ret_counter = m_current_counter;
    if (m_current_counter != NULL && m_current_counter->next_counter_offset != 0) {
        m_current_counter = reinterpret_cast<MappedCounter*>(
                reinterpret_cast<uint8_t*>(m_mapped_header)
                + m_current_counter->next_counter_offset);
    } else {
        m_current_counter = NULL;
    }
    return ret_counter;
}

void CountersIterator::Reset() {
    if (m_mapped_header != NULL && m_mapped_header->first_counter_offset != 0) {
        m_current_counter = reinterpret_cast<MappedCounter*>(
                reinterpret_cast<uint8_t*>(m_mapped_header)
                + m_mapped_header->first_counter_offset);
    } else {
        m_current_counter = NULL;
    }
}
