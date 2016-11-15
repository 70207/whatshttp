#WhatsHttp

whatshttp is a client http lib.
it can deal http's busi asynchronously;

right now ,it can be used on windows and linux.

api Class:

```c++
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
  
```
you can use it just like sample in main.cpp:

```c++
  WhatsHttp::getInstance()->doRequest(req, [](WhatsResponse *rsp, int error) {
        if(!rsp){
            printf("rsp error:%d\n", error);
        }
        else{
            std::string str = rsp->getResponse();
            printf("rsp length:%d\n", str.length());
        }
         
           
    });
    
```

