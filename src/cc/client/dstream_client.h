/**
* @file   dstream_client.h
* @brief    dstrean client is responsible for app management, include
*           app submission, app reconfiguration, app decommission,
*           PE migration and so on.
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_CLIENT_DSTREAM_CLIENT_H__ // NOLINT
#define __DSTREAM_CC_CLIENT_DSTREAM_CLIENT_H__ // NOLINT

#include <string>

#include "client/pm_client.h"
#include "common/proto/application.pb.h"

namespace dstream {
namespace client {
/** 
* @brief    client return code
* @author   liuguodong,liuguodong01@baidu.com
* @date     2013-07-24
*/
enum ClientReturnValue {
    kOk = 0,
    kCommandParseFailed          = -1,
    kInitConfigFileFailed        = -2,
    kRegisterZkFailed            = -3,
    kLoginPmFailed               = -4,
    kSetCommandFailed            = -5,
    kExecuteCommandFailed        = -6
};


/**
* @brief    command line arguments
* @author   konghui, konghui@baidu.com
* @date     2011-12-16
*/
struct CommandLineArgs {
    std::string action;
    std::string cfg_file;
    std::string pe;
    std::string app_dir;
    /***fangjun***/
    std::string username;
    std::string password;
    std::string cmd;
    bool        need_login_pm;
    int64_t     app_id;
    int         revision;
    std::string app_name;
    std::string processor;
    CommandLineArgs():
        need_login_pm(true),
        app_id(-1),
        revision(-1),
        processor() {}
};

extern int64_t g_app_id;
extern User    g_user;
extern CommandLineArgs  g_cmd_line_args;
extern PMClient*        g_pc;

inline bool is_valid_appname(const std::string app_name) {
    return ((app_name.find_first_not_of(
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "-_@0123456789.")
            == std::string::npos) &&
            (app_name.find_first_of(
                "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "_") == 0));
}
inline bool is_digit(const std::string digit) {
    return digit.find_first_not_of(
                "0123456789")
                == std::string::npos;
}

} // namespace client
} // namespace dstream

#endif // __DSTREAM_CC_CLIENT_DSTREAM_CLIENT_H__ //NOLINT
