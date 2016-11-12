#include "WhatsHttp.h"
#include "WhatsEventCycle.h"
static WhatsHttp* g_single = NULL;

WhatsHttp::WhatsHttp()
{
    WhatsEventCycle::getInstance()->start();
}

WhatsHttp::~WhatsHttp()
{

}

WhatsHttp*  WhatsHttp::getInstance()
{
    static WhatsHttp g_http;
    return &g_http;
}

void   WhatsHttp::doRequest(WhatsRequest &req, const std::function<void(WhatsResponse *rsp, int error)> &rspFunc)
{
    WhatsEventCycle::getInstance()->addEvent(req, rspFunc);
}

