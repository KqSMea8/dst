/***************************************************************************
 *
 * Copyright (c) Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/


/**
 * @author zhangyan04(@baidu.com)
 * @brief  processelement task
 *
 */

#ifndef __DSTREAM_CC_PROCESSELEMENT_TASK_H__ // NOLINT
#define __DSTREAM_CC_PROCESSELEMENT_TASK_H__ // NOLINT

#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <vector>
#include "common/condition.h"
#include "common/dstream.h"
#include "common/logger.h"
#include "common/mutex.h"
#include "processelement/loopback.h"

namespace dstream {
//-----------------------------------------------
// kylin
extern volatile bool transporter::g_init;     // esp system init
extern CESPSystem    transporter::g_esp;      // esp system
extern uint16_t      transporter::g_esp_port; // esp listen port
extern uint32_t      transporter::g_precision_ms; // precision of kylin system

extern int    transporter::g_kKylinExecWorkers; // num of worker thread
extern int    transporter::g_kKylinNetWorkers;  // num of network thread
extern int    transporter::g_kKylinDiskWorkers; // num of disk thread
extern int    transporter::g_kKylinBufSmallTranBufNum;  // num of small buf
extern int    transporter::g_kKylinBufSmallTranBufSize; // small buf size
extern int    transporter::g_kKylinBufBigTranBufNum; // num of big buf in one block,
                                        // each block is 64MB,
                                        // so each buf size is 64MB/32 = 2MB
extern double transporter::g_kKylinBufLowMark;     // soft-watermark to small buf
extern int    transporter::g_kKylinWriteWaterMark; // 30%
} // namespace dstream

namespace dstream {
namespace processelement {

/**
 * Counter to keep track of a property and its value.
 */
class Counter {
private:
    int          m_id;
    std::string* m_name;
    int          m_amount;

public:
    explicit Counter(int counterId) : m_id(counterId) {
        m_name = NULL;
        m_amount = 0;
    }
    Counter(int counterId, std::string counterName) : m_id(counterId) {
        m_name   = new std::string(counterName);
        m_amount = 0;
    }
    // Counter(const Counter& counter) : m_id(counter.id()) {}

    virtual ~Counter() {
        if (m_name) {
            delete m_name;
        }
    }

