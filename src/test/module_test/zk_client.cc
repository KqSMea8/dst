/**
* @file     zk_client.cc
* @brief    a simple zookeepr client sample.
* @author   konghui, konghui@baidu.com
* @version  1.0.1.0
* @date   2011-12-02
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <common/error.h>
#include <common/zk_client.h>

using namespace dstream;

const int   kStatusDumpInterval  = 2;     /**< status dump interval (seconds) */
const int   kSleepInterval       = 2;    /**< sleep time (seconds) */
const int   kBufferLen           = 4096;  /**< data buf len*/
const char* kPnNodeRoot = "/dstream_kong/pn";
/**< the root path of pn node tree */
const char* kClearPath = "/dstream_kong";
int32_t     g_node_num = 50;

class EventWatcher: public zk_client::Watcher
{
    void OnEvent(int type, int state, const char* path)
    {
        printf("[ZKCLIENT_TESTER] get event: type:[%d], state:[%d], path:[%s]\n",
               type, state, path);

        // do some thing here
    }
};
EventWatcher g_event_watcher;

struct Foo
{
    int32_t a;
    int64_t b;
    char    c[2];
};

void print_usage(const char* program)
{
    printf("[ZKCLIENT_TESTER] Usage: %s -h 'host:ip[,host:ip,host:ip]'\n", program);
    return;
}

void print_status()
{
    printf("[ZKCLIENT_TESTER] STATUS:\n");
    // TODO
    // printf the node list
}

int32_t SysEnvReady(zk_client::ZkClient& zk_client,
                    const char* root, int* exist)
{
    int32_t ret = zk_client.NodeExists(root, exist, NULL);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to test node@[%s] existence\n", root);
        return ret;
    }
    return error::OK;
}

int32_t SetupSysEnv(zk_client::ZkClient& zk_client, const char* root)
{
    /* check if system environment is ready */
    int exist = 0;
    int ret;
    ret = SysEnvReady(zk_client, root, &exist);
    if (ret < error::OK)
    {
        return ret;
    }
    if (exist)
    {
        printf("[ZKCLIENT_TESTER] System evironment has been setup before\n");
        return error::OK;
    }

    /* setup */
    return zk_client.InitSystemRoot(root);
}

int32_t ClearSysEnv(zk_client::ZkClient& zk_client, const char* root)
{
    return zk_client.DeleteNodeTree(root);
}

void* registe_routine(void* arg)
{
    printf("[ZKCLIENT_TESTER] registe routine start...\n");

    char* zk_servers_list = static_cast<char*>(arg);
    zk_client::ZkClient zk_client(zk_servers_list);

    int32_t ret = error::OK;
    std::string path;
    std::vector<std::string> paths(g_node_num);

    /* connect to zookeepr */
    ret = zk_client.Connect();
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to connect to zookeepr servers:[%s]\n",
               zk_servers_list);
        return NULL;
    }
    printf("[ZKCLIENT_TESTER] connect to zookeepr servers:[%s] ...OK\n",
           zk_servers_list);

    /* setup system environment */
    ret = SetupSysEnv(zk_client, kPnNodeRoot);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to setup system environment at zookeepr"
               "servers:[%s]\n", zk_servers_list);
        return NULL;
    }
    printf("[ZKCLIENT_TESTER] setup sys env:[%s] ...OK\n", kPnNodeRoot);


    Foo foo;
    foo.a = 1234;
    foo.b = 12345678901ULL;
    foo.c[0] = 'A';
    foo.c[1] = 'B';

    zk_client::ZkNode node;
    node.data.m_buf = reinterpret_cast<char*>(&foo);
    node.data.m_len = sizeof(foo);
    node.path.m_buf = static_cast<char*>(calloc(1, kBufferLen));
    node.path.m_len = kBufferLen;
    if (NULL == node.path.m_buf)
    {
        printf("[ZKCLIENT_TESTER] fail to alloc memory\n");
        goto ERR_RET;
    }

    sleep(kSleepInterval); /* wait for watcher */

    /* registe some ephemeral nodes */
    path += kPnNodeRoot;
    path += "/pn_192.168.0.10";
    for (int32_t i = 0; i < g_node_num; i++)
    {
        node.path.m_buf[0] = '\0';
        node.path.m_len = kBufferLen;
        ret = zk_client.CreateEphemeralNode(path.c_str(), &node);
        if (ret < error::OK)
        {
            printf("[ZKCLIENT_TESTER] fail to create ephemeral node[%d]@[%s][%s]\n",
                   i, path.c_str(), zk_servers_list);
            goto ERR_RET;
        }
        printf("[ZKCLIENT_TESTER] create ephemeral node[%d]@[%s][%s] ...OK\n",
               i, node.path.m_buf, zk_servers_list);
        /* copy node's real path for deletion in later */
        paths[i] = node.path.m_buf;
    }

    /* sleep a min, let others know our existence */
    sleep(kSleepInterval);

    /* test data getter */
    Foo* pfoo;
    memset(node.data.m_buf, 0, node.data.m_len);
    ret = zk_client.GetNodeData(node.path.m_buf, &node.data);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to get data@node[%s]\n", node.path.m_buf);
        goto ERR_RET;
    }
    pfoo = reinterpret_cast<Foo*>(node.data.m_buf);
    printf("[ZKCLIENT_TESTER] get data[a=%d, b=%ld, c=%c%c]@node[%s] ...OK\n",
           pfoo->a, pfoo->b, pfoo->c[0], pfoo->c[1], node.path.m_buf);

    /* unregiste ephemeral node */
    for (int32_t i = 0; i < g_node_num; i++)
    {
        ret = zk_client.DeleteEphemeralNode(paths[i].c_str());
        if (ret < error::OK)
        {
            printf("[ZKCLIENT_TESTER] fail to delete ephemeral node[%d]@[%s][%s]\n",
                   i, paths[i].c_str(), zk_servers_list);
            goto ERR_RET;
        }
        printf("[ZKCLIENT_TESTER] delete ephemeral node[%d]@[%s][%s] ...OK\n",
               i, paths[i].c_str(), zk_servers_list);
    }
    printf("[ZKCLIENT_TESTER] delete ephemeral nodes@[%s] ...OK\n",
           zk_servers_list);

    /* clear system environment */
    ret = ClearSysEnv(zk_client, kClearPath);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to clear up system environment\n");
        goto ERR_RET;
    }
    printf("[ZKCLIENT_TESTER] clear up system environment ...OK\n");

