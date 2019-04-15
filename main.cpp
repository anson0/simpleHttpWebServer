//
// Created by reborn on 19-4-14.
//
#include "server.h"
int main(int argc,char* argv[]){
    int threadNum=4;
    int port=80;
    int opt;
    while(-1!=(opt=getopt(argc,argv,"t:p:"))){
        switch(opt){
            case 't':
                threadNum=atoi(optarg);
                break;
            case 'p':
                port=atoi(optarg);
                break;
            default:
                fprintf(stderr,"Usage:%s [-t numThreads] [-p port]",argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind > argc) {
        fprintf(stderr, "Expected argument after options\n");
        exit(EXIT_FAILURE);
    }
    //std::shared_ptr<Server> pServer(std::make_shared<Server>(Server(threadNum,port)));
    std::shared_ptr<Server> pServer(new Server(threadNum,port));//(std::make_shared<Server>(Server(threadNum,port)));
    pServer->init();
    int result=pServer->start();
    if(result<0)
    {
        fprintf(stderr, "Start failed\n");
        pServer->pool.closePool();
    }
    return 0;
}
