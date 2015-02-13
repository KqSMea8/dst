/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief test for context.h/context.cc
 *
 */

#include "gtest/gtest.h"
#include "processelement/context.h"
#include "processelement/task.h"


using namespace dstream;
using namespace dstream::processelement;
using namespace dstream::processelement::internal;


int main(int argc, char** argv)
{
    logger::initialize("pe_task");
    setenv(Task::kEnvKeyProcessElementName, "echo_task", 1);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class ContextWrapper : public Context {
public:
    bool readEnvironment() {
        return Context::readEnvironment();
    }

};

class context_suite : public :: testing::Test {
public:
    friend class ContextWrapper;

protected:
    context_suite() {};
    ~context_suite() {};
    void SetUp() {};
    void TearDown() {};
};

TEST_F(context_suite, test_role_set)
{
    ContextWrapper context;
    const char* value = NULL;
    context.workAsImporter();
    value = getenv(Context::kEnvKeyProcessorRole);
    EXPECT_STREQ(Context::kRoleAsImporter, value);
    context.readEnvironment();

    context.workAsExporter();
    value = getenv(Context::kEnvKeyProcessorRole);
    EXPECT_STREQ(Context::kRoleAsExporter, value);
    context.readEnvironment();

    context.workAsWorker();
    value = getenv(Context::kEnvKeyProcessorRole);
    EXPECT_STREQ(Context::kRoleAsWorker, value);
    context.readEnvironment();
}

TEST_F(context_suite, test_env_read)
{
    ContextWrapper context;
    const char* value = NULL;

    value = getenv("COVFILE");
    clearenv(); // clear all first
    if (value) {
        setenv("COVFILE", value, 1);
    }

    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyWorkDirectory, "./", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyNodeUri, "ipc://dstream.pe.loopback", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyProcessElementId, "2012", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyHeartbeatPollTimeoutSeconds, "10", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyEachCommitLogPollTimeoutMillSeconds, "10", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyEachCommitLogPollTimeoutMillSeconds, "10", 1);
    ASSERT_FALSE(context.readEnvironment());


    setenv(Context::kEnvKeyLogLevel, "Foo", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv(Context::kEnvKeyLogLevel, Context::kLogLevelInfo, 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv(Context::kEnvKeyLogLevel, Context::kLogLevelDebug, 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv(Context::kEnvKeyLogLevel, Context::kLogLevelWarning, 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyProcessElementName, "work", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyProcessorStatus, Context::kDefaultHasStatus, 1);
    ASSERT_FALSE(context.readEnvironment());

    // importer
    setenv(Context::kEnvKeyProcessorRole, Context::kRoleAsImporter, 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv(Context::kEnvKeyDynamicProtoDir, Context::kDefaultDynamicProtoDir, 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv("dstream.pe.commitLog.number", "1", 1);
    ASSERT_FALSE(context.readEnvironment());

    // bigpipe
    setenv("dstream.pe.commitLog.0.type", "bigpipe", 1);
    ASSERT_FALSE(context.readEnvironment());

    setenv("dstream.pe.pnDirectory", ".", 1);
    ASSERT_FALSE(context.readEnvironment());

    //TODO: return false if not set ?
    setenv("dstream.pe.commitLog.0.bigpipe.name", "bptest", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv("dstream.pe.commitLog.0.bigpipe.pipeletId", "0", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv("dstream.pe.commitLog.0.bigpipe.confpath", "./conf", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv("dstream.pe.commitLog.0.bigpipe.confname", "bigpipe.conf", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv("dstream.pe.ESPUri", "127.0.0.1:9780", 1);
    ASSERT_FALSE(context.readEnvironment());
    setenv("dstream.pe.commitLog.0.bigpipe.token", "token", 1);
    setenv("dstream.pe.appName", "test-app", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.username", "test", 1);
    ASSERT_TRUE(context.readEnvironment());
    setenv("dstream.pe.commitLog.0.bigpipe.subpoint", "-1", 1);
    ASSERT_TRUE(context.readEnvironment());
}











