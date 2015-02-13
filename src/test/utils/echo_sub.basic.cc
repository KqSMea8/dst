#include "bigpipe_subscriber.h"
#include "bigpipe_api_factory.h"
#include "common_log.h"
#include "common_return_type.h"
#include "common_util.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

using namespace bigpipe;

/**
 * @brief 工具参数
 */
char pipe_name[128] = "";
int pipelet_num = 0;
int pipelet_id = -1;
int start_point = 0;
int peek_wait = 10;
int max_msg_count = -1;

/**
 * @brief 使用方法
 */
void usage()
{
    printf("\n\n./sub pipe_name:pipelet_num:pipelet_id:start_point:wait_time:max_msg_count\n");
    printf("==========================\n");
    printf("   pipelet_num : means the total pipelets in indicated pipe\n");
    printf("    pipelet_id : if set to -1 means consume all\n");
    printf(" max_msg_count : if set to -1 means consume no stop\n");
    printf("   start_point : if set to -1 means newest\n");
    printf("     wait_time : the peek fail retry time [ ms]\n\n\n");
    exit(-1);
}

/**
 * @brief 订阅及打印
 */
# define BUFF_SIZE (2*1024*1024+1024)
int sub_message(bigpipe_subscriber_t& subscriber, const char* pipe, uint32_t pipelet_id,
                int64_t start_point, int32_t peek_wait, int64_t max_count)
{
    int ret = BIGPIPE_E_OK;

    char message[BUFF_SIZE];
    uint32_t msg_len;

    char path[1024];
    sprintf(path, "%s_%d.txt", pipe, pipelet_id);

    FILE* fp = fopen(path, "w+");
    if (NULL == fp)
    {
        printf("open file (%s) failed\n", path);
        return -1;
    }

    ret = subscriber.init(pipe, pipelet_id, start_point, "./conf", "sub.conf");
    if (BIGPIPE_E_OK != ret)
    {
        printf("subscriber init failed [pipelet_id : %d]\n", pipelet_id);
        return -1;
    }
    printf("subscriber init success [pipelet_id : %d]\n", pipelet_id);

    ret = subscriber.connect();
    if (BIGPIPE_E_OK != ret)
    {
        printf("subscriber connect failed [pipelet_id : %d]\n", pipelet_id);
        return -1;
    }
    printf("subscriber connect success [pipelet_id : %d]\n", pipelet_id);

    bool stop_flag = false;
    int32_t msg_count = 0;
    int32_t timeo_counter = 0;
    int32_t peek_timeo = 10; // ms
    int32_t total_timeo = peek_wait * 1000 / peek_timeo; // 计算 peek retry 次数
    uint64_t cur_msg_id = 0;
    uint64_t cur_seq_id = 0;

    while (false == stop_flag)
    {
        ret = subscriber.peek(peek_timeo);
        if (BIGPIPE_E_READABLE == ret)
        {
            ret = subscriber.receive(message, BUFF_SIZE, msg_len, cur_msg_id, cur_seq_id);
            if (BIGPIPE_E_OK != ret)
            {
                BIGPIPE_LOG_FATAL("[sub] receive failed [ pipelet_id : %d]\n", pipelet_id);
                stop_flag = true;
            }
            else
            {
                // message formet is
                // pipelet_id:sequence_num:send_time:magic:receive_time:delay

                char* temp_str = message;
                struct timeval cur_time;
                gettimeofday(&cur_time, NULL);
                uint32_t micro_sec = (cur_time.tv_sec % (3600 * 24)) * 1000 + cur_time.tv_usec / 1000; // ms
                uint32_t send_time;
                sscanf(message, "%*d:%*d:%u", &send_time);

                for (int i = 0, j = 0; j < 4; i++) if (':' == *temp_str++) {
                        j++;    /*find the formet end.*/
                    }
                // append information in revceiver
                sprintf(temp_str, "%u:%u <%lu: %lu>", micro_sec, (86400000 + micro_sec - send_time) % 86400000,
                        cur_msg_id, cur_seq_id);
                //fwrite(message, strlen(message), 1, fp);
                fprintf(fp, "%s\n", message);
                timeo_counter = 0;
                msg_count++;
                if (msg_count == max_count)
                {
                    stop_flag = true;
                    BIGPIPE_LOG_WARNING("[sub] sub tool stop on max_msg_count [ pipelet_id : %d]\n", pipelet_id);
                }
            }
        }
        else if (BIGPIPE_E_UNREADABLE == ret)
        {
            timeo_counter++;
            if (timeo_counter >= total_timeo)
            {
                stop_flag = true;
                BIGPIPE_LOG_WARNING("[sub] sub tool stop on peek_wait_timeout [ pipelet_id : %d]\n", pipelet_id);
            }
            else
            {
                BIGPIPE_LOG_FATAL("[sub] peek BIGPIPE_E_UNREADABLE, retry [%d] [ pipelet_id : %d]\n", timeo_counter,
                                  pipelet_id);
            }
        }
        else
        {
            stop_flag = true;
        }
    }

    ret = subscriber.disconnect();
    if (BIGPIPE_E_OK != ret)
    {
        BIGPIPE_LOG_FATAL("[sub] disconnect fail [ pipelet_id : %d]\n", pipelet_id);
        return -1;
    }

    fclose(fp);

    return 0;
}

/**
 * @brief revceive pipelet
 */
void* receive_work(void* args)
{
    int pipeletId = (int)(long)(args);
    bigpipe_subscriber_t subscriber;

    int ret = sub_message(subscriber, pipe_name, pipeletId, start_point, peek_wait, max_msg_count);
    if (BIGPIPE_E_OK == ret)
    {
        printf("sub message succeed [pipelet_id : %d]\n", pipeletId);
    }
    else
    {
        BIGPIPE_LOG_FATAL("sub message fail[pipelet_id : %d]\n", pipeletId);
    }

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

    // 打印用法
    if (2 != argc)
    {
        usage();
    }

    // 加载配置
    BIGPIPE_LOAD_LOG("./conf", "sub.conf");

    // 加载工具参数
    for (int i = 0; i < strlen(argv[1]); i++)
    {
        if (':' == argv[1][i]) {
            argv[1][i] = ' ';
        }
    }
    printf("%s\n", argv[1]);
    sscanf(argv[1], "%s%d%d%d%d%d", pipe_name, &pipelet_num, &pipelet_id, &start_point, &peek_wait,
           &max_msg_count);
    printf("pipe_name: %s\n", pipe_name);
    printf("pipelet_num: %d\n", pipelet_num);
    printf("pipelet_id: %d\n", pipelet_id);
    printf("start_point: %d\n", start_point);
    printf("peek_wait: %d\n", peek_wait);
    printf("max_msg_count: %d\n", max_msg_count);
    printf("===================================================> let's go!!!\n\n\n");

    // 开始
    pthread_t work_thread[PTHREAD_THREADS_MAX];
    if ( -1 == pipelet_id )
    {
        for (int i = 0; i < pipelet_num; ++i)
        {
            printf("start piplelet-%d receive thread ...\n", i);
            pthread_create(work_thread + i, NULL, receive_work, (void*)(long)i);
            printf("start piplelet-%d receive thread (done)\n", i);
        }
        for (int i = 0; i < pipelet_num; ++i)
        {
            pthread_join(work_thread[i], NULL);
            printf("piplelet-%d receive thread (end)\n", i);
        }
    }
    else
    {
        receive_work((void*)(long)pipelet_id);
    }

    BIGPIPE_CLOSE_LOG();

    return 0;
}
