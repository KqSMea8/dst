/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file common/cglimit_utils.h
 * @author zhenpeng(zhenpeng@baidu.com)
 * @date 2013/01/17 19:31:27
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef DSTREAM_COMMON_CGLIMIT_UTILS_H
#define DSTREAM_COMMON_CGLIMIT_UTILS_H 

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>

namespace dstream {

struct CpuStat {
    uint64_t group;
    uint64_t system;
    CpuStat(uint64_t g, uint64_t s):
        group(g), system(s) {}
};

struct MemStat {
    uint64_t group;
    MemStat(uint64_t m): 
        group(m) {}
};

class CGLimitUtils {
public:
    CGLimitUtils(int pid);
    void Init(int pid);
    ~CGLimitUtils() {}

    double GetCpuUsage(const CpuStat& last, const CpuStat& cur);
    CpuStat GetCurCpuUsage();
    MemStat GetMemUsage();

private:
    int read_file_(char *path, char *buffer, int len);
    std::string m_cg_mem_path;
    std::string m_cg_cpu_path;
}; // end class CGLimitUtils

} // end namespace dstream

#endif  //DSTREAM_COMMON_CGLIMIT_UTILS_H

/* vim: set ts=4 sw=4 sts=4 tw=100 */
