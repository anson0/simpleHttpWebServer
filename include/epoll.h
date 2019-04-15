//
// Created by anson on 19-4-13.
//

#ifndef THREAD_POOL_EPOLL_H
#define THREAD_POOL_EPOLL_H
#include <sys/epoll.h>
#include <memory>
//#include "server.h"
#include "macros.h"
#include <mutex>
class Server;
class epoll {

public:
    std::weak_ptr<Server> m_ptrServer;
    //vector<struct epoll_event> m_mapEvents;
    //int m_cntEvents;
    epoll(int cnt=NUMPOLLEVENTS){
        m_epollFd=epoll_create(cnt);
        //m_mapEvents.resize(MAXCONNECT);
        //m_cntEvents=0;
    }

    void epoll_add(int fd);

    void epoll_del(int fd)
    {


        if(epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, NULL) < 0)
        {
            perror("epoll_del error");

        }
        return;
    }

    int poll();


private:
    int m_epollFd;
    int m_listenSocket;


};


#endif //THREAD_POOL_EPOLL_H
