#include <stdlib.h>
#include <errno.h>

#ifndef _WIN32
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#else
#include <WinSock2.h>
#include <Ws2tcpip.h>
#define close(s) closesocket(s)
#endif

#include "WhatsSocket.h"

WhatsSocket::WhatsSocket()
    :m_sockFd(0), m_ifNonBlock(false), m_ifNonInherit(false), m_timeout(0), m_peerPort(0), m_localPort(0)
{

}

WhatsSocket::~WhatsSocket()
{

}


void WhatsSocket::closeSocket()
{
    if (m_sockFd) {
        close(m_sockFd);
        m_sockFd = 0;
    }
    
}

bool WhatsSocket::setNonBlock(int socket)
{
    // 设置非阻塞
#ifndef _WIN32
    int iFcntlResult = fcntl(socket, F_SETFL, \
        fcntl(socket, F_GETFL) | O_NONBLOCK);
    if (-1 == iFcntlResult)
    {
        return false;
    }
#else
    unsigned long ul = 1;
    int ret = ioctlsocket(socket, FIONBIO, (unsigned long *)&ul);//设置成非阻塞模式。  
    if (ret == SOCKET_ERROR)
    {
        return false;
    }
#endif

    return true;
}

bool WhatsSocket::setBlock(int socket)
{
    // 设置非阻塞
#ifndef _WIN32
    int iFcntlResult = fcntl(socket, F_SETFL, \
        fcntl(socket, F_GETFL) & ~O_NONBLOCK);
    if (-1 == iFcntlResult)
    {
        return false;
    }
#endif

    return true;
}

bool WhatsSocket::setLinger(int socket)
{
    struct linger sndOver;
    memset(&sndOver, 0, sizeof(struct linger));
    int iSndOverLength = 0;

    sndOver.l_onoff = 1;
    sndOver.l_linger = 0;
    iSndOverLength = sizeof sndOver;

    int iSetResult = setsockopt(socket, SOL_SOCKET, SO_LINGER, \
        (char*)&sndOver, iSndOverLength);
    if (-1 == iSetResult)
    {
        return false;
    }

    return true;
}


bool WhatsSocket::setNonBlock()
{
    if (!setNonBlock(m_sockFd))
    {
        return false;
    }
    m_ifNonBlock = true;
    return true;
}

bool WhatsSocket::setBlock()
{
    if (!setBlock(m_sockFd))
    {
        return false;
    }

    m_ifNonBlock = false;
    return true;
}

bool WhatsSocket::setNonInherit()
{
#ifndef _WIN32
    int iFcntlResult = -1;

    iFcntlResult = fcntl(m_sockFd, F_SETFD, 1);
    if (-1 == iFcntlResult)
    {
        return false;
    }

    m_ifNonInherit = true;

    return true;

#endif

    return false;
}

bool WhatsSocket::setLinger()
{
    return setLinger(m_sockFd);
}

void WhatsSocket::setSockFd(int sockFd)
{
    m_sockFd = sockFd;
}

int WhatsSocket::getSockFd()
{
    return m_sockFd;
}


bool WhatsSocket::setTimeout()
{
    if (m_timeout <= 0 || m_sockFd < 0)
    {
        return false;
    }

    struct timeval time;
    time.tv_sec = m_timeout;
    time.tv_usec = 0;
    return (0 == setsockopt(m_sockFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time)));
}


//------------------------



WhatsHttpTcpSocket::WhatsHttpTcpSocket()
    :WhatsSocket()
{

}

WhatsHttpTcpSocket::WhatsHttpTcpSocket(int socket)
    : WhatsSocket()
{
    setSockFd(socket);
}

WhatsHttpTcpSocket::~WhatsHttpTcpSocket()
{

}


bool WhatsHttpTcpSocket::create()
{
    closeSocket();

    //socket
    int tmpSocketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (tmpSocketFd < 0)
    {
        return false;
    }

    setSockFd(tmpSocketFd);
    setTimeout();
    return true;
}


