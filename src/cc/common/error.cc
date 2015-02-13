/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <stdint.h>
#include <map>

#include "ESP.h"
#include "Kylin.h"

#include "common/error.h"

namespace dstream {

struct ErrorInfo {
    int32_t     code;
    const char*  text;
};

ErrorInfo error_info[] = {
    { error::APP_NOT_EXIST,               "app does not exist" },
    { error::OK_NO_DATA_FLUSHED,          "no data flushed" },
    { error::OK_REMAIN_DATA,              "data remains" },
    { error::OK_SOFT_WATERMARK,           "queue reaches soft watermark" },
    { error::OK_KEY_ALREADY_EXIST,        "key already exists" },
    { error::OK_KEY_NOT_EXIST,            "key not exist" },
    { error::OK,                          "ok" },
    { error::FAILED_EXPECTATION,          "failed expectation" },
    { error::EXTERNAL,                    "error" },
    { error::UNPOSSIBLE,                  "But that's unpossible!" },
    { error::BAD_MEMORY_ALLOCATION,       "fail to allocate memory"},
    { error::CANCELLED,                   "cancelled"},
    { error::VERSION_MISMATCH,            "version mismatched"},
    { error::DATA_CORRUPTED,              "data corrupted" },
    { error::TIMEOUT,                     "time out" },
    { error::COMMAND_PARSE_ERROR,         "fail to parse command" },
    { error::AUTH_FAIL,                   "fail to authenticate" },
    { error::BAD_ARGUMENT,                "bad arguments"},
    { error::CREATE_THREAD_ERROR,         "fail to create thread"},
    { error::MASK_SIG_ERROR,              "fail to mask signal"},
    { error::FORK_EXEC_ERROR,             "fail to fork executor"},
    { error::FILE_OPERATION_ERROR,        "fail to operation file"},
    { error::GET_HOST_IP_FAIL,            "fail to get local host ip"},
    { error::QUEUE_FULL,                  "queue is full"},
    { error::TRY_LOCK_FAIL,               "fail to try lock, may lock is busy"},
    { error::RPC_SEND_FAIL,               "rpc fail to send data"},
    { error::RPC_RECV_FAIL,               "rpc fail to recv data"},
    { error::RPC_CALL_FAIL,               "rpc call fail"},
    { error::QUEUE_EMPTY,                 "queue is empty"},
    { error::TOO_LARGE_DATA,              "data is to large"},
    { error::THREAD_ERROR,                "pthread error"},
    { error::ESP_BAD_MEM_ALLOC,           "fail to alloc esp mem"},
    { error::ESP_TOUCH_WATERMARK,         "esp mem touch the watermark"},
    { error::INIT_UBMONITOR_FAIL,         "fail to init ubmonitor"},
    { error::EMPTY_DIRECTORY,             "directory is empty"},

    { error::LOCAL_IO_ERROR,              "local i/o error" },
    { error::FILE_NOT_FOUND,              "file not found" },

    { error::ZK_BAD_ADDRESS,              "bad zookeeper address" },
    { error::ZK_INIT_ERROR,               "fail to init zookeeper client" },
    { error::ZK_CONNECT_ERROR,            "fail connect to zookeeper" },
    { error::ZK_CONNECTION_LOST,          "connection to zookeeper lost" },
    { error::ZK_NOT_EMPTY,                "node has children nodes" },
    { error::ZK_GET_NODE_FAIL,            "fail to get node data" },
    { error::ZK_SET_NODE_FAIL,            "fail to set node data" },
    { error::ZK_CREATE_NODE_FAIL,         "fail to create node" },
    { error::ZK_DELETE_NODE_FAIL,         "fail to delete node" },
    { error::ZK_SET_WATCHER_FAIL,         "fail to set watcher on node" },
    { error::ZK_GET_WATCHER_FAIL,         "fail to get watcher on node" },
    { error::ZK_DELETE_WATCHER_FAIL,      "fail to delete watcher on node" },
    { error::ZK_GET_CHILDREN_FAIL,        "fail to get node's children" },
    { error::ZK_NODE_EXISTS,              "node already exists" },
    { error::ZK_NO_NODE,                  "node not exists" },

    { error::COMM_NOT_CONNECTED,          "not connected" },
    { error::COMM_BROKEN_CONNECTION,      "broken connection" },
    { error::COMM_CONNECT_FAIL,           "fail to connect" },
    { error::COMM_ALREADY_CONNECTED,      "already connected" },
    { error::COMM_SEND_ERROR,             "send error" },
    { error::COMM_RECEIVE_ERROR,          "receive error" },
    { error::COMM_POLL_ERROR,             "poll error" },
    { error::COMM_CONFLICTING_ADDRESS,    "conflicting address" },
    { error::COMM_BAD_DOMAIN_NAME,        "bad domain name" },
    { error::COMM_SOCKET_ERROR,           "socket error" },
    { error::COMM_BIND_ERROR,             "bind error" },
    { error::COMM_LISTEN_ERROR,           "listen error" },
    { error::COMM_HEADER_CHECKSUM_MISMATCH,  "header checksum mismatch" },
    { error::COMM_PAYLOAD_CHECKSUM_MISMATCH, "payload checksum mismatch" },
    { error::COMM_BAD_HEADER,             "bad header" },

    { error::META_INVALID,                "invalid meta data" },
    { error::CREATE_PE_ERROR,             "fail to create pe" },
    { error::PE_ALREADY_EXISTS,           "pe alreay exists" },
    { error::NOT_FOUND_SEND_QUEUE,        "send queue not found" },
    { error::THREAD_STOP,                 "thread already stop" },
    { error::PE_NOT_DEPLORY_YET,          "pe has not been deployed" },
    { error::PEWRAPPER_NOT_FOUND,         "pe wrapper not found" },
    { error::UPSTREAM_PE_NOT_FOUND,       "pe's upstream pes not found" },
    { error::UPSTREAM_PE_CONN_NOT_FOUND,  "pe's upstream pes' connection not found" },
    { error::UPSTREAM_PE_RPC_TIMEOUT,     "pe's upstream pes' rpc timeout" },
    { error::UPSTREAM_PE_RPC_ERROR,       "pe's upstream pes' rpc error" },
    { error::UPSTREAM_QUEUE_EMPTY,        "upstream pe's queue is empty" },
    { error::NOT_SUBCRIBE_YET,            "not subscribe yet" },
    { error::SUBSCRIBE_FAIL,              "subscribe fail" },
    { error::GC_PE_CONN_FAIL,             "gc pe connection failed" },
    { error::CONSTROL_MSG,                "got a constrol msg" },
    { error::PN_RESOURCE_NOT_AVAILABLE,   "pn request resource not available"},
    { error::PEWRAPPER_STATUS_ERROR,      "pewrapper status errror"},
    { error::PEWRAPPER_ALREADY_EXIST,     "pewrapper already exists in pewrapper map"},

    { error::CONFIG_BAD_ARGUMENT,         "bad argument(s)"},
    { error::CONFIG_BAD_FILE,              "bad cfg file"},
    { error::CONFIG_ITERM_INVALID,        "invalid iterm"},
    { error::CONFIG_BAD_VALUE,            "bad config value"},

    { error::CLIENT_NOT_SUPPORTED,        "not supported cmd"},
    { error::CLIENT_CONNECT_ZK_FAIL,      "fail to connect zookeeper"},
    { error::CLIENT_CONNECT_PM_FAIL,      "fail to connect pm"},
    { error::CLIENT_INIT_TP_FAIL,         "fail to init topology"},
    { error::CLIENT_CHECK_TP_FAIL,        "fail to check topology"},
    { error::CLIENT_BUILD_TP_FAIL,        "fail to build topology"},
    { error::CLIENT_SUBMIT_APP_FAIL,      "fail to submit app"},
    { error::CLIENT_NOT_LOGIN_YET,        "not login yet"},
    { error::CLIENT_APPLY_APP_ID_FAIL,    "fail to apply app id from pm"},
    { error::CLIENT_NOTIFY_PM_FAIL,       "fail to notify pm"},
    { error::CLIENT_DECOMMISSION_APP_FAIL, "fail to decommission app"},
    { error::CLIENT_KILL_PE_FAIL,         "fail to kill pe"},
    { error::CLIENT_GET_APP_STATUE_FAIL,  "fail to get app stat"},
    { error::CLIENT_KILL_PN_FAIL,         "fail to kill pn"},
    { error::CLIENT_USER_OP_FAIL,         "fail to op user"},
    { error::CLIENT_MERGER_TOPO_FAIL,     "fail to merge sub tree into app"},
    { error::CLIENT_UPDATE_TOPO_FAIL,     "fail to update sub tree into app"},
    { error::CLIENT_FLOW_CONTROL_FAIL,    "fail to flow control app"},
    { error::CLIENT_UPDATE_PRO_FAIL,      "fail to update processor"},
    { error::CLIENT_VERSION_CHECK_FAIL,   "client version mismatched"},

    { error::MASTER_CONNECT_FAIL,         "fail to connect to master"},
    { error::NO_SUCH_REQUEST_URL,         "fail to find such request url"},
    { error::PM_RESOURCE_NOT_AVAILABLE,   "request resource not available"},

    { error::WATCH_FAIL,                   "fail to watch zk node" },

    { error::SEND_QUEUE_FULL,              "send queue is full" },
    { error::RECV_QUEUE_FULL,              "recv queue is full" },

    /* pe error */
    { error::BIGPIPE_CONNECTION_FAIL,      "fail to conn bigpipe" },
    { error::UNKNOWN_TUPLE_TYPE,           "unknown tuple type" },
    { error::UNKNOWN_PE_TYPE,              "unknown pe type" },
    { error::SERIAIL_TUPLE_FAIL,           "fail to serial tuple"},
    { error::DESERIAIL_TUPLE_FAIL,         "fail to deserial tuple"},

    /*transporter error*/
    { error::BIND_STUB_FAIL,               "fail to bind stub" },
    { error::NO_STUB_FOUND,                "fail to fine object's stub" },
    { error::SOURCE_ALREADY_EXIST,         "source already exists" },
    { error::SOURCE_NOT_EXIST,             "source not exists" },
    { error::ADD_SOURCE_TO_MAP_FAIL,       "fail to add source to map" },
    { error::POST_ESP_MSG_FAIL,            "fail to post esp msg" },
    { error::EXTRACT_TP_MSG_FAIL,          "fail to extract transporter msg" },
    { error::PUT_DATA_INTO_BUF_FAIL,       "fail to put data into buffer" },
    { error::DATA_IS_NULL,                 "fail to get data due to null" },
    { error::TP_ALREADY_EXIST,             "transporter already exists" },
    { error::TP_NOT_EXIST,                 "transporter not exists" },
    { error::TP_SEND_QUERY_FAIL,           "fail to send query" },
    { error::START_QUEUE_FAIL,             "fail to start queue" },
    { error::NOT_RUNNING,                  "not running" },
    { error::ALREADY_RUNNING,              "already running" },
    /* ring buffer error */
    { error::BUFFER_NOT_ENOUGH_DATA,       "ring buffer has not enough data" },
    { error::BUFFER_NO_PENDING_DATA,       "ring buffer no pending data" },

    { error::DROP_MSG_DUE_QUEUE_FULL,      "drop data due to queue full" },
    { error::DROP_MSG_DUE_ID_MISS_MATCH,   "drop data due to id miss match" },
    { error::DROP_MSG_DUE_ACK_MISS_MATCH,  "drop data due to ack miss match" },
    { error::DROP_MSG_DUE_ACK_STATUS_MISS_MATCH, "drop data due to ack status miss match" },
    { error::DROP_MSG_DUE_INFLOW_STATUS_MISS_MATCH, "drop data due to inflow status miss match" },
    { error::DROP_MSG_DUE_HAS_PENDING_DATA, "drop data due to has pending data" },
    { error::UNKNOWN_TP_STATUS,            "unknown transporter status" },
    { error::EXPIRED_MSG,                  "expired msg" },

    { error::SERIAIL_DATA_FAIL,            "fail to serial data"},
    { error::DESERIAIL_DATA_FAIL,          "fail to deserial data"},
    { error::INVALID_APP_NAME,             "App Name is invalid"},
    { error::REVISION_NOT_EXIST,           "revision does not exist"},
    { error::PROCESSOR_NOT_EXIST,          "processor does not exist"},

    { error::KYLIN_E_BEGIN, "invalid errcode, Kylin/Esp error code end edge, shoul not happen"},
    { error::KYLIN_E_END,   "invalid errcode, Kylin/Esp error code end edge, shoul not happen"},
    { error::HTTPD_FAILED,  "failed to start httpd server, perhaps the listen port was occupied"},
    { error::REQUEST_ERROR, "httpd request meet a internal error"},
    { error::USER_DEFINE_BEGIN,            "Message boundary, should not be used" }
};

typedef std::map<int32_t, const char*> TextMap;

TextMap& build_text_map() {
    TextMap* map = new TextMap();
    for (size_t i = 0; i < sizeof(error_info) / sizeof(ErrorInfo); i++) {
        if (NULL != error_info[i].text) {
            (*map)[error_info[i].code] = error_info[i].text;
        }
    }
    // init kylin error
    for (int err = -1; err > APFE_END; err--) {
        const char* str = APFError2Str(err);
        if (strcmp("Unknown error", str) != 0) {
            (*map)[error::KYLIN_E_BEGIN + err] = str;
        }
    }
    // init esp error
    for (int err = APFE_END; err > ESPE_END; err--) {
        const char* str = APFError2Str(err);
        if (strcmp("Unknown error", str) != 0) {
            (*map)[error::KYLIN_E_BEGIN + err] = str;
        }
    }
    return *map;
}

TextMap& text_map = build_text_map();


const char* error::get_text(int error) {
    const char* text = text_map[error];
    if (NULL == text) {
        return "ERROR NOT REGISTERED";
    }
    return text;
}

} // namespace dstream
