/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief
 *
 */

#include <string>
#include "common/logger.h"
#include "processelement/task.h"
#include "echo.pb.h"

#include "gtest/gtest.h"
using namespace dstream;
using namespace dstream::processelement;

int main(int argc, char** argv)
{
    logger::initialize("pe_task");
    setenv(Task::kEnvKeyLogLevel, "WARNING", 1);
    setenv(Task::kEnvKeyProcessElementName, "echo_task", 1);

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

    static const int kBatchSize = 10;
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
    // ----------------------------------------
    // mock process node parts...
    // when PE really works, user don't need the following code.
    // ----------------------------------------
    virtual void LoopSetUp(Tuples* tuples) {
        DSTREAM_DEBUG("generating tuples...");

        // generate one tuple 
        InTuple* tt0 = dynamic_cast<InTuple*>(AllocateTuple(EchoTask::in_tag));

        tt0->set_hash_code(0x1234);
        tt0->set_field("testing.........");
        tuples->push_back(tt0);

        // generate another
        InTuple* tt1 = dynamic_cast<InTuple*>(AllocateTuple(EchoTask::in_tag));

        tt1->set_hash_code(0x2345);
        tt1->set_field("testing2.........");

        tuples->push_back(tt1);
    }

    virtual void LoopTearDown(const Tuples& tuples) {
        DSTREAM_DEBUG("checking post tuples...");
        for (size_t i = 0; i < tuples.size(); ++i) {
            if (tuples[i]->tag() != std::string(EchoTask::out_tag)) {
                DSTREAM_WARN("unknown out tuple tags: %s", (tuples[i]->tag()).c_str());
                continue;
            }

            DSTREAM_INFO("Read out tuple: tag(%s) hash_code(%lu)", 
                    (tuples[i]->tag()).c_str(), tuples[i]->hash_code());
        }
    }
    // ----------------------------------------
    // actually working code which use need to care about.
    // ----------------------------------------
    virtual void onTuples(const Tuples& tuples) {
        Task::onTuples(tuples);
        for (size_t i = 0; i < tuples.size(); i++) {
            const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);

            OutTuple* out_tuple = dynamic_cast<OutTuple*>(emit(out_tag));
            out_tuple->set_field(in_tuple->field());
            out_tuple->set_hash_code(in_tuple->hash_code());
        }
        // update stats
        static int count = 0;
        count += tuples.size();

        if (count > 10000) {
            m_quit = true;    // exit when recv 10000 tuples
        }

        if (count % (kBatchSize * 100) == 0) {
            printf("handle %d tuples\n", count);
        }
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
    void SetUp() {};
    void TearDown() {};
};

TEST_F(petask_suite, test_work_loopback)
{
    sleep(2);
    //setenv(Task::kEnvKeyProcessorRole,Task::kRoleAsWorker, 1);
    EchoTask task;

    task.run(true);
    ASSERT_TRUE(true);
}
