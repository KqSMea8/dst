/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2012/4/2012/04/12
* @filename:    dstream_type.h
* @author:      zhanggengxin@baidu.com
* @brief:       dstream type types and typedefine
*/

#ifndef __DSTREAM_CC_COMMON_DSTREAM_TYPE_H__
#define __DSTREAM_CC_COMMON_DSTREAM_TYPE_H__
#include <set>
#include "common/auto_ptr.h"
#include "common/condition.h"
#include "common/config.h"
#include "common/config_const.h"
#include "common/error.h"
#include "common/logger.h"
#include "common/thread.h"

// include common proto file
#include "common/proto/dstream_proto_common.pb.h"

namespace dstream {
// define types used for resource scheduler
// Compare ProcessorID
class ProcessorIDComapre {
public:
    bool operator() (const ProcessorID& pid1, const ProcessorID& pid2) const;
};

// compare pn id
class PNIDCompare {
public:
    bool operator() (const PNID& pn1, const PNID& pn2) const;
};

typedef std::set<PNID, PNIDCompare> PNIDSet;
typedef PNIDSet::iterator PNIDSetIter;

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_DSTREAM_TYPE_H__

