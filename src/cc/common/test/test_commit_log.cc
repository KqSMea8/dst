/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/
#include <com_log.h>
#include <common/commit_log.h>
#include <gtest/gtest.h>
using namespace dstream;
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace scribe::thrift;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("processnode");
    com_loadlog("./conf", "bigpipe_api2.conf");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/**
 * @brief
**/
class test_LogInfo_suite : public ::testing::Test {
protected:
    test_LogInfo_suite() {};
    virtual ~test_LogInfo_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_LogInfo_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_LogInfo_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LogInfo_suite, test_LogInfo)
{
    LogInfo log_info;
    ASSERT_TRUE(log_info.byte_array.m_user_free);
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_LogInfo_suite, test_LogInfo2)
{
    LogInfo log_info;
    std::string message(1024, 'x');
    log_info.byte_array.Write((dstream::Byte*)message.c_str(), message.size());

    LogInfo log_info2 = log_info;
    ASSERT_TRUE(log_info2.byte_array.m_size == message.size());
}

/**
 * @brief
**/
class test_CommitLog_suite : public ::testing::Test {
protected:
    test_CommitLog_suite() {};
    virtual ~test_CommitLog_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_CommitLog_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_CommitLog_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CommitLog_suite, test_CommitLog__1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_CommitLog_suite, test_setSubpoint__1)
{
    //TODO
}

/**
 * @brief
**/
class test_BigPipeCommitLog_suite : public ::testing::Test {
protected:
    test_BigPipeCommitLog_suite() {};
    virtual ~test_BigPipeCommitLog_suite() {};
    virtual void SetUp() {
        pipename = "Inf-hpc-zhenpeng-auth";
        token = "dstream";
        conf_path = "./conf";
        conf_name = "bigpipe_api2.conf";
        ctx = NULL;
        user = "dstream";
    };
    virtual void TearDown() {
        delete ctx;
        ctx = NULL;
    };

    std::string pipename;
    std::string token;
    std::string conf_path;
    std::string conf_name;
    BigPipeCommitLog* ctx;
    std::string user;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_BigPipeCommitLog__1)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kPublisher,
                                             pipename,
                                             conf_path,
                                             conf_name,
                                             user,
                                             token);
    ASSERT_TRUE(ctx != NULL);
    int ret = ctx->Open();
    ASSERT_TRUE(ret == 0);
    ctx->Close();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_open_pub)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kPublisher,
                                             pipename,
                                             conf_path,
                                             conf_name,
                                             user,
                                             token);
    ASSERT_TRUE(ctx != NULL);

    int ret = ctx->Open();
    ASSERT_TRUE(ret == 0);
    ctx->Close();
}
/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_open_pub_failed)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kPublisher, "pipenotexist", conf_path,
                                             conf_name, user, token);
    ASSERT_TRUE(ctx != NULL);

    int ret = ctx->Open();
    ASSERT_FALSE(ret == 0);
    ctx->Close();
    ASSERT_TRUE(ctx->m_init == false);
    ASSERT_TRUE(ctx->m_read_buffer == NULL);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_write_normal)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kPublisher, pipename, conf_path, conf_name,
                                             user, token);
    ASSERT_TRUE(ctx != NULL);
    int ret = ctx->Open();
    ASSERT_TRUE(ret == 0);

    std::string line("hello dstream, new bigpipe api.");
    ReadableByteArray out_bytes(reinterpret_cast<const dstream::Byte*>(line.data()), line.size());

    for (int counter = 0; counter < 200; counter++) {
        int pipelet = rand() % 3; // only 3 pipelets
        ctx->SetPartition(pipelet);
        ret = ctx->Write(&out_bytes, NULL, NULL);
        ASSERT_TRUE(ret == 0);
        std::cout << "send one message to pipelet: "  << pipelet << std::endl;
    }
    ctx->Close();
}

TEST_F(test_BigPipeCommitLog_suite, test_open_sub)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kSubscriber, pipename, conf_path, conf_name,
                                             user, token);
    ASSERT_TRUE(ctx != NULL);

    int ret = ctx->Open();
    ASSERT_TRUE(ret == 0);
    ctx->Close();
    ASSERT_TRUE(ctx->m_init == false);
    ASSERT_TRUE(ctx->m_read_buffer == NULL);
}

TEST_F(test_BigPipeCommitLog_suite, test_open_sub_failed)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kSubscriber, "pipenotexist", conf_path,
                                             conf_name, user, token);
    ASSERT_TRUE(ctx != NULL) << "init OK." << std::endl;

    int ret = ctx->Open();
    ASSERT_FALSE(ret == 0);
    ctx->Close();
    ASSERT_TRUE(ctx->m_init == false);
    ASSERT_TRUE(ctx->m_read_buffer == NULL);
}

