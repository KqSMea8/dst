/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <map>
#include <vector>
#include "processnode/app_router.h"

namespace dstream {
namespace router {

const int ZK_FETCH_INTERVAL = 10000; // 10 ms
const char* AppRouter::kAppRouterTrace = "RouterTrace";

AppRouter::AppRouter()
        : m_app_id(0),
        m_flow_ctrl() {
}

AppRouter::~AppRouter() {
    Destroy();
}

error::Code AppRouter::Init(const uint64_t appid, const std::string& config_file) {
    error::Code err_ret;
    // read config
    config::Config conf;
    err_ret = conf.ReadConfig(config_file);
    if (err_ret < error::OK) {
        DSTREAM_ERROR("[%s] read config(%s) fail",
                      kAppRouterTrace, config_file.c_str());
        return err_ret;
    }

    m_zk_wrapper = ZkWrapper::ZkWrapperPtr(new (std::nothrow) ZkWrapper());
    UNLIKELY_IF(NULL == m_zk_wrapper) {
        return error::BAD_MEMORY_ALLOCATION;
    }
    err_ret = m_zk_wrapper->Init(conf, OnZKWatchEventCallback, this);
    UNLIKELY_IF(err_ret < error::OK) {
        return err_ret;
    }
    // first time get app
    m_app_id = appid;
    AppEventProcess(m_app_id);
    // add app children watch
    AppChildEventProcess(m_app_id);
    return err_ret;
}

void AppRouter::Destroy() {
    if (NULL != m_zk_wrapper) {
        // close zk, callback will never be called
        m_zk_wrapper->Destroy();
    }
    // clear local data
    {
        RWLockGuard lock_guard(m_data_lock, true);
        DestroyProcssorMap(&m_all_processor);
        DestroyWatchPEMap(&m_all_watch_pes);
    }
}

error::Code AppRouter::AddPE(uint64_t peid) {
    RWLockGuard lock_guard(m_data_lock, true); // write lock
    CONST_NOT_FIND_IF(PESet, m_local_pes, iter, peid) {
        std::pair<PESet::iterator, bool> ret = m_local_pes.insert(peid);
        UNLIKELY_IF(!ret.second) {
            DSTREAM_ERROR("[%s] fail to insert pe(%lu) into local_pes",
                          kAppRouterTrace, peid);
            return error::BAD_MEMORY_ALLOCATION;
        } else {
            DSTREAM_DEBUG("[%s] add pe(%lu) into local_pes",
                          kAppRouterTrace, peid);
        }
        // notify add local pe
        NotifyLocalPEAdd_L(peid);
    }
    return error::OK;
}

error::Code AppRouter::DelPE(uint64_t peid) {
    RWLockGuard lock_guard(m_data_lock, true); // write lock
    FIND_IF(PESet, m_local_pes, iter, peid) {
        m_local_pes.erase(iter);
    }
    // clear del local pe
    ClearLocalPEDel_L(peid);
    return error::OK;
}

error::Code AppRouter::ClearLocalPEDel_L(const uint64_t peid) {
    // find peid in all watching_pes & erase
    WatchPEMap::iterator pe_it;
    for (pe_it = m_all_watch_pes.begin(); pe_it != m_all_watch_pes.end(); ) {
        FIND_IF(PESet, pe_it->second->watching_pes, watch_pe_it, peid) {
            pe_it->second->watching_pes.erase(watch_pe_it);
            if (pe_it->second->watching_pes.size() == 0) {
                // GC AppPE
                delete pe_it->second;
                pe_it->second = NULL;
                m_all_watch_pes.erase(pe_it++);
            } else {
                pe_it++;
            }
        } else {
            pe_it++;
        }
    }
    return error::OK;
}

error::Code AppRouter::GetPNAddr(const ProcessorElement& t_pe,
        std::string* pn_addr) {
    if (t_pe.backups_size() > 0 &&
        t_pe.backups(0).backup_pe_id().id() == t_pe.primary(0).id()) {
        // pe has deployed
        // get pn addr
        error::Code t_ret = error::FAILED_EXPECTATION;
        PN pn;
        // TODO(bijia): add max try times
        while (t_ret != error::OK) {
            t_ret = m_zk_wrapper->GetPN(t_pe.backups(0).pn_id(), &pn);
            if (t_ret == error::ZK_NO_NODE ||
                t_ret == error::BAD_ARGUMENT ||
                t_ret == error::CONNECT_ZK_FAIL ||
                t_ret == error::ZK_CONNECTION_LOST) {
                *pn_addr = "";
                return t_ret;
            }
            usleep(ZK_FETCH_INTERVAL);
        }
        *pn_addr = pn.host() + ":";
        StringAppendNumber(pn_addr, pn.sub_port());
    } else {
        // pe has not deployed
        // set pn addr to empty
        *pn_addr = "";
    }
    return error::OK;
}

error::Code AppRouter::GetApplication(uint64_t appid, Application* t_app) {
    error::Code err_ret;
    AppID t_appid;
    t_appid.set_id(appid);
    while ((err_ret = m_zk_wrapper->GetApplication(t_appid, t_app)) < error::OK) {
        if (err_ret == error::ZK_NO_NODE ||
            err_ret == error::BAD_ARGUMENT ||
            err_ret == error::CONNECT_ZK_FAIL ||
            err_ret == error::ZK_CONNECTION_LOST) {
            return err_ret;
        }
        // fetch Application fail
        DSTREAM_WARN("[%s] fail to get Application appid(%lu), error_no(%d)",
                     kAppRouterTrace, appid, err_ret);
        usleep(ZK_FETCH_INTERVAL);
    }
    return error::OK;
}

error::Code AppRouter::GetPEList(uint64_t appid, PESet* peid_set) {
    error::Code err_ret;
    AppID t_appid;
    t_appid.set_id(appid);
    std::vector<std::string> peid_list;
    while ((err_ret = m_zk_wrapper->GetPEList(t_appid, &peid_list)) < error::OK) {
        if (err_ret == error::ZK_NO_NODE ||
            err_ret == error::BAD_ARGUMENT ||
            err_ret == error::CONNECT_ZK_FAIL ||
            err_ret == error::ZK_CONNECTION_LOST) {
            return err_ret;
        }
        // fetch Application children fail
        DSTREAM_WARN("[%s] fail to get Application's PE list appid(%lu), error_no(%d)",
                     kAppRouterTrace, appid, err_ret);
        peid_list.clear();
        usleep(ZK_FETCH_INTERVAL);
    }
    // change string to int
    peid_set->clear();
    FOR_EACH(std::vector<std::string>, peid_list, peid_iter) {
        uint64_t peid = atoll(peid_iter->c_str());
        if (0 == peid) {
            DSTREAM_ERROR("[%s] parse peid fail, peid:%s", kAppRouterTrace, peid_iter->c_str());
        } else {
            peid_set->insert(peid);
            // TODO(bijia) : check fail?
        }
    }
    return error::OK;
}

error::Code AppRouter::GetProcessorElement(uint64_t peid, ProcessorElement* t_pe) {
    error::Code err_ret;
    AppID t_appid;
    t_appid.set_id(m_app_id);
    PEID t_peid;
    t_peid.set_id(peid);
    while ((err_ret = m_zk_wrapper->GetProcessorElement(t_appid, t_peid, t_pe)) < error::OK) {
        if (err_ret == error::ZK_NO_NODE ||
            err_ret == error::BAD_ARGUMENT ||
            err_ret == error::CONNECT_ZK_FAIL ||
            err_ret == error::ZK_CONNECTION_LOST) {
            return err_ret;
        }
        // fetch ProcessorElement fail
        DSTREAM_WARN("[%s] fail to get processor element peid(%lu), error_no(%d)",
                     kAppRouterTrace, peid, err_ret);
        usleep(ZK_FETCH_INTERVAL);
    }
    return error::OK;
}

error::Code AppRouter::AddWatchPE_L(uint64_t peid, const PESet& watching_peids) {
    // check peid exist?
    error::Code err_ret;
    PESet union_res;
    FIND_IF(WatchPEMap, m_all_watch_pes, iter, peid) {
        // peid already exist
        // add watching_peid to ownership
        SET_UNION(iter->second->watching_pes, watching_peids, union_res);
        iter->second->watching_pes = union_res;
    } else {
        // add new pe watch
        ProcessorElement t_pe;
        std::string pn_addr = "";
        if ((err_ret = GetProcessorElement(peid, &t_pe)) < error::OK) {
            DSTREAM_WARN("[%s] fail to get ProcessorElement, peid(%lu), errno(%d), err(%s)",
                         kAppRouterTrace, peid, err_ret, ErrStr(err_ret));
            // if (err_ret == error::ZK_NO_NODE) {
            //   return err_ret;
            // }
            // else if get pe fail, we set pn_addr to nil
        } else {
            // fetch ProcessorElement success
            if ((err_ret = GetPNAddr(t_pe, &pn_addr)) < error::OK) {
                DSTREAM_ERROR("[%s] fail to get pe(%lu)'s pn_addr, errno(%d), err(%s)",
                              kAppRouterTrace, peid, err_ret, ErrStr(err_ret));
            }
        }
        DSTREAM_INFO("[%s] get pe(%lu)'s pn_addr(%s)",
                     kAppRouterTrace, peid, pn_addr.c_str());
        // allocate new AppPE structure
        AppPE* t_apppe_ptr = new (std::nothrow) AppPE();
        UNLIKELY_IF(NULL == t_apppe_ptr) {
            return error::BAD_MEMORY_ALLOCATION;
        }
        // insert new item to m_all_watch_pes
        t_apppe_ptr->app_pe = t_pe;
        t_apppe_ptr->pn_addr = pn_addr;
        t_apppe_ptr->watching_pes = watching_peids;
        std::pair<WatchPEMap::iterator, bool> ret =
            m_all_watch_pes.insert(WatchPEPair(peid, t_apppe_ptr));
        UNLIKELY_IF(!ret.second) {
            // insert fail
            delete t_apppe_ptr;
            t_apppe_ptr = NULL;
            return error::DATA_CORRUPTED;
        }
    }
    return error::OK;
}

error::Code AppRouter::DelWatchPE_L(uint64_t peid, uint64_t watching_peid) {
    // check peid exist?
    FIND_IF(WatchPEMap, m_all_watch_pes, iter, peid) {
        // peid exist
        // remove watching pe
        CONST_FIND_IF(PESet, iter->second->watching_pes, watch_pe_it, watching_peid) {
            iter->second->watching_pes.erase(watch_pe_it);
            if (iter->second->watching_pes.size() == 0) {
                delete iter->second;
                iter->second = NULL;
                m_all_watch_pes.erase(iter);
            }
        } else {
            DSTREAM_WARN("[%s] fail to find watching_peid(%lu) in m_all_watch_pes[%lu]",
                         kAppRouterTrace, watching_peid, peid);
        }
    } else {
        DSTREAM_WARN("[%s] fail to find peid(%lu) in m_all_watch_pes",
                     kAppRouterTrace, peid);
    }
    return error::OK;
}

error::Code AppRouter::GetWatchDownProcessor_L(const ProcessorMap& all_processor,
        WPMap& watch_down_processors) {
    watch_down_processors.clear();
    CONST_FOR_EACH(PESet, m_local_pes, pe_it) {
        const uint64_t& pe_id = *pe_it;
        const uint64_t& pro_id = EXTRACT_PROCESSOR_ID(pe_id);
        // DSTREAM_DEBUG("[%s] watch processor id(%lu)", kAppRouterTrace, pro_id);
        CONST_FIND_IF(ProcessorMap, all_processor, pro_it, pro_id) {
            // found processor in new_all_processor
            // foreach down_processor
            CONST_FOR_EACH(ProcessorSet, pro_it->second->down_processor, p_iter) {
                const uint64_t down_pro_id = *p_iter;
                // test if exist in watch_down_processors
                FIND_IF(WPMap, watch_down_processors, wp_it, down_pro_id) {
                    // already exist
                    std::pair<PESet::iterator, bool> ret = wp_it->second.insert(pe_id);
                    UNLIKELY_IF(!ret.second) {
                        DSTREAM_ERROR("[%s] fail to insert pe(%lu) to watching_pes"
                                "of watch_down_processors", kAppRouterTrace, pe_id);
                    }
                } else {
                    // not exist
                    PESet t_watching_pes;
                    t_watching_pes.insert(pe_id);
                    std::pair<WPMap::iterator, bool> p_ret =
                        watch_down_processors.insert(WPPair(down_pro_id, t_watching_pes));
                    UNLIKELY_IF(!p_ret.second) {
                        DSTREAM_ERROR("[%s] fail to insert pair<processor(%lu), set_pe(%lu)>"
                                "into watch_down_processors", kAppRouterTrace, down_pro_id, pe_id);
                    }
                }
            }
        } else {
            DSTREAM_WARN("[%s] fail to find processor_id(%lu) in all_processor",
                         kAppRouterTrace, pro_id);
        }
    }
    return error::OK;
}

error::Code AppRouter::GetWatchUpProcessor_L(const ProcessorMap& all_processor,
        WPMap& watch_up_processors) {
    watch_up_processors.clear();
    CONST_FOR_EACH(PESet, m_local_pes, pe_it) {
        const uint64_t& pe_id = *pe_it;
        const uint64_t& pro_id = EXTRACT_PROCESSOR_ID(pe_id);
        // DSTREAM_DEBUG("[%s] watch processor id(%lu)", kAppRouterTrace, pro_id);
        CONST_FIND_IF(ProcessorMap, all_processor, pro_it, pro_id) {
            // found processor in new_all_processor
            // foreach down_processor
            CONST_FOR_EACH(ProcessorSet, pro_it->second->up_processor, p_iter) {
                const uint64_t up_pro_id = *p_iter;
                // test if exist in watch_up_processors
                FIND_IF(WPMap, watch_up_processors, wp_it, up_pro_id) {
                    // already exist
                    std::pair<PESet::iterator, bool> ret = wp_it->second.insert(pe_id);
                    UNLIKELY_IF(!ret.second) {
                        DSTREAM_ERROR("[%s] fail to insert pe(%lu) to watching_pes of "
                                "watch_up_processors", kAppRouterTrace, pe_id);
                    }
                } else {
                    // not exist
                    PESet t_watching_pes;
                    t_watching_pes.insert(pe_id);
                    std::pair<WPMap::iterator, bool> p_ret = watch_up_processors.insert(
                            WPPair(up_pro_id, t_watching_pes));
                    UNLIKELY_IF(!p_ret.second) {
                        DSTREAM_ERROR("[%s] fail to insert pair<processor(%lu), set_pe(%lu)> "
                                "into watch_up_processors", kAppRouterTrace, up_pro_id, pe_id);
                    }
                }
            }
        } else {
            DSTREAM_WARN("[%s] fail to find processor_id(%lu) in all_processor",
                    kAppRouterTrace, pro_id);
        }
    }
    return error::OK;
}

void AppRouter::GetProcessorPEIDs(uint64_t app_id,
        uint64_t pro_id,
        int paral_begin,
        int paral_num,
        PESet& pes) {
    uint64_t peid_prefix = (app_id << 40) | (pro_id << 32);
    pes.clear();
    for (int i = paral_begin; i < paral_begin + paral_num; ++i) {
        pes.insert(peid_prefix | (i << 8));
    }
}

void AppRouter::GetProcessorPEIDs(uint64_t app_id,
        uint64_t pro_id,
        int parallel,
        PESet& pes) {
    GetProcessorPEIDs(app_id, pro_id, 0, parallel, pes);
}

error::Code AppRouter::FlowCtrlApplication_L(const Application& /*app*/) {
    return error::OK;
}

error::Code AppRouter::ParseProcessors(const Application& app,
        ProcessorMap& res_map) {
    // foreach processor in app {
    //   new AppProcessor
    //   add processor to t_map <processor_name, AppProcessorPtr>
    // }
    // clear res_map
    // foreach processor in app {
    //   foreach subscribe in processor {
    //     add processor to t_map[subscribe].down_processor
    //     add subscribe to t_map[processor].up_processor
    //   }
    // }
    // foreach processor in t_map {
    //   res_map[processor.id] = t_map[processor.name]
    // }
    UNLIKELY_IF(!app.has_topology()) {
        DSTREAM_ERROR("[%s] app has no topology!", kAppRouterTrace);
        return error::BAD_ARGUMENT;
    }
    const Topology& topo = app.topology();
    typedef std::map<std::string, AppProcessor*> NameProMap;
    typedef std::pair<std::string, AppProcessor*> NameProPair;
    NameProMap t_map;
    for (int i = 0; i < topo.processors_size(); ++i) {
        AppProcessor* t_app_pro_ptr = new (std::nothrow) AppProcessor();
        UNLIKELY_IF(NULL == t_app_pro_ptr) {
            return error::BAD_MEMORY_ALLOCATION;
        }
        t_app_pro_ptr->cur_pro = topo.processors(i);
        UNLIKELY_IF(!t_app_pro_ptr->cur_pro.has_name()) {
            DSTREAM_ERROR("[%s] processor has no name!", kAppRouterTrace);
            continue;
        }
        // we assume there is no duplicate processor with same name
        std::pair<NameProMap::iterator, bool> ret = t_map.insert(
                NameProPair(t_app_pro_ptr->cur_pro.name(), t_app_pro_ptr));
        UNLIKELY_IF(!ret.second) {
            DSTREAM_ERROR("[%s] fail to insert %s into NameProMap",
                    kAppRouterTrace, t_app_pro_ptr->cur_pro.name().c_str());
            continue;
        }
    }
    for (int i = 0; i < topo.processors_size(); ++i) {
        const Processor& cur_pro = topo.processors(i);
        if (cur_pro.has_subscribe()) { // has sub
            const Subscribe& sub = cur_pro.subscribe();
            for (int j = 0; j < sub.processor_size(); ++j) {
                std::string sub_pro_name = sub.processor(j).name();
                CONST_FIND_IF(NameProMap, t_map, t_map_it, sub_pro_name) {
                    // add processor to t_map[subscribe].down_processor
                    t_map[sub_pro_name]->down_processor.insert(cur_pro.id().id());
                    // add tags to t_map[processor].sub_tags
                    uint64_t sub_pro_id = t_map[sub_pro_name]->cur_pro.id().id();
                    t_map[cur_pro.name()]->sub_tags[sub_pro_id].insert(sub.processor(j).tags());
                    // add subscribe to t_map[processor].up_processor
                    t_map[cur_pro.name()]->up_processor.insert(sub_pro_id);
                    // TODO(bijia): need check insert fail?
                } else {
                    DSTREAM_ERROR("[%s] invalid subscribe processor(%s)!",
                            kAppRouterTrace, sub.processor(j).name().c_str());
                    return error::BAD_ARGUMENT;
                }
            }
        }
    }
    // output
    res_map.clear();
    NameProMap::iterator iter;
    for (iter = t_map.begin(); iter != t_map.end(); ++iter) {
        res_map[iter->second->cur_pro.id().id()] = iter->second;
    }
    return error::OK;
}

void AppRouter::DestroyProcssorMap(ProcessorMap* pro_map) {
    ProcessorMap::iterator iter;
    for (iter = pro_map->begin(); iter != pro_map->end(); ++iter) {
        if (iter->second) {
            delete iter->second;
            iter->second = NULL;
        }
    }
    pro_map->clear();
    DSTREAM_DEBUG("[%s] Destroy ProcessorMap, appid(%lu).",
                  kAppRouterTrace, m_app_id);
}

void AppRouter::DestroyWatchPEMap(WatchPEMap* t_map) {
    WatchPEMap::iterator iter;
    for (iter = t_map->begin(); iter != t_map->end(); ++iter) {
        if (NULL != iter->second) {
            delete iter->second;
            iter->second = NULL;
        }
    }
    t_map->clear();
}

error::Code AppRouter::GetAllDiffProcessor_L(const ProcessorMap& new_all_processor,
        std::set<uint64_t>& add_pro,
        std::set<uint64_t>& del_pro,
        std::set<uint64_t>& update_pro) {
    // clear all
    add_pro.clear();
    del_pro.clear();
    update_pro.clear();

    // foreach new_all_processor as new_processor {
    //   if new_processor isn't in m_all_processor {
    //     add new_processor to add_pro
    //   } else if (new_processor diff with old_processor) {
    //     add new_processor to update_pro
    //   }
    // }
    CONST_FOR_EACH(ProcessorMap, new_all_processor, new_pro_it) {
        CONST_NOT_FIND_IF(ProcessorMap, m_all_processor, old_pro_it, new_pro_it->first) {
            // new_pro_it isn't in m_all_processor
            add_pro.insert(new_pro_it->first);
        } else {
            if (DiffProcessor(*new_pro_it->second, *old_pro_it->second)) {
                update_pro.insert(new_pro_it->first);
            }
        }
    }
    // foreach m_all_processor as old_processor {
    //   if (old_processor isn't in new_processor) {
    //     add old_processor to del_pro
    //   }
    // }
    CONST_FOR_EACH(ProcessorMap, m_all_processor, old_pro_it) {
        CONST_NOT_FIND_IF(ProcessorMap, new_all_processor, new_pro_it, old_pro_it->first) {
            // old_pro_it isn't in new_all_processor
            del_pro.insert(old_pro_it->first);
        }
    }

    DSTREAM_DEBUG("[%s] add_pro(%zu), del_pro(%zu), update_pro(%zu)",
                  kAppRouterTrace, add_pro.size(), del_pro.size(), update_pro.size());
    return error::OK;
}

bool AppRouter::DiffProcessor(const AppProcessor& new_app_pro,
        const AppProcessor& old_app_pro,
        PESet* add_pes,
        PESet* del_pes) {
    const Processor& new_pro = new_app_pro.cur_pro;
    const Processor& old_pro = old_app_pro.cur_pro;
    // name & processor id
    if (new_pro.name() != old_pro.name()
        || new_pro.id().id() != old_pro.id().id()) {
        DSTREAM_ERROR("[%s] you can't diff between new_pro(%s, %lu) & old_pro(%s, %lu)",
                      kAppRouterTrace, new_pro.name().c_str(), new_pro.id().id(),
                      old_pro.name().c_str(), old_pro.id().id());
        return true;
    }
    bool ret = false;
    // parallism
    if (new_pro.parallism() > old_pro.parallism()) {
        if (add_pes) {
            GetProcessorPEIDs(m_app_id, new_pro.id().id(),
                              old_pro.parallism(),
                              new_pro.parallism() - old_pro.parallism(),
                              *add_pes);
        }
        ret = true;
    } else if (new_pro.parallism() < old_pro.parallism()) {
        if (del_pes) {
            GetProcessorPEIDs(m_app_id, new_pro.id().id(),
                              new_pro.parallism(),
                              old_pro.parallism() - new_pro.parallism(),
                              *del_pes);
        }
        ret = true;
    } // else old parallism == new parallism

    // tags
    if (new_app_pro.sub_tags.size() != old_app_pro.sub_tags.size()) {
        ret = true;
    } else {
        // tags num equal
        ProcessorSet p_set = uniq_map_key<uint64_t, TagSet >(new_app_pro.sub_tags,
                old_app_pro.sub_tags);
        if (p_set.size() != new_app_pro.sub_tags.size()
            || p_set.size() != old_app_pro.sub_tags.size()) {
            ret = true;
        } else {
            CONST_FOR_EACH(ProcessorSet, p_set, tag_iter) {
                SubTagMap::const_iterator new_pro_iter = new_app_pro.sub_tags.find(*tag_iter);
                SubTagMap::const_iterator old_pro_iter = old_app_pro.sub_tags.find(*tag_iter);
                if (new_pro_iter == new_app_pro.sub_tags.end()
                    || old_pro_iter == old_app_pro.sub_tags.end()) {
                    ret = true;
                } else {
                    TagSet tag_set;
                    SET_UNION(new_pro_iter->second, old_pro_iter->second, tag_set);
                    if (tag_set.size() != old_pro_iter->second.size()
                        || tag_set.size() != new_pro_iter->second.size()) {
                        ret = true;
                    }
                }
            }
        }
    }

    // flow control
    if (new_pro.flow_control().qps_limit() != old_pro.flow_control().qps_limit()) {
        ret = true;
    }
    return ret;
}

error::Code AppRouter::GetUpdateChange_L(const ProcessorMap& new_all_processor,
        const ProcessorSet& update_pro,
        const WPMap& all_watch_up_pro,
        const WPMap& all_watch_down_pro,
        ProcessorSet& update_watch_pro,
        ProcessorSet& update_flow_control_pro,
        PESet& add_watch_pe,
        PESet& del_watch_pe) {
    // down-stream tag or parallel change
    // foreach update_pro as pro {
    //   if pro in all_watch_down_pro {
    //     add pro to update_watch_pro
    //   }
    // }
    // up-stream parallel change
    // foreach update_pro as pro {
    //   if pro in all_watch_up_pro {
    //     diff pro with m_all_processor[pro]
    //     add new_pe to add_watch_pe
    //     add del_pe to del_watch_pe
    //   }
    // }
    CONST_FOR_EACH(ProcessorSet, update_pro, update_iter) {
        const uint64_t update_pro_id = *update_iter;
        // down-stream tag or parallel change
        CONST_FIND_IF(WPMap, all_watch_down_pro, down_watch_iter, update_pro_id) {
            // update_iter in all_watch_down_pro
            update_watch_pro.insert(update_pro_id);
        }
        // up-stream parallel change
        CONST_FIND_IF(WPMap, all_watch_up_pro, up_watch_iter, update_pro_id) {
            // update_iter in all_watch_up_pro
            PESet add_pes, del_pes, res_pes;
            CONST_NOT_FIND_IF(ProcessorMap, new_all_processor, new_pro_iter, update_pro_id) {
                DSTREAM_ERROR("[%s] cannot find property processor pro_id(%lu) "
                        "in new_all_processor", kAppRouterTrace, update_pro_id);
                continue;
            }
            CONST_NOT_FIND_IF(ProcessorMap, m_all_processor, old_pro_iter, update_pro_id) {
                DSTREAM_ERROR("[%s] cannot find property processor pro_id(%lu) "
                        "in old_all_processor", kAppRouterTrace, update_pro_id);
                continue;
            }
            DiffProcessor(*new_pro_iter->second, *old_pro_iter->second, &add_pes, &del_pes);
            // TODO(bijia): check fail
            // add new_pe to add_watch_pe
            SET_UNION(add_pes, add_watch_pe, res_pes);
            add_watch_pe = res_pes;
            res_pes.clear();
            // add del_pe to del_watch_pe
            SET_UNION(del_pes, del_watch_pe, res_pes);
            del_watch_pe = res_pes;
        }

        // flow control change
        CONST_FIND_IF(ProcessorMap, m_all_processor, tmp_iter, update_pro_id) {
            update_flow_control_pro.insert(update_pro_id);
        }
    }
    return error::OK;
}

error::Code AppRouter::GetAddChange(const ProcessorMap& new_all_processor,
        const ProcessorSet& add_pro,
        const WPMap& all_watch_up_pro,
        const WPMap& all_watch_down_pro,
        ProcessorSet& add_watch_pro,
        PESet& add_watch_pe) {
    // up-down-stream add & del pro
    // foreach add_pro as pro {
    //   if pro in all_watch_down_pro {
    //     add pro to add_watch_pro
    //   }
    //   if pro in all_watch_up_pro {
    //     add pro's all pe to add_watch_pe
    //   }
    // }
    CONST_FOR_EACH(ProcessorSet, add_pro, add_pro_iter) {
        const uint64_t add_pro_id = *add_pro_iter;
        CONST_FIND_IF(WPMap, all_watch_down_pro, down_pro_iter, add_pro_id) {
            // add pro to add_watch_pro
            add_watch_pro.insert(add_pro_id);
        }
        CONST_FIND_IF(WPMap, all_watch_up_pro, up_pro_iter, add_pro_id) {
            // add pro's all pe to add_watch_pe
            CONST_NOT_FIND_IF(ProcessorMap, new_all_processor, new_pro_iter, add_pro_id) {
                DSTREAM_ERROR("[%s] cannot find property processor pro_id(%lu) "
                        "in new_all_processor", kAppRouterTrace, add_pro_id);
                continue;
            }
            PESet pe_list;
            GetProcessorPEIDs(m_app_id,
                    *add_pro_iter, new_pro_iter->second->cur_pro.parallism(), pe_list);
            CONST_FOR_EACH(PESet, pe_list, pe_it) {
                add_watch_pe.insert(*pe_it);
            }
        }
    }
    return error::OK;
}

error::Code AppRouter::GetDelChange(const ProcessorMap& old_all_processor,
        const ProcessorSet& del_pro,
        const WPMap& old_all_watch_up_pro,
        const WPMap& old_all_watch_down_pro,
        ProcessorSet& del_watch_pro,
        PESet& del_watch_pe) {
    // up-down-stream del pro
    // foreach del_pro as pro {
    //   if pro in all_watch_down_pro {
    //     add pro to del_watch_pro
    //   }
    //   if pro in all_watch_up_pro {
    //     add pro's all pe to del_watch_pe
    //   }
    // }
    CONST_FOR_EACH(ProcessorSet, del_pro, del_pro_iter) {
        const uint64_t del_pro_id = *del_pro_iter;
        CONST_FIND_IF(WPMap, old_all_watch_down_pro, down_pro_iter, del_pro_id) {
            // add pro to del_watch_pro
            del_watch_pro.insert(del_pro_id);
        }
        CONST_FIND_IF(WPMap, old_all_watch_up_pro, up_pro_iter, del_pro_id) {
            // add pro's all pe to del_watch_pe
            // del_pro only exist in old m_all_processor
            CONST_NOT_FIND_IF(ProcessorMap, old_all_processor, old_pro_iter, del_pro_id) {
                DSTREAM_ERROR("[%s] cannot find property processor pro_id(%lu) "
                        "in old_all_processor", kAppRouterTrace, del_pro_id);
                continue;
            }
            PESet pe_list;
            GetProcessorPEIDs(m_app_id,
                    *del_pro_iter, old_pro_iter->second->cur_pro.parallism(), pe_list);
            CONST_FOR_EACH(PESet, pe_list, pe_it) {
                del_watch_pe.insert(*pe_it);
            }
        }
    }
    return error::OK;
}

error::Code AppRouter::NotifyLocalPEAdd_L(const uint64_t pe_id) {
    // notify all change according to add local peid
    // TODO(bijia)
    PESet notify_pes;
    notify_pes.insert(pe_id);
    const uint64_t pro_id = EXTRACT_PROCESSOR_ID(pe_id);
    // DSTREAM_DEBUG("[%s] watch processor id(%lu)", kAppRouterTrace, pro_id);
    CONST_FIND_IF(ProcessorMap, m_all_processor, pro_it, pro_id) {
        // found processor in m_all_processor
        // foreach up_processor, notify pe_id
        CONST_FOR_EACH(ProcessorSet, pro_it->second->up_processor, p_iter) {
            const uint64_t up_pro_id = *p_iter;
            CONST_FIND_IF(ProcessorMap, m_all_processor, up_pro_it, up_pro_id) {
                PESet all_up_pes;
                GetProcessorPEIDs(m_app_id, up_pro_id,
                                  up_pro_it->second->cur_pro.parallism(),
                                  all_up_pes);
                CONST_FOR_EACH(PESet, all_up_pes, pe_it) {
                    NotifySingleAddUpPE(*pe_it, notify_pes);
                }
            } else {
                DSTREAM_WARN("[%s] fail to find processor_id(%lu) in all_processor",
                             kAppRouterTrace, up_pro_id);
            }
        }
        // foreach down_processor, notify pe_id
        CONST_FOR_EACH(ProcessorSet, pro_it->second->down_processor, p_iter1) {
            const uint64_t down_pro_id = *p_iter1;
            CONST_FIND_IF(ProcessorMap, m_all_processor, down_pro_it, down_pro_id) {
                NotifySingleAddDownProcessor(down_pro_id, notify_pes, m_all_processor);
            }
        }
    } else {
        DSTREAM_WARN("[%s] fail to find processor_id(%lu) in all_processor",
                     kAppRouterTrace, pro_id);
    }

    return error::OK;
}


error::Code AppRouter::NotifyUpdateAction(const ProcessorMap& new_all_processor,
        const ProcessorSet& update_watch_pro,
        const WPMap& all_watch_down_pro) {
    // notify update action
    // foreach update_watch_pro as pro {
    //   OnUpdateDownProcessor
    // }
    CONST_FOR_EACH(ProcessorSet, update_watch_pro, tmp_iter) {
        const uint64_t update_watch_pro_id = *tmp_iter;
        CONST_NOT_FIND_IF(WPMap, all_watch_down_pro, down_pro_iter, update_watch_pro_id) {
            DSTREAM_ERROR("[%s] cannot found update_watch_pro_id(%lu) in all_watch_down_pro",
                          kAppRouterTrace, update_watch_pro_id);
            continue;
        }
        ProcessorMap::const_iterator new_pro_iter = new_all_processor.find(update_watch_pro_id);
        ProcessorMap::const_iterator old_pro_iter = m_all_processor.find(update_watch_pro_id);
        if (new_all_processor.end() != new_pro_iter
            && m_all_processor.end() != old_pro_iter) {
            const ProcessorSet& who_notify = down_pro_iter->second;
            CONST_FOR_EACH(ProcessorSet, who_notify, who_notify_iter) {
                // should diff change & make sure need notify
                const uint64_t& notify_pro_id = EXTRACT_PROCESSOR_ID(*who_notify_iter);
                if (CheckNeedNotify(notify_pro_id, *new_pro_iter->second, *old_pro_iter->second)) {
                    CONST_NOT_FIND_IF(SubTagMap,
                            new_pro_iter->second->sub_tags, tag_iter, notify_pro_id) {
                        DSTREAM_ERROR("[%s] cannot found notify_pro_id(%lu) in SubTagMap",
                                kAppRouterTrace, notify_pro_id);
                        continue;
                    }
                    ProcessorInfo ctx(
                            MAKE_APP_PROCESSOR_ID(m_app_id, update_watch_pro_id),
                            new_pro_iter->second->cur_pro.parallism(),
                            tag_iter->second);
                    NotifyFunc(UPDATE_DOWN_PRO,
                            *who_notify_iter/*notify peid*/,
                            &ctx);
                }
            }
        }
    }
    return error::OK;
}

bool AppRouter::CheckNeedNotify(uint64_t notify_proid,
        const AppProcessor& new_pro,
        const AppProcessor& old_pro) {
    bool ret = false;
    if (new_pro.cur_pro.parallism() != old_pro.cur_pro.parallism()) {
        ret = true;
    } else {
        // diff tag
        CONST_NOT_FIND_IF(SubTagMap, new_pro.sub_tags, new_tags_iter, notify_proid) {
            DSTREAM_WARN("[%s] cannot find proid(%lu)'s tags",
                         kAppRouterTrace, notify_proid);
            return false;
        }
        CONST_NOT_FIND_IF(SubTagMap, old_pro.sub_tags, old_tags_iter, notify_proid) {
            DSTREAM_WARN("[%s] cannot find proid(%lu)'s tags",
                         kAppRouterTrace, notify_proid);
            return false;
        }
        TagSet tag_set;
        SET_UNION(new_tags_iter->second, old_tags_iter->second, tag_set);
        if (tag_set.size() != new_tags_iter->second.size()
            || tag_set.size() != old_tags_iter->second.size()) {
            ret = true;
        }
    }
    return ret;
}

// void AppRouter::DumpSet(const ProcessorSet& s) {
//   CONST_FOR_EACH(ProcessorSet, s, iter) {
//     DSTREAM_WARN("set contain (%lu)", *iter);
//   }
// }

error::Code AppRouter::NotifyUpAddAction(const WPMap& all_watch_up_pro,
        const PESet& add_watch_pe) {
    error::Code err_ret = error::OK;
    // notify add action
    // foreach add_watch_pe as pe {
    //   AddWatchPE_L(pe)
    //   OnAddUpPE
    // }
    CONST_FOR_EACH(PESet, add_watch_pe, add_watch_pe_it) {
        const uint64_t& add_watch_pro_id = EXTRACT_PROCESSOR_ID(*add_watch_pe_it);
        CONST_NOT_FIND_IF(WPMap, all_watch_up_pro, add_watch_pro_it, add_watch_pro_id) {
            DSTREAM_ERROR("[%s] cannot found add_watch_pro_id(%lu) in all_watch_up_pro",
                          kAppRouterTrace, add_watch_pro_id);
            continue;
        }
        // find local pes from add_watch_pro_it->second as who_notify
        const PESet& who_notify = add_watch_pro_it->second;
        const uint64_t& add_watch_pe_id = *add_watch_pe_it;
        err_ret = NotifySingleAddUpPE(add_watch_pe_id, who_notify);
    }
    return err_ret;
}

error::Code AppRouter::NotifySingleAddUpPE(const uint64_t add_pe_id, const PESet& who_notify) {
    // we call lock-needed function
    error::Code err_ret = AddWatchPE_L(add_pe_id, who_notify);
    UNLIKELY_IF(err_ret < error::OK) {
        // TODO(bijia): add retry while watch fail, except ZK_NO_NODE error
        DSTREAM_ERROR("[%s] fail to add watch to pe(%lu), error_no(%d)",
                      kAppRouterTrace, add_pe_id, err_ret);
    } else {
        CONST_FOR_EACH(PESet, who_notify, who_notify_iter) {
            CONST_NOT_FIND_IF(WatchPEMap, m_all_watch_pes, addr_iter, add_pe_id) {
                DSTREAM_ERROR("[%s] cannot found add_pe_id(%lu) in m_all_watch_pes",
                        kAppRouterTrace, add_pe_id);
                continue;
            }
            PEInfo ctx(add_pe_id, addr_iter->second->pn_addr);
            NotifyFunc(ADD_UP_PE, *who_notify_iter, &ctx);
        }
    }
    return err_ret;
}

error::Code AppRouter::NotifyUpDelAction(const WPMap& all_watch_up_pro,
        const PESet& del_watch_pe) {
    // notify del action
    // foreach del_watch_pe as pe {
    //   OnDelUpPE
    //   DelWatchPE_L(pe)
    // }
    CONST_FOR_EACH(PESet, del_watch_pe, del_watch_pe_it) {
        const uint64_t& del_watch_pro_id = EXTRACT_PROCESSOR_ID(*del_watch_pe_it);
        CONST_NOT_FIND_IF(WPMap, all_watch_up_pro, del_watch_pro_it, del_watch_pro_id) {
            DSTREAM_ERROR("[%s] cannot found del_watch_pro_id(%lu) in all_watch_up_pro",
                    kAppRouterTrace, del_watch_pro_id);
            continue;
        }
        const PESet& who_notify = del_watch_pro_it->second;
        CONST_FOR_EACH(PESet, who_notify, who_notify_iter) {
            PEInfo ctx(*del_watch_pe_it, "");
            NotifyFunc(DEL_UP_PE, *who_notify_iter, &ctx);
            // OnDelUpPE(*who_notify_iter, *del_watch_pe_it);
            DelWatchPE_L(*del_watch_pe_it, *who_notify_iter);
            // TODO(bijia): check fail
        }
    }
    return error::OK;
}

error::Code AppRouter::NotifyFlowControlAction(const ProcessorMap& new_all_processor,
        const ProcessorSet& update_flow_control_pro) {
    CONST_FOR_EACH(ProcessorSet, update_flow_control_pro, tmp_iter) {
        CONST_FIND_IF(ProcessorMap, new_all_processor, pro_iter, *tmp_iter) {
            CtrlInfo ctx(pro_iter->second->cur_pro.flow_control());
            CONST_FOR_EACH(PESet, m_local_pes, pe_it) {
                if (EXTRACT_PROCESSOR_ID(*pe_it) == pro_iter->first) {
                    NotifyFunc(APP_FLOW_CTRL, *pe_it, &ctx);
                    DSTREAM_INFO("[%s ]update flowcontrol for pe %lu", kAppRouterTrace, *pe_it);
                }
            }
        }
    }
    return error::OK;
}

error::Code AppRouter::NotifyDownAddAction(const ProcessorMap& new_all_processor,
        const WPMap& all_watch_down_pro,
        const ProcessorSet& add_watch_pro) {
    CONST_FOR_EACH(ProcessorSet, add_watch_pro, add_watch_pro_it) {
        const uint64_t& add_watch_pro_id = *add_watch_pro_it;
        CONST_NOT_FIND_IF(WPMap, all_watch_down_pro, exist_iter, add_watch_pro_id) {
            DSTREAM_ERROR("[%s] cannot found add_watch_pro_id(%lu) in all_watch_down_pro",
                          kAppRouterTrace, add_watch_pro_id);
            continue;
        }
        const PESet& who_notify = exist_iter->second;
        NotifySingleAddDownProcessor(add_watch_pro_id, who_notify, new_all_processor);
    }
    return error::OK;
}

error::Code AppRouter::NotifySingleAddDownProcessor(
        const uint64_t add_pro_id,
        const PESet& who_notify,
        const ProcessorMap& all_processor) {
    CONST_FOR_EACH(PESet, who_notify, who_notify_iter) {
        CONST_FIND_IF(ProcessorMap, all_processor, pro_iter, add_pro_id) {
            const uint64_t& notify_pro_id = EXTRACT_PROCESSOR_ID(*who_notify_iter);
            CONST_NOT_FIND_IF(SubTagMap, pro_iter->second->sub_tags, tag_iter, notify_pro_id) {
                DSTREAM_ERROR("[%s] cannot found notify_pro_id(%lu) in SubTagMap",
                        kAppRouterTrace, notify_pro_id);
                continue;
            }
            ProcessorInfo ctx(MAKE_APP_PROCESSOR_ID(m_app_id, add_pro_id),
                    pro_iter->second->cur_pro.parallism(), tag_iter->second);
            NotifyFunc(ADD_DOWN_PRO, *who_notify_iter, &ctx);
        }
    }
    return error::OK;
}

error::Code AppRouter::NotifyDownDelAction(const WPMap& old_all_watch_down_pro,
        const ProcessorSet& del_watch_pro) {
    // foreach del_watch_pro as pro {
    //   OnDelDownProcessor
    // }
    CONST_FOR_EACH(ProcessorSet, del_watch_pro, del_watch_pro_it) {
        const uint64_t& del_watch_pro_id = *del_watch_pro_it;
        CONST_NOT_FIND_IF(WPMap, old_all_watch_down_pro, exist_iter, del_watch_pro_id) {
            DSTREAM_ERROR("[%s] cannot found del_watch_pro_id(%lu) in old_all_watch_down_pro",
                          kAppRouterTrace, del_watch_pro_id);
            continue;
        }
        const PESet& who_notify = exist_iter->second;
        CONST_FOR_EACH(PESet, who_notify, who_notify_iter) {
            TagSet t; // empty set
            ProcessorInfo ctx(MAKE_APP_PROCESSOR_ID(m_app_id, del_watch_pro_id), 0, t);
            NotifyFunc(DEL_DOWN_PRO, *who_notify_iter, &ctx);
        }
    }
    return error::OK;
}

error::Code AppRouter::DiffApplication_L(const ProcessorMap& new_all_processor) {
    error::Code err_ret = error::OK;
    // all-change: add_pro, del_pro, update_pro
    ProcessorSet add_pro, del_pro, update_pro;
    // up-stream: add_watch_pe, del_watch_pe
    PESet add_watch_pe, del_watch_pe;
    // down-stream: add_watch_pro, del_watch_pro, update_watch_pro
    ProcessorSet add_watch_pro, del_watch_pro, update_watch_pro, update_flow_control_pro;

    UNLIKELY_IF((err_ret = GetAllDiffProcessor_L(new_all_processor,
                                                 add_pro, del_pro, update_pro)) < error::OK) {
        DSTREAM_ERROR("[%s] fail to get diff processors, app_id(%lu)",
                      kAppRouterTrace, m_app_id);
        return err_ret;
    }

    // map<ProcessorID, set<peid> watch_owner>
    WPMap all_watch_up_pro, all_watch_down_pro;
    err_ret = GetWatchUpProcessor_L(new_all_processor, all_watch_up_pro);
    UNLIKELY_IF(err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get watching up processor, app_id(%lu)",
                      kAppRouterTrace, m_app_id);
        return err_ret;
    }
    err_ret = GetWatchDownProcessor_L(new_all_processor, all_watch_down_pro);
    UNLIKELY_IF(err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get watching down processor, app_id(%lu)",
                      kAppRouterTrace, m_app_id);
        return err_ret;
    }
    DSTREAM_DEBUG("[%s] get all watching processors, new_up_pro(%zu), new_down_pro(%zu)",
                  kAppRouterTrace, all_watch_up_pro.size(), all_watch_down_pro.size());

