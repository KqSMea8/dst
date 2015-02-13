#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "redis_env.h"
#include "common/utils.h"

using namespace dstream;

int kSentinelPort       = 5712;
int kRedisMasterPort    = 6712;
int kRedisSlavePort     = 7712;

std::string g_RootDir;
std::string g_RedisRootDir;
std::string g_RedisMasterCfgWithAuth;
std::string g_RedisSlaveCfgWithAuth;
std::string g_SentinelCfgWithAuth;
std::string g_RedisMasterNameForSentinel  = "UnitTestMasterName";
std::string g_RedisMasterPasswd = "StorageUnitTest";
std::string g_RedisSlavePasswd = g_RedisMasterPasswd; /*keep the same to master*/
std::string g_SentinelPasswd = g_RedisMasterPasswd;
std::string g_RedisMasterIpport;
std::string g_RedisSlaveIpport;
std::string g_SentinelIpport;
std::string g_RedisMasterName = "__StorageUnitTestRedisMasterServer";
std::string g_RedisSlaveName = "__StorageUnitTestRedisSlaveServer";
std::string g_SentinelName = "__StorageUnitTestSentinelServer";
std::string g_RedisMasterExec = std::string("./") + g_RedisMasterName;
std::string g_RedisSlaveExec = std::string("./") + g_RedisSlaveName;
std::string g_SentinelExec = std::string("./") + g_SentinelName;

static bool s_Init = false;
//static bool s_Runing = false;
static bool s_SentinelRuning = false;

int _StartRedisServer(const std::string& exec, int port,
                      const std::string& cfg_file);
int _StopRedisServer(const std::string& server_name);
int _RestartRedisServer(const std::string& cfg_file,
                        int port,
                        const std::string& server_name);

static int TestLocation() {
    if (g_RootDir.empty()) {
        for (int i = 0; i < 10; i++) {
            g_RootDir += "../";
            if (IsDir((g_RootDir + "/inf/computing/dredis/rclient").c_str())) {
                fprintf(stdout, "the root dir is:[%s]\n", g_RootDir.c_str());
                g_RedisRootDir = g_RootDir + "/inf/computing/dredis";
                return 0;
            }
        }
        fprintf(stderr, "where the hell am I!\n");
        return -1;
    }
    return 0;
}

int EnvSetUp() {
    if (TestLocation() < 0) {
        return -1;
    }
    fprintf(stdout, "the redis root dir is [%s]\n", g_RedisRootDir.c_str());
    if (s_Init) {
        return 0;
    }

    std::string cmd;
    cmd += "cd " + g_RedisRootDir + "&& sh build.sh >/dev/null&& cd - &&";
    cmd += "cp " + g_RedisRootDir + "/output/bin/redis-server " + g_RedisMasterExec + "&&";
    cmd += "cp " + g_RedisRootDir + "/output/bin/redis-server " + g_RedisSlaveExec + "&&";
    cmd += "cp " + g_RedisRootDir + "/output/bin/redis-server " + g_SentinelExec;
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());

    g_RedisMasterIpport += "127.0.0.1:";
    StringAppendNumber(&g_RedisMasterIpport, kRedisMasterPort);
    g_SentinelIpport += "127.0.0.1:";
    StringAppendNumber(&g_SentinelIpport, kSentinelPort);
    g_RedisMasterCfgWithAuth = "./conf/redis_master_with_auth.conf";
    g_RedisSlaveCfgWithAuth  = "./conf/redis_slave_with_auth.conf";
    g_SentinelCfgWithAuth    = "./conf/redis_sentinel_with_auth.conf";

    s_Init = true;
    return 0;
}

int EnvTearDown () {
    //TODO: rm envs
    StopRedisMasterServer();
    StopRedisSlaveServer();
    StopSentinelServer();
    std::string cmd;
    cmd += "rm";
    cmd += " " + g_RedisMasterExec;
    cmd += " " + g_RedisSlaveExec;
    cmd += " " + g_SentinelExec;
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());
    s_Init = false;
    return 0;
}

int StartRedisMasterServer(const std::string& cfg_file) {
    return _StartRedisServer(g_RedisMasterExec, kRedisMasterPort, cfg_file);
}
int StartRedisSlaveServer(const std::string& cfg_file) {
    return _StartRedisServer(g_RedisSlaveExec, kRedisSlavePort, cfg_file);
}
int _StartRedisServer(const std::string& exec,  int port,
                      const std::string& cfg_file) {
    static int id = 0;
    if (!s_Init) {
        if (EnvSetUp() < 0) {
            fprintf(stderr, "fail to setup env\n");
            return -1;
        }
    }
    //if (s_Runing) { return 0; }

    std::string cmd  = "./";
    cmd += exec;
    if (cfg_file.empty()) {
        cmd += " --port ";
        StringAppendNumber(&cmd, port);
    } else {
        cmd += " ";
        cmd += cfg_file;
    }
    cmd += " --loglevel debug 1>__storage_unit_test.";
    StringAppendNumber(&cmd, id++);
    cmd += ".log&";
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());
    //s_Runing = true;
    return 0;
}

int StopRedisMasterServer() {
    return _StopRedisServer(g_RedisMasterName);
}
int StopRedisSlaveServer() {
    return _StopRedisServer(g_RedisSlaveName);
}
int _StopRedisServer(const std::string& server_name) {
    std::string cmd;
    cmd += "killall ";
    cmd += server_name;
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());
    //s_Runing = false;
    return 0;
}

int RestartRedisMasterServer() {
    return _RestartRedisServer(g_RedisMasterCfgWithAuth,
                               kRedisMasterPort,
                               g_RedisMasterName);
}
int RestartRedisSlaveServer() {
    return _RestartRedisServer(g_RedisSlaveCfgWithAuth,
                               kRedisSlavePort,
                               g_RedisSlaveName);
}
int _RestartRedisServer(const std::string& cfg_file,
                        int port,
                        const std::string& server_name) {
    _StopRedisServer(server_name);
    sleep(1); //wait stop
    return _StartRedisServer(server_name, port, cfg_file);
}

int StartSentinelServer(const std::string& cfg_file) {
    if (!s_Init) {
        if (EnvSetUp() < 0) {
            fprintf(stderr, "fail to setup env\n");
            return -1;
        }
    }
    if (s_SentinelRuning) {
        return 0;
    }

    std::string cmd = g_SentinelExec;
    if (cfg_file.empty()) {
        cmd += " --port ";
        StringAppendNumber(&cmd, kSentinelPort);
    } else {
        cmd += " ";
        cmd += cfg_file;
    }
    cmd += " --sentinel --loglevel debug 2>&1 1>__storage_sentinel_unit_test.log&";
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());
    s_SentinelRuning = true;
    return 0;
}

int StopSentinelServer() {
    std::string cmd;
    cmd += "killall ";
    cmd += g_SentinelName;
    fprintf(stdout, "[%s]:[%s]...\n", __FUNCTION__, cmd.c_str());
    system(cmd.c_str());
    s_SentinelRuning = false;
    return 0;
}

void FillRandomKeys(std::vector<std::string>* values) {
    const size_t n = values->size();
    char a[n];
    for (size_t i = 0; i < n; i++) {
        int len = random() % (sizeof(a) / sizeof a[0]) + 1;
        for (int j = 0; j < len; j++) {
            a[j] = random() % 127;
        }
        values->at(i).assign((char*)a, sizeof(char) * len);;
    }
}
