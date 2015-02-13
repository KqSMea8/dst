/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#ifndef __DSTREAM_CC_PROCESSELEMENT_TUPLE_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_TUPLE_H__ // NOLINT

#include <stdint.h>
#include <string>
#include "common/dynamic_type.h"
#include "common/proto_serializer.h"
#include "common/serializer.h"

namespace dstream {
namespace processelement {

// ------------------------------------------------------------
// Tuple Interface
class Tuple: public serializer::Serializable {
public:
    Tuple();
    virtual ~Tuple() {}
    uint64_t hash_code() const {
        return m_hash_code;
    }
    void set_hash_code(uint64_t cur_hash_code) {
        m_hash_code = cur_hash_code;
    }
    const std::string& tag() const {
        return m_tag;
    }
    void set_tag(const std::string& cur_tag) {
        m_tag = cur_tag;
    }
    bool dynamic_type() const {
        return m_dynamic_type;
    }
    void set_dynamic_type(bool cur_dynamic_type) {
        m_dynamic_type = cur_dynamic_type;
    };
    static void increase_instance_counter() {
        ++m_instance_counter;
    }
    static uint64_t instance_counter() {
        return m_instance_counter;
    }

protected:
    bool            m_dynamic_type;

private:
    static uint64_t m_instance_counter;
    uint64_t        m_hash_code;
    std::string     m_tag;
}; // class Tuple

typedef std::vector< const Tuple* > Tuples;

// ------------------------------------------------------------
// wrapper protobuf object into Tuple, T should be a google::protobuf object.
template< typename T >
class ProtoTuple: public T, public Tuple {
public:
    ProtoTuple() {}
    virtual ~ProtoTuple() {}
    virtual bool Serialize(WriteableByteArray* bytes) const;
    virtual bool Deserialize(ReadableByteArray* bytes);
}; // class ProtoTuple

template< typename T >
bool ProtoTuple<T>::Serialize(WriteableByteArray* bytes) const {
    return proto_serializer::SerializeToWriteBuffer(this, bytes);
}

template< typename T >
bool ProtoTuple<T>::Deserialize(ReadableByteArray* bytes) {
    return proto_serializer::DeserizlizeFromReadArray(this, bytes);
}

// wrapper of dynamic protobuffer object
class DynamicProtoTuple : public Tuple {
public:
    explicit DynamicProtoTuple(google::protobuf::Message* msg) : m_msg(msg) {
        m_dynamic_type = true;
    }
    ~DynamicProtoTuple() {
        delete m_msg;
    }
    virtual bool Serialize(WriteableByteArray* bytes) const {
        if (NULL != m_msg) {
            return proto_serializer::SerializeToWriteBuffer(m_msg, bytes);
        }
        return false;
    }
    virtual bool Deserialize(ReadableByteArray* bytes) {
        if (NULL != m_msg) {
            return proto_serializer::DeserizlizeFromReadArray(m_msg, bytes);
        }
        return false;
    }

    const google::protobuf::Message* ProtoMessage() const {
        return m_msg;
    }

private:
    google::protobuf::Message* m_msg;
};

// ------------------------------------------------------------
// TupleTypeInfo Interface.
class TupleTypeInfo {
public:
    TupleTypeInfo() {}
    virtual ~TupleTypeInfo() {}
    // ------------------------------
    // user has to provide how to allocate tuple instance.
    virtual Tuple* allocate() = 0;
    virtual void deallocate(Tuple* tuple) {
        delete tuple;
    }
}; // class TupleTypeInfo

// ------------------------------------------------------------
template< typename T>
class DefaultTupleTypeInfo : public TupleTypeInfo {
public:
    virtual Tuple* allocate() {
        return new T();
    }
};

class DynamicTupleTypeInfo : public TupleTypeInfo {
public:
    virtual bool Init(const std::string& file, const std::string& type_name) = 0;
};

class DynamicProtoType : public DynamicTupleTypeInfo {
public:
    DynamicProtoType() : m_proto_type(0) {}
    bool Init(const std::string& file, const std::string& type_name) {
        m_proto_type = reflect.GetProtoType(file, type_name);
        return m_proto_type != NULL;
    }
    void SetProtoPath(const std::string& path) {
        return reflect.SetProtoPath(path);
    }
    Tuple* allocate() {
        if (0 != m_proto_type) {
            return new DynamicProtoTuple(m_proto_type->New());
        }
        return 0;
    }
private:
    static dynamic_type::DynamicProtoReflect reflect;
    const google::protobuf::Message* m_proto_type;
};

} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_TUPLE_H__ NOLINT
