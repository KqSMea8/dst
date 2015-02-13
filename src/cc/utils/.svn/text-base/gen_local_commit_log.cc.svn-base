/** 
* @file     gen_local_commit_log.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.0
* @date     2013-07-25
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream> //NOLINT
#include <string>

#include "common/byte_array.h"
#include "common/commit_log.h"
#include "common/utils.h"


int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Usage gen_local_commit_log filename size\n");
        exit(1);
    }

    std::string file_name(argv[1]);
    int size = atoi(argv[2]);

    dstream::CommitLog* p = (dstream::CommitLog*)new
            dstream::LocalFileCommitLog(dstream::CommitLog::kPublisher, file_name);

    uint64_t log_id;
    bool duplicated;
    if (p->Open() < 0) {
        printf("localfile commit log open error\n");
    }

    for (int i = 0; i < size; ++i) {
        std::string str;
        dstream::StringAppendNumber(&str, i);
        str = str + ":##################################";
        dstream::ReadableByteArray
            bytes(reinterpret_cast<const dstream::Byte*>(str.data()), str.length());
        dstream::ByteSize gen_size;
        printf("gen data bytes('%s') ", bytes.Data(&gen_size));
        printf("data size(%zu)\n", gen_size);

        p->Write(&bytes, &duplicated, &log_id);
    }

    p->Close();

    delete p;
    p = NULL;

    // check file
    p = new dstream::LocalFileCommitLog(dstream::LocalFileCommitLog::kSubscriber, file_name);

    if (p->Open() < 0) {
        printf("localfile commit log open error\n");
    }

    dstream::LogInfo log_info;
    bool eof = false;
    while (!eof) {
        dstream::WriteableByteArray in_bytes(true); // NOTICE:  need user free !!!
        if (p->Read(&log_info) != 0) {
            printf("commit_log read failed\n");
            return -1;
        }

        dstream::ByteSize ck_size = 0;
        const dstream::Byte* data = log_info.byte_array.Data(&ck_size);
        printf("check data bytes(%s) ", data);
        printf("data size(%zu)\n", ck_size);

        // do some clearup
        free(const_cast<dstream::Byte*>(data));
    }

    p->Close();

    delete p;
    p = NULL;

    return 0;
}


