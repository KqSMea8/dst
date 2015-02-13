/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  QA test for joiner pe 
 *
 */
#include <processelement/context.h>
#include <processelement/task.h>
#include <sys/time.h>
#include <string>
#include "joiner.pb.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;


void usage() {
    printf("usage:\n");
    printf("    -t --timestamp=n # set the importer start timestamp\n");
    printf("    -j --joinkey=n # set the importer start join key\n");
    printf("    -d --delay time=n # set the importer start delay time(s)\n");
    printf("    -o --out_tag=xxx # set the importer out_tag,default is importer\n");
    printf("    -s --speed=n # set the importer speed limit per second\n");
    printf("    -r --run_time=n # set the importer run time(s)\n");
    printf("    -h #show this help\n");
}
class Option {
    /*
        ³õÊ¼²ÎÊý£º
            1.start timestamp: ¿ÉÉèÖÃµÄÆðÊ¼Ê±¼ä´Á,Ä¬ÈÏÎª0
            2.start join_key:  ¿ÉÉèÖÃµÄÆäÊµjoin key,Ä¬ÈÏÎª0
            3.delay time:      pe Æô¶¯ÑÓ³ÙÊ±¼ä£¬Ä¬ÈÏÎª0
            4.out tag:         Êä³öµÄtag,Ä¬ÈÏÎªimporter
            5.speed limit:     importerµÄËÙ¶ÈÏÞÖÆ£¬Ä¬ÈÏÎª 100tuple/s
        ¹¤×÷Ä£Ê½£º
            1.Ã¿10s jk×öÒ»´Î++
            2.Ã¿1s timestamp×öÒ»´Î++
    */
public:
    uint64_t start_ts;
    uint64_t start_jk;
    int delay;
    char* out_tag;
    char* speed;
    int run_time;
    Option(): start_ts(0), start_jk(0), delay(0), out_tag("importer"), speed("100"), run_time(-1) {}
} g_options;

class JoinerImporter : public Task {
public:
    typedef ProtoTuple< Joiner > ImporterTuple;
    typedef DefaultTupleTypeInfo< ImporterTuple> ImporterTupleInfo;

    JoinerImporter() {
        registerTupleTypeInfo(g_options.out_tag, new ImporterTupleInfo());
        ts_ = g_options.start_ts;
        jk_ = g_options.start_jk;
        begin_ts_ = getTime();
        stop_ts_ = begin_ts_ + static_cast<uint64_t>(g_options.run_time);
        ts_count = 0;
        tuple_count = 0;
    }
    ~JoinerImporter() {}

    virtual void onSpout(ReadableByteArray* bytes) {
        jk_ts_check();
        ByteSize size = 0;
        string tag = g_options.out_tag;

        string line(reinterpret_cast<const char*>(bytes->data(&size)), size);

        ImporterTuple* in_tuple = static_cast<ImporterTuple*>(emit(g_options.out_tag));

        in_tuple->set_info(tag);
        in_tuple->set_jk(jk_);
        in_tuple->set_timestamp(ts_);
    }
    uint64_t getTime() {
        struct timeval now;
        gettimeofday(&now, NULL);
        return now.tv_sec;
    }
    void jk_ts_check() {
        // ¶Ôjk\ts½øÐÐ´¦Àí£¬Ã¿1s ts++¡,Ã¿10s jk++
        uint64_t now_ts_ = getTime();
        if (now_ts_ == stop_ts_) {
            // quit_ = true;
            sleep(3600);
        }
        if ((now_ts_ - begin_ts_) != 0) {
            if ((now_ts_ - begin_ts_) == 1) {
                DSTREAM_INFO("last second ts_ = %lu, ts_count_ = %d, join key = %lu, timestamp = %lu,tuples whole sec = %d",
                             ts_, ts_count, jk_, now_ts_, tuple_count);
                ts_++;
                ts_count++;
                if (ts_count == 10) {
                    jk_++;
                    ts_count = 0;
                    DSTREAM_INFO("10s passed,join key ++");
                }
                begin_ts_ = now_ts_; // record new ts
                tuple_count = 0; // 1 second passed,tuple count clean
                tuple_count++; // this is also a tuple
            } else {
                DSTREAM_ERROR("(now_ts_ - begin_ts_) != 1");
            }
        } else {
            tuple_count++;
        }
    }

protected:
    uint64_t ts_; // remember the start timestamp
    uint64_t jk_; // remember the start join key
    uint64_t begin_ts_;
    uint64_t stop_ts_; // begin_ts_ + run_time
    int ts_count; // count ts change times
    int tuple_count; // count tuple per second
};

int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "t:j:d:o:s:r:h")) > 0) {
        printf("- %c\n", static_cast<char>(opt));
        switch (opt) {
        case 't':
            g_options.start_ts = atoi(optarg);
            break;
        case 'j':
            g_options.start_jk = atoi(optarg);
            break;
        case 'd':
            g_options.delay = atoi(optarg);
            break;
        case 'o':
            g_options.out_tag = optarg;
            break;
        case 's':
            g_options.speed = optarg;
            break;
        case 'r':
            g_options.run_time = atoi(optarg);
            break;
        case 'h':
        default:
            usage();
            break;
        }
    }
    // setenv for fake mode,speedlimit&pe name
    setenv("dstream.importer.spoutlimit", g_options.speed, 1);
    setenv(Task::kEnvKeyProcessElementName, "JoinerImporter", 1);
    setenv(internal::Context::kEnvKeyCommitLogType, "fake", 1);
    setenv("dstream.pe.commitLog.0.type", "fake", 1);

    //  make a task instance and run.
    JoinerImporter task;
    task.run(false);
}


