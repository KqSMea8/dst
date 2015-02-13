/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_PROCESS_NODE_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_PROCESS_NODE_H__ // NOLINT

#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>

#include "processnode/zk_wrapper.h"
#include "common/application_tools.h"
#include "common/bounded_blocking_queue.h"
#include "common/commit_log.h"
#include "common/condition.h"
#include "common/config.h"
#include "common/error.h"
#include "common/file_meta.h"
#include "common/hdfs_client.h"
#include "common/logger.h"
#include "common/mutex.h"
#include "common/proto/application.pb.h"
#include "common/proto/pm_pn_rpc.pb.h"
#include "common/proto/pub_sub.pb.h"
#include "common/proto_rpc.h"
#include "common/proto_serializer.h"
#include "common/rpc.h"
#include "common/rpc_const.h"
#include "common/utils.h"
#include "common/zk_client.h"

#include "processelement/context.h"

#include "processnode/config_map.h"
#include "processnode/event_processor.h"
#include "processnode/pe_wrapper_manager.h"
#include "processnode/pn_reporter.h"

namespace dstream {
namespace processnode {

extern volatile int32_t g_quit;

class ProcessNode : public EventProcessor, /* for rpc handling */
    public processelement::internal::Context { /* for local testing */
public:
    ProcessNode();
    virtual ~ProcessNode();

    bool Init();
    void Destory();

    PN& GetPNMeta() {
        return m_pn_meta;
    }
    void WritePNMeta(const PN& pn);

    void SetPNMeta(const PN& pn_meta) {
        m_pn_meta = pn_meta;
        WritePNMeta(pn_meta);
    }

    // DEBUGGER(client) -> PN(server)
    virtual std::string OnDebugPN(const string& cmd, const string& args);

    // PE(client) -> PN(server)
    virtual bool OnReady(const PEID& peid);
    virtual void OnHeartbeat(const PEID& peid,
            const std::map<std::string, std::string>& metrics, FlowControl& flow_control);

    // PM(client) -> PN(server)
    virtual int32_t OnCreatePE(const AppID& app_id,
            const PEID& peid,
            const Processor& pro,
            const BackupPEID& bpeid,
            const uint64_t last_assign_time);

    /**
     * @Brief  动态更新PE
     *
     * @Param app_id 指定的APPID
     * @Param peid   指定的PEID
     * @Param pro    更新的Processor对象
     *
     * @Returns      error::OK 表示开始更新，或是更新进行中
     *               其他      表示启动更新线程失败
     */
    virtual int32_t OnUpdatePE(const AppID& app_id,
            const PEID& peid,
            uint32_t revision);

    virtual int32_t OnKillPE(const PEID& peid);

    // Utils Func
    static bool InitProcessNodeContext();
    static void DestoryProcessNode();
    static ProcessNode* GetProcessNode();

private:
    PN m_pn_meta;

    // singleton process node ptr
    static ProcessNode* m_process_node_instance;
    PEWrapperManager::PEWrapperManagerPtr m_wrapper_mgr_ptr;

    // package
    int m_config_tuples_batch_count;
    int m_config_force_forward_tuples_period_ms;
};

} // namespace processnode
} // namespace dstream

#endif // NOLINT

