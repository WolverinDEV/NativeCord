//
// Created by wolverindev on 23.08.16.
//

#include "StreamedDataBuffer.h"

char StreamedDataBuffer::read() {
    char buffer;
    int readedBytes = socket->readBytes(&buffer,1);
    if(readedBytes == -1)
        throw new StreamClosedException();
    if(readedBytes == 0)
        throw  new OEFException();
    return buffer;
}

void StreamedDataBuffer::read(const char *buffer, int length) {
    int readedBytes = socket->readBytes((char*) buffer,length);
    if(readedBytes == -1)
        throw new StreamClosedException();
    if(readedBytes == 0)
        throw  new OEFException();
}

void StreamedDataBuffer::write(char byte) {
    switch (socket->writeBytes(&byte,1)){
        case -1:
            throw new StreamClosedException();
    }
}

void StreamedDataBuffer::write(const char *buffer, int length) {
    socket->writeBytes((char*) buffer,length);
}

int StreamedDataBuffer::readableBytes() {
    return 1;
}

int StreamedDataBuffer::writeableBytes() {
    return -1;
}