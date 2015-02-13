/** 
* @file     streaming_app.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.1
* @date     2013-07-26
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include "streaming/streaming.h"

using namespace dstream::streaming;
int main(int argc, char** argv) {
    dstream::logger::initialize("streaming");
    ParseArgs(argc, argv);
    setenv(Task::kEnvKeyPEType, "PE_TYPE_STREAMING", 1);
    // make a task instance and run.
    StreamingTask task(g_end_of_tag, g_end_of_key,
                       g_hash_to_use, g_pipe_buffer_size, g_with_tag, g_with_logging);
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.run(loopback);
    return 0;
}
