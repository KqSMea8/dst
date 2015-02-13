/***************************************************************************
 *
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$
 *
 **************************************************************************/



/**
 * @file processelement/bp_progress.cc
 * @author zhenpeng(zhenpeng@baidu.com)
 * @date 2013/01/23 16:08:13
 * @version $Revision$
 * @brief
 *
 **/

#include "processelement/bp_progress.h"
#include "processelement/zk_client.h"

namespace dstream {
namespace processelement {

const char* kPEZKDefaultConfFile = "./pe_zk.conf";
const char* kProgressPersistPollIntervalMillSeconds = "ProgressInterval";
const char* kProgressAppName = "AppName";

#define CHECK_INIT() do { \
    if (!m_init) { \
        DSTREAM_WARN("bigpipe progress has not inited."); \
        return error::ZK_INIT_ERROR; \
    } \
} while (0);

int32_t BPProgress::Read(std::vector<SubPoint>* sub_point_list) {
    if (NULL == sub_point_list) {
        DSTREAM_WARN("param should not be NULL.");
        return error::BAD_ARGUMENT;
    }

    MutexLockGuard PointGuard(m_lock);
    SubPointMap::iterator iter = m_sub_point.begin();
    while (iter != m_sub_point.end()) {
        sub_point_list->push_back(iter->second);
        ++iter;
    }
    return 0;
}

int32_t BPProgress::Update(const SubPoint& point) {
    MutexLockGuard PointGuard(m_lock);
    m_sub_point[point.pipelet()] = point;
    return 0;
}

int32_t BPProgress::Save() {
    CHECK_INIT();
    int32_t ret = error::OK;
    int32_t res = error::OK;
    SubPointMap sub_point_map;

    {
        MutexLockGuard PointGuard(m_lock);
        sub_point_map = m_sub_point;
    }

    // write info to zk
    SubPointMap::iterator iter = sub_point_map.begin();
    while (iter != sub_point_map.end()) {
        res = m_zk_progress->SetProgress(iter->second);
        if (error::OK != res) {
            DSTREAM_WARN("set progress to zk failed, [error=%d]", res);
            ret = error::SET_ZKDATA_FAIL;
            break;
        }
        ++iter;
    }

    return ret;
}

int32_t BPProgress::Load(const std::vector<uint32_t>& pipelet_list) {
    CHECK_INIT();
    int32_t ret = error::OK;
    int32_t res = error::OK;

    std::vector<uint32_t>::const_iterator iter = pipelet_list.begin();
    while (iter != pipelet_list.end()) {
        SubPoint point;
        point.set_pipelet(*iter);

        // read info from zk
        res = m_zk_progress->GetProgress(&point);
        if (error::OK != res) {
            DSTREAM_WARN("get progress from zk failed, [error=%d]", res);
            ret = error::GET_ZK_DATA_FAIL;
            break;
        } else {
            MutexLockGuard PointGuard(m_lock);
            m_sub_point[point.pipelet()] = point;
        }

        ++iter;
    }

    return ret;
}

int32_t BPProgress::Remove(const SubPoint& point) {
    CHECK_INIT();
    return m_zk_progress->DelProgress(point);
}

int32_t BPProgress::RemoveAll() {
    CHECK_INIT();
    return m_zk_progress->DelProgressApp(m_app_name);
}

int32_t BPProgress::Init(uint64_t pe_id,
                         const std::string& app_name,
                         const std::string& config_file) {
    int32_t ret = error::OK;
    int32_t res = error::OK;
    bool go_on = true;
    std::string conf_file;
    config::Config conf;
    m_pe_id = pe_id;
    m_app_name = app_name;

    if (NULL != m_zk_progress) {
        DSTREAM_WARN("BPProgress seems init already");
        go_on = false;
        ret = error::UNPOSSIBLE;
    }

    if ("" == config_file) {
        conf_file = kPEZKDefaultConfFile;
    } else {
        conf_file = config_file;
    }

    if (go_on) {
        DSTREAM_INFO("PE conf file : %s",
                     conf_file.c_str());
        res = conf.ReadConfig(conf_file);
        if (error::OK != res) {
            DSTREAM_WARN("read conf file %s failed",
                         conf_file.c_str());
            go_on = false;
            ret = error::CONFIG_BAD_FILE;
        }
    }

    if (go_on) {
        std::string ulvalue;
        if (error::OK != conf.GetValue(
                kProgressPersistPollIntervalMillSeconds,
                &ulvalue)) {
            DSTREAM_WARN("get ProgressPersistPollInterval failed");
            m_poll_interval = 100;
        } else {
            m_poll_interval = strtoull(ulvalue.c_str(), NULL, 10);
        }
    }

    if (go_on) {
        m_zk_progress = new (std::nothrow) ZKProgress();
        if (NULL == m_zk_progress) {
            DSTREAM_WARN("no memory to allocate");
            go_on = false;
            ret = error::BAD_MEMORY_ALLOCATION;
        }
    }

    if (go_on) {
        res = m_zk_progress->Init(m_pe_id, m_app_name, &conf);
        if (error::OK != res) {
            DSTREAM_WARN("init zk progress failed");
            go_on = false;
            ret = error::ZK_INIT_ERROR;
        }
    }

    if (ret == error::OK) {
        m_init = true;
    }

    return ret;
}

int32_t BPProgress::Uninit() {
    m_pe_id = 0;
    if (NULL != m_zk_progress) {
        delete m_zk_progress;
        m_zk_progress = NULL;
    }
    m_init = false;
    return error::OK;
}

int32_t BPProgress::GetPollInterval(uint64_t* poll_interval_ptr) {
    int32_t ret = error::OK;
    bool go_on = true;
    if (NULL == poll_interval_ptr) {
        DSTREAM_WARN("invalid parm");
        go_on = false;
        ret = error::BAD_ARGUMENT;
    }

    if (go_on && NULL == m_zk_progress) {
        DSTREAM_WARN("seems still uninited");
        go_on = false;
        ret = error::ZK_INIT_ERROR;
    }

    if (go_on) {
        *poll_interval_ptr = m_poll_interval;
    }

    return ret;
}

ScribeRegister::~ScribeRegister() {
    if (NULL != m_zk_register) {
        delete m_zk_register;
        m_zk_register = NULL;
    }
}

int32_t ScribeRegister::Init(uint64_t peid,
                             const std::string& app_name,
                             const std::string& config_file,
                             int port) {
    m_zk_register = new (std::nothrow) ZKRegister();
    if (NULL == m_zk_register) {
        DSTREAM_WARN("no memory to allocate");
        return error::BAD_MEMORY_ALLOCATION;
    }
    return m_zk_register->Init(peid, app_name, config_file, port);
}

} // end namespace processelement
} // end namespace dstream

/* vim: set ts=4 sw=4 sts=4 tw=100 */
