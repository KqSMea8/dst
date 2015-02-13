/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file common/cglimit_utils.cc
 * @author zhenpeng01(zhenpeng01@baidu.com)
 * @date 2013/10/30 13:24:23
 * @version $Revision$ 
 * @brief 
 *  
 **/

#include "common/cglimit_utils.h"
#include "common/logger.h"
#include <errno.h>
#include <string.h>
#include <fstream>

namespace dstream {

CGLimitUtils::CGLimitUtils(int pid) {
    Init(pid);
}

void CGLimitUtils::Init(int pid) {
    char cgroup_path[1024];
    sprintf(cgroup_path, "/proc/%d/cgroup", pid);
    std::ifstream cgroup_file(cgroup_path); 

    if (!cgroup_file) {
        DSTREAM_WARN("open proc file(%s) failed, %s.", cgroup_path, strerror(errno));
        return;
    }

    std::string line;
    while (!cgroup_file.eof()) {   
        if (!getline(cgroup_file, line)) {
            break;
        }

        size_t pos = line.find("memory"); 
        if ( pos != std::string::npos) {    
            m_cg_mem_path = line.substr(pos + 7);
            continue;
        }

        pos = line.find("cpuacct");    
        if ( pos != std::string::npos) {    
            m_cg_cpu_path = line.substr(pos + 8);
            continue;
        }
    }                         
    cgroup_file.close(); 
    DSTREAM_DEBUG("mem path: %s, cpu path: %s\n", m_cg_mem_path.c_str(), m_cg_cpu_path.c_str());
}

double CGLimitUtils::GetCpuUsage(const CpuStat& last, const CpuStat& cur) {
    if (cur.group <= last.group || cur.system <= last.system) {
        return 0.0;
    }
    return (double)(cur.group - last.group) / (cur.system - last.system);
}

MemStat CGLimitUtils::GetMemUsage() {
    char path[128] = {0}, buffer[128] = {0};
    snprintf(path, sizeof(path), "/cgroup/memory/%s/memory.usage_in_bytes", m_cg_mem_path.c_str());
    if (read_file_(path, buffer, sizeof(buffer)) == 0) {
        return MemStat(strtoul(buffer, NULL, 10));
    }
    DSTREAM_WARN("read mem file(%s) failed.", path);
    return MemStat(0);
}

CpuStat CGLimitUtils::GetCurCpuUsage() {
    char path[128] = {0}, buffer[128] = {0};
    int64_t system_cpu = 0, cgroup_cpu = 0;

    // read group cpu
    snprintf(path, sizeof(path), "/cgroup/cpuacct/%s/cpuacct.usage", m_cg_cpu_path.c_str());
    if (read_file_(path, buffer, sizeof(buffer)) == 0) {
        cgroup_cpu = strtoul(buffer, NULL, 10);
    }

    // read system cpu
    snprintf(path, sizeof(path), "%s", "/cgroup/cpuacct/cpuacct.usage");
    if (read_file_(path, buffer, sizeof(buffer)) == 0) {
        system_cpu = strtol(buffer, NULL, 10);
    }

    return CpuStat(cgroup_cpu, system_cpu);
}

int CGLimitUtils::read_file_(char *path, char *buffer, int len)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        return -1;
    }

    int ret = read(fd, buffer, len);
    close(fd);
    if (ret < 0) {
        return -1;
    }
    buffer[ret] = '\0';
    return 0;
}
} // namespace dstream

/* vim: set ts=4 sw=4 sts=4 tw=100 */
