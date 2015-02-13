/** 
* @file     streaming.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.1
* @date     2013-07-26
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include "streaming/streaming.h"
#include <boost/algorithm/string.hpp>
namespace dstream {
namespace streaming {

std::string g_excutive_name;
std::string g_stream_exec_name;


static int RedirectStdInOutErr(const char* cmd, FILE*& pstdin, FILE*& pstdout, FILE*& pstderr) {
    int outfd[2];
    int infd[2];
    int errfd[2];
    pipe(outfd);
    pipe(infd);
    pipe(errfd);
    int pid = fork();
    if (0 == pid) {
        close(0);
        close(1);
        close(2);
        dup2(outfd[0], 0); // Make the read end of outfd pipe as stdin
        dup2(infd[1], 1); // Make the write end of infd as stdout
        dup2(errfd[1], 2); // stderr
        close(outfd[0]);
        close(outfd[1]);
        close(infd[0]);
        close(infd[1]);
        close(errfd[0]);
        close(errfd[1]);
        const char* args[] = { "/bin/bash", "-c", cmd, NULL };
        if (0 != execv(args[0], (char * const*) args)) {
            DSTREAM_ERROR("Error when execv: %s\n", strerror(errno));
        }
    } else {
        close(outfd[0]);
        close(infd[1]);
        close(errfd[1]);
        pstdin = fdopen(outfd[1], "w");
        pstdout = fdopen(infd[0], "r");
        pstderr = fdopen(errfd[0], "r");
    }
    return pid;
}

static char* GetFileStream(char* target, int max_len, FILE* file, const char* /*delim*/) {
    return fgets( target, max_len, file);
}

int GetChangeChar(const char* str, int* start) {
    assert(NULL != start);
    int c, n = 1;
    if (str[*start] == '0') {
        if (str[*start + 1] == 'x') {
            sscanf(str + *start, "%4x%n", &c, &n); // NOLINT
        } else {
            sscanf(str + *start, "%4o%n", &c, &n); // NOLINT
        }
    } else {
        switch (str[*start]) {
        case 'n':
            c = '\n';
            break;
        case 'b':
            c = '\b';
            break;
        case 't':
            c = '\t';
            break;
        case 'a':
            c = '\a';
            break;
        case 'r':
            c = '\r';
            break;
        case 'v':
            c = '\v';
            break;
        case 'f':
            c = '\f';
            break;
        default :
            c = str[*start];
            break;
        }
    }
    *start = *start + n;
    return c;
}

