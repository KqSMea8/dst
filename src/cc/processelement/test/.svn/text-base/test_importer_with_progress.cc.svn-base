/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/



/**
 * @file test/test_importer_with_progress.cc
 * @author zhenpeng(zhenpeng@baidu.com)
 * @date 2013/01/29 15:08:42
 * @version $Revision$
 * @brief
 *
 **/
#include <gtest/gtest.h>
#include <processelement/task.h>

using namespace std;
using namespace dstream;
using namespace dstream::processelement;

static const char* fifo_path = "./test_importer_with_progress.fifo";

class ProgressTask: public Task {
public:
    ProgressTask(uint64_t idx, int fifo): round(0), index(idx), first_run(true), fifo_(fifo) {
    }

    virtual void init()
    {
        // set messageid, seqid
        BPPoint val;
        int ret = read(fifo_, &val, sizeof(val));
        if (ret < 0) {
            DSTREAM_INFO("read subpoint info from parent failed.");
            _exit(-3);
        }
        DSTREAM_INFO("impoter[%lu] restore local progress: %ld:%lu", index, val.msg_id_, val.seq_id_);
        sub_point_ = val;
    }

    virtual void onSpout(ReadableByteArray* bytes)
    {
        std::string value;
        Context::get(kCtxImporterMessageID, value);
        int64_t messageid = strtoul(value.c_str(), NULL, 10);
        Context::get(kCtxImporterSequenceID, value);
        uint64_t sequenceid = strtoul(value.c_str(), NULL, 10);

        if (messageid == sub_point_.msg_id_) {
            if (sequenceid < sub_point_.seq_id_) {
                // subpoint restored from zk
                DSTREAM_INFO("seqid rollback, now: %lu, local: %lu", sequenceid, sub_point_.seq_id_);

            } else if (sequenceid > sub_point_.seq_id_ + 1) {
                DSTREAM_INFO("seqid skipped, now: %lu, local: %lu. error exit.", sequenceid, sub_point_.seq_id_);
                ack_parent_fail();
                _exit(-1); // error exit
            }

        } else if (messageid == sub_point_.msg_id_ + 1) {
            // normal case, recv a new message
            DSTREAM_INFO("messageid skiped, now: %lu, local: %lu", messageid, sub_point_.msg_id_);

        } else {
            // first run
            if (index == 0 && first_run) {
                first_run = false;
                sub_point_.msg_id_ = messageid;
                sub_point_.seq_id_ = sequenceid;
                return;
            }
            DSTREAM_INFO("unexcepted error!!!, now: %lu, local: %ld", messageid, sub_point_.msg_id_);
            ack_parent_fail();
            _exit(-2);
        }

        // random update
        if (rand() % 20 == 0) {
            sub_progress_.update(BPPoint(messageid, sequenceid));
            DSTREAM_INFO("update subpoint(%ld:%lu)", messageid, sequenceid);
        }

        // read last save message
        if (round++ % 30 == 0) {
            int ret = sub_progress_.load();
            if (ret != 0) {
                DSTREAM_INFO("load progress failed.");
                ack_parent_fail();
                _exit(-4);
            }
            BPPoint val;
            ret = sub_progress_.read(&val);
            if (ret != 0) {
                DSTREAM_INFO("read progress failed.");
                ack_parent_fail();
                _exit(-5);
            }
            last_save_point_ = val;
            DSTREAM_INFO("save subpoint(%ld:%lu)", val.msg_id_, val.seq_id_);
        }

        // update global counter
        sub_point_.msg_id_ = messageid;
        sub_point_.seq_id_ = sequenceid;

        if (round >= 1000) {
            DSTREAM_INFO("importer[%lu] finished, cur msg_id: %lu, seq_id: %lu", index, messageid, sequenceid);
            // write back
            write(fifo_, &last_save_point_, sizeof(last_save_point_));
            ::close(fifo_);
            quit_ = true; // exit
        }
    }

protected:
    void ack_parent_fail()
    {
        sub_point_.msg_id_ = 0;
        sub_point_.seq_id_ = 0;
        write(fifo_, &sub_point_, sizeof(sub_point_)); // ack parent
    }

