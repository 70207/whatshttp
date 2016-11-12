#include <stdio.h>
#include <WinSock2.h>
#include "src/WhatsHttp.h"


const char*  surl = "http://blog.csdn.net/a2011480169/article/details/51588253";//"http://news.sohu.com/20161112/n472976161.shtml";
int main()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }


    WhatsRequest           req;
    req.setType(WhatsRequest::WhatsHttpReqGet);
    std::string url = surl;
    req.setUrl(url);

    WhatsHttp::getInstance()->doRequest(req, [](WhatsResponse *rsp, int error) {
        if (!rsp) {
            printf("rsp error:%d", error);
        }
        else {
            std::string str = rsp->getResponse();
            printf("rsp length:%d", str.length());
            //printf("rsp:%s", str.c_str());
            FILE* file;
            char buffer[1024];
            sprintf(buffer, "test.html", surl);
            fopen_s(&file, buffer, "wb");
            if (file) {
                fwrite(str.c_str(), 1, str.length(), file);
                fclose(file);
            }
        }
    });
    
    
    while (1) {
        Sleep(1999);
    }
}