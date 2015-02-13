/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_COMMON_FILE_META_H__
#define __DSTREAM_CC_COMMON_FLIE_META_H__

#include <dirent.h>
#include <error.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include "common/error.h"
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {
namespace filemeta {


namespace fileutil {
static inline bool IsSpecialDir(const std::string& name) {
    return name == "." || name == "..";
}

static inline std::string TrimEnd(const std::string& str) {
    std::string::size_type pos = str.find_last_not_of(" \n\t");
    if (pos == std::string::npos) {
        return str;
    }
    return str.substr(0, pos + 1);
}
} // namespace fileutil

// Write Meta File . If mkdir_if_no_exist is set, this can only make a 1-deepth dir.
static inline error::Code WriteMeta(const std::string& path, const std::string& key,
        const std::string& value, bool overwrite = true, bool mkdir_if_no_exist = true) {
    if (mkdir_if_no_exist && access(path.c_str(), F_OK) != 0) {
        if (mkdir(path.c_str(), 0777) == 0) {
            DSTREAM_INFO("mkdir [%s] success for write meta file.", path.c_str());
        } else {
            DSTREAM_WARN("mkdir [%s] failed for write meta file.", path.c_str());
            return error::FILE_OPERATION_ERROR;
        }
    }
    std::string meta_path = path + "/" + key;
    const char* open_mod = overwrite ? "w" : "a+";
    FILE* fp = fopen(meta_path.c_str(), open_mod);
    if (fp == NULL) {
        return error::FILE_OPERATION_ERROR;
    }
    fprintf(fp, "%s", value.c_str());
    fclose(fp);
    return error::OK;
}

// get a meta from path to a string
static inline error::Code GetMeta(const std::string& path,
        const std::string& key, std::string* value) {
    std::string meta_path = path + "/" + key;
    std::ifstream file(meta_path.c_str());
    if (!file.is_open()) {
        DSTREAM_DEBUG("open meta file [%s] failed", meta_path.c_str());
        return error::FILE_NOT_FOUND;
    }
    std::stringstream streambuf;
    streambuf << file.rdbuf();
    *value = fileutil::TrimEnd(streambuf.str());
    DSTREAM_INFO("key is %s, value is %s", key.c_str(), value->c_str());
    return error::OK;
}

// get metas from Path to a map
static inline error::Code GetMeta(const std::string& path,
        std::map<std::string, std::string>* meta) {
    meta->clear();
    if (access(path.c_str(), F_OK) != 0) {
        return error::FILE_NOT_FOUND;
    }
    DIR* dir;
    dir = opendir(path.c_str());
    if (dir == NULL) {
        DSTREAM_WARN("opendir [%s] failed..tried to create it,but failed again", path.c_str());
        return error::FILE_NOT_FOUND;
    }
    std::string value;
    error::Code err;
    struct dirent dir_entry, *dir_ptr = NULL;
    while (readdir_r(dir, &dir_entry, &dir_ptr) == 0 && dir_ptr != NULL) {
        if (fileutil::IsSpecialDir(dir_ptr->d_name)) {
            continue;
        }
        err = GetMeta(path, dir_ptr->d_name, &value);
        if (err != error::OK) {
            DSTREAM_WARN("read meta file [%s] failed", dir_ptr->d_name);
            continue;
        }
        (*meta)[dir_ptr->d_name] = value;
    }
    closedir(dir);
    return error::OK;
}

static inline error::Code ClearMeta(const std::string& path, const std::string& key) {
    if (unlink((path + "/" + key).c_str()) == 0) {
        return error::OK;
    }
    return error::FILE_OPERATION_ERROR;
}

static inline void ClearMeta(const std::string& path,
        const std::map<std::string, std::string>& meta) {
    typedef std::map<std::string, std::string> meta_map;
    for (meta_map::const_iterator it = meta.begin(); it != meta.end(); it++) {
        if (ClearMeta(path, it->first) != error::OK) {
            DSTREAM_WARN("remove meta file [%s] failed", it->first.c_str());
        }
    }
}

} // namespace filemeta
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_FILE_META_H__
