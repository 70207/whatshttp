#ifndef _WHATS_REQUEST_H__
#define _WHATS_REQUEST_H__

#include <string>
#include <map>


class WhatsRequest {
public:
    WhatsRequest();
    ~WhatsRequest();

    WhatsRequest &operator = (const WhatsRequest &);

    typedef enum {
        WhatsHttpReqGet=0,
        WhatsHttpReqSet=1
    }WhatsHttpReqType;

    void  setUrl(std::string& url);
    std::string getUrl();
    void  addHeader(std::string& key, std::string& value);
    void  setType(WhatsHttpReqType type);
    void  pack();
    

    int   getData(int pos, std::string &data);
    int   getDataLen();

private:



    std::string                                    m_url;
    std::map<std::string, std::string>             m_kv;
    WhatsHttpReqType                               m_type;
    

    std::string                                    m_data;

};







#endif