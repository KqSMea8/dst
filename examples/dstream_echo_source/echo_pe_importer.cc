/** 
* @brief    example for importer
* @date     2013-07-26
*/

#include <fstream>
#include <iostream>
#include <string>
#include <time.h>

#include "com_log.h" // for bigpipe conf
#include "common/utils.h"
#include "processelement/task.h"

#include "echo.pb.h"

class EchoImporter : public dstream::processelement::Task {
public:
    // types we may encounter.
    static const char* out_tag;
    typedef dstream::processelement::ProtoTuple< EchoRequest >             ImporterTuple;
    typedef dstream::processelement::DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    EchoImporter() {
        RegisterTupleTypeInfo(EchoImporter::out_tag, new ImporterTupleInfo());
    }

    // importer logic
    virtual void OnSpout(dstream::ReadableByteArray* bytes) {
        uint64_t hash_code;
        dstream::ByteSize size = 0;

        usleep(1000);//sleep 1000ms, to limit the speed
        
        std::string line(reinterpret_cast<const char*>(bytes->Data(&size)), size);

        double now  = dstream::GetTimeUS();
        printf("Echo data : %s: timestamp=%f, time_lag=0\n",
               line.c_str(), now);

        // set hash code with time
        hash_code = static_cast<uint64>(now);

        // emit tuple
        ImporterTuple* in_tuple  = dynamic_cast<ImporterTuple*>(emit(out_tag));
        in_tuple->set_field(line);
        in_tuple->set_hash_code(hash_code);
        in_tuple->set_timestamp(now);
    }
}; // class EchoImporter
const char* EchoImporter::out_tag = "EchoImporter";

int main(int argc, char *argv[]) {
    DSTREAM_UNUSED_VAL(argc); 
    DSTREAM_UNUSED_VAL(argv); 

    com_loadlog("./conf", "bigpipe.conf");

    setenv(dstream::processelement::Task::kEnvKeyLogLevel, "WARNING", 1);

    // make a task instance and run.
    EchoImporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.Run(loopback);
}