    WPMap old_all_watch_up_pro, old_all_watch_down_pro;
    err_ret = GetWatchUpProcessor_L(m_all_processor, old_all_watch_up_pro);
    UNLIKELY_IF(err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get old watching up processor, app_id(%lu)",
                      kAppRouterTrace, m_app_id);
        return err_ret;
    }
    err_ret = GetWatchDownProcessor_L(m_all_processor, old_all_watch_down_pro);
    UNLIKELY_IF(err_ret < error::OK) {
        DSTREAM_ERROR("[%s] fail to get old watching down processor, app_id(%lu)",
                      kAppRouterTrace, m_app_id);
        return err_ret;
    }
    DSTREAM_DEBUG("[%s] get all watching processors, old_up_pro(%zu), old_down_pro(%zu)",
                  kAppRouterTrace, old_all_watch_up_pro.size(), old_all_watch_down_pro.size());
    // update
    // =========================================
    GetUpdateChange_L(new_all_processor, update_pro,
                      all_watch_up_pro, all_watch_down_pro,
                      update_watch_pro, update_flow_control_pro,
                      add_watch_pe, del_watch_pe);
    // add & del
    // =========================================
    GetAddChange(new_all_processor, add_pro,
                 all_watch_up_pro, all_watch_down_pro,
                 add_watch_pro, add_watch_pe);
    GetDelChange(m_all_processor, del_pro,
                 old_all_watch_up_pro, old_all_watch_down_pro,
                 del_watch_pro, del_watch_pe);

    // do action actually
    NotifyUpdateAction(new_all_processor, update_watch_pro, all_watch_down_pro);
    NotifyUpAddAction(all_watch_up_pro, add_watch_pe);
    NotifyUpDelAction(old_all_watch_up_pro, del_watch_pe);
    NotifyDownAddAction(new_all_processor, all_watch_down_pro, add_watch_pro);
    NotifyDownDelAction(old_all_watch_down_pro, del_watch_pro);
    NotifyFlowControlAction(new_all_processor, update_flow_control_pro);

    // update m_all_processor to new_all_processor
    // merge add_pro, del_pro, update_pro to all_processor
    DestroyProcssorMap(&m_all_processor);
    m_all_processor = new_all_processor;
    return error::OK;
}

