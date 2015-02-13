#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>

#include "com_log.h"
#include "common/utils.h"
#include "common/logger.h"
#include "processelement/task.h"

#include "echo.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string excutive_name;

class EchoImporter : public Task
{
public:
    // types we may encounter.
    static const char* out_tag;
    typedef ProtoTuple< EchoRequest > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    EchoImporter(): hash_(0UL) {
        registerTupleTypeInfo(EchoImporter::out_tag, new ImporterTupleInfo());
    }

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes) {
        ByteSize size = 0;

        //usleep(100);//sleep us

        string line(reinterpret_cast<const char*>(bytes->data(&size)), size);

        double now  = GetTimeUS();
        //DSTREAM_INFO("Echo Data : %s: timestamp=%f, time_lag=0\n",
        //              line.c_str(), now);

        // emit tuple
        ImporterTuple* in_tuple  = dynamic_cast<ImporterTuple*>(emit(out_tag));
        in_tuple->set_field(line);
        in_tuple->set_hash_code(hash_++);
        in_tuple->set_timestamp(now);
    }

private:
    uint64_t hash_;
}; // class EchoImporter

const char* EchoImporter::out_tag = "EchoImporter";

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    //dstream::transporter::g_kKylinExecWorkers = 2;
    //dstream::transporter::g_kKylinNetWorkers  = 2;
    //dstream::transporter::g_kKylinDiskWorkers = 2;
    //assert(dstream::transporter::g_esp.Init(dstream::transporter::g_kKylinExecWorkers,
    //                                        dstream::transporter::g_kKylinNetWorkers,
    //                                        dstream::transporter::g_kKylinDiskWorkers,
    //                                        NULL,
    //                                        dstream::transporter::g_precision_ms));
    //dstream::transporter::g_init = true;

    excutive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "echo_importer", 1);
    setenv(internal::Context::kEnvKeyCommitLogType, "fake", 1);
    setenv("dstream.pe.commitLog.0.type", "fake", 1);
    setenv(internal::Context::kEnvKeyCommitLogFakeLogLen, "1024", 1);
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
