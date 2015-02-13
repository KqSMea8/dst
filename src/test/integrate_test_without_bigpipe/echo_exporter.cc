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

std::string executive_name;

class EchoExporter : public Task
{
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< EchoRequest > TaskTuple;
    typedef DefaultTupleTypeInfo< TaskTuple > TaskTupleInfo;

    EchoExporter(): hash_(0UL) {
        registerTupleTypeInfo(EchoExporter::in_tag, new TaskTupleInfo());
        registerTupleTypeInfo(EchoExporter::out_tag, new TaskTupleInfo());
    }

    ~EchoExporter() {
        delete latency_counter_;
    }

    virtual void init() {
        uint64_t pe_id = id();
        uint64_t app_id = pe_id >> 40;
        uint64_t processor_id = pe_id >> 32;
        latency_counter_ = common::Counters::CreateUint64NumCounter("EchoPe", "ProcessorLatency", pe_id,
                           app_id, processor_id);
    }

    virtual void onTuples(const Tuples& tuples) {
        printf("Echo receive %lu tuples", tuples.size());
        // export echo data
        for (size_t i = 0; i < tuples.size(); i++) {
            const TaskTuple* in_tuple = dynamic_cast<const TaskTuple*>(tuples[i]);
            string line(in_tuple->field());

            double last = in_tuple->timestamp();
            double now  = GetTimeUS();
            //DSTREAM_INFO("Echo Data : %s: timestamp=%f, time_lag=%f\n",
            //             line.c_str(), now, now - last);
            uint64_t lag = now > last ? (uint64_t)(now - last) : 0UL;
            latency_counter_->Set(lag);

            TaskTuple* out_tuple = dynamic_cast<TaskTuple*>(emit(out_tag));
            out_tuple->set_field(line);
            out_tuple->set_hash_code(hash_++);
            out_tuple->set_timestamp(now);
        }
    }
private:
    uint64_t hash_;
    common::Uint64NumCounter* latency_counter_;
}; // class EchoExporter

const char* EchoExporter::in_tag = "EchoTask";
const char* EchoExporter::out_tag = "EchoTask";

int main(int argc, char** argv)
{
    executive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "EchoExporter", 1);

    // make a task instance and run.
    EchoExporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }

    task.run(loopback);
}
