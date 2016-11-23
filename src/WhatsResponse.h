#ifndef _WHATS_RESPONSE_H__
#define _WHATS_RESPONSE_H__

#include <string>

class WhatsResponse
{
public:
    WhatsResponse();
    ~WhatsResponse();

    WhatsResponse& operator=(const WhatsResponse &);

    std::string getResponse();
    void appendResponse(std::string &data);
    void appendResponse(char *buffer, int size);
    inline void setError(int error)
    {
        m_error = error;
    }

    inline int getError()
    {
        return m_error;
    }


    bool checkFull(bool hasClosed);

    void decodeData();

private:
    void decodeGzipChuncked();
    void decodeGzipContent(int size);

    void decodeChuncked();
    void decodeContent(int size);
    
private:
    std::string m_data;
    std::string m_body;
    std::string m_header;
    int     m_contentLen;
    int     m_error;
    bool    m_bodyChecked;
    bool    m_bodyChuncked;
};








#endif