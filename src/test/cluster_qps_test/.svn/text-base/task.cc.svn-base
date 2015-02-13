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

    EchoTask(): round(0), latency_counter_(NULL)
    {
        registerTupleTypeInfo(EchoTask::in_tag, new ImporterTupleInfo());
        registerTupleTypeInfo(EchoTask::out_tag, new TaskTupleInfo());
    }

    virtual void init()
    {
        uint64_t pe_id = id();
        uint64_t app_id = pe_id >> 40;
        uint64_t processor_id = pe_id >> 32;
        latency_counter_ = Counters::CreateUint64NumCounter("EchoPe", "ProcessorLatency", pe_id, app_id,
                           processor_id);
    }

    ~EchoTask()
    {
        delete latency_counter_;
    }

    virtual void onTuples(const Tuples& tuples)
    {
        uint64_t hash_code;

        //count the number of words
        for (size_t i = 0; i < tuples.size(); i++)
        {
            const ImporterTuple* in_tuple = dynamic_cast<const ImporterTuple*>(tuples[i]);
            string line(in_tuple->field());

            // update counter
            update_counter(in_tuple->time());

            // set hash code
            hash_code = round++;

            // emit tuple
            TaskTuple* out_tuple = dynamic_cast<TaskTuple*>(emit(out_tag));
            out_tuple->set_field(line);
            out_tuple->set_hash_code(hash_code);
            out_tuple->set_time(get_time());
        }
    }

    void update_counter(uint64_t upper_time)
    {

        struct timeval now;
        gettimeofday(&now, NULL);
        uint64_t cur_time = now.tv_sec * 1000 + now.tv_usec / 1000;

        //DSTREAM_WARN("upper time: %llu, cur_time: %llu", (long long)upper_time, (long long)cur_time);
        if (upper_time && cur_time && upper_time <= cur_time) {
            latency_counter_->Set(cur_time - upper_time);
        }
    }

    uint64_t get_time()
    {
        struct timeval now;
        gettimeofday(&now, NULL);
        uint64_t msec = now.tv_sec * 1000 + now.tv_usec / 1000;
        return msec;
    }

private:
    int round;
    common::Uint64NumCounter* latency_counter_;
}; // class EchoTask

const char* EchoTask::in_tag = "EchoImporter";
const char* EchoTask::out_tag = "EchoTask";

int main(int argc, char** argv)
{
    excutive_name = argv[0];

    setenv(Task::kEnvKeyLogLevel, "INFO", 1);
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
