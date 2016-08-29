//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_SOCKET_H
#define CBUNGEE_SOCKET_H

#include "../protocoll/Buffers/DataBuffer.h"
#include <stdio.h>
#include <signal.h>

class Socket {
public:
    Socket(int fd){
        this->fd = fd;
    }

        ~Socket();

    int writeBytes(char* buffer,int length);
    int writeBuffer(DataBuffer& buffer);
    int readBytes(char* buffer,int length);
    void closeSocket();
    DataBuffer* readBuffer(int length);
private:
    int fd;
};


#endif //CBUNGEE_SOCKET_H
