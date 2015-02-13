/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include "common/dynamic_type.h"
#include "common/logger.h"

namespace dstream {
namespace dynamic_type {

// file : the filename of load protobuffer type
// type_name : the type name to be loaded
const ProtoMessage* DynamicProtoReflect::GetProtoType(const std::string& file,
        const std::string& type_name) {
    std::map<std::string, ProtoFiledes*>::const_iterator find_fd;
    std::string file_name = file + ".proto";
    find_fd = m_file_des_map.find(file_name);
    if (find_fd == m_file_des_map.end()) {
        const ProtoFiledes* fd = m_proto_dynamic.import(file_name);
        if (NULL == fd) {
            DSTREAM_WARN("proto buffer import file [%s] fail", file.c_str());
            return NULL;
        }
        m_file_des_map[file_name] = fd;
        find_fd = m_file_des_map.find(file_name);
    }
    const google::protobuf::FileDescriptor* fd = find_fd->second;
    const google::protobuf::Descriptor* descriptor = common::ProtoDynamic::findMessageTypeByName(fd,
                                                     type_name);
    if (descriptor != NULL) {
        return m_factory->GetPrototype(descriptor);
    }
    return NULL;
}

void DynamicProtoReflect::SetProtoPath(const std::string& path) {
    return m_proto_dynamic.addMapPath("", path.c_str());
}

// Get filename and type name from tag
bool GetDynamicTypeInfo(const std::string& tag, std::string* file, std::string* type) {
    size_t file_name_pos = tag.find('.');
    if (file_name_pos == std::string::npos || file_name_pos == tag.length() - 1) {
        return false;
    }
    *file = tag.substr(0, file_name_pos);
    *type = tag.substr(file_name_pos + 1, tag.length() - file_name_pos);
    return true;
}

} // namespace dynamic_type
} // namespace dstream
