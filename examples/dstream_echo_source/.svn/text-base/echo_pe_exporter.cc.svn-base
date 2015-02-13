/** 
* @brief    example for exporter
*/
#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "processelement/task.h"

#include "echo.pb.h"

class EchoExporter : public dstream::processelement::Task {
public:
    // types we may encounter.
    static const char* in_tag;
    typedef dstream::processelement::ProtoTuple< EchoRequest > TaskTuple;
    typedef dstream::processelement::DefaultTupleTypeInfo< TaskTuple > TaskTupleInfo;

    EchoExporter() {
        registerTupleTypeInfo(EchoExporter::in_tag, new TaskTupleInfo());
    }

    virtual void OnTuples(const dstream::processelement::Tuples& tuples) {
        //printf("Echo receive %lu tuples", tuples.size());
        // export echo data
        for(size_t i = 0; i < tuples.size(); i++) {
            const TaskTuple* in_tuple = dynamic_cast<const TaskTuple*>(tuples[i]);
            std::string line(in_tuple->field());

            double last = in_tuple->timestamp();
            double now  = dstream::GetTimeUS();
            printf("Echo data : %s: timestamp=%f, time_lag=%f\n",
                    line.c_str(), now, now - last);
        }
    }
}; // class EchoExporter

const char* EchoExporter::in_tag = "EchoTask";

int main(int argc, char* argv[]) {
    DSTREAM_UNUSED_VAL(argc); 
    DSTREAM_UNUSED_VAL(argv); 

    setenv(dstream::processelement::Task::kEnvKeyLogLevel, "WARNING", 1);
    
    // make a task instance and run.
    EchoExporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback=true;
    }

    task.Run(loopback);
}
