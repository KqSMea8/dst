/**
 * @file common_utils.h
 * @Brief  ͨ�ù��ߺ�����
 * @author zhenpeng01@baidu.com
 * @version 1.0.0
 * @date 2012-10-17
 */

#ifndef CTR_RTS_COMMON_UTILS_H
#define CTR_RTS_COMMON_UTILS_H

#include <stdint.h>
#include <string>
#include <vector>

enum return_type {
    // common errors
    E_OK                  =   0,
    E_BAD_PARAMS          =   -1000, // ��������

    // importer errors
    E_MESSAGE_TYPE_ERROR  = -2000,   // ��Ϣ���ʹ���
    E_MESSAGE_BODY_ERROR  = -2001,   // ��Ϣ�����
    E_MESSAGE_UNPACK_ERROR = -2002,  // ���ʧ��
    E_MESSAGE_BUFFER_OVERFLOW = -2003, // ���������Ȳ���
    E_MESSAGE_COMPACK_ERROR   = -2004, // COMPACK�������
    //
    // joiner errors
    //
    // exporter errors
};

uint64_t GetStoreID(const uint64_t& id);
int32_t SplitStringEx(const std::string& str, const char token, std::vector<std::string>* vec);

#endif // end CTR_RTS_COMMON_UTILS_H
