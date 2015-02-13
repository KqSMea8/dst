#include <stdlib.h>

#include <gtest/gtest.h>

#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_common.h"

using namespace dstream;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

const int kNum = 10;

class test_DumpTuples_suite : public ::testing::Test {
protected:
    test_DumpTuples_suite() {};
    virtual ~test_DumpTuples_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

void FillData(BufHandle** hdl, int* size, int* batch) {
    CBufWriteStream bws;
    CRandom rand;
    rand.SetSeed(-1);

    *size = 0;
    *batch = 0;
    for (int i = 0; i < kNum; ++i) {
        int len = (int)(rand.Next() * 94) + 1;
        std::string s;
        for (int j = 32; j < 32 + len; ++j) {
            s += (char)j;
        }

        int tag_len   = (int)(strlen(kTestTag) + 1);
        int data_len  = (int)s.size();
        int total_len = (int)(sizeof(uint64) + sizeof(int)) + tag_len + data_len;
        ASSERT_TRUE(bws.PutObject<int>(total_len));//total len
        ASSERT_TRUE(bws.PutObject<uint64>((uint64)(i)));//hash code
        ASSERT_TRUE(bws.PutObject<int>(tag_len));//tag len
        ASSERT_EQ(bws.PutBuffer(kTestTag, tag_len), tag_len);//tag
        ASSERT_EQ(bws.PutBuffer(s.data(), (int)s.size()), (int)s.size());
        (*batch)++;
        *size = *size + total_len + (int)sizeof(int);
    }
    *hdl = bws.GetBegin();
}

TEST_F(test_DumpTuples_suite, DumpTuples) {
    BufHandle* hdl = NULL;
    int size;
    int batch;

    FillData(&hdl, &size, &batch);

    std::string s;
    ASSERT_EQ(transporter::DumpTuplesFromBufHdl(hdl, size, batch, &s),
              error::OK);
    printf("tuples: %s\n", s.c_str());

    ChainFreeHdl(hdl, NULL);
    hdl = NULL;
}
