/** -*- c++ -*-
 * Copyright (C) 2011 Realtime Team (Baidu, Inc)
 *
 * This file is part of DStream.
 *
 * DStream is free software; you can redistribute it and/   or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License.
 */
#include "common/logger.h"
#include "common/utils.h"
#include "log4cplus/loggingmacros.h"

namespace dstream {
namespace logger {

size_t kLogMaxLen = 2048;

Logging::Logger s_logger;
bool s_show_line_numbers = true;

static void initLogger(const std::string& name, int level) {
    std::string pattern = "[%D{%Y-%m-%d %H:%M:%S}][%p][%c]%x:%m%n";
    std::auto_ptr<Logging::Layout> layout(new Logging::PatternLayout(pattern));
    Logging::SharedAppenderPtr appender(new Logging::ConsoleAppender());
    appender->setLayout(layout);
    appender->setName(name);
    s_logger = Logging::Logger::getInstance(LOG4CPLUS_TEXT(name));
    s_logger.setLogLevel(level);
    s_logger.addAppender(appender);
}

void initialize(const std::string& name) {
    initialize(name, Logging::INFO_LOG_LEVEL, false, std::cout);
}

void initialize(const std::string& name, int level) {
    initialize(name, level, false, std::cout);
}

void initialize(const std::string& name, int level, bool flush_per_log) {
    initialize(name, level, flush_per_log, std::cout);
}

void initialize(const std::string& name,
                int level,
                bool flush_per_log,
                std::ostream& out) {
    DSTREAM_UNUSED_VAL(flush_per_log);
    DSTREAM_UNUSED_VAL(out);

    // find config file
    std::string logfile = "";
    if (access("./log.conf", 0) == 0) {
        logfile = "./log.conf";
    } else {
        if (access("./conf/log.conf", 0) == 0) {
            logfile = "./conf/log.conf";
        }
    }
    if (logfile != "") {
        Logging::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(logfile.c_str()));
    } else {
        initLogger(name, level);
        return;
    }

    if (name.empty()) {
        s_logger = log4cplus::Logger::getRoot();
    } else {
        s_logger = Logging::Logger::getInstance(LOG4CPLUS_TEXT(name));
    }
}

void close() {
    s_logger.shutdown();
}

void set_level(int priority) {
    s_logger.setLogLevel(priority);
}

void suppress_line_numbers() {
    s_show_line_numbers = false;
}

} // namespace logger 
} // namespace dstream
