#include "common/config_const.h"

namespace dstream {
namespace config_const {

const char* kLocalHostIPName         = "LocalhostIP";
const char* kHDFSClientDirName       = "HDFS.HadoopClientDir";
const char* kHDFSRetryName           = "HDFS.Retry";
const char* kHDFSTimeoutName         = "HDFS.TimeoutMs";

const char*   kHDFSClientDir         = "./hadoop-client/hadoop/";
const int32_t kDefaultHDFSRetry      = 3;
const int32_t kDefaultHDFSTimeoutMs  = 60;

const char* kZKRetryName             = "ZooKeeperPath.Retry";
const char* kZKTimeoutName           = "ZooKeeperPath.TimeoutMs";
const char* kZKPathName              = "ZooKeeperPath.Address";
const char* kZKRootPathName          = "ZooKeeperPath.Root";
const char* kPMRootPathName          = "ZooKeeperPath.PM.Root";
const char* kBackupPMRootName        = "ZooKeeperPath.BackupPM.Root";
const char* kPNRootPathName          = "ZooKeeperPath.PN.Root";
const char* kAppRootPathName         = "ZooKeeperPath.App.Root";
const char* kConfigRootPathName      = "ZooKeeperPath.Config.Root";
const char* kClientRootName          = "ZooKeeperPath.Client.Root";
const char* kPNGroupRootPathName     = "ZooKeeperPath.PNGroup.Root";
const char* kSubPointRootPathName    = "ZooKeeperPath.SubPoint.Root";
const char* kScribeRootPathName      = "ZooKeeperPath.Scribe.Root";

const int32_t kDefaultZKRetry        = 3;
const int32_t kDefaultZKTimeoutMs    = 30000;

const char* kDefaultSysPath          = "/DStream";
const char* kDefaultZKRootPath       = kDefaultSysPath;
const char* kDefaultPMRootPath       = "/DStream/PM";
const char* kDefaultPNRootPath       = "/DStream/PN";
const char* kDefaultClientRootPath   = "/DStream/Client";
const char* kDefaultAppRootPath      = "/DStream/App";
const char* kDefaultConfigRootPath   = "/DStream/Config";
const char* kDefaultPNGroupRootPath  = "/DStream/PNGroup";
const char* kDefaultSubPointRootPath = "/DStream/SubPoint";

const char* kMetaManagerName         = "PM.MetaManager";
const char* kZKMetaManagerName       = "ZKMetaManager";
const char* kMemoryMetaManagerName   = "MemoryMetaManager";

// pm config
const char* kPMConfigFile            = "pm_config.xml";
const char* kTestPMConfigFile        = "pm_test.xml";
const char* kSchedulerName           = "PM.Scheduler";
const char* kResourceScheduler       = "ResourceScheduler";
const char* kMaxAsyncRPCName         = "PM.MaxAsyncRPCNumber";
const char* kGCIntervalName          = "PM.GCInterval";
const int kDefaultGCInterval         = 1 * 1000;
const int kDefaultMaxAsyncRPCNumber  = 100;
const int kDefaultReportIntervalSec  = 10;
const int kReconnectToPMIntervalSec  = 10;
const char* kRootName                = "root";
const char* kRootPassWord            = "root";
const int   kDefaultWaitPNTimeoutSec = 5;
const float kMinActivePNPercentage   = 0.5;
const int   kDefaultMaxPENumOnPN     = 16;
const int   kDefaultMaxPEFailOnPnNum = 3;
const int  kDefaultPEFailReserveTime = 30;
const char* kConfRPCTimeout          = "PM.RPCTimeoutMs";

// pn transporter config
const char* kPNSmallTranBufNum       = "PN.Transporter.SmallTranBufNum";
const char* kPNSmallTranBufSize      = "PN.Transporter.SmallTranBufSize";
const char* kPNBigTranBufNum         = "PN.Transporter.BigTranBufNum";
const char* kPNWriteWaterMark        = "PN.Transporter.WriteWaterMark";
const int   kDefaultPNSmallTranBufNum= 512 * 1024;
const int   kDefaultPNSmallTranBufSize= 16 * 1024;
const int   kDefaultPNBigTranBufNum  = 32;
const int   kDefaultPNWriteWaterMark = 80;

// pn config
const char* kPNToPMRetryName         = "PN.PM.Retry";
const char* kPNToPMTimeoutName       = "PN.PM.TimeoutMs";

const char* kPNPMListenThreadNumName = "PN.PMListenThreadNum";
const char* kPNListenUriName         = "PN.ListenUri";
const char* kPNListenPortName        = "PN.ListenPort";

const char* kPNDebugListenThreadNumName = "PN.DebugListenThreadNum";
const char* kPNDebugListenUriName    = "PN.DebugUri";
const char* kPNDebugListenPortName   = "PN.DebugListenPort";

const char* kPNPNListenThreadNumName = "PN.PNListenThreadNum";
const char* kPNPNListenUriName       = "PN.PNUri";
const char* kPNPNListenPortName      = "PN.PNListenPort";

const char* kPNPEListenThreadNumName = "PN.PEListenThreadNum";
const char* kPNPEListenUriName       = "PN.PEUri";
const char* kPNPEListenPortName      = "PN.PEListenPort";

const char* kImporterUriName         = "PN.ImporterUri";
const char* kImporterPortName        = "PN.ImporterPort";
const char* kImporterTypeName        = "PN.ImporterType";
const char* kPNWorkDirName           = "PN.WorkDir";
const char* kPNSubChkIntervalSecName = "PN.SubChkIntervalSec";
const char* kPNSubQueueSizeName      = "PN.MaxSubQueueSize";
const char* kPNSendQueueSizeName     = "PN.MaxSendQueueSize";
const char* kPNRecvQueueSizeName     = "PN.MaxRecvQueueSize";
const char* kPNRecvWaterMarkName     = "PN.RecvQueueWaterMark";
const char* kPNSendWaterMarkName     = "PN.SendQueueWaterMark";
const char* kPNTuplesBatchCountName  = "PN.TuplesBatchCount";
const char* kPNForceForwardTuplesPeriodMillSecondsName = "PN.ForceForwardTuplesPeriodMillSeconds";
const char* kPNSuspendWhenQueueFullName = "PN.SuspendWhenQueueFull";
const char* kPNGroupName             = "PN.GroupName";
const char* kExporterTypeName        = "ExporterType";
// total avaible memory
const char* kPNTotalMemory           = "PN.TotalMemory";
const char* kPNTotalCPU              = "PN.TotalCPU";
const char* kPNBufferMemory          = "PN.BufferMemory";

const int32_t kPNToPMRetry           = 3;
const int32_t kPNToPMTimeoutMs       = 2000;
const int32_t kPNListenThreadNum     = 3;
const int32_t kPNListenPort          = 6000;
const int32_t kPNDebugListenPort     = 6001;
const int32_t kPNPNListenPort        = 6002;
const int32_t kPNPEListenPort        = 6003;
const int32_t kImporterPort          = 19870;
const char*   kImporterType          = "bigpipe";
const char*   kPNWorkDir             = ".";
const int32_t kPNSubChkIntervalSec   = 10000;
const uint64_t kPNSubQueueSize       = 10000;
const uint64_t kPNSendQueueSize      = 128 * 1024 * 1024;
const uint64_t kPNRecvQueueSize      = 128 * 1024 * 1024;
const float   kPNSendWaterMark       = 0.8;
const float   kPNRecvWaterMark       = 0.8;
const int32_t kPNIntervalSecs          = 5;
const int32_t kPNTuplesBatchCount    = 1024;
const int32_t kPNForceForwardTuplesPeriodMillSeconds = 1000;
const char*   kPNSuspendWhenQueueFull = "false";

// transporter config
const char*   kDataCheckIntervalName  = "PN.DataCheckInterval";
const int32_t kDataCheckIntervalMs    = 100; // ms
const char*   kDataFetchRetryName     = "PN.DataFetchRetry";
const int32_t kDataFetchRetry         = 10;
const char*   kDataSendRetryName      = "PN.DataSendRetry";
const int32_t kDataSendRetry          = 10;
const char*   kDataFetchTimeoutName   = "PN.DataFetchTimeoutMs";
const int32_t kDataFetchTimeoutMs     = 10000; // ms
const char*   kTPAllTag               = "_ALL_TAG";
const char*   kPNTuplesBatchSizeName  = "PN.TuplesBatchSizeByte";
const int32_t kPNTuplesBatchSizeByte  = 5 * 1024 * 1024;
const char*   kPNTuplesBatchNumName   = "PN.TuplesBatchNum";
const int32_t kPNTuplesBatchNum       = 10;
const int32_t kCounterDumpIntervalMs  = 1000; // ms

// for bigpipe exporter
const char*   kExporterTypeBigpipe   = "bigpipe";
// for pn group
const char*   kDefaultPNGroup        = "default";
// client config
/* config file name */
const char* kClusterCfgFileName      = "cluster.cfg.xml";
const char* kAppCfgFileName          = "application.cfg.xml";
const char* kSubTreeCfgFileName      = "sub_tree.cfg.xml";
const char* kDynamicImporterCfgFileName  = "dynamic_importer.cfg.xml";
const char* kUpdateProcessorCfgFileName  = "update.cfg.xml";

const char* kClientUserNameName      = "Client.User.UserName";
const char* kClientPasswordName      = "Client.User.Password";
const char* kClientToPMRetryName     = "Client.PM.Retry";
const char* kClientToPMTimeoutName   = "Client.PM.TimeoutMs";
const char* kAppNameName             = "AppName";
const char* kClientHostNameName      = "ClientHostName";

const int32_t kClientToPMRetry       = 3;
const int32_t kClientToPMTimeoutMs   = -1;

// test item config
const char* kTestZKMetaConfigFile    = "test_zk_meta_config.xml";

// user log config
const char* kDefaultUserLogLocalStore   = "false";
const char* kDefaultUserLogHdfsStore    = "true";
const char* kDefaultUserLogConsoleShow  = "false";
const char* kUserLogLocalStoreName    = "Log.PE.LocalStore";
const char* kUserLogHdfsStoreName     = "Log.PE.HdfsStore";
const char* kUserLogConsoleShowName   = "Log.PE.ConsoleShow";
const char* kDefaultUserLogTimeFormat = "%Y_%m_%d_%H_%M";
const char* kUserLogTimeFormatName    = "Log.PE.TimeFormat";

const char* kDefaultUserLogLocalPath = "./Log/PE";
const char* kUserLogLocalPathName    = "Log.PE.LocalPath";

const char* kPeProcFilePath           = "./pe_proc";

const char* kUseCglimitName           = "PN.UseCglimit";
const char* kDefaultUseCglimit        = "true";

// user app config
const char* kAppDataStrategyName         = "DataStrategy";
const char* kProcessorSendQSizeName      = "Transporter.SendQSize";
const char* kProcessorRecvQSizeName      = "Transporter.RecvQSize";
const char* kProcessorSendQWaterMarkName = "Transporter.SendQWaterMark";
const char* kProcessorRecvQWaterMarkName = "Transporter.RecvQWaterMark";
const char* kProcessorFetchTimeoutMsName = "Transporter.FetchTimeoutMs";
const char* kProcessorSendTimeoutMsName  = "Transporter.SendTimeoutMs";
const char* kProcessorBatchCountName     = "Transporter.TuplesBatchCount";
const char* kProcessorBatchSizeName      = "Transporter.TuplesBatchSize";
const char* kProcessorForceForwardMsName = "Transporter.ForceForwardTuplesPeriodMillSeconds";

const int32_t kDefaultAppDataStrategy    = 0; // drop old
const char* kDataStrategyDropAgelong = "DropAgelong"; // drop old
const char* kDataStrategyDropNew = "DropNew"; // drop new
const char* kDataStrategyHoldData = "HoldData"; // hang on

// ResourceSchedule config
const char* kResourceCmpName            = "PM.ResourceCompare";
const char* kPENumResourceCmp           = "PENumCompare";
const char* kMemoryCompare              = "MemoryCompare";
const char* kResourceAlgorithmName      = "PM.SchedulerAlgorithm";
const char* kDefaultResourceAlgorithm   = "BFDAlgorithm";
// Version Control
const char* kUnknownClientVersion       = "unknown client version";
const char* kUnknownPMVersion           = "unknown pm version";
const char* kPMMinRequiredClientVersion = "1-5-2-0";
const size_t kVersionLen                = 7;

const char* kQPSLimit                   = "QPSLimit";
const char* kAllProcessors              = "__ALL_PROCESSORS__";

// httpd dynamic port
const uint32_t kMinDynamicHttpdPort = 8000;
const uint32_t kMaxDynamicHttpdPort = 9000;

} // namespace config_const
} // namespace dstream
