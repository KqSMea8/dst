/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#ifndef __DSTREAM_CC_PROCESSNODE_EVENT_PROCESSOR_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_EVENT_PROCESSOR_H__ // NOLINT

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "common/application_tools.h"
#include "common/rpc_const.h"

namespace dstream {
namespace processnode {

// Interface
class EventProcessor {
public:
    virtual ~EventProcessor() {}

    // ----------------------------------------
    // enum all events pe will send to pn, and let the pn can handle
    // all these events. LoopBack on some sort level like mock pn
    // but on purpose we ignore some info to make the interface much simpler.
    // like 'ForwardLog', mock pn won't see the tuplemeta.
    // well, I think to many situations, it's OK.:).
    virtual std::string OnDebugPN(const string& cmd, const string& args) = 0;

    // PN <-> PE
    virtual bool OnReady(const PEID& peid) = 0;
    virtual void OnHeartbeat(const PEID& peid,
            const std::map< std::string, std::string >& metrics, FlowControl& flow_control) = 0;

    // PM <-> PN
    virtual int32_t OnCreatePE(const AppID& app_id, const PEID& peid, const Processor& pro,
            const BackupPEID& bpeid, const uint64_t last_assign_time) = 0;
    virtual int32_t OnKillPE(const PEID& peid) = 0;
};
} // namespace processnode
} // namespace dstream
#endif // __DSTREAM_CC_PROCESSNODE_EVENT_PROCESSOR_H__ NOLINT

