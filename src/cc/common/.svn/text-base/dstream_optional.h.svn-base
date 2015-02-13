/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/08/06
* @filename:    dstream_optional.h
* @author:      zhanggengxin
* @brief:       parse argument , this is not thread safe
*/

#ifndef __DSTREAM_CC_COMMON_DSTREAM_OPTIONAL_H__
#define __DSTREAM_CC_COMMON_DSTREAM_OPTIONAL_H__

#include <stdint.h>
#include <unistd.h>

#include <map>
#include <string>

namespace dstream {

// deal with optional : return 0 if success, the value may be 0
typedef int32_t (*optional_fun) (const char* opt, const char* value);

// the map is not thread safe
extern std::map<std::string, optional_fun> optional_fun_map;

// Add a optional
void AddOptional(const std::string& optional, optional_fun fun);

// parse optional
int32_t ParseOptional(int argc, char** argv);

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_DSTREAM_OPTIONAL_H__
