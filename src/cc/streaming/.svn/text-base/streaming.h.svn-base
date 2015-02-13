/***************************************************************************
 *
 * Copyright (c) 2012 baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/
#ifndef __DSTREAM_CC_STREAMING_STREAMING_H__ // NOLINT
#define __DSTREAM_CC_STREAMING_STREAMING_H__

/**
 * @file streaming.h
 * @author Yuncong Zhang(zhangyuncong@baidu.com)
 * @date 2012/06/05 14:06:18
 * @version 1.0
 * @brief
 *
 **/

#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>     // NOLINT
#include <iostream>	   // NOLINT
#include <map>
#include <string>

#include "common/logger.h"
#include "common/proto/streaming.pb.h"
#include "common/utils.h"
#include "processelement/task.h"

#include <getopt.h> // NOLINT

namespace dstream {
namespace streaming {
using namespace dstream::processelement;
class Hash {
public:
    virtual uint64_t HashCode(const char* key) const = 0;
};
class ElfHash : public Hash {
public:
    virtual uint64_t HashCode(const char* key) const {
        uint32_t hash = 0;
        uint32_t x    = 0;
        while (*key) {
            hash = (hash << 4) + (*key++);
            if ((x = hash & 0xF0000000) != 0) {
                hash ^= (x >> 24);
                hash &= ~x;
            }
        }
        return static_cast<uint64_t>(hash & 0x7FFFFFFF);
    }
};
class HashFromShell : public Hash {
public:
    explicit HashFromShell(const string& shell_cmd): m_shell_cmd(shell_cmd) {
    }
    virtual uint64_t HashCode(const char* key) const {
        uint64_t ret = 0;
        FILE* out = popen((string("echo ") + key + " | " + m_shell_cmd).c_str(), "r");
        if (fscanf(out, "%lu", &ret) != 1) {
            DSTREAM_ERROR("meet an error when get stdout of the user hash shell");
        }
        pclose(out);
        return ret;
    }
    virtual ~HashFromShell() {
    }
protected:
    string m_shell_cmd;
};

static int s_child_pid = 0;

class StreamingTask : public Task {
public:
    // types we may encounter.
    typedef ProtoTuple< StreamingKVType > StreamingTuple;
    typedef DefaultTupleTypeInfo< StreamingTuple > StreamingTupleInfo;
    static const char* kTag;
    static const char* kContentStop;
    static const int kDefaultBufSize = 1 << 22; // 4M
    static const char* kDefaultTagStop;
    static const char* kDefaultKeyStop;

    StreamingTask(std::string end_of_tag, std::string end_of_key,
                  Hash* hash, int buf_size, bool with_tag, bool with_logging);
    StreamingTask(std::string end_of_tag, std::string end_of_key,
                  Hash* hash, int buf_size) {
        StreamingTask(end_of_tag, end_of_key, hash, buf_size, false, false);
    }
    StreamingTask(std::string end_of_tag, std::string end_of_key, Hash* hash) {
        StreamingTask(end_of_tag, end_of_key, hash, kDefaultBufSize, false, false);
    }
    StreamingTask(std::string end_of_tag, std::string end_of_key) {
        StreamingTask(end_of_tag, end_of_key, new ElfHash, kDefaultBufSize, false, false);
    }
    explicit StreamingTask(std::string end_of_tag) {
        StreamingTask(end_of_tag, kDefaultKeyStop, new ElfHash, kDefaultBufSize, false, false);
    }
    StreamingTask() {
        StreamingTask(kDefaultTagStop, kDefaultKeyStop, new ElfHash, kDefaultBufSize, false, false);
    }

    virtual ~StreamingTask() {
        StopThreads();
    }

    static void IgnoreSignal(int signo) {
        DSTREAM_WARN("signal [%d] arrived but ignored.", signo);
    }

    static void TerminateSignal(int signo) {
        DSTREAM_ERROR("signal [%d] arrived and terminate streaming PE.", signo);
        // if segment fault, handle specially
        if (signo == SIGSEGV) {
            signal(SIGSEGV, SIG_DFL);
            if (0 != s_child_pid) {
                kill(s_child_pid, SIGKILL);
            }
            kill(getpid(), signo);
        }
        // kill user process
        kill(0, SIGKILL);
    }

    void SetSignalHandler() {
        // static int ignore_signals[]={};
        static int terminate_signals[] = {SIGABRT, SIGFPE, SIGINT, SIGTERM/*SIGSEGV*/};
        // special handle signals
        for (uint32_t i = 0; i != sizeof(terminate_signals) / sizeof(int); i++) { //NOLINT
            signal(terminate_signals[i], TerminateSignal);
        }
    }
    virtual void OnTuples(const Tuples& tuples);
    virtual void OnSpout(ReadableByteArray* bytes);
    void StartProcess();
    void StartThreads();
    void StopThreads();
    void ReadStdErr();
    void ReadStdout();

protected:
    static void* ReadStderrThread(void* data) {
        StreamingTask* streaming = static_cast<StreamingTask*>(data);
        streaming->ReadStdErr();
        return NULL;
    }
    static void* ReadStdoutThread(void* data) {
        StreamingTask* streaming = static_cast<StreamingTask*>(data);
        streaming->ReadStdout();
        return NULL;
    }
    static void* StartProcessThread(void* data) {
        StreamingTask* streaming = static_cast<StreamingTask*>(data);
        streaming->StartProcess();
        return NULL;
    }

    bool   m_stop;
    string m_content_stop;
    string m_tag_stop;
    string m_key_stop;
    Hash*  m_hash;
    FILE*  m_child_stdin;
    FILE*  m_child_stdout;
    FILE*  m_child_stderr;
    int    m_buf_size;
    bool   m_with_tag;
    bool   m_with_logging;
	char*  m_out_buf; // for user stdout
	char*  m_in_buf; // for user stdin

    pthread_t m_stdout_capture_thread;
    pthread_t m_stderr_capture_thread;
    pthread_t m_user_process_handler_thread;
}; // class StreamingTask

extern string g_end_of_tag; // NOLINT
extern string g_end_of_key; // NOLINT
extern bool g_with_tag;
extern bool g_with_logging;
extern int g_pipe_buffer_size;
extern dstream::streaming::Hash* g_hash_to_use;
void ParseArgs(int argc, char** argv);

} // namespace streaming
} // namespace dstream


#endif  // __DSTREAM_CC_STREAMING_STREAMING_H__ // NOLINT

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
