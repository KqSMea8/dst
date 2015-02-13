#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

#include "common/logger.h"
#include "processelement/task.h"
#include "processelement/context.h"

#include "echo.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;
using namespace dstream::common;

std::string excutive_name;

class EchoImporter : public Task
{
public:
    // types we may encounter.
    static const char* out_tag;

    typedef ProtoTuple< EchoRequest > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    EchoImporter(): round(0)
    {
        registerTupleTypeInfo(EchoImporter::out_tag, new ImporterTupleInfo());
    }

    ~EchoImporter()
    {
    }

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes)
    {
        uint64_t hash_code;
        ByteSize size = 0;

        string line(reinterpret_cast<const char*>(bytes->data(&size)), size);
        hash_code = round++;


        // emit tuple
        ImporterTuple* in_tuple  = dynamic_cast<ImporterTuple*>(emit(out_tag));
        in_tuple->set_field(line);
        in_tuple->set_hash_code(hash_code);
        in_tuple->set_time(get_time());

        struct timespec sleeptime;
        sleeptime.tv_sec = 0;
        sleeptime.tv_nsec = 1e4;
        //nanosleep(&sleeptime, NULL); // sleep for 10us
    }

    uint64_t get_time()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        uint64_t msec = now.tv_sec * 1000 + now.tv_usec / 1000;
        return msec;
    }

private:
    uint64_t round;
    Uint64NumCounter* latency_;
}; // class EchoImporter

const char* EchoImporter::out_tag = "EchoImporter";

int main(int argc, char** argv)
{
    excutive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "DEBUG", 1);
    //setenv(Task::kEnvKeyProcessElementName, "EchoImporter", 1);
    //setenv(internal::Context::kEnvKeyCommitLogType, "fake", 1);
    //setenv("dstream.pe.commitLog.0.type", "fake", 1);
    //setenv("dstream.importer.spoutlimit", "20000", 1);

    // make a task instance and run.
    EchoImporter task;
    // get env of loopback & importer


    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }

    task.run(loopback);
}
