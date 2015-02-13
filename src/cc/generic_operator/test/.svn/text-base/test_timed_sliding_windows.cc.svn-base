#include "generic_operator/timed_sliding_windows.h"
#include <gtest/gtest.h>
#include "generic_operator/test/mock_storage.h"

using namespace dstream;
using namespace dstream::generic_operator;

class TestEnv : public ::testing::Environment {
public:
    virtual void SetUp() {
        logger::initialize("timed_sliding_windows_test");
    }

    virtual void TearDown() {
    }
};

class WindowData {
public:
    uint32_t ts;
    std::set<uint32_t> t_set;
};

typedef boost::shared_ptr<WindowData> WindowDataPtr;

class TimedSlidingWindowsTest : public ::testing::Test {
public:
    TimedSlidingWindowsTest();

    static int CreatingCallback(WindowDataPtr data_ptr,
                                uint32_t ts,
                                storage::IStoreTablePtr store_table,
                                void* context);

    static int SimpleSlidingOutCallback(WindowDataPtr data_ptr,
                                        uint32_t ts,
                                        void* context);

    static int ComplexSlidingOutCallback(WindowDataPtr data_ptr,
                                         uint32_t ts,
                                         void* context);

protected:
    void CheckWindow(WindowDataPtr data_ptr, uint32_t t);

    void CheckNewWindow(WindowDataPtr data_ptr, uint32_t ts);
    void CheckSlidingOutWindow(WindowDataPtr data_ptr, uint32_t ts);
    void CheckFullSlidingOutWindow(WindowDataPtr data_ptr, uint32_t ts);

    uint32_t step_;
    uint32_t size_by_step_;
    uint32_t create_num_;
    uint32_t slide_out_num_;
};

TimedSlidingWindowsTest::TimedSlidingWindowsTest()
    : step_(5),
      size_by_step_(100),
      create_num_(0),
      slide_out_num_(0) {
}

int TimedSlidingWindowsTest::CreatingCallback(WindowDataPtr data_ptr,
                                              uint32_t ts,
                                              storage::IStoreTablePtr /*store_table*/,
                                              void* context) {
    TimedSlidingWindowsTest* p_this = reinterpret_cast<TimedSlidingWindowsTest*>(context);
    ++p_this->create_num_;
    p_this->CheckNewWindow(data_ptr, ts);
    data_ptr->ts = ts;
    return 0;
}

void TimedSlidingWindowsTest::CheckNewWindow(WindowDataPtr data_ptr, uint32_t ts) {
    ASSERT_TRUE(NULL != data_ptr);
    ASSERT_EQ(0u, ts % step_);
}

int TimedSlidingWindowsTest::SimpleSlidingOutCallback(WindowDataPtr data_ptr,
        uint32_t ts,
        void* context) {
    TimedSlidingWindowsTest* p_this = reinterpret_cast<TimedSlidingWindowsTest*>(context);
    ++p_this->slide_out_num_;
    p_this->CheckSlidingOutWindow(data_ptr, ts);
    return 0;
}

void TimedSlidingWindowsTest::CheckSlidingOutWindow(WindowDataPtr data_ptr, uint32_t ts) {
    ASSERT_EQ(0u, ts % step_);

    if (NULL != data_ptr) {
        ASSERT_EQ(ts, data_ptr->ts);
        for (std::set<uint32_t>::const_iterator itr = data_ptr->t_set.begin();
             itr != data_ptr->t_set.end();
             ++itr) {
            ASSERT_LE(ts, *itr);
            ASSERT_GT(ts + step_, *itr);
        }
    }
}

int TimedSlidingWindowsTest::ComplexSlidingOutCallback(WindowDataPtr data_ptr,
        uint32_t ts,
        void* context) {
    TimedSlidingWindowsTest* p_this = reinterpret_cast<TimedSlidingWindowsTest*>(context);
    p_this->CheckFullSlidingOutWindow(data_ptr, ts);
    ++p_this->slide_out_num_;
    return 0;
}

