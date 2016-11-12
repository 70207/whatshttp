#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <zlib.h>

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
#include <Ws2tcpip.h>
#define close(s) closesocket(s)
#define stricmp _stricmp
#endif

#include "WhatsUtility.h"
#include "WhatsLog.h"

using namespace std;



WhatsUtility::WhatsUtility()
{

}

WhatsUtility::~WhatsUtility()
{

}

int WhatsUtility::getIpByUrl(std::string &url, std::string &ip)
{
    int status = checkUrlFormat(url);
    if (status != OK) {
        return status;
    }

    std::string    domain, para;
    status = getDomainParamByUrl(url, domain,para);
    if (status != OK) {
        return status;
    }

    return getIpByDomain(domain, ip);
}

int WhatsUtility::getIpByDomain(std::string &domain, std::string &ip)
{
  /*  struct hostent *remoteHost;
    char **pptr;
    char str[32];
    int i = 0;
    remoteHost = gethostbyname(domain.c_str());

   
    if (remoteHost == NULL) {
        return DOMAIN_GET_IP_FAILED;
    }
    else {
        for (pptr = remoteHost->h_addr_list; *pptr != NULL; pptr++)
        {
            if (NULL != inet_ntop(remoteHost->h_addrtype, *pptr, str, 32))
            {
                ip = str;
                return OK; 
            }
        }
    }*/


    struct addrinfo *answer, hint, *curr;
    char str[32];
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    int ret = getaddrinfo(domain.c_str(), NULL, &hint, &answer);
    if (ret != 0) {
        return DOMAIN_GET_IP_FAILED;
    }

    for (curr = answer; curr != NULL; curr = curr->ai_next) {
        if (((struct sockaddr_in *)(curr->ai_addr))->sin_addr.s_addr == 0) {
            continue;
        }
        inet_ntop(AF_INET,
            &(((struct sockaddr_in *)(curr->ai_addr))->sin_addr),
            str, 16);
        ip = str;
        freeaddrinfo(answer);
        return OK;
    }

    freeaddrinfo(answer);

    return DOMAIN_GET_IP_FAILED;

}

int WhatsUtility::getChCountFromString(char c, std::string &str)
{
    int   count = 0;
    const char* cs = str.c_str();

    while (*cs) {
        if (*cs == c) {
            count++;
        }
        cs++;
    }

    return count;
}




int WhatsUtility::compressGzipInit(unsigned char* dst, int dstsize)
{
    m_strm = { 0 };
    m_strm.zalloc = Z_NULL;
    m_strm.zfree = Z_NULL;
    m_strm.opaque = Z_NULL;
    m_strm.avail_in = 0;
    m_strm.next_in = Z_NULL;
    int ret = deflateInit2(&m_strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS + 16, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK) {
        return DEPRESS_ERROR;
    }

    m_strm.avail_out = dstsize;
    m_strm.next_out = (Bytef*)dst;

    return OK;
}

int WhatsUtility::compressGzipEnd(int &size)
{
    size = m_strm.total_out;
    inflateEnd(&m_strm);
    return OK;
}

int WhatsUtility::compressGzip(const char* src, int size)
{
    m_strm.avail_in = size;
    m_strm.next_in = (Bytef*)src;
    int ret = deflate(&m_strm, Z_NO_FLUSH);
    if (ret != Z_OK) {
        WHATSLOG("gzip init error!");
    }

    return 0;
}

int WhatsUtility::depressGzipInit(unsigned char* dst, int dstsize)
{
    m_strm = { 0 };
    m_strm.zalloc = Z_NULL;
    m_strm.zfree = Z_NULL;
    m_strm.opaque = Z_NULL;
    m_strm.avail_in = 0;
    m_strm.next_in = Z_NULL;
    int ret = inflateInit2(&m_strm, MAX_WBITS + 16);
    if (ret != Z_OK) {
        return DEPRESS_ERROR;
    }

    m_strm.avail_out = dstsize;
    m_strm.next_out = (Bytef*)dst;
    

    return OK;
}

int WhatsUtility::depressGzipEnd(int &size)
{
    size = m_strm.total_out;
    inflateEnd(&m_strm);
    return OK;
}

int WhatsUtility::depressGzip(const char* src, int size)
{
    m_strm.avail_in = size;
    m_strm.next_in = (Bytef*)src;
    int ret = inflate(&m_strm, Z_NO_FLUSH);
    if (ret != Z_OK && ret != Z_STREAM_END) {
        WHATSLOG("gzip depress error!");
    }

    return 0;
}

int  WhatsUtility::checkUrlFormat(std::string &url)
{
    int len = url.length();
    if (len < 9) {
        return URL_FORMAT_WRONG;
    }

    std::string  bg = url.substr(0, 7);
    static const char* http = "http://";
    
    if (stricmp(bg.c_str(), http) != 0) {
        return URL_FORMAT_WRONG;
    }

    return OK;
}



int  WhatsUtility::getDomainParamByUrl(std::string &url, std::string &domain, std::string &para)
{

    int domainFirstPos = 0;
    int firstpos = 0;
    int secondpos = 0;
    int thirdpos = 0;
    const char* urlstr = url.c_str();
    

    if (url.length() > 7) {
        if (((urlstr[0] | 32) == 'h') &&     //check http://
            ((urlstr[1] | 32) == 't' )&&
            ((urlstr[2] | 32) == 't') &&
            ((urlstr[3] | 32) == 'p' )&&
            (urlstr[4] == ':') &&
            (urlstr[5] == '/') &&
            (urlstr[6] == '/')) {
            firstpos = 5;
            secondpos = 6;
        }
        else {
            firstpos = 0;
            secondpos = 0;
        }
    }


    thirdpos = url.find("/", secondpos+1);
    if (thirdpos <= 0) {
        domain = url.substr(secondpos+1);
    }
    else {
        domain = url.substr(secondpos+1, thirdpos-secondpos - 1);
        para = url.substr(thirdpos+1);
    }

    return OK;
   
}

