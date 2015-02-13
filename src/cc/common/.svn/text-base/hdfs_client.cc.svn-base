/**
* @file   hdfs_client.cc
* @brief    hdfs client class
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/error.h"
#include "common/hdfs_client.h"
#include "common/logger.h"

namespace dstream {
namespace hdfs_client {

const static int32_t kRetryTimes = 3;
// i think that's enough
const uint32_t kMaxReturnBufferLen = 1024 * 1024;
const static uint32_t kMaxReturnValueLen = 1024;


int32_t HdfsClient::UploadFile(const std::string& local_file,
        const std::string& remote_file, bool over_write) const {
    int32_t ret = error::OK;
    DSTREAM_DEBUG("local file is %s, remote file is %s", local_file.c_str(), remote_file.c_str());

    if (over_write) {
        std::string filter_local_file;
        std::string::size_type position = local_file.find("*");
        if (position == local_file.npos) {
            filter_local_file = local_file;
        } else {
            filter_local_file = local_file.substr(0, position - 1);
        }
        ret = RemoveFile(remote_file);
        if (ret < error::OK) {
            DSTREAM_ERROR("fail to remove file item :[%s]", remote_file.c_str());
        } else {
            DSTREAM_DEBUG("remove file item :[%s]", remote_file.c_str());
        }

        if (IsDir(filter_local_file.c_str())) {
            ret = MakeFile(remote_file);
            if (ret < error::OK) {
                DSTREAM_ERROR("fail to remove file item :[%s]", remote_file.c_str());
            } else {
                DSTREAM_DEBUG("remove file item :[%s]", remote_file.c_str());
            }
        }
    }

    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -copyFromLocal ");
    cmd.append(local_file).append(" ").append(remote_file);
    int ret_val = 0;
    ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || ret_val != 0) {
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}

int32_t HdfsClient::MakeFile(const std::string& src) const {
    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -mkdir ");
    cmd.append(src);
    int ret_val = 0;
    int32_t ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || ret_val != 0) {
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}

int32_t HdfsClient::MoveFile(const std::string& src_file,
        const std::string& dst_file) const {
    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -mv ");
    cmd.append(src_file).append(" ").append(dst_file);
    int ret_val = 0;
    int32_t ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || ret_val != 0) {
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}

int32_t HdfsClient::TestAndRemoveDir(const std::string& dir) const {
    bool existed;
    if (TestFile(dir, existed) < error::OK) {
        return error::FILE_OPERATION_ERROR;
    }
    if (existed) {
        std::string cmd(m_hadoop_client_path);
        cmd.append("/bin/hadoop fs -rmr ");
        cmd.append(dir);
        int ret_val = 0;
        int32_t ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
        if (ret != error::OK || ret_val != 0) {
            return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
        }
        return error::OK;
    } else {
        return error::OK;
    }
}

int32_t HdfsClient::CopyOverWriteFile(const std::string& src_file,
        const std::string& dst_file) const {
    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -copyOverwrite ");
    cmd.append(src_file).append(" ").append(dst_file);
    int ret_val = 0;
    int32_t ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || ret_val != 0) {
        DSTREAM_DEBUG("hdfs client download err(%d)", ret_val);
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}


int32_t HdfsClient::DownloadFile(const std::string& remote_file,
        const std::string& local_file) const {
    int32_t ret = error::OK;

    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -copyToLocal ");
    cmd.append(remote_file).append(" ").append(local_file);
    int ret_val = 0;
    ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || ret_val != 0) {
        DSTREAM_WARN("hdfs client download ret(%d), cmd ret(%d)", ret, ret_val);
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}

int32_t HdfsClient::TestFile(const std::string& remote_file, bool& exist) const {
    int32_t ret = error::OK;

    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -test -e ");
    cmd.append(remote_file).append(";echo $?");
    char* res_buffer = new char[kMaxReturnBufferLen];
    if (NULL == res_buffer) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    memset(res_buffer, 0, kMaxReturnBufferLen);
    ret = HPCmdExec(cmd, m_retry_time, res_buffer);
    if (error::OK == ret) {
        int32_t cmd_res = atoi(res_buffer);
        exist = !cmd_res ? true : false;
    }
    delete [] res_buffer;
    return ret;
}

int32_t HdfsClient::ListFile(const std::string& remote_file,
        std::vector<std::string>* file_list) const {
    int32_t ret = error::OK;

    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -ls ");
    cmd.append(remote_file).append(" | awk '{if($8!=\"\") print $8}'");
    char* res_buffer = new char[kMaxReturnBufferLen];
    if (NULL == res_buffer) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    memset(res_buffer, 0, kMaxReturnBufferLen);
    ret = HPCmdExec(cmd, m_retry_time, res_buffer);
    if (error::OK == ret) {
        file_list->clear();
        char* last_s;
        char* p = strtok_r(res_buffer, "\n", &last_s);
        while (p) {
            std::string file_item(p);
            file_list->push_back(file_item);
            DSTREAM_DEBUG("list split :[%s]", file_item.c_str());
            p = strtok_r(NULL, "\n", &last_s);
        }
    }
    delete [] res_buffer;
    return ret;
}

int32_t HdfsClient::RemoveFile(const std::string& remote_file) const {
    std::string cmd(m_hadoop_client_path);
    cmd.append("/bin/hadoop fs -rmr ");
    cmd.append(remote_file);
    int ret_val = 0;
    int32_t ret = HPCmdExec(cmd, m_retry_time, NULL, &ret_val);
    if (ret != error::OK || (ret_val != 0 && ret_val != 255)) {
        return ret == error::OK ? error::FILE_OPERATION_ERROR : ret;
    }
    return error::OK;
}

int32_t HdfsClient::HPCmdExec(const std::string& cmd,
        int retry_time /*= 1*/,
        char* res_buf /*= NULL*/,
        int* ret_val /*=NULL*/) {
    int32_t ret = error::OK;
    FILE* fp = NULL;
    /* set signal operation */
    void (*old_cld_hdlr)(int signum);
    old_cld_hdlr = signal(SIGCHLD, SIG_DFL);
    if (SIG_ERR == old_cld_hdlr) {
        DSTREAM_WARN("fail to set SIGCHLD handler");
        return error::MASK_SIG_ERROR;
    }
    /* exec cmd */
    for (int32_t retry = 0; retry < retry_time; retry++) {
        DSTREAM_DEBUG("prepare to exec:[%s]", cmd.c_str());
        if ((fp = popen(cmd.c_str(), "r")) == NULL) {
            DSTREAM_WARN("[%d]th fail to exec cmd:[%s], error(%s)",
                    retry, cmd.c_str(), strerror(errno));
            continue;
        }
        if (res_buf) {
            size_t res_count = fread(res_buf, sizeof(char), kMaxReturnBufferLen, fp);
            DSTREAM_DEBUG("read [%zu] bytes from pipe", res_count);
            if (res_count != kMaxReturnBufferLen) {
                if (!feof(fp)) { // read error
                    pclose(fp);
                    ret = error::FILE_OPERATION_ERROR;
                    goto ERR_RET;
                }
                res_buf[res_count] = '\0';
                // eof
                DSTREAM_DEBUG("result pipe read eof");
            } else { // buffer full
                DSTREAM_WARN("result buffer full!!");
                pclose(fp);
                break;
            }
        }
        pclose(fp);
        ret = error::OK;
        goto ERR_RET;
    }
    ret = error::FORK_EXEC_ERROR;
ERR_RET:
    /* reset signal operation */
    if (NULL != old_cld_hdlr) {
        if (signal(SIGCHLD, old_cld_hdlr) == SIG_ERR) {
            DSTREAM_WARN("fail to reset SIGCHLD");
        }
    }
    return ret;
}

} // namespace hdfs_client
} // namespace dstream
