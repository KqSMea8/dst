/********************************************************************
    author:     zhanggengxin@baidu.com

    purpose:    test for event manager
*********************************************************************/

#include <list>
#include <unistd.h>

#include "common/logger.h"
#include "processmaster/event_manager.h"
#include "gtest/gtest.h"
#include "processmaster/dstream_event.h"

namespace dstream {
namespace test_event_manager {

using namespace dstream_event;
using namespace event_manager;

/************************************************************************/
/* MockEventManager : used for test add event and event priority        */
/************************************************************************/
class MockEventManager : public EventManager
{
public:
    MockEventManager() : add_event_(false), handle_event_size_(0) {}
    void set_add_event(bool add_event) {
        add_event_ = add_event;
    }
    void set_handle_event_size(int size) {
        handle_event_size_ = size;
    }
    int handle_event_size() const {
        return handle_event_size_;
    }
protected:
    virtual void HandleEvents(EventMap* events);
private:
    bool add_event_;
    int handle_event_size_;
};

void MockEventManager::HandleEvents(EventMap* events)
{
    std::list<int> event_type_list;
    for (EventMapIter event_iter = events->begin();
         event_iter != events->end();
         ++event_iter) {
        DSTREAM_DEBUG("type %d", event_iter->first);
        event_type_list.push_back(event_iter->first);
        for (EventSet::iterator iter = event_iter->second.begin();
             iter != event_iter->second.end();
             ++iter) {
            if (add_event_) {
                AddEvent(*iter);
            }
        }
        handle_event_size_ += event_iter->second.size();
    }
    if (add_event_) {
        add_event_ = false;
    }
    if (event_type_list.size() > 1) {
        int size = event_type_list.size();
        int i = 1;
        std::list<int>::iterator event_iter = event_type_list.begin();
        do {
            int type = *event_iter;
            ++event_iter;
            DSTREAM_DEBUG("%d %d %zu", type, *event_iter, event_type_list.size());
            ASSERT_TRUE(type < *event_iter);
        } while (++i < size);
    }
}

static void sleep_event(MockEventManager& mock_event_manager)
{
    for (int i = 0; i < 100; ++i) {
        if (mock_event_manager.wait_event()) {
            return;
        }
        usleep(10000);
    }
}

/************************************************************************/
/* TestEventManager : test event manager                                */
/************************************************************************/
class TestEventManager : public ::testing::Test
{
protected:
    void TestAddEvent();
    void TestEventPriority();
    void TestEventComp();
};

void TestEventManager::TestAddEvent()
{
    MockEventManager mock_event_manager;
    ASSERT_TRUE(mock_event_manager.Start());
    AppID app_id;
    PEID pe_id;
    BackupPEID backup_pe_id;
    PNID pn_id;
    ProcessorElement pe;
    backup_pe_id.set_id(12);
    app_id.set_id(1);
    pe_id.set_id(2);
    pn_id.set_id("test");
    *(pe.mutable_pe_id()) = pe_id;
    mock_event_manager.Suspend();
    // test normal
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_FALSE(mock_event_manager.AddDelayExecEvent(500, EventPtr(new CreatePEEvent(app_id, pe_id,
                                                      pe.resource()))));
    ASSERT_EQ(1, mock_event_manager.EventSize());
    ASSERT_EQ(1, mock_event_manager.AllEventSize());
    ASSERT_TRUE(mock_event_manager.suspend());
    mock_event_manager.Resume();
    sleep_event(mock_event_manager);
    ASSERT_EQ(0, mock_event_manager.EventSize());
    ASSERT_EQ(0, mock_event_manager.AllEventSize());

    // test add two create event
    mock_event_manager.Suspend();
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    pe.mutable_pe_id()->set_id(3);
    pe_id.set_id(3);
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_EQ(2, mock_event_manager.EventSize());
    ASSERT_EQ(2, mock_event_manager.AllEventSize());
    mock_event_manager.Resume();
    sleep_event(mock_event_manager);
    ASSERT_EQ(0, mock_event_manager.EventSize());
    ASSERT_EQ(0, mock_event_manager.AllEventSize());

    // test add fail event
    mock_event_manager.Suspend();
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    pe_id.set_id(2);
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    pn_id.set_id("test2");
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id,
                                                     pn_id))));
    pn_id.set_id("test");
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id,
                                                     pn_id))));
    mock_event_manager.Resume();
    sleep_event(mock_event_manager);
    ASSERT_EQ(0, mock_event_manager.EventSize());
    ASSERT_EQ(0, mock_event_manager.AllEventSize());
    ASSERT_TRUE(mock_event_manager.wait_event());

    // test readd event in handle event
    mock_event_manager.set_add_event(true);
    mock_event_manager.Suspend();
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    ASSERT_EQ(2, mock_event_manager.EventSize());
    ASSERT_EQ(2, mock_event_manager.AllEventSize());

    mock_event_manager.Resume();
    sleep_event(mock_event_manager);
    ASSERT_EQ(0, mock_event_manager.EventSize());
    ASSERT_TRUE(mock_event_manager.wait_event());
    mock_event_manager.Stop();
    ASSERT_FALSE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id,
                                                      pe.resource()))));
}

