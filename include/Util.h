//
// Created by anson on 19-4-13.
//

#ifndef THREAD_POOL_UTIL_H
#define THREAD_POOL_UTIL_H



#pragma once
#include <cstdlib>
#include <string>


int setSocketNonBlocking(int fd);
void setSocketNodelay(int fd);
void setSocketNoLinger(int fd);
void shutDownWR(int fd);
int socket_bind_listen(int port);

#endif //THREAD_POOL_UTIL_H