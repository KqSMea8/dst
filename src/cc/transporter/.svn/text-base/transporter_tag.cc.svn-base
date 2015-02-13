#include "transporter/transporter_tag.h"
#include <boost/foreach.hpp>

namespace dstream {
namespace transporter {

ErrCode TPTagMap::SubTags(TPOutflow* p, const std::string& tag) {
    _TestAndInsert(&m_all_subs_vec, p);

    if (_IsAllTag(tag)) {
        return _SubAllTags(p);
    }
    SubMap::iterator it = m_sub_map.find(tag);
    if (it != m_sub_map.end()) {
        return _TestAndInsert(&(it->second), p);
    }
    // new tag
    PtrVec v(m_sub_all_vec);
    v.push_back(p);
    m_sub_map[tag] = v;
    return error::OK;
}

ErrCode TPTagMap::SubTags(TPOutflow* p, const std::vector<std::string>& tags) {
    if (_IsAllTag(tags)) {
        return _SubAllTags(p);    // to add new tags
    }
    for (size_t i = 0; i < tags.size(); ++i) {
        SubTags(p, tags[i]);
    }
    return error::OK;
}

ErrCode TPTagMap::DelTags(TPOutflow* p, const std::string& tag) {
    if (_IsAllTag(tag)) {
        return DelTags(p);
    }
    SubMap::iterator it = m_sub_map.find(tag);
    if (it != m_sub_map.end()) {
        if ((error::OK == _TestAndDelete(&(it->second), p)) &&
            it->second.empty()) {
            m_sub_map.erase(it);
            _TestAndDelete(&m_all_subs_vec, p);
        }
    }
    return error::OK;
}

ErrCode TPTagMap::DelTags(TPOutflow* p) {
    _TestAndDelete(&m_sub_all_vec, p);
    _TestAndDelete(&m_all_subs_vec, p);
    for (SubMap::iterator it = m_sub_map.begin();
         it != m_sub_map.end();) {
        if ((error::OK == _TestAndDelete(&(it->second), p)) &&
            it->second.empty()) {
            it = m_sub_map.erase(it);
        } else {
            it++;
        }
    }
    return error::OK;
}

ErrCode TPTagMap::UpdTags(TPOutflow* p, const std::vector<std::string> tags) {
    DelTags(p);
    return SubTags(p, tags);
}

bool TPTagMap::_IsAllTag(const std::string& tag) {
    return tag.empty() || config_const::kTPAllTag == tag;
}

bool TPTagMap::_IsAllTag(const std::vector<std::string>& tags) {
    if (tags.empty()) {
        return true;
    }
    BOOST_FOREACH(const std::string & tag, tags) {
        if (_IsAllTag(tag)) {
            return true;
        }
    }
    return false;
}

ErrCode TPTagMap::_SubAllTags(TPOutflow* p) {
    BOOST_FOREACH(SubMap::value_type & pair, m_sub_map) {
        _TestAndInsert(&pair.second, p);
    }
    _TestAndInsert(&m_sub_all_vec, p);
    _TestAndInsert(&m_all_subs_vec, p);
    return error::OK;
}

ErrCode TPTagMap::_TestAndInsert(PtrVec* vec, TPOutflow* p) {
    PtrVec::const_iterator it = std::find(vec->begin(), vec->end(), p);
    if (it != vec->end()) {
        return error::OK_KEY_ALREADY_EXIST;
    }
    vec->push_back(p);
    return error::OK;
}

ErrCode TPTagMap::_TestAndDelete(PtrVec* vec, TPOutflow* p) {
    PtrVec::iterator it = std::find(vec->begin(), vec->end(), p);
    if (it != vec->end()) {
        vec->erase(it);
        return error::OK;
    }
    return error::OK_KEY_NOT_EXIST;
}

ErrCode TPTagMap::Dump(std::string* s) {
    if (NULL == s) {
        return error::BAD_ARGUMENT;
    }

    s->append("SubMap:[[Subs:");
    StringAppendNumber(s, m_all_subs_vec.size());
    s->append("]:[");
    uint64_t i = 0;
    BOOST_FOREACH(TPOutflow * ptr, m_all_subs_vec) {
        if (i) {
            s->append(", ");
        }
        char ptr_str[64] = {0};
        snprintf(ptr_str, sizeof(ptr_str), "%p", ptr);
        *s += ptr_str;
        ++i;
    }
    s->append("], ");

    s->append("[SubAll:");
    StringAppendNumber(s, m_sub_all_vec.size());
    s->append("]:[");
    i = 0;
    BOOST_FOREACH(TPOutflow * ptr, m_sub_all_vec) {
        if (i) {
            s->append(", ");
        }
        char ptr_str[64] = {0};
        snprintf(ptr_str, sizeof(ptr_str), "%p", ptr);
        *s += ptr_str;
        ++i;
    }
    s->append("], ");

    s->append("[SubMap:");
    StringAppendNumber(s, m_sub_map.size());
    s->append("]:[");
    i = 0;
    BOOST_FOREACH(SubMap::value_type & pair, m_sub_map) {
        if (i) {
            s->append(", ");
        }
        s->append(pair.first);
        s->append(":[");
        uint64_t j = 0;
        BOOST_FOREACH(PtrVec::value_type & ptr, pair.second) {
            if (j) {
                s->append(", ");
            }
            char ptr_str[64] = {0};
            snprintf(ptr_str, sizeof(ptr_str), "%p", ptr);
            *s += ptr_str;
            ++j;
        }
        s->append("]");
        ++i;
    }
    s->append("]");
    return error::OK;
}

} // namespace transporter
} // namespace dstream

