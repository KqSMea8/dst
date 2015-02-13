/**
* @file     storage_accessor.h
* @brief    �洢���ʲ����
* @author   konghui, konghui@baidu.com
* @version  1.0.0.1
* @date     2012-10-23
* Copyright (c) 2012, Baidu, Inc. All rights reserved.
*/

#ifndef __DSTREAM_STORAGE_STORAGE_ACCESSOR_H__ //NOLINT
#define __DSTREAM_STORAGE_STORAGE_ACCESSOR_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <string>
#include "storage/error.h"
#include "storage/storage_table.h"

namespace dstream {
namespace storage {

/**
* @brief  Store Type
*/
typedef enum {
    STORE_TYPE_BEGIN,
    DREDIS,
    STORE_TYPE_END
} StoreType;

class IStoreTable;
typedef boost::shared_ptr<IStoreTable> IStoreTablePtr;
/**
* @brief    �洢���ʲ㣬�û����Լ̳и���
* @author   konghui, konghui@baidu.com
* @date     2012-10-24
* @notice   1) ������Ҫ�����罻�����û���Ҫ�����API��ʱ�����
*           2) �̳б��ӿ�ʱ���û���Ҫ�Լ�ʵ�ֳ�ʼ�������ӵȽӿڣ�
*              �ٽ��б������
*/
class StoreAccessor {
public:
    StoreAccessor(): m_is_initialized(false), m_is_valid(false) {}
    explicit StoreAccessor(const StoreType& t):
                            m_is_initialized(false), m_is_valid(false), m_type(t) {}
    virtual ~StoreAccessor() {}
    /**
    * @brief    ����Table
    * @param    table_name[in]  table��
    * @param    table_type[in]  table����
    * @param    replace_old[in] �Ƿ񸲸Ǿɱ�����; true:���ǣ�false:ʧ�ܷ��أ�
    * @param    stpp[out]       �洢Table���ָ���ָ��
    * @return   �ɹ�: storage::OK(0)
    *           ʧ�ܣ�<0, ErrCode��
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int CreateTable(const std::string& table_name,
                            StoreTableType table_type,
                            bool replace_old,
                            IStoreTablePtr* pt) = 0;
    /**
    * @brief    ɾ��Table
    * @param    table_name[in]    table��
    * @return   �ɹ�: storage::OK(0)
    *           ʧ�ܣ�<0, ErrCode��
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int DeleteTable(const std::string& table_name) = 0;
    /**
    * @brief    ��ȡTable
    * @param    table_name[in] Table��
    * @param    stpp[out]    ���Table���ָ���ָ��
    * @return   �ɹ�: storage::OK(0)
    *           ʧ�ܣ�<0, ErrCode��
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int GetTable(const std::string& table_name,
                         IStoreTablePtr* pt) = 0;
    /**
    * @brief    �ͷ�Table
    * @param    stp[in]    Tableָ��
    * @author   konghui, konghui@baidu.com
    * @return   �ɹ�: storage::OK(0)
    *           ʧ�ܣ�<0, ErrCode��
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    // virtual int ReleaseTable(IStoreTable* stp) = 0;
    /**
    * @brief    ���Table�Ƿ����
    * @param    table_name[in] Table��
    * @param    exists[out]    ���Table�Ƿ���ڣ�true:���ڣ�false��������
    * @return   �ɹ�: storage::OK(0)
    *           ʧ�ܣ�<0, ErrCode��
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int TestTableExist(const std::string& table_name, bool* exists) = 0;

    const std::string& name() const {
        return m_name;
    }
    void  set_name(const std::string& n) {
        m_name = n;
    }
    const StoreType& type() const {
        return m_type;
    }

protected:
    // typedef std::map< std::string, boost::shared_ptr<IStoreTable> > TableMap;
    // typedef TableMap::iterator TableMapIter;
    // TableMap    tables_;

    bool        m_is_initialized;
    bool        m_is_valid;
    std::string m_name;
    StoreType   m_type;
};

} // namespace storage
} // namespace dstream

#endif //__DSTREAM_STORAGE_STORAGE_ACCESSOR_H__ // NOLINT
