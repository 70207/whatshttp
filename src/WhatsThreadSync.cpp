#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "WhatsThreadSync.h"


WhatsHttpMutex::WhatsHttpMutex()
{
    memset(&m_mutex, 0, sizeof(pthread_mutex_t));
    Init();
}


WhatsHttpMutex::~WhatsHttpMutex()
{
    pthread_mutex_unlock(&m_mutex);
    pthread_mutex_destroy(&m_mutex);
}


int WhatsHttpMutex::Init()
{
    if (pthread_mutex_init(&m_mutex, NULL) != 0)
    {
        return 0;
    }

    return 1;
}


int WhatsHttpMutex::Lock()
{
    if (0 == pthread_mutex_lock(&m_mutex))
    {
        return 1;
    }

    return 0;
}


int WhatsHttpMutex::TryLock()
{
    if (0 == pthread_mutex_trylock(&m_mutex))
    {
        return 1;
    }

    return 0;
}

int WhatsHttpMutex::Unlock()
{
    if (0 == pthread_mutex_unlock(&m_mutex))
    {
        return 1;
    }

    return 0;
}



WhatsScopeGuard::WhatsScopeGuard(WhatsHttpMutex& mutex)
{
    m_pMutex = &mutex;
    if (m_pMutex->Lock())
    {
        m_bLocked = 1;
    }
    else
    {
        m_bLocked = 0;
    }
}

WhatsScopeGuard::~WhatsScopeGuard()
{
    if (m_bLocked)
    {
        if (m_pMutex->Unlock())
        {
            m_bLocked = 0;
        }
    }
}



int WhatsScopeGuard::Lock()
{
    if (m_pMutex->Lock())
    {
        m_bLocked = 1;
        return 1;
    }
    else
    {
        m_bLocked = 0;
        return 0;
    }
}


int WhatsScopeGuard::Unlock()
{
    if (m_bLocked)
    {
        if (m_pMutex->Unlock())
        {
            m_bLocked = 0;
            return 1;
        }
        return 0;
    }

    return 1;
}
