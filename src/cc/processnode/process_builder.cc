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
#include "processnode/process_builder.h"

#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>

#include "common/logger.h"
#include "processelement/context.h"
#include "processnode/process.h"
#include "processnode/process_manager.h"

namespace dstream {
namespace processnode {
char kCglimitCommand[] = "/bin/cglimit";

ProcessBuilder::ProcessBuilder(bool use_cglimit): m_cpu_usage(0), m_memory_limit(0),
    m_redirect_input_stream(false)\
    , m_redirect_output_stream(false), m_redirect_error_stream(false), m_use_cglimit(use_cglimit) {
}
void ProcessBuilder::cpu_usage(uint32_t percentage) {
    m_cpu_usage = percentage;
}
uint32_t ProcessBuilder::cpu_usage() const {
    return m_cpu_usage;
}
void ProcessBuilder::memory_limit(uint32_t kilobytes) {
    m_memory_limit = kilobytes;
}
uint32_t ProcessBuilder::memory_limit() const {
    return m_memory_limit;
}
std::map<std::string, std::string>& ProcessBuilder::Environment() {
    return m_environment;
}

void ProcessBuilder::SetEnvironment(const ProcessBuilder::EnvType& env) {
    m_environment = env;
}

bool ProcessBuilder::redirect_error_stream() const {
    return m_redirect_error_stream;
}
void ProcessBuilder::redirect_error_stream(bool redirect) {
    m_redirect_error_stream = redirect;
}
bool ProcessBuilder::redirect_input_stream() const {
    return m_redirect_input_stream;
}
void ProcessBuilder::redirect_input_stream(bool redirect) {
    m_redirect_input_stream = redirect;
}
bool ProcessBuilder::redirect_output_stream() const {
    return m_redirect_output_stream;
}
void ProcessBuilder::redirect_output_stream(bool redirect) {
    m_redirect_output_stream = redirect;
}

void ProcessBuilder::SetResourceLimit(const Processor& processor) {
    const Resource& res = processor.resource_require();
    if (processor.has_resource_require()) {
        if (res.has_cpu()) {
            float cpu = res.cpu();
            if (cpu > 1 || cpu < 0) {
                DSTREAM_WARN("cpu usage must be greater than 0 and not greater than 1");
            } else {
                DSTREAM_INFO("set pe cpu usage %.2f ", cpu);
                cpu_usage(static_cast<int>(cpu * 100));
            }
        }
        if (res.has_memory()) {
            // limit/cglimit limit memory
            memory_limit(res.memory() / 1024);
        }
    }
}

char* new_dup_str(const char* old) {
    int len = strlen(old) + 1;
    char* str = new char[len];
    strncpy(str, old, len);
    return str;
}
char** ProcessBuilder::CreateExecArgv(const std::string& commandline, bool noCglimit)const {
    // const uint32_t kMaxIntStrSize = 30;
    // need to translate a integer to an char[].
    const uint32_t kMaxCglimitArgNum =
        10; // it doesn't contain the arg number of the new process commandline.
    uint32_t top = 0;
    // the number 3 is for the NULL ptr and cglimit and the target commandline
    char** array_for_exec_argv = new
    char*[kMaxCglimitArgNum + 3];
    // if noCglimit is set ,use "bash -c cmdline" to run the process.
    if (noCglimit) {
        // find limit3 in path
        // if(access("/bin/limit3", X_OK) != 0) {
        // not find limit3 in system, so use /bin/bash
        array_for_exec_argv[top++] = new_dup_str("/bin/bash");
        array_for_exec_argv[top++] = new_dup_str("-c");
        array_for_exec_argv[top++] = new_dup_str(commandline.c_str());
        /*} else {
        //use /bin/limit3
        array_for_exec_argv[top++] = new_dup_str("/bin/limit3");
        array_for_exec_argv[top++] = new_dup_str("-m");
        array_for_exec_argv[top++] = new_dup_str((Convert<std::string>(m_memory_limit)).c_str());
        array_for_exec_argv[top++] = new_dup_str(commandline.c_str());
        }*/

    } else {
        array_for_exec_argv[top++] = new_dup_str(kCglimitCommand);
        if (m_cpu_usage != 0) {
            array_for_exec_argv[top++] = new_dup_str("-C");
            array_for_exec_argv[top++] = new_dup_str(NumberToString(m_cpu_usage).c_str());
        }
        if (m_memory_limit != 0) {
            array_for_exec_argv[top++] = new_dup_str("-M");
            array_for_exec_argv[top++] = new_dup_str(
                (NumberToString(m_memory_limit) + "K").c_str());
        }
        array_for_exec_argv[top++] = new_dup_str(commandline.c_str());
    }
    array_for_exec_argv[top] = NULL;
    std::string dbgline;
    for (uint32_t i = 0; i != top; i++) {
        dbgline += std::string("[") + array_for_exec_argv[i] + "]";
    }
    DSTREAM_INFO("create exec argvs %s", dbgline.c_str());
    return array_for_exec_argv;
}

char** ProcessBuilder::CreateExecEnv() const {
    uint32_t top = 0;
    char** array_for_exec_env = new char*[m_environment.size() + 1];
    typedef std::map<std::string, std::string>::const_iterator Iterator;
    uint32_t str_size = 0;
    for (Iterator iter = m_environment.begin(); iter != m_environment.end(); ++iter) {
        str_size = iter->first.size() + iter->second.size() + 2;
        array_for_exec_env[top] = new char[str_size];
        std::string kv = iter->first + "=" + iter->second;
        strncpy(array_for_exec_env[top], kv.c_str(), str_size);
        array_for_exec_env[top][str_size - 1] = 0;
        top++;
    }
    array_for_exec_env[top] = NULL;
    return array_for_exec_env;
}

void* thread_function(void* data) {
    int status = 0;
    int pid = reinterpret_cast<int64_t>(data);

    pthread_detach(pthread_self());

    ProcessManager::ProcessPtr child_process = ProcessManager::GetInstance()->QueryProcess(pid);
    if (child_process) {
        DSTREAM_INFO("start waiting process %d", pid);
        OnExitCallback* callback = child_process->GetOnExitCallback();
        if (waitpid(pid, &status, 0) < 0) {
            DSTREAM_ERROR("waitpid failed , pid [%d],strerror:[%s]", pid , strerror(errno));
        }

        ProcessManager::GetInstance()->DestroyProcess(pid);
        DSTREAM_INFO("process %d exited", pid);

        if (callback != NULL) {
            callback->Call(pid, status, WEXITSTATUS(status), WIFEXITED(status));
            DSTREAM_INFO("called the user's callback successfully for pid [%d]", pid);
            delete callback;
            callback = NULL;
        }
    } else {
        DSTREAM_WARN("process pid(%d) not found in pid->process map", pid);
    }

    return NULL;
}
// create a new process use first param "commandline"
// and it will call the callback which is pointed by the second param
// in a new thread when the child process is running over.
ProcessManager::ProcessPtr ProcessBuilder::Start(const std::string& commandline,
                                                 OnExitCallback* on_exit_callback,
                                                 OnStartCallback* on_start_callback) {
    ProcessManager::ProcessPtr get_proc;
    do {
        int32_t pid = 0;
        bool noCglimit = !m_use_cglimit;
        DSTREAM_DEBUG("use cglimit [%s]", m_use_cglimit ? "Yes" : "No");
        if (!noCglimit && access(kCglimitCommand, X_OK) != 0) {
            DSTREAM_WARN("cglimit not found in path %s", kCglimitCommand);
            noCglimit = true;
        }
        if (commandline.empty()) {
            DSTREAM_WARN("commandline shouldn't be empty");
            break;
        }
        int in_pipe[2], out_pipe[2], error_pipe[2];
        if (m_redirect_input_stream) {
            if (pipe(in_pipe) != 0) {
                DSTREAM_WARN("Create Process Failed,redirect inputstream failed");
                break;
            }
        }
        if (m_redirect_output_stream) {
            if (pipe(out_pipe) != 0) {
                DSTREAM_WARN("Create Process Failed,redirect outputstream failed");
                break;
            }
        }
        if (m_redirect_error_stream) {
            if (pipe(error_pipe) != 0) {
                DSTREAM_WARN("Create Process Failed,redirect errorstream failed");
                break;
            }
        }
        std::string pwd;
        std::map<std::string, std::string>::iterator it;
        it = m_environment.find(processelement::internal::Context::kEnvKeyWorkDirectory);
        if (it != m_environment.end()) {
            pwd = it->second;
        } else {
            pwd = ".";
        }
        char* ld_library_path_env = getenv("LD_LIBRARY_PATH");
        std::string export_cmd = "export LD_LIBRARY_PATH=";
        export_cmd += ld_library_path_env;
        export_cmd = export_cmd + "; exec ";
        // std::string export_cmd = "";
        m_environment["LD_LIBRARY_PATH"] = ld_library_path_env ? ld_library_path_env : "";
        std::string exec_cmd = export_cmd + commandline;
        char** argv = CreateExecArgv(exec_cmd, noCglimit);
        char** env  = CreateExecEnv();
        // get maximum number of file descriptors
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
            DSTREAM_WARN("get file limit error, set rl.rlim_max to 1024");
            rl.rlim_max = 1024;
        }

        if ((pid = fork()) == 0) {
            // Close all open file descriptors, except 0,1,2
            // now the m_redirect_output_stream ... will not work .
            if (rl.rlim_max == RLIM_INFINITY) {
                rl.rlim_max = 1024;
            }
            for (int i = 3; i != static_cast<int>(rl.rlim_max); i++) {
                if (!( m_redirect_input_stream && (in_pipe[0] == i || in_pipe[1] == i )
                      || m_redirect_output_stream && (out_pipe[0] == i || out_pipe[1] == i)
                      || m_redirect_error_stream && (error_pipe[0] == i || error_pipe[1] == i)
                    )) {
                    close(i);
                }
            }

            if (m_redirect_input_stream) {
                close(in_pipe[1]);
                dup2(in_pipe[0], STDIN_FILENO);
            }
            if (m_redirect_output_stream) {
                close(out_pipe[0]);
                dup2(out_pipe[1], STDOUT_FILENO);
            }
            if (m_redirect_error_stream) {
                close(error_pipe[0]);
                dup2(error_pipe[1], STDERR_FILENO);
            }
            chdir(pwd.c_str());
            execve(argv[0], argv, env);
            std::cerr << ("UNKNOWN ERROR,Create child process failed in ProcessBuilder::Start");
            exit(-1);
        }
        DSTREAM_INFO("fork: child pid [%d]", pid);
        for (uint32_t i = 0; argv[i] != NULL; i++) {
            delete [] argv[i];
            argv[i] = NULL;
        }
        delete [] argv;
        for (uint32_t i = 0; env[i] != NULL; i++) {
            delete [] env[i];
            env[i] = NULL;
        }
        delete [] env;
        if (pid < 0) {
            break;
        }
        get_proc = ProcessManager::GetInstance()->CreateProcess(pid, on_exit_callback);
        if (!get_proc) {
            DSTREAM_ERROR("create process failed.");
            break;
        }
        get_proc->m_input_stream = m_redirect_input_stream ?
                                   (close(in_pipe[0]), in_pipe[1]) : 0;
        get_proc->m_output_stream = (m_redirect_output_stream) ?
                                    (close(out_pipe[1]), out_pipe[0]) : 0;
        get_proc->m_error_stream = (m_redirect_error_stream) ?
                                   (close(error_pipe[1]), error_pipe[0]) : 0;
        if (on_start_callback != NULL) {
            on_start_callback->Call(get_proc);
            delete on_start_callback;
            on_start_callback = NULL;
        }
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, thread_function, reinterpret_cast<void*>(pid)) != 0) {
            DSTREAM_WARN("create wait thread failed when create process");
            break;
        }
        get_proc->m_wait_thread_id = thread_id;
        return get_proc;
    } while (0);

    if (on_start_callback) {
        delete on_start_callback;
        on_start_callback = NULL;
    }
    if (on_exit_callback) {
        delete on_exit_callback;
        on_exit_callback = NULL;
    }
    return get_proc;
}


} // namespace processnode
} // namespace dstream
