/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  linked list queue.
 *
 */

#ifndef __DSTREAM_CC_COMMON_LINKED_LIST_QUEUE_H__
#define __DSTREAM_CC_COMMON_LINKED_LIST_QUEUE_H__

namespace dstream {

// T has field next whose type is T
template<typename T>
class LinkedListQueue {
public:
    LinkedListQueue():
        m_head(NULL), m_tail(NULL) {
    }
    void push(T item) {
        if (m_head == m_tail && m_head == NULL) {
            m_head = item;
            m_tail = item;
        } else {
            m_tail->next = item;
            m_tail = item;
        }
    }
    T pop() {
        T saved = m_head;
        if (m_head != NULL) {
            m_head = m_head->next;
            if (m_head == NULL) {
                m_tail = NULL;
            }
        }
        return saved;
    }
    T head() const {
        return m_head;
    }
    T tail() const {
        return m_tail;
    }

private:
    T m_head;
    T m_tail;
}; // class LinkedListQueue

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_LINKED_LIST_QUEUE_H__
