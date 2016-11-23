#include <stdio.h>
#ifdef _WIN32
#include <WinSock2.h>
#else
#include <unistd.h>
#endif
#include "src/WhatsHttp.h"


const char*  surl = "http://open.matchvs.com/wc4/getItemRank.do?&gameID=100011&item=scoreA&top=55";//"http://news.sohu.com/20161112/n472976161.shtml";
int main()
{
#ifdef WIN32
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }
#endif


    WhatsRequest           req;
    req.setType(WhatsRequest::WhatsHttpReqGet);
    std::string url = surl;
    req.setUrl(url);

    WhatsHttp::getInstance()->doRequest(req, [](WhatsResponse *rsp, int error) {
        if (!rsp) {
            printf("rsp error:%d\n", error);
        }
        else {
            std::string str = rsp->getResponse();
            printf("rsp length:%d\n", str.length());
            //printf("rsp:%s", str.c_str());
            FILE* file;
            char buffer[1024];
            sprintf(buffer, "./test1.html", surl);
#ifdef _WIN32
            fopen_s(&file, buffer, "wb");
#else
            file = fopen(buffer, "wb");
#endif
            if (file) {
                fwrite(str.c_str(), 1, str.length(), file);
                fclose(file);
            }
        }
    });


    
    while (1) {
#ifdef _WIN32
        Sleep(1999);
#else
        usleep(1000);
#endif
    }
}