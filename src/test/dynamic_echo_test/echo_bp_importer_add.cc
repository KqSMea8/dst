/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  QA test for dynamic importer 
 *
 */
#include <time.h>
#include <compack/compack.h>
#include <map>
#include <string>
#include "common/logger.h"
#include "dynamic.pb.h"
#include "processelement/task.h"

#define PACKET_TYPE_NORMAL 0

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

std::string excutive_name;
std::string excutive_type;

class EchoImporter : public Task {
public:
    // types we may encounter.
    static const char* out_tag;
    typedef ProtoTuple< TestDynamicTest > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple > ImporterTupleInfo;

    EchoImporter() {
        registerTupleTypeInfo(EchoImporter::out_tag, new ImporterTupleInfo());
    }

    // importer logic
    virtual void onSpout(ReadableByteArray* bytes) {
        uint64_t hash_code;
        struct timeval tv;
        ByteSize size = 0;
        string line;

        if (excutive_type == "logagent") {
            try {
                ByteSize size = 0;
                char* packet_body = (char*)(bytes->data(&size));
                compack::Buffer wrap(packet_body, size);
                compack::buffer::Reader reader(wrap);
                compack::ObjectIterator element;

                bool is_found = reader.find("type", element);
                if (is_found) {
                    int packet_type = element.getAsNum<int>();
                    if (packet_type == PACKET_TYPE_NORMAL) {
                        if (is_found = reader.find("body", element)) {
                            packet_body = (char*)(element.getAsString());
                            line.assign(packet_body);
                            // DSTREAM_INFO("  ====>TESTdata : %s", packet_body);
                        } else {
                            DSTREAM_WARN("message body not found");
                            return;
                        }
                    } else {
                        DSTREAM_INFO("EOF");
                        return;
                    }
                } else {
                    DSTREAM_WARN("type not find");
                    return;
                }
            } catch (const bsl::Exception& ex) {
                DSTREAM_ERROR("src_adaptor/unpack: unknown bsl exception.[exception:%s]"
                    , ex.what());
                return;
            }
        } else {
            line.assign(reinterpret_cast<const char*>(bytes->data(&size)), size);
        }
        // TODO(liyuanjian): some process to 'line'
        printf("Echo data : %s", line.c_str());

        // set hash code with time
        gettimeofday(&tv, NULL);
        hash_code = tv.tv_usec;

        // emit tuple
        ImporterTuple* in_tuple  = static_cast<ImporterTuple*>(emit(out_tag));
        in_tuple->set_name(line);
        in_tuple->set_hash_code(hash_code);
    }
}; // class EchoImporter

const char* EchoImporter::out_tag = "dynamic.TestDynamicTest";

int main(int argc, char** argv) {
    excutive_name = argv[0];
    excutive_type = argv[1];

    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "EchoImporter", 1);

    // make a task instance and run.
    EchoImporter task;
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.run(loopback);
}
