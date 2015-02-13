/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_COMMON_ERROR_H__
#define __DSTREAM_CC_COMMON_ERROR_H__

#include <iostream>

namespace dstream {
namespace error {
/**
* @brief    error codes
* @notice   1) negative number means error
*           2) positive number means expected results
*/
enum Code {
    OK_NO_DATA_FLUSHED    = 201,
    OK_REMAIN_DATA        = 100,
    OK_SOFT_WATERMARK     = 50,
    OK_KEY_ALREADY_EXIST  = 2,
    OK_KEY_NOT_EXIST      = 1,

    OK                    = 0,

    /* common error */
    FAILED_EXPECTATION    = -1,
    EXTERNAL              = -2,
    UNPOSSIBLE            = -3,
    BAD_MEMORY_ALLOCATION = -4,
    BAD_MEM_ALLOC         = BAD_MEMORY_ALLOCATION,
    CANCELLED             = -5,
    VERSION_MISMATCH      = -6,
    DATA_CORRUPTED        = -7,
    TIMEOUT               = -8,
    COMMAND_PARSE_ERROR   = -9,
    AUTH_FAIL             = -10,
    BAD_ARGUMENT          = -11,
    CREATE_THREAD_ERROR   = -12,
    MASK_SIG_ERROR        = -13,
    FORK_EXEC_ERROR       = -14,
    FILE_OPERATION_ERROR  = -15,
    GET_HOST_IP_FAIL      = -16,
    QUEUE_FULL            = -17,
    HDFS_EXEC_ERROR       = -18,
    TRY_LOCK_FAIL         = -19,
    RPC_SEND_FAIL         = -20,
    RPC_RECV_FAIL         = -21,
    RPC_CALL_FAIL         = -22,
    CANNOT_CHDIR          = -23,
    CHILD_WAIT_INTERRUPT  = -24,
    CHILD_WAIT_TIMEOUT    = -25,
    SCRIPT_EXEC_NOT_ALLOWED = -26,
    QUEUE_EMPTY           = -27,
    TOO_LARGE_DATA        = -28,
    GET_REMOTEHOST_FAIL   = -29,
    THREAD_ERROR          = -30,
    ESP_BAD_MEM_ALLOC     = -31,
    ESP_TOUCH_WATERMARK   = -32,
    INIT_UBMONITOR_FAIL   = -33,
    EMPTY_DIRECTORY       = -34,

    /* fs io error */
    LOCAL_IO_ERROR        = -100,
    FILE_NOT_FOUND        = -101,
    FILE_READ_EOF         = -102,

    /* zookeepr error */
    ZK_BAD_ADDRESS        = -200,
    ZK_INIT_ERROR         = -201,
    ZK_CONNECT_ERROR      = -202,
    ZK_CONNECTION_LOST    = -203,
    ZK_NOT_EMPTY          = -204,
    ZK_GET_NODE_FAIL      = -205,
    ZK_SET_NODE_FAIL      = -206,
    ZK_CREATE_NODE_FAIL   = -207,
    ZK_DELETE_NODE_FAIL   = -208,
    ZK_SET_WATCHER_FAIL   = -209,
    ZK_GET_WATCHER_FAIL   = -210,
    ZK_DELETE_WATCHER_FAIL  = -211,
    ZK_GET_CHILDREN_FAIL  = -212,
    ZK_NODE_EXISTS        = -213,
    ZK_NO_NODE            = -214,

    /* network error */
    COMM_NOT_CONNECTED              = -300,
    COMM_BROKEN_CONNECTION          = -301,
    COMM_CONNECT_FAIL               = -302,
    COMM_ALREADY_CONNECTED          = -303,
    COMM_SEND_ERROR                 = -304,
    COMM_RECEIVE_ERROR              = -305,
    COMM_POLL_ERROR                 = -306,
    COMM_CONFLICTING_ADDRESS        = -307,
    COMM_BAD_DOMAIN_NAME            = -308,
    COMM_SOCKET_ERROR               = -309,
    COMM_BIND_ERROR                 = -310,
    COMM_LISTEN_ERROR               = -311,
    COMM_HEADER_CHECKSUM_MISMATCH   = -312,
    COMM_PAYLOAD_CHECKSUM_MISMATCH  = -313,
    COMM_BAD_HEADER                 = -314,
    /* protobuf error */

    /* meta error */
    META_INVALID                    = -400,

    /* config error */
    CONFIG_BAD_ARGUMENT             = -450,
    CONFIG_BAD_FILE                 = -451,
    CONFIG_ITERM_INVALID            = -452,
    CONFIG_BAD_VALUE                = -453,
    CONFIG_NO_NODE                  = -454,