TEST_F(test_BigPipeCommitLog_suite, test_open_failed_zero)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kSubscriber, pipename, conf_path,
                                             conf_name, user, token);
    ctx->set_subpoint(0);
    ASSERT_TRUE(ctx != NULL);

    SubPoint point;
    ctx->set_subpoint(0);
    point.set_pipelet(0);
    point.set_msg_id(0);
    point.set_seq_id(1);
    ctx->AddPipelet(point);

    int ret = ctx->Open();
    ASSERT_FALSE(ret == 0);

    ctx->Close();
    ASSERT_TRUE(ctx->m_init == false);
    ASSERT_TRUE(ctx->m_read_buffer == NULL);
}

TEST_F(test_BigPipeCommitLog_suite, test_open_failed_small)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kSubscriber, pipename, conf_path,
                                             conf_name, user, token);
    SubPoint point;
    ctx->set_subpoint(0);
    point.set_pipelet(0);
    point.set_msg_id(1);
    point.set_seq_id(1);
    ctx->AddPipelet(point);
    ASSERT_TRUE(ctx != NULL);
    int ret = ctx->Open();
    ASSERT_TRUE(ret != 0);

    ctx->Close();
    ASSERT_TRUE(ctx->m_init == false);
    ASSERT_TRUE(ctx->m_read_buffer == NULL);
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_read_normal)
{
    ctx = new(std::nothrow) BigPipeCommitLog(CommitLog::kSubscriber, pipename, conf_path, conf_name,
                                             user, token);
    ASSERT_TRUE(ctx != NULL);

    SubPoint point;
    ctx->set_subpoint(-2);

    point.set_pipelet(0);
    point.set_msg_id(-2);
    point.set_seq_id(1);
    ctx->AddPipelet(point);

    point.set_pipelet(1);
    point.set_msg_id(-2);
    point.set_seq_id(1);
    ctx->AddPipelet(point);

    int ret = ctx->Open();
    ASSERT_TRUE(ret == 0);

    // read data
    //LogInfo log_item;
    //int counter = 10;
    //while (ret == 0 && counter--) {
    //    ret = ctx->Read(&log_item);
    //    ASSERT_TRUE(ret == 0);
    //    std::cout << "ppid:" << log_item.pipelet_id
    //        << "\tmsgid:" << log_item.msg_id
    //        << "\tseqid:" << log_item.seq_id
    //        << "\tlast:" << log_item.is_last << std::endl;
    //}
}



/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_close__1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_set_partition__1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_BigPipeCommitLog_suite, test_add_pipelet__1)
{
    //TODO
}

/**
 * @brief
**/
class test_Partitioner_suite : public ::testing::Test {
protected:
    test_Partitioner_suite() {};
    virtual ~test_Partitioner_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_Partitioner_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_Partitioner_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_Partitioner_suite, test_set_partition__1)
{
    //TODO
}

/**
 * @brief
**/
class test_LocalFileCommitLog_suite : public ::testing::Test {
protected:
    test_LocalFileCommitLog_suite() {};
    virtual ~test_LocalFileCommitLog_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_LocalFileCommitLog_suite, *)
        commit_log_file_name_ = "test_loop_local_file";
        commit_log_no_exist_file_name_ = "no_exist_file_name";
        read_str_base_ = "##################################";
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_LocalFileCommitLog_suite, *)
    };

    std::string commit_log_file_name_;
    std::string commit_log_no_exist_file_name_;
    std::string read_str_base_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_LocalFileCommitLog__1)
{
    //TODO
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, true);
    ASSERT_TRUE(cl != NULL);
}

/**
 * @brief
 * @begin_version
**/

TEST_F(test_LocalFileCommitLog_suite, test_open_sub_read_to_end)
{
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, false);
    ASSERT_TRUE(cl != NULL);
    cl->set_subpoint(0);
    int ret = cl->Open();
    ASSERT_TRUE(ret == 0);
    cl->Close();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_open_sub_read_to_no_exist_file)
{
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_no_exist_file_name_, false);
    ASSERT_TRUE(cl != NULL);
    cl->set_subpoint(0);
    int ret = cl->Open();
    ASSERT_TRUE(ret == -1);
    cl->Close();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_open_sub_read_loop)
{
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, true);
    ASSERT_TRUE(cl != NULL);
    cl->set_subpoint(0);
    int ret = cl->Open();
    ASSERT_TRUE(ret == 0);
    cl->Close();
}


