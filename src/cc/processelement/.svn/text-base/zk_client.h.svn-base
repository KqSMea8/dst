 /***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
* @brief  zk client of processelement
* @author fangjun, fangjun02@baidu.com
* @date   2013-01-23
*/
#ifndef __DSTREAM_CC_PROCESSELEMENT_ZKCLIENT_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_ZKCLIENT_H__ // NOLINT

#include <string>
#include "common/config.h"
#include "metamanager/meta_manager.h"
#include "processelement/bp_progress.h"

namespace dstream {
namespace processelement {

class ZKProgress {
public:
    ZKProgress() {}

    ~ZKProgress() {
        meta_manager::MetaManager::GetMetaManager()->DestoryMetaManager();
    }

    int32_t Init(uint64_t peid, const std::string& app_name, Config* config);
    int32_t GetProgress(SubPoint* point);
    int32_t SetProgress(const SubPoint& point);
    int32_t DelProgress(const SubPoint& point);
    int32_t DelProgressApp(const std::string& app_path);

private:
    std::string m_path;
};

// used for scribe importer
class ZKRegister {
public:
    ZKRegister() {}
    ~ZKRegister() {
        meta_manager::MetaManager::GetMetaManager()->DestoryMetaManager();
    }

    int32_t Init(uint64_t peid,
                 const std::string& app_name,
                 const std::string& config_file,
                 int port);
};

} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_ZKCLIENT_H__ NOLINT
