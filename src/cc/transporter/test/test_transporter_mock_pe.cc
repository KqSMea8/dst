#include <stdlib.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Random.h"
#include "transporter_env.h"
#include "transporter_test_common.h"
#include "transporter/transporter_stubmgr.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_inflow.h"

using ::testing::_;
using ::testing::Eq;
using ::testing::Field;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::NotNull;
using ::testing::AtLeast;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::DefaultValue;
using ::testing::MockFunction;

using namespace dstream;
using namespace dstream::transporter;

typedef std::vector<IfSourceMeta> TPIfSourceVec;
typedef std::vector<OfMeta>       TPOfSourceVec;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}

class test_Transporter_TransporterDataFlow_suite : public ::testing::Test {
protected:
    test_Transporter_TransporterDataFlow_suite(): tp_(NULL), id_(kLocalId) {
    }

    virtual ~test_Transporter_TransporterDataFlow_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
        if (NULL != tp_) {
            tp_->Stop();
            tp_->Release();
            tp_ = NULL;
        }
        g_pTPStubMgr->FreeOfStub(id_);
    };
public:
    static const uint64_t kUpId       = MAKE_PEID(1UL, 0UL, 1UL);
    static const uint64_t kLocalId    = MAKE_PEID(1UL, 1UL, 3UL);
    static const uint64_t kDownId     = MAKE_PEID(1UL, 2UL, 2UL);

public:
    Transporter*        tp_;
    uint64_t            id_;
    uint64_t            stub_;
};

TEST_F(test_Transporter_TransporterDataFlow_suite, FullDataFlow) {
}
