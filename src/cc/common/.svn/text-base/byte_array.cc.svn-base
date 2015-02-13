/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  byte array for read and Write
 *
 */

#include "common/byte_array.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {

// ------------------------------------------------------------
// ReadableByteArray Implementation
ReadableByteArray::ReadableByteArray(const Byte* bytes, ByteSize size):
    m_bytes(bytes), m_size(size), m_cur(0) {
}

bool ReadableByteArray::Read(Byte* bytes, ByteSize size) {
    if ((m_cur + size) > m_size) {
        return false;
    }
    if (bytes) {
        memcpy(bytes, m_bytes + m_cur, size);
    }
    m_cur += size;
    return true;
}

const Byte* ReadableByteArray::Remain(ByteSize* size) const {
    if (size) {
        *size = m_size - m_cur;
    }
    return m_bytes + m_cur;
}

const Byte* ReadableByteArray::Data(ByteSize* size) const {
    if (size) {
        *size = m_size;
    }
    return m_bytes;
}

// ------------------------------------------------------------
// WriteableByteArray Implementation
WriteableByteArray::WriteableByteArray(bool user_free):
    m_user_free(user_free), m_bytes(small_), m_size(kSmallBufferSize), m_cur(0) {
    // if this buffer need user to free
    // we must do real allocation.
    if (m_user_free) {
        m_bytes = NULL;
        m_size = 0;
    }
}

WriteableByteArray::~WriteableByteArray() {
    if (!m_user_free && m_bytes != small_) {
        free(m_bytes);
    }
}

bool WriteableByteArray::Write(const Byte* bytes, ByteSize size) {
    Byte* _bytes = Allocate(size);
    if (!_bytes) {
        return false;
    }
    memcpy(_bytes, bytes, size);
    return true;
}

Byte* WriteableByteArray::Allocate(ByteSize size) {
    if ((m_cur + size) > m_size) {
        if (m_size == 0) {
            m_size = kSmallBufferSize;
        }
        ByteSize next_size = m_size * 2;
        while ((m_cur + size) > next_size) {
            next_size *= 2;
        }
        if (m_bytes == small_) {
            Byte* tmp = static_cast<Byte*>(malloc(next_size));
            if (tmp == NULL) {
                DSTREAM_WARN("malloc(%zu) failed(%s)", next_size, strerror(errno));
                return NULL;
            }
            m_bytes = tmp;
            memcpy(m_bytes, small_, m_cur);
        } else {
            Byte* tmp = static_cast<Byte*>(realloc(m_bytes, next_size));
            if (tmp == NULL) {
                DSTREAM_WARN("realloc(%p,%zu) failed(%s)", m_bytes, next_size, strerror(errno));
                return NULL;
            }
            m_bytes = tmp;
        }
        m_size = next_size;
    }
    Byte* addr = m_bytes + m_cur;
    m_cur += size;
    return addr;
}

const Byte* WriteableByteArray::Data(ByteSize* size) const {
    if (size) {
        *size = m_cur;
    }
    return m_bytes;
}

const ByteSize WriteableByteArray::DataSize() const {
    return m_cur;
}

// ------------------------------------------------------------
// STLStringByteArray Implementation
STLStringByteArray::STLStringByteArray(std::string* s):
    ReadableByteArray(reinterpret_cast<const Byte*>(s->data()),
                      s->size()),
    m_s(s) {
}

bool STLStringByteArray::Read(Byte* bytes, ByteSize size) {
    return ReadableByteArray::Read(bytes, size);
}

bool STLStringByteArray::Write(const Byte* bytes, ByteSize size) {
    m_s->append(reinterpret_cast<const char*>(bytes), size);
    return true;
}

const Byte* STLStringByteArray::Data(ByteSize* size) const {
    return ReadableByteArray::Data(size);
}

const Byte* STLStringByteArray::Remain(ByteSize* size) const {
    return ReadableByteArray::Remain(size);
}

Byte* STLStringByteArray::Allocate(ByteSize size) {
    ByteSize old_size = m_s->size();
    m_s->resize(old_size + size);
    char* addr = StringAsArray(m_s);
    return reinterpret_cast<Byte*>(addr + old_size);
}

// ------------------------------------------------------------
// FileDescriptorByteArray Implementation
FileDescriptorByteArray::FileDescriptorByteArray(int fd):
    ReadableByteArray(0, 0), // dummy value.
    m_fd(fd) {
}

bool FileDescriptorByteArray::Read(Byte* bytes, ByteSize size) {
    ssize_t code = ::read(m_fd, bytes, size);
    if (code < 0) {
        DSTREAM_WARN("::read(%d,%p,%zu) failed(%s)", m_fd, bytes, size, strerror(errno));
        return false;
    } else if (static_cast<ByteSize>(code) != size) {
        DSTREAM_WARN("::read(%d,%p,%zu) return(%zd)", m_fd, bytes, size, code);
        return false;
    }
    return true;
}

bool FileDescriptorByteArray::Write(const Byte* bytes, ByteSize size) {
    ssize_t code = ::write(m_fd, bytes, size);
    if (code < 0) {
        DSTREAM_WARN("::write(%d,%p,%zu) failed(%s)", m_fd, bytes, size, strerror(errno));
        return false;
    } else if (static_cast<ByteSize>(code) != size) {
        DSTREAM_WARN("::write(%d,%p,%zu) return(%zd)", m_fd, bytes, size, code);
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// FixBufByteArray Implementation
FixBufByteArray::FixBufByteArray(Byte* bytes, ByteSize size) :
    ReadableByteArray(bytes, size) {
    WriteableByteArray::m_bytes = bytes;
    WriteableByteArray::m_size  = size;
    WriteableByteArray::m_user_free = true;
}

bool FixBufByteArray::Read(Byte* bytes, ByteSize size) {
    return ReadableByteArray::Read(bytes, size);
}

bool FixBufByteArray::Write(const Byte* bytes, ByteSize size) {
    Byte* _bytes = Allocate(size);
    if (!_bytes) {
        return false;
    }
    memcpy(_bytes, bytes, size);
    return true;
}

const Byte* FixBufByteArray::Data(ByteSize* size) const {
    return ReadableByteArray::Data(size);
}

const Byte* FixBufByteArray::Remain(ByteSize* size) const {
    return ReadableByteArray::Remain(size);
}

Byte* FixBufByteArray::Allocate(ByteSize size) {
    if ((WriteableByteArray::m_cur + size) > WriteableByteArray::m_size) {
        return false;
    }
    Byte* addr = WriteableByteArray::m_bytes + WriteableByteArray::m_cur;
    WriteableByteArray::m_cur += size;
    return addr;
}

} // namespace dstream
