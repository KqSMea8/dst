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
#include <time.h>
#include <map>
#include <string>
#include "common/logger.h"
#include "processelement/task.h"
#include "dynamic.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string excutive_name;

class EchoImporter : public Task {
public:
    // types we may encounter.
    static const char* out_tag;
    typedef ProtoTuple< TestDynamicTest > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    EchoImporter() {
        registerTupleTypeInfo(EchoImporter::out_tag, new ImporterTupleInfo());
    }

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes) {
        uint64_t hash_code;
        struct timeval tv;
        ByteSize size = 0;

        string line(reinterpret_cast<const char*>(bytes->data(&size)), size);

        // TODO(liyuanjian): some process to 'line'
        printf("Echo data : %s", line.c_str());

        // set hash code with time
        gettimeofday(&tv, NULL);
        hash_code = tv.tv_usec;

        // emit tuple
        ImporterTuple* in_tuple  = static_cast<ImporterTuple*>(emit(out_tag));
        in_tuple->set_name(line);
        in_tuple->set_hash_code(hash_code);
    }
}; // class EchoImporter

const char* EchoImporter::out_tag = "dynamic.TestDynamicTest";

int main(int argc, char** argv) {
    excutive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "EchoImporter", 1);

    // make a task instance and run.
    EchoImporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.run(loopback);
}
