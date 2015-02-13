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

#ifndef __DSTREAM_CC_PROCESSNODE_PROCESS_BUILDER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PROCESS_BUILDER_H__ // NOLINT

#include <map>
#include <string>

#include "common/dstream_type.h"
#include "common/proto/application.pb.h"
#include "processnode/process_manager.h"

namespace dstream {
namespace processnode {

class Process;
class OnExitCallback;
class OnStartCallback;

class ProcessBuilder {
public:
    typedef std::map<std::string, std::string> EnvType;
    explicit ProcessBuilder(bool use_cglimit = true);

    void cpu_usage(uint32_t percentage);
    uint32_t cpu_usage() const;
    void memory_limit(uint32_t kilobytes);
    uint32_t memory_limit() const;
    EnvType& Environment();
    void SetEnvironment(const EnvType& env);
    ProcessManager::ProcessPtr Start(const std::string& commandline,
                                     OnExitCallback* on_exit_callback = NULL,
                                     OnStartCallback* on_start_callback = NULL);

    void redirect_error_stream(bool redirect);
    bool redirect_error_stream() const;
    void redirect_input_stream(bool redirect);
    bool redirect_input_stream() const;
    void redirect_output_stream(bool redirect);
    bool redirect_output_stream() const;

    // add by lanbijia
    // 2012-7-9
    void SetResourceLimit(const Processor& processor);

private:
    char** CreateExecArgv(const std::string& commandline, bool noCglimit = false) const;
    char** CreateExecEnv() const;
    uint32_t m_cpu_usage;
    uint32_t m_memory_limit;
    std::map<std::string, std::string> m_environment;
    bool m_redirect_input_stream;
    bool m_redirect_output_stream;
    bool m_redirect_error_stream;
    bool m_use_cglimit;
}; // class ProcessBuilder

} // namespace processnode
} // namespace dstream

#endif // NOLINT

