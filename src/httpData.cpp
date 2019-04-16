//
// Created by anson on 19-4-13.
//

#include "httpData.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Util.h"
#include "server.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
//#define _BSD_SOURCE
using namespace std;
pthread_once_t MimeType::once_control = PTHREAD_ONCE_INIT;
std::unordered_map<std::string, std::string> MimeType::mime;

void MimeType::init()
{
    mime[".html"] = "text/html";
    mime[".avi"] = "video/x-msvideo";
    mime[".bmp"] = "image/bmp";
    mime[".c"] = "text/plain";
    mime[".doc"] = "application/msword";
    mime[".gif"] = "image/gif";
    mime[".gz"] = "application/x-gzip";
    mime[".htm"] = "text/html";
    mime[".ico"] = "image/x-icon";
    mime[".jpg"] = "image/jpeg";
    mime[".png"] = "image/png";
    mime[".txt"] = "text/plain";
    mime[".mp3"] = "audio/mp3";
    mime["default"] = "text/html";
}

std::string MimeType::getMime(const std::string &suffix)
{
    pthread_once(&once_control, MimeType::init);
    if (mime.find(suffix) == mime.end())
        return mime["default"];
    else
        return mime[suffix];
}
void httpData::acceptConnection(int listen_fd)
{
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    //socklen_t client_addr_len = 0;
    socklen_t client_addr_len = sizeof(client_addr);
    int accept_fd = 0;
    while((accept_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_addr_len)) > 0)
    {
        cout << inet_ntoa(client_addr.sin_addr) << endl;
        cout << ntohs(client_addr.sin_port) << endl;
        /*
        // TCP的保活机制默认是关闭的
        int optval = 0;
        socklen_t len_optval = 4;
        getsockopt(accept_fd, SOL_SOCKET,  SO_KEEPALIVE, &optval, &len_optval);
        cout << "optval ==" << optval << endl;
        */
        // 限制服务器的最大并发连接数
        if (accept_fd >= MAXFDS)
        {
            close(accept_fd);
            continue;
        }
        int optval=1;
        if (setsockopt(accept_fd, SOL_SOCKET, SO_REUSEADDR,  &optval , sizeof(int)) < 0)
        {
            perror("setsockopt(SO_REUSEADDR) failed");
        }
        // 设为非阻塞模式
        int ret = setSocketNonBlocking(accept_fd);
        if (ret < 0)
        {
            perror("Set non block failed!");
            return;
        }
        setSocketNodelay(accept_fd);
        if(auto temp=m_ptrServer.lock())
        {
            temp->m_epoll.epoll_add(accept_fd);
        }

    }
    //if(accept_fd == -1)
    //   perror("accept");
}
int httpData::readAllData(const int& fd,int& bFlagReturn,char* pBuff,int nCountMax){
    int nCount=0;
    int nCountSum=0;
    do {
         nCount = read(fd, pBuff, nCountMax);

         if(nCount<0)
         {
             if(errno==EAGAIN)
             {
                 bFlagReturn=0;
                 break;
             }
             else if(errno==EINTR){
                 continue;
             }
             else{
                 close(fd);
                 if(auto temp=m_ptrServer.lock()) {
                     {
//                         std::unique_lock<std::mutex> lock(temp->mu);
                         std::unique_lock<std::mutex> lock(temp->m_muArray[fd]);
                         m_mapFd[fd].clear();
                         //temp->m_epoll.updateData(fd);
                     }
                     temp->m_epoll.epoll_del(fd);
                 }
                 perror("ooops error happens");
                 bFlagReturn=-1;
                 break;

             }
         }
         else if(nCount==0)
         {
             close(fd);
             bFlagReturn=-1;
             if(auto temp=m_ptrServer.lock())
             {
                 {
//                     std::unique_lock<std::mutex> lock(temp->mu);
                     std::unique_lock<std::mutex> lock(temp->m_muArray[fd]);
                     m_mapFd[fd].clear();
                     //temp->m_epoll.updateData(fd);
                 }
                 temp->m_epoll.epoll_del(fd);
//                 auto it=temp->m_epoll.m_mapEvents.begin();
//                 for(;it!=temp->m_epoll.m_mapEvents.end();it++)
//                 {
//                     if((it->data==fd))
//                     {
//                         break;
//                     }
//                 }
//                 if(it!=temp->m_epoll.m_mapEvents.end()) {
//                     {
//
//                         temp->m_epoll.epoll_del(fd);
//                         temp->m_epoll.m_mapEvents.erase(it);
//                         temp->m_epoll.m_cntEvents--;
//                     }
//
//                 }
             }
             break;
         }
        if(auto temp=m_ptrServer.lock())
        {
//            std::unique_lock<std::mutex> lock(temp->mu);
            std::unique_lock<std::mutex> lock(temp->m_muArray[fd]);
            temp->m_data.m_mapFd[fd].append(pBuff,nCount);
        }
         nCountSum+=nCount;
    }while(nCount>0);
    
    return nCountSum;


}


