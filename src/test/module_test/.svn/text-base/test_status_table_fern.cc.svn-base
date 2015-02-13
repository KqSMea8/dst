/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author liufeng07(@baidu.com)
 * @brief
 *
 */

#include <iostream>
#include "common/byte_array.h"
#include "processelement/status_table.h"

using namespace dstream;
using namespace dstream::processelement;
using namespace std;

static StatusTable<int, int>* table;
StatusTable<int, int> table1("click", 100);
StatusTable<std::string, std::string > table2("click", 100);

/***************
** help function
*****************/
void check_kv(int k, int v) {
    int v_;
    if (table1.get(k, &v_)) {
        printf("==> [k:%d, v:%d, %s]\n", k, v_, (v_ == v) ? "" : "[ERROR]");
    }
    else {
        printf("==> [k:%d] not exist [ERROR]\n", k);
    }
}

void check_kv(std::string k, std::string v) {
    std::string v_;
    if (table2.get(k, &v_)) {
        printf("==> [k:%s, v:%s, %s]\n", k.c_str(), v_.c_str(), v_ == v ? "" : "[ERROR]");
    }
    else {
        printf("==> [k:%s] not exist [ERROR]\n", k.c_str());
    }
}

void check_not_exist(int k) {
    int v_;
    printf("==> [k:%d] %s\n", k, (table1.get(k, &v_) == false) ? "not exist" : "exist [ERROR]");
}

void check_not_exist(std::string k) {
    std::string v_;
    printf("==> [k:%s] %s\n", k.c_str(), (table2.get(k, &v_) == false) ? "not exist" : "exist [ERROR]");
}

/***************
** tests
*****************/

void* pe_thread(void* arg) {
    int id = (int)(long)arg;
    for (int i = 0; i < 100000; i++) {
        assert(table1.set(i + id * 1000000, i + id * 1000000));
        usleep(10);
    }
    for (int i = 0; i < 100000; i++) {
        table1.del(i + id * 1000000);
        usleep(10);
    }
    return NULL;
}

void* PN_thread(void* /*arg*/) {
    for (int i = 0; i < 10000; i++) {
        table1.mergeRagToPatch();
        table1.mergePatchToTrunk();
        usleep(100);
    }
    return NULL;
}

void test_multithreads() {
    table->clear();
    pthread_t tid[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(tid + i, NULL, pe_thread, (void*)(long)i);
    }
    for (int i = 0; i < 10; i++) {
        pthread_join(tid[i], NULL);
    }
}

