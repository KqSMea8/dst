/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2011/12/14
* @filename:    pm_start_check.h
* @author:      zhanggengxin@baidu.com
* @brief:       process master check task before start
*/
#ifndef __PM_START_CHECK_H__ // NOLINT
#define __PM_START_CHECK_H__ // NOLINT

#include <string>

namespace dstream {
namespace pm_start_check {

/************************************************************************/
/* pm check task before start up                                        */
/*pm will run these tasks before startup, if task is error pm will exit */
/************************************************************************/
class PMStartCheckTask {
public:
    virtual ~PMStartCheckTask() {}
    // Check if pm can start for name
    virtual bool RunCheckTask() = 0;
    std::string name() {
        return m_name;
    }
protected:
    std::string m_name;
};

} // namespace pm_start_check
} // namespace dstream

#endif // NOLINT
