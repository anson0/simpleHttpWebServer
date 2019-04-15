//
// Created by anson on 19-4-13.
//

#ifndef THREAD_POOL_HTTPDATA_H
#define THREAD_POOL_HTTPDATA_H

//#include "server.h"
#include <mutex>
#include <unordered_map>
#include "macros.h"
#include <map>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <memory>
#include <unistd.h>
class Server;

using namespace std;
enum HttpMethod
{
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD
};
enum ProcessState
{
    STATE_PARSE_URI = 1,
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH
};
enum URIState
{
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS,
};

enum HeaderState
{
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR
};

enum AnalysisState
{
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR
};

enum ParseState
{
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF
};
enum HttpVersion
{
    HTTP_10 = 1,
    HTTP_11
};

class MimeType
{
private:
    static void init();
    static std::unordered_map<std::string, std::string> mime;
    MimeType();
    MimeType(const MimeType &m);

public:
    static std::string getMime(const std::string &suffix);

private:
    static pthread_once_t once_control;
};
class httpData {
public:
    std::weak_ptr<Server> m_ptrServer;
    std::mutex m_muData;
    std::unordered_map<int,std::string> m_mapFd;//[MAXCONNECT];

    httpData(std::shared_ptr<Server> && temp){
        m_ptrServer=temp;
        //m_mapFd.resize(MAXCONNECT);


    }
    void acceptConnection(int fd);

    int readAllData(const int& fd,int& bFlagReturn,char* pBuff,int nCountMax=MAXREADBYTES);
    void readingEvent(struct epoll_event& event)
    {
//        if(event.events&EPOLLIN)
//        {
//            if(event.data.fd==m_listenSocket)
//            {
//                acceptConnection(m_listenSocket);
//
//            }
//            else{
//                char buffer[MAXREADBYTES+1];
//                int bFlagReturn=0;
//                int nCountRead=readAllData(event.data.fd,bFlagReturn,buffer);
//                if(bFlagReturn==-1)//not received data
//                {
//
//                }
//                else
//                {
//                    //parse the received data
//                    parseRecvMsg(event.data.fd);
//                }
//
//            }
//        }
        parseRecvMsg(event.data.fd);
    }

    void obtainListenSocket();
    bool checkRequestComplete(string& str,int& nRequestEndPos){
        nRequestEndPos=str.find("\r\n\r\n");
        if(nRequestEndPos==std::string::npos)
            return false;
        return true;

    };
    void response(int fd,std::string &outBuffer_){
        if (outBuffer_.size() > 0)
        {

            int nCountOutBuffer=outBuffer_.size();
            const char* ptrToTransfer=outBuffer_.c_str();
            int nLeftCountToTransfer=nCountOutBuffer;
            //if(nCountOutBuffer>MAXREADBYTES)
            do{
                int nCountEachTime;
                nCountEachTime=write(fd, ptrToTransfer,std::min(nLeftCountToTransfer,MAXREADBYTES));
                if(nCountEachTime<0)
                {
                    if(errno==EAGAIN)
                        break;
                    else if(errno==EINTR)
                        continue;
                    else
                    {
                        perror("write error");
                        std::cout<<strerror(errno)<<std::endl;
                        break;
                    }
                }
                nLeftCountToTransfer-=nCountEachTime;
                ptrToTransfer+=nCountEachTime;
            }while(nLeftCountToTransfer>0);
        }
    }
    URIState parseURI( string& strRequest, int& method,string& strFileName,int& HTTPVersion_);
    HeaderState parseHeaders( string& strRequest,std::map<string,string>& sMapHeaderInfo,int& hstate);
    AnalysisState analysisRequest(const int fd,int & method_,map<string,string>& headers_,string& fileName,string& outBuffer_);
    void parseRecvMsg(int fd);
    void handleError(int fd, int err_num, string short_msg);
    void setListenSocket(int fd){
        m_listenSocket=fd;
    }
private:
    int m_listenSocket;

};


#endif //THREAD_POOL_HTTPDATA_H
