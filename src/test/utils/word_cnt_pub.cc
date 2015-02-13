#include "bigpipe_publisher.h"
#include "bigpipe_async_publisher.h"
#include "bigpipe_api_factory.h"
#include "common_log.h"
#include "common_return_type.h"
#include "common_util.h"
#include "common_sleeper.h"
#include <pthread.h>
#include <time.h>
#include <stdlib.h>

using namespace bigpipe;
using namespace std;

/**
 * @brief 工具参数
 */
char producer_type[16] = ""; // pub means sync, apub means async
char pipe_name[128] = "";
int pipelet_num = 0;
int pipelet_id = -1;
int last_time = -1; // s

/**
 * @brief 使用方法
 */
void usage()
{
    printf("\n============================\n");
    printf("./word_cnt_pub send_type:pipe_name:pipelet_num:pipelet_id:last_time:words_in_dict:sentence_len\n");
    printf("============================\n\n");

    printf("     send_type : pub means sync publisher; apub means aysnc\n");
    printf("   pipelet_num : the total pipelets in indicated pipe\n");
    printf("    pipelet_id : if set to -1, send msgs to all pipelets\n");
    printf("     last_time : (the publish last time : s) if last_time set to -1, the sending won't stop\n");
    printf(" words_in_dict : set a number >= 1, the word number in dict\n");
    printf("  sentence_len : set a number >= 1, the max word number in onesentence\n");
    printf("\n\n\n=======example as follow=========\n");
    printf(" ./word_cnt_pub pub:pipe1:10:-1:300:2:10\n");
    printf(" ./word_cnt_pub apub:pipe1:2:0:600:100:20\n");
    exit(-1);
}

unsigned int get_random_num(int min, int max)  //  min <= return number << max
{
    static bool init_flag = false;
    if (!init_flag)
    {
        srand((unsigned int)time(NULL));
        init_flag = true;
    }
    if (max != min)
    {
        min += rand() % (max - min);
    }
    return min;
}

int MAX_SENTENCE_LEN = 24; // words number
int MAX_WORD_Type = 100;
char** selected_words;
void init_selected_words()
{
    selected_words = new char *[MAX_WORD_Type];
    cout << "word dicts \n=========\n";
    for (int i = 0; i < MAX_WORD_Type; i++)
    {
        int size = get_random_num(3, 8); // words len between 3~8
        selected_words[i] = new char[size + 1];
        for (int j = 0; j < size; j++)
        {
            selected_words[i][j] = char((int)'a' + get_random_num(0, 26));
        }
        selected_words[i][size] = 0;
        cout << selected_words[i] << " ";
    }
    cout << "\n===========\n\n";
}
void free_selected_words()
{
    for (int i = 0; i < MAX_WORD_Type; i++)
    {
        free(selected_words[i]);
    }
    free(selected_words);
}
std::map<string, int> word_cnt;
void generate_sentence(std::string& sentence)
{
    sentence.clear();
    unsigned int word_num = get_random_num(1, MAX_SENTENCE_LEN);
    for (int i = 0; i < (int)word_num; i++)
    {
        int word_index = get_random_num(0, MAX_WORD_Type);
        //printf(" -- add one word: %s\n", selected_words[word_index]);
        std::map<string, int>::iterator it = word_cnt.find(selected_words[word_index]);
        if (it == word_cnt.end())
        {
            word_cnt.insert(map<std::string, int>::value_type(selected_words[word_index], 1));
        }
        else
        {
            word_cnt[selected_words[word_index]]++;
        }
        sentence.append(selected_words[word_index]);
        if ((i + 1) != (int)word_num) {
            sentence.append(" ");
        }
    }
    printf(" -- sending sentence: %s, len: %d\n", sentence.c_str(), sentence.size());
}
void dump_word_cnt()
{
    std::map<string, int>::iterator it;
    cout << "========word_cnt=======\n";
    for (it = word_cnt.begin(); it != word_cnt.end(); it++)
    {
        cout << it->first << ":" << it->second << endl;
    }
}

/**
 * @brief send pipelet
 */