    uint64_t round;
    uint64_t index;
    bool first_run;
    BPPoint sub_point_;
    BPPoint last_save_point_;
    int fifo_;
};

class test_importer_suite : public ::testing::Test {
protected:
    test_importer_suite(): kRound(2)
    {};
    virtual ~test_importer_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
        if (unlink(fifo_path) < 0) {
            perror("unlink failed");
        }
    };

    // total test round
    const int kRound;
};

void setenv()
{
    setenv(dstream::processelement::Task::kEnvKeyLogLevel, "INFO", 1);
    setenv(dstream::processelement::Task::kEnvKeyProcessElementName, "test_importer_with_progress", 1);
    setenv("dstream.pe.workDirectory", ".", 1);
    setenv("dstream.pe.nodeUri", "tcp://127.0.0.1:5101", 1);
    setenv("dstream.pe.id", "20130129", 1);
    setenv("dstream.pe.chkpPollTimeoutSeconds", "1", 1);
    setenv("dstream.pe.heartbeatPollTimeoutSeconds", "1", 1);
    setenv("dstream.pe.eachCommitLogPollTimeoutMillSeconds", "100", 1);
    setenv("dstream.pe.tuplesBatchCount", "30", 1);
    setenv("dstream.pe.forceForwardTuplesPeriodMillSeconds", "100", 1);
    setenv("dstream.pe.pnDirectory", ".", 1);
    setenv("dstream.pe.importerUri", "ipc://dstream.pe.importer", 1);
    setenv("dstream.pe.role", "importer", 1);
    setenv("dstream.pe.hasStatus", "0", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.token", "token", 1);
    setenv("dstream.pe.commitLog.0.type", "bigpipe", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.pipeletId", "1", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.subpoint", "-2", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.name", "inf-hpc-zp", 1);
    setenv("dstream.pe.commitLog.number", "1", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.confpath", "./conf", 1);
    setenv("dstream.pe.commitLog.0.bigpipe.confname", "test_importer_with_progress_bigpipe.conf", 1);
    setenv("importer.bp_progress.switch", "true", 1);
}

TEST_F(test_importer_suite, run)
{
    ASSERT_TRUE(mkfifo(fifo_path, 0777) == 0);
    BPPoint local_progress(-1, 0);
    for (int i = 0; i < kRound; i++) {
        int fifo = open(fifo_path, O_RDWR);
        if (fifo < 0) {
            perror("open fifo failed.");
        }
        ASSERT_TRUE(fifo > 0);

        int pid = fork();
        if (pid == 0) {
            DSTREAM_INFO("start importer[%d]...", i);
            ProgressTask task(i, fifo);
            task.run(true); // use loopback mode
            _exit(-1);

        } else if (pid > 0) {
            // send last progress
            sleep(1);
            int ret = write(fifo, &local_progress, sizeof(local_progress));
            ASSERT_TRUE(ret > 0);
            DSTREAM_INFO("send local progress(%ld:%lu) to importer[%d]", local_progress.msg_id_,
                         local_progress.seq_id_, i);

            DSTREAM_INFO("wait for importer[%d] finish.", i);
            // get last progress from pe
            BPPoint tmp_progress;
            ret = read(fifo, &tmp_progress, sizeof(tmp_progress));
            waitpid(pid, NULL, 0);

            // make sure progress is growing
            ASSERT_TRUE(tmp_progress.msg_id_ >= local_progress.msg_id_);
            if (tmp_progress.msg_id_ <= local_progress.msg_id_) {
                ASSERT_TRUE(tmp_progress.seq_id_ > local_progress.seq_id_);
            }
            local_progress = tmp_progress;
            DSTREAM_INFO("get importer[%d] last progress(%ld:%lu)", i, local_progress.msg_id_,
                         local_progress.seq_id_);

        } else {
            // error exit
            ASSERT_TRUE(false);
        }
    }
}

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_importer_with_progress");
    testing::InitGoogleTest(&argc, argv);
    com_loadlog("./conf/", "test_importer_with_progress_bigpipe.conf");
    setenv();
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
