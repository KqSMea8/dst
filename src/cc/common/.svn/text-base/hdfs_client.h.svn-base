/**
* @file   hdfs_client.h
* @brief    hdfs client class
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date   2011-12-15
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/


#ifndef __DSTREAM_CC_PROCESSNODE_HDFS_CLIENT_H__
#define __DSTREAM_CC_PROCESSNODE_HDFS_CLIENT_H__

#include <string>
#include <vector>
#include "common/dstream_type.h"

namespace dstream {
namespace hdfs_client {

/**
* @brief    hdfs client
* @author   konghui, konghui@baidu.com
* @date   2011-12-15
*/

extern const uint32_t kMaxReturnBufferLen;

class HdfsClient {
public:
    HdfsClient() {}
    HdfsClient(std::string hp_path, int32_t retry_time)
        : m_hadoop_client_path(hp_path),
        m_retry_time(retry_time) {}
    virtual ~HdfsClient() {}

    virtual int32_t UploadFile(const std::string& local_file,
                               const std::string& remote_file, bool over_write = false) const;
    virtual int32_t DownloadFile(const std::string& remote_file,
                                 const std::string& local_file) const;
    virtual int32_t TestFile(const std::string& remote_file, bool& exist) const;
    virtual int32_t ListFile(const std::string& remote_file,
            std::vector<std::string>* file_list) const;
    virtual int32_t RemoveFile(const std::string& remote_file) const;
    virtual int32_t MakeFile(const std::string& src) const;
    virtual int32_t MoveFile(const std::string& src_file,
                             const std::string& dst_file) const;
    virtual int32_t TestAndRemoveDir(const std::string& dir) const;
    virtual int32_t CopyOverWriteFile(const std::string& src_file,
                                      const std::string& dst_file) const;
    static int32_t HPCmdExec(const std::string& cmd, int retry_time = 1,
                             char* res_buf = NULL,
                             int* ret_val = NULL);

private:
    std::string m_hadoop_client_path;
    int32_t     m_retry_time;
};
} // namespace hdfs_client
} // namespace dstream
#endif // __DSTREAM_CC_PROCESSNODE_HDFS_CLIENT_H__
