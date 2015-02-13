/** -*- c++ -*-
 * Copyright (C) 2011 Realtime Team (Baidu, Inc)
 *
 * This file is part of DStream.
 *
 * DStream is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 */

#ifndef __DSTREAM_CC_PROCESSNODE_PROLOG_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PROLOG_H__ // NOLINT

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>

#include "common/error.h"

namespace dstream {
namespace processnode {

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif
#define PIPE_OUTPUT_BUFFER_LEN 4096
#define CHECK_LIMIT_TIMEOUT     30 // wait 30s, then kill child

class Prolog {
public:
    Prolog() {}
    virtual ~Prolog() {}

    static error::Code RunProlog(uint64_t peid, const std::string& workdir, uint64_t last,
                                 const std::string& exe);
};

} // namespace processnode
} // namespace dstream

#endif // NOLINT
