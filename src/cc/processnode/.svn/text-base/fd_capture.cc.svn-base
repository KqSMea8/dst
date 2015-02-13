/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
#include <assert.h>
#include <ev.h>
#include <fcntl.h>

#include <algorithm>
#include <list>

#include "common/mutex.h"
#include "common/utils.h"
#include "processnode/fd_capture.h"

namespace dstream {
namespace processnode {
#define CheckErrorReturn(retval, errret, errfmt, ...) do { \
    if (retval < 0) { \
        DSTREAM_WARN(errfmt, ##__VA_ARGS__); \
        return errret; \
    } \
} while (0)

#define CheckErrorRetF(retval, errfmt, ...) \
    CheckErrorReturn(retval, false, errfmt, ##__VA_ARGS__)
#define CheckErrorRetN(retval, errfmt, ...) \
    CheckErrorReturn(retval, , errfmt, ##__VA_ARGS__)
#define DSTREAM_UNUSED(var) ((void)(var))

FdWatcher::FdWatcher(const std::string dir, const std::string filename_prefix, int fd, int fsize)
        : m_fd(fd),
        m_fsize(fsize),
        m_dir(dir),
        m_filename_prefix(filename_prefix) {
    assert(fd >= 0); // fd must not less than 0
    m_watcher.data = this;
    ev_io_init(&m_watcher, &FdCapture::OnWriteCallBack, m_fd, EV_READ);
    std::string path = GetPath();
    m_outfd = open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
    DSTREAM_DEBUG("open fd: %d %d for %s", m_fd, m_outfd, GetPath().c_str());
}

FdWatcher::~FdWatcher() {
    close(m_fd);
    close(m_outfd);
    unlink(GetPath().c_str());
    DSTREAM_DEBUG("close fd: %d %d for %s", m_fd, m_outfd, GetPath().c_str());
}

bool FdWatcher::OnRoll() {
    DSTREAM_DEBUG("start roll file");
    unsigned timestamp = static_cast<unsigned>(time(NULL));
    close(m_outfd);
    std::string path = GetPath();

    // suffix ".dlog" to flag the done logs
    std::string save_path = path + "." + NumberToString(timestamp) + ".dlog";
    CheckErrorRetF(rename(path.c_str(), save_path.c_str()), "rename file [%s] to [%s] failed",
            path.c_str(), save_path.c_str());
    m_outfd = open(path.c_str(), O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
    CheckErrorRetF(m_outfd, "open file [%s] failed", path.c_str());
    return true;
}

bool FdWatcher::OnWrite() {
    int len = read(m_fd, m_buffer, kBufSize);
    CheckErrorRetF(len, "%s", "read from fd failed");
    if (len == 0) {
        DSTREAM_DEBUG("meet end-of-file,so return");
        OnRoll();
        return false;
    }
    CheckErrorRetF(write(m_outfd, m_buffer, len), "write file in dir %s failed", m_dir.c_str());
    return true;
}

bool FdCapture::AddWatcher(FdWatcher::FdWatcherPtr watcher) {
    if (watcher->GetOutFd() < 0) {
        DSTREAM_WARN("open file [%s] failed,so not add this watcher", watcher->GetPath().c_str());
        return false;
    }

    {   // LockArea
        MutexLockGuard guard(m_watchers_lock);
        DSTREAM_DEBUG("added an io watcher to the list...");
        m_watchers.push_back(watcher);
        if (m_watchers.end() == m_pos) {
            --m_pos;
        }
    }

    ev_async_send(m_loop, &m_async);
    return true;
}

bool FdCapture::RemoveWatcher(FdWatcher::FdWatcherPtr watcher) {
    if (!watcher) {
        return true;
    }

    Iter it = std::find(m_watchers.begin(), m_watchers.end(), watcher);
    DSTREAM_DEBUG("start to remove ...");
    return RemoveWatcher(it);
}

bool FdCapture::RemoveWatcher(Iter iter) {
    DSTREAM_DEBUG("remove (iter) start...");
    if (iter == m_watchers.end()) {
        return false;
    }
    DSTREAM_DEBUG("remove ...");
    if (m_pos == iter) {
        ++m_pos;
    }
    FdWatcher::FdWatcherPtr watcher = *iter;
    ev_io_stop(m_loop, watcher->GetWatcher());
    ev_io_stop(m_loop, watcher->GetWatcher());
    ev_io_stop(m_loop, watcher->GetWatcher());
    m_watchers.erase(iter);
    DSTREAM_DEBUG("remove ok...");
    return true;
}

ev_io* FdWatcher::GetWatcher() {
    return &m_watcher;
}
int FdWatcher::GetOutFd() const {
    return m_outfd;
}
std::string FdWatcher::GetPath()const {
    return m_dir + "/" + m_filename_prefix;
}

struct ev_loop* FdCapture::m_loop = EV_DEFAULT;

FdCapture::FdCapture() {
    // libev will install a sigaction with SIGCHLD..we must restoreit to the default action.
    struct sigaction action, old_action;
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &action, &old_action) < 0) {
        DSTREAM_WARN("SIGNAL ERROR");
    }
    m_pos = m_watchers.begin();
    m_stop_ev = false;
}

void FdCapture::Start(int roll_time) {
    ev_timer_init(&m_timer, &OnTimeCallBack, roll_time, 0.0);
    m_timer.repeat = roll_time;
    ev_timer_start(m_loop, &m_timer);
    ev_async_init(&m_async, &OnAsyncCallBack);
    ev_async_start(m_loop, &m_async);
    ev_async_send(m_loop, &m_async);
    ev_run(m_loop, 0);
}

void FdCapture::DestoryEv_() {
    m_stop_ev = true;
    for (Iter it = m_watchers.begin(); it != m_watchers.end(); ) {
        RemoveWatcher(it++);
    }
    DSTREAM_DEBUG("Delete Watcher Over..");
    ev_timer_stop(m_loop, &m_timer);
    ev_async_stop(m_loop, &m_async);
    ev_break(m_loop, EVBREAK_ONE);
    DSTREAM_DEBUG("break over");
}

void FdCapture::Stop() {
    m_stop_ev = true;
    DSTREAM_DEBUG("FdCapture Stop...");
    ev_async_send(m_loop, &m_async);
}

FdCapture::~FdCapture() {
}

// this callback is empty, it is to make sure ev_check called
void FdCapture::OnAsyncCallBack(EV_P, ev_async* /*w*/, int /*revents*/) {
    Iter& m_pos = m_instance.m_pos;
    if (m_instance.m_stop_ev) {
        DSTREAM_DEBUG("stop ....");
        m_instance.DestoryEv_();
    }
    if (m_pos == m_instance.m_watchers.end()) {
        return;
    }
    DSTREAM_DEBUG("check...");
    MutexLockGuard guard(m_instance.m_watchers_lock);
    for ( ; m_pos != m_instance.m_watchers.end(); ++m_pos) {
        m_instance.StartWatcher_(m_pos);
    }
    DSTREAM_DEBUG("check over");
}

void FdCapture::StartWatcher_(Iter iter) {
    DSTREAM_DEBUG("start watcher..");
    FdWatcher::FdWatcherPtr watcher = *iter;
    ev_io_start(m_loop, watcher->GetWatcher());
    DSTREAM_DEBUG("start watcher..OK");
}

void FdCapture::OnWriteCallBack(EV_P, ev_io* w, int /*revents*/) {
    FdWatcher* watcher = reinterpret_cast<FdWatcher*>(w->data);

    if ((NULL != watcher) && !watcher->OnWrite()) {
        MutexLockGuard guard(m_instance.m_watchers_lock);
        m_instance.RemoveWatcher(watcher->SharedFromThis());
        DSTREAM_INFO("fd watcher removed, fd(%d)", watcher->GetOutFd());
    }
}

void FdCapture::OnTimeCallBack(EV_P, ev_timer* w, int /*revents*/) {
    MutexLockGuard guard(m_instance.m_watchers_lock);
    FdWatcher::FdWatcherPtr watcher;
    for (Iter it = m_instance.m_watchers.begin(); it != m_instance.m_pos ; ) {
        watcher = *it;
        if (!watcher->OnRoll()) {
            m_instance.RemoveWatcher(it++);
        } else {
            ++it;
        }
    }
    ev_timer_again(m_loop, w);
}

FdCapture* FdCapture::GetInstance() {
    return &m_instance;
}

FdCapture FdCapture::m_instance;
#undef CheckErrorReturn
#undef CheckErrorRetF
#undef CheckErrorRetN
#undef DSTREAM_UNUSED

} // namespace processnode
} // namespace dstream