    /* client error */
    CLIENT_NOT_SUPPORTED            = -500,
    CLIENT_CONNECT_ZK_FAIL          = -501,
    CLIENT_CONNECT_PM_FAIL          = -502,
    CLIENT_INIT_TP_FAIL             = -503,
    CLIENT_CHECK_TP_FAIL            = -504,
    CLIENT_BUILD_TP_FAIL            = -505,
    CLIENT_SUBMIT_APP_FAIL          = -506,
    CLIENT_NOT_LOGIN_YET            = -507,
    CLIENT_APPLY_APP_ID_FAIL        = -508,
    CLIENT_NOTIFY_PM_FAIL           = -510,
    CLIENT_DECOMMISSION_APP_FAIL    = -511,
    CLIENT_KILL_PE_FAIL             = -512,
    CLIENT_GET_APP_STATUE_FAIL      = -513,
    CLIENT_KILL_PN_FAIL             = -514,
    CLIENT_USER_OP_FAIL             = -515,
    CLIENT_MERGER_TOPO_FAIL         = -516,
    CLIENT_UPDATE_TOPO_FAIL         = -517,
    CLIENT_FLOW_CONTROL_FAIL        = -518,
    CLIENT_UPDATE_PARA_FAIL         = -519,
    CLIENT_UPDATE_PRO_FAIL          = -520,
    CLIENT_VERSION_CHECK_FAIL       = -521,

    /* pm error */
    MASTER_CONNECT_FAIL             = -550,
    DEL_BACKUP_PM_FAIL              = -551,
    UPDATE_BACKUP_PM_FAIL           = -552,
    INVALID_POINTER                 = -553,
    ADD_PN_REPORT_FAIL              = -554,
    REMOVE_PN_FAIL                  = -555,
    NO_CLIENT                       = -556,
    REMOVE_FILE_FAIL                = -557,
    ADD_BLACK_LIST_FAIL             = -558,
    FOUND_ZK_PN_FAIL                = -559,
    ADD_APPSTATUS_FAIL              = -560,
    DEL_APPSTATUS_FAIL              = -561,
    UPD_APPSTATUS_FAIL              = -562,
    GET_APPSTATUS_FAIL              = -563,
    NO_SUCH_REQUEST_URL             = -564,
    PM_RESOURCE_NOT_AVAILABLE       = -565,
    

    // resource scheduler error
    RESOURCE_SCHEDULE_NOT_INIT      = -580,
    ADD_PN_RESOURCE_FAIL            = -581,
    RESOURCE_REQ_EXIST              = -582,
    ADD_RESOURCE_REQUEST_FAIL       = -583,
    UPDATE_SCEDULER_FAIL            = -584,
    ALLOC_RESOURCE_FAIL             = -585,
    RESOURCE_NOT_ENOUGH             = -586,
    PN_STATICS_EXIST                = -587,
    PN_STATICS_NOT_EXIST            = -588,
    RESOURCE_FAIL                   = -589,
    PN_RESOURCE_NOT_ENOUGH          = -590,
    SELECT_PRIMARY_FAIL             = -591,

    /* pn error */
    CREATE_PE_ERROR                 = -600,
    PE_ALREADY_EXISTS               = -601,
    PIPE_OPEN_ERROR                 = -602,
    PIPE_PEEK_ERROR                 = -603,
    PIPE_READ_ERROR                 = -604,
    PIPE_WRITE_ERROR                = -605,
    GEN_ENV_ERROR                   = -606,
    RECV_QUEUE_EMPTY                = -607,
    NOT_FOUND_RECV_QUEUE            = -608,
    SEND_QUEUE_EMPTY                = -609,
    NOT_FOUND_SEND_QUEUE            = -610,
    NOT_FIND_PE                     = -611,
    THREAD_STOP                     = -612,
    PE_NOT_DEPLORY_YET              = -613,
    PEWRAPPER_NOT_FOUND             = -614,
    UPSTREAM_PE_NOT_FOUND           = -615,
    UPSTREAM_PE_CONN_NOT_FOUND      = -616,
    UPSTREAM_PE_RPC_TIMEOUT         = -617,
    UPSTREAM_PE_RPC_ERROR           = -618,
    UPSTREAM_QUEUE_EMPTY            = -619,
    NOT_SUBCRIBE_YET                = -620,
    SUBSCRIBE_FAIL                  = -621,
    GC_PE_CONN_FAIL                 = -622,
    ACTIVE_SUBSCRIBE                = -623,
    BUSY_SUBSCRIBE                  = -624,
    SEND_QUEUE_FULL                 = -625,
    RECV_QUEUE_FULL                 = -626,
    UPDATE_PE_CONFIG_FAIL           = -627, /*PEWrapper::UpdatePEConfig Fail*/
    GEN_LOCAL_PE_ENV_FAIL           = -628, /*PEWrapper::GenLocalEnv Fail*/
    GEN_PROCESS_ENV_FAIL            = -629, /*PEWrapper::GenProcessEnv Fail*/
    START_PE_FAIL                   = -630, /*PEWrapper::StartPEProcess Fail*/
    PN_RESOURCE_NOT_AVAILABLE       = -631,
    PEWRAPPER_STATUS_ERROR          = -632,
    PEWRAPPER_ALREADY_EXIST         = -633,

