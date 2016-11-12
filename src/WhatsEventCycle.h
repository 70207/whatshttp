#ifndef _WHATS_EVENT_CYCLE_H__
#define _WHATS_EVENT_CYCLE_H__

#include <map>
#include <list>
#include "WhatsEvent.h"
#include "WhatsThread.h"
#include "WhatsThreadSync.h"

class WhatsEventCycle : public WhatsThread
{
public:
    static WhatsEventCycle* getInstance();
    ~WhatsEventCycle();

    void addEvent(WhatsEvent &);
    void addEvent(WhatsRequest &req, const std::function<void(WhatsResponse *rsp, int error)> &rspFunc);
    void delEvent(int fd);

    

private:
    WhatsEventCycle();
    void preEvents();
    void innerAddEvent(WhatsEvent *e);
    void delEvents();

    void run();
    void dealItem();
    std::map<int, WhatsEvent*>    m_events;
    std::list<WhatsEvent*>        m_waitDealEvents;
    std::list<WhatsEvent*>        m_waitDelEvents;
    WhatsHttpMutex                    m_eventsMutex;
};




#endif _WHATS_EVENT_CYCLE_H__
