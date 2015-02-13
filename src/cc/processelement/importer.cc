/* coding:utf-8
 * Copyright (C) dirlt
 */

#include <string>
#include "common/logger.h"
#include "common/proto/pub_sub.pb.h"
#include "processelement/task.h"

using namespace dstream;
using namespace dstream::processelement;

class ImporterTask:
    public Task {
public:
    typedef ProtoTuple< ImporterTuple > ImporterTupleWrapper;
    typedef DefaultTupleTypeInfo < ImporterTupleWrapper > ImporterTupleInfo;
    ImporterTask() {
        registerTupleTypeInfo(kImporterTupleTag, new ImporterTupleInfo());
    }
    virtual void onSpout(ReadableByteArray* bytes) {
        DSTREAM_DEBUG("onSpout....");
        ByteSize size = 0;
        const dstream::Byte* data = bytes->data(&size);
        ImporterTupleWrapper* tuple = dynamic_cast<ImporterTupleWrapper*>(emit(kImporterTupleTag));
        tuple->set_raw(data, size);
    }
}; // class ImporterTask

int main() {
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    ImporterTask task;
    task.run(loopback);
    return 0;
}
