#include <boost/foreach.hpp>
#include <gtest/gtest.h>

#include "Kylin.h"
#include "ESP.h"
#include "BufStream.h"

#include "transporter_env.h"
#include "transporter/transporter_cli.h"
#include "transporter/transporter_tag.h"


using namespace dstream;
//using namespace dstream::transporter;

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new TransporterEnv());
    return RUN_ALL_TESTS();
}

class test_TPTagMap_suite : public ::testing::Test {
protected:
    test_TPTagMap_suite() {};
    virtual ~test_TPTagMap_suite() {};
    virtual void SetUp() {
        for (uint64_t i = 0; i < kNum; ++i) {
            ofs_.push_back((transporter::TPOutflow*)(kMask + i));//make TPOutflow pointers
            std::vector<std::string> tv;
            tv.push_back(kSubPrefix + NumberToString(i));
            tv.push_back(kSubPrefix + NumberToString(i + 1));
            tv.push_back(kFixSub);
            tags_.push_back(tv);
        }
    };
    virtual void TearDown() {
    };
public:
    static const uint64_t    kNum;
    static const uint64_t    kMask;
    static const uint64_t    kSubAll;
    static const std::string kSubPrefix;
    static const std::string kFixSub;
    static const std::string kNotExistSub;
public:
    std::vector<transporter::TPOutflow*>    ofs_;
    std::vector< std::vector<std::string> > tags_;
public:
    transporter::TPTagMap tag_map_;
};

const uint64_t  test_TPTagMap_suite::kNum    = 5;
const uint64_t  test_TPTagMap_suite::kSubAll = kNum + 1;
const uint64_t  test_TPTagMap_suite::kMask   = 0x01;
const std::string test_TPTagMap_suite::kSubPrefix   = "Sub";
const std::string test_TPTagMap_suite::kFixSub      = "FixSub";
const std::string test_TPTagMap_suite::kNotExistSub = "_NotExistSub";

TEST_F(test_TPTagMap_suite, SubTag) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub),  error::OK);
    }
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub),  error::OK_KEY_ALREADY_EXIST);
    }
    transporter::TPTagMap::SubVecPtr subs;
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);

    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Sub Tags[%s] : %s\n", kFixSub.c_str(), s.c_str());
}

TEST_F(test_TPTagMap_suite, SubTags) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], tags_[i]), error::OK);
    }
    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Sub Tags: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, EmptySubAllTags) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub), error::OK);
    }
    transporter::TPTagMap::SubVecPtr subs;
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    size_t old_size = subs->size();

    transporter::TPOutflow* tp = (transporter::TPOutflow*)(kNum + 1);
    ASSERT_EQ(tag_map_.SubTags(tp, ""), error::OK);

    //check
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), old_size + 1);

    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Sub All: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, SubAllTags) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub), error::OK);
    }
    transporter::TPTagMap::SubVecPtr subs;
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    size_t old_size = subs->size();

    ASSERT_EQ(tag_map_.SubTags((dstream::transporter::TPOutflow*)kSubAll,
                               config_const::kTPAllTag), error::OK);
    //check
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), old_size + 1);

    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Sub All: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, DynamicSubAllTags) {
    transporter::TPTagMap::SubVecPtr subs;

    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs == NULL);

    //sub all
    ASSERT_EQ(tag_map_.SubTags((dstream::transporter::TPOutflow*)kSubAll,
                               config_const::kTPAllTag), error::OK);
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), 1UL);//should has one tag

    //add tags
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub), error::OK);
    }

    //check
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum + 1);

    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Sub All: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, DelTag) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], tags_[i]), error::OK);
    }

    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.DelTags(ofs_[i], tags_[i][0]), error::OK);
        ASSERT_EQ(tag_map_.DelTags(ofs_[i], tags_[i][1]), error::OK);
    }

    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.DelTags(ofs_[i], tags_[i][0]), error::OK);
        ASSERT_EQ(tag_map_.DelTags(ofs_[i], tags_[i][1]), error::OK);
    }
}

TEST_F(test_TPTagMap_suite, DelTags) {
    transporter::TPTagMap::SubVecPtr subs;
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], tags_[i]), error::OK);
    }
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);

    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.DelTags(ofs_[i]), error::OK);
    }
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs == NULL);
    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Del Sub: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, DelSubAllTag) {
    //add tags
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], kFixSub), error::OK);
    }
    //sub all
    ASSERT_EQ(tag_map_.SubTags((dstream::transporter::TPOutflow*)kSubAll,
                               config_const::kTPAllTag), error::OK);

    transporter::TPTagMap::SubVecPtr subs;
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum + 1);
    std::string s;
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("Before del SubAll: %s\n", s.c_str());

    //del sub all
    ASSERT_EQ(tag_map_.DelTags((dstream::transporter::TPOutflow*)kSubAll,
                               config_const::kTPAllTag), error::OK);

    //check
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);

    s.clear();
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After del SubAll: %s\n", s.c_str());
}

TEST_F(test_TPTagMap_suite, UpdTags) {
    transporter::TPTagMap::SubVecPtr subs;
    std::string s;

    //first sub
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], tags_[i]), error::OK);
    }
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);

    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("Befor Upd Sub: %s\n", s.c_str());

    //change sub
    for (uint64_t i = 0; i < kNum; ++i) {
        tags_[i].clear();
        tags_[i].push_back(kSubPrefix + NumberToString(i + 10));
        ASSERT_EQ(tag_map_.UpdTags(ofs_[i], tags_[i]), error::OK);
    }
    subs = tag_map_.GetSubs(kFixSub);

    s.clear();
    ASSERT_EQ(tag_map_.Dump(&s), error::OK);
    printf("After Upd Sub: %s\n", s.c_str());

    ASSERT_TRUE(subs == NULL);
}

TEST_F(test_TPTagMap_suite, GetSubs) {
    for (uint64_t i = 0; i < kNum; ++i) {
        ASSERT_EQ(tag_map_.SubTags(ofs_[i], tags_[i]), error::OK);
    }

    transporter::TPTagMap::SubVecPtr subs;
    subs = tag_map_.GetSubs(kNotExistSub);
    ASSERT_TRUE(subs == NULL);
    subs = tag_map_.GetSubs(kFixSub);
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);

    subs = tag_map_.GetSubs("");//empty, all subs
    ASSERT_TRUE(subs != NULL);
    ASSERT_EQ(subs->size(), kNum);
}
