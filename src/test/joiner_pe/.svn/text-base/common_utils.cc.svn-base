/**
 * @file common_utils.cpp
 * @Brief  通用工具函数集
 * @author zhenpeng01@baidu.com
 * @version 1.0.0
 * @date 2012-10-17
 */

#include "common_utils.h"


uint64_t GetStoreID(const uint64_t& id) {
    return (id & (static_cast<uint64_t>(-1) >> (sizeof(uint64_t) * 8 - 40)));
}

int32_t SplitStringEx(const std::string& str, const char token, std::vector<std::string>* vec) {
    std::string::size_type beg = 0;
    std::string::size_type end = 0;
    int32_t i;
    for (i = 0; ; i++) {
        while (beg != std::string::npos && str[beg] == token ) {
            beg++;
        }
        end = str.find_first_of(token, beg);
        if (end == std::string::npos) {
            break;
        }
        if (end != beg) {
            if (!str.substr(beg, end - beg).empty()) {
                vec->push_back(str.substr(beg, end - beg));
            }
        }
        beg = end + 1;
    }

    if (!str.substr(beg, end - beg).empty()) {
        vec->push_back(str.substr(beg));
        i++;
    }

    return i;
}

