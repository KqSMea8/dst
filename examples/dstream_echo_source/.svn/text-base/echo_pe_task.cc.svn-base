/** 
* @brief    example for task
*/
#include <iostream>
#include <fstream>
#include <map>
#include <string>

#include "processelement/task.h"

#include "echo.pb.h"

class EchoTask : public dstream::processelement::Task {
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef dstream::processelement::ProtoTuple< EchoRequest >             ImporterTuple;
    typedef dstream::processelement::DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;
    typedef dstream::processelement::ProtoTuple< EchoRequest >             TaskTuple;
    typedef dstream::processelement::DefaultTupleTypeInfo< TaskTuple >     TaskTupleInfo;

    EchoTask() {
        registerTupleTypeInfo(EchoTask::in_tag, new ImporterTupleInfo());
        registerTupleTypeInfo(EchoTask::out_tag, new TaskTupleInfo());
    }

    virtual void OnTuples(const dstream::processelement::Tuples& tuples) {
        uint64_t hash_code;
        DSTREAM_INFO("Echo receive %lu tuples", tuples.size());

        //count the number of words
        for(size_t i = 0; i < tuples.size(); i++) {
	        string line;
            double last = 0;
            double now  = dstream::GetTimeUS();
            if(tuples[i]->dynamic_type()) {
              DSTREAM_INFO("dynamic proto type");
              const dstream::processelement::DynamicProtoTuple* in_tuple =
                        dynamic_cast<const dstream::processelement::DynamicProtoTuple*>(tuples[i]);
              const google::protobuf::Message* msg = in_tuple->ProtoMessage();
              std::string conv;
              msg->SerializeToString(&conv);
              EchoRequest echo_req;
              echo_req.ParseFromString(conv);
              line = echo_req.field();
              printf("Echo data : %s\n", line.c_str());
            } else {
              DSTREAM_INFO("static type");
              const ImporterTuple* in_tuple = dynamic_cast<const ImporterTuple*>(tuples[i]);
              line = in_tuple->field();
              last = in_tuple->timestamp();
              printf("Echo data : %s: timestamp=%f, time_lag=%f\n",
                      line.c_str(), now, now - last);
            }

            // set hash code with time
            hash_code = static_cast<uint64>(now);

            // emit tuple
            TaskTuple* out_tuple = dynamic_cast<TaskTuple*>(emit(out_tag));
            out_tuple->set_field(line);
            out_tuple->set_hash_code(hash_code);
            out_tuple->set_timestamp(now);
        }
    }
}; // class EchoTask

const char* EchoTask::in_tag = "EchoImporter";
const char* EchoTask::out_tag = "EchoTask";

int main(int argc, char *argv[]) {
    DSTREAM_UNUSED_VAL(argc); 
    DSTREAM_UNUSED_VAL(argv); 

    setenv(dstream::processelement::Task::kEnvKeyLogLevel, "WARNING", 1);

    // make a task instance and run.
    EchoTask task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.Run(loopback);
}