int main() {
    // test1 int,int table
    printf("------------------------------test 1------------------------------\n");
    table1.set(1, 1);
    table1.mergeRagToPatch();
    table1.set(1, 2, true);
    table1.del(1);
    table1.mergeRagToPatch();
    table1.set(1, 3, true);
    table1.del(1);
    table1.mergeRagToPatch();
    table1.writeDescription(cout);
    table1.set(2, 2);
    table1.del(1);
    table1.del(3);
    table1.set(1, 2);
    table1.set(1, 3);
    StatusTable<int, int>::iterator it(table1);
    int k, v;
    bool deleted;
    while (it.next(&k, &v, &deleted)) {
        printf("[%d,%d][%s]\n", k, v, deleted ? "deleted" : "exist");
    }
    check_kv(1, 2);
    check_not_exist(3);
    table1.set(1, 3, true);
    check_kv(1, 3);
    table1.mergeRagToPatch();
    check_kv(1, 3);
    table1.mergePatchToTrunk();
    check_kv(1, 3);
    table1.del(1);
    check_not_exist(1);
    table1.set(1, 4, true);
    check_kv(1, 4);
    table1.mergeRagToPatch();
    table1.set(1, 5, true);
    table1.del(1);
    check_not_exist(1);
    table1.set(1, 6);
    check_kv(1, 6);
    printf("========status========\n");
    table1.writeDescription(cout);
    table1.clear();
    printf("========status========\n");
    table1.writeDescription(cout);

    // test merge operation
    printf("------------------------------test 2------------------------------\n");
    for (int i = 1; i <= 3; i++) {
        table1.set(i, i);
    }
    table1.mergeRagToPatch();
    StatusTable<int, int>::iterator it2(table1);
    for (int i = 1; i <= 3; i++) {
        table1.set(i, 0 - i, true);
    }
    it2.rewind();
    printf("======> traversal\n");
    while (it2.next(&k, &v, &deleted)) {
        printf("[%d,%d][%s]\n", k, v, deleted ? "deleted" : "exist");
    }
    table1.mergePatchToTrunk();
    table1.mergeRagToPatch();
    it2.rewind();
    printf("======> traversal\n");
    while (it2.next(&k, &v, &deleted)) {
        printf("[%d,%d][%s]\n", k, v, deleted ? "deleted" : "exist");
    }
    table1.mergePatchToTrunk();
    it2.rewind();
    printf("======> traversal\n");
    while (it2.next(&k, &v, &deleted)) {
        printf("[%d,%d][%s]\n", k, v, deleted ? "deleted" : "exist");
    }

    // test string,string table
    printf("------------------------------test 3------------------------------\n");
    table2.set("1", "1");
    table2.set("2", "2");
    table2.del("1");
    table2.del("3");
    table2.set("1", "2");
    table2.set("1", "3");
    StatusTable<std::string, std::string>::iterator itstr(table2);
    std::string kstr, vstr;
    while (itstr.next(&kstr, &vstr, &deleted)) {
        printf("[%s,%s][%s]\n", kstr.c_str(), vstr.c_str(), deleted ? "deleted" : "exist");
    }
    check_kv("1", "2");
    check_not_exist("3");
    table2.set("1", "3", true);
    check_kv("1", "3");
    table2.mergeRagToPatch();
    table2.mergePatchToTrunk();
    table2.del("1");
    check_not_exist("1");

    // test serialize + deserialize
    printf("------------------------------test 4------------------------------\n");
    WriteableByteArray bytes;
    table1.serializeRag(&bytes);
    ByteSize size;
    const Byte* data = bytes.data(&size);
    ReadableByteArray bytes2(data, size);
    table1.clear();
    table1.deserializeRag(&bytes2);
    it.rewind();
    while (it.next(&k, &v, &deleted)) {
        printf("[%d,%d][%s]\n", k, v, deleted ? "deleted" : "exist");
    }

    table1.mergeRagToPatch();
    table1.mergePatchToTrunk();
    WriteableByteArray bytes3;
    table1.serialize(&bytes3);
    data = bytes3.data(&size);
    ReadableByteArray bytes4(data, size);
    table1.clear();
    table1.deserialize(&bytes4);
    it.rewind();
    while (it.next(&k, &v, &deleted)) {
        printf("[%d,%d][%c]\n", k, v, deleted ? 'X' : '!');
    }
    table1.writeDescription(std::cout);

    // test full size
    printf("------------------------------test 5------------------------------\n");
    for (int i = 0; i < 100000; i++) {
        assert(table1.set(i, i, true));
    }
    table1.mergeRagToPatch();
    //table1.mergeRagToTrunk();
    for (int i = 50000; i < 150000; i++) {
        assert(table1.set(i, i, true));
    }
    //table1.mergeRagToTrunk();
    table1.mergePatchToTrunk();
    table1.mergeRagToTrunk();
    for (int i = 0; i < 150000; i++) {
        int v_;
        if (table1.get(i, &v_)) {
            if (v_ != i) {
                printf("==> [k:%d, v:%d, %s]\n", i, v_, "[ERROR]");
            }
        } else {
            printf("==> [k:%d] not exist [ERROR]\n", i);
        }
    }

    // test multithreads
    printf("------------------------------test 6------------------------------\n");
    //test_multithreads(); // it is not safe to merge and set without lock
    return 0;
}
