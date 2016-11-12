#ifndef _WHATS_THREAD_H__
#define _WHATS_THREAD_H__

#define HAVE_STRUCT_TIMESPEC
#include <pthread.h>

class WhatsThread
{
public:
    WhatsThread();
    ~WhatsThread();


    bool start();

    virtual void run() = 0;

    void setDetachable(bool detachable) { m_detachable = detachable; }

    bool   m_isStarted;
    bool   m_shouldExit;

    void   shouldQuit();
private:
    static void* threadProc(void* para);
    bool   m_detachable;
    pthread_t m_threadID;
};





#endif
