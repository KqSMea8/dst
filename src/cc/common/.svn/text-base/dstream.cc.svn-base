/**
* @file       dstream.cc
* @brief      default const defining file
* @author   konghui@baidu.com
* @version  1.0.1.0
* @date       2011-11-29
* Copyright (c) 2011, Baidu, Inc. All rights reserved.
*/

#include "common/dstream.h"

namespace dstream {

/* config file name */
const char* kClusterCfgFileName   = "cluster.cfg.xml";
const char* kAppCfgFileName       = "application.cfg.xml";

const char* PEEventTypeToStr(int type) {
    switch (type) {
    case PEET_STOP_DUE_TO_UPD:
        return "quit due to update";
    case PEET_STOP_DUE_TO_DEL:
        return "quit due to deletion";
    case PEET_UNKOWN:
        return "unkown";
    default:
        return "unkown type";
    };
}

const char* DataStrategyTypeToStr(int type) {
    switch (type) {
    case DST_WHEN_QUEUE_FULL_DROP_AGELONG:
        return "drop agelong data when queue is full";
    case DST_WHEN_QUEUE_FULL_DROP_FRESH:
        return "drop fresh data when queue is full";
    case DST_WHEN_QUEUE_FULL_HOLD:
        return "hold stream when queue is full";
    default:
        return "unkown type";
    };
}

} // namespace dstream
