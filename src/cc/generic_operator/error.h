/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file error.h
 * @author lanbijia
 * @date 2012/9/22
 * @brief defination of generic operator error
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_ERROR_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_ERROR_H__ // NOLINT

namespace dstream {
namespace generic_operator {
namespace error {
/**
 * @brief  error codes
 * @notice 1) negative number means error
 *         2) positive number means expected results
 */
enum Code {
    OK = 0,

    JOINER_UNINIT = -1,
    JOINER_ARGUMENT_INVALID = -2,
    JOINER_ID_NOT_EXIST = -3,
    JOINER_CONF_ERROR = -4,
    JOINER_WINDOW_INIT_ERROR = -5,
    JOINER_GET_WINDOW_FAIL = -6,
    JOINER_RECORD_DATA_FAIL = -7,
    JOINER_KEY_NOT_FOUND = -8,
    JOINER_ALLOC_WINDOW_FAIL = -9,
    JOINER_WINDOW_STORE_ERROR = -10,
};

} // namespace error
} // namespace generic_operator
} // namespace dstream

#endif // NOLINT