void TimedSlidingWindowsTest::CheckFullSlidingOutWindow(WindowDataPtr data_ptr, uint32_t ts) {
    ASSERT_EQ(0u, ts % step_);

    if (NULL != data_ptr) {
        ASSERT_EQ(ts, data_ptr->ts);
        ASSERT_EQ(step_, data_ptr->t_set.size());
        for (std::set<uint32_t>::const_iterator itr = data_ptr->t_set.begin();
             itr != data_ptr->t_set.end();
             ++itr) {
            ASSERT_EQ(ts++, *itr);
        }
        ASSERT_EQ(data_ptr->ts + step_, ts);
    }
}

void TimedSlidingWindowsTest::CheckWindow(WindowDataPtr data_ptr, uint32_t t) {
    ASSERT_TRUE(NULL != data_ptr);
    // check ts <= t < ts + step_
    ASSERT_GE(t, data_ptr->ts);
    ASSERT_LT(t, data_ptr->ts + step_);
}

TEST_F(TimedSlidingWindowsTest, SimpleTest) {
    step_ = 5;
    size_by_step_ = 100;
    create_num_ = 0;
    slide_out_num_ = 0;

    TimedSlidingWindows<WindowData> windows;
    ASSERT_EQ(0, windows.Init(step_, size_by_step_));
    windows.SetWindowCreatingCallback(CreatingCallback, this);
    windows.SetWindowSlidingOutCallback(SimpleSlidingOutCallback, this, false);

    // check iterator for empty windows
    TimedSlidingWindowsIterator<WindowData> empty_windows_itr(windows);
    WindowDataPtr empty_data_itr;
    uint32_t empty_ts_itr;
    uint32_t empty_total_num = 0;
    for (; empty_windows_itr.GetCurrent(&empty_data_itr, &empty_ts_itr); empty_windows_itr.Next()) {
        ++empty_total_num;
    }
    ASSERT_EQ(0u, empty_total_num);

    WindowDataPtr data_ptr;
    int ret;
    // get first window
    uint32_t t1 = 10000;
    ret = windows.GetWindow(t1, &data_ptr);
    ASSERT_EQ(0, ret);
    CheckWindow(data_ptr, t1);
    // one window should be created
    ASSERT_EQ(1u, create_num_);
    ASSERT_EQ(0u, slide_out_num_);
    // push time into data_ptr
    data_ptr->t_set.insert(t1);

    // try to get window that is too old
    ret = windows.GetWindow(t1 - 1000, &data_ptr);
    ASSERT_EQ(0, ret);
    ASSERT_TRUE(NULL == data_ptr);
    ASSERT_EQ(1u, create_num_);
    ASSERT_EQ(0u, slide_out_num_);

    // get the last window
    ret = windows.GetWindow(t1 + 499, &data_ptr);
    ASSERT_EQ(0, ret);
    CheckWindow(data_ptr, t1 + 499);
    ASSERT_EQ(2u, create_num_);
    ASSERT_EQ(size_by_step_ - 1, slide_out_num_);

    // get windows in the middle
    create_num_ = 0;
    slide_out_num_ = 0;
    for (uint32_t t = t1; t < t1 + step_ * size_by_step_; ++t) {
        ret = windows.GetWindow(t, &data_ptr);
        ASSERT_EQ(0, ret);
        CheckWindow(data_ptr, t);
        data_ptr->t_set.insert(t);
    }
    ASSERT_EQ(size_by_step_ - 2, create_num_);
    ASSERT_EQ(0u, slide_out_num_);

    // check iterator
    TimedSlidingWindowsIterator<WindowData> windows_itr(windows);
    WindowDataPtr data_itr;
    uint32_t ts_itr;
    uint32_t total_num = 0;
    for (; windows_itr.GetCurrent(&data_itr, &ts_itr); windows_itr.Next()) {
        ASSERT_TRUE(NULL != data_itr);
        ASSERT_EQ(ts_itr, data_itr->ts);
        for (std::set<uint32_t>::const_iterator itr = data_itr->t_set.begin();
             itr != data_itr->t_set.end();
             ++itr) {
            ASSERT_LE(ts_itr, *itr);
            ASSERT_GT(ts_itr + step_, *itr);
        }
        ++total_num;
    }
    ASSERT_EQ(size_by_step_, total_num);
}

