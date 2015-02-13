/******************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
*******************************************************************/
/********************************************************************
  author:    lanbijia@baidu.com

  purpose:  time cached map
*********************************************************************/

#ifndef __DSTREAM_CC_COMMON_TIMECACHE_MAP_H__ // NOLINT
#define __DSTREAM_CC_COMMON_TIMECACHE_MAP_H__ // NOLINT

#include <functional>
#include <list>
#include <map>
#include "common/rwlock.h"
#include "common/thread.h"

namespace dstream {

template<typename K, typename V, typename Cmp = std::less<K> >
class TimeCacheMap : public thread::Thread {
public:
    TimeCacheMap(int expire_secs, int buckets_num, void (*callback)(const V&))
        : m_expire_secs(expire_secs), m_buckets_num(buckets_num), m_callback(callback) {
        assert(m_buckets_num >= 2);
        // init buckets
        for (int i = 0; i < m_buckets_num; i++) {
            typename std::map<K, V, Cmp> m;
            m_buckets.push_back(m);
        }

        Start();
    };
    virtual ~TimeCacheMap() {
        Stop();
    }

    bool Contain_key(const K& key) {
        typename std::list< std::map<K, V, Cmp> >::iterator iter;
        typename std::map<K, V, Cmp>::iterator map_iter;
        RWLockGuard lock_guard(m_data_lock, false);
        for (iter = m_buckets.begin(); iter != m_buckets.end(); iter++) {
            if (iter->find(key) != iter->end()) {
                return true;
            }
        }
        return false;
    };

    void Get(K key, V* v_ptr) {
        typename std::list< std::map<K, V, Cmp> >::iterator iter;
        typename std::map<K, V, Cmp>::iterator map_iter;
        RWLockGuard lock_guard(m_data_lock, false);
        for (iter = m_buckets.begin(); iter != m_buckets.end(); iter++) {
            if ((map_iter = iter->find(key)) != iter->end()) {
                *v_ptr = map_iter->second;
            }
        }
        // v_ptr = NULL;
    };

    void Put(K key, const V& value) {
        typename std::list< std::map<K, V, Cmp> >::iterator iter;
        typename std::map<K, V, Cmp>::iterator map_iter;
        RWLockGuard lock_guard(m_data_lock, true);
        iter = m_buckets.begin();
        (*iter)[key] = value;
        iter++;
        while (iter != m_buckets.end()) {
            if ((map_iter = iter->find(key)) != iter->end()) {
                iter->erase(map_iter);
            }
            iter++;
        }
    };

    void Remove(K key, V* v_ptr) {
        typename std::list< std::map<K, V, Cmp> >::iterator iter;
        typename std::map<K, V, Cmp>::iterator map_iter;
        RWLockGuard lock_guard(m_data_lock, true);
        for (iter = m_buckets.begin(); iter != m_buckets.end(); iter++) {
            if ((map_iter = iter->find(key)) != iter->end()) {
                *v_ptr = map_iter->second;
                iter->erase(map_iter);
            }
        }
        // v_ptr = NULL;
    }

    size_t Size() {
        typename std::list< std::map<K, V, Cmp> >::iterator iter;
        size_t size = 0;
        RWLockGuard lock_guard(m_data_lock, false);
        for (iter = m_buckets.begin(); iter != m_buckets.end(); iter++) {
            size += iter->size();
        }
        return size;
    }

    void Run() {
        uint64_t sleep_time = m_expire_secs * 1000000 / (m_buckets_num - 1);

        while (!stop_) {
            usleep(sleep_time);

            typename std::map<K, V, Cmp> m;
            typename std::map<K, V, Cmp> old_m;
            typename std::map<K, V, Cmp>::iterator map_iter;
            {
                RWLockGuard lock_guard(m_data_lock, true);
                // remove last bucket
                m_buckets.push_front(m);
                old_m = m_buckets.back();
                m_buckets.pop_back();

                // exec callback
                if (!m_callback) {
                    continue;
                }
                for (map_iter = old_m.begin();
                     map_iter != old_m.end();
                     map_iter++) {
                    m_callback(map_iter->second);
                }
            }
        }
    }

private:
    RWLock m_data_lock;
    typename std::list< std::map<K, V, Cmp> > m_buckets;

    uint64_t m_expire_secs;
    uint64_t m_buckets_num;
    void (*m_callback)(const V&);
};

} // namespace dstream

#endif // __DSTREAM_CC_COMMON_TIMECACHE_MAP_H__ NOLINT

