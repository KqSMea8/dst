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

#ifndef __DSTREAM_CC_COMMON_LOGGER_H__ // NOLINT
#define __DSTREAM_CC_COMMON_LOGGER_H__ // NOLINT

#include <iostream>

#include "log4cplus/appender.h"
#include "log4cplus/configurator.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/fileappender.h"
#include "log4cplus/fstreams.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/property.h"
#include "log4cplus/layout.h"
#include "log4cplus/logger.h"
#include "log4cplus/loglevel.h"
#include "log4cplus/ndc.h"
#include "log4cplus/streams.h"
#include "log4cplus/syslogappender.h"

namespace Logging = log4cplus;

namespace dstream {
namespace logger {
using Logging::LogLevel;

void initialize(const std::string& name);
void initialize(const std::string& name, int level);
void initialize(const std::string& name, int level, bool flush_per_log);
void initialize(const std::string& name, int level, bool flush_per_log, std::ostream& out);
void close();

void set_level(int level);
void suppress_line_numbers();

extern Logging::Logger  s_logger;
extern bool             s_show_line_numbers;
extern size_t           kLogMaxLen;
} // namespace logger
namespace Logger = logger;  // !!!! abandont namespace!!!
} // namespace dstream

// printf interface macro helper

#define DSTREAM_LOG(_enabled_, _cat_, fmt, ...) do { \
    char buf[dstream::logger::kLogMaxLen]; \
    buf[dstream::logger::kLogMaxLen - 1] = '\0'; \
    if (dstream::logger::s_logger.isEnabledFor(Logging::_enabled_)) { \
        if (dstream::logger::s_show_line_numbers) { \
            snprintf(buf, dstream::logger::kLogMaxLen - 1, "[%s:%d]:" fmt, \
                     __FILE__, __LINE__, ##__VA_ARGS__); \
            dstream::logger::s_logger.log(Logging::_cat_, buf); \
        } else { \
            snprintf(buf, dstream::logger::kLogMaxLen - 1, fmt, ##__VA_ARGS__); \
            dstream::logger::s_logger.log(Logging::_cat_, buf);  \
        } \
    } \
} while (0)

// no stream interface macro helpers
// TODO(fangjun02)

// Logging macros interface starts here
#ifndef DSTREAM_DISABLE_LOG_ALL

#ifndef DSTREAM_DISABLE_LOG_DEBUG
#define DSTREAM_DEBUG(msg, ...) DSTREAM_LOG(DEBUG_LOG_LEVEL, DEBUG_LOG_LEVEL, msg, ##__VA_ARGS__)

#else
#define DSTREAM_DEBUG(msg, ...)
#endif // DSTREAM_DISABLE_LOG_DEBUG

#ifndef DSTREAM_DISABLE_LOG_INFO
#define DSTREAM_INFO(msg, ...) DSTREAM_LOG(INFO_LOG_LEVEL, INFO_LOG_LEVEL, msg, ##__VA_ARGS__)
#else
#define DSTREAM_INFO(msg, ...)
#endif // DSTREAM_DISABLE_LOG_INFO

#ifndef DSTREAM_DISABLE_LOG_WARN
#define DSTREAM_WARN(msg, ...) DSTREAM_LOG(WARN_LOG_LEVEL, WARN_LOG_LEVEL, msg, ##__VA_ARGS__)
#else
#define DSTREAM_WARN(msg, ...)
#endif // DSTREAM_DISABLE_LOG_WARN

#ifndef DSTREAM_DISABLE_LOG_ERROR
#define DSTREAM_ERROR(msg, ...) DSTREAM_LOG(ERROR_LOG_LEVEL, ERROR_LOG_LEVEL, msg, ##__VA_ARGS__)
#else
#define DSTREAM_ERROR(msg, ...)
#endif // DSTREAM_DISABLE_LOG_ERROR

#ifndef DSTREAM_DISABLE_LOG_FATAL
#define DSTREAM_FATAL(fmt, ...) DSTREAM_ERROR(fmt, ##__VA_ARGS__); abort()
#else
#define DSTREAM_FATAL(msg, ...)
#endif // DSTREAM_DISABLE_LOG_FATAL

#else // DSTREAM_DISABLE_LOGGING

#define DSTREAM_DEBUG(msg, ...)
#define DSTREAM_INFO(msg, ...)
#define DSTREAM_WARN(msg, ...)
#define DSTREAM_ERROR(msg, ...)
#define DSTREAM_FATAL(fmt, ...)

#endif // NOLINT

#endif // NOLINT


// !!!!! abandont !!!!
#define DSTREAM_NOTICE(fmt, ...)  DSTREAM_INFO(fmt, ##__VA_ARGS__)
#define DSTREAM_WARNING(fmt, ...) DSTREAM_WARN(fmt, ##__VA_ARGS__)
