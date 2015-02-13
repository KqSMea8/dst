/****************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*****************************************************************/
/**
 * @file mock_timed_sliding_window.h
 * @author lanbijia
 * @date 2012/9/22
 * @brief mock class TimedSlidingWindow & class MockTimedSlidingWindowsIterator
 */

#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_TIMED_SLIDING_WINDOW_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_TIMED_SLIDING_WINDOW_H__

// system
#include "gtest/gtest.h"
#include "gmock/gmock.h"
// mocked class
#include "generic_operator/timed_sliding_windows.h"

namespace dstream {
namespace generic_operator {

template<class T>
class MockTimedSlidingWindow : public TimedSlidingWindows<T>
{
public:
    typedef boost::shared_ptr<T> WindowDataPtr;
    typedef int (*WindowCreatingCallback)(WindowDataPtr window_data_ptr,
                                          uint32_t ts,
                                          storage::IStoreTablePtr store_table,
                                          void* context);
    typedef int (*WindowSlidingOutCallback)(WindowDataPtr window_data_ptr,
                                            uint32_t ts,
                                            void* context);

    MOCK_METHOD2_T(Init, int(uint32_t, uint32_t));
    MOCK_METHOD5_T(SetStore, int(storage::StoreAccessor*, const std::string&, bool, bool,
                                 storage::StoreTableType));
    MOCK_METHOD2_T(GetWindow, int(uint32_t, boost::shared_ptr<T>*));
    MOCK_METHOD2_T(SetWindowCreatingCallback, void(WindowCreatingCallback, void*));
    MOCK_METHOD3_T(SetWindowSlidingOutCallback, void(WindowSlidingOutCallback, void*, bool));
};

template<class T>
class MockTimedSlidingWindowsIterator : public TimedSlidingWindowsIterator<T>
{
public:
    MockTimedSlidingWindowsIterator() {};
    virtual ~MockTimedSlidingWindowsIterator() {};

    MOCK_METHOD2_T(GetCurrent, bool(T**, uint32_t*));
    MOCK_METHOD0_T(Next, void());
};

} // namespace generic_operator
} // namespace dstream

#endif
