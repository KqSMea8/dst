/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  naive sample of blocking queue, check template works or not.
 *
 */

#include <cassert>
#include "common/bounded_blocking_queue.h"

using namespace dstream;
class A {
public:
    A(int ax) {
        a = ax;
    }
    int a;
};
int main() {
    A* a1 = new A(1);
    A* a2 = new A(2);
    // int x = a1->a;
    BoundedBlockingQueue<A*> Q(32);
    Q.Put(a1);
    Q.Put(a2);
    assert(Q.size() == 2);
    assert(((Q.Take())->a) == 1);
    assert(((Q.Take())->a) == 2);
    return 0;
}
