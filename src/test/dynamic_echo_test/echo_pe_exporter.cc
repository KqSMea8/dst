/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  QA test for dynamic importer 
 *
 */
#include <map>
#include <string>
#include "common/logger.h"
#include "echo.pb.h"
#include "processelement/task.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string executive_name;

char str[1024 * 1024];

void set_timestamp() {
    char timestamp[16];
    char* temp_str = str;
    struct timeval cur_time;

    gettimeofday(&cur_time, NULL);
    uint32_t micro_sec = (cur_time.tv_sec % (3600 * 24)) * 1000 + cur_time.tv_usec / 1000; //  ms
    // int last_timestamp=0;
    for (; ':' != *temp_str;) {
        temp_str++;    /*find the formet end.*/
    }
    // DSTREAM_INFO("  ====> in : %s, ", str);
    snprintf(timestamp, sizeof(timestamp), "%u:", micro_sec);
    *temp_str++ = '.';
    size_t i;
    for (i = 0; i < strlen(timestamp); i++) {
        temp_str[i] = timestamp[i];
    }
    temp_str[i + 2] = 0;
    DSTREAM_INFO("  ====> out : %s, onSpout", str);
}

FILE* fp;
char out_file[256] = "echo_result.txt";

class WordCount : public Task {
public:
    //  types we may encounter.
    static const char* tag1;
    static const char* tag2;

#if 1
    typedef ProtoTuple< EchoRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< EchoRequest > OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;
#endif
#if 0
    typedef ProtoTuple< ::google::protobuf::Message > InTuple;
    typedef DefaultTupleTypeInfo< ::google::protobuf::Message > InTupleInfo;
    typedef ProtoTuple< ::google::protobuf::Message > OutTuple;
    typedef DefaultTupleTypeInfo< ::google::protobuf::Message > OutTupleInfo;
#endif

    WordCount() {
        registerTupleTypeInfo(WordCount::tag1, new InTupleInfo());
        registerTupleTypeInfo(WordCount::tag2, new InTupleInfo());
        // registerTupleTypeInfo(WordCount::tag1, NULL);
        // registerTupleTypeInfo(WordCount::tag2, NULL);
    }

    virtual void onTuples(const Tuples& tuples) {
        // DSTREAM_WARN("\n[TEST]onTuples");
        // DSTREAM_NOTICE(" ====> %s, onTuples", executive_name.c_str());

        // count the number of words
        for (size_t i = 0; i < tuples.size(); i++) {
            string line;
            if (tuples[i]->dynamic_type()) {
                DSTREAM_INFO("dynamic type ");
                const DynamicProtoTuple* in_tuple =
                    static_cast<const DynamicProtoTuple*>(tuples[i]);
                const google::protobuf::Message* msg = in_tuple->ProtoMessage();
                std::string conv;
                msg->SerializeToString(&conv);
                EchoRequest echo_req;
                echo_req.ParseFromString(conv);
                line = echo_req.field();
                snprintf(str, sizeof(str), "%s", line.c_str());
            } else {
                DSTREAM_INFO("static type");
                const InTuple* in_tuple = static_cast<const InTuple*>(tuples[i]);
                in_tuple->DebugString();
                snprintf(str, sizeof(str), "%s", in_tuple->field().c_str());
            }
            set_timestamp();
            fprintf(fp, "%s\n", str);
            fflush(fp);
        }
    }
}; //  class WordCount

const char* WordCount::tag1 = "tag1";
const char* WordCount::tag2 = "tag2";

int main(int argc, char** argv) {
    executive_name = argv[0];
    if (argc == 2) {
        snprintf(out_file, sizeof(out_file), "..// %s", argv[1]);
    }

    Logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "wordcount_exporter", 1);

    //  make a task instance and run.
    WordCount task;
    //  get env of loopback & importer

    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    fp = fopen(out_file, "w+");
    task.run(loopback);
    fclose(fp);
}