TEST_F(TimedSlidingWindowsTest, ComplexTest) {
    step_ = 5;
    size_by_step_ = 100;
    create_num_ = 0;
    slide_out_num_ = 0;

    TimedSlidingWindows<WindowData> windows;
    ASSERT_EQ(0, windows.Init(step_, size_by_step_));
    windows.SetWindowCreatingCallback(CreatingCallback, this);
    windows.SetWindowSlidingOutCallback(ComplexSlidingOutCallback, this, false);

    WindowDataPtr data_ptr;
    int ret;
    // get first window
    uint32_t t1 = step_ * size_by_step_ * 999 + 1;
    ret = windows.GetWindow(t1, &data_ptr);
    ASSERT_EQ(0, ret);
    CheckWindow(data_ptr, t1);
    // one window should be created
    ASSERT_EQ(1u, create_num_);
    ASSERT_EQ(0u, slide_out_num_);
    // push time into data_ptr
    data_ptr->t_set.insert(t1);

    // try to get windows that are too old to appear
    for (uint32_t i = 0; i < 100; ++i) {
        uint32_t old = t1 - (size_by_step_ + i) * step_;
        ret = windows.GetWindow(old, &data_ptr);
        ASSERT_EQ(0, ret);
        ASSERT_TRUE(NULL == data_ptr);
        ASSERT_EQ(1u, create_num_);
        ASSERT_EQ(0u, slide_out_num_);
    }

    // fill windows that are older than t1
    uint32_t t_start = t1 - (size_by_step_ - 1) * step_;
    t_start -= t_start % step_;
    for (uint32_t i = 0; i < step_; ++i) {
        for (uint32_t j = 0; j < size_by_step_; ++j) {
            if (j % 2 != 0) {
                uint32_t t = t_start + step_ * j + i;
                ret = windows.GetWindow(t, &data_ptr);
                ASSERT_EQ(0, ret);
                CheckWindow(data_ptr, t);
                data_ptr->t_set.insert(t);
            }
        }
    }
    ASSERT_EQ(size_by_step_ / 2, create_num_);
    ASSERT_EQ(0u, slide_out_num_);

    // slide the window one by one
    create_num_ = 0;
    slide_out_num_ = 0;
    uint32_t slide_num = size_by_step_ * 2 + 9;
    for (uint32_t i = 1; i <= slide_num; ++i) {
        t1 += step_;
        ret = windows.GetWindow(t1, &data_ptr);
        ASSERT_EQ(0, ret);
        CheckWindow(data_ptr, t1);
        for (uint32_t j = 1; j < i; ++j) {
            uint32_t t_pre = t1 - j * step_;
            ret = windows.GetWindow(t_pre, &data_ptr);
            ASSERT_EQ(0, ret);
            if (j >= size_by_step_) {
                ASSERT_TRUE(NULL == data_ptr);
            } else {
                ASSERT_TRUE(NULL != data_ptr);
                t_pre -= t_pre % step_;
                for (uint32_t k = 0; k < step_; ++k) {
                    data_ptr->t_set.insert(t_pre + k);
                }
            }
        }
    }
    // fill the last window
    ret = windows.GetWindow(t1, &data_ptr);
    ASSERT_EQ(0, ret);
    CheckWindow(data_ptr, t1);
    t1 -= t1 % step_;
    for (uint32_t i = 0; i < step_; ++i) {
        data_ptr->t_set.insert(t1 + i);
    }
    ASSERT_EQ(slide_num, create_num_);
    ASSERT_EQ(slide_num, slide_out_num_);

    // slide the window by 3
    create_num_ = 0;
    slide_out_num_ = 0;
    slide_num = size_by_step_ * 3 + 9;
    for (uint32_t i = 0; i < slide_num / 3; ++i) {
        t1 += step_ * 3;
        ret = windows.GetWindow(t1, &data_ptr);
        ASSERT_EQ(0, ret);
        CheckWindow(data_ptr, t1);
        for (uint32_t j = 1; j <= 3; ++j) {
            uint32_t t_pre = t1 - j * step_;
            ret = windows.GetWindow(t_pre, &data_ptr);
            ASSERT_EQ(0, ret);
            if (j >= size_by_step_) {
                ASSERT_TRUE(NULL == data_ptr);
            } else {
                ASSERT_TRUE(NULL != data_ptr);
                t_pre -= t_pre % step_;
                for (uint32_t k = 0; k < step_; ++k) {
                    data_ptr->t_set.insert(t_pre + k);
                }
            }
        }
    }
    // fill the last window
    ret = windows.GetWindow(t1, &data_ptr);
    ASSERT_EQ(0, ret);
    CheckWindow(data_ptr, t1);
    t1 -= t1 % step_;
    for (uint32_t i = 0; i < step_; ++i) {
        data_ptr->t_set.insert(t1 + i);
    }
    ASSERT_EQ(slide_num, create_num_);
    ASSERT_EQ(slide_num, slide_out_num_);

    // check iterator
    TimedSlidingWindowsIterator<WindowData> windows_itr(windows);
    WindowDataPtr data_ptr_itr;
    uint32_t ts_itr;
    uint32_t total_num = 0;
    for (; windows_itr.GetCurrent(&data_ptr_itr, &ts_itr); windows_itr.Next()) {
        ASSERT_TRUE(NULL != data_ptr_itr);
        ASSERT_EQ(ts_itr, data_ptr_itr->ts);
        ASSERT_EQ(step_, data_ptr_itr->t_set.size());
        for (std::set<uint32_t>::const_iterator itr = data_ptr_itr->t_set.begin();
             itr != data_ptr_itr->t_set.end();
             ++itr) {
            ASSERT_EQ(ts_itr++, *itr);
        }
        ASSERT_EQ(data_ptr_itr->ts + step_, ts_itr);
        ++total_num;
    }
    ASSERT_EQ(size_by_step_, total_num);

    // slide out all windows
    create_num_ = 0;
    slide_out_num_ = 0;
    windows.GetWindow(t1 + size_by_step_ * 10, &data_ptr);
    ASSERT_EQ(1u, create_num_);
    ASSERT_EQ(size_by_step_, slide_out_num_);
}