ERR_RET:
    if (node.path.m_buf) free(node.path.m_buf);
    zk_client.Disconnect();
    return NULL;
}

void* watch_routine(void* arg)
{
    printf("[ZKCLIENT_TESTER] watch routine start...\n");

    char* zk_servers_list = static_cast<char*>(arg);
    zk_client::ZkClient zk_client(zk_servers_list);

    int32_t ret = error::OK;
    /* connect to zookeepr */
    if (zk_client.Connect() < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to connect to zookeepr servers:[%s]\n", zk_servers_list);
        return NULL;
    }
    printf("[ZKCLIENT_TESTER] connect to zookeepr servers:[%s] ...OK\n", zk_servers_list);

    /* wait system environment is ready */
    int exist = 0;
    do
    {
        ret = SysEnvReady(zk_client, kPnNodeRoot, &exist);
        if (ret < error::OK)
        {
            return NULL;
        }
    }
    while (!exist);

    /* registe an watcher */
    ret = zk_client.WatchChildrenDelete(kPnNodeRoot, &g_event_watcher);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to watch node:[%s]\n", kPnNodeRoot);
        return NULL;
    }
    printf("[ZKCLIENT_TESTER] watch node:[%s] ...OK\n", kPnNodeRoot);

    /* dump status periodically */
    int cnt = 0;
    while (true)
    {
        print_status();
        zk_client.DumpStatus();
        sleep(kSleepInterval);
        if (cnt > 3)
        {
            break;
        }
        cnt++;
    }

    /* unregiste watcher */
    ret = zk_client.UnWatchChildrenDelete(kPnNodeRoot);
    if (ret < error::OK)
    {
        printf("[ZKCLIENT_TESTER] fail to unwatch node:[%s]\n", kPnNodeRoot);
        return NULL;
    }
    printf("[ZKCLIENT_TESTER] unwatch node:[%s] for ...OK\n", kPnNodeRoot);

    print_status();
    zk_client.DumpStatus();

    zk_client.Disconnect();
    return NULL;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        print_usage(argv[0]);
        return 1;
    }

    int opt;
    opterr = 0;

    char* zk_servers_list = NULL;
    while ((opt = getopt(argc, argv, "h:n:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            zk_servers_list = optarg;
            break;
        case 'n':
            g_node_num = atoi(optarg);
            if (g_node_num <= 0)
            {
                printf("bad number:[%d]!\n", g_node_num);
                return 1;
            }
            break;
        default:
            printf("[ZKCLIENT_TESTER] Unkown option\n");
            print_usage(argv[0]);
            return 1;
        }
    }

    logger::initialize(argv[0]);

    /* create a regeste thread and a watch thread */
    pthread_t registe_tid = 0;
    pthread_t watch_tid   = 0;
    int ret = 0;
    ret = pthread_create(&registe_tid, NULL, registe_routine, zk_servers_list);
    if (0 != ret)
    {
        printf("[ZKCLIENT_TESTER] create registe thread failed, ret=[%d]\n", ret);
        return 1;
    }
    ret = pthread_create(&watch_tid, NULL, watch_routine, zk_servers_list);
    if (0 != ret)
    {
        printf("[ZKCLIENT_TESTER] create watch thread failed, ret=[%d]\n", ret);
        return 1;
    }

    ret = pthread_join(registe_tid, NULL);
    if (0 != ret)
    {
        printf("[ZKCLIENT_TESTER] join registe thread failed, ret=[%d]\n", ret);
        return 1;
    }
    ret = pthread_join(watch_tid, NULL);
    if (0 != ret)
    {
        printf("[ZKCLIENT_TESTER] join watch thread failed, ret=[%d]\n", ret);
        return 1;
    }

    printf("[ZKCLIENT_TESTER] over\n");
    return 0;
}
