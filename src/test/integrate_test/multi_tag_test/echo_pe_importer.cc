#include <string>
//#include <com_log.h>
#include "common/logger.h"
#include "processelement/task.h"
#include <map>
#include <iostream>
#include <fstream>
#include <time.h>

#include "../test/integrate_test/echo_test/echo.pb.h"
using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string excutive_name;

char str[1024 * 1024];
void set_timestamp()
{
    char timestamp[16];
    char* temp_str = str;
    struct timeval cur_time;

    gettimeofday(&cur_time, NULL);
    uint32_t micro_sec = (cur_time.tv_sec % (3600 * 24)) * 1000 + cur_time.tv_usec / 1000; // ms
    //int last_timestamp=0;
    for (; ':' != *temp_str;) {
        temp_str++;    /*find the formet end.*/
    }
    //DSTREAM_INFO("  ====> in : %s, ", str);
    sprintf(timestamp, "%u:", micro_sec);
    *temp_str++ = '.';
    size_t i;
    for (i = 0; i < strlen(timestamp); i++) {
        temp_str[i] = timestamp[i];
    }
    temp_str[i + 2] = 0;
    DSTREAM_INFO("  ====> out : %s, onSpout", str);
    temp_str[i + 2] = '#';
}

class WordCount : public Task
{
public:
    // types we may encounter.
    static const char* tag1;
    static const char* tag2;
    typedef ProtoTuple< EchoRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< EchoRequest> OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    map<string, int> wordcounts;

    WordCount()
    {
        registerTupleTypeInfo(WordCount::tag1, new InTupleInfo());
        registerTupleTypeInfo(WordCount::tag2, new InTupleInfo());
    }

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes)
    {
        static int tag_index = 0;
        DSTREAM_INFO("  ====> %s, onSpout", excutive_name.c_str());
        ByteSize size = 0;
        string line(reinterpret_cast<const char*>(bytes->Data(&size)), size);

        //DSTREAM_INFO("  ====> %s, TESTdata : %s", excutive_name.c_str(), line.c_str());

        sprintf(str, "%s", line.c_str());
        set_timestamp();
        size_t hash_code;
        sscanf(str, "%zu.", &hash_code);

        InTuple* in_tuple = 0;
        if (tag_index == 0) {
            in_tuple  = dynamic_cast<InTuple*>(emit(tag1));
        }
        else {
            in_tuple  = dynamic_cast<InTuple*>(emit(tag2));
        }
        tag_index = (tag_index + 1) % 2;

        in_tuple->set_field(str);
        in_tuple->set_hash_code(hash_code);
    }
}; // class WordCount

const char* WordCount::tag1 = "tag1";
const char* WordCount::tag2 = "tag2";

int main(int /*argc*/, char** argv)
{
    excutive_name = argv[0];

    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount_importer", 1);

    // make a task instance and run.
    WordCount task;
    // get env of loopback & importer

    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }
    task.run(loopback);
}
