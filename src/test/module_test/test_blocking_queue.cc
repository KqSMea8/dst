/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief  module test of blocking queue, check queue works or not.
 *
 */

#include <cassert>
#include "common/blocking_queue.h"
#include <gtest/gtest.h>


using namespace dstream;

// declare testQ
BlockingQueue<int> Q_int;
BlockingQueue<std::string> Q_string;

int get_rand(int min, int max)
{
    srand(time(NULL));
    return rand() % (max - min + 1) + min;
}

/**
 * @brief
**/
class BlcockingQ_suite : public ::testing::Test
{
protected:
    BlcockingQ_suite() {};
    virtual ~BlcockingQ_suite() {};
    virtual void SetUp()
    {
    };
    virtual void TearDown()
    {
    };
public:
    static void ASSERT_FAIL()
    {
        ASSERT_TRUE(false);
    };

    static void EXPECT_FAIL()
    {
        EXPECT_TRUE(false);
    };
};

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


/**
 * @brief
**/
TEST_F(BlcockingQ_suite, test_basic)
{
    // test Q_int
    int start_int = -1000;
    for (int i = 0; i < 2000; i++)
    {
        Q_int.Put(start_int++);
    }
    ASSERT_EQ(2000u, Q_int.size());

    start_int = -1000;
    for (int i = 0; i < 2000; i++)
    {
        if ((start_int++) != Q_int.Take())
        {
            ASSERT_TRUE(0);
        }
    }
    ASSERT_EQ(0u, Q_int.size());
}

/**
 * @brief multi-write and multi-read
 * @begin_version
**/
void* read_routine(void* /*arg*/)
{
    string test_str;
    int i = 0;
    for (; i < 5000; i++)
    {
        test_str = Q_string.Take();
        usleep(get_rand(1, 100));
    }
    //std::cout<<test_str.c_str()<<":"<<i<<endl;
    return NULL;
}
void* write_routine(void* /*arg*/)
{
    string test_str = "test_str";
    for (int i = 0; i < 5000 + 1; i++)
    {
        Q_string.Put(test_str);
        usleep(get_rand(1, 100));
    }
    return NULL;
}
TEST_F(BlcockingQ_suite, test_blocking)
{
    int write_threads = 50;
    int read_threads = 50;
    pthread_t write_tid[write_threads];
    pthread_t read_tid[read_threads];
    int ret = 0;

    for (int i = 0; i < read_threads; i++)
    {
        ret = pthread_create(&read_tid[i], NULL, read_routine, NULL);
        if (0 != ret)
        {
            ASSERT_TRUE(0);
        }
    }
    for (int i = 0; i < write_threads; i++)
    {
        ret = pthread_create(&write_tid[i], NULL, write_routine, NULL);
        if (0 != ret)
        {
            ASSERT_TRUE(0);
        }
    }

    for (int i = 0; i < write_threads; i++)
    {
        ret = pthread_join(write_tid[i], NULL);
        if (0 != ret)
        {
            ASSERT_TRUE(0);
        }
    }
    for (int i = 0; i < read_threads; i++)
    {
        ret = pthread_join(read_tid[i], NULL);
        if (0 != ret)
        {
            ASSERT_TRUE(0);
        }
    }
}

/**
 * @brief
 * @begin_version
**/
TEST_F(BlcockingQ_suite, test_exception)
{
    // current no exception
}




