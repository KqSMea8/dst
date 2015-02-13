/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  byte array for read and write
 *
 */

#ifndef __DSTREAM_CC_COMMON_BYTE_ARRAY_H__
#define __DSTREAM_CC_COMMON_BYTE_ARRAY_H__

#include <stdint.h>
#include <sys/types.h>
#include <string>
#include "common/utils.h"

namespace dstream {

typedef uint8_t Byte;
typedef size_t ByteSize;

// ------------------------------------------------------------
class ReadableByteArray {
public:
    ReadableByteArray(const Byte* bytes, ByteSize size);
    virtual ~ReadableByteArray() {}
    // abstract interface.
    virtual bool Read(Byte* bytes, ByteSize size);
    // interface only works with internal implementation is memory buffer.
    virtual const Byte* Data(ByteSize* size) const;
    virtual const Byte* Remain(ByteSize* size) const;
    //-----------------------------------------------------------------
    // !!!!! Abandoned interfaces
    virtual bool read(Byte* bytes, ByteSize size) {
        return Read(bytes, size);
    }
    // interface only works with internal implementation is memory buffer.
    virtual const Byte* data(ByteSize* size) const {
        return Data(size);
    }
    virtual const Byte* remain(ByteSize* size) const {
        return Remain(size);
    }

protected:
    const Byte* m_bytes;
    ByteSize m_size;
    ByteSize m_cur;
}; // class ReadableByteArray

// ------------------------------------------------------------
class WriteableByteArray {
public:
    // if 'user_free'==true, user has to free 'data' manually
    // in some situation, it's much efficient.
    // and to now, this feature can be applied to 'rpc'.
    WriteableByteArray(bool user_free = false);
    virtual ~WriteableByteArray();
    // abstract interface.
    virtual bool Write(const Byte* bytes, ByteSize size);
    virtual const Byte* Data(ByteSize* size) const;
    virtual const ByteSize DataSize() const;
    virtual Byte* Allocate(ByteSize size);
protected:
    bool     m_user_free;
    Byte*    m_bytes;
    ByteSize m_size;
    ByteSize m_cur;
    static const int kSmallBufferSize = 128;
    Byte small_[kSmallBufferSize];
}; // class WriteableByteArray

// ------------------------------------------------------------
// wrapper std::string, so user can serialize to std::string directly.
// but use as WriteableByteArray or ReadableByteArray on one instance.
class STLStringByteArray:
    public ReadableByteArray,
    public WriteableByteArray {
public:
    STLStringByteArray(std::string* s);
    virtual ~STLStringByteArray() {}
    virtual bool Read(Byte* bytes, ByteSize size);
    virtual bool Write(const Byte* bytes, ByteSize size);
    virtual const Byte* Data(ByteSize* size) const;
    virtual const Byte* Remain(ByteSize* size) const;
    virtual Byte* Allocate(ByteSize size);
private:
    std::string* m_s;
}; // class STLStringByteArray

// ------------------------------------------------------------
// wrapper system fd, so user can serialize to fd directly.
// but use ad WriteableByteArray or ReadableByteArray on one instance.
class FileDescriptorByteArray:
    public ReadableByteArray,
    public WriteableByteArray {
public:
    FileDescriptorByteArray(int fd);
    virtual ~FileDescriptorByteArray() {}
    virtual bool Read(Byte* bytes, ByteSize size);
    virtual bool Write(const Byte* bytes, ByteSize size);
    // never called...
    virtual const Byte* Data(ByteSize* /*size*/) const {
        return NULL;
    }
    virtual const Byte* Remain(ByteSize* /*size*/) const {
        return NULL;
    }
    virtual Byte* Allocate(ByteSize /*size*/) {
        return NULL;
    }
private:
    int m_fd;
};

// ------------------------------------------------------------
// wrapper fixed buf array, so user can serialize to appointed buf.
// but use ad WriteableByteArray or ReadableByteArray on one instance.
class FixBufByteArray:
    public ReadableByteArray,
    public WriteableByteArray {
public:
    FixBufByteArray(Byte* bytes, ByteSize size);
    ~FixBufByteArray() {}

    bool Read(Byte* bytes, ByteSize size);
    bool Write(const Byte* bytes, ByteSize size);
    DSTREAM_FORCE_INLINE const Byte* Data(ByteSize* size) const;
    DSTREAM_FORCE_INLINE const Byte* Remain(ByteSize* size) const;
    Byte* Allocate(ByteSize size);
};

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_BYTE_ARRAY_H__
