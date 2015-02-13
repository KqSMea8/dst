#ifndef __DSTREAM_STORAGE_TEST_ENV_H__
#define __DSTREAM_STORAGE_TEST_ENV_H__

#include <string>
#include <vector>

extern int kSentinelPort;
extern int kRedisMasterPort;
extern int kRedisSlavePort;
extern std::string g_RedisMasterNameForSentinel;
extern std::string g_RedisMasterName1ForSentinel;
extern std::string g_RedisMasterPasswd;
extern std::string g_SentinelPasswd;
extern std::string g_RedisMasterCfgWithAuth;
extern std::string g_RedisSlaveCfgWithAuth;
extern std::string g_SentinelCfgWithAuth;
extern std::string g_RedisRootDir;
extern std::string g_RedisMasterIpport;
extern std::string g_SentinelIpport;
extern std::string g_RedisMasterExec;
extern std::string g_RedisSlaveExec;
extern std::string g_SentinelExec;
extern std::string g_RedisMasterName;
extern std::string g_RedisSlaveName;
extern std::string g_SentinelName;

int EnvSetUp();
int EnvTearDown();
int StartRedisMasterServer(const std::string& cfg_file = "");
int StartRedisSlaveServer(const std::string& cfg_file = "");
int StopRedisMasterServer();
int StopRedisSlaveServer();
int RestartRedisMasterServer();
int RestartRedisSlaveServer();
int StartSentinelServer(const std::string& cfg_file = "");
int StopSentinelServer();


void FillRandomKeys(std::vector<std::string>* values);

#endif //__DSTREAM_STORAGE_TEST_ENV_H__
