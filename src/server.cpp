//
// Created by anson on 19-4-14.
//

#include "server.h"
void Server::init(){
    m_listenFd_=socket_bind_listen(m_port_);
    if(m_listenFd_==-1)
    {
        std::cout<<"fail to listen\n"<<std::endl;
        shutdown();
        return;

    }
    setSocketNonBlocking(m_listenFd_);
    m_data.obtainListenSocket();//then make handling ready
    m_epoll.m_ptrServer=getSharedServer();
    m_data.m_ptrServer=getSharedServer();
    m_data.setListenSocket(m_listenFd_);
    m_epoll.epoll_add(m_listenFd_);

}
