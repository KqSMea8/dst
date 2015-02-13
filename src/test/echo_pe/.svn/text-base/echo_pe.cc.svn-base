/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief  QA test for DStream
 *
 */
#include "echo.pb.h"
#include <assert.h>
#include <time.h>
#include <compack/compack.h>
#include <map>
#include <string>
#include "common/logger.h"
#include "processelement/task.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

#define PACKET_TYPE_EOF 1
#define PACKET_TYPE_NORMAL 0

void usage() {
    printf("usage:\n");
    printf("    -s --src=local_file|logagent|fake # default is local_file\n");
    printf("    -t --tags=n # out_tags = [tag1 ~ tagn], default is 1\n");
    printf("    -l --loop=n # cpu, defaut is 1, if set -1, infinite\n");
    printf("    -d --delay=n # emit delay ms, default is 0, if set to -1, infinite\n");
    printf("    -m --memory=n # MB, defaut is 0, if set -1, infinite\n");
    printf("    -p --speed=n # tuple/s\n");
    printf("    \n\n");
}

class Option {
public:
    enum {
        Logagent = 0,
        Localfile = 1,
        Fake = 2,
    };
    int tags;
    int loop;
    int delay;
    int memory;
    int importer_type;
    int _batch_size;
    char* speed;
    Option(): tags(1), loop(1), delay(0), memory(0), importer_type(Option::Localfile), _batch_size(1),
        speed(NULL) {}
} g_options;

class EchoTask : public Task {
public:
    //  types we may encounter.
    vector<std::string> tags;
    char line_str_[1024 * 1024];
    char* cost_memery ;
    FILE* fp ;
    int batch_size;

#define BENCH_MARK
#ifdef BENCH_MARK
    // handle format: n.timestamp.timestamp:#########
    void set_timestamp()
    {
        char timestamp[16];
        char* temp_str = line_str_;
        struct timeval cur_time;
        gettimeofday(&cur_time, NULL);
        sprintf(timestamp, "%u:", (uint32_t)((cur_time.tv_sec % (3600 * 24)) * 1000 + cur_time.tv_usec /
                                             1000));
        for (; ':' != *temp_str;) {
            temp_str++;    /*find the formet end ":".*/
        }
        *temp_str++ = '.';
        size_t i;
        for (i = 0; i < strlen(timestamp); i++) {
            temp_str[i] = timestamp[i];
        }
        temp_str[i] = 0;
        //DSTREAM_INFO("  ====> out : %s, onSpout", line_str_);
        //fprintf(fp,"%s\n",line_str_); //fflush(fp);
        temp_str[i] = ':';
    }
#endif

    typedef ProtoTuple<EchoRequest> EchoTuple;
    typedef DefaultTupleTypeInfo<EchoTuple> EchoTupleInfo;

    EchoTask(): fp(NULL), cost_memery(NULL), batch_size(1) {
        for (int i = 1; i <= g_options.tags; i++) {
            char tag_[16];
            sprintf(tag_, "tag%d", i);
            tags.push_back(tag_);
            printf(" ===> set out tag (%s)\n", tag_);
        }
        for (int i = 1; i <= 1000; i++) {
            char tag_[16];
            sprintf(tag_, "tag%d", i);
            registerTupleTypeInfo(tag_, new EchoTupleInfo());
        }
        fp = fopen("datalog", "w+");
        if (g_options.memory != 0) {
            cost_memery = new char(g_options.memory * 1024 * 1024);
        }
    }

    ~EchoTask() {
        if (cost_memery != NULL)
        {
            delete cost_memery;
            cost_memery = NULL;
        }
        fclose(fp);
    }
    /*
        void init(){
            std::string value;
            Context::get("pipe-name", value);
            DSTREAM_INFO("init pipe-name %s", value.c_str());
            Context::get("pipelet-id", value);
            DSTREAM_INFO("init pipelet-id %s", value.c_str());
            Context::get("message-id", value);
            DSTREAM_INFO("init  message-id %s", value.c_str());
            Context::get("sequence-id", value);
            DSTREAM_INFO("init sequece-id %s", value.c_str());
            Context::get("receive-time", value);
            DSTREAM_INFO("init receive-time %s", value.c_str());
            char *pipe_name = getenv("dstream.pe.commitLog.0.bigpipe.name");
            DSTREAM_INFO("init dstream.pe.commitLog.0.bigpipe.name %s", value.c_str());
            char *pipelet_id = getenv("dstream.pe.commitLog.0.bigpipe.pipeletId");
            DSTREAM_INFO("init dstream.pe.commitLog.0.bigpipe.pipeletId %s", value.c_str());
        }
    */
    // importer logic
    virtual void onSpout(ReadableByteArray* bytes) {
        /*  use for test write sub_point on zk
        std::string pipelet_id;
        Context::get("pipelet-id", pipelet_id);
        DSTREAM_INFO("onSpout pipelet-id %s", pipelet_id.c_str());
        std::string message_id;
        Context::get("message-id", message_id);
        DSTREAM_INFO("onSpout message-id %s", message_id.c_str());
            std::string sequence_id;
        Context::get("sequence-id", sequence_id);
        DSTREAM_INFO("onSpout sequece-id %s", sequence_id.c_str());

        SubPoint subpoint;
        subpoint.set_pipelet((int64_t)atoi(pipelet_id.c_str()));
        subpoint.set_pipelet((int64_t)atoi(message_id.c_str()));
        subpoint.set_pipelet((int64_t)atoi(sequence_id.c_str()));

        std::vector<uint32_t> pipelet_list(1,(uint32_t)atoi(pipelet_id.c_str()));
        sub_progress_.update(subpoint);
            sub_progress_.save();
            sub_progress_.load(pipelet_list);
        std::vector<SubPoint> sub_point_list;
        sub_progress_.read(&sub_point_list);
        SubPoint &read_subpoint = sub_point_list[0];
        DSTREAM_INFO("onSpout messge_id %ld, sequence_id %ld ",read_subpoint.msg_id(), read_subpoint.seq_id());
        */
        static int tag_index = 0;
        string line;
        ByteSize size = 0;

        // now we can automaticly determine if it is loagent data, unpack if true
        if (g_options.importer_type == Option::Logagent) {
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
                            packet_body = (char*)element.getAsString();
                            line.assign(packet_body);
                            //DSTREAM_INFO("  ====>TESTdata : %s", packet_body);
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
            } catch (bsl::Exception& ex) {
                DSTREAM_ERROR("src_adaptor/unpack: unknown bsl exception.[exception:%s]", ex.what());
                return;
            }
        } else {
            line.assign(reinterpret_cast<const char*>(bytes->data(&size)), size);
        }

