#include "WhatsRequest.h"
#include "WhatsUtility.h"

WhatsRequest::WhatsRequest()
{


}

WhatsRequest::~WhatsRequest()
{

}

WhatsRequest &WhatsRequest::operator= (const WhatsRequest &other){
    if (this == &other) {
        return *this;
    }

    m_url = other.m_url;
    m_kv = other.m_kv;
    m_type = other.m_type;
    m_data = other.m_data;

    return *this;
   
}


void  WhatsRequest::setUrl(std::string& url)
{
    m_url.assign(url);
}

std::string WhatsRequest::getUrl()
{
    return m_url;
}

void  WhatsRequest::addHeader(std::string& key, std::string& value)
{
    m_kv.insert(std::pair<std::string, std::string>(key, value));
}

void  WhatsRequest::setType(WhatsHttpReqType type)
{
    m_type = type;
}

void  WhatsRequest::pack()
{
    if (!m_data.empty()) {
        return;
    }

    //todo
    std::string domain, param;

    WhatsUtility::getDomainParamByUrl(m_url, domain, param);

    char buffer[4096];
    sprintf(buffer, "GET /%s HTTP/1.1 \r\nHost: %s\r\n\
Connection: keep-alive\r\n\
Accept-Encoding: gzip, deflate, sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n", param.c_str(), domain.c_str());

    printf("%s\n", buffer);
    m_data = buffer;
//    m_data = "GET / HTTP/1.1 \r\n\
//Host: www.qq.com\r\n\
//Connection: keep-alive\r\n\
//Upgrade-Insecure-Requests: 1\r\n\
//User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36\r\n\
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n\
//Accept-Encoding: gzip, deflate, sdch\r\n\
//Accept-Language: zh-CN,zh;q=0.8\r\n\r\n";

}

int  WhatsRequest::getData(int pos, std::string &data)
{
    if (pos >= m_data.length()) {
        return 0;
    }

    if (pos < 0) {
        return 0;
    }

    data.assign(m_data.c_str() + pos);
    return m_data.length() - pos;
}

int   WhatsRequest::getDataLen()
{
    return m_data.length();
}

