#include "common/logger.h"
#include "common/proto/dstream_proto_common.pb.h"
#include <gtest/gtest.h>
#include "processelement/task.h"

using namespace dstream;
using namespace dstream::processelement;

//test in tuple type
typedef ProtoTuple<PNID> PNIDInTuple;
typedef DefaultTupleTypeInfo<PNIDInTuple> PNIDInTupleInfo;
//Test Task class
class TestTaskCounterTask : public Task
{
public:
    TestTaskCounterTask() {
        registerTupleTypeInfo("in", new PNIDInTupleInfo);
    }
};

TEST(TestTaskIncounter, TestTaskIncounter)
{
    TestTaskCounterTask tct;
    PNIDInTuple tuple;
    ASSERT_EQ(tuple.hash_code(), 0u);
    PNIDInTuple* emit_tuple = (PNIDInTuple*)tct.emit("in");
    ASSERT_EQ(emit_tuple->hash_code(), 0u);
    PNIDInTuple tp;
    ASSERT_EQ(tp.hash_code(), 1u);
    PNIDInTuple* emit2 = (PNIDInTuple*)tct.emit("in");
    ASSERT_EQ(emit2->hash_code(), 1u);
}

TEST(TestCounter, TestCounter)
{
    Counter counter1(0);
    Counter counter2(1, "counter");

    ASSERT_EQ(counter1.id(), 0);
    ASSERT_EQ(counter2.id(), 1);
    ASSERT_TRUE(counter1.name() == NULL);
    ASSERT_STREQ(counter2.name()->c_str(), "counter");

    counter1.add_amount(3);
    counter2.add_amount(5);
    counter2.add_amount(7);
    ASSERT_EQ(counter1.amount(), 3);
    ASSERT_EQ(counter2.amount(), 12);

    //Counter counter3(counter2);
    //counter3.add_amount(1);
    //ASSERT_EQ(counter3.amount(), 13);
}

int main(int argc, char* argv[])
{
    ::dstream::logger::initialize("test_task_incounter");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