class TimedSlidingWindowsStoreTest : public ::testing::Test {
public:
    static int AssertCreatingCallback(boost::shared_ptr<int> /*data_ptr*/,
                                      uint32_t /*ts*/,
                                      storage::IStoreTablePtr /*store_table*/,
                                      void* context) {
        TimedSlidingWindowsStoreTest* p_this = reinterpret_cast<TimedSlidingWindowsStoreTest*>(context);
        p_this->AssertCallback();
        return 0;
    }

    static int AssertSlidingOutCallback(boost::shared_ptr<int> /*data_ptr*/,
                                        uint32_t /*ts*/,
                                        void* context) {
        TimedSlidingWindowsStoreTest* p_this = reinterpret_cast<TimedSlidingWindowsStoreTest*>(context);
        p_this->AssertCallback();
        return 0;
    }

    static int CreatingCallback(boost::shared_ptr<int> data_ptr,
                                uint32_t ts,
                                storage::IStoreTablePtr /*store_table*/,
                                void* /*context*/) {
        *data_ptr = ts;
        return 0;
    }

    static int SlidingOutCallback(boost::shared_ptr<int> /*data_ptr*/,
                                  uint32_t /*ts*/,
                                  void* /*context*/) {
        return 0;
    }

    static int RestoreCreatingCallback(boost::shared_ptr<int> data_ptr,
                                       uint32_t ts,
                                       storage::IStoreTablePtr /*store_table*/,
                                       void* /*context*/) {
        *data_ptr = ts;
        return 0;
    }

