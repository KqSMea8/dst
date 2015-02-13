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
#include "common/blocking_queue.h"

using namespace dstream;
int main() {
    BlockingQueue<int> Q;
    Q.Put(1);
    Q.Put(2);
    assert(Q.size() == 2);
    assert(Q.Take() == 1);
    assert(Q.Take() == 2);
    return 0;
}
