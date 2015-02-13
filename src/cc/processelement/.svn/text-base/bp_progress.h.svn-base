/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/

/**
 * @file bp_progress.h
 * @author zhenpeng(zhenpeng@baidu.com)
 * @author chaihua(chaihua@baidu.com)
 * @date 2013/01/23 15:37:15
 * @version $Revision$
 * @brief
 *
 **/
#ifndef DSTREAM_PROCESSELEMENT_BP_PROGRESS_H // NOLINT
#define DSTREAM_PROCESSELEMENT_BP_PROGRESS_H // NOLINT

#include <stdint.h>
#include <map>
#include <string>
#include <vector>
#include "common/mutex.h"
#include "common/proto/application.pb.h"

namespace dstream {
namespace processelement {

class ZKProgress;
class ZKRegister;

class BPProgress {
public:
    /**
     * @Brief  read inner progress to *point
     *
     * @Param[out] point
     *
     * @Returns OK, return 0
     */
    int32_t Read(std::vector<SubPoint>* sub_point_list);

    /**
     * @Brief  update inner progress
     *
     * @Param[in] point
     *
     * @Returns OK, return 0
     */
    int32_t Update(const SubPoint& point);

    /**
     * @Brief  save inner progress to ZK
     *
     * @Returns OK, return 0
     */
    int32_t Save();

    /**
     * @Brief  read ZK progress to inner progress
     *
     * @Returns OK, return 0
     */
    int32_t Load(const std::vector<uint32_t>& pipelet_list);
    int32_t Remove(const SubPoint& point);
    int32_t RemoveAll();

    // @brief get thread store progress in zk interval in milli-second
    // @param poll_interval_ptr, interval-ptr
    // @Returns OK ,return 0
    int32_t GetPollInterval(uint64_t* poll_interval_ptr);

    // @brief init data structure
    // @param pe_id pe identifier
    // @param config_file conf file for zk info
    // @Returns OK ,return 0
    int32_t Init(uint64_t pe_id,
                 const std::string& app_name) {
        return Init(pe_id, app_name, "");
    }
    int32_t Init(uint64_t pe_id,
                 const std::string& app_name,
                 const std::string& config_file);

    // @brief uninit data structure
    // @param none
    int32_t Uninit();

    BPProgress() : m_zk_progress(NULL), m_pe_id(0LU), m_init(false) {}

private:
    typedef std::map<uint32_t, SubPoint> SubPointMap;

    SubPointMap     m_sub_point;
    MutexLock       m_lock;
    ZKProgress*     m_zk_progress;
    uint64_t        m_pe_id;
    uint64_t        m_poll_interval;
    bool            m_init;
    std::string     m_app_name;

public:
//-----------------------------------------------------------------
// !!!!! Abandoned interfaces
    int32_t read(std::vector<SubPoint>* sub_point_list) { return Read(sub_point_list); }
    int32_t update(const SubPoint& point) { return Update(point); }
    int32_t save() { return Save(); }
    int32_t load(const std::vector<uint32_t>& pipelet_list) { return Load(pipelet_list); }
    int32_t remove(const SubPoint& point) { return Remove(point); }
    int32_t remove_all() { return RemoveAll(); }
}; // end class BPProgress

class ScribeRegister {
public:
    ScribeRegister() : m_zk_register(NULL) {}
    virtual ~ScribeRegister();
    int32_t Init(uint64_t peid,
                 const std::string& app_name,
                 const std::string& config_file,
                 int port);
private:
    ZKRegister* m_zk_register;
}; // end class ScribeRegister

} // end namespace processelement
} // end namespace dstream

#endif  // DSTREAM_PROCESSELEMENT_BP_PROGRESS_H NOLINT

/* vim: set ts=4 sw=4 sts=4 tw=100 */
