/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file mock_join_table.h
 * @author lanbijia
 * @date 2012/9/22
 * @brief mock class JoinTable
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_JOIN_TABLE_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_JOIN_TABLE_H__

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mocked class
#include "generic_operator/join_table.h"
#include "generic_operator/multi_join_table.h"
#include "generic_operator/timed_sliding_windows.h"

namespace dstream {
namespace generic_operator {

template < class K,
         class V,
         class KeySerializer = DefaultSerializer<K>,
         class ValueSerializer = DefaultSerializer<V>,
         class Hash = boost::hash<K>,
         class Pred = std::equal_to<K> >
class MockMultiJoinTable : public MultiJoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>
{
public:
    MOCK_METHOD2_T(Init, int(const std::string&, storage::StoreAccessor*));
    MOCK_METHOD0_T(Destroy, int());
    MOCK_METHOD2_T(Insert, int(const K&, const V&));
    MOCK_METHOD2_T(Find, int(const K&, std::vector<V>*));
    MOCK_METHOD1_T(Remove, int(const K&));
};

template < class K,
         class V,
         class KeySerializer = DefaultSerializer<K>,
         class ValueSerializer = DefaultSerializer<V>,
         class Hash = boost::hash<K>,
         class Pred = std::equal_to<K> >
class MockSingleJoinTable : public JoinTable<K, V, KeySerializer, ValueSerializer, Hash, Pred>
{
public:
    MOCK_METHOD2_T(Init, int(const std::string&, storage::StoreAccessor*));
    MOCK_METHOD0_T(Destroy, int());
    MOCK_METHOD3_T(Insert, int(const K&, const V&, bool));
    MOCK_METHOD2_T(Find, int(const K&, V*));
    MOCK_METHOD1_T(Remove, int(const K&));
};


} // namespace generic_operator
} // namespace dstream

#endif
