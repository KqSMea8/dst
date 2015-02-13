/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief
 *
 */

#include <string>
#include "echo.pb.h"
#include "processelement/task.h"

using namespace dstream;
using namespace dstream::processelement;

class EchoTask : public Task {
public:
  // types we may encounter.
  static const char* in_tag;
  static const char* out_tag;
  typedef ProtoTuple< EchoRequest > InTuple;
  typedef DefaultTupleTypeInfo< InTuple > InTupleInfo;
  typedef ProtoTuple< EchoResponse> OutTuple;
  typedef DefaultTupleTypeInfo< OutTuple > OutTupleInfo;

  static const int kBatchSize = 10;
  static const int kDefaultUpPEId = 2011;
  static const int kDefaultUpPipeId = 2011;
  // ----------------------------------------
  EchoTask():
    seq_(0),
    first_fetch_(true),
    first_forward_(true),
    first_ontuples_(true) {
    registerTupleTypeInfo(EchoTask::in_tag, new InTupleInfo());
    registerTupleTypeInfo(EchoTask::out_tag, new OutTupleInfo());
  }
  // ----------------------------------------
  // mock process node parts...
  // when PE really works, user don't need the following code.
  // ----------------------------------------
  virtual void onFetchLog(const uint64_t peid,
                          bool replay_mode,
                          uint64_t* pipeid,
                          uint64_t* upeid,
                          uint64_t* upipeid,
                          bool* end_replay,
                          std::vector< Tuple*>* tuples) {
    if(first_fetch_) {
      DSTREAM_NOTICE("[TEST]onfetchLog(%llu)...", peid);
      first_fetch_ = false;
    }

    if(replay_mode) { // we don't replay.
      *end_replay = true;
    } else {
      // we don't want whole system work too fast.
      // sleep(1);
      *upeid = kDefaultUpPEId;
      *upipeid = kDefaultUpPipeId;
      *pipeid = seq_;
      for(int i = 0; i < kBatchSize; i++) {
        char s[32];
        snprintf(s, sizeof(s), "tuple#%d", i + seq_);
        InTuple* tuple = dynamic_cast<InTuple*>(allocateTuple(in_tag));
        tuple->set_field(s);
        tuples->push_back(tuple);
      }
      seq_+=kBatchSize;
    }
  }
  virtual void onForwardLog(const uint64_t peid, const std::vector< Tuple* >& tuples) {
    if(first_forward_) {
      DSTREAM_NOTICE("[TEST]onForwardLog(%llu)...", peid);
      first_forward_ = false;
    }
    for(size_t i=0;i<tuples.size();i++){
      OutTuple* tuple = dynamic_cast<OutTuple*>(tuples[i]);
      DSTREAM_INFO("onForwardLog('%s')",tuple->field().c_str());
    }
  }
  // ----------------------------------------
  // actually working code which use need to care about.
  // ----------------------------------------
  virtual void onTuples(const Tuples& tuples) {
    if(first_ontuples_) {
      DSTREAM_NOTICE("[TEST]onTuples");
      first_ontuples_ = false;
    }
    for(size_t i = 0; i < tuples.size(); i++) {
      const InTuple* in_tuple = dynamic_cast<const InTuple*>(tuples[i]);
      OutTuple* out_tuple = dynamic_cast<OutTuple*>(emit(out_tag));
      out_tuple->set_field(in_tuple->field());
      DSTREAM_INFO("echo content '%s'", in_tuple->field().c_str());
    }
  }
private:
  int seq_;
  bool first_fetch_;
  bool first_forward_;
  bool first_ontuples_;
}; // class EchoTask

const char* EchoTask::in_tag = "echo.in";
const char* EchoTask::out_tag = "echo.out";

int main() {
  Logger::initialize("worker");
  bool loopback = false;
  if(getenv("loopback") != 0) {
    loopback = true;
  }
  EchoTask task;
  task.run(loopback);
  DSTREAM_INFO("exit...");
  return 0;
}
