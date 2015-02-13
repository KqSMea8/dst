/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/

#ifndef __DSTREAM_CC_PROCESSNODE_FD_CAPTURE_H__ // NOLINT
#define __DSTREAM_CC_PROCESSNODE_FD_CAPTURE_H__ // NOLINT

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <ev.h>
#include <list>
#include <string>

#include "common/mutex.h"
namespace dstream {
namespace processnode {

// This class Should Not Be Extended !!! it must not have vptr
class FdWatcher: public boost::enable_shared_from_this<FdWatcher> {
public:
    typedef boost::shared_ptr<FdWatcher> FdWatcherPtr;

    FdWatcher(const std::string dir, const std::string filename_prefix, int fd, int fsize);
    ~FdWatcher();

    FdWatcherPtr SharedFromThis() {
        return shared_from_this();
    }
    bool OnRoll();
    bool OnWrite();
    int GetOutFd() const;
    ev_io* GetWatcher();
    std::string GetPath() const;

private:
    const static int kBufSize = 8192;
    ev_io       m_watcher;
    int         m_fd;
    int         m_fsize;
    int         m_outfd;
    std::string m_dir;
    std::string m_filename_prefix;
    int         m_buffer[kBufSize];
}; // class  FdWatcher

class FdCapture {
public:
    // Run the loop to Capture the output pipes.
    void Start(int roll_time = 60);
    // True will be returned if OK. Otherwise,false will be returned.
    // It will free the watcher's memory automaticly.
    // User should not free the memory of the watcher.
    bool AddWatcher(FdWatcher::FdWatcherPtr watcher);
    // Warn:This function is thread-unsafe.
    // and this function will stop the watcher and free the memory of the watcher
    // maybe ,this function will never be called
    bool RemoveWatcher(FdWatcher::FdWatcherPtr watcher);

    ~FdCapture();
    void Stop();

    static FdCapture* GetInstance();
    static void OnTimeCallBack(EV_P, ev_timer* w, int revents);
    static void OnWriteCallBack(EV_P, ev_io* w, int revents);
    static void OnAsyncCallBack(EV_P, ev_async* w, int revents);

private:
    typedef std::list<FdWatcher::FdWatcherPtr>::iterator Iter;
    // Warn:This function is thread-unsafe.
    // and this function will stop the watcher and free the memory of the watcher
    bool RemoveWatcher(Iter iter);
    void StartWatcher_(Iter iter);
    void DestoryEv_();

    FdCapture();
    FdCapture(const FdCapture&);
    FdCapture& operator = (const FdCapture&);

    static FdCapture m_instance;

    std::list<FdWatcher::FdWatcherPtr> m_watchers;
    std::list<FdWatcher::FdWatcherPtr>::iterator m_pos;
    MutexLock                  m_watchers_lock;
    ev_timer                   m_timer;
    ev_async                   m_async;
    bool                       m_stop_ev;
    static struct ev_loop*     m_loop;
}; // class  FdCapture

} // namespace processnode
} // namespace dstream

#endif // __DSTREAM_CC_PROCESSNODE_FD_CAPTURE_H__ NOLINT
