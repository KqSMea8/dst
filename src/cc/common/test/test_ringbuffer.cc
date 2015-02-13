#include "common/ringbuffer.h"
#include "common/error.h"
#include "common/logger.h"
#include <gtest/gtest.h>
	
int main(int argc, char **argv)
{
    ::dstream::Logger::initialize("test_ringbuffer");
 	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
/**
 * @brief 
**/
class test_RingBuffer_RingBuffer_suite : public ::testing::Test{
    protected:
        test_RingBuffer_RingBuffer_suite(){};
        virtual ~test_RingBuffer_RingBuffer_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RingBuffer_RingBuffer_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RingBuffer_RingBuffer_suite, *)
        };
};
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_RingBuffer_RingBuffer_suite, case_name1)
{
	//TODO
}
 
/**
 * @brief 
**/
class test_RingBuffer_Init_suite : public ::testing::Test{
    protected:
        test_RingBuffer_Init_suite(){};
        virtual ~test_RingBuffer_Init_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RingBuffer_Init_suite, *)
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RingBuffer_Init_suite, *)
        };
};
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_RingBuffer_Init_suite, case_name1)
{
	//TODO
}
 
/**
 * @brief 
**/
class test_RingBuffer_IsEmpty_suite : public ::testing::Test{
    protected:
        test_RingBuffer_IsEmpty_suite(){};
        virtual ~test_RingBuffer_IsEmpty_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RingBuffer_IsEmpty_suite, *)
            m_ring_buffer = new dstream::common::RingBuffer(32);
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RingBuffer_IsEmpty_suite, *)
            if (NULL != m_ring_buffer) {
                delete m_ring_buffer;
                m_ring_buffer = NULL;
            }
        };
        void TestIsEmpty();
    private:
        dstream::common::RingBuffer* m_ring_buffer;
};

void test_RingBuffer_IsEmpty_suite::TestIsEmpty() {
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Init());
    ASSERT_TRUE(m_ring_buffer->IsEmpty());
}
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_RingBuffer_IsEmpty_suite, IsEmpty)
{
    TestIsEmpty();
}
 
/**
 * @brief 
**/
class test_RingBuffer_Put_suite : public ::testing::Test{
    protected:
        test_RingBuffer_Put_suite(){};
        virtual ~test_RingBuffer_Put_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RingBuffer_Put_suite, *)
            m_ring_buffer = new dstream::common::RingBuffer(2);
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RingBuffer_Put_suite, *)
            if (NULL != m_ring_buffer) {
                delete m_ring_buffer;
                m_ring_buffer = NULL;
            }
        };
        void TestPut();
    private:
        dstream::common::RingBuffer* m_ring_buffer;
};

void test_RingBuffer_Put_suite::TestPut() {
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Init());
    char a = 'a';
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(&a, sizeof(a)));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(&a, sizeof(a)));
    ASSERT_EQ(dstream::error::QUEUE_FULL, m_ring_buffer->Put(&a, sizeof(a)));
}
 
/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_RingBuffer_Put_suite, Put)
{
    TestPut();
}
 
/**
 * @brief 
**/
class test_RingBuffer_Get_suite : public ::testing::Test{
    protected:
        test_RingBuffer_Get_suite(){};
        virtual ~test_RingBuffer_Get_suite(){};
        virtual void SetUp() {
            //Called befor every TEST_F(test_RingBuffer_Get_suite, *)
            m_ring_buffer = new dstream::common::RingBuffer(12);
        };
        virtual void TearDown() {
            //Called after every TEST_F(test_RingBuffer_Get_suite, *)
            if (NULL != m_ring_buffer) {
                delete m_ring_buffer;
                m_ring_buffer = NULL;
            }
        };
        void TestGet();
    private:
        dstream::common::RingBuffer* m_ring_buffer;
};

void test_RingBuffer_Get_suite::TestGet() {
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Init());
    std::string a = "hello";
    std::string b = "world";
    std::string c = "dstream";
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(a.c_str(), a.length()));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(b.c_str(), b.length()));
    ASSERT_EQ(dstream::error::QUEUE_FULL, m_ring_buffer->Put(c.c_str(), c.length()));
    char d[9];
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get(d, sizeof(d) - 1));
    d[8] = '\0';
    ASSERT_STREQ(d, "hellowor");
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(c.c_str(), c.length()));
    char e[10];
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get(e, sizeof(e) - 1));
    e[9] = '\0';
    ASSERT_STREQ(e, "lddstream");
    ASSERT_TRUE(m_ring_buffer->IsEmpty());

    std::string f = "a";
    std::string g = "b";
    int len = f.length();
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put((char*)(&len), sizeof(len)));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(f.c_str(), f.length()));
    len = g.length();
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put((char*)(&len), sizeof(len)));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(g.c_str(), g.length()));
    ASSERT_EQ(dstream::error::QUEUE_FULL, m_ring_buffer->Put(a.c_str(), a.length()));
    char h[2];
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get((char*)(&len), sizeof(len)));
    ASSERT_EQ(1, len);
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get(h, sizeof(h) - 1));
    h[1] = '\0';
    ASSERT_STREQ(h, "a");

    len = f.length();
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put((char*)(&len), sizeof(len)));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Put(f.c_str(), f.length()));
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get((char*)(&len), sizeof(len)));
    ASSERT_EQ(1, len);
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get(h, sizeof(h) - 1));
    h[1] = '\0';
    ASSERT_STREQ(h, "b");
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get((char*)(&len), sizeof(len)));
    ASSERT_EQ(1, len);
    ASSERT_EQ(dstream::error::OK, m_ring_buffer->Get(h, sizeof(h) - 1));
    h[1] = '\0';
    ASSERT_STREQ(h, "a");
}

/**
 * @brief 
 * @begin_version 
**/
TEST_F(test_RingBuffer_Get_suite, Get)
{
    TestGet();
}
 
