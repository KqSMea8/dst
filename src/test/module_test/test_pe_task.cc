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
//#include <com_log.h>
#include "common/logger.h"
#include "processelement/task.h"
#include "rpc_hello.pb.h"
#include "gtest/gtest.h"
using namespace dstream;
using namespace dstream::processelement;

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}




class EchoTask : public Task {
public:
    // types we may encounter.
    static const char* in_tag;
    static const char* out_tag;
    typedef ProtoTuple< EchoRequest > InTuple;
    typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
    typedef ProtoTuple< EchoResponse> OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    static const int kBatchSize = 10000;
    static const int kDefaultUpPEId = 2011;
    static const int kDefaultUpPipeId = 2011;
    // ----------------------------------------
    EchoTask():
        seq_(0),
        first_fetch_(true),
        first_forward_(true),
        first_upchkp_(true),
        first_dochkp_(true) {
        registerTupleTypeInfo(EchoTask::in_tag, new InTupleInfo());
        registerTupleTypeInfo(EchoTask::out_tag, new OutTupleInfo());
    }
    //   // ----------------------------------------
    //   // specify how to allocate in tuple and out tuple.
    //   // in convinent, user can use google::protobuf easily.
    //   Tuple* allocateInTuple() {
    //     return new InTuple();
    //   }
    //   Tuple* allocateOutTuple() {
    //     return new OutTuple();
    //   }
    // ----------------------------------------
    // mock process node parts...
    // when PE really works, user don't need the following code.
    // ----------------------------------------
    virtual void onFetchLog(const uint64_t peid,
                            bool replay_mode,
                            uint64_t* pipeid,
                            uint64_t* upeid,
                            uint64_t* upipeid,
                            bool* end_replay,
                            std::vector< Tuple*>* tuples) {
        if (first_fetch_) {
            DSTREAM_WARN("\n[TEST]onfetchLog(%zu)...", peid);
            first_fetch_ = false;
        }

        if (replay_mode) { // we don't replay.
            *end_replay = true;
        } else {
            // we don't want whole system work too fast.
            // sleep(1);
            *upeid = kDefaultUpPEId;
            *upipeid = kDefaultUpPipeId;
            *pipeid = seq_;
            for (int i = 0; i < kBatchSize; i++) {
                char s[32];
                snprintf(s, sizeof(s), "tuple#%d", i + seq_);
                InTuple* tuple = dynamic_cast<InTuple*>(allocateTuple(in_tag));
                tuple->set_field(s);
                tuples->push_back(tuple);
            }
        }
    }
    virtual void onForwardLog(const uint64_t peid, const std::vector< Tuple* >& tuples) {
        if (first_forward_) {
            DSTREAM_WARN("\n[TEST]onForwardLog(%zu)...", peid);
            first_forward_ = false;
        }
        // importer and normal has different schema.
        if (!m_as_importer) {
            // check pe result.
            for (int i = 0; i < kBatchSize; i++) {
                char s[32];
                snprintf(s, sizeof(s), "tuple#%d", i + seq_);
                OutTuple* tuple = dynamic_cast<OutTuple*>(tuples[i]);
                assert(tuple->field() == s);
            }
            seq_ += kBatchSize;
        }
    }
    virtual void onUploadCheckpoint(const uint64_t peid, const std::string& chkpdir) {
        if (first_upchkp_) {
            DSTREAM_WARN("\n[TEST]onUploadCheckpoint(%zu,%s)...", peid, chkpdir.c_str());
            first_upchkp_ = false;
        }
        // remove it to startup directory.
        // just navie implementation.
        char command[1024];
        snprintf(command, sizeof(command), "rm -rf %s && mv %s %s",
                 kWorkDirectoryStartup,
                 chkpdir.c_str(),
                 kWorkDirectoryStartup);
        DSTREAM_WARN("\n %s", command);
        system(command);
    }
    virtual bool onDoCheckpoint(const uint64_t peid) {
        if (first_dochkp_) {
            DSTREAM_WARN("\n[TEST]onDoCheckpoint(%zu)...", peid);
            first_dochkp_ = false;
        }
        // always allow them to do so.
        return true;
    }
    virtual void onSubscribe(const uint64_t peid,
                             const std::map< uint64_t, uint64_t >& /*subscribePoint*/) {
        DSTREAM_WARN("\n[TEST]onSubscribe(%zu)...", static_cast<uint64_t>(peid));
        //TEST: NO USE
        //for(std::map< uint64_t, uint64_t >::const_iterator it = subscribePoint.begin();
        //    it != subscribePoint.end(); ++it) {
        //  DSTREAM_WARN("subscribe peid=%llu, pipeid=%llu", it->first, it->second);
        //}
    }
    virtual void onHeartbeat(const uint64_t peid,
                             const std::map< std::string, std::string>& /*metrics*/) {
        DSTREAM_INFO("\n[TEST]onHeartbeat(%zu)...", peid);
        //TEST
        //  for(std::map< std::string, std::string>::const_iterator it = metrics.begin();
        //        it != metrics.end(); ++it) {
        //      DSTREAM_INFO("metric '%s' = '%s'", it->first.c_str(), it->second.c_str());
        //    }
    }
    virtual void onSubscribeImporter(std::map< std::string, uint64_t>* progress) {
        DSTREAM_INFO("\n[TEST]onSubscribeImporter...");
        std::map< std::string, uint64_t>& sub = *progress;
        sub[kDefaultCommitLogLFSName] = 0;
    }
    // ----------------------------------------
    // actually working code which use need to care about.
    // ----------------------------------------
    virtual void onTuples(const Tuples& tuples) {
        DSTREAM_WARN("\n[TEST]onTuples");
        for (size_t i = 0; i < tuples.size(); i++) {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
            OutTuple* out_tuple = dynamic_cast<OutTuple*>(emit(out_tag));
            out_tuple->set_field(in_tuple->field());
        }
        // update stats
        int count;
        count = tuples.size();
        if (count % (kBatchSize * 100) == 0) {
            printf("handle %d tuples\n", count);
        }
    }
    // importer logic
    virtual void onSpout(ReadableByteArray* bytes) {
        DSTREAM_WARN("\n[TEST]onSpout");
        const char* data = reinterpret_cast<const char*>(bytes->Data(NULL));
        DSTREAM_INFO("TESTdata : %s", data);
    }
private:
    int seq_;
    bool first_fetch_;
    bool first_forward_;
    bool first_upchkp_;
    bool first_dochkp_;
}; // class EchoTask