bool WhatsHttpTcpSocket::connectPeer()
{
    if (m_localPort > 0)
    {
        struct sockaddr_in localSockAddr;
        memset(&localSockAddr, 0, sizeof(struct sockaddr_in));
        localSockAddr.sin_family = AF_INET;
        inet_pton(AF_INET, m_localIP.c_str(), (void*)&localSockAddr.sin_addr.s_addr);
        //localSockAddr.sin_addr.s_addr = inet_addr(m_localIP.c_str());
        localSockAddr.sin_port = htons(m_localPort);

        //bind
        int iBindResult = -1;
        iBindResult = bind(m_sockFd, (const sockaddr*)&localSockAddr, sizeof(struct sockaddr_in));

        if (iBindResult < 0)
        {
            closeSocket();
            return false;
        }
    }


    struct sockaddr_in peerSockAddr;
    memset(&peerSockAddr, 0, sizeof(struct sockaddr_in));

    peerSockAddr.sin_family = AF_INET;
    inet_pton(AF_INET, m_peerIP.c_str(), (void*)&peerSockAddr.sin_addr.s_addr);
    //peerSockAddr.sin_addr.s_addr = inet_addr(m_peerIP.c_str());
    peerSockAddr.sin_port = htons(m_peerPort);

    if (connect(m_sockFd, (const sockaddr*)&peerSockAddr, sizeof(struct sockaddr_in)) == -1)
    {
        closeSocket();
        return false;
    }

    setNonBlock();


    return true;
}

int WhatsHttpTcpSocket::recvNoblockData(char* buffer, int &recvLen, int &recvedSize)
{
    return WhatsHttpTcpSocket::recvNoblockData(m_sockFd, buffer, recvLen, recvedSize);
}

int WhatsHttpTcpSocket::recvData(char* buffer, int recvLen)
{
    return WhatsHttpTcpSocket::recvData(m_sockFd, buffer, recvLen);
}

int WhatsHttpTcpSocket::sendData(const char* buffer, int sendLen, int &sentSize)
{
    return WhatsHttpTcpSocket::sendData(m_sockFd, buffer, sendLen, sentSize);
}

#ifdef _WIN32
#define MSG_NOSIGNAL 0
#endif

int WhatsHttpTcpSocket::recvNoblockData(int fd_sock, char* buffer, int &recv_len, int &recvedSize)
{
    int recved_bytes = 0;
    int recv_ret = 0;
    while (recved_bytes < recv_len)
    {

        recv_ret = recv(fd_sock, buffer + recved_bytes, recv_len - recved_bytes, MSG_NOSIGNAL);
        if (recv_ret < 0)
        {
            int err = errno;
            switch (err) {
            case 0: //for windows
                return SOCK_CLOSED;
                break;
            case EAGAIN:
                return SOCK_AGAIN;
            case EINTR:
                continue;
            default:
                return SOCK_ERROR;
            }
          
        }
        else if (recv_ret == 0)
        {
            return SOCK_CLOSED;
        }

        recved_bytes += recv_ret;
        recvedSize += recv_ret;
    }
    return SOCK_OK;

}

int WhatsHttpTcpSocket::recvData(int fd_sock, char* buffer, int recv_len)
{
    int total_len = recv_len;

    int recv_bytes = 0;
    while (recv_len > 0)
    {
        recv_bytes = recv(fd_sock, buffer, recv_len, MSG_NOSIGNAL);
        if (recv_bytes == 0)
        {
            return SOCK_CLOSED;
        }
        else if (recv_bytes < 0)
        {
            int err = (int)errno;
            if (err == EINTR)
            {
                continue;
            }
            else if (err != 11 && err != 104)
            {

            }
            else
            {

            }
            return SOCK_ERROR;
        }

        buffer += recv_bytes;
        recv_len -= recv_bytes;
    }
    return total_len;
}

int WhatsHttpTcpSocket::sendData(int fd_sock, const char* buffer, int send_len, int &sentSize)
{
    int total_len = send_len;
    int send_bytes = 0;
    while (send_len > 0)
    {
        send_bytes = send(fd_sock, buffer, send_len, MSG_NOSIGNAL);
        if (send_bytes <= 0)
        {
            int error = errno;
            switch (error) {
            case EINTR:
                continue;
            case EAGAIN:
                return SOCK_AGAIN;
            }
            return SOCK_ERROR;
        }

        buffer += send_bytes;
        send_len -= send_bytes;
        sentSize += send_bytes;
    }
    
    return SOCK_OK;
}