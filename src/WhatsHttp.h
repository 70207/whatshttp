#ifndef _WHATS_HTTP_H__
#define _WHATS_HTTP_H__

#include <functional>
#include "WhatsRequest.h"
#include "WhatsResponse.h"


class WhatsHttp
{
public:
    ~WhatsHttp();
    static WhatsHttp *getInstance();

    void   doRequest(WhatsRequest &req, const std::function<void(WhatsResponse *rsp, int error)> &rspFunc);

    static const int ERROR_SYS = -2;
    static const int ERROR_CLOSED = -1;
    static const int ERROR_OK = 0;
private:
    WhatsHttp();

};










#endif