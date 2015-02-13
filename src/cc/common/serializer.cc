/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  serializer and deserializer of some types
 *
 */

#include "common/logger.h"
#include "common/serializer.h"
#include "common/utils.h"

namespace dstream {
namespace serializer {

// TOOD(zhangyan04):much more efficient and compact.
bool Serialize(const uint64_t uint64_value, WriteableByteArray* bytes) {
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(&uint64_value),
                        sizeof(uint64_value));
}

bool Deserialize(uint64_t* uint64_value, ReadableByteArray* bytes) {
    return bytes->Read(reinterpret_cast<dstream::Byte*>(uint64_value),
                       sizeof(*uint64_value));
}

bool Serialize(const int64_t int64_value, WriteableByteArray* bytes) {
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(&int64_value),
                        sizeof(int64_value));
}

bool Deserialize(int64_t* int64_value, ReadableByteArray* bytes) {
    return bytes->Read(reinterpret_cast<dstream::Byte*>(int64_value),
                       sizeof(*int64_value));
}

bool Serialize(const uint32_t uint32_value, WriteableByteArray* bytes) {
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(&uint32_value),
                        sizeof(uint32_value));
}

bool Deserialize(uint32_t* uint32_value, ReadableByteArray* bytes) {
    return bytes->Read(reinterpret_cast<dstream::Byte*>(uint32_value),
                       sizeof(*uint32_value));
}

bool Serialize(const int32_t int32_value, WriteableByteArray* bytes) {
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(&int32_value),
                        sizeof(int32_value));
}

bool Deserialize(int32_t* int32_value, ReadableByteArray* bytes) {
    return bytes->Read(reinterpret_cast<dstream::Byte*>(int32_value),
                       sizeof(*int32_value));
}

bool Serialize(const char char_value, WriteableByteArray* bytes) {
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(&char_value),
                        sizeof(char_value));
}

bool Deserialize(char* char_ptr_value, ReadableByteArray* bytes) {
    return bytes->Read(reinterpret_cast<dstream::Byte*>(char_ptr_value),
                       sizeof(*char_ptr_value));
}

bool Serialize(const std::string& string_value, WriteableByteArray* bytes) {
    if (!Serialize(string_value.size(), bytes)) {
        return false;
    }
    return bytes->Write(reinterpret_cast<const dstream::Byte*>(string_value.data()),
                        string_value.size());
}

bool Deserialize(std::string* string_value, ReadableByteArray* bytes) {
    size_t size;
    if (!Deserialize(&size, bytes)) {
        return false;
    }
    // DEBUG("std::string size:%zu", size);
    string_value->resize(size);
    return bytes->Read(reinterpret_cast<dstream::Byte*>(StringAsArray(string_value)), size);
}

} // namespace serializer
} // namespace dstream
