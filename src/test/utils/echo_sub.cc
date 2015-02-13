#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include "common/error.h"
#include "common/commit_log.h"

using namespace std;
using namespace dstream;
/**
 * @brief 工具参数
 */
char pipe_name[256] = "";
char output_file_name[256] = "";
int pipelet_num = 1;
int pipelet_id = 0;
int start_point = 1;
int peek_wait = 10000;
int max_msg_count = -1;


/**
 * @brief 使用方法
 */
void usage()
{
    printf("\n\n./sub pipe_name:pipelet_num:pipelet_id:output_file_name:max_msg_count\n");
    printf("==========================\n");
    printf("   pipe_name : bigpipe name\n");
    printf("   pipelet_num : means the total pipelets in indicated pipe\n");
    printf("   pipelet_id : means the pipelet num\n");
    printf("   start_point : sub point (>= 1)\n");
    printf("   output_file_name : output commitlog file name prefix\n");
    printf("   max_msg_count : read total msg num from bigpipe\n");
    exit(-1);
}

/**
 * @brief 订阅及打印
 */
#define BUFF_SIZE (2*1024*1024+1024)
/**
 * @brief revceive pipelet
 */
void* receive_work(void* args)
{
    int ret = 0;
    int pipeletId = *reinterpret_cast<int*>(args);

    char message[BUFF_SIZE];
    memset(message, '#', sizeof(message));

    CommitLog* log = (CommitLog*)new BigPipeCommitLog(
                         CommitLog::kSubscriber,
                         pipe_name,
                         "./conf", "pub.conf",
                         "stoken", NULL);

    sleep(1);
    // 初始化& 连接
    ret = log->Open();
    if (0 != ret) {
        printf("subscriber init & connect failed [pipelet_id : %d]\n", pipeletId);
        return reinterpret_cast<void*>(ret);
    }
    printf("subscriber init & connect success [pipelet_id : %d]\n", pipeletId);

    string file_name(output_file_name);
    file_name += "-";
    StringAppendNumber(&file_name, pipeletId);

    CommitLog* localfile_log = (CommitLog*)new LocalFileCommitLog(
                                   LocalFileCommitLog::kPublisher, file_name);

    ret = localfile_log->Open();
    if (0 != ret) {
        printf("file publisher init & connect failed(%d)\n", ret);
        return reinterpret_cast<void*>(ret);
    }
    printf("file publisher init & connect success\n");

    int count = 0;
    LogInfo log_info;
    while (ret == 0) {
        if (max_msg_count > 0) {
            if (count >= max_msg_count) {
                break;
            }
        }
        ret = log->Read(&log_info);
        if (ret != 0) {
            goto ERROR;
        }
        ByteSize size = 0;
        const dstream::Byte* data = log_info.byte_array.Data(&size);
        if (size <= 0) {
            goto ERROR;
        }
        ReadableByteArray log_rbuf(data, size);
        if (localfile_log->Write(&log_rbuf, NULL, NULL) != 0) {
            goto ERROR;
        }
        free(const_cast<dstream::Byte*>(data));
        count++;
    }
ERROR:
    localfile_log->Close();
    log->Close();
    return 0;
}

/**
 * @brief 入口
 */
int main(int argc, char* argv[])
{
    ::dstream::logger::initialize("echo sub");
    // 忽略SIGPIPE信号
    struct sigaction action;
    action.sa_handler = SIG_IGN;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);

    // 打印用法
    if (2 != argc) {
        usage();
    }

    // 加载配置

    // 加载工具参数
    for (unsigned int i = 0; i < strlen(argv[1]); i++) {
        if (':' == argv[1][i]) {
            argv[1][i] = ' ';
        }
    }
    printf("%s\n", argv[1]);
    sscanf(argv[1], "%s%d%d%d%s%d", pipe_name,
           &pipelet_num, &pipelet_id, &start_point,
           output_file_name, &max_msg_count);
    printf("pipe_name: %s\n", pipe_name);
    printf("pipelet_num: %d\n", pipelet_num);
    printf("pipelet_id: %d\n", pipelet_id);
    printf("start_point: %d\n", start_point);
    printf("output_file_name: %s\n", output_file_name);
    printf("max_msg_count: %d\n", max_msg_count);
    printf("===================================================> let's go!!!\n\n\n");

    // 开始
    pthread_t work_thread[PTHREAD_THREADS_MAX];
    if ( -1 == pipelet_id ) {
        for (int i = 0; i < pipelet_num; ++i) {
            printf("start piplelet-%d receive thread ...\n", i);
            pthread_create(work_thread + i, NULL, receive_work, (void*)(long)i);
            printf("start piplelet-%d receive thread (done)\n", i);
        }

        for (int i = 0; i < pipelet_num; ++i) {
            pthread_join(work_thread[i], NULL);
            printf("piplelet-%d receive thread (end)\n", i);
        }
    } else {
        receive_work(&pipelet_id);
    }


    return 0;
}
