/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include "common/dstream_type.h"

namespace dstream {

bool ProcessorIDComapre::operator() (const ProcessorID& pid1,
                                     const ProcessorID& pid2) const {
    return pid1.id() < pid2.id();
}

bool PNIDCompare::operator() (const PNID& pn1, const PNID& pn2) const {
    return pn1.id() < pn2.id();
}

} // namespace dstream

