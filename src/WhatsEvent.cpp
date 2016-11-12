#include "WhatsEvent.h"
#include "WhatsHttp.h"
#include "WhatsUtility.h"

using namespace std;

WhatsEvent::WhatsEvent()
    :m_reqSize(0), m_reqSendPos(0)
    ,m_fd(0), m_isSendFinish(false)
    , m_shouldSend(false)
{
    
}

WhatsEvent::WhatsEvent(const WhatsEvent& event)
{
    m_req = event.m_req;
    m_rsp = event.m_rsp;
    m_sock = event.m_sock;

    m_reqSize = event.m_reqSize;
    m_reqSendPos = event.m_reqSendPos;

    m_isSendFinish = event.m_isSendFinish;

    m_rspFunc = event.m_rspFunc;

    m_fd = event.m_fd;
    m_shouldSend = event.m_shouldSend;
}

WhatsEvent::~WhatsEvent()
{

}

WhatsEvent& WhatsEvent::operator=(const WhatsEvent& other)
{
    if (this == &other) {
        return *this;
    }

    m_req = other.m_req;
    m_reqSendPos = other.m_reqSendPos;
    m_reqSize = other.m_reqSize;
    m_rsp = other.m_rsp;

    return *this;
}

void WhatsEvent::setReq(WhatsRequest &req)
{
    m_req = req;
    m_req.pack();
    setShouldSend(true);
}

void WhatsEvent::appendResponse(std::string &data)
{
    m_rsp.appendResponse(data);
}

int  WhatsEvent::getReqData(std::string &data)
{
    return m_req.getData(m_reqSendPos, data);
}

void  WhatsEvent::setSendPos(int pos)
{
    m_reqSendPos = pos;
}

int  WhatsEvent::onSendEvent()
{
    if (m_isSendFinish) {
        return EVENT_OK;
    }

    int sentSize = 0;
    std::string   data;
    m_req.getData(m_reqSendPos, data);
    int status = m_sock.sendData(data.c_str(), data.length(), sentSize);
    m_reqSendPos += sentSize;
    switch (status) {
    case WhatsSocket::SOCK_CLOSED:
        m_rspFunc(0, WhatsHttp::ERROR_CLOSED);
        status = EVENT_CLOSED;
        break;
    case WhatsSocket::SOCK_ERROR:
        m_rspFunc(0, WhatsHttp::ERROR_CLOSED);
        status = EVENT_CLOSED;
        break;
    case WhatsSocket::SOCK_OK:
        break;
    }

    if (m_reqSendPos == m_req.getDataLen()) {
        m_shouldSend = false;
    }

    return status;
}

int  WhatsEvent::onRecvEvent()
{
    char                            buffer[10 * 4096];
    bool                            hasClosed = false;
    int len = 10 * 4096;
    int recvedSize = 0;
    int status = m_sock.recvNoblockData(buffer, len, recvedSize);
    switch (status) {
    case WhatsSocket::SOCK_CLOSED:
       // m_rspFunc(0, WhatsHttp::ERROR_CLOSED);
        hasClosed = true;
        status = EVENT_CLOSED;
        break;
    case WhatsSocket::SOCK_ERROR:
        m_rspFunc(0, WhatsHttp::ERROR_CLOSED);
        status = EVENT_CLOSED;
        hasClosed = true;
        break;
    case WhatsSocket::SOCK_OK:

        break;
    }

    if (recvedSize > 0) {
        m_rsp.appendResponse(buffer, recvedSize);
        if (m_rsp.checkFull(hasClosed)) {
            m_rspFunc(&m_rsp, WhatsHttp::ERROR_OK);
            status = EVENT_DONE;
        }
    }


    return status;
}


void  WhatsEvent::setRspFunc(const std::function<void(WhatsResponse *rsp, int error)> rspFunc)
{
    m_rspFunc = rspFunc;
}

void  WhatsEvent::responseError()
{
    m_rspFunc(0, WhatsHttp::ERROR_SYS);
}

bool WhatsEvent::shouldSend()
{
    return m_shouldSend;
}

void WhatsEvent::setShouldSend(bool shouldSend)
{
    m_shouldSend = shouldSend;
}


bool WhatsEvent::createSock()
{
    return m_sock.create();
}

int  WhatsEvent::getSockFd()
{
    return m_sock.getSockFd();
}

bool WhatsEvent::initPeer()
{
    std::string url = m_req.getUrl();

    std::string ip;
    
    if (WhatsUtility::OK != WhatsUtility::getIpByUrl(m_req.getUrl(), ip)) {
        return false;
    }

    m_sock.setPeerIP(ip);
    m_sock.setPeerPort(80);

    return true;
}


bool WhatsEvent::connectPeer()
{
    return m_sock.connectPeer();
}

bool WhatsEvent::shouldRecv()
{
    return true;
}

