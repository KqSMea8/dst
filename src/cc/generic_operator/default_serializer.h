/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
/**
 * @file default_serializer.h
 * @author luxing
 * @date 2012/11/01
 * @brief 定义default serializer，用于对常见类型进行序列化和反序列化
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_DEFAULT_SERIALIZER_H__ // NOLINT
#define __DSTREAM_GENERIC_OPERATOR_DEFAULT_SERIALIZER_H__ // NOLINT

#include <string>

namespace dstream {
namespace generic_operator {

/**
 * @brief DefaultSerializer模版类，用于序列化和反序列化数据
 *
 * @param T 被序列化和反序列化的类型
 */
template<class T>
class DefaultSerializer {
public:
    /**
     * @brief 序列化
     *
     * @param t[in] 需要序列化的对象
     * @param output[out] 返回序列化的结果
     *
     * @return 序列化是否成功
     */
    bool Serialize(const T& t, std::string* output);

    /**
     * @brief 反序列化
     *
     * @param input[in] 反序列化的数据
     * @param t[out] 返回反序列化的结果
     *
     * @return 反序列化是否成功
     */
    bool Deserialize(const std::string& input, T* t);
};

template<>
class DefaultSerializer<std::string> {
public:
    bool Serialize(const std::string& t, std::string* output) {
        assert(NULL != output);
        *output = t;
        return true;
    }

    bool Deserialize(const std::string& input, std::string* t) {
        assert(NULL != t);
        *t = input;
        return true;
    }
};

template<>
class DefaultSerializer<int> {
public:
    bool Serialize(const int& t, std::string* output) {
        assert(NULL != output);
        output->assign(reinterpret_cast<const char*>(&t), sizeof(int));
        return true;
    }

    bool Deserialize(const std::string& input, int* t) {
        assert(NULL != t);
        if (input.size() != sizeof(int)) {
            return false;
        }
        memcpy(t, input.data(), input.size());
        return true;
    }
};

} // namespace generic_operator
} // namespace dstream

#endif // __DSTREAM_GENERIC_OPERATOR_DEFAULT_SERIALIZER_H__ NOLINT
