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

#include <sys/time.h>
#include "common/rpc.h"
#include "rpc_hello.pb.h"
#include "common/proto_rpc.h"

using namespace dstream;

// 1024 x
#define ECHO_BUF_MAX_SIZE (1024*10)
static char kContent[ECHO_BUF_MAX_SIZE] = "";

// ------------------------------------------------------------
// unit is ms.
static int kCounterUnit = 10000;
inline double gettime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec * 0.001;
}

std::string addr("tcp://127.0.0.1:19870");

void print_usage(const char* program) {
    printf("Usage example: %s -t 8 -s 1024 -h tcp://10.237.22.17:19870\n", program);
    printf("'-t' is the client threads, '-s' is the echo buff size, '-h' is the server addr\n");
    return;
}

void* client_routine(void* arg) {
    unsigned long thread_id = (unsigned long)arg;
    char log_file[32] = "";
    sprintf(log_file, "rpc_benchmark_%lu", thread_id);
    logger::initialize(log_file);
    rpc::Context::init();
    rpc::Connection conn;
    DSTREAM_NOTICE("connect %s\n", addr.c_str());
    if (conn.Connect(addr.c_str()) < 0) {
        return NULL;
    }
    size_t counter = 0;
    double _whole_times = 0.0;
    double _period_start = 0.0;
    _period_start = gettime();
    while (1) {
        proto_rpc::RPCRequest<EchoRequest> req;
        proto_rpc::RPCResponse<EchoResponse> res;
        req.set_method("echo");
        req.set_field(kContent);
        // no timeout.
        if (conn.Call(&req, &res, -1) < 0) {
            DSTREAM_WARNING("call failed, reconnect...");
            conn.Reconnect();
            return NULL;
        }
        counter++;
        if ((counter % kCounterUnit) == 0) {
            double _period_end = gettime();
            _whole_times += _period_end - _period_start;
            DSTREAM_NOTICE("last %6d calls cost %.3lf ms, average %.3lf ms, whole %8zu calls cost %.3lf, average %.3lf ms",
                           kCounterUnit, (_period_end - _period_start), (_period_end - _period_start) / kCounterUnit,
                           counter, _whole_times, _whole_times / counter);
            _period_start = _period_end;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    int ret = 0;
    int client_threads = 1;
    int echo_str_len = 1024;
    memset(kContent, 'x', ECHO_BUF_MAX_SIZE);

    // get options
    int opt;
    opterr = 0;
    while ((opt = getopt(argc, argv, "h:t:s:")) != -1) {
        switch (opt) {
        case 'h':
            addr = optarg;
            break;
        case 't':
            client_threads = atoi(optarg);
            break;
        case 's':
            echo_str_len = atoi(optarg);
            break;
        default:
            print_usage(argv[0]);
            return 1;
        }
    }
    printf(" ====> echo str len = %s\n", addr.c_str());
    printf(" ====> client threads = %d\n", client_threads);
    printf(" ====> echo str len = %d\n", echo_str_len);
    kContent[echo_str_len] = (char)0;

    pthread_t _tid[client_threads];
    for (int i = 0; i < client_threads; i++) {
        ret = pthread_create(&_tid[i], NULL, client_routine, (void*)(unsigned long)i);
        if (0 != ret) {
            printf("create client thread failed, ret=[%d]\n", ret);
            return 1;
        }
    }
    for (int i = 0; i < client_threads; i++) {
        pthread_join(_tid[i], NULL);
    }

    return 0;
}

