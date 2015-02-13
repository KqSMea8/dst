#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "common/logger.h"
#include "common/counters.h"
#include "processelement/task.h"

#include "echo.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string excutive_name;

class EchoTask : public Task
{
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< EchoRequest > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;
    typedef ProtoTuple< EchoRequest > TaskTuple;
    typedef DefaultTupleTypeInfo< TaskTuple > TaskTupleInfo;

    EchoTask(): hash_(0) {
        registerTupleTypeInfo(EchoTask::in_tag, new ImporterTupleInfo());
        registerTupleTypeInfo(EchoTask::out_tag, new TaskTupleInfo());


    }

    ~EchoTask() {
        delete latency_counter_;
    }

    virtual void init() {

        uint64_t pe_id = id();
        uint64_t app_id = pe_id >> 40;
        uint64_t processor_id = pe_id >> 32;
        latency_counter_ = common::Counters::CreateUint64NumCounter("EchoPe", "ProcessorLatency", pe_id,
                           app_id, processor_id);

    }

    virtual void onTuples(const Tuples& tuples)
    {
        printf("Echo receive %lu tuples", tuples.size());

        //count the number of words
        for (size_t i = 0; i < tuples.size(); i++)
        {
            string line;
            double last = 0;
            double now  = GetTimeUS();
            if (tuples[i]->dynamic_type()) {
                DSTREAM_INFO("dynamic proto type");
                const DynamicProtoTuple* in_tuple = dynamic_cast<const DynamicProtoTuple*>(tuples[i]);
                const google::protobuf::Message* msg = in_tuple->ProtoMessage();
                std::string conv;
                msg->SerializeToString(&conv);
                EchoRequest echo_req;
                echo_req.ParseFromString(conv);
                line = echo_req.field();
                printf("Echo data : %s\n", line.c_str());
            } else {
                //DSTREAM_INFO("static type");
                const ImporterTuple* in_tuple = dynamic_cast<const ImporterTuple*>(tuples[i]);
                line = in_tuple->field();
                last = in_tuple->timestamp();
                uint64_t lag = now > last ? (uint64_t)(now - last) : 0UL;
                latency_counter_->Set(lag);
                //DSTREAM_INFO("Echo Data : %s: timestamp=%f, time_lag=%f\n",
                //              line.c_str(), now, now - last);
            }

            // emit tuple
            TaskTuple* out_tuple = dynamic_cast<TaskTuple*>(emit(out_tag));
            out_tuple->set_field(line);
            out_tuple->set_hash_code(hash_++);
            out_tuple->set_timestamp(now);
        }
    }
private:
    uint64_t hash_;
    common::Uint64NumCounter* latency_counter_;
}; // class EchoTask

const char* EchoTask::in_tag = "EchoImporter";
const char* EchoTask::out_tag = "EchoTask";

int main(int argc, char** argv)
{
    excutive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "EchoTask", 1);

    // make a task instance and run.
    EchoTask task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }
    task.run(loopback);
}
