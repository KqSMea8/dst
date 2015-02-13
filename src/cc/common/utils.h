/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#ifndef __DSTREAM_CC_COMMON_UTILS_H__ // NOLINT
#define __DSTREAM_CC_COMMON_UTILS_H__ // NOLINT
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <map>
#include <sstream>
#include <string>

#include "atomic.h" // from kylin NOLINT
#include "common/error.h"
#include "common/logger.h"

namespace dstream {

// for kylin, add by konghui
#ifndef __GNU_LIBRARY__
#define __GNU_LIBRARY__
#endif

// force inline, add by konghui
#ifndef DSTREAM_FORCE_INLINE
#define DSTREAM_FORCE_INLINE inline __attribute__((always_inline))
#endif

// ------------------------------------------------------------
// language stuff.
#ifndef BEGIN_CDECLS
#ifdef __cplusplus
#define BEGIN_CDECLS extern "C" {
#define END_CDELS }
#else
#define BEGIN_CDECLS
#define END_CDELS
#endif
#endif

#define DSTREAM_UNUSED_VAL(V) ((void) V)

#ifdef DSTREAM_PROFILE
#define TIMER_PRINT_PREFIX struct timeval begintv, endtv;
#define TIMER_PRINT_BEGIN() gettimeofday(&begintv, NULL)
#define TIMER_PRINT_END(str)  do { \
            gettimeofday(&endtv, NULL); \
            DSTREAM_WARN("DSTREAM PROFILER: %s: %lu usec\n", str, \
                         ((endtv.tv_sec - begintv.tv_sec) * 1000000 + \
                          (endtv.tv_usec - begintv.tv_usec))); \
} while (0)
#define TIMER_PRINT(str)  do { \
            struct timeval _now_intv; \
            gettimeofday(&_now_intv, NULL); \
            DSTREAM_WARN("DSTREAM PROFILER: %s: %lu usec\n", str, \
                         (_now_intv.tv_sec * 1000000 + _now_intv.tv_usec)); \
} while (0)
#else
#define TIMER_PRINT_PREFIX
#define TIMER_PRINT_BEGIN()
#define TIMER_PRINT_END(str) do {} while(0) //NOLINT
#define TIMER_PRINT(str) do {} while(0)     //NOLINT
#endif  //  DSTREAM_PROFILE

#ifdef DSTREAM_PROFILE
#define _DS_TIMER_PREFIX() struct timeval _ds_beg_tv, _ds_end_tv;
#define _DS_TIMER_BEGIN() gettimeofday(&_ds_beg_tv, NULL)
#define _DS_TIMER_END_STR(fmt, ...) \
    do { \
        gettimeofday(&_ds_end_tv, NULL); \
        DSTREAM_INFO("DS_PROFILER: %lu us: "fmt, \
                     ((_ds_end_tv.tv_sec - _ds_beg_tv.tv_sec) * 1000000 + \
                     (_ds_end_tv.tv_usec - _ds_beg_tv.tv_usec)), ##__VA_ARGS__); \
    } while (0);
#define _DS_TIMER_END(throttle) \
    do { \
        gettimeofday(&_ds_end_tv, NULL); \
        uint64_t lag = (_ds_end_tv.tv_sec - _ds_beg_tv.tv_sec) * 1000000 + \
                       (_ds_end_tv.tv_usec - _ds_beg_tv.tv_usec); \
        UNLIKELY_IF(lag >= throttle) {\
            DSTREAM_INFO("DS_PROFILER: %lu us: ", lag);\
        } \
    } while (0);
#else
#define _DS_TIMER_PREFIX()
#define _DS_TIMER_BEGIN()
#define _DS_TIMER_END_STR(fmt, ...)
#define _DS_TIMER_END(throttle)
#endif

// (zhangyan04):steal google protobuf code.
// Return a mutable char* pointing to a string's internal buffer,
// which may not be null-terminated. Writing through this pointer will
// modify the string.
//
// StringAsArray(&str)[i] is valid for 0 <= i < str.size() until the
// next call to a string method that invalidates iterators.
//
// As of 2006-04, there is no standard-blessed way of getting a
// mutable reference to a string's internal buffer. However, issue 530
// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
// proposes this as the method. According to Matt Austern, this should
// already work on all current implementations.
static inline char* StringAsArray(std::string* str) {
    // DO NOT USE const_cast<char*>(str->data())! See the unittest for why.
    return str->empty() ? NULL : &*str->begin();
}

// ------------------------------------------------------------
// arithmetic stuff.
static inline uint32_t ReverseEndian(uint32_t p) {
    return (((p & 0x000000ff) << 24) + ((p & 0x0000ff00) << 8) + ((p & 0x00ff0000) >> 8) + ((
                p & 0xff000000) >> 24));
}

static inline bool IsExp2(uint64_t x) {
    return (x & (x - 1)) ? false : true;
}

static inline uint32_t ReverseUint32(uint32_t x) {
    x = (x & 0x55555555) << 1 | (x & 0xAAAAAAAA) >> 1;
    x = (x & 0x33333333) << 2 | (x & 0xCCCCCCCC) >> 2;
    x = (x & 0x0F0F0F0F) << 4 | (x & 0xF0F0F0F0) >> 4;
    x = (x & 0x00FF00FF) << 8 | (x & 0xFF00FF00) >> 8;
    x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
    return x;
}

// ----------------------------------------
// atomic operation.
// return old value.
static inline int64_t AtomicAdd64(volatile int64_t* count, int64_t add) {
    __asm__ __volatile__(
        "lock xadd %0, (%1);"
        : "=a"(add)
        : "r"(count), "a"(add)
        : "memory");
    return add;
}


#define AtomicGetValue(x) (atomic_comp_swap(&(x), 0, 0))
#define AtomicSetValue(x, v) (atomic_swap(&(x), (v)))
#define AtomicSetValueIf(x, v, ifn) (atomic_comp_swap(&(x), (v), ifn))
#define AtomicDec64(c) (AtomicAdd64(&(c), -1) - 1)
#define AtomicInc64(c) (AtomicAdd64(&(c), 1) + 1)

// ------------------------------------------------------------
// operation stuff.
static inline double GetTimeMS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec * 0.001;
}

