#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "common/error.h"
#include "common/commit_log.h"

using namespace dstream;

/**
 * @brief 工具参数
 */
char producer_type[16] = "pub"; // pub means sync, apub means async
char pipe_name[128] = "";
int pipelet_num = 1;
int pipelet_id = 0;
int msg_cnt = -1;
int msg_size_max = 100;
int msg_size_min = 100;
int thread_mode = 1; // 1 mans multi-thread, 0 means single
char magic_str[128] = "";

/**
 * @brief 使用方法
 */
void usage()
{
    printf("\n============================\n");
    printf("./pub pipe_name:pipelet_num:pipelet_id:msg_size:msg_cnt\n");
    printf("============================\n\n");
    printf("    msg_size : formet is : a-b, if a != b, will generate random size between a-b\n");
    printf("    msg_cnt : how many lines to send, -1 means forever\n");
    exit(-1);
}

/**
 * @brief send pipelet
 */
void* send_work(void* args)
{
    int ret = error::OK;
    int pipeletId = (int)(long)(args);

    char message[1024], context[1024];

    CommitLog* pub_log = new BigPipeCommitLog(BigPipeCommitLog::kPublisher,
                                              pipe_name, "./conf", "pub.conf", "_default_token_", NULL);

    sleep(1);
    printf("start send work on pipelet-%d\n", pipeletId);

    // 初始化& 连接
    ret = pub_log->Open();
    if (0 != ret) {
        printf("publisher init & connect failed [pipelet_id : %d]\n", pipeletId);
        return 0;
    }
    printf("publisher init & connect success [pipelet_id : %d]\n", pipeletId);

    bool stop_flag = false;

    printf(" in sending ... [pipelet_id : %d]\n", pipeletId);
    for ( int sequence = 1; stop_flag != true; sequence++ )
    {
        for (int id = 0; id < pipelet_num; id++)
        {
            char pipelet_char = 65 + id;
            memset(context, (int)pipelet_char, 1000);


            //sprintf(message, "%d:%d:%u:%s:",id,sequence,micro_sec,magic_str);
            sprintf(message, "%d:%s", sequence, context);
            //message[strlen[message]] = '#';
            printf(" PUBLISH------------------------------ pipelet: %d, sequence: %d, message: %s\n", id,
                   sequence, message);
            ReadableByteArray log_rbuf((const dstream::Byte*)message, sizeof(message));
            ret = pub_log->Write(&log_rbuf, NULL, NULL);
            if (0 != ret)
            {
                printf("[pub] publisher send failed [pipelet_id : %d]\n", id);
                stop_flag = true;
                continue;
            }
        }
        msg_cnt ++;
        if ( msg_cnt == sequence) {
            stop_flag = true;
            printf("[pub] publisher reaches max msg_num [pipelet_id : %d]\n", pipeletId);
        }
    }

    ret = pub_log->Close();
    if (0 != ret) {
        printf("[pub] close failed [pipelet_id : %d]\n", pipeletId);
        return 0;
    }
    printf("close success [pipelet_id : %d]\n", pipeletId);

    return 0;
}

/**
 * @brief 入口
 */
int main(int argc, char* argv[])
{
    // 忽略SIGPIPE信号
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
    ::dstream::logger::initialize("echo_pub");

    // 打印用法
    if (2 != argc)
    {
        usage();
    }

    // 解析工具参数
    char temp_str[64] = "";
    for (unsigned int i = 0; i < strlen(argv[1]); i++)
    {
        if (':' == argv[1][i]) {
            argv[1][i] = ' ';
        }
    }
    printf("%s\n", argv[1]);
    sscanf(argv[1], "%s%d%d%s%d", pipe_name, &pipelet_num, &pipelet_id, temp_str, &msg_cnt);
    sscanf(temp_str, "%d-%d", &msg_size_min, &msg_size_max);
    printf("producer_type: %s\n", producer_type);
    printf("pipe_name: %s\n", pipe_name);
    printf("pipelet_num: %d\n", pipelet_num);
    printf("pipelet_id: %d\n", pipelet_id);
    printf("msg_size_min: %d\n", msg_size_min);
    printf("msg_size_max: %d\n", msg_size_max);
    printf("thread_mode: %d\n", thread_mode);
    printf("magic_str: %s\n\n", magic_str);
    printf("===================================================> let's go!!!\n\n\n");

    // 开始
    pthread_t work_thread[PTHREAD_THREADS_MAX];
    if ( 1 == thread_mode ) {
        if ( -1 == pipelet_id ) {
            for (int i = 0; i < pipelet_num; ++i)
            {
                printf("start piplelet-%d send thread ...\n", i);
                pthread_create(work_thread + i, NULL, send_work, (void*)(long)i);
                printf("start piplelet-%d send thread (done)\n", i);
            }
            for (int i = 0; i < pipelet_num; ++i)
            {
                pthread_join(work_thread[i], NULL);
                printf("piplelet-%d send thread (end)\n", i);
            }
        } else {
            send_work((void*)(long)pipelet_id);
        }
    } else {
        send_work((void*)(long)pipelet_id);
    }

    return 0;
}
