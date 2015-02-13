/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/08
* @filename:    pm_auto_ptr.h
* @author:      zhanggengxin
* @brief:       auto ptr manager,this file used only in processmaster
                other module shoud not use
*/

#ifndef __DSTREAM_CC_COMMON_AUTO_PTR_H__
#define __DSTREAM_CC_COMMON_AUTO_PTR_H__

#include <cstddef>
#include "common/logger.h"
#include "common/utils.h"

namespace dstream {
namespace auto_ptr {

/************************************************************************/
/* release arrary pointer                                               */
/************************************************************************/
#define RELEASEARRAY(array_ptr)     \
    if (NULL != array_ptr) {        \
        delete []array_ptr;         \
        array_ptr = NULL;           \
    }

/************************************************************************/
/* release object pointer                                               */
/************************************************************************/
#define RELEASEPTR(ptr)             \
    if (NULL != ptr) {              \
        delete ptr;                 \
        ptr = NULL;                 \
    }

/************************************************************************/
/*AutoPtr : manage object pointer, cannot used for array,used only in pm*/
/************************************************************************/
template <class T>
class AutoPtr {
public:
    AutoPtr() : m_obj(NULL), m_ref(NULL) {}
    AutoPtr(T* t);
    AutoPtr(const AutoPtr<T>& ptr);
    ~AutoPtr() {
        Release();
    }
    bool PtrValid() {
        return NULL != m_obj;
    }
    operator T* () {
        return m_obj;
    }
    operator T* ()const {
        return m_obj;
    }
    T* operator ->() {
        return m_obj;
    }
    T* operator->() const {
        return m_obj;
    }
    const AutoPtr<T>& operator = (const AutoPtr<T>& ptr);
    void CopyPtr(const AutoPtr<T>& ptr);
    // used for test
    const int64_t ref() const {
        return NULL == m_ref ? 0 : AtomicGetValue(*m_ref);
    }

    const T* const ObjAddr() const {
        return m_obj;
    }

protected:
    T*       m_obj;
    int64_t* m_ref;

private:
    void Release();
};

template <class T>
AutoPtr<T>::AutoPtr(T* t) : m_obj(NULL), m_ref(NULL) {
    if (NULL != t) {
        m_ref = new int64_t(1);
        m_obj = t;
    }
}

template <class T>
AutoPtr<T>::AutoPtr(const AutoPtr<T>& ptr) {
    CopyPtr(ptr);
}

template <class T>
void AutoPtr<T>::CopyPtr(const AutoPtr<T>& ptr) {
    m_obj = ptr.m_obj;
    m_ref = ptr.m_ref;
    if (NULL != m_ref) {
        AtomicInc64(*m_ref);
    }
}

template <class T>
void AutoPtr<T>::Release() {
    if (NULL != m_ref) {
        if (AtomicDec64(*m_ref) == 0 && NULL != m_obj) {
            RELEASEPTR(m_obj);
            RELEASEPTR(m_ref);
        }
    }
}

template <class T>
const AutoPtr<T>& AutoPtr<T>::operator = (const AutoPtr<T>& ptr) {
    Release();
    CopyPtr(ptr);
    return *this;
}

} // namespace auto_ptr
} // namespace dstream

#endif // __DSTREAM_CC_COMMON_AUTO_PTR_H__
