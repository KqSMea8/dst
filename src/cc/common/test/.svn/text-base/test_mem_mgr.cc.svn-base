
#include "common/logger.h"
#include "common/mem_mgr.h"
#include <gtest/gtest.h>

using namespace dstream;
using namespace dstream::mem_mgr;

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_mem_mgr");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
/**
 * @brief
**/
class test_MemMgr_Instance_suite : public ::testing::Test {
protected:
    test_MemMgr_Instance_suite() {};
    virtual ~test_MemMgr_Instance_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_Instance_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_Instance_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_Instance_suite, TestInstance)
{
    ASSERT_TRUE(MemMgr::Instance() != NULL);
}

/**
 * @brief
**/
class test_MemMgr_MemMgr_suite : public ::testing::Test {
protected:
    test_MemMgr_MemMgr_suite() {};
    virtual ~test_MemMgr_MemMgr_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_MemMgr_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_MemMgr_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_MemMgr_suite, case_name1)
{
    //TODO
}

/**
 * @brief
**/
class test_MemMgr_Init_suite : public ::testing::Test {
protected:
    test_MemMgr_Init_suite() {};
    virtual ~test_MemMgr_Init_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_Init_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_Init_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_Init_suite, TestInit)
{
    MemMgr::MemMgrPtr inst = MemMgr::Instance();
    ASSERT_TRUE(inst != NULL);
    ASSERT_EQ(MemMgr::Instance()->Init(1000), error::OK);
}

/**
 * @brief
**/
class test_MemMgr_AllocateMem_suite : public ::testing::Test {
protected:
    test_MemMgr_AllocateMem_suite() {};
    virtual ~test_MemMgr_AllocateMem_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_AllocateMem_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_AllocateMem_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_AllocateMem_suite, TestAllocateMem)
{
    MemMgr::MemMgrPtr inst = MemMgr::Instance();
    ASSERT_TRUE(inst != NULL);
    ASSERT_EQ(MemMgr::Instance()->Init(1000), error::OK);
    ASSERT_EQ(MemMgr::Instance()->AllocateMem(1001), error::BAD_MEMORY_ALLOCATION);
    ASSERT_EQ(MemMgr::Instance()->AllocateMem(10), error::OK);
}

/**
 * @brief
**/
class test_MemMgr_ReleaseMem_suite : public ::testing::Test {
protected:
    test_MemMgr_ReleaseMem_suite() {};
    virtual ~test_MemMgr_ReleaseMem_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_ReleaseMem_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_ReleaseMem_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_ReleaseMem_suite, TestReleaseMem)
{
    MemMgr::MemMgrPtr inst = MemMgr::Instance();
    ASSERT_TRUE(inst != NULL);
    ASSERT_EQ(MemMgr::Instance()->Init(1000), error::OK);
    ASSERT_EQ(MemMgr::Instance()->AllocateMem(10), error::OK);
    ASSERT_EQ(MemMgr::Instance()->ReleaseMem(10), error::OK);
}

/**
 * @brief
**/
class test_MemMgr_GetTotalMem_suite : public ::testing::Test {
protected:
    test_MemMgr_GetTotalMem_suite() {};
    virtual ~test_MemMgr_GetTotalMem_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_GetTotalMem_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_GetTotalMem_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_GetTotalMem_suite, TestGetTotalMem)
{
    MemMgr::MemMgrPtr inst = MemMgr::Instance();
    ASSERT_TRUE(inst != NULL);
    ASSERT_EQ(MemMgr::Instance()->Init(1000), error::OK);

    ASSERT_EQ(MemMgr::Instance()->GetTotalMem(), 1000u);
}

/**
 * @brief
**/
class test_MemMgr_DumpMemStat_suite : public ::testing::Test {
protected:
    test_MemMgr_DumpMemStat_suite() {};
    virtual ~test_MemMgr_DumpMemStat_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_MemMgr_DumpMemStat_suite, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_MemMgr_DumpMemStat_suite, *)
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_MemMgr_DumpMemStat_suite, TestDumpMemStat)
{
    MemMgr::MemMgrPtr inst = MemMgr::Instance();
    ASSERT_TRUE(inst != NULL);
    ASSERT_EQ(MemMgr::Instance()->Init(1000), error::OK);

    DSTREAM_INFO("dump info: %s", MemMgr::Instance()->DumpMemStat().c_str());
}