error::Code AppRouter::DiffProcessorElement_L(const ProcessorElement& new_pe) {
    // up-stream: update_pe
    // update new_pe in m_all_watch_pes
    uint64_t new_peid = new_pe.pe_id().id();
    NOT_FIND_IF(WatchPEMap, m_all_watch_pes, pe_it, new_peid) {
        DSTREAM_WARN("[%s] fail to get peid(%lu) in m_all_watch_pes, maybe deleted",
                     kAppRouterTrace, new_peid);
        return error::OK;
    }

    UNLIKELY_IF(NULL == pe_it->second) {
        DSTREAM_WARN("[%s] fail to get peid(%lu) in m_all_watch_pes, ptr(nil)",
                     kAppRouterTrace, new_peid);
        return error::DATA_CORRUPTED;
    }

    std::string pn_addr;
    if (!pe_it->second->pn_addr.empty() /* deal with no assign PN */
        && (pe_it->second->app_pe.primary_size() > 0)
        && (pe_it->second->app_pe.primary(0).id() == new_pe.primary(0).id())) {
        // pe still on the same pn
        DSTREAM_WARN("[%s] pe(%lu) still on the same pn_id(%lu)",
                     kAppRouterTrace, new_peid, new_pe.primary(0).id());
        // do nothing
        return error::OK;
    } else {
        // PE migrate, get new pn_addr
        error::Code err_ret;
        if ((err_ret = GetPNAddr(new_pe, &pn_addr)) < error::OK) {
            DSTREAM_ERROR("[%s] fail to get peid(%lu)'s pn_addr, update AppPE fail",
                          kAppRouterTrace, new_peid);
            return err_ret;
        }
        if (pn_addr.empty()) {
            DSTREAM_WARN("[%s] pe(%lu) has not been deployed",
                         kAppRouterTrace, new_peid);
        }
        pe_it->second->app_pe = new_pe;
        pe_it->second->pn_addr = pn_addr;
    }

    // call OnMigrateUpPE, notify local pes
    CONST_FOR_EACH(PESet, pe_it->second->watching_pes, who_notify_iter) {
        // pn_addr maybe empty, which means that pe has not been deployed
        PEInfo ctx(new_peid, pn_addr);
        NotifyFunc(UPDATE_UP_PE, *who_notify_iter, &ctx);
    }
    return error::OK;
}

