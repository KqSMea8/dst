/**
 * @file    error.h
 * @brief   error infoÎÄ¼þ
 * @author  konghui
 * @version 1.0.0.1
 * @date    2012-10-25
 * Copyright (c) 2012, Baidu, Inc. All rights reserved.
 */

#ifndef __DSTREAM_STORAGE_ERROR_H__ // NOLINT
#define __DSTREAM_STORAGE_ERROR_H__ // NOLINT

namespace dstream {
namespace storage {

/**
* @brief    error code
*/
enum ErrCode {
    KEY_ALREADY_EXISTS  = 2,
    KEY_NOT_EXISTS      = 1,
    OK                  = 0,

    UNKNOWN             = -1,

    /* redis cluster error */
    NOT_READABLE        = -10,
    NOT_WRITABLE        = -11,
    ACCESS_DENY         = -13,
    TABLE_ALREADY_EXISTS = -14,
    EXEC_CMD_ERR        = -15,

    /* redis client error */
    NOT_INIT_YET        = -100,
    TIMEOUT             = -101,
    SYNC_CONN_FAIL      = -102,
    ASYNC_CONN_FAIL     = -103,
    VERSION_MISMATCH    = -104,
    DATA_CORRUPTED      = -105,
    NOT_ENOUGH_BUF      = -106,
    NOT_ALL_OK          = -107,
    UNKNOWN_TABLE_TYPE  = -108,
    COPY_CFG_FAIL       = -109,
    GET_NULL_REPLY      = -110,
    GET_ERR_REPLY     = -111,

    /* sys error */
    BAD_MEM_ALLOC       = -200,
    SOCKET_ERR          = -201,
    FILE_OPER_ERR       = -202,

    /* parameters error */
    BAD_ARGS            = -301,

    /* user defined error code start up */
    USER_DEFINE_BEGIN   = -1000
};

/**
* @brief    translate error code int string
* @param    error[in] error code
* @return    NULL or pointer to error string
*/
const char* err_str(int error);
} // namespace storage
} // namespace dstream

#endif // __DSTREAM_STORAGE_ERROR_H__ //NOLINT
