/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author lanqiuju(@baidu.com)
 * @brief test statustable's function:
 * 1 set, get,del when multi thread operates the table
 * 2 serialize & unserialize function
 *
 */

#include <iostream>
#include "common/byte_array.h"
#include "processelement/status_table.h"
#include "gtest/gtest.h"

using namespace std;
using namespace dstream;
using namespace dstream::processelement;


static StatusTable<int, int> table("global", 10);

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

class Statustable_suite : public ::testing::Test {

protected:
    Statustable_suite() {};
    virtual ~Statustable_suite() {};
    virtual void SetUp()
    {

    };
    virtual void TearDown()
    {
    };
};
void* pe_thread(void* arg)
{
    int id = (int)(long)arg, value;
    for (int i = 0; i < 100; i++) {
        assert(table.set(i + id * 10000, i + id * 10000));
        assert(table.get(i + id * 10000, &value));

        assert(value == i + id * 10000);
    }
    for (int i = 0; i < 90; i++) {
        table.del(i + id * 10000);
        //index has been deleted ,then get return false
        assert(table.get(i + id * 10000, &value) == 0);

    }
    return NULL;
}

void test_multithreads() {
    table.clear();
    pthread_t tid[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(tid + i, NULL, pe_thread, (void*)(long)i);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }

}

TEST_F(Statustable_suite, test_multi_get_set_del)
{


    test_multithreads();

}

TEST_F(Statustable_suite, test_set_get_del_overrite)
{
    //table's name=click,hashsize =100
    StatusTable<int, int> table1("click", 100);
    int value;
    //table = table1;
    table1.set(1, 1);
    table1.set(2, 2);
    ASSERT_TRUE(table1.get(1, &value));
    ASSERT_EQ(value, 1);
    ASSERT_TRUE(table1.get(2, &value));
    ASSERT_EQ(value, 2);

    //overwrite=false, set failed
    ASSERT_FALSE(table1.set(1, 3));
    ASSERT_TRUE(table1.get(1, &value));
    ASSERT_EQ(value, 1);
    //overwrite=true, set success
    ASSERT_TRUE(table1.set(1, 3, true));
    ASSERT_TRUE(table1.get(1, &value));
    ASSERT_EQ(value, 3);
    //
    //
    table.del(1);
    ASSERT_FALSE(table.get(1, &value));
    StatusTable<int, int>::iterator it(table1);
    ASSERT_TRUE(table.set(1, 1));
    ASSERT_TRUE(table.get(1, &value));
    ASSERT_EQ(1, value);

    table1.writeDescription(std::cout);
}

TEST_F(Statustable_suite, test_serialize_deserialize)
{

    StatusTable<int, string> table1("click", 100);
    table1.set(1, "echoechoechoechoecho");
    table1.set(2, "hellohellohellohellohello");
    table1.set(3, "worldworldworldworldworld");

    table1.del(3);
    StatusTable<int, string>::iterator it(table1);
    int k;
    string v = "";
    bool deleted;

    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(1, k);
    ASSERT_EQ("echoechoechoechoecho", v);
    ASSERT_EQ(false, deleted);
    // table1.get(2, &v)
    v.clear();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(2, k);
    ASSERT_EQ("hellohellohellohellohello", v);
    ASSERT_EQ(false, deleted);
    v.clear();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(3, k);
    ASSERT_EQ("worldworldworldworldworld", v);
    ASSERT_EQ(true, deleted);
    v.clear();
    ASSERT_FALSE(it.next(&k, &v, &deleted));
    table1.writeDescription(std::cout);

    WriteableByteArray bytes;
    //  write rag
    table1.serializeRag(&bytes);

    ByteSize size;
    const Byte* data = bytes.data(&size);
    ReadableByteArray bytes2(data, size);
    table1.clear();
    table1.deserializeRag(&bytes2);
    it.rewind();
    //table1.get(1, &v)
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(1, k);
    ASSERT_EQ("echoechoechoechoecho", v);
    ASSERT_EQ(false, deleted);
    // table1.get(2, &v)
    v.clear();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(2, k);
    ASSERT_EQ("hellohellohellohellohello", v);
    ASSERT_EQ(false, deleted);
    v.clear();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(3, k);
    ASSERT_EQ("worldworldworldworldworld", v);
    ASSERT_EQ(true, deleted);
    v.clear();
    ASSERT_FALSE(it.next(&k, &v, &deleted));
    table1.writeDescription(std::cout);
    exit(0);
    //trunk
    table1.mergeRagToPatch();
    table1.mergePatchToTrunk();
    WriteableByteArray bytes3;
    table1.serialize(&bytes3);
    data = bytes3.data(&size);
    ReadableByteArray bytes4(data, size);
    table1.clear();
    table1.deserialize(&bytes4);
    it.rewind();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(1, k);
    ASSERT_EQ("echoechoechoechoecho", v);
    ASSERT_EQ(false, deleted);
    // table1.get(2, &v)
    v.clear();
    ASSERT_TRUE(it.next(&k, &v, &deleted));
    ASSERT_EQ(2, k);
    ASSERT_EQ("hellohellohellohellohello", v);
    ASSERT_EQ(false, deleted);
    v.clear();
    ASSERT_FALSE(it.next(&k, &v, &deleted));
    table1.writeDescription(std::cout);
}
