/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
#include <streaming/streaming_logagent_importer.h>
#include <gtest/gtest.h>
#include <google/protobuf/text_format.h>
#include "person.pb.h"
	
using namespace std;
using namespace dstream;
using namespace dstream::streaming;
	
LogAgentStreamingTask *STask;

int main(int argc, char **argv)
{
    ::dstream::logger::initialize("test_streaming_logagent_importer");
    STask = new LogAgentStreamingTask(" ", " ", NULL, 1024, true, true);
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
 
/**
 * @brief 
**/
class test_LogAgentStreamingTask_suite : public ::testing::Test{
    protected:
        test_LogAgentStreamingTask_suite(){};
        virtual ~test_LogAgentStreamingTask_suite(){};
        virtual void SetUp() {
            srand(time(NULL));
        };
        virtual void TearDown() {
            srand(time(NULL));
        };

        std::string rand_string() {
            std::string result;
            int randsz = rand() % 100 + 1; // max 100
            for (int i = 0; i < randsz; ++i) {
                result.append(1, '0'+rand()%78);
            }
            return result;
        }

        Person GetSomebody() {
            Person anonymous;
            anonymous.set_id(rand());
            anonymous.set_name(rand_string());
            anonymous.set_email(rand_string());
            return anonymous;
        }
};
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_LogAgentStreamingTask_suite, test_LogAgentStreamingTask)
{
    ASSERT_TRUE(true);
}
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_LogAgentStreamingTask_suite, test_Base64EncodeDecode)
{
    for (int i = 0; i < 100; ++i) {
        Person pp = GetSomebody();
        std::string serilaze_buf, result, line;

        // test encode
        pp.SerializeToString(&serilaze_buf);
        STask->Base64Encode(serilaze_buf.data(), serilaze_buf.size(), &result);

        // test decode
        char buffer[10240];
        size_t buflen = 10240;
        STask->Base64Decode(result, buffer, &buflen);

        // test deserialize
        Person npp;
        npp.ParseFromArray(buffer, buflen);

        //std::string outstring;
        //google::protobuf::TextFormat::PrintToString(npp, &outstring);
        //std::cout << "result decode: " << outstring << std::endl;

        ASSERT_TRUE(pp.id() == npp.id());
        ASSERT_TRUE(pp.name() == npp.name());
        ASSERT_TRUE(pp.email() == npp.email());
    }
}
 
/* vim: set ts=4 sw=4 sts=4 tw=100 */
