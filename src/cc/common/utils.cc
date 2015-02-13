/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#include "common/utils.h"

#include <string.h>

namespace dstream {

error::Code GetLocalhostIP(std::string* s, const char* ifrname_pattern) {
    if (NULL == s || NULL == ifrname_pattern) {
        return error::BAD_ARGUMENT;
    }

    (void)(ifrname_pattern); // not used
    struct ifaddrs* ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return error::COMM_SOCKET_ERROR;
    }

    error::Code founded = error::COMM_SOCKET_ERROR;
    for (ifa = ifaddr; NULL != ifa; ifa = ifa->ifa_next) {
        if (NULL == ifa->ifa_addr) {
            continue;
        }
        if (AF_INET == ifa ->ifa_addr->sa_family) { // IP4
            void* sin_addr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            char ip[INET_ADDRSTRLEN];
            const char* result = inet_ntop(AF_INET, sin_addr, ip, INET_ADDRSTRLEN);
            if (!result) {
                continue; // skip invalid address
            }
            if (strncmp(ifa->ifa_name, "lo", strlen("lo")) == 0) {
                continue; // skip loopback interface
            }

            *s = ip;
            founded = error::OK;
            break;
        }
    }
    freeifaddrs(ifaddr);
    return founded;
}

error::Code GetRemotehostIP(const std::string& remote_host, std::string& remote_ip) {
    if ("" == remote_host) {
        return error::BAD_ARGUMENT;
    }

    char ip[1024];
    error::Code res = error::OK;
    char dns_buf[8192];
    struct hostent hbuf;
    struct hostent* result = NULL;
    int herr = 0;
    int get_host_res = 0;
    if ((get_host_res = gethostbyname_r(remote_host.c_str(), &hbuf, dns_buf,
                                        sizeof(dns_buf), &result, &herr)) == 0 &&
        NULL != result) {
        res = error::FAILED_EXPECTATION;
        if (result->h_addrtype == AF_INET) {
            for (char** pptr = result->h_addr_list; *pptr != NULL; ++pptr) {
                if (inet_ntop(result->h_addrtype, *pptr,
                              ip, sizeof(ip)) >= 0) {
                    remote_ip = ip;
                    res = error::OK;
                } else {
                    //          DSTREAM_WARN("address to ip fail");
                }
            }
        } else {
            res = error::BAD_ARGUMENT;
        }
    } else {
        //    DSTREAM_WARN("get remote host name fail");
        res = error::COMM_SOCKET_ERROR;
    }
    return res;
}

void GetMachineInfo(int* processor_num,
                    uint64_t* total_memory, uint64_t* free_memory) {
    assert(NULL != processor_num);
    assert(NULL != total_memory);
    assert(NULL != free_memory);

    *processor_num = sysconf(_SC_NPROCESSORS_CONF);
    uint64_t page_size = sysconf(_SC_PAGESIZE);
    uint64_t total_page = sysconf(_SC_PHYS_PAGES);
    uint64_t free_page = sysconf(_SC_AVPHYS_PAGES);
    *total_memory = page_size * total_page;
    *free_memory = page_size * free_page;
}

uint64_t GetConfigMemory(std::string* memory_config) {
    uint64_t memory = static_cast<uint64_t>(-1);
    uint64_t base = 1;
    if (memory_config != NULL && memory_config->size() > 1) {
        if (memory_config->size() > 2 && *(memory_config->end() - 1) == 'B') {
            memory_config->erase(memory_config->end() - 1);
        }
        char ch = *(memory_config->end() - 1);
        switch (ch) {
        case 'K':
            base = 1024;
            break;
        case 'M':
            base = 1024 * 1024;
            break;
        case 'G':
            base = 1024 * 1024 * 1024;
            break;
        }
        if (base > 1) {
            memory_config->erase(memory_config->end() - 1);
        }
        memory = atoi(memory_config->c_str());
        if (memory > 0) {
            memory *= base;
        } else {
            memory = static_cast<uint64_t>(-1);
        }
    }
    return memory;
}

int32_t PrintPMVersion(const char* /*opt*/, const char* /*value*/) {
    PrintVersion("pm");
    exit(0);
}

void PrintVersion(const char* module_name) {
    printf("[%s] version is [%s], build time [%s]\n",
            module_name, VERSION, BUILD_TIME);
}

} // namespace dstream
