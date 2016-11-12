#ifndef _WHATS_LOCK_H__
#define _WHATS_LOCK_H__

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

class WhatsHttpMutex
{
public:
    WhatsHttpMutex();
    ~WhatsHttpMutex();

    //0--failed, 1--success
    int Lock();
    //0--failed, 1--success
    int TryLock();
    //0--failed, 1--success
    int Unlock();

private:
    //0--failed, 1--success
    int Init();
private:
    pthread_mutex_t m_mutex;

};

class WhatsScopeGuard
{
public:
    WhatsScopeGuard(WhatsHttpMutex &mutex);
    ~WhatsScopeGuard();
    //0--failed, 1--success
    int Lock();
    //0--failed, 1--success
    int Unlock();
private:
    int 		 m_bLocked;
    WhatsHttpMutex* m_pMutex;
};


#endif
