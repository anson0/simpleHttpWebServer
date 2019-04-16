//
// Created by anson on 19-4-12.
//
#pragma once
#include <SafeQueue.h>

#ifndef THREAD_POOL_THREADPOOLBAR_H
#define THREAD_POOL_THREADPOOLBAR_H

#endif //THREAD_POOL_THREADPOOLBAR_H

#include <thread>
#include <vector>
#include <condition_variable>
#include <mutex>
#include <future>
#include <unistd.h>
#include <string.h>
//#define  NUMTHREADS 5
class threadPool{
public:
    threadPool(int cnt):m_bClose(false),m_vecThreads(std::vector<std::thread>(cnt)){}
    SafeQueue<std::function<void()>> m_queueTask;
    int nThreadNumber;
    std::mutex mu;
    std::condition_variable m_cond;
    bool m_shutdown;
    int m_bClose;
    class workerThread{
    public:
        workerThread(threadPool* ptr,int id){m_ptrPool=ptr;m_idNumber=id;};
        threadPool* m_ptrPool;
        bool bTemp;
        int m_idNumber;
        void operator()(){
             char buffTemp[100];
            std::function<void()> func;
             while(!m_ptrPool->m_bClose) {
                 //printf("enter into id[%d]\n",m_idNumber);
                 {
                     std::unique_lock<std::mutex> lock(m_ptrPool->mu);
                     if (m_ptrPool->m_queueTask.empty()) {
                         m_ptrPool->m_cond.wait(lock);//  wait(&m_ptrPool->mu);
                     }
                     //printf("queue size %d\n",m_ptrPool->m_queueTask.size());

                     bTemp = m_ptrPool->m_queueTask.dequeue(func);
                     //snprintf(buffTemp, sizeof(buffTemp), "id[%d]\n", m_idNumber);

                     //write(STDOUT_FILENO, buffTemp, strlen(buffTemp));
                 }
                 //flush();
                 if(bTemp== true)
                     func();


             }
        };
    };
    std::vector<std::thread> m_vecThreads;
    void initialize(){
        for(int i=0;i<m_vecThreads.size();i++)
        {
            //std::shared_ptr<workerThread>ptr(std::make_shared<workerThread>(this,i));
            m_vecThreads[i]=std::thread(workerThread(this,i));
        }

    };

    template <class F,class... Args>
    auto submit(F&& f,Args&&... args)->void{
        std::function<decltype(f(args...))()>func=std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        std::function<void()> f2=[func](){func();};
        m_queueTask.enqueue(f2);
        m_cond.notify_one();
        return;
    }
    void closePool(){
        m_bClose=true;
        m_cond.notify_all();
        for(int i=0;i<m_vecThreads.size();i++)
        {
            if(m_vecThreads[i].joinable())
                m_vecThreads[i].join();
        }
    }




};