char* StrEsc(char* dest, char* src) {
    char* dest_orig = dest;
    bool escape = false;
    int pos = 0;
    while (*src) {
        if (escape) {
            pos = 0;
            *dest++ = GetChangeChar(src, &pos);
            src += pos;
            escape = false;
        } else if (*src == '\\') {
            escape = true;
            src++;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = 0;
    return dest_orig;
}

static string StringTrim(string str) {
    boost::trim(str);
    return str;
}


const char* StreamingTask::kTag = Task::kStreamingKVTagName;
const char* StreamingTask::kContentStop = "\n";
const char* StreamingTask::kDefaultTagStop = "\t\t";
const char* StreamingTask::kDefaultKeyStop = "\t";

StreamingTask::StreamingTask(string end_of_tag, string end_of_key,
                             Hash* hash, int buf_size, bool with_tag, bool with_logging):
    m_stop(false),
    m_content_stop(kContentStop),
    m_tag_stop(end_of_tag),
    m_key_stop(end_of_key),
    m_hash(hash),
    m_buf_size(buf_size),
    m_with_tag(with_tag),
    m_with_logging(with_logging) {
    RegisterTupleTypeInfo(StreamingTask::kTag, new StreamingTupleInfo());
    m_out_buf = new(std::nothrow) char[buf_size];
    m_in_buf = new(std::nothrow) char[buf_size];
    assert(m_in_buf);
    assert(m_out_buf);

    // create a new pgroup, kill pgroup while terminating
    setpgid(getpid(), getpid());
    SetSignalHandler();
    StartThreads();  // start thread to read user's stdout and stderr
}

void StreamingTask::OnTuples(const Tuples& tuples) {
    for (size_t i = 0; i < tuples.size(); i++) {
        const StreamingTuple* tuple = dynamic_cast<const StreamingTuple*>(tuples[i]); // NOLINT
        if (m_with_tag) {
            snprintf(m_in_buf, kDefaultBufSize, "%s%s%s%s%s%s",
                    tuple->tag().c_str(), m_tag_stop.c_str(),
                    tuple->key().c_str(), m_key_stop.c_str(),
                    tuple->value().c_str(), m_content_stop.c_str());
        } else {
            snprintf(m_in_buf, kDefaultBufSize, "%s%s%s%s",
                    tuple->key().c_str(), m_key_stop.c_str(),
                    tuple->value().c_str(), m_content_stop.c_str());
        }

        DSTREAM_DEBUG("flush to user scirpt:%s", m_in_buf);
        if ((fprintf(m_child_stdin, "%s", m_in_buf) < 0) || (fflush(m_child_stdin) < 0)) {
            DSTREAM_WARN("OnSpout Error: %s", strerror(errno));
            m_stop = true;
            break;
        }
    }
    if (m_stop) {
        exit(0);
    }
}

void StreamingTask::OnSpout(ReadableByteArray* bytes) {
    ByteSize size = 0;
    static uint64_t cnt = 0;
    DSTREAM_DEBUG("spout start:\tkey:%lu,value:%s", cnt,
            reinterpret_cast<const char*>(bytes->Data(&size)));
    if (m_with_tag) {
        snprintf(m_in_buf, kDefaultBufSize, "%s%s%lu%s%s%s",
                kTag, m_tag_stop.c_str(), cnt++, m_key_stop.c_str(), (char*)bytes->Data(&size), m_content_stop.c_str());
    } else {
        snprintf(m_in_buf, kDefaultBufSize, "%lu%s%s%s",
                cnt++, m_key_stop.c_str(), (char*)bytes->Data(&size), m_content_stop.c_str());
    }
    fprintf(m_child_stdin, "%s", m_in_buf);
    fflush(m_child_stdin);
    DSTREAM_DEBUG("flush to user script: %s", m_in_buf);
    if (m_stop) {
        exit(0);
    }
}

void StreamingTask::StartProcess() {
    m_child_stdin  = NULL;
    m_child_stdout = NULL;
    m_child_stderr = NULL;
    s_child_pid = RedirectStdInOutErr(g_stream_exec_name.c_str(), m_child_stdin,
                                      m_child_stdout, m_child_stderr);
    if ( s_child_pid == 0 ) {
        DSTREAM_ERROR("error in function RedirectStdInOutErr for %s\n", g_stream_exec_name.c_str());
        exit(-1);
    } else {
        waitpid(s_child_pid, NULL, 0);
        m_stop = true;
        DSTREAM_ERROR("[ERROR] user process exit!! \n");
        // to avoid main thread not exit in the case no tuple/log coming,
        // call exit() any way in 2s
        usleep(2000000);
        exit(0);
    }
}

void StreamingTask::StartThreads() {
    int errcode = 0;
    // start user process
    errcode = pthread_create(&m_user_process_handler_thread, NULL, StartProcessThread, this);
    if (errcode != 0) {
        DSTREAM_ERROR("create thread failed with error code :[%d]\n", errcode);
        exit(-1);
    }
    usleep(2000000); // wait 2 seconds for subprocess start
    errcode = pthread_create(&m_stdout_capture_thread, NULL, ReadStdoutThread, this);
    if (errcode != 0) {
        DSTREAM_ERROR("create thread failed with error code :[%d]\n", errcode);
        exit(-1);
    }
    errcode = pthread_create(&m_stderr_capture_thread, NULL, ReadStderrThread, this);
    if (errcode != 0) {
        DSTREAM_ERROR("create thread failed with error code :[%d]\n", errcode);
        exit(-1);
    }
}

void StreamingTask::StopThreads() {
    if (pthread_join(m_stdout_capture_thread, NULL) != 0) {
        DSTREAM_ERROR("join output_thread failed\n");
    }
    if (pthread_join(m_stderr_capture_thread, NULL) != 0) {
        DSTREAM_ERROR("join stderr_thread failed\n");
    }
    if (pthread_join(m_user_process_handler_thread, NULL) != 0) {
        DSTREAM_ERROR("join stderr_thread failed\n");
    }
}

void StreamingTask::ReadStdErr() {
    char c = -1;
    while (!m_stop) {
        c = fgetc(m_child_stderr);
        if ( c == EOF || c == '\0' ) {
            m_stop = true;
            break;
        }
        putchar(c);
        if (c == '\n') {
            fflush(stdout);
        }
    }
    fclose(m_child_stderr);
}

void StreamingTask::ReadStdout() {
    while (!m_stop) {
        if (GetFileStream(m_out_buf, m_buf_size, m_child_stdout, m_content_stop.c_str()) == NULL) {
            m_stop = true;
            break;
        }
        DSTREAM_DEBUG("receive original line: %s", m_out_buf);
        // trim line
        std::string line = StringTrim(m_out_buf);
        if (line.empty()) {
            DSTREAM_INFO("receive stream empty line, ignore and continue;");
            continue;
        }
        DSTREAM_DEBUG("receive user script stdout(len=%zd):%s|||", line.size(), line.data());

        StreamingTuple* o_tuple = dynamic_cast<StreamingTuple*>(EmitWithLock(kTag)); // NOLINT
        string::size_type pos = string::npos;
        if (m_with_tag) {
            if ((pos = line.find(m_tag_stop)) == string::npos) {      // no tag
                o_tuple->set_tag(kTag);
            } else {
                o_tuple->set_tag(line.substr(0, pos));
                line = line.substr(pos + m_tag_stop.size());
            }
        }

        static int cnt = 0;
        if ((pos = line.find(m_key_stop)) == string::npos) {     // no key
            o_tuple->set_key(Convert<string>(cnt++));
            o_tuple->set_value(line);
        } else {
            o_tuple->set_key(line.substr(0, pos));
            o_tuple->set_value(line.substr(pos + m_key_stop.size()));
        }
        uint64_t hash_code = m_hash->HashCode(o_tuple->key().c_str());
        o_tuple->set_hash_code(hash_code);
        DSTREAM_DEBUG("send a tuple to downstream T[%s]K[%s]V[%s],hash_code[%lu]",
                o_tuple->tag().c_str(), o_tuple->key().c_str(),
                o_tuple->value().c_str(), o_tuple->hash_code());
        emitUnlock();
    }
    DSTREAM_WARN("%s", "User program exited ?? This shouldn't have happened");
    if (m_out_buf != NULL) {
        delete[]m_out_buf;
        m_out_buf = NULL;
    }
}

string g_end_of_tag = StreamingTask::kDefaultTagStop; // NOLINT
string g_end_of_key = StreamingTask::kDefaultKeyStop; // NOLINT
bool g_with_tag = false;
bool g_with_logging = false;
int g_pipe_buffer_size = StreamingTask::kDefaultBufSize;
Hash* g_hash_to_use       = NULL;

void PrintVersion(const char* proc) {
    fprintf(stderr, "[%s]: --version: %s, build time: %s\n",
            proc, VERSION, BUILD_TIME);
    exit(-2);
}

void ParseArgs(int argc, char** argv) {
    g_excutive_name = argv[0];
    const char short_opt[] = "t:k:s:p:l:Tv";
    struct option long_opt[] = {
        {"end-of-tag", 1, NULL, 't'},
        {"end-of-key", 1, NULL, 'k'},
        {"hash-exec-shell", 1, NULL, 's'},
        {"pipe-buffer-size", 1, NULL, 'p'},
        {"with-tag", 0, NULL, 'T'},
        {"spout-limit", 1, NULL, 'l'},
        {"version", 0, NULL, 'v'},
        {"with-logging", 0, NULL, 'L'},
        {0, 0, 0, 0}
    };

    char* spout_limit = NULL;
    string hash_exec  = "";
    int c;
    while ((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1) {
        switch (c) {
        case 't':
            g_end_of_tag = StrEsc(optarg, optarg);
            break;
        case 'k':
            g_end_of_key = StrEsc(optarg, optarg);
            break;
        case 's':
            hash_exec  = optarg;
            break;
        case 'p':
            g_pipe_buffer_size = atoi(optarg);
            break;
        case 'T':
            g_with_tag = true;
            break;
        case 'l':
            spout_limit = optarg;
            break;
        case 'L':
            g_with_logging = true;
            break;

        case 'v':
        default:
            PrintVersion(argv[0]);
            break;
        }
    }
    if (argv[optind] == NULL) {
        DSTREAM_ERROR("user shellcmd is needed by streaming in commandline");
        exit(-1);
    }

    if (spout_limit != NULL) {
        setenv(internal::Context::kEnvKeySpoutLimit, spout_limit, 1);
    }

    g_stream_exec_name = argv[optind];
    if (hash_exec != "") {
        g_hash_to_use = new HashFromShell(hash_exec);
    } else {
        g_hash_to_use = new ElfHash;
    }
}

} // namespace streaming
} // namespace dstream

