#include "common/logger.h"
#include "common/proto/dstream_proto_common.pb.h"
#include <gtest/gtest.h>
#include "processelement/task.h"

using namespace dstream;
using namespace dstream::processelement;

//test in tuple type
typedef ProtoTuple<PNID> PNIDInTuple;
typedef DefaultTupleTypeInfo<PNIDInTuple> PNIDInTupleInfo;
typedef ProtoTuple<AppID> AppIDInTuple;
typedef DefaultTupleTypeInfo<AppIDInTuple> AppIDInTupleInfo;
//Test Task class
class TestTaskCounterTask : public Task
{
public:
    TestTaskCounterTask() {
        registerTupleTypeInfo("in", new PNIDInTupleInfo);
        registerTupleTypeInfo("in2", new AppIDInTupleInfo);
    }
};

TEST(TestTaskIncounter, TestTaskIncounter)
{
    TestTaskCounterTask tct;
    PNIDInTuple* emit_tuple = (PNIDInTuple*)tct.emit("in");
    ASSERT_TRUE(emit_tuple != 0); //tag exist

    emit_tuple = (PNIDInTuple*)tct.emit("in_xx");
    ASSERT_TRUE(emit_tuple == 0); //tag not exist

    emit_tuple = (PNIDInTuple*)tct.emitWithLock("in2");
    ASSERT_TRUE(emit_tuple != 0); //tag exist
    tct.emitUnlock();

    emit_tuple = (PNIDInTuple*)tct.emit("in_yy");
    ASSERT_TRUE(emit_tuple == 0); //tag not exist
    tct.emitUnlock();
}

int main(int argc, char* argv[])
{
    ::dstream::logger::initialize("test_task_incounter");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
