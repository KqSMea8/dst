#include <string>
//#include <com_log.h>
#include "common/logger.h"
#include "processelement/task.h"
#include <map>
#include <iostream>
#include <fstream>

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
    size_t i = 0;
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
    typedef ProtoTuple< EchoRequest > OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    WordCount()
    {
        registerTupleTypeInfo(WordCount::tag1, new InTupleInfo());
        registerTupleTypeInfo(WordCount::tag2, new InTupleInfo());
        registerTupleTypeInfo(WordCount::tag1, new OutTupleInfo());
        registerTupleTypeInfo(WordCount::tag2, new OutTupleInfo());
    }

    virtual void onTuples(const Tuples& tuples)
    {
        static int tag_index = 0;
        DSTREAM_INFO(" ====> %s, [TEST]onTuples, receive %lu tuples", excutive_name.c_str(), tuples.size());

        //count the number of words
        for (size_t i = 0; i < tuples.size(); i++)
        {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
            sprintf(str, "%s", in_tuple->field().c_str());
            set_timestamp();
            size_t hash_code;
            sscanf(str, "%zu.", &hash_code);

            OutTuple* out_tuple;
            if (tag_index == 0) {
                out_tuple = dynamic_cast<OutTuple*>(emit(tag1));
            }
            else {
                out_tuple = dynamic_cast<OutTuple*>(emit(tag2));
            }
            tag_index = (tag_index + 1) % 2;

            out_tuple->set_field(str);
            out_tuple->set_hash_code(hash_code);
        }
    }
}; // class WordCount

const char* WordCount::tag1 = "tag1";
const char* WordCount::tag2 = "tag2";

int main(int /*argc*/, char** argv)
{
    excutive_name = argv[0];

    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount", 1);

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
