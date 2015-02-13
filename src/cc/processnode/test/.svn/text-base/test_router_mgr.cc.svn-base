
#include <gtest/gtest.h>

#include "processnode/router_mgr.h"

using namespace dstream;
using namespace dstream::router;

inline uint64_t ConstructPEID(uint64_t appid, uint64_t proid, uint64_t serial) {
    return MAKE_PEID(appid, proid, serial);
}

int main(int argc, char** argv)
{
    ::dstream::logger::initialize("test_router_mgr");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


/**
 * @brief
**/
class test_RouterMgr_Instance_suite : public ::testing::Test {
protected:
    test_RouterMgr_Instance_suite() {};
    virtual ~test_RouterMgr_Instance_suite() {};
    virtual void SetUp() {
    };
    virtual void TearDown() {
    };
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RouterMgr_Instance_suite, Instance)
{
    ASSERT_FALSE(RouterMgr::IsCreated());

    RouterMgr* ptr = NULL;
    ASSERT_TRUE(ptr = RouterMgr::Instance());

    ASSERT_TRUE(ptr == RouterMgr::Instance());

    ASSERT_TRUE(RouterMgr::IsCreated());

    delete RouterMgr::Instance();
}


/**
 * @brief
**/
class test_RouterMgr_AddPE_suite : public ::testing::Test {
protected:
    test_RouterMgr_AddPE_suite() {};
    virtual ~test_RouterMgr_AddPE_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RouterMgr_AddPE_suite, *)
        router_mgr_ = new (std::nothrow) RouterMgr();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RouterMgr_AddPE_suite, *)
        delete router_mgr_;
    };
private:
    RouterMgr* router_mgr_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RouterMgr_AddPE_suite, AddPEOK)
{
    ASSERT_NE(router_mgr_, reinterpret_cast<RouterMgr*>(NULL));

    router_mgr_->SetConfigFile("test_router_mgr.conf.xml");

    uint64_t peid1 = ConstructPEID(1, 2, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid1), error::OK);
    // add pe exist
    ASSERT_EQ(router_mgr_->AddPE(peid1), error::OK);
    uint64_t peid2 = ConstructPEID(2, 2, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid2), error::OK);

    RouterMgr::AppRouterMap::const_iterator c_iter;
    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 2u);
    c_iter = router_mgr_->m_app_router_map.find(1);
    ASSERT_TRUE(router_mgr_->m_app_router_map.end() != c_iter);
    ASSERT_EQ(c_iter->second->LocalPENum(), 1u);
    c_iter = router_mgr_->m_app_router_map.find(2);
    ASSERT_TRUE(router_mgr_->m_app_router_map.end() != c_iter);
    ASSERT_EQ(c_iter->second->LocalPENum(), 1u);

    uint64_t peid3 = ConstructPEID(1, 1, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid3), error::OK);
    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 2u);
    c_iter = router_mgr_->m_app_router_map.find(1);
    ASSERT_TRUE(router_mgr_->m_app_router_map.end() != c_iter);
    ASSERT_EQ(c_iter->second->LocalPENum(), 2u);
}


TEST_F(test_RouterMgr_AddPE_suite, ConfigFail)
{
    ASSERT_NE(router_mgr_, reinterpret_cast<RouterMgr*>(NULL));
    router_mgr_->SetConfigFile("invalid.conf.xml");

    uint64_t peid1 = ConstructPEID(1, 2, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid1), error::CONFIG_BAD_FILE);

}

/**
 * @brief
**/
class test_RouterMgr_DelPE_suite : public ::testing::Test {
protected:
    test_RouterMgr_DelPE_suite() {};
    virtual ~test_RouterMgr_DelPE_suite() {};
    virtual void SetUp() {
        //Called befor every TEST_F(test_RouterMgr_DelPE_suite, *)
        router_mgr_ = new (std::nothrow) RouterMgr();
    };
    virtual void TearDown() {
        //Called after every TEST_F(test_RouterMgr_DelPE_suite, *)
        delete router_mgr_;
    };
private:
    RouterMgr* router_mgr_;
};

/**
 * @brief
 * @begin_version
**/
TEST_F(test_RouterMgr_DelPE_suite, DelPEOK)
{
    ASSERT_NE(router_mgr_, reinterpret_cast<RouterMgr*>(NULL));

    router_mgr_->SetConfigFile("test_router_mgr.conf.xml");

    uint64_t peid1 = ConstructPEID(1, 2, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid1), error::OK);
    uint64_t peid2 = ConstructPEID(1, 3, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid2), error::OK);
    uint64_t peid3 = ConstructPEID(2, 2, 3);
    ASSERT_EQ(router_mgr_->AddPE(peid3), error::OK);

    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 2u);
    // test del pe
    RouterMgr::AppRouterMap::iterator c_iter;
    ASSERT_EQ(router_mgr_->DelPE(peid3), error::OK);
    c_iter = router_mgr_->m_app_router_map.find(1);
    ASSERT_TRUE(router_mgr_->m_app_router_map.end() != c_iter);
    ASSERT_EQ(c_iter->second->LocalPENum(), 2u);
    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 1u);

    ASSERT_EQ(router_mgr_->DelPE(peid2), error::OK);
    c_iter = router_mgr_->m_app_router_map.find(1);
    ASSERT_TRUE(router_mgr_->m_app_router_map.end() != c_iter);
    ASSERT_EQ(c_iter->second->LocalPENum(), 1u);
    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 1u);

    ASSERT_EQ(router_mgr_->DelPE(peid1), error::OK);
    ASSERT_EQ(router_mgr_->m_app_router_map.size(), 0u);
    // delete pe not exist
    ASSERT_EQ(router_mgr_->DelPE(peid1), error::OK);
}

