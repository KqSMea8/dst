/**
* @file     storage_table.h
* @brief    storage table
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_STORAGE_TABLE_H__ // NOLINT
#define __DSTREAM_STORAGE_STORAGE_TABLE_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include "storage/error.h"

namespace dstream {
namespace storage {

/**
* @brief  Store Table Type
*/
typedef enum {
    STORE_TABLE_TYPE_BEING,
    INVALID_TABLE_TYPE = STORE_TABLE_TYPE_BEING,
    LIST_TABLE,
    HASH_TABLE,
    REDIS_LIST_TABLE = LIST_TABLE,/*compatible old interface*/
    REDIS_HASH_TABLE = HASH_TABLE,
    STORE_TABLE_TYPE_END
} StoreTableType;
class IStoreTable;
typedef boost::shared_ptr<IStoreTable> IStoreTablePtr;

inline bool IsInValidStoreTableType(StoreTableType t) {
    return (t > STORE_TABLE_TYPE_BEING) && (t < STORE_TABLE_TYPE_END);
}

/**
* @brief    StoreTable接口类，所有StoreTable均继承此类
* @author   konghui, konghui@baidu.com
* @date     2012-10-24
*/
class IStoreTable {
public:
    IStoreTable() {}
    IStoreTable(const std::string& n, const StoreTableType& t):
        name_(n), type_(t) {}
    virtual ~IStoreTable() {}
    virtual int Count(uint64_t* n) = 0;
    const std::string& name() {
        return name_;
    }
    void set_name(const std::string& n) {
        name_ = n;
    }
    StoreTableType type() {
        return type_;
    }
protected:
    std::string     name_;
    StoreTableType  type_;
};

/**
* @brief    List Table基类，value为raw格式
* @author   konghui, konghui@baidu.com
* @date     2012-10-24
*/
class IListTable : public IStoreTable {
public:
    IListTable() {}
    IListTable(const std::string& n, const StoreTableType& t):
        IStoreTable(n, t) {}
    virtual ~IListTable() {}

    /**
     * @brief    GetAll中的数据回调
     * @param    ctx[in]   用户上下文
     * @param    err[in]   错误码
     * @param    data[in]   data
     * @return   storage::OK(0): 成功;
     *           <0: ErrCode;
     * @author   konghui, konghui@baidu.com
     * @date     2012-11-06
     */
    typedef int(*ListTraverseCallBack)(void* ctx, int err,              // NOLINT 
                                       const std::string& data);
    /**
    * @brief    获取表内所有数据
    * @param    cb[in]  用户传入的用于处理每条数据的回调
    * @return   >0: 成功，返回value的长度;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int GetAll(ListTraverseCallBack cb, void* ctx) = 0;
    /**
    * @brief    获取队列head/tail的iterm
    * @param    s[out]      获取value的buf
    * @return   >0: 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int GetFront(std::string* s) = 0;
    virtual int GetBack(std::string* s) = 0;
    /**
    * @brief    Push iterm到队列head/tail
    * @param    value[in]  value缓冲区
    * @param    len[in]    指示用户传入缓冲区的长度
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int PushFront(const std::string& value) = 0;
    virtual int PushBack(const std::string& value) = 0;
    /**
    * @brief    批量push
    * @param    values[in]  value
    * @param    ret_vals[out] 返回值，0: 成功;
    *                         <0: ErrCode;
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int PushFront(const std::vector<std::string>& values,
                            std::vector<int>* ret_vals) = 0;
    virtual int PushBack(const std::vector<std::string>& values,
                            std::vector<int>* ret_vals) = 0;
    /**
    * @brief    Pop队列head/tail的iterm
    * @param    value[out]  获取value的buf
    * @return   >=0: 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int PopFront(std::string* value) = 0;
    virtual int PopBack(std::string* value) = 0;
    /**
    * @brief    批量pop
    * @param    n[in]         指示获取多少个iterm
    * @param    values[out]   获取value的buf
    * @param    ret_vals[out] 返回值，>=0: 成功，返回value的长度;
    *                         <0: ErrCode;
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int PopFront(int n,
                         std::vector<std::string>* values,
                         std::vector<int>* ret_vals) = 0;
    virtual int PopBack(int n,
                        std::vector<std::string>* values,
                        std::vector<int>* ret_vals) = 0;
    /**
    * @brief    清空Table
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Clear() = 0;
};

/**
* @brief    Hash Table基类，key和value为raw格式
* @author   konghui, konghui@baidu.com
* @date     2012-10-24
*/
class IHashTable : public IStoreTable {
public:
    IHashTable() {}
    IHashTable(const std::string& n, const StoreTableType& t):
        IStoreTable(n, t) {}
    virtual ~IHashTable() {}

