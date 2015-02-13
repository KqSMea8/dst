/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <cstdio>
#include <cassert>
#include "common/linked_list_queue.h"

using namespace dstream;

struct Entry {
    int value;
    Entry* next;
    Entry(int _value):
        value(_value), next(NULL) {
    }
};

int main() {
    LinkedListQueue<Entry*> Q;
    Q.push(new Entry(10));
    Q.push(new Entry(20));
    Entry* head = Q.head();
    Entry* tail = Q.tail();
    assert(head->next == tail);
    assert(head->value == 10);
    assert(tail->value == 20);
    delete Q.pop();
    delete Q.pop();
    assert(Q.head() == NULL);
    assert(Q.tail() == NULL);
    return 0;
}