AnalysisState httpData::analysisRequest(const int fd_,int & method_,map<string,string>& headers_,string& fileName_,string& outBuffer_)
{


    if (method_ == METHOD_POST)
    {
       //currently disable this option
    }
    else if (method_ == METHOD_GET || method_ == METHOD_HEAD)
    {
        string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end() && (headers_["Connection"] == "Keep-Alive" || headers_["Connection"] == "keep-alive"))
        {
            //keepAlive_ = true;
            header += string("Connection: Keep-Alive\r\n") + "Keep-Alive: timeout=" + to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
        }
        int dot_pos = fileName_.find('.');
        string filetype;
        if (dot_pos < 0)
            filetype = MimeType::getMime("default");
        else
            filetype = MimeType::getMime(fileName_.substr(dot_pos));


        // echo test
        if (fileName_ == "hello")
        {
            outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
            return ANALYSIS_SUCCESS;
        }


        struct stat sbuf;
        if (stat(fileName_.c_str(), &sbuf) < 0)
        {
            header.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        header += "Content-Type: " + filetype + "\r\n";
        header += "Content-Length: " + to_string(sbuf.st_size) + "\r\n";
        header += "Server: LinYa's Web Server\r\n";
        // 头部结束
        header += "\r\n";
        outBuffer_ += header;

        if (method_ == METHOD_HEAD)
            return ANALYSIS_SUCCESS;

        int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        if (src_fd < 0)
        {
            outBuffer_.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        void *mmapRet = mmap(NULL, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if (mmapRet == (void *)-1)
        {
            munmap(mmapRet, sbuf.st_size);
            outBuffer_.clear();
            handleError(fd_, 404, "Not Found!");
            return ANALYSIS_ERROR;
        }
        char *src_addr = static_cast<char*>(mmapRet);
        outBuffer_ += string(src_addr, src_addr + sbuf.st_size);;
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}


void httpData::handleError(int fd, int err_num, string short_msg)
{
    short_msg = " " + short_msg;
    char send_buff[4096];
    string body_buff, header_buff;
    body_buff += "<html><title>哎~出错了</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += to_string(err_num) + short_msg;
    body_buff += "<hr><em> LinYa's Web Server</em>\n</body></html>";

    header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";
    header_buff += "Server: LinYa's Web Server\r\n";;
    header_buff += "\r\n";
    // 错误处理不考虑writen不完的情况
    sprintf(send_buff, "%s", header_buff.c_str());
    //writen(fd, send_buff, strlen(send_buff));
    response(fd,header_buff);
    sprintf(send_buff, "%s", body_buff.c_str());
    //writen(fd, send_buff, strlen(send_buff));
    response(fd,body_buff);
}


URIState httpData::parseURI( string& strRequest, int& method_,string& strFileName,int& HTTPVersion_)
{
    string& str = strRequest;
    string cop = str;
    // 读到完整的请求行再开始解析请求
    size_t pos = str.find('\r', 0);
    if (pos < 0)
    {
        return PARSE_URI_AGAIN;
    }
    // 去掉请求行所占的空间，节省空间
    string request_line = str.substr(0, pos);
    if (str.size() > pos + 1)
        str = str.substr(pos + 1);
    else
        str.clear();
    // Method
    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");

    if (posGet >= 0)
    {
        pos = posGet;
        method_ = METHOD_GET;
    }
    else if (posPost >= 0)
    {
        pos = posPost;
        method_ = METHOD_POST;
    }
    else if (posHead >= 0)
    {
        pos = posHead;
        method_ = METHOD_HEAD;
    }
    else
    {
        return PARSE_URI_ERROR;
    }

    // filename
    pos = request_line.find("/", pos);
    if (pos < 0)
    {
        strFileName = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    }
    else
    {
        size_t _pos = request_line.find(' ', pos);
        if (_pos < 0)
            return PARSE_URI_ERROR;
        else
        {
            if (_pos - pos > 1)
            {
                strFileName = request_line.substr(pos + 1, _pos - pos - 1);
                size_t __pos = strFileName.find('?');
                if (__pos >= 0)
                {
                    strFileName = strFileName.substr(0, __pos);
                }
            }

            else
                strFileName= "index.html";
        }
        pos = _pos;
    }
    //cout << "fileName_: " << fileName_ << endl;
    // HTTP 版本号
    pos = request_line.find("/", pos);
    if (pos < 0)
        return PARSE_URI_ERROR;
    else
    {
        if (request_line.size() - pos <= 3)
            return PARSE_URI_ERROR;
        else
        {
            string ver = request_line.substr(pos + 1, 3);
            if (ver == "1.0")
                HTTPVersion_ = HTTP_10;
            else if (ver == "1.1")
                HTTPVersion_ = HTTP_11;
            else
                return PARSE_URI_ERROR;
        }
    }
    return PARSE_URI_SUCCESS;
}


void httpData::parseRecvMsg(int fd){
    bool bFlagReuestComplete=false;
    int nIndexPosRequestEnd=0;
    bFlagReuestComplete=checkRequestComplete(m_mapFd[fd],nIndexPosRequestEnd);
    if(bFlagReuestComplete==false)
    {
        //handleError(fd, 404, "Bad Request!");
        return;
    }
    //chop off the request
    string strRequest=m_mapFd[fd].substr(0,nIndexPosRequestEnd+4);
    m_mapFd[fd].erase(0,nIndexPosRequestEnd+4);
    int method=-1;
    string strFileName;
    std::map<std::string,std::string> sMapHeaderInfo;
    bool error_=false;
    string outBuffer_;
    int httpVersion=HTTP_11;
    int hState=H_START;
    //ParseState hState_;
    int state_=STATE_PARSE_URI;
    do {
        if (state_ == STATE_PARSE_URI) {
            URIState flag = parseURI(strRequest, method, strFileName, httpVersion);
            if (flag == PARSE_URI_AGAIN)
                break;

            else if (flag == PARSE_URI_ERROR) {
                perror("2");


                error_ = true;
                handleError(fd, 400, "Bad Request");
                break;
            } else
                state_ = STATE_PARSE_HEADERS;
        }
        if (state_ == STATE_PARSE_HEADERS) {
            HeaderState flag = parseHeaders(strRequest, sMapHeaderInfo, hState);
            if (flag == PARSE_HEADER_AGAIN)
                break;

            else if (flag == PARSE_HEADER_ERROR) {
                perror("3");
                error_ = true;
                handleError(fd, 400, "Bad Request");
                break;
            }
            if (method == METHOD_POST) {
                // POST方法准备
                state_ = STATE_RECV_BODY;
            } else {
                state_ = STATE_ANALYSIS;
            }
        }
        if (state_ == STATE_RECV_BODY) {
            int content_length = -1;
            if (sMapHeaderInfo.find("Content-length") != sMapHeaderInfo.end()) {
                content_length = stoi(sMapHeaderInfo["Content-length"]);
            } else {
                //cout << "(state_ == STATE_RECV_BODY)" << endl;
                error_ = true;
                handleError(fd, 400, "Bad Request: Lack of argument (Content-length)");
                break;
            }
            if (static_cast<int>(strRequest.size()) < content_length)
                break;

            state_ = STATE_ANALYSIS;
        }
        if (state_ == STATE_ANALYSIS) {
            AnalysisState flag = this->analysisRequest(fd, method, sMapHeaderInfo, strFileName, outBuffer_);
            if (flag == ANALYSIS_SUCCESS) {
                state_ = STATE_FINISH;
                break;
            } else {
                //cout << "state_ == STATE_ANALYSIS" << endl;
                error_ = true;
                break;
            }
        }
    }while(false);
        //cout << "state_=" << state_ << endl;
        if (!error_) {
            response(fd, outBuffer_);
//            if (outBuffer_.size() > 0)
//            {
//                handleWrite();
//                int nCountOutBuffer=outBueer_.size();
//                const char* ptrToTransfer=outBuffer.c_str();
//                int nLeftCountToTransfer=nCountOutBuffer;
//                //if(nCountOutBuffer>MAXREADBYTES)
//                do{
//                     int nCountEachTime;
//                     nCountEachTime=write(fd, ptrtToTransfer,std::min(nLeftCountToTransfer,MAXREADBYTES));
//                     if(nCountEachTime<0)
//                     {
//                         if(nCountEachTime==EAGAIN)
//                             break;
//                         else if(nCountEachTime==EINTR)
//                             continue;
//                         else
//                         {
//                             perror("write error");
//                             std::cout<<strerror(errno)<<std::endl;
//                             break;
//                         }
//                     }
//                     nLeftCountToTransfer-=nCountEachTime;
//                     ptrToTransfer+=nCountEachTime;
//                }while(nLeftCountToTransfer>0)
//            }

        }


};

HeaderState httpData::parseHeaders( string& strRequest,std::map<string,string>& sMapHeaderInfo,int& hstate)
{
    string& str = strRequest;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    int& hState_=hstate;
    //hState_=H_START;

    for (; i < str.size() && notFinish; ++i)
    {
        switch(hState_)
        {
            case H_START:
            {
                if (str[i] == '\n' || str[i] == '\r')
                    break;
                hState_ = H_KEY;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case H_KEY:
            {
                if (str[i] == ':')
                {
                    key_end = i;
                    if (key_end - key_start <= 0)
                        return PARSE_HEADER_ERROR;
                    hState_ = H_COLON;
                }
                else if (str[i] == '\n' || str[i] == '\r')
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_COLON:
            {
                if (str[i] == ' ')
                {
                    hState_ = H_SPACES_AFTER_COLON;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_SPACES_AFTER_COLON:
            {
                hState_ = H_VALUE;
                value_start = i;
                break;
            }
            case H_VALUE:
            {
                if (str[i] == '\r')
                {
                    hState_ = H_CR;
                    value_end = i;
                    if (value_end - value_start <= 0)
                        return PARSE_HEADER_ERROR;
                }
                else if (i - value_start > 255)
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_CR:
            {
                if (str[i] == '\n')
                {
                    hState_ = H_LF;
                    string key(str.begin() + key_start, str.begin() + key_end);
                    string value(str.begin() + value_start, str.begin() + value_end);
                    sMapHeaderInfo[key] = value;
                    now_read_line_begin = i;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_LF:
            {
                if (str[i] == '\r')
                {
                    hState_ = H_END_CR;
                }
                else
                {
                    key_start = i;
                    hState_ = H_KEY;
                }
                break;
            }
            case H_END_CR:
            {
                if (str[i] == '\n')
                {
                    hState_ = H_END_LF;
                }
                else
                    return PARSE_HEADER_ERROR;
                break;
            }
            case H_END_LF:
            {
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    if (hState_ == H_END_LF)
    {
        str = str.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}

void httpData::obtainListenSocket(){
    if(auto temp=m_ptrServer.lock())
        m_listenSocket=temp->getListenSocket();
}