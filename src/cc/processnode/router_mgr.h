/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_ROUTER_MGR_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_ROUTER_MGR_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

#include <string>

#include "common/error.h"
#include "common/mutex.h"
#include "processnode/app_router.h"
#include "processnode/pe_wrapper_manager.h"
#include "processnode/zk_operator.h"
#include "transporter/transporter_stubmgr.h"

namespace dstream {
namespace router {

class AppRouterNotifier : public AppRouter {
public:
    virtual void NotifyFunc(NotifyEvent type, uint64_t notify_peid, void* notify_ctx);

    static const char* kTraceTag;
};

class RouterMgr {
public:
    static RouterMgr* Instance() {
        if (NULL != pInstance) {
            return pInstance;
        }
        RWLockGuard lock_guard(instance_lock, true);
        if (NULL == pInstance) {
            pInstance = new (std::nothrow) RouterMgr();
        }
        return pInstance;
    }

    static bool IsCreated() {
        RWLockGuard lock_guard(instance_lock, false);
        return (NULL != pInstance);
    }

public:
    // sync interface
    // ---------------------------
    // add local PE, if new app, modify app_router_map
    error::Code AddPE(uint64_t peid);
    // del local PE, if none local PE in app, del app from app_router_map
    error::Code DelPE(uint64_t peid);

    void SetConfigFile(const std::string& conf) {
        m_conf_file = conf;
    }

    typedef boost::shared_ptr< AppRouterNotifier > AppRouterPtr;
    typedef boost::unordered_map< uint64_t, AppRouterPtr > AppRouterMap;

protected:
    RouterMgr();
    static RouterMgr* pInstance;
    static RWLock instance_lock;

    static const char* kRouterMgrTrace;

private:
    // AppID -> AppRouterPtr
    AppRouterMap m_app_router_map;
    MutexLock m_app_router_lock;
    std::string m_conf_file;
};

} // namespace router
} // namespace dstream

#endif // NOLINT
