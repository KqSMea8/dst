/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_CC_COMMON_ID_H__
#define __DSTREAM_CC_COMMON_ID_H__

#include <stdint.h>

namespace dstream {
namespace common {

#define S_DOTTED_PEID           "%lu.%lu.%lu"
#define DOTTED_PEID(id)          static_cast<uint64_t>(((id) & 0xFFFFFF0000000000ULL) >> 40), \
                                 static_cast<uint64_t>(((id) & 0x000000FF00000000ULL) >> 32), \
                                 static_cast<uint64_t>(((id) & 0x00000000FFFFFF00ULL) >> 8)
#define EXTRACT_APP_ID(id)       static_cast<uint64_t>(((id) & 0xFFFFFF0000000000ULL) >> 40)
#define EXTRACT_PROCESSOR_ID(id) static_cast<uint64_t>(((id) & 0x000000FF00000000ULL) >> 32)
#define EXTRACT_PE_ID (id)       static_cast<uint64_t>(((id) & 0xFFFFFFFFFFFFFF00ULL) >> 8)
#define EXTRACT_SEQUENCE_ID(id)  static_cast<uint64_t>(((id) & 0x00000000FFFFFF00ULL) >> 8)
#define EXTRACT_APPPRO_ID(id)    static_cast<uint64_t>(((id) & 0xFFFFFFFF00000000ULL) >> 32)
#define EXTRACT_PROSEQ_ID(id)    static_cast<uint64_t>(((id) & 0x000000FFFFFFFF00ULL) >> 8)

#define EXTRACT_APP_ID_FROM_APPPRO_ID(id) \
    static_cast<uint64_t>(((id) & 0xFFFFFF00) >> 8)

#define MAKE_PROCESSOR_PE_ID(pro_id, seq) \
    static_cast<uint64_t>(static_cast<uint64_t>(pro_id) << 32 | \
    static_cast<uint64_t>(static_cast<uint64_t>(seq) << 8))

#define MAKE_PEID(app_id, pro_id, seq) \
    static_cast<uint64_t>(static_cast<uint64_t>((app_id) << 40) | \
            static_cast<uint64_t>((pro_id) << 32) | \
            static_cast<uint64_t>((seq) << 8))

#define MAKE_APP_PROCESSOR_ID(app_id, pro_id) \
    static_cast<uint64_t>(static_cast<uint64_t>((app_id) << 8) | \
                          static_cast<uint64_t>((pro_id)))


} // namespace common
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_ID_H__
