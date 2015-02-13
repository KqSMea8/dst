/**
* @file       dstream.h
* @brief      default const defining file
* @author   konghui@baidu.com
* @version      1.0.1.0
* @date         2011-11-29
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_CC_COMMON_DSTREAM_H__
#define __DSTREAM_CC_COMMON_DSTREAM_H__

namespace dstream {

enum PEEventType {
    PEET_STOP_DUE_TO_UPD             = 0,   // PE���ڸ��¶��˳�
    PEET_STOP_DUE_TO_DEL             = 1,   // PE���ڱ�ɾ�����˳�
    PEET_UNKOWN                      = 2,   // PE����δ֪ԭ���˳�
    PEET_TYPE_END                    = 1000
};
const char* PEEventTypeToStr(int type);

enum DataStrategyType {
    DST_WHEN_QUEUE_FULL_DROP_AGELONG = 0,
    DST_WHEN_QUEUE_FULL_DROP_FRESH   = 1,
    DST_WHEN_QUEUE_FULL_HOLD         = 2,
    DST_TYPE_END                     = 1000
};

const char* DataStrategyTypeToStr(int type);
inline bool  IsValidDataStrategyType(int type) {
    return (type >= 0) && (type < DST_TYPE_END);
}

/* config file name */
extern const char* kClusterCfgFileName;
extern const char* kAppCfgFileName;

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_DSTREAM_H__
