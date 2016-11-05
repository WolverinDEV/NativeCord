//
// Created by wolverindev on 23.08.16.
//

#include <poll.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../../include/connection/Socket.h"
#include "../../include/log/LogUtils.h"
#include "../../include/utils/TimeUtils.h"
#include "../../include/utils/SocketUtil.h"

DataBuffer* Socket::readBuffer(int length) {
    char* bbuffer = (char*) malloc(length);
    debugMessage("Readed "+to_string(readBytes(bbuffer,length))+" of "+to_string(length));
    return new DataBuffer(bbuffer,length,false);
}

int Socket::readBytes(char *buffer, int length, int timeout = 5000) {
    if(length == 0)
        return 0;
    if(this->connected == 0){
        debugMessage("Try to read a message[length: "+to_string(length)+"] on a closed socket[fd: "+to_string(fd)+"]");
        return 0;
    }
    signal(SIGPIPE,  SIG_IGN);
    uint64_t start = TimeUtils::getCurrentTimeMillis();
    int readed = 0;
    while (readed != length){
        if(!connected)
            return -1;
        if(!hasData(timeout-(TimeUtils::getCurrentTimeMillis()-start)))
            return 0;
        int r = read(this->fd,buffer+readed,length-readed);
        if(r > 0) {
            readed += r;
            continue;
        }
#ifdef SOCK_READED_ZERO_TIMEOUT
        if(r == 0){
            if(TimeUtils::getCurrentTimeMillis()-start > timeout)
                return 0;
            continue;
        }
#else
        if(r == 0)
            return r;
#endif
        if(r <= 0){
            return r;
        }
        if(TimeUtils::getCurrentTimeMillis()-start > timeout)
            return 0;
        usleep(1000);
        continue;
    }
    return readed;
}

bool Socket::hasData(int timeout = 5000) {
    switch (poll(&pollFd, 1, timeout)){
        case 0:
        case -1:
            return false;
        default:
            return true;
    }
}

int Socket::writeBuffer(DataBuffer &buffer) {
    return writeBytes((char*) buffer.getBuffer(),buffer.getBufferLength());
}

int Socket::writeBytes(char *buffer, int length) {
    if(length == 0)
        return 0;
    if(connected == 0) {
        debugMessage("Try to send a message[length: "+to_string(length)+"] on a closed socket[fd: "+to_string(fd)+"]");
        return -1;
    }
    signal(SIGPIPE, SIG_IGN);
    int writeState = write(this->fd,buffer,length);
    if(writeState <= 0)
        connected = false;
    return writeState;
}

void Socket::closeSocket() {
    if(!connected)
        return;
    this->connected = false;
    close(this->fd);
}

Socket::~Socket() {
    closeSocket();
}

int Socket::getFd() const {
    return fd;
}

void Socket::connect() {
    fd = SocketUtil::createTCPFD(this->host.c_str(), this->port);
    if(fd > 0){
        this->pollFd.fd = fd;
        connected = true;
    }
}