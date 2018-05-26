#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "event.h"
#define CPU_NUM 3
class CTcpServer
{
public:
    CTcpServer(const char *ip, unsigned short port);
    ~CTcpServer();
    void run();
private:
    int mListenfd;
    struct event_base* mBase;
    //创建主次线程通信用的管道
    int sktpr[3][2];
    
    friend void listen_cb(int fd, short event, void *lparg);
};


#endif
