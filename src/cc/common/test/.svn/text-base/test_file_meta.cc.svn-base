#include <cstdio>
#include "gtest/gtest.h"
#include "common/error.h"
#include "common/file_meta.h"

using namespace dstream;

class TestFileMeta: public ::testing::Test {
protected:
    bool CheckFile(const char* filename);
};

bool TestFileMeta::CheckFile(const char* filename) {
    return access(filename, F_OK) == 0;
}

TEST_F(TestFileMeta, TestWriteMetaDeepth_1) {
    ASSERT_FALSE(filemeta::WriteMeta("./1111", "abc", "ddd", true, false) == dstream::error::OK);
    ASSERT_FALSE(CheckFile("./test/test"));

    ASSERT_TRUE(filemeta::WriteMeta("./test", "test", "ddd", true, true) == dstream::error::OK);
    ASSERT_TRUE(CheckFile("./test/test"));
    std::string value;
    ASSERT_EQ(dstream::error::OK, filemeta::GetMeta("./test", "test", &value));
    ASSERT_EQ("ddd", value);
    filemeta::WriteMeta("./test", "abc", "def");
    filemeta::WriteMeta("./test", "acd", "acd");
    filemeta::WriteMeta("./test", "acd", "bcd", false);
    std::map<std::string, std::string> meta;
    filemeta::GetMeta("./test", &meta);
    ASSERT_EQ(meta.size(), 3u);
    ASSERT_EQ("def", meta["abc"]);
    filemeta::ClearMeta("./test", meta);
    filemeta::GetMeta("./test", &meta);
    ASSERT_EQ(0u, meta.size());
    ASSERT_EQ(0, remove("./test"));
    ASSERT_NE(dstream::error::OK, filemeta::GetMeta("./test", "abc", &value));
}



int main(int argc, char** argv) {
    ::logger::initialize("test_file_meta");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
