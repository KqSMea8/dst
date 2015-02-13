/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_TRANSPORTER_TEST_COMMON_H__
#define __DSTREAM_CC_PROCESSNODE_TRANSPORTER_TEST_COMMON_H__

#include <vector>
#include <boost/unordered_map.hpp>
#include "Singleton.h"
#include "SyncObj.h"
#include "common/mutex.h"
#include "common/id.h"
#include "common/utils.h"
#include "transporter/transporter.h"
#include "transporter/transporter_common.h"

namespace dstream {

//-----------------------------------------------
// Condition Sync
extern CCond g_tp_test_cond;

void TPTestStart(int count = 1);
void TPTestWait();
void TPTestComplete();

//-----------------------------------------------
// TestTransptorterMgr
class TestTransptorterMgr {
    DECLARE_SINGLETON(TestTransptorterMgr)
public:
    transporter::ErrCode SetTransporter(uint64_t id, transporter::Transporter* tp);
    transporter::Transporter* GetTransporter(uint64_t id);
    void DelTransporter(uint64_t id);
private:
    typedef boost::unordered_map<uint64_t, transporter::Transporter*> TestTransporterMap;
private:
    MutexLock          mutex_;
    TestTransporterMap map_;
};

#define g_TestTransptorterMgr dstream::TestTransptorterMgr::Instance()

//-----------------------------------------------
// TestTransptorterMgr
extern const char* kTestTag;
extern int   kMaxSingleLen;
transporter::ErrCode FillTuplesAtMostN(BufHandle** hdl, int* size, int* batch);
transporter::ErrCode FillTuplesAtLeastN(BufHandle** hdl, int* size, int* batch);

} //namespace dstream

#endif //__DSTREAM_CC_PROCESSNODE_TRANSPORTER_TEST_COMMON_H__
