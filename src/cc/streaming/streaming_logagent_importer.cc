/** 
* @file     streaming_logagent_importer.cc
* @brief    
* @author   liuguodong,liuguodong01@baidu.com
* @version  1.1
* @date     2013-07-26
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include <compack/compack.h>
#include <sstream>
#include <string>
#include "streaming/streaming_logagent_importer.h"

using namespace dstream::streaming;

#define PACKET_TYPE_NORMAL  0

#define BODY_TYPE_NORMAL    0
#define BODY_TYPE_LOGGING   1
#define BODY_TYPE_B2LOG     2

LogAgentStreamingTask::LogAgentStreamingTask(string end_of_tag,
        string end_of_key, Hash* hash, int bufsize, bool with_tag, bool with_logging):
        StreamingTask(end_of_tag, end_of_key, hash, bufsize, with_tag, with_logging) {
    m_key = 0;
    m_message_len = 0;
    ClearBuffer();
}

bool LogAgentStreamingTask::Base64Encode(const char* src, size_t srclen, std::string* dst) {
    assert(dst != NULL);
    using namespace boost::archive::iterators;
    typedef base64_from_binary<transform_width<const char*, 6, 8 > > Base64Text;
    dst->assign(Base64Text(src), Base64Text(src+srclen));
    size_t append_size = (3 - srclen % 3) % 3; // append paddings
    dst->append(append_size, '=');
    return true;
}

bool LogAgentStreamingTask::Base64Decode(std::string src, char* dst, size_t *dstlen) {
    using namespace boost::archive::iterators;
    typedef transform_width<
        binary_from_base64<std::string::const_iterator>, 8, 6 > BinaryText;
    size_t padding_size = std::count(src.begin(), src.end(), '=');

    std::replace(src.begin(), src.end(), '=', 'A');
    std::string dst_str(BinaryText(src.begin()), BinaryText(src.end()));
    dst_str.erase(dst_str.end() - padding_size, dst_str.end());

    assert(dst_str.size() <= *dstlen);
    *dstlen = dst_str.size();
    memcpy(dst, dst_str.data(), *dstlen);
    return true;
}

void LogAgentStreamingTask::OnSpout(dstream::ReadableByteArray* bytes) {
    DSTREAM_DEBUG("  ====> OnSpout");
    ClearBuffer();
    // now we can automaticly determine if it is loagent data, unpack if true
    try {
        dstream::ByteSize size = 0;
        const char* packet_body = reinterpret_cast<const char*>(bytes->Data(&size));
        compack::Buffer wrap(const_cast<char*>(packet_body), size);
        compack::buffer::Reader reader(wrap);
        compack::ObjectIterator element;

        bool is_found = reader.find("type", element);
        if (!is_found || (PACKET_TYPE_NORMAL != element.getAsNum<int>())) {
            DSTREAM_WARN("unknown packet format, skip.");
            return;
        }

        // read body type
        is_found = reader.find("body_type", element);
        if (!is_found) {
            DSTREAM_WARN("unknown body type, skip.");
            return;
        }

        // read body
        int body_type = element.getAsNum<int>();
        if (BODY_TYPE_NORMAL == body_type) {
            if (is_found = reader.find("body", element)) {
                packet_body = reinterpret_cast<const char*>(element.getAsString());
                m_message_len = element.length();
                memcpy(m_message, packet_body, m_message_len+1); // with '\0'
                DSTREAM_DEBUG("read message body: %s", m_message);
            } else {
                DSTREAM_WARN("message body not found");
                return;
            }

        } else if (BODY_TYPE_LOGGING == body_type) {
            if (!m_with_logging) {
                DSTREAM_WARN("get logging format message, skip.");
                return;
            }

            if (is_found = reader.find("bin_body", element)) {
                packet_body = reinterpret_cast<const char*>(element.getAsBinary());
                std::string tmpstr;
                Base64Encode(packet_body, element.length(), &tmpstr);
                m_message_len = sizeof(m_message);
                assert(tmpstr.size() <= m_message_len);
                m_message_len = tmpstr.size();
                memcpy(m_message, tmpstr.data(), m_message_len+1);
                DSTREAM_DEBUG("base64 encode message bin_body size: %zd", m_message_len);
            } else {
                DSTREAM_WARN("message bin_body not found");
                return;
            }
        } else {
            DSTREAM_WARN("unknown message body type: %d", body_type);
            return;
        }

        // read tag
        if (is_found == reader.find("tag", element)) {
            const char* tag = (reinterpret_cast<const char*>(element.getAsString()));
            memcpy(m_logagent_tag, tag, strlen(tag)+1); // with '\0'
            DSTREAM_DEBUG("read logagent tag: %s", m_logagent_tag);
        } else {
            DSTREAM_WARN("logagent tag not found");
        }
    } catch (const bsl::Exception& ex) {
        DSTREAM_ERROR("src_adaptor/unpack: unknown bsl exception.[exception:%s]", ex.what());
        return;
    }

    DSTREAM_DEBUG("spout start: key:%lu,value:%s", m_key, m_message);
    if (m_with_tag) {
        // Note: Here we use logagent tag
        snprintf(m_in_buf, g_pipe_buffer_size, "%s%s%lu%s%s%s",
                (m_logagent_tag[0] == '\0') ? kTag : m_logagent_tag, m_tag_stop.c_str(),
                m_key++, m_key_stop.c_str(),
                m_message, m_content_stop.c_str());
    } else {
        snprintf(m_in_buf, g_pipe_buffer_size, "%lu%s%s%s",
                m_key++, m_key_stop.c_str(),
                m_message, m_content_stop.c_str());
    }

    DSTREAM_DEBUG("flush to user script: %s", m_in_buf);
    if (fprintf(m_child_stdin, "%s", m_in_buf) < 0) {
        DSTREAM_WARN("OnSpout Error: %s", strerror(errno));
    }
    fflush(m_child_stdin);

    if (m_stop) {
        Quit();
    }
}
