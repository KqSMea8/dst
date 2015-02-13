/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_PROCESSNODE_APP_ROUTER_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_APP_ROUTER_H__ // NOLINT

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <algorithm>
#include <set>
#include <string>
#include <utility>

#include "common/id.h"
#include "common/rpc_const.h"
#include "common/rwlock.h"
#include "processnode/zk_operator.h"

namespace dstream {
namespace router {

#ifndef CONST_FIND_IF
#define CONST_FIND_IF(type, vset, viter, v) type::const_iterator viter = (vset).find((v)); \
    if (viter != (vset).end())
#endif

#ifndef CONST_NOT_FIND_IF
#define CONST_NOT_FIND_IF(type, vset, viter, v) type::const_iterator viter = (vset).find((v)); \
    if (viter == (vset).end())
#endif

#ifndef FIND_IF
#define FIND_IF(type, vset, viter, v) type::iterator viter = (vset).find((v)); \
    if (viter != (vset).end())
#endif

#ifndef NOT_FIND_IF
#define NOT_FIND_IF(type, vset, viter, v) type::iterator viter = (vset).find((v)); \
    if (viter == (vset).end())
#endif

#ifndef CONST_FOR_EACH
#define CONST_FOR_EACH(type, vlist, viter) for (type::const_iterator viter = (vlist).begin(); \
        viter != (vlist).end(); ++viter)
#endif

#ifndef FOR_EACH
#define FOR_EACH(type, vlist, viter) for (type::iterator viter = (vlist).begin(); \
                                          viter != (vlist).end(); ++viter)
#endif

#ifndef SET_UNION
#define SET_UNION(s1, s2, rs) std::set_union((s1).begin(), (s1).end(), (s2).begin(), (s2).end(), \
                                             inserter((rs), rs.begin()));
#endif

typedef std::set<uint64_t> PESet;
typedef std::set<uint64_t> ProcessorSet;
typedef std::set<std::string> TagSet;
typedef boost::unordered_map<uint64_t, TagSet > SubTagMap;

class AppPE {
public:
    // record ProcessorElement
    ProcessorElement app_pe;
    // pe location
    std::string pn_addr;
    // PEs who care about this app_pe
    // when this app_pe change or delete, we should notify all watching_pes
    PESet watching_pes;
};

class AppProcessor {
public:
    // record processor name, parallel
    Processor cur_pro;

    // processor's all peid can derive from <AppID + ProcessorID + Serial>
    // see GetProcessorPEIDs

    // record all up-processor
    ProcessorSet up_processor;
    // record all down-processor
    ProcessorSet down_processor;
    // record subscribe tags to up-processor
    SubTagMap sub_tags;
};

class WatchProcessor {
    uint64_t processor_id;
    PESet watching_pes;
};

template <class K, class V>
std::set<K> uniq_map_key(const boost::unordered_map<K, V>& lhs,
        const boost::unordered_map<K, V>& rhs) {
    typedef typename boost::unordered_map<K, V>::const_iterator in_iter;
    std::set<K> result;
    for (in_iter it1 = lhs.begin(); it1 != lhs.end(); ++it1) {
        if (rhs.find(it1->first) != rhs.end()) {
            result.insert(it1->first);
        }
    }
    return result;
}

struct PEInfo {
    uint64_t pe_id;
    std::string location;

    PEInfo(uint64_t id, const std::string& addr)
            : pe_id(id), location(addr) {}
};

struct ProcessorInfo {
    uint64_t pro_id;
    uint32_t parallel;
    TagSet tags;

    ProcessorInfo(uint64_t id, uint32_t p, const TagSet& t)
            : pro_id(id), parallel(p), tags(t) {}
};

struct CtrlInfo {
    FlowControl flag;

    explicit CtrlInfo(FlowControl f) : flag(f) {}
};

class AppRouter {
public:
    AppRouter();
    virtual ~AppRouter();

    /**
     * @brief init app_router
     *        1. create zk_client, set callback
     *        2. connect
     *        3. watch app node
     * @param appid[in] app id
     * @param config_file[in] config file name
     * @return error::OK success
     *         others    fail
     */
    virtual error::Code Init(const uint64_t appid,
            const std::string& config_file);

    /**
     * @brief destroy app_router
     *        1. disconnect, destroy zk_client
     *        2. clear local structure
     * @return void
     */
    virtual void Destroy();

    /**
     * @brief 1. lock all
     *        2. add pe to m_local_pes
     *        3. diff app, update watch & notify
     * @param peid[in] pe id
     * @return error::OK success
     *         others    fail
     */
    virtual error::Code AddPE(uint64_t peid);

    /**
     * @brief 1. lock all
     *        2. delete pe from m_local_pes
     *        3. diff app, update watch & notify
     * @param peid[in] pe id
     * @return error::OK success
     *         others    fail
     */
    virtual error::Code DelPE(uint64_t peid);

