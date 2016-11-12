#ifndef _WHATS_SOCKET_H__
#define _WHATS_SOCKET_H__

#include <string>
#include <time.h>
#ifdef _WIN32
#include <WinSock2.h>
#endif
using namespace std;
class WhatsSocket
{
public:
    WhatsSocket();
    ~WhatsSocket();

    static const int SOCK_ERROR = -1;
    static const int SOCK_AGAIN = -11;
    static const int SOCK_CLOSED = -12;
    static const int SOCK_OK = 0;

    virtual bool create() = 0;

    void closeSocket();

    bool setNonBlock();
    bool setBlock();
    bool setNonInherit();
    bool setLinger();

    static bool setNonBlock(int socket);
    static bool setBlock(int socket);
    static bool setLinger(int socket);

    void setSockFd(int sockFd);
    int getSockFd();

    static void setTimeout(int socket, unsigned timeout)
    {
        if (timeout <= 0 || socket < 0)
        {
            return;
        }

        struct timeval time;
        time.tv_sec = timeout;
        time.tv_usec = 0;
        setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&time, sizeof(time));
        setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&time, sizeof(time));
    }

    inline void setLocalIP(const string& ip)
    {
        m_localIP = ip;
    }

    inline string& getLocalIP()
    {
        return m_localIP;
    }
    inline void setLocalPort(unsigned short port)
    {
        m_localPort = port;
    }
    inline unsigned short getLocalPort()
    {
        return m_localPort;
    }

    inline void setPeerIP(const string& ip)
    {
        m_peerIP = ip;
    }
    inline string& getPeerIP()
    {
        return m_peerIP;
    }
    inline void setPeerPort(unsigned short port)
    {
        m_peerPort = port;
    }
    inline unsigned short getPeerPort()
    {
        return m_peerPort;
    }

protected:
    bool setTimeout();

protected:

    string         m_localIP;
    unsigned short m_localPort;

    string          m_peerIP;
    unsigned short  m_peerPort;

    int             m_sockFd;

    bool            m_ifNonBlock;
    bool            m_ifNonInherit;

    unsigned		m_timeout;
};


class WhatsHttpTcpSocket : public WhatsSocket
{
public:
    WhatsHttpTcpSocket();
    WhatsHttpTcpSocket(int socket);
    ~WhatsHttpTcpSocket();




    bool create();
    bool connectPeer();


    int recvNoblockData(char* buffer, int &recvLen, int &recvedSize);
    int recvData(char* buffer, int recvLen);
    int sendData(const char* buffer, int sendLen, int &sentSize);

private:

    static int recvNoblockData(int fd_sock, char* buffer, int &recv_len, int &recvedSize);
    static int recvData(int fd_sock, char* buffer, int recv_len);
    static int sendData(int fd_sock, const char* buffer, int send_len, int &sentSize);

};






#endif