void* send_work(void* args)
{
    int ret = BIGPIPE_E_OK;
    int pipeletId = (int)(long)(args);

    // progress 和 partitioner
    bigpipe_progress_t prog;
    bigpipe_partitioner_t pt;

    bigpipe_publisher_t publisher;
    bigpipe_async_publisher_t async_publisher;
    bigpipe_publisher_t* pub = 0;

    // 使用哪种发送方式
    if (0 == strcmp("pub", producer_type))
    {
        pub = &publisher;
    }
    else if (0 == strcmp("apub", producer_type))
    {
        pub = &async_publisher;
    }
    else
    {
        usage();
    }

    sleep(1);
    printf("start send work on pipelet-%d\n", pipeletId);

    // 初始化
    ret = publisher.init(pipe_name, &pt /* partition */, "./conf", "pub.conf");
    if (BIGPIPE_E_OK != ret)
    {
        printf("publisher init failed [pipelet_id : %d]\n", pipeletId);
        return 0;
    }
    printf("publisher init success [pipelet_id : %d]\n", pipeletId);

    // 连接
    ret = publisher.connect();
    if (BIGPIPE_E_OK != ret)
    {
        printf("publisher connect failed [pipelet_id : %d]\n", pipeletId);
        return 0;
    }
    printf("publisher connect success [pipelet_id : %d]\n", pipeletId);

    bool stop_flag = false;
    int max_last_time_ms = last_time * 1000;
    int last_time_ms = 0;

    printf(" in sending ... [pipelet_id : %d]\n", pipeletId);
    for ( int sequence = 1; stop_flag != true; sequence++ )
    {
        std::string message;
        generate_sentence(message);
        if (-1 == pipeletId)
        {
            for (int id = 0; id < pipelet_num; id++)
            {
                ret = publisher.batch_send_one(id, message.c_str(), message.size(), NULL);
                if (BIGPIPE_E_OK != ret)
                {
                    BIGPIPE_LOG_FATAL("[pub] publisher send failed [pipelet_id : %d]\n", id);
                    stop_flag = true;
                    continue;
                }
            }
        }
        else
        {
            ret = publisher.batch_send_one(pipeletId, message.c_str(), message.size(), NULL);
            if (BIGPIPE_E_OK != ret)
            {
                BIGPIPE_LOG_FATAL("[pub] publisher send failed [pipelet_id : %d]\n", pipeletId);
                stop_flag = true;
                continue;
            }
        }
        usleep(1000);
        last_time_ms = last_time_ms + 1000;
        printf(" past time: %d s\n", last_time_ms);
        if (last_time_ms == max_last_time_ms)
        {
            sleep(2);
            ret = publisher.batch_send_one(pipeletId, "EOF", 3, NULL);
            if (BIGPIPE_E_OK != ret)
            {
                BIGPIPE_LOG_FATAL("[pub] publisher send failed [pipelet_id : %d]\n", pipeletId);
                stop_flag = true;
                continue;
            }
            stop_flag = true;
            BIGPIPE_LOG_NOTICE("[pub] publisher reaches max last_time [pipelet_id : %d]\n", pipeletId);
        }
    }

    ret =  publisher.disconnect();
    if (BIGPIPE_E_OK != ret)
    {
        BIGPIPE_LOG_FATAL("[pub] disconnect failed [pipelet_id : %d]\n", pipeletId);
        return 0;
    }
    printf("disconnect success [pipelet_id : %d]\n", pipeletId);

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
    BIGPIPE_LOAD_LOG("./conf", "pub.conf");

    // 解析工具参数
    char temp_str[64] = "";
    for (int i = 0; i < (int)strlen(argv[1]); i++)
    {
        if (':' == argv[1][i]) {
            argv[1][i] = ' ';
        }
    }
    printf("%s\n", argv[1]);
    sscanf(argv[1], "%s%s%d%d%d%d%d", producer_type, pipe_name, &pipelet_num, &pipelet_id, &last_time,
           &MAX_WORD_Type, &MAX_SENTENCE_LEN);
    printf("producer_type: %s\n", producer_type);
    printf("pipe_name: %s\n", pipe_name);
    printf("pipelet_num: %d\n", pipelet_num);
    printf("pipelet_id: %d\n", pipelet_id);
    printf("last_time: %d\n", last_time);
    printf("words in dict: %d\n", MAX_WORD_Type);
    printf("sentence len: %d\n", MAX_SENTENCE_LEN);
    printf("===================================================> let's go!!!\n\n\n");

    // 开始
    init_selected_words();
    send_work((void*)(long)pipelet_id);
    free_selected_words();
    dump_word_cnt();

    BIGPIPE_CLOSE_LOG();

    return 0;
}
