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

#ifndef __DSTREAM_CC_COMMON_SERIALIZER_H__ // NOLINT
#define __DSTREAM_CC_COMMON_SERIALIZER_H__ // NOLINT

#include <string>
#include "common/byte_array.h"

namespace dstream {
namespace serializer {

// ------------------------------------------------------------
class Serializable {
public:
    virtual ~Serializable() {}
    virtual bool Serialize(WriteableByteArray* bytes) const = 0;
    virtual bool Deserialize(ReadableByteArray* bytes) = 0;
}; // class Serializable

// ------------------------------------------------------------
template<typename T>
class Stream {
public:
    bool out(const T& x, WriteableByteArray* bytes) const {
        return x.Serialize(bytes);
    }
    bool in(T* x, ReadableByteArray* bytes) {
        return x->Deserialize(bytes);
    }
}; // class Stream

bool Serialize(const uint64_t uint64_value, WriteableByteArray* bytes);
bool Deserialize(uint64_t* uint64_value, ReadableByteArray* bytes);
template<>
class Stream<uint64_t> {
public:
    bool out(const uint64_t& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(uint64_t* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

bool Serialize(const int64_t int64_value, WriteableByteArray* bytes);
bool Deserialize(int64_t* int64_value, ReadableByteArray* bytes);
template<>
class Stream<int64_t> {
public:
    bool out(const int64_t& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(int64_t* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

bool Serialize(const uint32_t uint32_value, WriteableByteArray* bytes);
bool Deserialize(uint32_t* uint32_value, ReadableByteArray* bytes);
template<>
class Stream<uint32_t> {
public:
    bool out(const uint32_t& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(uint32_t* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

bool Serialize(const int32_t int32_value, WriteableByteArray* bytes);
bool Deserialize(int32_t* int32_value, ReadableByteArray* bytes);
template<>
class Stream<int32_t> {
public:
    bool out(const int32_t& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(int32_t* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

bool Serialize(const char char_value, WriteableByteArray* bytes);
bool Deserialize(char* char_ptr_value, ReadableByteArray* bytes);
template<>
class Stream<char> {
public:
    bool out(const char& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(char* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

bool Serialize(const std::string& string_value, WriteableByteArray* bytes);
bool Deserialize(std::string* string_value, ReadableByteArray* bytes);
template<>
class Stream<std::string> {
public:
    bool out(const std::string& x, WriteableByteArray* bytes) const {
        return Serialize(x, bytes);
    }
    bool in(std::string* x, ReadableByteArray* bytes) const {
        return Deserialize(x, bytes);
    }
};

#undef DSTREAM_CC_COMMON_SERIALIZER_ENUM_TYPE

} // namespace serializer
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_SERIALIZER_H__ NOLINT