const char* EchoTask::in_tag = "echo.in";
const char* EchoTask::out_tag = "echo.out";

class petask_suite :  public :: testing::Test {

public:
    friend class EchoTask;

protected:
    petask_suite() {};
    ~petask_suite() {};
    void SetUp()
    {

    };
    void TearDown()
    {
    };

};



TEST_F(petask_suite, test_pe_emit_tuple)
{


    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "echo_task", 1);

    //TEST:commit log.can test log's content
    // prepare commit log if necessary.
    // here we use local filesystem commit log instead of bigpipe.
    if (!DoesPathExist(Task::kDefaultCommitLogLFSName)) {
        const char* content = "Love Way You Lie(Rihana Feat. Eminem)";
        LocalFileCommitLog cl(LocalFileCommitLog::kPublisher, Task::kDefaultCommitLogLFSName);
        DSTREAM_WARN("cl open...");
        if (cl.Open() == -1) {
            DSTREAM_WARN("cl open failed");
            exit(-1);
        }
        DSTREAM_WARN("cl.write...");
        // 10 records.
        for (int i = 0; i < 10; i++) {
            DSTREAM_WARN("cl.write ... %d", i);
            ReadableByteArray bytes(reinterpret_cast<const dstream::Byte*>(content), strlen(content) + 1);
            if (cl.Write(&bytes, NULL, NULL) == -1) {
                DSTREAM_WARN("cl.write failed");
                exit(-1);
            }
        }
    }
    // make a task instance and run.
    EchoTask task;
    //test as an importer
    setenv(Task::kEnvKeyProcessorRole, Task::kRoleAsImporter, 1);
    task.run(true);
    //test as normal primary pe
    //task.run(true, false);
}
