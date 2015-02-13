/*******************************************************************
*
* Copyright (c) Baidu.com, Inc. All Rights Reserved
*
*********************************************************************/
/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    pm watcher
*********************************************************************/

#include "processmaster/pm_watcher.h"

#include <cstddef>

namespace dstream {
namespace pm_watcher {

void PMWatcher::OnSessionExpire(const char* /*path*/) {
    if (NULL != m_pm) {
        m_pm->Stop();
    }
}

void PMWatcher::OnNodeDelete(const char* path) {
    if (NULL != m_pm && m_pm->working() && !m_pm->stop()) {
        m_pm->NodeDelete(path);
    }
}

} // namespace pm_watcher
} // namespace dstream