void AppRouter::AppChildEventProcess(uint64_t event_appid) {
    error::Code err_ret;
    PESet peid_set;
    UNLIKELY_IF((err_ret = GetPEList(event_appid, &peid_set)) < error::OK) {
        DSTREAM_ERROR("[%s] fail to get App(%lu)'s pe list from zookeeper, "
                "errno(%d), err(%s)", kAppRouterTrace, event_appid, err_ret, ErrStr(err_ret));
    } else {
        // get pe list success
        // check pending watch PE event
        m_data_lock.LockForRead();
        PESet watch_pes;
        FOR_EACH(WatchPEMap, m_all_watch_pes, iter) {
            // PE has no assign PN
            UNLIKELY_IF(iter->second && iter->second->pn_addr.empty()) {
                watch_pes.insert(iter->first);
            }
        }
        m_data_lock.Unlock();
        FOR_EACH(PESet, watch_pes, pe_iter) {
            const uint64_t watch_peid = *pe_iter;
            FIND_IF(PESet, peid_set, find_iter, watch_peid) {
                // NOTICE : we ignored pe delete detect here
                //          AppPE structure in m_all_watch_pes was deleted in only follow case:
                //          update by diff app, & delete it when none watching PE focus on
                DSTREAM_DEBUG("[%s] pe(%lu) added detect", kAppRouterTrace, watch_peid);
                // look as migrate PE & notify
                PEEventProcess(watch_peid);
            }
        }
    }
}

