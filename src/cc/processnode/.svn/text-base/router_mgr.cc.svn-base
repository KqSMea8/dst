/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#include <string>
#include <utility>
#include <vector>
#include "processnode/router_mgr.h"

namespace dstream {
namespace router {

typedef error::Code ErrCode;
typedef std::vector<transporter::IfSourceMeta> TPIfSourceVec;
typedef std::vector<transporter::OfMeta>       TPOfSourceVec;

const char* AppRouterNotifier::kTraceTag = "NotifierTrace";

void AppRouterNotifier::NotifyFunc(NotifyEvent type, uint64_t notify_peid, void* notify_ctx) {
    ErrCode err_ret = error::OK;
    UNLIKELY_IF(processnode::PEWrapperManager::GetPEWrapperManager() == NULL) {
        return;
    }
    // get pe wrapper ptr
    processnode::PEWrapper::PEWrapperPtr ptr(
        processnode::PEWrapperManager::GetPEWrapperManager()->GetPEWrapper(notify_peid));
    UNLIKELY_IF(NULL == ptr) {
        DSTREAM_ERROR("[%s] approuter(%lu) get PEwrapper ptr fail",
                      kTraceTag, EXTRACT_APP_ID(notify_peid));
        return;
    }
    if (type < PE_EVENT_END) {
        PEInfo* ctx = static_cast<PEInfo*>(notify_ctx);
        // ctx->pe_id;
        // ctx->location;
        switch (type) {
        case ADD_UP_PE:
        {
            DSTREAM_INFO("[%s] pe(%lu) ADD_UP_PE(%lu) location(%s)",
                         kTraceTag, notify_peid, ctx->pe_id, ctx->location.c_str());
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPIfSourceVec* sources = new TPIfSourceVec();
            transporter::IfSourceMeta if_meta = {ctx->pe_id, ESPFindNode(ctx->location.c_str())};
            sources->push_back(if_meta);
            if ((err_ret = tp->AddIfSource(sources)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to add in_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pe_id, err_ret);
            }
            break;
        }
        case DEL_UP_PE:
        {
            DSTREAM_INFO("[%s] pe(%lu) DEL_UP_PE(%lu)",
                         kTraceTag, notify_peid, ctx->pe_id);
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPIfSourceVec* sources = new TPIfSourceVec();
            transporter::IfSourceMeta if_meta = {ctx->pe_id, 0};
            sources->push_back(if_meta);
            if ((err_ret = tp->DelIfSource(sources)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to del in_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pe_id, err_ret);
            }
            break;
        }
        case UPDATE_UP_PE:
        {
            DSTREAM_INFO("[%s] pe(%lu) UPDATE_UP_PE(%lu) location(%s)",
                         kTraceTag, notify_peid, ctx->pe_id, ctx->location.c_str());
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPIfSourceVec* sources = new TPIfSourceVec();
            transporter::IfSourceMeta if_meta = {ctx->pe_id, ESPFindNode(ctx->location.c_str())};
            sources->push_back(if_meta);
            if ((err_ret = tp->UpdIfSource(sources)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to update in_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pe_id, err_ret);
            }
            break;
        }
        default:
            DSTREAM_ERROR("[%s] unknown pe notify type(%d)", kTraceTag, type);
            break;
        } // end switch
    } else if (type < PROCESSOR_EVENT_END) {
        ProcessorInfo* ctx = static_cast<ProcessorInfo*>(notify_ctx);
        // ctx->pro_id;
        // ctx->parallel;
        // ctx->tags;
        switch (type) {
        case ADD_DOWN_PRO:
        {
            DSTREAM_INFO("[%s] pe(%lu) ADD_DOWN_PRO(%lu) parallel(%u) tags_size(%zu)",
                         kTraceTag, notify_peid, ctx->pro_id, ctx->parallel, ctx->tags.size());
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPOfSourceVec* outflows = new TPOfSourceVec();
            std::vector<std::string> tag_vec;
            CONST_FOR_EACH(TagSet, ctx->tags, iter) {
                tag_vec.push_back(*iter);
            }
            transporter::OfMeta of_meta = {ctx->pro_id, ctx->parallel, tag_vec};
            outflows->push_back(of_meta);
            if ((err_ret = tp->AddOutflow(outflows)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to add out_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pro_id, err_ret);
            }
            break;
        }
        case DEL_DOWN_PRO:
        {
            DSTREAM_INFO("[%s] pe(%lu) DEL_DOWN_PRO(%lu)",
                         kTraceTag, notify_peid, ctx->pro_id);
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPOfSourceVec* outflows = new TPOfSourceVec();
            std::vector<std::string> tag_vec;
            CONST_FOR_EACH(TagSet, ctx->tags, it) {
                tag_vec.push_back(*it);
            }
            transporter::OfMeta of_meta = {ctx->pro_id, ctx->parallel, tag_vec};
            outflows->push_back(of_meta);
            if ((err_ret = tp->DelOutflow(outflows)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to del out_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pro_id, err_ret);
            }
            break;
        }
        case UPDATE_DOWN_PRO:
        {
            DSTREAM_INFO("[%s] pe(%lu) UPDATE_DOWN_PRO(%lu) parallel(%u) tags_size(%zu)",
                         kTraceTag, notify_peid, ctx->pro_id, ctx->parallel, ctx->tags.size());
            transporter::Transporter* tp = ptr->GetTransporter();
            assert(tp);
            TPOfSourceVec* outflows = new TPOfSourceVec();
            std::vector<std::string> tag_vec;
            CONST_FOR_EACH(TagSet, ctx->tags, it) {
                tag_vec.push_back(*it);
            }
            transporter::OfMeta of_meta = {ctx->pro_id, ctx->parallel, tag_vec};
            outflows->push_back(of_meta);
            if ((err_ret = tp->UpdOutflow(outflows)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to update out_flow source(%lu), err(%d)",
                              kTraceTag, ctx->pro_id, err_ret);
            }
            break;
        }
        default:
            DSTREAM_ERROR("[%s] unknown processor notify type(%d)", kTraceTag, type);
            break;
        } // end switch
    } else if (type == APP_FLOW_CTRL) {
        CtrlInfo* ctx = static_cast<CtrlInfo*>(notify_ctx);
        DSTREAM_INFO("[%s] pe(%lu) APP_FLOW_CTRL, set qps limit: %d)",
                     kTraceTag, notify_peid, ctx->flag.qps_limit());
        ptr->set_flow_control(ctx->flag);
    } else {
        DSTREAM_ERROR("[%s] unknown notify type(%d)", kTraceTag, type);
    }
}

RouterMgr* RouterMgr::pInstance = NULL;
RWLock RouterMgr::instance_lock;

const char* RouterMgr::kRouterMgrTrace = "RouterMgrTrace";

RouterMgr::RouterMgr() {}

error::Code RouterMgr::AddPE(uint64_t peid) {
    uint64_t appid = EXTRACT_APP_ID(peid);

    MutexLockGuard lock_guard(m_app_router_lock); // write lock
    AppRouterMap::iterator iter = m_app_router_map.find(appid);
    if (m_app_router_map.end() != iter) {
        // the app has already exist
        return iter->second->AddPE(peid);
    } else {
        // the app is new
        AppRouterPtr new_app(new AppRouterNotifier());
        // init AppRouter
        error::Code ret = new_app->Init(appid, m_conf_file);
        if (error::OK == ret) {
            std::pair<AppRouterMap::iterator, bool> insert_ret =
                m_app_router_map.insert(std::make_pair(appid, new_app));
            assert(insert_ret.second);
            ret = insert_ret.first->second->AddPE(peid);
        }
        return ret;
    }
}

error::Code RouterMgr::DelPE(uint64_t peid) {
    error::Code ret = error::OK;
    uint64_t appid = EXTRACT_APP_ID(peid);

    MutexLockGuard lock_guard(m_app_router_lock); // write lock
    AppRouterMap::iterator find_iter = m_app_router_map.find(appid);
    if (find_iter != m_app_router_map.end()) {
        ret = find_iter->second->DelPE(peid);
        if (find_iter->second->LocalPENum() == 0) {
            m_app_router_map.erase(appid);
        }
    } else {
        DSTREAM_WARN("[%s] delete pe(%lu) is not exist",
                     kRouterMgrTrace, peid);
    }
    return ret;
}

} // namespace router
} // namespace dstream

