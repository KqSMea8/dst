// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: dynamic_tag.proto

#ifndef PROTOBUF_dynamic_5ftag_2eproto__INCLUDED
#define PROTOBUF_dynamic_5ftag_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_dynamic_5ftag_2eproto();
void protobuf_AssignDesc_dynamic_5ftag_2eproto();
void protobuf_ShutdownFile_dynamic_5ftag_2eproto();

class DynamicTag1;
class DynamicTag2;
class TagDynamicTest;

// ===================================================================

class DynamicTag1 : public ::google::protobuf::Message {
public:
    DynamicTag1();
    virtual ~DynamicTag1();

    DynamicTag1(const DynamicTag1& from);

    inline DynamicTag1& operator=(const DynamicTag1& from) {
        CopyFrom(from);
        return *this;
    }

    inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
        return _unknown_fields_;
    }

    inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
        return &_unknown_fields_;
    }

    static const ::google::protobuf::Descriptor* descriptor();
    static const DynamicTag1& default_instance();

    void Swap(DynamicTag1* other);

    // implements Message ----------------------------------------------

    DynamicTag1* New() const;
    void CopyFrom(const ::google::protobuf::Message& from);
    void MergeFrom(const ::google::protobuf::Message& from);
    void CopyFrom(const DynamicTag1& from);
    void MergeFrom(const DynamicTag1& from);
    void Clear();
    bool IsInitialized() const;

    int ByteSize() const;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream* input);
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream* output) const;
    ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
    int GetCachedSize() const {
        return _cached_size_;
    }
private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const;
public:

    ::google::protobuf::Metadata GetMetadata() const;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // optional string name = 1;
    inline bool has_name() const;
    inline void clear_name();
    static const int kNameFieldNumber = 1;
    inline const ::std::string& name() const;
    inline void set_name(const ::std::string& value);
    inline void set_name(const char* value);
    inline void set_name(const char* value, size_t size);
    inline ::std::string* mutable_name();
    inline ::std::string* release_name();

    // @@protoc_insertion_point(class_scope:DynamicTag1)
private:
    inline void set_has_name();
    inline void clear_has_name();

    ::google::protobuf::UnknownFieldSet _unknown_fields_;

    ::std::string* name_;

    mutable int _cached_size_;
    ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

    friend void  protobuf_AddDesc_dynamic_5ftag_2eproto();
    friend void protobuf_AssignDesc_dynamic_5ftag_2eproto();
    friend void protobuf_ShutdownFile_dynamic_5ftag_2eproto();

    void InitAsDefaultInstance();
    static DynamicTag1* default_instance_;
};
// -------------------------------------------------------------------

class DynamicTag2 : public ::google::protobuf::Message {
public:
    DynamicTag2();
    virtual ~DynamicTag2();

    DynamicTag2(const DynamicTag2& from);

    inline DynamicTag2& operator=(const DynamicTag2& from) {
        CopyFrom(from);
        return *this;
    }

    inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
        return _unknown_fields_;
    }

    inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
        return &_unknown_fields_;
    }

    static const ::google::protobuf::Descriptor* descriptor();
    static const DynamicTag2& default_instance();

    void Swap(DynamicTag2* other);

    // implements Message ----------------------------------------------

    DynamicTag2* New() const;
    void CopyFrom(const ::google::protobuf::Message& from);
    void MergeFrom(const ::google::protobuf::Message& from);
    void CopyFrom(const DynamicTag2& from);
    void MergeFrom(const DynamicTag2& from);
    void Clear();
    bool IsInitialized() const;

    int ByteSize() const;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream* input);
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream* output) const;
    ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
    int GetCachedSize() const {
        return _cached_size_;
    }
private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const;
public:

    ::google::protobuf::Metadata GetMetadata() const;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // optional string name = 1;
    inline bool has_name() const;
    inline void clear_name();
    static const int kNameFieldNumber = 1;
    inline const ::std::string& name() const;
    inline void set_name(const ::std::string& value);
    inline void set_name(const char* value);
    inline void set_name(const char* value, size_t size);
    inline ::std::string* mutable_name();
    inline ::std::string* release_name();

    // @@protoc_insertion_point(class_scope:DynamicTag2)
private:
    inline void set_has_name();
    inline void clear_has_name();

    ::google::protobuf::UnknownFieldSet _unknown_fields_;

    ::std::string* name_;

    mutable int _cached_size_;
    ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

    friend void  protobuf_AddDesc_dynamic_5ftag_2eproto();
    friend void protobuf_AssignDesc_dynamic_5ftag_2eproto();
    friend void protobuf_ShutdownFile_dynamic_5ftag_2eproto();

    void InitAsDefaultInstance();
    static DynamicTag2* default_instance_;
};
// -------------------------------------------------------------------

class TagDynamicTest : public ::google::protobuf::Message {
public:
    TagDynamicTest();
    virtual ~TagDynamicTest();

    TagDynamicTest(const TagDynamicTest& from);

    inline TagDynamicTest& operator=(const TagDynamicTest& from) {
        CopyFrom(from);
        return *this;
    }

    inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
        return _unknown_fields_;
    }

    inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
        return &_unknown_fields_;
    }

    static const ::google::protobuf::Descriptor* descriptor();
    static const TagDynamicTest& default_instance();

    void Swap(TagDynamicTest* other);

    // implements Message ----------------------------------------------

    TagDynamicTest* New() const;
    void CopyFrom(const ::google::protobuf::Message& from);
    void MergeFrom(const ::google::protobuf::Message& from);
    void CopyFrom(const TagDynamicTest& from);
    void MergeFrom(const TagDynamicTest& from);
    void Clear();
    bool IsInitialized() const;

    int ByteSize() const;
    bool MergePartialFromCodedStream(
        ::google::protobuf::io::CodedInputStream* input);
    void SerializeWithCachedSizes(
        ::google::protobuf::io::CodedOutputStream* output) const;
    ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
    int GetCachedSize() const {
        return _cached_size_;
    }
private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const;
public:

    ::google::protobuf::Metadata GetMetadata() const;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // optional string name = 1;
    inline bool has_name() const;
    inline void clear_name();
    static const int kNameFieldNumber = 1;
    inline const ::std::string& name() const;
    inline void set_name(const ::std::string& value);
    inline void set_name(const char* value);
    inline void set_name(const char* value, size_t size);
    inline ::std::string* mutable_name();
    inline ::std::string* release_name();

    // @@protoc_insertion_point(class_scope:TagDynamicTest)
private:
    inline void set_has_name();
    inline void clear_has_name();

    ::google::protobuf::UnknownFieldSet _unknown_fields_;

    ::std::string* name_;

    mutable int _cached_size_;
    ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

    friend void  protobuf_AddDesc_dynamic_5ftag_2eproto();
    friend void protobuf_AssignDesc_dynamic_5ftag_2eproto();
    friend void protobuf_ShutdownFile_dynamic_5ftag_2eproto();

    void InitAsDefaultInstance();
    static TagDynamicTest* default_instance_;
};
// ===================================================================


// ===================================================================

// DynamicTag1

// optional string name = 1;
inline bool DynamicTag1::has_name() const {
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void DynamicTag1::set_has_name() {
    _has_bits_[0] |= 0x00000001u;
}
inline void DynamicTag1::clear_has_name() {
    _has_bits_[0] &= ~0x00000001u;
}
inline void DynamicTag1::clear_name() {
    if (name_ != &::google::protobuf::internal::kEmptyString) {
        name_->clear();
    }
    clear_has_name();
}
inline const ::std::string& DynamicTag1::name() const {
    return *name_;
}
inline void DynamicTag1::set_name(const ::std::string& value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void DynamicTag1::set_name(const char* value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void DynamicTag1::set_name(const char* value, size_t size) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* DynamicTag1::mutable_name() {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    return name_;
}
inline ::std::string* DynamicTag1::release_name() {
    clear_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        return NULL;
    } else {
        ::std::string* temp = name_;
        name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
        return temp;
    }
}

// -------------------------------------------------------------------

// DynamicTag2

// optional string name = 1;
inline bool DynamicTag2::has_name() const {
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void DynamicTag2::set_has_name() {
    _has_bits_[0] |= 0x00000001u;
}
inline void DynamicTag2::clear_has_name() {
    _has_bits_[0] &= ~0x00000001u;
}
inline void DynamicTag2::clear_name() {
    if (name_ != &::google::protobuf::internal::kEmptyString) {
        name_->clear();
    }
    clear_has_name();
}
inline const ::std::string& DynamicTag2::name() const {
    return *name_;
}
inline void DynamicTag2::set_name(const ::std::string& value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void DynamicTag2::set_name(const char* value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void DynamicTag2::set_name(const char* value, size_t size) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* DynamicTag2::mutable_name() {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    return name_;
}
inline ::std::string* DynamicTag2::release_name() {
    clear_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        return NULL;
    } else {
        ::std::string* temp = name_;
        name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
        return temp;
    }
}

// -------------------------------------------------------------------

// TagDynamicTest

// optional string name = 1;
inline bool TagDynamicTest::has_name() const {
    return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TagDynamicTest::set_has_name() {
    _has_bits_[0] |= 0x00000001u;
}
inline void TagDynamicTest::clear_has_name() {
    _has_bits_[0] &= ~0x00000001u;
}
inline void TagDynamicTest::clear_name() {
    if (name_ != &::google::protobuf::internal::kEmptyString) {
        name_->clear();
    }
    clear_has_name();
}
inline const ::std::string& TagDynamicTest::name() const {
    return *name_;
}
inline void TagDynamicTest::set_name(const ::std::string& value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void TagDynamicTest::set_name(const char* value) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(value);
}
inline void TagDynamicTest::set_name(const char* value, size_t size) {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* TagDynamicTest::mutable_name() {
    set_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        name_ = new ::std::string;
    }
    return name_;
}
inline ::std::string* TagDynamicTest::release_name() {
    clear_has_name();
    if (name_ == &::google::protobuf::internal::kEmptyString) {
        return NULL;
    } else {
        ::std::string* temp = name_;
        name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
        return temp;
    }
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_dynamic_5ftag_2eproto__INCLUDED
