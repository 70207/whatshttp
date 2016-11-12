
#include "WhatsThread.h"


WhatsThread::WhatsThread()
    :m_isStarted(false)
    ,  m_detachable(false)
    ,m_shouldExit(false)
{
#ifndef _WIN32
    m_threadID = 0;
#endif
}

WhatsThread::~WhatsThread()
{

}


bool WhatsThread::start()
{
    if (m_isStarted) {
        return false;
    }

    m_shouldExit = false;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (m_detachable) {
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    }

    if (0 != pthread_create(&m_threadID, &attr, threadProc, (void*)this)) {
        return false;
    }

    m_isStarted = true;

    pthread_attr_destroy(&attr);

    return true;
}



void* WhatsThread::threadProc(void* para)
{
    WhatsThread*  thread = (WhatsThread*)para;
    thread->run();
    thread->m_isStarted = false;

    return 0;
}

void   WhatsThread::shouldQuit()
{
    m_shouldExit = true;
}