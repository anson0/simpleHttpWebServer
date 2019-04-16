//
// Created by anson on 19-4-13.
//

#include "epoll.h"
#include <memory>
#include "server.h"
void epoll::epoll_add(int fd)
{
    struct epoll_event event;
    epoll_data_t data;
    data.fd=fd;
    event.data=data;
    event.events=EPOLLIN;
    static bool bFlagListen=true;

    if(epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error");
        return;
    }
    //if(event.fd>MAXCONNECT-1||m_cntEvents>MAXCONNECT-1)
    if(fd>MAXCONNECT-1)
    {
        perror("max connect already\n");
        return;
    }
    //if(m_cntEvents!=0)//not log listen socket
    if(bFlagListen==false)
    {
        if(std::shared_ptr<Server> temp=m_ptrServer.lock()) {
            //if(temp.m_data.m_mapFd.find(event.fd)==temp.m_date.m_mapFd.end())
            temp->m_data.m_mapFd[fd] = string("");
        }
    }
    if(auto temp=m_ptrServer.lock()) {
//        std::unique_lock <std::mutex> lock(temp->mu);
        //m_mapEvents[m_cntEvents] = event;
        //m_cntEvents++;
    }
    bFlagListen=false;

}

int epoll::poll(){
    struct epoll_event events[MAX_EVENTS];
    while (true)
    {

        struct epoll_event ev;
        for(int i=0;i<MAX_EVENTS;i++)
            events[i].data.fd=-1;
        //////thread pool for main//////////////////
//        if(m_bEventHappen!=false&&poolMain.m_cntEvents!=0)
//        {
//            std::unique_lock<std::mutex>lock(m_muMain);
//            m_pCondMain->wait(lock);
//        }
//////thread pool for main//////////////////
        volatile  int event_count = epoll_wait(m_epollFd, events, MAX_EVENTS, -1);//anson add volatile debug
        if (event_count < 0)
            perror("epoll wait error");
        auto temp=m_ptrServer.lock();
        if(!temp)
        {
            perror("epoll get server failure\n");
            return -1;

        }
        //printf("event cnt is [%d]\n",event_count);//anson debug
        //////thread pool for main//////////////////
//        if(event_count>0)
//        {
//            m_bEventHappen=true;
//            poolMain.m_cntEvents=event_count;
//        }
//////thread pool for main//////////////////
        for(int i=0;i<event_count;i++)
        {
            //////thread pool for main//////////////////
//            auto request=[events,i,this](){
//                this->handlingEvent(events[i]);
//            };
//            poolMain.submit(request);
           ////thread pool for main//////////////////
//            auto request=[events,i,temp]( )
//            {
//                struct epoll_event e=events[i];
//                temp->m_data.readingEvent(e);
//
//            };
//            printf("add event fd [%d]\n",events[i].data.fd);
//            temp->pool.submit(request);
///////////////////////////////////////////////////

            if(events[i].data.fd==temp->getListenSocket())
            {
                temp->m_data.acceptConnection(events[i].data.fd);
            } else
            {
                //drain the coming data
                //std::shared_ptr<string> sBuff;
                char pBuff[MAXREADBYTES];
                int nCount;
                bool bEventDealingFinish=false;
                do{
                    nCount = read(events[i].data.fd, pBuff, MAXREADBYTES);
                    if(nCount<0)
                    {
                        if(errno==EAGAIN)
                        {
                            bEventDealingFinish=true;
                            break;
                        }
                        else if(errno==EINTR){
                            continue;
                        }
                        else{


                                {
//                                    std::unique_lock<std::mutex> lock(temp->mu);
                                    std::unique_lock<std::mutex> lock(temp->m_muArray[events[i].data.fd]);
                                    temp->m_data.m_mapFd[events[i].data.fd].clear();
                                    //temp->m_epoll.updateData(fd);
                                }
                               epoll_del(events[i].data.fd);
                               close(events[i].data.fd);
                            perror("ooops error happens");
                            //bFlagReturn=-1;
                            break;

                        }
                    }
                    else if(nCount==0) {

                        //bFlagReturn = -1;

                            {
//                                std::unique_lock<std::mutex> lock(temp->mu);
                                std::unique_lock<std::mutex> lock(temp->m_muArray[events[i].data.fd]);
                                temp->m_data.m_mapFd[events[i].data.fd].clear();
                                //temp->m_epoll.updateData(fd);
                            }
                            temp->m_epoll.epoll_del(events[i].data.fd);
                            close(events[i].data.fd);

                    }
                    {
//                        std::unique_lock<std::mutex> lock(temp->mu);
                        std::unique_lock<std::mutex> lock(temp->m_muArray[events[i].data.fd]);
                        temp->m_data.m_mapFd[events[i].data.fd].append(pBuff,nCount);
                    }
                    //nCountSum+=nCount;
                }while(nCount>0);
                if(bEventDealingFinish==true)
                {
                    auto request=[events,i,temp]( )
                    {
                        struct epoll_event e=events[i];
                        temp->m_data.readingEvent(e);

                    };
//                    printf("add event fd [%d]\n",events[i].data.fd);
                    temp->pool.submit(request);

                }
            }
        }
    }

}


void epoll::handlingEvent(struct epoll_event event){
    auto temp=m_ptrServer.lock();
    if(!temp)
    {
        perror("epoll get server failure\n");
        return ;

    }
    if(event.data.fd==temp->getListenSocket())
    {
        temp->m_data.acceptConnection(event.data.fd);
    } else
    {
        //drain the coming data
        //std::shared_ptr<string> sBuff;
        char pBuff[MAXREADBYTES];
        int nCount;
        bool bEventDealingFinish=false;
        do{
            nCount = read(event.data.fd, pBuff, MAXREADBYTES);
            if(nCount<0)
            {
                if(errno==EAGAIN)
                {
                    bEventDealingFinish=true;
                    break;
                }
                else if(errno==EINTR){
                    continue;
                }
                else{


                    {
//                                    std::unique_lock<std::mutex> lock(temp->mu);
                        std::unique_lock<std::mutex> lock(temp->m_muArray[event.data.fd]);
                        temp->m_data.m_mapFd[event.data.fd].clear();
                        //temp->m_epoll.updateData(fd);
                    }
                    epoll_del(event.data.fd);
                    close(event.data.fd);
                    perror("ooops error happens");
                    //bFlagReturn=-1;
                    break;

                }
            }
            else if(nCount==0) {

                //bFlagReturn = -1;

                {
//                                std::unique_lock<std::mutex> lock(temp->mu);
                    std::unique_lock<std::mutex> lock(temp->m_muArray[event.data.fd]);
                    temp->m_data.m_mapFd[event.data.fd].clear();
                    //temp->m_epoll.updateData(fd);
                }
                temp->m_epoll.epoll_del(event.data.fd);
                close(event.data.fd);

            }
            {
//                        std::unique_lock<std::mutex> lock(temp->mu);
                std::unique_lock<std::mutex> lock(temp->m_muArray[event.data.fd]);
                temp->m_data.m_mapFd[event.data.fd].append(pBuff,nCount);
            }
            //nCountSum+=nCount;
        }while(nCount>0);
        if(bEventDealingFinish==true)
        {
            auto request=[event,temp]( )
            {

                temp->m_data.readingEvent(event);

            };
//                    printf("add event fd [%d]\n",events[i].data.fd);
            temp->pool.submit(request);

        }
    }

}