void AppRouter::AppEventProcess(uint64_t event_appid) {
    error::Code err_ret;
    // get new Application & rewatch
    Application new_app;
    UNLIKELY_IF((err_ret = GetApplication(event_appid, &new_app)) < error::OK) {
        DSTREAM_ERROR("[%s] fail to get App(%lu) from zookeeper, errno(%d), err(%s)",
                      kAppRouterTrace, event_appid, err_ret, ErrStr(err_ret));
    } else {
        // get app success
        ProcessorMap new_processors;
        UNLIKELY_IF((err_ret = ParseProcessors(new_app, new_processors)) < error::OK) {
            DSTREAM_ERROR("[%s] fail to parse App(%lu), errno(%d), err(%s)",
                          kAppRouterTrace, event_appid, err_ret, ErrStr(err_ret));
            // gc struct
            DestroyProcssorMap(&new_processors);
        } else {
            RWLockGuard lock_guard(m_data_lock, true);
            // flow control
            err_ret = FlowCtrlApplication_L(new_app);
            UNLIKELY_IF(err_ret < error::OK) {
                DSTREAM_ERROR("[%s] fail to flow control App(%lu), errno(%d), err(%s)",
                              kAppRouterTrace, event_appid, err_ret, ErrStr(err_ret));
            }
            err_ret = DiffApplication_L(new_processors);
            UNLIKELY_IF(err_ret < error::OK) {
                DSTREAM_ERROR("[%s] fail to diff App(%lu), errno(%d), err(%s)",
                              kAppRouterTrace, event_appid, err_ret, ErrStr(err_ret));
            }
        }
    }
}

