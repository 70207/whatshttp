#ifndef _WHATS_EVENT_H__
#define _WHATS_EVENT_H__

#include <functional>

#include "WhatsRequest.h"
#include "WhatsResponse.h"
#include "WhatsSocket.h"

class WhatsEvent
{
public:
    WhatsEvent();
    WhatsEvent(const WhatsEvent& event);
    ~WhatsEvent();

    static const int EVENT_OK = 0;
    static const int EVENT_CLOSED = -1;
    static const int EVENT_DONE = -2;
    static const int EVENT_ERROR = -3;

    WhatsEvent& operator=(const WhatsEvent&);

    void  setReq(WhatsRequest &req);
    void  appendResponse(std::string &data);
    int   getReqData(std::string &data);
    void  setSendPos(int pos);

    int  onSendEvent();
    int  onRecvEvent();

    void  setRspFunc(const std::function<void(WhatsResponse *rsp, int error)> rspFunc);
    
    void responseError();
    bool shouldRecv();
    bool shouldSend();
    void setShouldSend(bool shouldSend);

    bool createSock();
    int  getSockFd();
    bool initPeer();
    bool connectPeer();

    


    

private:

    WhatsRequest      m_req;
    WhatsResponse     m_rsp;
    WhatsHttpTcpSocket    m_sock;

    int                m_reqSize;
    int                m_reqSendPos;

    bool               m_isSendFinish;

    std::function<void(WhatsResponse *rsp, int error)>   m_rspFunc;

    int                m_fd;
    bool               m_shouldSend;
};


#endif _WHATS_EVENT_H__