static inline double GetTimeUS() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000.0 + tv.tv_usec;
}

static inline int ChmodCB(const char*        fpath,
                          const struct stat* sb,
                          int                typeflag,
                          struct FTW*        ftwbuf) {
    DSTREAM_UNUSED_VAL(sb);
    DSTREAM_UNUSED_VAL(typeflag);
    DSTREAM_UNUSED_VAL(ftwbuf);
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    int rv = chmod(fpath, mode);
    if (rv) {
        perror(fpath);
    }
    return rv;
}

static inline int ChmodRF(const char* path) {
    int err_no = 0;
    if (nftw(path, ChmodCB, 64, FTW_DEPTH | FTW_PHYS) < 0) {
        err_no = errno;
    }
    return err_no;
}

static inline int MoveDir(const char* oldpath, const char* newpath) {
    int err_no = 0;
    if (rename(oldpath, newpath) < 0) {
        err_no = errno;
    }
    return err_no;
}

static inline int MakeDir(const char* path, mode_t mode) {
    int err_no = 0;
    if (mkdir(path, mode) < 0) {
        err_no = errno;
    }
    return err_no;
}

static inline int RemoveDir(const char* path) {
    int err_no = 0;
    if (rmdir(path) < 0) {
        err_no = errno;
    }
    return err_no;
}

static inline int UnlinkCB(const char*          fpath,
                            const struct stat*  sb,
                            int                 typeflag,
                            struct FTW*         ftwbuf) {
    DSTREAM_UNUSED_VAL(sb);
    DSTREAM_UNUSED_VAL(typeflag);
    DSTREAM_UNUSED_VAL(ftwbuf);
    int rv = remove(fpath);
    if (rv) {
        perror(fpath);
    }
    return rv;
}

static inline int RemoveRF(const char* path) {
    int err_no = 0;
    if (nftw(path, UnlinkCB, 64, FTW_DEPTH | FTW_PHYS) < 0) {
        err_no = errno;
    }
    return err_no;
}

static inline bool IsDir(const char* path) {
    struct stat buf;
    if (stat(path, &buf) < 0) {
        return false;
    }
    return S_ISDIR(buf.st_mode) == 1;
}

static inline bool HasExecutedFiles(const char* path) {
    struct stat buf;
    if (stat(path, &buf) < 0) {
        return false;
    }
    if (!S_ISDIR(buf.st_mode)) {
        return false;
    }
    DIR* dirp;
    struct dirent dir_entry;
    struct dirent* pdir_result = NULL;

    if ((dirp = opendir(path)) == NULL) {
        return false;
    }
    while (!readdir_r(dirp, &dir_entry, &pdir_result) && pdir_result != NULL) {
        if (strcmp(dir_entry.d_name, ".") == 0 ||
            strcmp(dir_entry.d_name, "..") == 0 ||
            strcmp(dir_entry.d_name, ".svn") == 0) {
            continue;
        }
        std::string file = path;
        file += "/";
        file += dir_entry.d_name;
        if (access(file.c_str(), X_OK) == 0) {
            closedir(dirp);
            return true;
        }
    }
    closedir(dirp);
    return false;
}

static inline bool IsFile(const char* path) {
    struct stat buf;
    if (stat(path, &buf) < 0) {
        return false;
    }
    return S_ISREG(buf.st_mode) == 1;
}

static inline bool DoesPathExist(const char* path) {
    struct stat buf;
    if (stat(path, &buf) < 0) {
        return false;
    }
    return true;
}

static inline void StringAppendNumber(std::string* s, ssize_t number) {
    char txt[32]; // I think it's enough.
    snprintf(txt, sizeof(txt), "%zd", number);
    s->append(txt);
}

static inline std::string NumberToString(ssize_t number) {
    char txt[32]; // I think it's enough.
    snprintf(txt, sizeof(txt), "%zd", number);
    return txt;
}

// To convert a thing to a certain type.Perhaps not efficient, but convenient
// for example:
// Convert<std::string>(123.456)
// Convert<int>("123")
// note: Do Not Try to Convert Numbers to char * !!! You can use std::string instead.
// author : zhangyuncong @ baidu
template<typename ToType, typename FromType>
static inline ToType Convert(FromType value) {
    std::stringstream bufstream;
    ToType ret_val;
    bufstream << value;
    bufstream >> ret_val;
    return ret_val;
}

// get local host address
error::Code GetLocalhostIP(std::string* s, const char* ifrname_pattern = "eth");
// get remote host address
error::Code GetRemotehostIP(const std::string& remote_host, std::string& remote_ip);

// get processor number and memory number
void GetMachineInfo(int* processor_num, uint64_t* total_memory, uint64_t* free_memory);
// convert config memory
uint64_t GetConfigMemory(std::string* memory_config);

// print version
int32_t PrintPMVersion(const char* opt, const char* value);
void PrintVersion(const char* module_name);

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_UTILS_H__ //NOLINT
