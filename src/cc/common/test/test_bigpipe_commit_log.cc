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

#include <gtest/gtest.h>
#include "common/logger.h"
#include "common/commit_log.h"

using namespace dstream;
#define TAG "@@@@@@@@@@@@@@@@@@@@"

// seems there are a lot of problems in there.
TEST(BigPipeCommitLog, AllCases) {
    static const int kPipeId = 2012;
    BigPipeCommitLog pub(BigPipeCommitLog::kPublisher, kPipeId,
                         "pbatch1k", BigPipeCommitLog::kDefaultPipeletId, "./conf", "pub.conf");
    ASSERT_TRUE(pub.open() == 0);
    DSTREAM_NOTICE("session : %s", pub.session().c_str());
    const char* data = "I can't you tell what really is I can only tell you what feels like(Eninem)";
    for (int i = 0; i < 10; i++) {
        ReadableByteArray bytes(reinterpret_cast<const dstream::Byte*>(data), strlen(data) + 1);
        uint64_t log_id;
        ASSERT_TRUE(pub.write(&bytes, NULL, &log_id) == 0);
        printf("%zu\n", static_cast<size_t>(log_id));
    }
    pub.close();

    BigPipeCommitLog sub(BigPipeCommitLog::kSubscriber, kPipeId,
                         "pbatch1k", BigPipeCommitLog::kDefaultPipeletId, "./conf", "sub.conf");
    ASSERT_TRUE(sub.open(1) == 0);
    while (1) {
        WriteableByteArray bytes;
        uint64_t id;
        bool eof;
        ASSERT_TRUE(sub.peek(&eof, 1000) == 0);
        if (!eof) {
            ASSERT_TRUE(sub.read(&bytes, &id) == 0);
            const char* s = reinterpret_cast<const char*>(bytes.data(NULL));
            printf("@@@@@@@@@@@@@@@@@@@@%s, id=%lu\n", s, id);
        } else {
            break;
        }
    }
    sub.close();
}

int main(int argc, char** argv) {
    ::logger::initialize("test_bigpipe_commit_log");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
