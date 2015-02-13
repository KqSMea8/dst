/******************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
*******************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    define processmaster config const
*********************************************************************/

#ifndef __DSTREAM_COMMON_CONFIG_CONST_H__
#define __DSTREAM_COMMON_CONFIG_CONST_H__

#include <cstddef>
#include <string>

namespace dstream {
namespace config_const {

extern const char* kLocalHostIPName;
extern const char* kHDFSClientDirName;
extern const char* kHDFSRetryName;
extern const char* kHDFSTimeoutName;

extern const char*   kHDFSClientDir;
extern const int32_t kDefaultHDFSRetry;
extern const int32_t kDefaultHDFSTimeoutMs;

/************************************************************************/
/* zk config item name                                                  */
/************************************************************************/
extern const char* kZKRetryName;
extern const char* kZKTimeoutName;
extern const char* kZKPathName;
extern const char* kZKRootPathName;
extern const char* kPMRootPathName;
extern const char* kBackupPMRootName;
extern const char* kPNRootPathName;
extern const char* kAppRootPathName;
extern const char* kConfigRootPathName;
extern const char* kClientRootName;
extern const char* kPNGroupRootPathName;
extern const char* kSubPointRootPathName;
extern const char* kScribeRootPathName;

/************************************************************************/
/* zookeeper default config value                                       */
/************************************************************************/
extern const int32_t kDefaultZKRetry;
extern const int32_t kDefaultZKTimeoutMs;

extern const char* kDefaultSysPath;
extern const char* kDefaultZKRootPath;
extern const char* kDefaultPMRootPath;
extern const char* kDefaultPNRootPath;
extern const char* kDefaultAppRootPath;
extern const char* kDefaultClientRootPath;
extern const char* kDefaultConfigRootPath;
extern const char* kDefaultPNGroupRootPath;
extern const char* kDefaultSubPointRootPath;

// MetaManager config item
extern const char* kMetaManagerName;
extern const char* kZKMetaManagerName;
extern const char* kMemoryMetaManagerName;

// pm config
extern const char* kPMConfigFile;
extern const char* kTestPMConfigFile;
extern const char* kSchedulerName;
extern const char* kResourceScheduler;
extern const char* kMaxAsyncRPCName;
extern const char* kGCIntervalName;
extern const int   kDefaultMaxAsyncRPCNumber;
extern const int   kDefaultGCInterval;
extern const int   kDefaultReportIntervalSec;
extern const int   kReconnectToPMIntervalSec;
extern const char* kRootName;
extern const char* kRootPassWord;
extern const int   kDefaultWaitPNTimeoutSec;
extern const float kMinActivePNPercentage;
extern const int   kDefaultMaxPENumOnPN;
extern const int   kDefaultMaxPEFailOnPnNum;
extern const int   kDefaultPEFailReserveTime;
extern const char* kConfRPCTimeout;
// pn transporter config
extern const char* kPNSmallTranBufNum;
extern const char* kPNSmallTranBufSize;
extern const char* kPNBigTranBufNum;
extern const char* kPNWriteWaterMark;
extern const int   kDefaultPNSmallTranBufNum;
extern const int   kDefaultPNSmallTranBufSize;
extern const int   kDefaultPNBigTranBufNum;
extern const int   kDefaultPNWriteWaterMark;
// pn config
extern const char* kPNToPMRetryName;
extern const char* kPNToPMTimeoutName;
extern const char* kPNPNListenThreadNumName;
extern const char* kPNPEListenThreadNumName;
extern const char* kPNPMListenThreadNumName;
extern const char* kPNDebugListenThreadNumName;
extern const char* kPNListenUriName;
extern const char* kPNListenPortName;
extern const char* kPNDebugListenUriName;
extern const char* kPNDebugListenPortName;
extern const char* kPNPNListenUriName;
extern const char* kPNPNListenPortName;
extern const char* kPNPEListenUriName;
extern const char* kPNPEListenPortName;
extern const char* kImporterUriName;
extern const char* kImporterPortName;
extern const char* kImporterTypeName;
extern const char* kPNWorkDirName;
extern const char* kPNSubChkIntervalSecName;
extern const char* kPNSubQueueSizeName;
extern const char* kPNSendQueueSizeName;
extern const char* kPNRecvQueueSizeName;
extern const char* kPNSendWaterMarkName;
extern const char* kPNRecvWaterMarkName;
extern const char* kPNTuplesBatchCountName;
extern const char* kPNForceForwardTuplesPeriodMillSecondsName;
extern const char* kPNSuspendWhenQueueFullName;
extern const char* kPNTotalMemory;
extern const char* kPNTotalCPU;
extern const char* kPNBufferMemory;
extern const char* kExporterTypeName;
extern const char* kPNGroupName;

extern const int32_t kPNToPMRetry;
extern const int32_t kPNToPMTimeoutMs;
extern const int32_t kPNListenThreadNum;
extern const int32_t kPNListenPort;
extern const int32_t kPNDebugListenPort;
extern const int32_t kPNPNListenPort;
extern const int32_t kPNPEListenPort;
extern const int32_t kImporterPort;
extern const char*   kImporterType;
extern const char*   kPNWorkDir;
extern const int32_t kPNSubChkIntervalSec;
extern const uint64_t kPNSubQueueSize;
extern const uint64_t kPNSendQueueSize;
extern const uint64_t kPNRecvQueueSize;
extern const float   kPNSendWaterMark;
extern const float   kPNRecvWaterMark;
extern const int32_t kPNIntervalSecs;
extern const int32_t kPNTuplesBatchCount;
extern const int32_t kPNForceForwardTuplesPeriodMillSeconds;
extern const char*   kPNSuspendWhenQueueFull;
extern const char*   kExporterTypeBigpipe;
extern const char*   kDefaultPNGroup;

// transporter config
// extern const int32_t kDefaultRingBufSize; // TODO: type redefine
extern const char*   kDataCheckIntervalName;
extern const int32_t kDataCheckIntervalMs;
extern const char*   kDataFetchRetryName;
extern const int32_t kDataFetchRetry;
extern const char*   kDataSendRetryName;
extern const int32_t kDataSendRetry;
extern const char*   kDataFetchTimeoutName;
extern const int32_t kDataFetchTimeoutMs;
extern const char*   kTPAllTag;
extern const char*   kPNTuplesBatchSizeName;
extern const int32_t kPNTuplesBatchSizeByte;
extern const char*   kPNTuplesBatchNumName;
extern const int32_t kPNTuplesBatchNum;
extern const int32_t kCounterDumpIntervalMs; // ms

// client config
/* config file name */
extern const char* kClusterCfgFileName;
extern const char* kAppCfgFileName;
extern const char* kSubTreeCfgFileName;
extern const char* kDynamicImporterCfgFileName;
extern const char* kUpdateProcessorCfgFileName;

extern const char* kClientUserNameName;
extern const char* kClientPasswordName;
extern const char* kClientToPMRetryName;
extern const char* kClientToPMTimeoutName;
extern const char* kAppNameName;
extern const char* kClientHostNameName;

extern const int32_t kClientToPMRetry;
extern const int32_t kClientToPMTimeoutMs;

// test item config
extern const char* kTestZKMetaConfigFile;

// pe user log config
extern const char* kDefaultUserLogLocalStore;
extern const char* kDefaultUserLogHdfsStore;
extern const char* kDefaultUserLogConsoleShow;
extern const char* kUserLogLocalStoreName;
extern const char* kUserLogHdfsStoreName;
extern const char* kUserLogConsoleShowName;

extern const char* kDefaultUserLogTimeFormat;
extern const char* kUserLogTimeFormatName;

extern const char* kUserLogLocalPathName;
extern const char* kDefaultUserLogLocalPath;

extern const char* kPeProcFilePath;

extern const char* kUseCglimitName;
extern const char* kDefaultUseCglimit;

// user app config
extern const char* kProcessorSendQSizeName;
extern const char* kProcessorRecvQSizeName;
extern const char* kProcessorSendQWaterMarkName;
extern const char* kProcessorRecvQWaterMarkName;
extern const char* kProcessorFetchTimeoutMsName;
extern const char* kProcessorSendTimeoutMsName;
extern const char* kProcessorBatchCountName;
extern const char* kProcessorBatchSizeName;
extern const char* kProcessorForceForwardMsName;
extern const char* kAppDataStrategyName;
// defaul value
extern const int64_t kDefaultAppSendQSize;
extern const int64_t kDefaultAppRecvQSize;
extern const float kDefaultAppSendQWaterMark;
extern const float kDefaultAppRecvQWaterMark;
extern const int32_t kDefaultAppDataStrategy;

extern const char* kDataStrategyDropAgelong;
extern const char* kDataStrategyDropNew;
extern const char* kDataStrategyHoldData;

// ResourceSchedule config
extern const char* kResourceCmpName;
extern const char* kMemoryCompare;
extern const char* kPENumResourceCmp;
extern const char* kResourceAlgorithmName;
extern const char* kDefaultResourceAlgorithm;

// Version Contorl
extern const char* kUnknownClientVersion;
extern const char* kUnknownPMVersion;
extern const char* kPMMinRequiredClientVersion;
extern const size_t kVersionLen; 

// flow control
extern const char* kQPSLimit;
extern const char* kAllProcessors;

// httpd dynamic port
extern const uint32_t kMinDynamicHttpdPort;
extern const uint32_t kMaxDynamicHttpdPort;
} // namespace config_const
} // namespace dstream

#endif // __DSTREAM_COMMON_CONFIG_CONST_H__