    /* pe error */
    BIGPIPE_CONNECTION_FAIL         = -700,
    UNKNOWN_TUPLE_TYPE              = -701,
    UNKNOWN_PE_TYPE                 = -702,
    SERIAIL_TUPLE_FAIL              = -703,
    DESERIAIL_TUPLE_FAIL            = -704,
    PE_TERMINATING                  = -705,
    IMPORTER_QUEUE_EMPTY            = -706,
    INSTALL_SIGNAL_HANDLER_FAIL     = -707,

    /*transporter error*/
    BIND_STUB_FAIL                  = -750,
    NO_STUB_FOUND                   = -751,
    SOURCE_ALREADY_EXIST            = -752,
    SOURCE_NOT_EXIST                = -753,
    ADD_SOURCE_TO_MAP_FAIL          = -754,
    POST_ESP_MSG_FAIL               = -755,
    EXTRACT_TP_MSG_FAIL             = -756,
    PUT_DATA_INTO_BUF_FAIL          = -757,
    DATA_IS_NULL                    = -758,
    TP_ALREADY_EXIST                = -759,
    TP_NOT_EXIST                    = -760,
    TP_SEND_QUERY_FAIL              = -761,
    RB_BUFFER_FULL                  = -762,
    RB_BUFFER_EMPTY                 = -763,
    START_QUEUE_FAIL                = -764,
    NOT_RUNNING                     = -765,
    ALREADY_RUNNING                 = -766,
    /* ring buffer error */
    BUFFER_NOT_ENOUGH_DATA          = -768,
    BUFFER_NO_PENDING_DATA          = -769,

    DROP_MSG_DUE_QUEUE_FULL         = -780,
    DROP_MSG_DUE_ID_MISS_MATCH      = -781,
    DROP_MSG_DUE_ACK_MISS_MATCH     = -782,
    DROP_MSG_DUE_ACK_STATUS_MISS_MATCH = -783,
    DROP_MSG_DUE_INFLOW_STATUS_MISS_MATCH = -784,
    DROP_MSG_DUE_HAS_PENDING_DATA   = -785,
    UNKNOWN_TP_STATUS               = -786,
    EXPIRED_MSG                     = -787,



    /* rpc error*/
    CREATE_RESPONSE_FAIL            = -800,
    CREATE_CONNECTION_FAIL          = -801,

    /* data message */
    CONSTROL_MSG                    = -850,

    /*meta error*/
    INVALID_USER                    = -900,
    ADD_APP_FAIL                    = -901,
    APP_NOT_EXIST                   = -902,
    APP_EXIST                       = -903,
    APP_PE_ERROR                    = -904,
    PE_NOT_EXIST                    = -905,
    ADD_PM_FAIL                     = -906,
    PM_NOT_EXIST                    = -907,
    PN_NOT_EXIST                    = -908,
    PN_EXSIT                        = -909,
    PN_ADD_FAIL                     = -910,
    CHECK_PM_FAIL                   = -911,
    CHECK_PN_FAIL                   = -912,
    CREATE_ZK_FAIL                  = -913,
    WATCH_FAIL                      = -914,
    UNWATCH_FAIL                    = -915,
    CONNECT_ZK_FAIL                 = -916,
    GET_ZK_DATA_FAIL                = -917,
    PARSE_DATA_FAIL                 = -918,
    SERIAIL_DATA_FAIL               = -919,
    DESERIAIL_DATA_FAIL             = -920,
    ALLOC_BUFFER_FAIL               = -921,
    SET_ZKDATA_FAIL                 = -922,
    INSERT_SET_FAIL                 = -923,
    ADD_PE_FAIL                     = -924,
    CONFIG_NOT_EXIST                = -925,
    INVALID_APP_NAME                = -926,
    REVISION_NOT_EXIST              = -927,
    PROCESSOR_NOT_EXIST             = -928,

    // Kylin/Esp error
    KYLIN_E_BEGIN                   = -5000,
    KYLIN_E_END                     = -5999,
    // HTTPD error
    HTTPD_FAILED                    = -6000,                        REQUEST_ERROR                   = -6001,


    /**
    * @brief    error codes that user defined should be great than this number
    */
    USER_DEFINE_BEGIN              = -10000
};

const char* get_text(int error);
inline const char* ErrStr(int error) {
    return get_text(error);
}

}  // namespace error
}  // namespace dstream
#endif