        //DSTREAM_INFO("  ====>TESTdata : %s", line.c_str());
        for (int r1 = 0; r1 < g_options.loop; r1++) {
            sprintf(line_str_, "%s", line.c_str());
        }
#ifdef BENCH_MARK
        set_timestamp();
#endif
        static size_t hash_code = 0;
        EchoTuple* out_tuple = 0;
        batch_size = g_options._batch_size;
        for (int i = 0; i < tags.size(); i++) {
            for (int cycle = 0; cycle <  batch_size; cycle ++)
            {
                //DSTREAM_INFO("emit tuple, tag(%s)", tags[i].c_str());
                EchoTuple* out_tuple = dynamic_cast<EchoTuple*>(emit(tags[i]));
                out_tuple->set_field(line_str_);
                out_tuple->set_hash_code(hash_code);
            }
        }
        hash_code++;
        if (-1 == g_options.delay) {
            sleep(1000000);
        }
        else if (g_options.delay > 0) {
            usleep(1000 * g_options.delay);
        }
    }

    // normal pe/exporter logic
    virtual void onTuples(const Tuples& tuples) {
        static int tag_index = 0;
        //count the number of words
        for (size_t i = 0; i < tuples.size(); i++) {
            const EchoTuple* in_tuple = dynamic_cast<const EchoTuple*>(tuples[i]);
            for (int r1 = 0; r1 < g_options.loop; r1++) {
                sprintf(line_str_, "%s", in_tuple->field().c_str());
            }
#ifdef BENCH_MARK
            set_timestamp();
#endif
            static size_t hash_code = 0;
            for (int i = 0; i < tags.size(); i++) {
                //DSTREAM_INFO("emit tuple, tag(%s)", tags[i].c_str());
                EchoTuple* out_tuple = dynamic_cast<EchoTuple*>(emit(tags[i]));
                out_tuple->set_field(line_str_);
                out_tuple->set_hash_code(hash_code);
            }
            hash_code++;
        }
        if (-1 == g_options.delay) {
            sleep(1000000);
        }
        else if (g_options.delay > 0) {
            usleep(1000 * g_options.delay);
        }
    }
};

int main(int argc, char** argv)
{

    int c;
    while ((c = getopt(argc, argv, "s:t:l:d:m:b:p:vh")) > 0) {
        printf(" - %c\n", (char) c);
        switch (c)
        {
        case 's':
            if (strcmp(optarg, "local_file") == 0) {
                g_options.importer_type = Option::Localfile;
                printf("Option::Localfile\n");
            } else if (strcmp(optarg, "fake") == 0) {
                g_options.importer_type = Option::Fake;
                printf("Option::Fake\n");
            } else if (strcmp(optarg, "logagent") == 0) {
                g_options.importer_type = Option::Logagent;
                printf("Option::Logagent\n");
            } else {
                printf("type error!!\n");
            }
            break;
        case 't':
            g_options.tags = atoi(optarg);
            break;
        case 'l':
            g_options.loop = atoi(optarg);
            break;
        case 'd':
            g_options.delay = atoi(optarg);
            break;
        case 'm':
            g_options.memory = atoi(optarg);
            break;
        case 'b':
            g_options._batch_size =  atoi(optarg);
            break;
        case 'p':
            g_options.speed = new char(sizeof(optarg));
            strcpy(g_options.speed, optarg);
            break;
        case 'v':
            printf("version: 1.0.0 \nonly for dstream test\n");
            break;
        case 'h':
        default:
            usage();
            break;
        }
    }
    if (g_options.speed != NULL)
    {
        setenv("dstream.importer.spoutlimit", g_options.speed, 1);
    }
    if (g_options.importer_type == Option::Fake)
    {
        setenv(internal::Context::kEnvKeyCommitLogType, "fake", 1);
        setenv("dstream.pe.commitLog.0.type", "fake", 1);
    }
    //  make a task instance and run.
    EchoTask task;
    task.run(false);
}
