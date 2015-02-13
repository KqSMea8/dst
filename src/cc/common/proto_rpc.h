/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  protobuf for rpc.
 *
 */

#ifndef __DSTREAM_CC_COMMON_PROTO_RPC_H__ // NOLINT
#define __DSTREAM_CC_COMMON_PROTO_RPC_H__ // NOLINT

#include "common/logger.h"
#include "common/proto_serializer.h"
#include "common/rpc.h"

namespace dstream {
namespace proto_rpc {

// ------------------------------------------------------------
// T is a protobuf object. wrapper it to be a Request.
template <class T>
class RPCRequest : public rpc::Request, public T {
public:
    virtual ~RPCRequest() {
    }
    virtual bool Serialize(WriteableByteArray* bytes) const;
    virtual bool Deserialize(ReadableByteArray* bytes);
}; // class RPCRequest

template <class T>
bool RPCRequest<T>::Serialize(WriteableByteArray* bytes) const {
    return proto_serializer::SerializeToWriteBuffer(this, bytes);
}

template <class T>
bool RPCRequest<T>::Deserialize(ReadableByteArray* bytes) {
    return proto_serializer::DeserizlizeFromReadArray(this, bytes);
}

// ------------------------------------------------------------
// T is a protobuf object. wrapper it to be a Response.
template <class T>
class RPCResponse : public T, public rpc::Response {
public:
    virtual ~RPCResponse() {
    }
    virtual bool Serialize(WriteableByteArray* bytes) const;
    virtual bool Deserialize(ReadableByteArray* bytes);
}; // class RPCResponse

template <class T>
bool RPCResponse<T>::Serialize(WriteableByteArray* bytes) const {
    return proto_serializer::SerializeToWriteBuffer(this, bytes);
}

template <class T>
bool RPCResponse<T>::Deserialize(ReadableByteArray* bytes) {
    return proto_serializer::DeserizlizeFromReadArray(this, bytes);
}

// ------------------------------------------------------------
// REQUEST and RESPONSE are protobuf objects.
template <class REQUEST, class RESPONSE>
class RPCService : public rpc::Service {
public:
    virtual rpc::Request* AllocateRequest();
    virtual rpc::Response* AllocateResponse();
    virtual void Run(const rpc::Request* request, rpc::Response* response);
    virtual std::string handleRequest(const REQUEST* req, RESPONSE* res) = 0;
}; // class RPCService.

template <class REQUEST, class RESPONSE>
rpc::Request* RPCService<REQUEST, RESPONSE>::AllocateRequest() {
    return new RPCRequest<REQUEST>();
}

template <class REQUEST, class RESPONSE>
rpc::Response* RPCService<REQUEST, RESPONSE>::AllocateResponse() {
    return new RPCResponse<RESPONSE>();
}

template <class REQUEST, class RESPONSE>
void RPCService<REQUEST, RESPONSE>::Run(const rpc::Request* request, rpc::Response* response) {
    const RPCRequest<REQUEST>* req = dynamic_cast<const RPCRequest<REQUEST>*>(request); // NOLINT
    RPCResponse<RESPONSE>* res = dynamic_cast<RPCResponse<RESPONSE>*>(response); // NOLINT
    std::string return_value = handleRequest(req, res);
    res->set_return_value(return_value.c_str());
}

} // namespace proto_rpc
} // namespace dstream

#endif //  NOLINT