    static int RestoreSlidingOutCallback(boost::shared_ptr<int> /*data_ptr*/,
                                         uint32_t /*ts*/,
                                         void* context) {
        // should not sliding out in restore stage
        TimedSlidingWindowsStoreTest* p_this = reinterpret_cast<TimedSlidingWindowsStoreTest*>(context);
        p_this->AssertCallback();
        return 0;
    }

protected:
    void AssertCallback() {
        // the callback should never be invoked
        ASSERT_TRUE(false);
    }
};

TEST_F(TimedSlidingWindowsStoreTest, StoreTest) {
    dstream::storage::MockStoreAccessor mock_store_accessor;

    TimedSlidingWindows<int> windows;
    uint32_t step = 100;
    uint32_t size_by_step = 10;
    int ret = windows.Init(step, size_by_step);
    ASSERT_EQ(0, ret);

    windows.SetWindowCreatingCallback(AssertCreatingCallback, this);
    windows.SetWindowSlidingOutCallback(AssertSlidingOutCallback, this, false);

    // set store
    ret = windows.SetStore(&mock_store_accessor, "test_window", true, true, storage::HASH_TABLE);
    ASSERT_EQ(0, ret);

    // the windows should be empty
    boost::shared_ptr<int> data_ptr;
    uint32_t ts;
    uint32_t total = 0;
    uint32_t data_num = 0;
    for (TimedSlidingWindowsIterator<int> itr(windows);
         itr.GetCurrent(&data_ptr, &ts);
         itr.Next()) {
        ++total;
        if (NULL != data_ptr) {
            ++data_num;
        }
    }
    ASSERT_EQ(0u, total);
    ASSERT_EQ(0u, data_num);

    windows.SetWindowCreatingCallback(CreatingCallback, this);
    windows.SetWindowSlidingOutCallback(SlidingOutCallback, this, false);

    // create some windows
    std::set<uint32_t> ts_set;
    std::vector<uint32_t> expected_ts_vec;
    uint32_t start_ts = step * size_by_step * 999;
    for (size_t i = 0; i < size_by_step; ++i) {
        if (i % 3 == 0) {
            uint32_t get_ts = start_ts + step * i;
            ret = windows.GetWindow(get_ts, &data_ptr);
            ASSERT_EQ(0, ret);
            if (ts_set.find(get_ts) == ts_set.end()) {
                ts_set.insert(get_ts);
                expected_ts_vec.push_back(get_ts);
            }
        }
    }
    for (size_t i = 0; i < size_by_step; ++i) {
        if (i % 2 == 0) {
            uint32_t get_ts = start_ts + step * i;
            ret = windows.GetWindow(get_ts, &data_ptr);
            ASSERT_EQ(0, ret);
            if (ts_set.find(get_ts) == ts_set.end()) {
                ts_set.insert(get_ts);
                expected_ts_vec.push_back(get_ts);
            }
        }
    }
    std::sort(expected_ts_vec.begin(), expected_ts_vec.end());

    // now restore the data into a new windows
    TimedSlidingWindows<int> windows_restore;
    ret = windows_restore.Init(step, size_by_step);
    ASSERT_EQ(0, ret);

    windows_restore.SetWindowCreatingCallback(CreatingCallback, this);
    windows_restore.SetWindowSlidingOutCallback(SlidingOutCallback, this, false);

    // set store
    ret = windows_restore.SetStore(&mock_store_accessor, "test_window", true, true,
                                   storage::HASH_TABLE);
    ASSERT_EQ(0, ret);

    // check the restored windows is correct
    total = 0;
    data_num = 0;
    for (TimedSlidingWindowsIterator<int> itr(windows_restore);
         itr.GetCurrent(&data_ptr, &ts);
         itr.Next()) {
        ++total;
        if (NULL != data_ptr) {
            ASSERT_EQ(expected_ts_vec[data_num], ts);
            ++data_num;
        }
    }
    ASSERT_EQ(size_by_step, total);
    ASSERT_EQ(expected_ts_vec.size(), data_num);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TestEnv());
    return RUN_ALL_TESTS();
}