void AppRouter::PEEventProcess(uint64_t event_peid) {
    error::Code err_ret;
    // get new ProcessorElement & rewatch
    ProcessorElement new_pe;
    UNLIKELY_IF((err_ret = GetProcessorElement(event_peid, &new_pe)) < error::OK) {
        DSTREAM_ERROR("[%s] fail to get ProcessorElement(%lu) from zookeeper, errno(%d), err(%s)",
                kAppRouterTrace, event_peid, err_ret, ErrStr(err_ret));
    } else {
        // get pe success
        RWLockGuard lock_guard(m_data_lock, true);
        err_ret = DiffProcessorElement_L(new_pe);
        UNLIKELY_IF(err_ret < error::OK) {
            DSTREAM_ERROR("[%s] fail to diff ProcessorElement(%lu), errno(%d), err(%s)",
                    kAppRouterTrace, event_peid, err_ret, ErrStr(err_ret));
        }
    }
}

#ifndef ZK_CALLBACK_PARAM_FMT
#define ZK_CALLBACK_PARAM_FMT \
    "callback param type(%d) state(%d) path(%s) ctx(%p)"
#endif
#ifndef ZK_CALLBACK_PARAM
#define ZK_CALLBACK_PARAM \
    type, state, path, context
#endif

// zookeeper callback function
int AppRouter::OnZKWatchEventCallback(int type, int state, const char* path, void* context) {
    // here we can safely use app_router_ptr,
    // for deconstruct function of AppRouter will join this call back thread
    AppRouter* app_router_ptr = reinterpret_cast<AppRouter*>(context);
    UNLIKELY_IF(NULL == app_router_ptr) {
        DSTREAM_ERROR("[%s] callback context error "ZK_CALLBACK_PARAM_FMT,
                      kAppRouterTrace, ZK_CALLBACK_PARAM);
        return -1;
    }
    DSTREAM_DEBUG("[%s] get zk watch event @app_router", kAppRouterTrace);
    uint64_t event_appid = application_tools::GetAppID(path);
    if (app_router_ptr->m_app_id != event_appid) {
        // ignored event not belong to this app
        DSTREAM_DEBUG("[%s] ignored event not belong to this app(%lu) "ZK_CALLBACK_PARAM_FMT,
                      kAppRouterTrace, event_appid, ZK_CALLBACK_PARAM);
        return 0;
    }
    if (ZOO_CHANGED_EVENT == type) {
        // App / PE Node change
        uint64_t event_peid = application_tools::GetProcessElementID(path);
        if (0 == event_peid) {
            DSTREAM_DEBUG("[%s] get app event : app(%lu) "ZK_CALLBACK_PARAM_FMT,
                          kAppRouterTrace, event_appid, ZK_CALLBACK_PARAM);
            // App Node change
            app_router_ptr->AppEventProcess(event_appid);
        } else {
            DSTREAM_DEBUG("[%s] get pe event : app(%lu), pe(%lu) "ZK_CALLBACK_PARAM_FMT,
                          kAppRouterTrace, event_appid, event_peid, ZK_CALLBACK_PARAM);
            // PE Node change
            app_router_ptr->PEEventProcess(event_peid);
        }
    } else if (ZOO_DELETED_EVENT == type) {
        // App / PE Node delete
        DSTREAM_DEBUG("[%s] app(%lu) delete event ignored, "ZK_CALLBACK_PARAM_FMT,
                      kAppRouterTrace, app_router_ptr->m_app_id, ZK_CALLBACK_PARAM);
    } else if (ZOO_CHILD_EVENT == type) {
        // PE Node add or delete
        DSTREAM_DEBUG("[%s] app(%lu) child change event, "ZK_CALLBACK_PARAM_FMT,
                      kAppRouterTrace, app_router_ptr->m_app_id, ZK_CALLBACK_PARAM);
        app_router_ptr->AppChildEventProcess(event_appid);
    } else {
        DSTREAM_WARN("[%s] app(%lu) event ignored, "ZK_CALLBACK_PARAM_FMT,
                     kAppRouterTrace, app_router_ptr->m_app_id, ZK_CALLBACK_PARAM);
    }
    return 0;
}

} // namespace router
} // namespace dstream

