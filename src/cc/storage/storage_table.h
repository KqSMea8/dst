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
* @brief    StoreTable�ӿ��࣬����StoreTable���̳д���
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
* @brief    List Table���࣬valueΪraw��ʽ
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
     * @brief    GetAll�е����ݻص�
     * @param    ctx[in]   �û�������
     * @param    err[in]   ������
     * @param    data[in]   data
     * @return   storage::OK(0): �ɹ�;
     *           <0: ErrCode;
     * @author   konghui, konghui@baidu.com
     * @date     2012-11-06
     */
    typedef int(*ListTraverseCallBack)(void* ctx, int err,              // NOLINT 
                                       const std::string& data);
    /**
    * @brief    ��ȡ������������
    * @param    cb[in]  �û���������ڴ���ÿ�����ݵĻص�
    * @return   >0: �ɹ�������value�ĳ���;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int GetAll(ListTraverseCallBack cb, void* ctx) = 0;
    /**
    * @brief    ��ȡ����head/tail��iterm
    * @param    s[out]      ��ȡvalue��buf
    * @return   >0: �ɹ�;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int GetFront(std::string* s) = 0;
    virtual int GetBack(std::string* s) = 0;
    /**
    * @brief    Push iterm������head/tail
    * @param    value[in]  value������
    * @param    len[in]    ָʾ�û����뻺�����ĳ���
    * @return   storage::OK(0): �ɹ�;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int PushFront(const std::string& value) = 0;
    virtual int PushBack(const std::string& value) = 0;
    /**
    * @brief    ����push
    * @param    values[in]  value
    * @param    ret_vals[out] ����ֵ��0: �ɹ�;
    *                         <0: ErrCode;
    * @return   storage::OK(0): �ɹ�;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int PushFront(const std::vector<std::string>& values,
                            std::vector<int>* ret_vals) = 0;
    virtual int PushBack(const std::vector<std::string>& values,
                            std::vector<int>* ret_vals) = 0;
    /**
    * @brief    Pop����head/tail��iterm
    * @param    value[out]  ��ȡvalue��buf
    * @return   >=0: �ɹ�;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int PopFront(std::string* value) = 0;
    virtual int PopBack(std::string* value) = 0;
    /**
    * @brief    ����pop
    * @param    n[in]         ָʾ��ȡ���ٸ�iterm
    * @param    values[out]   ��ȡvalue��buf
    * @param    ret_vals[out] ����ֵ��>=0: �ɹ�������value�ĳ���;
    *                         <0: ErrCode;
    * @return   storage::OK(0): �ɹ�;
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
    * @brief    ���Table
    * @return   storage::OK(0): �ɹ�;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Clear() = 0;
};

/**
* @brief    Hash Table���࣬key��valueΪraw��ʽ
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
     * @brief    GetAll�е����ݻص�
     * @param    ctx[in]   �û�������
     * @param    err[in]   ������
     * @param    key[in]   key
     * @param    value[in] value
     * @return   storage::OK(0): �ɹ�;
     *           <0: ErrCode;
     * @author   konghui, konghui@baidu.com
     * @date     2012-11-06
     */
    typedef int(*HashTraverseCallBack)(void* ctx, int err, // NOLINT
                                       const std::string& key,
                                       const std::string& value);

    /**
    * @brief    ��ȡ������������
    * @param    cb[in]  �û���������ڴ���ÿ�����ݵĻص�
    * @return   >0: �ɹ�������value�ĳ���;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-11-06
    */
    virtual int GetAll(HashTraverseCallBack cb, void* ctx) = 0;

    /**
    * @brief    ����iterm
    * @param    key[in]    ������iterm��key
    * @param    klen[in]   key�ĳ���
    * @param    value[in]  �洢value�Ļ�����,�û�����
    * @param    vlen[in]   ָʾ�û����뻺�����ĳ���
    * @return   >0: �ɹ�������value�ĳ���;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Find(const std::string& key, std::string* value) = 0;
    /**
    * @brief    ����iterm
    * @param    key[in]    key������
    * @param    klen[in]   ָʾ�û�����key�������ĳ���
    * @param    value[in]  value������
    * @param    vlen[in]   ָʾ�û�����value�������ĳ���
    * @param    update_if_exists[in]    ������ھ͸���,
    *                                   Ĭ�ϲ�����
    * @return   storage::OK(0): �ɹ�;
    *           storage::KEY_ALREADY_EXISTS(>0): key�Ѿ�����;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Insert(const std::string& key, const std::string& value) = 0;
    virtual int Insert(const std::string& key, const std::string& value,
                       bool update_if_exists) = 0;
    /**
    * @brief    ��������item�ӿ�
    *
    * @param    keys[in]    key����������
    * @param    values[in]  value����������
    * @param    ret_vals[out]   ���ؽ������
    * @param    update_if_exists[in]    ������ھ͸���,
    *                                   Ĭ�ϲ�����
    * @return   storage::OK(0): �ɹ�;
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
    * @brief    ɾ��iterm
    * @param    key[in]  key������
    * @return   storage::OK(0): �ɹ�;
    *           storage::KEY_NOT_EXISTS(>0): key������;
    *           <0: ErrCode;
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-25
    */
    virtual int Erase(const std::string& key) = 0;
    virtual int Erase(const std::vector<std::string>& keys,
                      std::vector<int>* ret_vals) = 0;
    /**
    * @brief    ���Table
    * @return   storage::OK(0): �ɹ�;
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
//      * @brief    MultiMapTable������
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
