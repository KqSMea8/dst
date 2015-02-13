
/**
* @brief  test of importer loopback
* @author fangjun,fangjun02@baidu.com
* @date   2013-02-28
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
    setenv(Task::kEnvKeyLogLevel, "DEBUG", 1);
    setenv(Task::kEnvKeyProcessElementName, "echo_task", 1);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

bool g_ut_flag = true;
std::string  error_str;

class EchoImporter : public Task {
public:
    static const char* out_tag;
    typedef ProtoTuple< EchoResponse> OutTuple;
    typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

    static const int kBatchSize = 10000;
    static const int kDefaultUpPEId = 2013;
    static const int kDefaultUpPipeId = 2013;
    // ----------------------------------------
    EchoImporter() {
        registerTupleTypeInfo(EchoImporter::out_tag, new OutTupleInfo());
    }

    virtual void onHeartbeat(const uint64_t peid,
                             const std::map< std::string, std::string>& /*metrics*/) {
        DSTREAM_INFO("\n[TEST]onHeartbeat(%lu)...", (uint64_t)peid);
    }

    virtual void onSpout(ReadableByteArray* bytes) {
        Task::onSpout(bytes);
        DSTREAM_INFO("\n[TEST]onSpout");
        const char* data = reinterpret_cast<const char*>(bytes->Data(NULL));
        static int count = 0;
        count++;
        if (count > 100) {
            m_quit = true; // exit when recv 10000 tuples
        }
        DSTREAM_INFO("TESTdata : %s  [END]", data);

        static size_t hash_code = 0;
        OutTuple* out_tuple = dynamic_cast<OutTuple*>(emit(out_tag));
        if (out_tuple == NULL) {
            g_ut_flag = false;
            error_str.append("allocate out tuple fail.\n");
            return;
        }
        out_tuple->set_field(data);
        out_tuple->set_hash_code(hash_code);
        hash_code++;
    }
private:
}; // class EchoImporter

const char* EchoImporter::out_tag = "echo.out";

class test_Importer_suite :  public :: testing::Test {

public:
    friend class EchoImporter;

protected:
    test_Importer_suite() {};
    ~test_Importer_suite() {};
    void SetUp() {
    };
    void TearDown() {
    };
};

TEST_F(test_Importer_suite, test_importer_loopback)
{
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
    setenv(Task::kEnvKeyProcessorRole, Task::kRoleAsImporter, 1);
    EchoImporter importer;
    //test as an importer
    importer.run(true);
    ASSERT_TRUE(g_ut_flag);
    if (false == g_ut_flag) {
        printf("ERROR: \n%s", error_str.c_str());
    }
    importer.Release();
}

