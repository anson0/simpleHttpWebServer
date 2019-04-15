//
// Created by anson on 19-4-13.
//
#ifndef SERVER_THREAD_POOL__
#define SERVER_THREAD_POOL__ 
#include "threadPoolBar.h"
#include "Util.h"
#include <mutex>
#include <condition_variable>
#include "epoll.h"
#include "httpData.h"
#include <memory>
//#pragma once
class Server:public enable_shared_from_this<Server>{

public:
    threadPool pool;
    epoll m_epoll;
    httpData m_data;
    std::mutex mu;
    Server(int numThreadsInPool,int port):pool(numThreadsInPool),m_port_(port),m_data(NULL),m_epoll()
    {
        pool.initialize();

    }

    void init();
//    {
//        m_listenFd_=socket_bind_listen(m_port_);
//        if(m_listenFd_==-1)
//        {
//            std::cout<<"fail to listen\n"<<std::endl;
//            shutdown();
//            return;
//
//        }
//        setSocketNonBlocking(m_listenFd_);
//        m_data.obtainListenSocket();//then make handling ready
//        m_epoll.m_ptrServer=getSharedServer();
//        m_data.m_ptrServer=getSharedServer();
//        m_epoll.epoll_add(m_listenFd_);
//
//    }
    int  start(){
       return  m_epoll.poll();

    }
    void shutdown()
    {
       pool.closePool();
    }


    std::shared_ptr<Server> getSharedServer(){
        return shared_from_this();

    };
    int getListenSocket(){return m_listenFd_;}

private:
    int m_listenFd_;
    int m_port_;
    bool m_closePool;


};
class worker{

};



#endif