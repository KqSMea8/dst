/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
 * @author zhanggengxin@baidu.com
 * @brief protobuf object serializer and deserializer.
 */

#ifndef __DSTREAM_CC_COMMON_PROTO_SERIAZLIZER_H__ // NOLINT
#define __DSTREAM_CC_COMMON_PROTO_SERIAZLIZER_H__ // NOLINT

#include <google/protobuf/io/coded_stream.h>

#include "common/logger.h"
#include "common/serializer.h"

namespace dstream {
namespace proto_serializer {

// ------------------------------------------------------------
// wrapper protobuf serialize and deserialize method.
// !!!only works with memory buffer.
template <class T>
bool SerializeToWriteBuffer(const T* obj, WriteableByteArray* bytes) {
    if (!bytes) {
        DSTREAM_WARN("bytes==0");
        return false;
    }
    size_t size = obj->ByteSize();
    if (!serializer::Serialize(size, bytes)) {
        DSTREAM_WARN("serialize(%zu, %p) failed", size, bytes);
        return false;
    }
    Byte* serial_buf = bytes->Allocate(size);
    if (!serial_buf) {
        DSTREAM_WARN("allocate(%zu) failed", size);
        return false;
    }
    if (!obj->SerializeToArray(serial_buf, size)) {
        DSTREAM_WARN("SerializeToArray(%p,%zu) failed", serial_buf, size);
        return false;
    }
    return true;
}

template <class T>
bool DeserizlizeFromReadArray(T* obj, ReadableByteArray* bytes) {
    if (!bytes) {
        DSTREAM_WARN("bytes==0");
        return false;
    }
    size_t size;
    if (!serializer::Deserialize(&size, bytes)) {
        DSTREAM_WARN("deserialize(%zu, %p) failed", size, bytes);
        return false;
    }
    const Byte* data = bytes->Remain(NULL);
    google::protobuf::io::CodedInputStream input(data, size);
    if (!obj->ParseFromCodedStream(&input)) {
        DSTREAM_WARN("ParseFromCodedStream(%p,%zu) failed", data, size);
        return false;
    }
    bytes->Read(NULL, size);
    return true;
}

} // namespace proto_serializer
} // namespace dstream

#endif // NOLINT
