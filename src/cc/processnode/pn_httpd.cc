/** 
* @file     pn_httpd.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-08-27
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/
#include "boost/algorithm/string/trim.hpp"
#include "processnode/pe_wrapper_manager.h"
#include "processnode/process_node.h"
#include "processnode/pn_httpd.h"
#include "common/utils.h"
namespace dstream {
namespace pn_httpd {
dstream::error::Code ReadFileContent(const char* file_name, std::string *output) {
    assert(NULL != output);
    std::string file_buf = ""; 
    int fd = open(file_name, O_RDONLY);
    if (fd < 0) {
        DSTREAM_WARN("open file error:[%s]", file_name);
        return dstream::error::LOCAL_IO_ERROR;
    }
    char buf[1024];
    ssize_t read_bytes = 0;
    while((read_bytes = read(fd, buf, 1023)) > 0) {
        if (read_bytes <= 1023) {
            buf[read_bytes] = '\0';
        } else {
            DSTREAM_WARN("unexpected read file error:[%s]", file_name);
            return dstream::error::LOCAL_IO_ERROR;
        }
        file_buf += buf;
    }
    if(read_bytes < 0) {
        DSTREAM_WARN("read file error:[%s]", file_name);
        close(fd);
        return dstream::error::LOCAL_IO_ERROR;
    } else {
        close(fd);
        
        boost::algorithm::trim_if(file_buf, boost::is_any_of("\n\t "));
        output->assign(file_buf);
        return dstream::error::OK;
    }
}
// we assure a counter a is only vaild in 30 seconds, 
static const uint32_t kCounterMaxVaildTime = 30;
dstream::error::Code PnHttpdResponder::GetTransporterData(std::string* output) {
    assert(NULL != output);
    const char* path = "./counter";
    std::string json_str = "{";
    // check if file is validate
    struct stat buf;
    if (stat(path, &buf) < 0) {
        DSTREAM_WARN("could not open directory[%s]",path);
        return dstream::error::REQUEST_ERROR;
    }
    if (!S_ISDIR(buf.st_mode)) {
        DSTREAM_WARN("counter path[%s] is not a dir", path);
        return dstream::error::REQUEST_ERROR;
    }
    DIR* dirp;
    struct dirent dir_entry;
    struct dirent* pdir_result = NULL;
    if ((dirp = opendir(path)) == NULL) {
        DSTREAM_WARN("open dir error[%s]", path);
        return dstream::error::REQUEST_ERROR;
    }
    while (!readdir_r(dirp, &dir_entry, &pdir_result) && pdir_result != NULL) {
        if (strcmp(dir_entry.d_name, ".") == 0 ||
            strcmp(dir_entry.d_name, "..") == 0 ||
            strcmp(dir_entry.d_name, ".svn") == 0) {
            continue;
        }
        std::string file = dir_entry.d_name;
        std::string ab_path = path;
        ab_path += "/";
        ab_path += file;
        const std::string file_type = ".cnt.json";
        size_t found = file.rfind(file_type);
        if(found != std::string::npos &&
               file.length() > found && 
               file.length() - found == file_type.length()) {
            if(stat(ab_path.c_str(), &buf) < 0) {
                DSTREAM_WARN("can not open file[%s]", ab_path.c_str());
                return dstream::error::REQUEST_ERROR;
            }
            uint64_t now = static_cast<uint64_t>(time(NULL));
            uint64_t file_modify_time = static_cast<uint64_t>(buf.st_mtime);
            if ((now - file_modify_time) > kCounterMaxVaildTime) { // kCounterMaxVaildTime
                DSTREAM_WARN("counter file too old, ignored! [%s] which is %zu, now %zu",
                             ab_path.c_str(), file_modify_time, now);
                continue;
            }
            std::string content;
            if( ReadFileContent(ab_path.c_str(), &content) < dstream::error::OK) {
                DSTREAM_WARN("read counter file failed: [%s]", ab_path.c_str());
                continue;
            } else {
                std::string counter_name;
                size_t pos = file.find('.');
                if (pos != std::string::npos) {
                    counter_name = "\"";
                    counter_name += file.substr(0, pos);
                    counter_name += "\"";

                } else {
                    DSTREAM_WARN("parser counter file name error,"
                    " could not find '.' pos:[%s]", file.c_str());
                    continue;
                }
                json_str += counter_name;
                json_str += ":";
                json_str += content;
                json_str += ",";
            }

        }
    }
    closedir(dirp);
    size_t len = json_str.length();
    if(len > 0 && json_str[len - 1] == ',') {
        json_str[len - 1] = '}';
    } else {
        json_str.append("}");
    }
    output->assign(json_str);
    return dstream::error::OK;
}
dstream::error::Code PnHttpdResponder::GetPEWrapperMgrData(std::string* output) {
    assert(NULL != output);
    DSTREAM_DEBUG("httpd handle a request for getting pe wrapper manager info");
    dstream::processnode::PEWrapperManager::PEWrapperManagerPtr 
             pe_wrapper_manager_ptr = 
                    dstream::processnode::PEWrapperManager::GetPEWrapperManager();
    if (pe_wrapper_manager_ptr == NULL) {
        return dstream::error::PN_RESOURCE_NOT_AVAILABLE;
    } else {
        return pe_wrapper_manager_ptr->Dump(output);
    }
}

dstream::error::Code PnHttpdResponder::GetRootData(std::string* output) {
    assert(NULL != output);
    std::string ret_buf;
    ret_buf = "/transporter,/pewrapper";
    output->assign(ret_buf);
    return dstream::error::OK;
}

PnHttpdResponder::PnHttpdResponder() {
    m_request_map.clear();
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/",
                                            PnHttpdResponder::GetRootData));
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/transporter",
                                            PnHttpdResponder::GetTransporterData));
    m_request_map.insert(std::pair<std::string, httpd::DataRequestHandler>("/pewrapper",
                                            PnHttpdResponder::GetPEWrapperMgrData));

}

} // namespace pm_httpd
} // namespace dstream
