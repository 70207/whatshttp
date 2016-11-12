#include <stdlib.h>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#else
#include <WinSock2.h>
#endif

#include <list>
#include "WhatsSocket.h"
#include "WhatsEventCycle.h"



WhatsEventCycle* WhatsEventCycle::getInstance()
{
    static WhatsEventCycle g_cycle;
    return &g_cycle;
}

WhatsEventCycle::WhatsEventCycle()
{

}

WhatsEventCycle::~WhatsEventCycle()
{

}

void WhatsEventCycle::addEvent(WhatsEvent &event)
{
    WhatsEvent *e = new WhatsEvent(event);
    if (!e->createSock()) {
        event.responseError();
        delete e;
        return;
    }

    WhatsScopeGuard  guard(m_eventsMutex);
    m_waitDealEvents.push_back(e);

}

void WhatsEventCycle::addEvent(WhatsRequest &req, const std::function<void(WhatsResponse *rsp, int error)> &rspFunc)
{
    WhatsEvent *e = new WhatsEvent();
    e->setReq(req);
    e->setRspFunc(rspFunc);
    
    if (!e->createSock()) {
        e->responseError();
        delete e;
        return;
    }

    WhatsScopeGuard  guard(m_eventsMutex);
    m_waitDealEvents.push_back(e);
}

void WhatsEventCycle::delEvent(int fd)
{
    std::map<int, WhatsEvent*>::iterator it = m_events.find(fd);
    if (it == m_events.end()) {
        return;
    }

    WhatsEvent *te = it->second;
    delete te;
    m_events.erase(it);
}

void WhatsEventCycle::preEvents()
{
    std::list<WhatsEvent*>        _toDealEvents;
    {
        WhatsScopeGuard  guard(m_eventsMutex);
        _toDealEvents.swap(m_waitDealEvents);
        m_waitDealEvents.clear();
    }

    for (std::list<WhatsEvent*>::iterator it = _toDealEvents.begin(); it != _toDealEvents.end(); ++it) {
        WhatsEvent *e = *it;
        if (!e->initPeer()) {
            e->responseError();
            delete e;
        }

        if (!e->connectPeer()) {
            e->responseError();
            delete e;
        }

        innerAddEvent(e);
    }
}

void WhatsEventCycle::innerAddEvent(WhatsEvent *e)
{
    int fd = e->getSockFd();

    std::map<int, WhatsEvent*>::iterator it = m_events.find(fd);
    if (it != m_events.end()) {
        WhatsEvent *te = it->second;
        delete te;
        it->second = e;
        return;
    }

    m_events.insert(std::pair<int, WhatsEvent*>(fd, e));
}

void WhatsEventCycle::delEvents()
{

}

void WhatsEventCycle::run()
{
    while (!m_shouldExit) {
        preEvents();
        WhatsEventCycle::dealItem();   
        delEvents();
#ifdef _WIN32
        Sleep(500);
#else
        usleep(5000);
#endif
    }
}


void WhatsEventCycle::dealItem()
{
   
    if (m_events.empty()) {
        return;
    }

    fd_set readSet, writeSet, errorSet;
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    FD_ZERO(&errorSet);
    int maxFd = 0;
    int eventCount = 0;


    std::map<int, WhatsEvent*>::iterator it = m_events.begin();

    struct timeval timeout = { 5,0 };

    for (;it != m_events.end(); ++it) {
        WhatsEvent* event = it->second;
        int fd = it->first;
        if (fd > maxFd) {
            maxFd = fd;
        }

        if (event->shouldRecv()) {
            FD_SET(fd, &readSet);
            eventCount++;
        }

        if (event->shouldSend()) {
            FD_SET(fd, &writeSet);
            eventCount++;
        }
    }

    if (eventCount <= 0) {
        return;
    }

    int flag = select(maxFd + 1, &readSet, &writeSet, NULL, &timeout);
    switch (flag) {
    case -1:
        printf("select error");
        break;
    case 0:
        return;
    }

    std::list<int>       waitDel;
    bool isDeled = false;
    it = m_events.begin();
    for (;it != m_events.end(); ++it) {
        WhatsEvent* event = it->second;
        int fd = it->first;
        isDeled = false;
        if (FD_ISSET(fd, &readSet)) {
            int status = event->onRecvEvent();
         
            switch (status) {
            case WhatsEvent::EVENT_DONE:
            case WhatsEvent::EVENT_CLOSED:
                delete event;
                it->second = 0;
                waitDel.push_back(fd);
                isDeled = true;
                break;
            }
        }

        if (FD_ISSET(fd, &writeSet)) {
            int status = event->onSendEvent();

            switch (status) {
            case WhatsEvent::EVENT_DONE:
            case WhatsEvent::EVENT_CLOSED:
                delete event;
                it->second = 0;
                if (!isDeled) {
                    waitDel.push_back(fd);
                    isDeled = true;
                }
                break;
            }
        }
    }

    for (std::list<int>::iterator lit = waitDel.begin(); lit != waitDel.end(); ++lit) {
        m_events.erase(*lit);
    }
}
  

