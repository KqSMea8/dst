/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTOR_TAG_H__
#define __DSTREAM_SRC_CC_TRANSPORTOR_TAG_H__

#include <boost/unordered_map.hpp>
#include <set>
#include <string>
#include <vector>
#include "common/config_const.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h" // for Tag define

namespace dstream {
namespace transporter {

const int32_t kMaxTagLen = 512;

class TPOutflow;
/**
* @brief
* @notice shares lifecycle with transporter.
*/
class TPTagMap {
public:
    typedef std::vector<TPOutflow*>                    PtrVec;
    typedef std::set<TPOutflow*>                       PtrSet;
    typedef const PtrVec*                              SubVecPtr;
    typedef boost::unordered_map< std::string, PtrVec> SubMap;

public:
    TPTagMap() {}
    ~TPTagMap() {}

public:
    //-----------------------------------------------
    // tag sub query, called very frequently
    // DSTREAM_FORCE_INLINE SubVecPtr
    SubVecPtr
    GetSubs(const std::string& tag) {
        if (tag.empty()) {
            return &m_all_subs_vec;
        }
        SubMap::const_iterator it = m_sub_map.find(tag);
        UNLIKELY_IF(it == m_sub_map.end()) { // new tag
            if (LIKELY(!m_sub_all_vec.empty())) {
                m_sub_map[tag] = m_sub_all_vec;
                return &m_sub_all_vec;
            }
            return NULL;
        }
        return &(it->second);
    }

    //-----------------------------------------------
    // tag sub query
    ErrCode SubTags(TPOutflow* p, const std::string& tag);
    ErrCode SubTags(TPOutflow* p, const std::vector<std::string>& tags);
    ErrCode DelTags(TPOutflow* p, const std::string& tag);
    ErrCode DelTags(TPOutflow* p);
    ErrCode UpdTags(TPOutflow* p, const std::vector<std::string> tags);

    //-----------------------------------------------
    // debug
    ErrCode Dump(std::string* s);

private:
    // DSTREAM_FORCE_INLINE
    bool    _IsAllTag(const std::string& tag);
    // DSTREAM_FORCE_INLINE
    bool    _IsAllTag(const std::vector<std::string>& tags);
    ErrCode _SubAllTags(TPOutflow* p);
    ErrCode _TestAndInsert(PtrVec* vec, TPOutflow* p);
    ErrCode _TestAndDelete(PtrVec* vec, TPOutflow* p);

private:
    SubMap                   m_sub_map;
    PtrVec                   m_sub_all_vec;
    PtrVec                   m_all_subs_vec;
}; // class Transportor

} // namespace transporter
} // namespace dstream

#endif // __DSTREAM_SRC_CC_TRANSPORTOR_TAG_H__
