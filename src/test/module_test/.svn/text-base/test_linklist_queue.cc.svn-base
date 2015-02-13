/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liyuanjian(@baidu.com)
 * @brief  module test of linked_list_queue
 *
 */

#include <cstdio>
#include <cassert>
#include "common/linked_list_queue.h"
#include <gtest/gtest.h>
#include <pthread.h>

using namespace dstream;

struct Entry {
    int value;
    Entry* next;
    Entry(int _value):
        value(_value), next(NULL) {
    }
};

LinkedListQueue<Entry*> Q;


//---------------------Gtest-----------------------------
class LinkListQ_suite : public ::testing::Test
{
protected:
    LinkListQ_suite() {};
    virtual ~LinkListQ_suite() {};
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

//-------------------------------------------------------
void* push_routine(void* arg)
{
    for (int i = 1; i < 10; i++) {
        Q.push(new Entry(i));
    }
    return NULL;
}

void* pop_routine(void* arg)
{
    sleep(2);
    for (int j = 1; j < 10; j++)
    {
        Entry* t = Q.pop();
        std::cout << t->value << " ";
        delete t;
    }
    std::cout << endl;
    return NULL;
}

TEST_F(LinkListQ_suite, test_list)
{
    int push_threads = 10;
    int pop_threads = 10;

    pthread_t push_tid[push_threads];
    pthread_t pop_tid[pop_threads];

    int ret = 0;

    for (int i = 0; i < push_threads; i++)
    {
        ret = pthread_create(&push_tid[i], NULL, push_routine, NULL);
        ASSERT_EQ(0, ret);
    }

    for (int i = 0; i < pop_threads; i++)
    {
        ret = pthread_create(&pop_tid[i], NULL, pop_routine, NULL);
        ASSERT_EQ(0, ret);
    }

    for (int i = 0; i < push_threads; i++)
    {
        ret = pthread_join(push_tid[i], NULL);
        ASSERT_EQ(0, ret);
        ASSERT_EQ(1, Q.head()->value);
    }

    for (int i = 0; i < pop_threads; i++)
    {
        ret = pthread_join(pop_tid[i], NULL);
        ASSERT_EQ(0, ret);
    }
    sleep(1);
    ASSERT_EQ(NULL, Q.head());
    ASSERT_EQ(NULL, Q.tail());
}














