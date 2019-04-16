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
#include "threadPoolBarMain.h"
class Server;
class epoll {

public:
    std::weak_ptr<Server> m_ptrServer;
    //vector<struct epoll_event> m_mapEvents;
    //int m_cntEvents;
    //epoll(int numThreadsMain,int cnt=NUMPOLLEVENTS):m_numThreads(numThreadsMain),poolMain(m_numThreads,m_pCondMain),m_bEventHappen(false){
    epoll(int cnt=NUMPOLLEVENTS){
    //////thread pool for main//////////////////
//            m_pCondMain=std::make_shared<std::condition_variable>();
//            poolMain.m_pCond=m_pCondMain;
//            poolMain.initialize();
//////thread pool for main//////////////////
            m_epollFd=epoll_create(cnt);
            //m_mapEvents.resize(MAXCONNECT);
            //m_cntEvents=0;

    }
//    ~epoll(){poolMain.closePool();}

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
    void handlingEvent(struct epoll_event event);



private:
    int m_epollFd;
    int m_listenSocket;
//    int m_numThreads;
//    std::shared_ptr<std::condition_variable> m_pCondMain;
//    std::mutex              m_muMain;
//    threadPoolMain poolMain;
//    bool m_bEventHappen;



};


#endif //THREAD_POOL_EPOLL_H
