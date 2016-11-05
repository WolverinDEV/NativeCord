//
// Created by wolverindev on 23.08.16.
//
#include "../../../include/protocoll/StreamedDataBuffer.h"

char StreamedDataBuffer::read() {
    char buffer;
    read(&buffer,1);
    return buffer;
}

int StreamedDataBuffer::read(const char *buffer, int length)/* throw(Exception) */{
    if(socket == nullptr || !socket->isConnected())
        throw StreamClosedException();
    if(length <= 0){
        logError("Try to read with length 0!");
        return 0;
    }
    int readedBytes = -2;
    if(encodeCipper != nullptr){
        char* bbuffer = (char*) malloc(length);
        readedBytes = socket->readBytes((char*) bbuffer,length);
        encodeCipper->cipher(bbuffer,length,(char*) buffer,false);
        free(bbuffer);
    }
    else readedBytes = socket->readBytes((char*) buffer,length);
    if(readedBytes != length)
        throw Exception("Didnt readed all bytes: "+to_string(readedBytes)+" of "+to_string(length));
    if(readedBytes == -1)
        throw StreamClosedException();
    if(readedBytes == 0)
        throw OEFException();
    return readedBytes;
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