    // notify transmitter interface
    // ---------------------------
    enum NotifyEvent {
        PE_EVENT_BEGIN = 0,
        ADD_UP_PE,
        DEL_UP_PE,
        UPDATE_UP_PE,
        PE_EVENT_END,
        PROCESSOR_EVENT_BEGIN,
        ADD_DOWN_PRO,
        DEL_DOWN_PRO,
        UPDATE_DOWN_PRO,
        PROCESSOR_EVENT_END,
        APP_FLOW_CTRL
    };

    /**
     * @brief this callback will be called while any change occured to the App
     * @param type[in] notify event type
     * @param notify_peid[in] watching PE id
     * @param notify_ctx[in] notify context (ProcessorInfo*, PEInfo*)
     * @return void
     */
    virtual void NotifyFunc(NotifyEvent type, uint64_t notify_peid, void* notify_ctx) {}

    size_t LocalPENum() {
        return m_local_pes.size();
    }

    typedef boost::shared_ptr<AppRouter> AppRouterPtr;

    // trace tag
    static const char* kAppRouterTrace;

protected:
    typedef std::pair<uint64_t, ProcessorSet > WPPair;
    typedef boost::unordered_map<uint64_t, ProcessorSet > WPMap;
    typedef std::pair<uint64_t, AppPE*> WatchPEPair;
    typedef boost::unordered_map<uint64_t, AppPE*> WatchPEMap;
    typedef std::pair<uint64_t, AppProcessor*> ProcessorPair;
    typedef boost::unordered_map<uint64_t, AppProcessor*> ProcessorMap;

    /**
     * @brief decleare watch to pe
     *        1. get & watch pe
     *        2. add pe to m_all_watch_pes
     *           if exist, update watching_pes
     * @param peid[in] pe id
     * @param watching_peids[in] watching pe id, pe who care about this peid
     * @return error::OK success
     *         others    fail
     */
    error::Code AddWatchPE_L(uint64_t peid,
            const std::set<uint64_t>& watching_peids);

    /**
     * @brief only remove from m_all_watch_pes structure
     *        if other watching still exist, remove from watching_pes
     * NOTICE: zookeeper has not supported unwatch operation!
     * @param peid[in] pe id
     * @param watching_peid[in] watching pe id, pe who care about this peid
     * @return error::OK success
     *         others    fail
     */
    error::Code DelWatchPE_L(uint64_t peid, uint64_t watching_peid);

    // get info from zk
    // ---------------------------
    /**
     * @brief Get all local_pes' down-processor id list
     *        m_local_pes -> new_all_processor -> down_processor id
     * @param new_all_processor[in] all processors map
     * @param watch_down_processors[out] down processors which we interest in
     * @return error::OK success
     *         others    fail
     */
    error::Code GetWatchDownProcessor_L(const ProcessorMap& new_all_processor,
            WPMap& watch_down_processors);

    /**
     * @brief Get all local_pes' down-processor id list
     *        m_local_pes -> new_all_processor -> up_processor id
     * @param new_all_processor[in] all processors map
     * @param watch_up_processors[out] down processors which we interest in
     * @return error::OK success
     *         others    fail
     */
    error::Code GetWatchUpProcessor_L(const ProcessorMap& new_all_processor,
            WPMap& watch_up_processors);

    /**
     * @brief Get diff processors from new_all_processor
     * @param new_all_processor[in] new all_processor map
     * @param add_pro[out] add processors set
     * @param del_pro[out] del processors set
     * @param update_pro[out] update processors set
     * @return error::OK success
     *         others    fail
     */
    error::Code GetAllDiffProcessor_L(const ProcessorMap& new_all_processor,
            std::set<uint64_t>& add_pro,
            std::set<uint64_t>& del_pro,
            std::set<uint64_t>& update_pro);

    /**
     * @brief find processor's pe which belong to local_pe's up-pe
     * @param app_id[in] app id
     * @param pro_id[in] processor id
     * @param paral_begin[in] parallel PE begin index
     * @param paral_num[in] parallel number
     * @param pes[out] up pe list which we interest in
     * @return void
     */
    void GetProcessorPEIDs(uint64_t app_id,
            uint64_t pro_id,
            int paral_begin,
            int paral_num,
            PESet& pes);

    /**
     * @brief find processor's pe which belong to local_pe's up-pe
     * @param app_id[in] app id
     * @param pro_id[in] processor id
     * @param parallel[in] parallel
     * @param pes[out] up pe list which we interest in
     * @return void
     */
    void GetProcessorPEIDs(uint64_t app_id,
            uint64_t pro_id,
            int parallel,
            PESet& pes);

    /**
     * @brief diff between two Processors
     * @param new_app_pro[in] new processor
     * @param old_app_pro[in] old processor
     * @param add_pes[out] output add pes set
     * @param del_pes[out] output del pes set
     * @return true : has diff
     *         false : no diff
     */
    bool DiffProcessor(const AppProcessor& new_app_pro,
            const AppProcessor& old_app_pro,
            PESet* add_pes = NULL,
            PESet* del_pes = NULL);

