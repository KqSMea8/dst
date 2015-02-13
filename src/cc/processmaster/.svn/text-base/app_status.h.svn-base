/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/
/**
* @created:     2013/7/2013/07/01
* @filename:    app_status.h
* @author:      fangjun02@baidu.com
* @brief:       app status functions used for pm
*/



#ifndef __DSTREAM_CC_PROCESSMASTER_PM_APP_STATUS_H__ // NOLINT
#define __DSTREAM_CC_PROCESSMASTER_PM_APP_STATUS_H__  // NOLINT

#include <boost/unordered_map.hpp>
#include <set>
#include <utility>
#include "common/application_tools.h"

namespace dstream {
namespace app_status {

typedef boost::unordered_map<uint64_t, unsigned int> PidRevMap;
typedef boost::unordered_map<uint64_t, unsigned int>::iterator PidRevMapIter;
typedef boost::unordered_map<uint64_t, unsigned int>::const_iterator PidRevMapConstIter;
typedef std::pair<uint64_t, unsigned int> PidRevPair;

class AppStatus {
public:
    static AppStatus* GetAppStatus();
    static int32_t InitAppStatus();
    static void DestroyAppStatus();
    void UpdateApplication(const Application& app);
    int32_t AddProcessor(const uint64_t& pid, const unsigned int& revision);
    int32_t DelProcessor(const uint64_t& pid);
    int32_t UpdateProcessor(const uint64_t& pid, const unsigned int& revision);
    int32_t GetProcessorRevision(const uint64_t& pid, unsigned int* revision);

private:
    AppStatus() {}
    void GetDifference(const PidRevMap& large_map,
                       const PidRevMap& less_map, PidRevMap& diff_map);
    static AppStatus* m_app_status_instance;
    PidRevMap m_pid_rev_map;
    MutexLock m_lock;
};

} // namespace app_status
} // namespace dstream

#endif // NOLINT
