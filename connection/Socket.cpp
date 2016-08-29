//
// Created by wolverindev on 23.08.16.
//

#include "Socket.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

DataBuffer* Socket::readBuffer(int length) {
    char* bbuffer = (char*) malloc(length);
    readBytes(bbuffer,length);
    return new DataBuffer(bbuffer,length,false);
}

int Socket::readBytes(char *buffer, int length) {
    signal(SIGPIPE, SIG_IGN);
    //cout << "Read bytes: " << length << " from " << fd << " to buffer " << (void*) buffer << endl;
    return read(this->fd,buffer,length);
}

int Socket::writeBuffer(DataBuffer &buffer) {
    return writeBytes((char*) buffer.getBuffer(),buffer.getBufferLength());
}

int Socket::writeBytes(char *buffer, int length) {
    signal(SIGPIPE, SIG_IGN);
    //cout << "Write: " << (int) buffer[0] << " Length: " << length << endl;
    return write(this->fd,buffer,length);
}

void Socket::closeSocket() {
    close(this->fd);
}

Socket::~Socket() {
    closeSocket();
}