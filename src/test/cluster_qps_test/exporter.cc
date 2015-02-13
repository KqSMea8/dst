#include <string>
#include <map>
#include <iostream>
#include <fstream>

#include "common/logger.h"
#include "processelement/task.h"

#include "echo.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;
using namespace dstream::common;

std::string executive_name;

class EchoExporter : public Task
{
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* stat_file;
    typedef ProtoTuple< EchoRequest > TaskTuple;
    typedef DefaultTupleTypeInfo< TaskTuple > TaskTupleInfo;

    EchoExporter(): round(0), latency_counter_(NULL)
    {
        registerTupleTypeInfo(EchoExporter::in_tag, new TaskTupleInfo());
    }

    ~EchoExporter()
    {
        delete latency_counter_;
    }
    virtual void init()
    {
        uint64_t pe_id = id();
        uint64_t app_id = pe_id >> 40;
        uint64_t processor_id = pe_id >> 32;
        latency_counter_ = Counters::CreateUint64NumCounter("EchoPe", "ProcessorLatency", pe_id, app_id,
                           processor_id);
    }

    virtual void onTuples(const Tuples& tuples)
    {
        // export echo data
        for (size_t i = 0; i < tuples.size(); i++)
        {
            const TaskTuple* in_tuple = dynamic_cast<const TaskTuple*>(tuples[i]);
            string line(in_tuple->field());
            update_counter(in_tuple->time());
        }
    }

    void update_counter(uint64_t upper_time)
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        uint64_t cur_time = now.tv_sec * 1000 + now.tv_usec / 1000;

        //DSTREAM_WARN("upper time:%llu, cur_time:%llu", (long long)upper_time, (long long)cur_time);
        if (upper_time && cur_time && cur_time >= upper_time) {
            latency_counter_->Set(cur_time - upper_time);
        }
    }
private:
    uint64_t round;
    common::Uint64NumCounter* latency_counter_;

}; // class EchoExporter

const char* EchoExporter::in_tag = "EchoTask";

int main(int argc, char** argv)
{
    executive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "EchoExporter", 1);

    // make a task instance and run.
    EchoExporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0)
    {
        loopback = true;
    }

    task.run(loopback);
}
