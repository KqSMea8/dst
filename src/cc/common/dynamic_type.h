/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_COMMON_DYNAMIC_TYPE_H__
#define __DSTREAM_CC_COMMON_DYNAMIC_TYPE_H__

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>

#include <map>
#include <string>

#include "common/proto_dynamic.h"

typedef google::protobuf::Message ProtoMessage;
namespace dstream {
namespace dynamic_type {

typedef const google::protobuf::FileDescriptor ProtoFiledes;
// Get proto buffer type
class DynamicProtoReflect {
public:
    DynamicProtoReflect() {
        m_factory = new google::protobuf::DynamicMessageFactory;
    }
    ~DynamicProtoReflect() {
        delete m_factory;
    }
    // file : the filename of load protobuffer type
    // type_name : the type name to be loaded
    const ProtoMessage* GetProtoType(const std::string& file, const std::string& type_name);
    void SetProtoPath(const std::string& path);

private:
    std::map<std::string, ProtoFiledes*>        m_file_des_map;
    google::protobuf::DynamicMessageFactory*    m_factory;
    common::ProtoDynamic                        m_proto_dynamic;
};

// Get filename and type name from tag
bool GetDynamicTypeInfo(const std::string& tag, std::string* file, std::string* type);

} // namespace dynamic_type
} // namespace dstream
#endif // __DSTREAM_CC_COMMON_DYNAMIC_TYPE_H__
