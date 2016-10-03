//
// Created by wolverindev on 23.08.16.
//

#include "../../../include/protocoll/StreamedDataBuffer.h"

char StreamedDataBuffer::read() {
    char buffer;
    read(&buffer,1);
    return buffer;
}

void StreamedDataBuffer::read(const char *buffer, int length) {
    int readedBytes = -2;
    if(encodeCipper != NULL){
        char* bbuffer = (char*) malloc(length);
        readedBytes = socket->readBytes((char*) bbuffer,length);
        encodeCipper->cipher(bbuffer,length,(char*) buffer,false);
        free(bbuffer);
    }
    else readedBytes = socket->readBytes((char*) buffer,length);
    if(readedBytes == -1)
        throw new StreamClosedException();
    if(readedBytes == 0)
        throw  new OEFException();
}

void StreamedDataBuffer::write(char byte) {
    write(&byte,1);
}

void StreamedDataBuffer::write(const char *buffer, int length) {
    int state = 0;
    if(decodeCipper != NULL){
        char* bbuffer = decodeCipper->cipher((char *) buffer,length, false);
        state = socket->writeBytes((char*) bbuffer,length);
        delete bbuffer;
    } else state = socket->writeBytes((char*) buffer,length);
    switch (state){
        case -1:
            throw new StreamClosedException();
    }
}

int StreamedDataBuffer::readableBytes() {
    return 1;
}

int StreamedDataBuffer::writeableBytes() {
    return -1;
}