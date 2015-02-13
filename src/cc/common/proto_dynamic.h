/* coding:utf-8
 * Copyright (C) dirlt
 */

#ifndef __DSTREAM_COMMON_PROTO_DYNAMIC_H__ // NOLINT
#define __DSTREAM_COMMON_PROTO_DYNAMIC_H__ // NOLINT

#include <iostream>
#include <string>
#include <vector>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include "common/logger.h"

namespace dstream {
namespace common {

// ------------------------------------------------------------
// simplify usage of dynamic message in protobuf.
class ProtoDynamic :
    public google::protobuf::compiler::MultiFileErrorCollector {
public:
    ProtoDynamic(): m_importer(&m_tree, this) {
        m_tree.MapPath("/", "/"); // absolute path.
        m_tree.MapPath("", "."); // default starts with current directory.
    }
    void addMapPath(const char* vpath, const char* path) {
        m_tree.MapPath(vpath, path);
    }
    virtual ~ProtoDynamic() {}
    struct ErrorMessage {
        std::string filename;
        int line;
        int column;
        std::string message;
    };
    std::vector< ErrorMessage > error_msg;
    virtual void AddError(const std::string& filename,
                          int line, int column,
                          const std::string& message) {
        ErrorMessage msg;
        msg.filename = filename;
        msg.line = line;
        msg.column = column;
        msg.message = message;
        error_msg.push_back(msg);
        DSTREAM_WARN("Dynamic Error : file [%s], line [%d], message [%s]",
                     filename.c_str(), line, message.c_str());
    }

    const google::protobuf::FileDescriptor* import(const std::string& filename) {
        return m_importer.Import(filename);
    }
    static const google::protobuf::Descriptor* findMessageTypeByName(
        const google::protobuf::FileDescriptor* fd,
        const std::string& name) {
        return fd->pool()->FindMessageTypeByName(name);
    }
    static google::protobuf::MessageFactory* newMessageFactory(
        const google::protobuf::FileDescriptor* fd) {
        return new google::protobuf::DynamicMessageFactory(fd->pool());
    }
    static void deleteMessageFactory(google::protobuf::MessageFactory* factory) {
        delete factory;
    }

private:
    google::protobuf::compiler::DiskSourceTree m_tree;
    google::protobuf::compiler::Importer m_importer;
}; // class ProtoDynamic

} // namespace common
} // namespace dstream

#endif // NOLINT
