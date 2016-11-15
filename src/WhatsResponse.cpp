#include <stdio.h>
#include <string.h>
#include "WhatsLog.h"
#include "WhatsResponse.h"
#include "WhatsUtility.h"

#ifdef _WIN32
#define stricmp _stricmp
#else
#define stricmp strcasecmp
#endif

WhatsResponse::WhatsResponse()
    :m_error(0),m_contentLen(-1)
    , m_bodyChecked(false), m_bodyChuncked(false)
{

}

WhatsResponse::~WhatsResponse()
{

}

WhatsResponse& WhatsResponse::operator=(const WhatsResponse &other)
{
    if (this == &other) {
        return *this;
    }

    m_data = other.m_data;

    return *this;
   
}



std::string WhatsResponse::getResponse()
{
    if (m_bodyChecked) {
        return m_body;
    }

    int pos = m_header.find("Content-Encoding");
    if (pos <= 0) {
        m_bodyChecked = true;
        return m_body;
    }

    int len = sizeof("Content-Encoding");

    const char* ch = m_header.c_str() + pos + len;
    pos += len;
    while (*ch == ' ') {
        pos++;
        ch++;
    }

    int rnp = m_header.find("\r\n", pos);
    if (rnp <= 0) {
        m_bodyChecked = true;
        return m_body;
    }

    std::string ec = m_header.substr(pos, rnp - pos);

    if (stricmp(ec.c_str(), "gzip")) {
        //todo
    }

    return m_body;
}

void WhatsResponse::appendResponse(std::string &data)
{
    m_data.append(data);
}

void WhatsResponse::appendResponse(char *buffer, int size)
{
    m_data.append(buffer, size);
}

bool  WhatsResponse::checkFull(bool hasClosed)
{

    int length = m_data.length();
    if (length < 10) {
        return false;
    }

    int bodyPos = m_data.find("\r\n\r\n");
    if (bodyPos <= 0) {
        return false;
    }

    bodyPos += 4;

    if (m_contentLen <= 0) {
        int fpos = m_data.find("Content-Length:");
        if (fpos > 0) {
            const char* nump = m_data.c_str() + fpos + strlen("Content-Length:");
            while (*nump == ' ') {
                nump++;
            }
            m_contentLen = atoi(nump);
           
        }
    }

    if (m_contentLen > 0 && !hasClosed) {
        if (m_data.length() - bodyPos < m_contentLen) {
            return false;
        }
    }

 
    int lastPos = 0; 
    if (m_data.length() - bodyPos < m_contentLen) {
        lastPos = m_data.find("\r\n\r\n", bodyPos);
        if (lastPos <= 0 && !hasClosed) {
            return false;
        }
    }
   
    if (lastPos <= 0) {
        lastPos = m_data.length() - 1;
    }


    if (m_header.length() == 0) {
        m_header = m_data.substr(0, bodyPos);
    }

    if (m_header.length() == 0) {
        WHATSLOG("header is error\n");
        return false;
    }


    if (m_contentLen <= 0) {
        m_bodyChuncked = true;
        m_contentLen = lastPos - bodyPos;
    }

    m_body = m_data.substr(bodyPos/*, m_contentLen*/);

    decodeData();

    return true;


}
#define ZLIB_BUFFER_LEN_RESERVE    409600

void WhatsResponse::decodeData()
{
    if (m_header.find("gzip") > 0) {
        if (m_bodyChuncked) {
            decodeGzipChuncked();
        }
        else {
            decodeGzipContent(m_contentLen);
        }
    }
}

void WhatsResponse::decodeGzipContent(int size)
{
    unsigned char buffer[ZLIB_BUFFER_LEN_RESERVE];

    WhatsUtility     utility;
    utility.depressGzipInit(buffer, ZLIB_BUFFER_LEN_RESERVE);

    utility.depressGzip(m_body.c_str(), size);
  
    if (WhatsUtility::OK != utility.depressGzipEnd(size)) {
        m_body.assign((const char*)buffer, size);
    }
}

void WhatsResponse::decodeGzipChuncked()
{
    unsigned char buffer[ZLIB_BUFFER_LEN_RESERVE];

    WhatsUtility     utility;
    utility.depressGzipInit(buffer, ZLIB_BUFFER_LEN_RESERVE);

    int fpos = 0;
    while (1) {
        int dilpos = m_body.find("\r\n", fpos);
        if (dilpos <= 0) {
            break;
        }
        std::string        countstr = m_body.substr(fpos, dilpos - fpos);
        int count = strtol(countstr.c_str(), 0, 16);
        if (count <= 0) {
            break;
        }

        utility.depressGzip(m_body.c_str() + dilpos + 2, count);
        fpos = dilpos + 2 + count + 2;
    }

    int size = 0;
    if (WhatsUtility::OK != utility.depressGzipEnd(size)) {
        m_body.assign((const char*)buffer, size);
    }

    
}