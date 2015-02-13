/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <string>
#include "echo.pb.h"
#include "common/logger.h"
#include "processelement/task.h"

using namespace dstream;
using namespace dstream::processelement;

class NaiveTask : public Task {
public:
    // types we may encounter.
    static const char* tag;
    typedef ProtoTuple< Message > MessageTuple;
    typedef DefaultTupleTypeInfo< MessageTuple > MessageTupleInfo;
    // ----------------------------------------
    NaiveTask() {
        registerTupleTypeInfo(NaiveTask::tag, new MessageTupleInfo());
    }
    // ----------------------------------------
    // mock pn behaviours.
    virtual void onForwardLog(uint64_t peid, const std::vector< Tuple* >& tuples) {
        for (size_t i = 0; i < tuples.size(); i++) {
            const MessageTuple* tuple = dynamic_cast<const MessageTuple*>(tuples[i]);
            printf("---------->onForwardLog:%s'<----------\n", tuple->data().c_str());
        }
    }
    // ----------------------------------------
    // actually working code which use need to care about.
    // ----------------------------------------
    virtual void onSpout(ReadableByteArray* bytes) {
        ByteSize size = 0;
        const char* data = reinterpret_cast<const char*>(bytes->data(&size));
        printf("---------->onSpout:%s'<----------\n", data);
        MessageTuple* tuple = dynamic_cast<MessageTuple*>(emit(tag));
        tuple->set_data(data);
    }
}; // class NaiveTask

const char* NaiveTask::tag = "naive";

int main() {
    Logger::initialize("worker", Logger::Priority::DEBUG);
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    NaiveTask task;
    task.run(loopback);
    DSTREAM_INFO("exit...");
    return 0;
}