void TestEventManager::TestEventPriority()
{
    MockEventManager mock_event_manager;
    ASSERT_TRUE(mock_event_manager.Start());
    mock_event_manager.Suspend();
    AppID app_id;
    BackupPEID backup_pe_id;
    PEID pe_id;
    PNID pn_id;
    ProcessorElement pe;
    app_id.set_id(1);
    pn_id.set_id("local_1");
    pe_id.set_id(1);
    backup_pe_id.set_id(1);
    *(pe.mutable_pe_id()) = pe_id;
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_EQ(1, mock_event_manager.EventSize());
    pe.mutable_pe_id()->set_id(2);
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_EQ(2, mock_event_manager.EventSize());
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_EQ(2, mock_event_manager.EventSize());
    ASSERT_EQ(1, mock_event_manager.EventTypeSize());
    app_id.set_id(2);
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource()))));
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    ASSERT_EQ(4, mock_event_manager.EventSize());
    ASSERT_EQ(2, mock_event_manager.EventTypeSize());
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    backup_pe_id.set_id(13);
    ASSERT_TRUE(mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(),
                                                     backup_pe_id, pn_id))));
    app_id.set_id(3);
    mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(), backup_pe_id,
                                         pn_id)));
    mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(), backup_pe_id,
                                         pn_id)));
    mock_event_manager.AddEvent(EventPtr(new PEFailEvent(app_id, pe_id, pe.resource(), backup_pe_id,
                                         pn_id)));
    ASSERT_EQ(6, mock_event_manager.EventSize());
    mock_event_manager.AddEvent(EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id, pn_id)));
    backup_pe_id.set_id(123);
    mock_event_manager.AddEvent(EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id, pn_id)));
    ASSERT_EQ(8, mock_event_manager.EventSize());
    mock_event_manager.Resume();
    sleep_event(mock_event_manager);
    ASSERT_EQ(8, mock_event_manager.handle_event_size());
}

void TestEventManager::TestEventComp()
{
    AppID app_id;
    PEID pe_id;
    BackupPEID backup_pe_id;
    PNID pn_id;
    ProcessorElement pe;
    backup_pe_id.set_id(12);
    app_id.set_id(1);
    pe_id.set_id(2);
    pn_id.set_id("test");
    *(pe.mutable_pe_id()) = pe_id;
    EventPtr event1 = (EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource())));
    EventPtr event2 = (EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource())));
    pe_id.set_id(3);
    EventPtr event3 = (EventPtr(new CreatePEEvent(app_id, pe_id, pe.resource())));
    EventSet event_set;
    EventSetIter event_it;
    ASSERT_TRUE(event_set.insert(event1).second);
    ASSERT_FALSE((event_it = event_set.find(event2)) == event_set.end());
    ASSERT_FALSE(event_set.insert(event2).second);
    ASSERT_TRUE((event_it = event_set.find(event3)) == event_set.end());
    ASSERT_TRUE(event_set.insert(event3).second);
    ASSERT_EQ(2u, event_set.size());

    event_set.clear();
    pe_id.set_id(3);
    pn_id.set_id("test");
    EventPtr event4 = (EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id, pn_id)));

    pe_id.set_id(2);
    pn_id.set_id("test2");
    EventPtr event5 = (EventPtr(new KillPEEvent(app_id, pe_id, backup_pe_id, pn_id)));

    ASSERT_TRUE(event_set.insert(event4).second);
    ASSERT_FALSE((event_it = event_set.find(event4)) == event_set.end());
    ASSERT_TRUE(event_set.insert(event5).second);
    ASSERT_FALSE((event_it = event_set.find(event5)) == event_set.end());
    ASSERT_EQ(2u, event_set.size());
}

TEST_F(TestEventManager, test_add_event)
{
    TestAddEvent();
}

TEST_F(TestEventManager, test_event_priority)
{
    //  TestEventPriority();
}

TEST_F(TestEventManager, test_event_comp)
{
    TestEventComp();
}

} //namespace test_event_manager
} //namespace dstream

