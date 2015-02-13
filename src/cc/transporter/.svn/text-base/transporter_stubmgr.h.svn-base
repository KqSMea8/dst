/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_SRC_CC_TRANSPORTOR_STUB_MGR_H__
#define __DSTREAM_SRC_CC_TRANSPORTOR_STUB_MGR_H__

#include <boost/unordered_map.hpp>
#include <set>
#include <vector>
#include "common/mutex.h"
#include "common/utils.h"
#include "transporter/transporter_common.h"
#include "transporter/transporter_protocol.h"

namespace dstream {
namespace transporter {

/**
* @brief    PEID => stub
* @notice   shares lifecycle with pn
*/
class TPStubMgr: public CESPObject {
    DECLARE_SINGLETON(TPStubMgr)
    ESP_DECLARE_OBJECT(TPStubMgr)

public:
    static const uint64_t kMaxRetry = 65535;
    static const uint64_t KMaxStub  = 65535;

public:
    ErrCode Start();
    void    Stop();

    //-----------------------------------------------
    // stub management
    /**
    * @brief    Œ™inflow°¢outflow…Í«Îstub
    * @param    id  local peid
    * @return   valid stub
    * @author   konghui, konghui@baidu.com
    * @date     2013-01-16
    */
    ESPSTUB AllocIfStub(uint64_t id);
    ESPSTUB AllocOfStub(uint64_t id);
    void    FreeIfStub(uint64_t id);
    void    FreeOfStub(uint64_t id);

    //-----------------------------------------------
    // stub query
    inline void OnQueryInflowStub(ESPMessage* req_msg) {
        _OnQueryStub(req_msg, m_if_stub_map, TPE_SM_QUERY_IF_ACK);
    }
    inline void OnQueryOutflowStub(ESPMessage* req_msg) {
        _OnQueryStub(req_msg, m_of_stub_map, TPE_SM_QUERY_OF_ACK);
    }

public:
    void OnCompletion(AsyncContext* pctx);

private:
    ~TPStubMgr(); /*forbidden*/

private:
    typedef boost::unordered_map<uint64_t, ESPSTUB> StubMap;
    typedef std::set<ESPSTUB>                       StubSet;
    enum SMAction {
        SM_STOP = TPA_USER_BEGIN
    };
    enum SMStatus {
        SM_RUNNING = 0,
        SM_NOT_RUNNING = 1
    };

private:
    void _OnStop();
    void _OnQueryStub(ESPMessage* req_msg,
                      const StubMap& stub_map,
                      TPEspMsgAction action);

    ESPSTUB _AllocStub(uint64_t id, StubMap* stub_map, const char* name);
    void    _FreeStub(uint64_t id, StubMap* stub_map, const char* name);
    inline bool _TestFreeAndInsertStub(ESPSTUB stub);

    inline ESPSTUB _ConvertStub(int64_t stub) {
        return (ESPSTUB)((stub & 0x000000000000FFFF) %
                         (KMaxStub - kTPStubMgrStub) +
                         kTPStubMgrStub);
    }

private:
    mutable BufHandle* phdl_; // shared buffer handler ptr
    TPStubMsg*         pmsg_; // shared msg head ptr

private:
    uint64_t          m_running;
    mutable MutexLock m_mutex;
    StubMap           m_if_stub_map; // inflow local_peid => stub
    StubMap           m_of_stub_map; // outflow local_peid => stub
    StubSet           m_stub_set;
    volatile int64_t  m_stub;
};

} // namespace transporter
} // namespace dstream

#define g_pTPStubMgr   dstream::transporter::TPStubMgr::Instance()

#endif // __DSTREAM_SRC_CC_TRANSPORTOR_STUB_MGR_H__
