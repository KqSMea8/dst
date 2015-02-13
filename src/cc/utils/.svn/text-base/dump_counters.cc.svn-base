/** 
* @file     dump_counters.cc
* @brief    
* @author   konghui, konghui@baidu.com
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <time.h>
#include <unistd.h>
#include "common/counters.h"
#include "common/logger.h"

void Usage() {
    printf("Usage: dump_countets counter_file [-o out_file]\n");
}

bool ParseArgs(int argc, char** argv, char*& counter_file, char*& out_file) {
    const char* optstring = "o:";
    while (true) {
        int c = getopt(argc, argv, optstring);
        if (c == -1) {
            break;
        }
        switch (c) {
        case 'o':
            out_file = optarg;
            break;
        default:
            printf("there is invalid option\n");
            break;
        }
    }
    if (optind < argc) {
        counter_file = argv[optind];
    } else {
        Usage();
        return false;
    }
    return true;
}

bool DumpCounters(const char* counter_file, std::ostream& os) { // NOLINT
    dstream::common::CountersIterator counters_itr;
    if (!counters_itr.Init(counter_file)) {
        printf("Failed to init counters iterator from %s\n", counter_file);
        return false;
    }
    // write output header
    os << "#time,name,type,attr1,attr2,attr3,value\n";
    while (true) {
        dstream::common::MappedCounter* counter = counters_itr.Next();
        if (counter == NULL) {
            break;
        }
        time_t cur_time = time(NULL);
        const char* counter_name = counter->GetCounterName();
        dstream::common::CounterType counter_type = counter->counter_type;
        uint64_t value;
        switch (counter_type) {
        case dstream::common::kCounterTypeRate:
            value = counter->GetRateAndReset();
            break;
        case dstream::common::kCounterTypeNum:
            counter->GetStatisticsAndReset(&value);
            break;
        default:
            printf("Invalid counter type: %d", counter->counter_type);
            return false;
        }
        os << cur_time << ","
           << counter_name << ","
           << counter_type << ","
           << counter->attr1 << ","
           << counter->attr2 << ","
           << counter->attr3 << ","
           << value << "\n";
    }
    return true;
}

int main(int argc, char** argv) {
    dstream::logger::initialize("dump_counters");

    char* counter_file = NULL;
    char* out_file = NULL;
    if (!ParseArgs(argc, argv, counter_file, out_file)) {
        return -1;
    }

    std::ostream* os = &std::cout;
    std::ofstream out_file_stream;
    if (out_file != NULL) {
        out_file_stream.open(out_file);
        if (out_file_stream.fail()) {
            printf("Failed to open %s for output\n", out_file);
            return -1;
        }
        os = &out_file_stream;
    }
    if (!DumpCounters(counter_file, *os)) {
        return -1;
    }

    return 0;
}
