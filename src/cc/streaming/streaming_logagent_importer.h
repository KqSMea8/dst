/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file streaming_logagent_importer.h
 * @author zhenpeng(zhenpeng01@baidu.com)
 * @date 2013/11/14 15:50:30
 * @version $Revision$ 
 * @brief 
 *  
 **/
#ifndef DSTREAM_STREAMING_STREAMING_LOGAGENT_IMPORTER_H
#define DSTREAM_STREAMING_STREAMING_LOGAGENT_IMPORTER_H

#include "streaming/streaming.h"

namespace dstream {
namespace streaming {

class LogAgentStreamingTask: public StreamingTask {
public:
    LogAgentStreamingTask(string end_of_tag,
            string end_of_key, Hash* hash, int bufsize, bool with_tag, bool with_logging);
    bool Base64Encode(const char* src, size_t srclen, std::string* dst);
    bool Base64Decode(std::string src, char* dst, size_t *dstlen);
    virtual void OnSpout(dstream::ReadableByteArray* bytes);

private:
    void ClearBuffer() {
        m_message[0] = '\0';
        m_logagent_tag[0] = '\0';
    }

    char    m_message[3 << 20]; // 3M
    size_t  m_message_len;
    char    m_logagent_tag[128]; // from bigpipe definition
    int64_t m_key;
}; // class LogAgentStreamingTask

} // namespace streaming
} // namespace dstream

#endif // DSTREAM_STREAMING_STREAMING_LOGAGENT_IMPORTER_H
/* vim: set ts=4 sw=4 sts=4 tw=100 */
