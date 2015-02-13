/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <stdio.h>

#include "common/dstream_optional.h"

namespace dstream {

std::map<std::string, optional_fun> optional_fun_map;

// Add a optional
void AddOptional(const std::string& optional, optional_fun fun) {
    if (optional.size() > 1 && optional[0] == '-') {
        if (optional[1] != '-') {
            optional_fun_map[optional] = fun;
            return;
        }
        if (optional.size() > 2 && optional[2] != '-') {
            optional_fun_map[optional] = fun;
            return;
        }
    }
    printf("[%s] add optional [%s] fail\n", __FUNCTION__, optional.c_str());
}

// parse optional
int32_t ParseOptional(int argc, char** argv) {
    int res = 0;
    std::map<std::string, optional_fun>::iterator find, end;
    end = optional_fun_map.end();
    for (int i = 0; i < argc; ++i) {
        find = optional_fun_map.find(argv[i]);
        if (find != end) {
            if (i + 1 < argc) {
                int opt_res;
                if (argv[i + 1][0] != '-') {
                    opt_res = (*find->second)(argv[i], argv[ i + 1]);
                    ++i;
                } else {
                    opt_res = (*find->second)(argv[i], 0);
                }
                if (opt_res != 0) {
                    res = opt_res;
                    printf("[%d:%s] deal with optional [%s] fail\n", __LINE__, __FUNCTION__,
                           find->first.c_str());
                }
            } else {
                int opt_res = (*find->second)(argv[i], 0);
                if (opt_res != 0) {
                    printf("[%d:%s] deal with optional [%s] fail\n",
                            __LINE__, __FUNCTION__, argv[i]);
                    res = opt_res;
                }
            }
        } else {
            if (strlen(argv[i]) > 4 && strncmp(argv[i], "--", 2) == 0) {
                std::string opt_string = argv[i];
                size_t pos = opt_string.find('=');
                if (pos != std::string::npos && pos + 1 < opt_string.size()) {
                    std::string key = opt_string.substr(0, pos);
                    std::string value = opt_string.substr(pos + 1);
                    if ((find = optional_fun_map.find(key)) != end) {
                        int opt_res = (*find->second)(key.c_str(), value.c_str());
                        if (opt_res < 0) {
                            printf("[%d:%s] deal with optional [%s] fail\n",
                                    __LINE__, __FUNCTION__, key.c_str());
                            res = opt_res;
                        }
                        continue;
                    }
                }
            }
            printf("[%d:%s] cannot deal optional [%s]\n", __LINE__, __FUNCTION__, argv[i]);
            if (res == 0) {
                res = -109;
            }
        }
    }
    return res;
}

} // namespace dstream