    int id() const {
        return m_id;
    }
    std::string* name() const {
        return m_name;
    }
    int amount() const {
        return m_amount;
    }
    void AddAmount(int cur_amount) {
        m_amount += cur_amount;
    }
    // !!!! Abandoned
    void add_amount(int cur_amount) {
        m_amount += cur_amount;
    }
};

// ------------------------------------------------------------
/**
* @brief    Task
*           !!! singleton thread unsafe.
*           if user use google::protobuf to define tuple, you can use ProtoTuple<T>
*           instead of raw 'Tuple' where T is the type of google::protobuf object.
*/
class Task : public MutexLock,
    public LoopBack,
    public DynamicProtoType {
public:
    static const char* kImporterTupleTag;
    Task();
    virtual ~Task();
    // --------------------------------------------
    /**
    * User Attention Area.
    * how to process tuples.
    */

    /**
    * @brief  functions with user custom callback.
    *         - Init():
    *             prepare to start pe, called at the very start of
    *             the progress of pe. user can load dicts and other
    *             initializing stuffs.
    *         - OnStart():
    *             signaling that pe is ready to go.
    *         - OnStop():
    *             signaling that pe is going to die, user should
    *             flush all the data in this function.
    *         - Close():
    *             called at pe's exit. user should reclaim the
    *             resources and close the handles in this.
    *@param   event   the event indicating the context for quiting,
    *             eg.
    *             PEET_STOP_DUE_TO_UPD : quit due to update
    *             PEET_STOP_DUE_TO_DEL : quit due to deletion
    *             PEET_UNKOWN          : unkown
    *@TODO(somerd@baidu.com)    onStart() will be available at v1.5.4
    */

    virtual void Init() {}
    virtual void OnStart() {}
    virtual void OnStop(PEEventType event) { DSTREAM_UNUSED_VAL(event); }
    virtual void Close() {}

    /**
    * @brief stop PE run loop
    */
    void Quit();

    /**
    * @brief  called when tuples arrived.
    *         default action is to discard them.
    *
    *         !!! DOESN'T work on importers.
    * @param  Tuples: tuples arrray.
    */
    virtual void OnTuples(const Tuples& /*tuples*/) {}

    /**
    * @brief  called when raw message arrives.
    *         user has to provide way to parse
    *         message from spout and forward.
    *         default action is to discard them.
    *
    *         !!! ONLY works on importers.
    * @param  bytes: raw message array
    */
    virtual void OnSpout(ReadableByteArray* /*bytes*/) {}

    /**
    * @brief  emit tuples.
    *         EmitWithLock() is a multi-thread version for Emit(),
    *         and need EmitUnlock() to unlock.
    * @param  tag: type tag
    * @return new tuple
    */
    Tuple* Emit(const std::string& tag);
    Tuple* EmitWithLock(const std::string& tag) {
        m_send_tuples_lock.lock();
        return Emit(tag);
    }
    void EmitUnlock() {
        m_send_tuples_lock.unlock();
    }

    /**
    * @brief  register tuple type
    *
    * @param  tag     tag for tuple type
    * @param  info    tuple info
    */
    void RegisterTupleTypeInfo(const std::string& tag, TupleTypeInfo* info);

    /**
    * @brief  Register a counter with the given name.
    *
    * @param  name    name of counter
    *
    * @return counter
    */
    virtual Counter* GetCounter(const std::string& name);
    /**
    * @brief   Increment the value of the counter with the given amount.
    *
    * @param  counter counter
    * @param  amount  number to increase
    */
    virtual void IncrementCounter(const Counter* counter, uint64_t amount);

    /**
    * @brief  if 'use_loopback', then task will allocate
    *         a separate worker threads to mock pn and
    *         communicate with pe. it's much easier for
    *         local test. default we assume user don't use
    *         loopback functionality.
    * @param  use_loopback    flag for loopback.
    */
    void Run() { return Run(false); }
    void Run(bool use_loopback);

    /**
    * @brief  get instance
    */
    static Task* Instance() { return m_instance; }

    /**
    * @brief  callbacks for flow control
    */
    virtual void StartCallback() {}
    virtual void StopCallback() {}

private:
    virtual bool Init(const std::string& file, const std::string& type_name) {
        return DynamicProtoType::Init(file, type_name);
    }

public:
    void* ImporterThreadFunction(const std::string& commit_name); // use internally.
    void* ImporterProgressThreadFunction();
    void* LoopbackThreadFunction();  // use internally.
    void* HeartbeatThreadFunction(); // use internally.
    void* PingThreadFunction();      // use internally.

    void SetToQuit();
    bool ReadyToQuit();

protected:
    // --------------------
    bool          m_use_loopback;
    volatile bool m_quit;
    volatile bool m_terminating;
    volatile bool m_term_recvived;

private:
    int StartTransporter();
    // int normal_workflow(rpc::Connection& conn);
    int NormalWorkflow();
    int ImporterWorkflow();
    int SyncNode();
    // int forward_log(rpc::Connection& conn);
    int ForwardLog();
    int ForwardLogToBigpipe();
    // signal handle
    void SetSignalHandler();
    // commit_log open loop
    bool OpenLoop(const std::string& commit_name, CommitLog* commit_log);

private:
    // assure task is a singleton.
    static Task*    m_instance;
    // for user-defined counters
    static int                  m_instance_counter;
    volatile int64_t            m_counter_id;
    std::map<int64_t, Counter*> m_counter_map;
    MutexLock                   m_counter_map_lock;
//-----------------------------------------------------------------
// !!!!! Abandoned interfaces
public:
    virtual void init() {}
    virtual void onStart() {}
    virtual void onStop(PEEventType event) { DSTREAM_UNUSED_VAL(event); }
    virtual void close() {}
    void         quit() {
        return Quit();
    }

    virtual void onTuples(const Tuples& tuples) {}
    virtual void onSpout(ReadableByteArray* bytes) {}
    Tuple* emit(const std::string& tag) {
        return Emit(tag);
    }
    Tuple* emitWithLock(const std::string& tag) {
        return EmitWithLock(tag);
    }
    void emitUnlock() {
        return EmitUnlock();
    }
    Tuple* emit_with_lock(const std::string& tag) {
        return EmitWithLock(tag);
    }
    void emit_unlock() {
        return EmitUnlock();
    }

    void registerTupleTypeInfo(const std::string& tag, TupleTypeInfo* info) {
        return RegisterTupleTypeInfo(tag, info);
    }
    virtual Counter* getCounter(const std::string& name) {
        return GetCounter(name);
    }
    virtual void incrementCounter(const Counter* counter, uint64_t amount) {
        return IncrementCounter(counter, amount);
    }

    void run(bool use_loopback = false) { return Run(use_loopback); }
    static Task* instance() { return Instance(); }
    virtual void startCallback() {}
    virtual void stopCallback() {}

    void* importer_thread_function(std::string commit_name) { // use internally.
        return ImporterThreadFunction(commit_name);
    }
    void* importer_progress_thread_function() {
        return ImporterProgressThreadFunction();
    }
    void* loopback_thread_function() {  // use internally.
        return LoopbackThreadFunction();
    }
    void* heartbeat_thread_function() { // use internally.
        return HeartbeatThreadFunction();
    }
    void* ping_thread_function() { // use internally.
        return PingThreadFunction();
    }
}; // class Task

// for debug, add by konghui
void DumpTuples(const std::vector<const Tuple*>& tuples, std::string* s);

} // namespace processelement
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSELEMENT_TASK_H__ NOLINT
