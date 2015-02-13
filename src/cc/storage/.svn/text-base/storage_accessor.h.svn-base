/**
* @file     storage_accessor.h
* @brief    存储访问层抽象
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
* @brief    存储访问层，用户可以继承该类
* @author   konghui, konghui@baidu.com
* @date     2012-10-24
* @notice   1) 本类主要和网络交互，用户需要留意各API的时间代价
*           2) 继承本接口时，用户需要自己实现初始化、连接等接口，
*              再进行表操作；
*/
class StoreAccessor {
public:
    StoreAccessor(): m_is_initialized(false), m_is_valid(false) {}
    explicit StoreAccessor(const StoreType& t):
                            m_is_initialized(false), m_is_valid(false), m_type(t) {}
    virtual ~StoreAccessor() {}
    /**
    * @brief    创建Table
    * @param    table_name[in]  table名
    * @param    table_type[in]  table类型
    * @param    replace_old[in] 是否覆盖旧表并创建; true:覆盖；false:失败返回；
    * @param    stpp[out]       存储Table句柄指针的指针
    * @return   成功: storage::OK(0)
    *           失败：<0, ErrCode；
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int CreateTable(const std::string& table_name,
                            StoreTableType table_type,
                            bool replace_old,
                            IStoreTablePtr* pt) = 0;
    /**
    * @brief    删除Table
    * @param    table_name[in]    table名
    * @return   成功: storage::OK(0)
    *           失败：<0, ErrCode；
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int DeleteTable(const std::string& table_name) = 0;
    /**
    * @brief    获取Table
    * @param    table_name[in] Table名
    * @param    stpp[out]    输出Table句柄指针的指针
    * @return   成功: storage::OK(0)
    *           失败：<0, ErrCode；
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    virtual int GetTable(const std::string& table_name,
                         IStoreTablePtr* pt) = 0;
    /**
    * @brief    释放Table
    * @param    stp[in]    Table指针
    * @author   konghui, konghui@baidu.com
    * @return   成功: storage::OK(0)
    *           失败：<0, ErrCode；
    * @author   konghui, konghui@baidu.com
    * @date     2012-10-24
    */
    // virtual int ReleaseTable(IStoreTable* stp) = 0;
    /**
    * @brief    检测Table是否存在
    * @param    table_name[in] Table名
    * @param    exists[out]    输出Table是否存在，true:存在；false：不存在
    * @return   成功: storage::OK(0)
    *           失败：<0, ErrCode；
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
