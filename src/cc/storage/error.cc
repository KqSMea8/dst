/**
 * @file    error.h
 * @brief   error infoÎÄ¼þ
 * @author  konghui
 * @version 1.0.0.1
 * @date    2012-10-25
 * Copyright (c) 2012, Baidu, Inc. All rights reserved.
 */

#include <stdint.h>
#include <map>
#include "storage/error.h"

namespace dstream {
namespace storage {

struct ErrVector {
    int32_t     code;
    const char* text;
};

ErrVector g_err_vectors[] = {
    {KEY_ALREADY_EXISTS, "key already exists" },
    { KEY_NOT_EXISTS,     "key not exists" },
    { OK,                 "ok" },
    { UNKNOWN,            "unknown error"},
    { NOT_READABLE,       "not readable"},
    { NOT_WRITABLE,       "not writable"},
    { ACCESS_DENY,        "access deny"},
    { TABLE_ALREADY_EXISTS, "table already exists"},
    { EXEC_CMD_ERR,       "fail to exec cmd"},
    { NOT_INIT_YET,       "not init yet"},
    { TIMEOUT,            "time out"},
    { SYNC_CONN_FAIL,     "fail to sync connect server"},
    { ASYNC_CONN_FAIL,    "fail to async connect server"},
    { VERSION_MISMATCH,   "version mismatch"},
    { DATA_CORRUPTED,     "data corrupted"},
    { NOT_ENOUGH_BUF,     "not enough buffer"},
    { NOT_ALL_OK,         "not ret all ok"},
    { UNKNOWN_TABLE_TYPE, "unknown table type"},
    { COPY_CFG_FAIL,      "fail to copy config"},
    { GET_NULL_REPLY,     "get NULL reply"},
    { GET_ERR_REPLY,    "get error reply"},
    { BAD_MEM_ALLOC,      "fail to alloc mem"},
    { SOCKET_ERR,         "socket err"},
    { FILE_OPER_ERR,      "file operate err"},
    { BAD_ARGS,           "bad arguments"},
    { USER_DEFINE_BEGIN,  "user defined error code start up"},
    {0, NULL}
};

typedef std::map<int32_t, const char*> ErrVecMap;

ErrVecMap& build_err_vec_map() {
    ErrVecMap* map = new ErrVecMap();
    for (int i = 0; g_err_vectors[i].text != 0; i++) {
        (*map)[g_err_vectors[i].code] = g_err_vectors[i].text;
    }
    return *map;
}

ErrVecMap& g_err_vec_map = build_err_vec_map();

const char* err_str(int error) {
    const char* text = g_err_vec_map[error];
    return text ? text : "UNKNOWN_REGISTERED_ERROR_CODE";
}

} // namespace storage
} // namespace dstream