    /**
     * @brief    GetAll中的数据回调
     * @param    ctx[in]   用户上下文
     * @param    err[in]   错误码
     * @param    key[in]   key
     * @param    value[in] value
     * @return   storage::OK(0): 成功;
     *           <0: ErrCode;
     * @author   konghui, konghui@baidu.com
     * @date     2012-11-06
     */
    typedef int(*HashTraverseCallBack)(void* ctx, int err, // NOLINT
                                       const std::string& key,
                                       const std::string& value);

    /**
    * @brief    获取表内所有数据
    * @param    cb[in]  用户传入的用于处理每条数据的回调
    * @return   >0: 成功，返回value的长度;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int GetAll(HashTraverseCallBack cb, void* ctx) = 0;

    /**
    * @brief    查找iterm
    * @param    key[in]    待查找iterm的key
    * @param    klen[in]   key的长度
    * @param    value[in]  存储value的缓冲区,用户传入
    * @param    vlen[in]   指示用户传入缓冲区的长度
    * @return   >0: 成功，返回value的长度;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Find(const std::string& key, std::string* value) = 0;
    /**
    * @brief    插入iterm
    * @param    key[in]    key缓冲区
    * @param    klen[in]   指示用户传入key缓冲区的长度
    * @param    value[in]  value缓冲区
    * @param    vlen[in]   指示用户传入value缓冲区的长度
    * @param    update_if_exists[in]    如果存在就覆盖,
    *                                   默认不覆盖
    * @return   storage::OK(0): 成功;
    *           storage::KEY_ALREADY_EXISTS(>0): key已经存在;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Insert(const std::string& key, const std::string& value) = 0;
    virtual int Insert(const std::string& key, const std::string& value,
                       bool update_if_exists) = 0;
    /**
    * @brief    批量插入item接口
    *
    * @param    keys[in]    key缓冲区数组
    * @param    values[in]  value缓冲区数组
    * @param    ret_vals[out]   返回结果数据
    * @param    update_if_exists[in]    如果存在就覆盖,
    *                                   默认不覆盖
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @return
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Insert(const std::vector<std::string>& keys,
                       const std::vector<std::string>& values,
                       std::vector<int>* ret_vals,
                       bool update_if_exists) = 0;
    /**
    * @brief    删除iterm
    * @param    key[in]  key缓冲区
    * @return   storage::OK(0): 成功;
    *           storage::KEY_NOT_EXISTS(>0): key不存在;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Erase(const std::string& key) = 0;
    virtual int Erase(const std::vector<std::string>& keys,
                      std::vector<int>* ret_vals) = 0;
    /**
    * @brief    清空Table
    * @return   storage::OK(0): 成功;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Clear() = 0;
};

//  class MultiMapTable : public IStoreTable {
//  public:
//      MultiMapTable() {}
//      ~MultiMapTable() {//TODO}
//      /**
//      * @brief    MultiMapTable迭代器
//      * @author     konghui, konghui@baidu.com
//      * @date     2012-10-23
//      */
//      class MultiMapTableIter {
//          //TODO
//      };
//  };

} // namespace storage
} // namespace dstream

#endif //__DSTREAM_STORAGE_STORAGE_TABLE_H__ // NOLINT
