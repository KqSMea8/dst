
#ifndef __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_STORAGE_H__
#define __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_STORAGE_H__

#include <map>
#include <string>
#include "storage/storage_accessor.h"
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

class MockStorageHashTable : public IHashTable {
public:
    MockStorageHashTable () {
        type_ = HASH_TABLE;
    }

    virtual int Find(const std::string& key, std::string* value) {
        MockMap::const_iterator itr = map_.find(key);
        if (itr != map_.end()) {
            *value = itr->second;
            return 0;
        } else {
            return -1;
        }
    }

    virtual int Insert(const std::string& key, const std::string& value) {
        return Insert(key, value, false);
    }

    virtual int Insert(const std::string& key, const std::string& value, bool update_if_exists) {
        if (update_if_exists) {
            map_[key] = value;
            return 0;
        } else {
            MockMap::const_iterator itr = map_.find(key);
            if (itr != map_.end()) {
                return 1;
            } else {
                map_[key] = value;
                return 0;
            }
        }
    }

    virtual int Insert(const std::vector<std::string>& keys,
                       const std::vector<std::string>& values,
                       std::vector<int>* ret_vals,
                       bool update_if_exists) {
        for (size_t i = 0; i < keys.size(); ++i) {
            Insert(keys[i], values[i], update_if_exists);
            ret_vals->push_back(0);
        }
        return 0;
    }

    virtual int Erase(const std::string& key) {
        map_.erase(key);
        return 0;
    }

    virtual int Erase(const std::vector<std::string>& keys,
                      std::vector<int>* ret_vals) {
        for (size_t i = 0; i < keys.size(); ++i) {
            Erase(keys[i]);
            ret_vals->push_back(0);
        }
        return 0;
    }

    virtual int Clear() {
        map_.clear();
        return 0;
    }

    virtual int GetAll(HashTraverseCallBack cb, void* ctx) {
        for (MockMap::const_iterator itr = map_.begin();
             itr != map_.end();
             ++itr) {
            cb(ctx, 0, itr->first, itr->second);
        }
        return 0;
    }

    virtual int Count(uint64_t* n) {
        *n = map_.size();
        return 0;
    }

protected:
    typedef std::map<std::string, std::string> MockMap;
    MockMap map_;
};

class MockStorageListTable : public IListTable {
public:
    MockStorageListTable () {
        type_ = LIST_TABLE;
    }

    virtual int GetFront(std::string* s) {
        if (list_.size() > 0) {
            *s = list_[0];
            return 0;
        } else {
            return -1;
        }
    }

    virtual int GetBack(std::string* s) {
        if (list_.size() > 0) {
            *s = list_[list_.size() - 1];
            return 0;
        } else {
            return -1;
        }
    }

    virtual int PushFront(const std::string& value) {
        list_.insert(list_.begin(), value);
        return 0;
    }

    virtual int PushBack(const std::string& value) {
        list_.push_back(value);
        return 0;
    }

    virtual int PushFront(const std::vector<std::string>& values,
                          std::vector<int>* ret_vals) {
        for (size_t i = 0; i < values.size(); ++i) {
            list_.insert(list_.begin(), values[i]);
            ret_vals->push_back(0);
        }
        return 0;
    }

    virtual int PushBack(const std::vector<std::string>& values,
                         std::vector<int>* ret_vals) {
        for (size_t i = 0; i < values.size(); ++i) {
            list_.push_back(values[i]);
            ret_vals->push_back(0);
        }
        return 0;
    }

    virtual int PopFront(std::string* value) {
        if (list_.size() > 0) {
            *value = list_[0];
            list_.erase(list_.begin());
            return 0;
        } else {
            return -1;
        }
    }

    virtual int PopBack (std::string* value) {
        if (list_.size() > 0) {
            *value = list_[list_.size() - 1];
            list_.pop_back();
            return 0;
        } else {
            return -1;
        }
    }

    virtual int PopFront(int /*n*/,
                         std::vector<std::string>* /*values*/,
                         std::vector<int>* /*ret_vals*/) {
        return -1;
    }

    virtual int PopBack (int /*n*/,
                         std::vector<std::string>* /*values*/,
                         std::vector<int>* /*ret_vals*/) {
        return -1;
    }

    virtual int Clear() {
        list_.clear();
        return 0;
    }

    virtual int Count(uint64_t* n) {
        *n = list_.size();
        return 0;
    }

    virtual int GetAll(ListTraverseCallBack cb, void* ctx) {
        for (size_t i = 0; i < list_.size(); ++i) {
            cb(ctx, 0, list_[i]);
        }
        return 0;
    }

protected:
    typedef std::vector<std::string> MockList;
    MockList list_;
};

class MockStoreAccessor : public StoreAccessor {
public:
    MockStoreAccessor() {
    }

    virtual ~MockStoreAccessor() {
        for (TableRefMap::const_iterator itr = table_ref_map_.begin();
             itr != table_ref_map_.end();
             ++itr) {
            assert(0 == itr->second);
        }
    }

    /* virtual int CreateTable(const std::string& table_name,  */
    /*                         StoreTableType table_type,  */
    /*                         IStoreTable** stpp) { */

    virtual int CreateTable(const std::string& table_name,
                            StoreTableType table_type,
                            bool replace_old,
                            IStoreTablePtr* pt) {
        if (table_map_.find(table_name) != table_map_.end() && !replace_old) {
            return TABLE_ALREADY_EXISTS;
        }

        if (LIST_TABLE == table_type) {
            pt->reset(new MockStorageListTable());
        } else if (HASH_TABLE == table_type) {
            pt->reset(new MockStorageHashTable());
        } else {
            assert(false);
        }
        (*pt)->set_name(table_name);

        table_map_[table_name] = *pt;

        return storage::OK;
    }

    virtual int DeleteTable(const std::string& table_name) {
        if (table_map_.find(table_name) != table_map_.end()) {
            table_map_.erase(table_name);
        }
        return storage::OK;
    }

    virtual int GetTable(const std::string& table_name,
                         IStoreTablePtr* pt) {
        if (table_map_.find(table_name) != table_map_.end()) {
            *pt = table_map_[table_name];
        } else {
            pt->reset();
        }
        return storage::OK;
    }

    int ReleaseTable(IStoreTable* stp) {
        //  virtual int ReleaseTable(IStoreTable* stp) {
        assert(table_map_.find(stp->name()) != table_map_.end());
        assert(table_ref_map_.find(stp->name()) != table_ref_map_.end());
        --table_ref_map_[stp->name()];
        return storage::OK;
    }

    virtual int TestTableExist(const std::string& table_name, bool* exists) {
        if (table_map_.find(table_name) != table_map_.end()) {
            *exists = true;
        } else {
            *exists = false;
        }
        return storage::OK;
    }

protected:
    typedef std::map<std::string, IStoreTablePtr> TableMap;
    typedef std::map<std::string, int> TableRefMap;

    TableMap table_map_;
    TableRefMap table_ref_map_;
};

} // namespace storage
} // namespace dstream

#endif // __DSTREAM_GENERIC_OPERATOR_TEST_MOCK_STORAGE_H__
