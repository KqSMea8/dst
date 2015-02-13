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

#include "processnode/prolog.h"
#include <string>
#include "common/logger.h"

namespace dstream {
namespace processnode {

static int prolog_input() {
    return (open("/dev/null", O_RDONLY, 0));
}

error::Code Prolog::RunProlog(uint64_t peid, const std::string& workdir, uint64_t last,
                              const std::string& exe) {
    char* arg[5];
    int waitst;
    int real_output_len;
    int sig = 0;
    int script_ret = 0;

    int LastArg;
    int aindex;
    int rc;

    char output_buf[PIPE_OUTPUT_BUFFER_LEN];
    int buf_len = PIPE_OUTPUT_BUFFER_LEN;

    struct stat sbuf;
    rc = stat(exe.c_str(), &sbuf);
    if (rc == -1) {
        if (errno == ENOENT || errno == EBADF) {
            DSTREAM_ERROR("prolog(%s) does not exist.", exe.c_str());
            return error::FILE_NOT_FOUND;

        } else {
            DSTREAM_ERROR("prolog(%s) cannot stat.", exe.c_str());
            return error::FILE_NOT_FOUND;
        }
    }

    int fd_input = prolog_input();
    if (fd_input < 0) {
        return error::FILE_OPERATION_ERROR;
    }

    int  fd_output[2];
    if (pipe(fd_output) < 0) {
        return error::FILE_OPERATION_ERROR;
    }
    DSTREAM_INFO("peid(%lu) running prolog(%s)", peid, exe.c_str());

    // get maximum number of file descriptors
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        DSTREAM_WARN("get file limit error, set rl.rlim_max to 1024");
        rl.rlim_max = 1024;
    }

    int run_exit = 0;
    pid_t child = vfork();
    if (child > 0) {
        /* parent - watch for check_limit script to complete */
        close(fd_input);
        /* close write pipe */
        close(fd_output[1]);

        int total_sleep_time = CHECK_LIMIT_TIMEOUT * 1000000;
        const int sleep_step = 10000;
        int wait_ret = 0;
        while ((wait_ret = waitpid(child, &waitst, WNOHANG | WUNTRACED)) == 0
               && total_sleep_time > 0) {
            usleep(sleep_step);
            total_sleep_time -= sleep_step;
        }
        if (total_sleep_time <= 0) {
            // wait timeout
            if (kill(child, SIGKILL) < 0) {
                DSTREAM_WARN("kill process id(%u) fail, errno(%d)", child, errno);
                // cannot kill prolog/epilog, give up
                run_exit = -5;
            } else {
                // wait again, must success
                wait_ret = waitpid(child, &waitst, WNOHANG);
                run_exit = -4;
            }
        } else {
            if (wait_ret < 0) {
                run_exit = -3;
            } else {
                // check wait result
                // check child traced
                if (WIFSTOPPED(waitst)) {
                    // child traced
                    sig = WSTOPSIG(waitst);
                    run_exit = -3;
                } else if (WIFEXITED(waitst)) {
                    // normal exit
                    // child process must return >= 0
                    script_ret = WEXITSTATUS(waitst);
                } else if (WIFSIGNALED(waitst)) {
                    // exit abnormal
                    sig = WTERMSIG(waitst);
                    run_exit = -3;
                }
            }
        }
        if (run_exit == 0) {
            /* read output from child's pipe */
            memset(output_buf, 0, buf_len);
            real_output_len = read(fd_output[0], output_buf, buf_len);
            buf_len = real_output_len;
        } else {
            buf_len = 0;
        }
        close(fd_output[0]);

    } else if (0 == child) {
        /* child - run script */
        if (fd_input != 0) {
            if (dup2(fd_input, 0) == -1) {}
            close(fd_input);
        }
        /* close read pipe */
        close(fd_output[0]);

        if (fd_output[1] != 0) {
            if (dup2(fd_output[1], 1) == -1) {
                DSTREAM_WARN("PE(%lu) exe prolog(%s) redirect stdout error(%s)",
                             peid, exe.c_str(), strerror(errno));
            }

            if (dup2(fd_output[1], 2) == -1) {
                DSTREAM_WARN("PE(%lu) exe prolog(%s) redirect stdout error(%s)",
                             peid, exe.c_str(), strerror(errno));
            }
            close(fd_output[1]);
        }
        // close all fds
        for (int i = 3; i != static_cast<int>(rl.rlim_max == RLIM_INFINITY ? 1024 : rl.rlim_max);
             i++) {
            close(i);
        }

        char pe_id[32]; // I think it's enough.
        snprintf(pe_id, sizeof(pe_id), "%zd", peid);
        char last_assign_time[64]; // I think it's enough.
        snprintf(last_assign_time, sizeof(last_assign_time), "%lu", last);

        arg[0] = const_cast<char*>(exe.c_str());
        arg[1] = pe_id;
        arg[2] = const_cast<char*>(workdir.c_str());
        arg[3] = last_assign_time; // 0 means run first time
        arg[4] = NULL;

        LastArg = 3;
        for (aindex = 0; aindex < LastArg; aindex++) {
            if (arg[aindex] == NULL) {
                strcpy(arg[aindex], "");
            }
        }  /* END for (aindex) */

        if (execv(exe.c_str(), arg) == -1) {
            DSTREAM_ERROR("PE(%lu) exe prolog(%s) unexpected execv error(%s)",
                          peid, exe.c_str(), strerror(errno));
        }
        fsync(2);
        _exit(255);
    } else {
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) fork error(%d)",
                      peid, exe.c_str(), errno);
        return error::FORK_EXEC_ERROR;
    }

    error::Code ret = error::OK;
    switch (run_exit) {
    case 0:
        /* SUCCESS */
        DSTREAM_INFO("PE(%lu) exec prolog(%s) success script_ret(%d)",
                     peid, exe.c_str(), script_ret);
        break;
    case -3:
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) child wait interrupted, sig(%d)",
                      peid, exe.c_str(), sig);
        ret = error::CHILD_WAIT_INTERRUPT;
        break;
    case -4:
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) child wait timeout, script_ret(%d)",
                      peid, exe.c_str(), script_ret);
        ret = error::CHILD_WAIT_TIMEOUT;
        break;
    case -5:
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) child wait timeout, cannot kill child",
                      peid, exe.c_str());
        ret = error::CHILD_WAIT_TIMEOUT;
        break;
    case 1:
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) not allowed", peid, exe.c_str());
        ret = error::SCRIPT_EXEC_NOT_ALLOWED;
        break;
    default:
        DSTREAM_ERROR("PE(%lu) exec prolog(%s) unknown error", peid, exe.c_str());
        ret = error::EXTERNAL;
        break;
    }  /* END switch (run_exit_) */
    return (ret);
}

} // namespace processnode
} // namespace dstream

