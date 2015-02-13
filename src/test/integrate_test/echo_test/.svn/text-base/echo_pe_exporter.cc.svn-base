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

std::string executive_name;

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
    temp_str[i] = 0;
    DSTREAM_INFO("  ====> out : %s, onSpout", str);
}

FILE* fp;
char out_file[256] = "echo_result.txt";

class WordCount : public Task
{
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< EchoRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< EchoRequest > OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    WordCount()
    {
        registerTupleTypeInfo(WordCount::in_tag, new InTupleInfo());
        registerTupleTypeInfo(WordCount::out_tag, new OutTupleInfo());
    }

    virtual void onTuples(const Tuples& tuples)
    {
        //DSTREAM_WARN("\n[TEST]onTuples");
        //DSTREAM_INFO(" ====> %s, onTuples", executive_name.c_str());

        //count the number of words
        for (size_t i = 0; i < tuples.size(); i++)
        {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
            sprintf(str, "%s", in_tuple->field().c_str());
            set_timestamp();
            fprintf(fp, "%s\n", str);
            fflush(fp);
        }
    }

}; // class WordCount

const char* WordCount::in_tag = "in";
const char* WordCount::out_tag = "out";

int main(int argc, char** argv)
{
    executive_name = argv[0];
    if (argc == 2) {
        sprintf(out_file, "..//%s", argv[1]);
    }

    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount_exporter", 1);

    // make a task instance and run.
    WordCount task;
    // get env of loopback & importer

    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }
    fp = fopen("echo_result.txt", "w+");
    task.run(loopback);
    fclose(fp);
}
