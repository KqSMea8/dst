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

#include "processelement/zk_client.h"
#include "common/id.h"
#include "common/proto/application.pb.h"

namespace dstream {
namespace processelement {

int32_t ZKProgress::Init(uint64_t peid, const std::string& app_name, Config* config) {
    if (!meta_manager::MetaManager::InitMetaManagerContext(config)) {
        DSTREAM_WARN("fail to init meta manager context");
        return error::ZK_INIT_ERROR;
    }
    // init appname sub-dir
    // we use an empty subpoint to init appname dir
    int ret = meta_manager::MetaManager::GetMetaManager()->UpdateSubPoint(app_name, SubPoint());
    if (ret != 0) {
        DSTREAM_WARN("add AppName(%s) sub-dir failed. ret=%d", app_name.c_str(), ret);
        return ret;
    }
    // path = "AppName/(ProcessorID)"
    std::ostringstream oss;
    oss << app_name << "/" << EXTRACT_PROCESSOR_ID(peid);
    ret = meta_manager::MetaManager::GetMetaManager()->UpdateSubPoint(oss.str(), SubPoint());
    if (ret != 0) {
        DSTREAM_WARN("add AppName/Processor(%s) sub-dir failed. ret=%d", oss.str().c_str(), ret);
        return ret;
    }
    m_path.assign(oss.str());
    DSTREAM_INFO("init zk progress(%s) OK.", m_path.c_str());

    return error::OK;
}

int32_t ZKProgress::GetProgress(SubPoint* point) {
    int32_t ret = 0;
    std::ostringstream oss;
    oss << m_path << "/" << point->pipelet();
    ret = meta_manager::MetaManager::GetMetaManager()->GetSubPoint(oss.str(), point);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to get path[%s] progress", oss.str().c_str());
        return ret;
    }

    return error::OK;
}

int32_t ZKProgress::SetProgress(const SubPoint& point) {
    int32_t ret;

    std::ostringstream oss;
    oss << m_path << "/" << point.pipelet();
    ret = meta_manager::MetaManager::GetMetaManager()->UpdateSubPoint(oss.str(), point);
    if (ret < error::OK) {
        DSTREAM_WARN("fail to set path[%s] progress", oss.str().c_str());
        return ret;
    }
    return error::OK;
}

int32_t ZKProgress::DelProgress(const SubPoint& point) {
    SubPoint vpoint = point;
    // check exist
    std::ostringstream oss;
    oss << m_path << "/" << point.pipelet();
    int ret = meta_manager::MetaManager::GetMetaManager()->GetSubPoint(oss.str(), &vpoint);
    if (ret == error::ZK_NO_NODE) {
        DSTREAM_WARN("subpoint path[%s] not exist.", oss.str().c_str());
        return error::OK;
    }

    ret = meta_manager::MetaManager::GetMetaManager()->DelSubPoint(oss.str(), vpoint);
    if (ret != error::OK) {
        DSTREAM_WARN("delete %s subpoint failed", oss.str().c_str());
    }
    return ret;
}

int32_t ZKProgress::DelProgressApp(const std::string& app_path) {
    return meta_manager::MetaManager::GetMetaManager()->DelAppSubPoint(app_path);
}

int32_t ZKRegister::Init(uint64_t peid,
                         const std::string& app_name,
                         const std::string& config_file,
                         int port) {
    int ret = error::OK;
    config::Config config;
    ret = config.ReadConfig(config_file);
    if (error::OK != ret) {
        DSTREAM_WARN("read conf file [%s] failed, error [%s]",
                     config_file.c_str(), error::ErrStr(ret));
        return ret;
    }
    if (!meta_manager::MetaManager::InitMetaManagerContext(&config)) {
        DSTREAM_WARN("fail to init meta manager context");
        return error::ZK_INIT_ERROR;
    }
    ret = meta_manager::MetaManager::GetMetaManager()->AddScribeAppNode(app_name, "{}");
    if (ret != 0) {
        DSTREAM_WARN("set AppName[%s] failed. error [%s]", app_name.c_str(), error::ErrStr(ret));
        return ret;
    }
    std::ostringstream oss;
    oss << app_name << "/" << peid;
    std::string ip;
    ret = GetLocalhostIP(&ip);
    if (error::OK != ret) {
        DSTREAM_WARN("Get Localhost ip fail");
        return ret;
    }
    ip += ":";
    StringAppendNumber(&ip, port);
    std::ostringstream json;
    json << "{\"address\":\"" << ip << "\"}";
    ret = meta_manager::MetaManager::GetMetaManager()->AddScribePENode(oss.str(), json.str());
    if (ret != 0) {
        DSTREAM_WARN("set AppName(%s) pe (%lu) ip_port(%s) failed. ret=%d", app_name.c_str(),
                     peid, ip.c_str(), ret);
        return ret;
    }
    return error::OK;
}

} // namespace processelement
} // namespace dstream
