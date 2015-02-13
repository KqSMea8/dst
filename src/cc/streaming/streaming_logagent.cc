/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
/**
 * @file streaming/streaming_logagent.cc
 * @author zhenpeng(zhenpeng01@baidu.com)
 * @date 2013/11/14 16:30:06
 * @version $Revision$ 
 * @brief 
 *  
 **/
#include "streaming/streaming_logagent_importer.h"

using namespace dstream::streaming;

int main(int argc, char** argv) {
    dstream::logger::initialize("streaming-logagent");
    ParseArgs(argc, argv);
    setenv(Task::kEnvKeyPEType, "PE_TYPE_STREAMING" , 1);
    // make a task instance and run.
    LogAgentStreamingTask task(g_end_of_tag, g_end_of_key, g_hash_to_use,
                               g_pipe_buffer_size, g_with_tag, g_with_logging);
    // get env of loopback & importer
    bool loopback = false;
    if (getenv("loopback") != 0) {
        loopback = true;
    }
    task.run(loopback);
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