/**
 * @brief local file store 10 records, as read loop we can always read data
 * even more than the local file got.
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_read_loop__1)
{
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, true);
    ASSERT_TRUE(cl != NULL);
    cl->set_subpoint(0);
    int ret = cl->Open();
    ASSERT_TRUE(ret == 0);
    for (int i = 0; i < 23 ; i++)
    {
        LogInfo log;
        ret = cl->Read(&log);
        ASSERT_TRUE(ret == 0);
        std::string str_test;
        ByteSize size = 0;
        StringAppendNumber(&str_test, i % 10);
        str_test = str_test + ":" + read_str_base_;
        string str_log(reinterpret_cast<const char*>(log.byte_array.Data(&size)), size);
        ASSERT_STREQ(str_log.c_str(), str_test.c_str());
    }
    cl->Close();
}

/**
 * @brief when 10 records run out ,you can not read data any more.
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_read_to_end_1)
{
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, false);
    ASSERT_TRUE(cl != NULL);
    cl->set_subpoint(0);
    int ret = cl->Open();
    ASSERT_TRUE(ret == 0);
    for (int i = 0; i < 23 ; i++)
    {
        LogInfo log;
        ret = cl->Read(&log);
        if ( i < 10 ) {
            ASSERT_TRUE(ret == 0);
            std::string str_test;
            ByteSize size = 0;
            StringAppendNumber(&str_test, i);
            str_test = str_test + ":" + read_str_base_;
            string str_log(reinterpret_cast<const char*>(log.byte_array.Data(&size)), size);
            ASSERT_STREQ(str_log.c_str(), str_test.c_str());
        }
        else {
            ASSERT_TRUE(ret == error::FILE_READ_EOF);
        }
    }
    cl->Close();
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_write__1)
{
    //TODO
}

/**
 * @brief
 * @begin_version
**/
TEST_F(test_LocalFileCommitLog_suite, test_close__1)
{
    //TODO
    LocalFileCommitLog* cl = new LocalFileCommitLog(CommitLog::kSubscriber,
                                                    commit_log_file_name_, true);
    ASSERT_TRUE(cl != NULL);
    int ret = cl->Open();
    ASSERT_TRUE(ret == 0);
    ret = cl->Close();
    ASSERT_TRUE(ret == 0);
}

/**
 * @brief
**/
class test_ScribeCommitLog_suite : public ::testing::Test {
protected:
    test_ScribeCommitLog_suite() {};
    virtual ~test_ScribeCommitLog_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_ScribeCommitLog_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_ScribeCommitLog_suite, *)
    };
private:
};

/**
 * @brief
 * @begin_version
**/
//TEST_F(test_ScribeCommitLog_suite, test_ScribeCommitLog__open)
//{
//    boost::shared_ptr<ScribeCommitLog> ctx = boost::shared_ptr<ScribeCommitLog>(new 
//        ScribeCommitLog(CommitLog::kSubscriber, 123));
//    ASSERT_TRUE(ctx != NULL);
//    int ret = ctx->Open();
//    ASSERT_TRUE(ret == 0);
//    while(!ctx->IsInited()) {
//        usleep(1000);
//    }
//    ctx->Stop();
//    pthread_cancel(ctx->m_tid);
//}

/**
 * @brief
 * @begin_version
**/
//TEST_F(test_ScribeCommitLog_suite, test_ScribeCommitLog__read)
//{
//    boost::shared_ptr<ScribeCommitLog> ctx = boost::shared_ptr<ScribeCommitLog>(new 
//        ScribeCommitLog(CommitLog::kSubscriber, 123));
//    ASSERT_TRUE(ctx != NULL);
//    int ret = ctx->Open();
//    ASSERT_TRUE(ret == 0);
//
//    while(!ctx->IsInited()) {
//        usleep(1000);
//    }
//    // make a thrift client to send message
//    boost::shared_ptr<TTransport> socket(new TSocket("127.0.0.1", ctx->Port()));
//    boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
//    boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
//    boost::shared_ptr<scribeClient> sbclient(new scribeClient(protocol));
//    transport->open();
//    LogEntry le;
//    le.message = "hello";
//    le.category = "test";
//    std::vector<LogEntry> messages;
//    messages.push_back(le);
//    sbclient->Log(messages);
//    transport->close();
//
//    LogInfo log_info;
//    ret = ctx->Read(&log_info);
//    const char* str = reinterpret_cast<const char*>(log_info.byte_array.Data(NULL));
//    ASSERT_STREQ(str, "hello");
//
//    ctx->Stop();
//    pthread_cancel(ctx->m_tid);
//}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