    /**
     * @brief parse AppProcessor Structure from Application
     * @param app[in] Application of protobuf format
     * @param res_map[out] application info same as local format
     * @return error::OK success
     *         other     fail
     */
    error::Code ParseProcessors(const Application& app,
            ProcessorMap& res_map);

    /**
     * @brief destroy AppProcessor Structure
     * @param pro_map[in/out] application info same as local format
     * @return void
     */
    void DestroyProcssorMap(ProcessorMap* pro_map);

    /**
     * @brief destroy AppPE Structure
     * @param t_map[in/out] ProcessorElement info same as local format
     * @return void
     */
    void DestroyWatchPEMap(WatchPEMap* t_map);

    /**
     * @brief diff between remote and local, then notify
     * @param new_all_processor[in] Application of ProcessorMap format
     * @return error::OK success
     *         others    fail
     */
    error::Code DiffApplication_L(const ProcessorMap& new_all_processor);

    /**
     * @brief diff between remote pe and local one, then notify
     * @param new_pe[in] remote pe info
     * @return error::OK success
     *         others    fail
     */
    error::Code DiffProcessorElement_L(const ProcessorElement& new_pe);

protected:
    error::Code GetUpdateChange_L(const ProcessorMap& new_all_processor,
            const ProcessorSet& update_pro,
            const WPMap& all_watch_up_pro,
            const WPMap& all_watch_down_pro,
            ProcessorSet& update_watch_pro,
            ProcessorSet& update_flow_control_pro,
            PESet& add_watch_pe,
            PESet& del_watch_pe);

    error::Code GetAddChange(const ProcessorMap& new_all_processor,
            const ProcessorSet& add_pro,
            const WPMap& all_watch_up_pro,
            const WPMap& all_watch_down_pro,
            ProcessorSet& add_watch_pro,
            PESet& add_watch_pe);

    error::Code GetDelChange(const ProcessorMap& old_all_processor,
            const ProcessorSet& del_pro,
            const WPMap& old_all_watch_up_pro,
            const WPMap& old_all_watch_down_pro,
            ProcessorSet& del_watch_pro,
            PESet& del_watch_pe);

    /**
     * Notify functions for up-stream, refering to PE change
     */
    error::Code NotifyUpdateAction(const ProcessorMap& new_all_processor,
            const ProcessorSet& update_watch_pro,
            const WPMap& all_watch_down_pro);

    error::Code NotifyUpAddAction(const WPMap& all_watch_up_pro,
            const PESet& add_watch_pe);

    error::Code NotifyUpDelAction(const WPMap& all_watch_up_pro,
            const PESet& del_watch_pe);

    error::Code NotifyFlowControlAction(const ProcessorMap& new_all_processor,
            const ProcessorSet& update_flow_control_pro);
    /**
     * Notify functions for down-stream, refering to Processor change
     */
    error::Code NotifyDownAddAction(const ProcessorMap& new_all_processor,
            const WPMap& all_watch_down_pro,
            const ProcessorSet& add_watch_pro);

    error::Code NotifyDownDelAction(const WPMap& old_all_watch_down_pro,
            const ProcessorSet& del_watch_pro);

    // notify funtion specially call for adding local watch PE
    error::Code NotifyLocalPEAdd_L(const uint64_t pe_id);
    error::Code NotifySingleAddUpPE(const uint64_t add_pe_id,
            const PESet& who_notify);

    error::Code NotifySingleAddDownProcessor(const uint64_t add_pro_id,
            const PESet& who_notify,
            const ProcessorMap& all_processor);

    // App change event handler
    void AppEventProcess(uint64_t event_appid);
    // App's children change event handler
    void AppChildEventProcess(uint64_t event_appid);
    // PE change event handler
    void PEEventProcess(uint64_t event_peid);
    error::Code ClearLocalPEDel_L(const uint64_t peid);
    // flow control change event handler
    error::Code FlowCtrlApplication_L(const Application& app);

    // some helper function
    error::Code GetApplication(uint64_t appid, Application* t_app);
    error::Code GetPEList(uint64_t appid, PESet* peid_set);
    error::Code GetProcessorElement(uint64_t peid, ProcessorElement* t_pe);
    error::Code GetPNAddr(const ProcessorElement& t_pe, std::string* pn_addr);
    bool CheckNeedNotify(uint64_t notify_proid, const AppProcessor& new_pro,
            const AppProcessor& old_pro);
    // void DumpSet(const ProcessorSet& s);

    // zookeeper callback function
    static int OnZKWatchEventCallback(int type, int state, const char* path, void* context);

private:
    uint64_t m_app_id;
    // flow ctrl flag
    FlowControl m_flow_ctrl;
    // data lock
    RWLock m_data_lock;
    // local running PEs
    PESet m_local_pes;
    // map ProcessorID -> AppProcessorPtr
    ProcessorMap m_all_processor;
    // map PEID -> AppPEPtr
    WatchPEMap m_all_watch_pes;

    // zookeeper accessor
    dstream::router::ZkWrapper::ZkWrapperPtr m_zk_wrapper;
};

} // namespace router
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSNODE_APP_ROUTER_H__ NOLINT
