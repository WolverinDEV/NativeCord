/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DataBuffer.cpp
 * Author: wolverindev
 * 
 * Created on August 11, 2016, 7:47 PM
 */

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "../../../include/protocoll/DataBuffer.h"

using namespace std;

int DataBuffer::creations = 0;

DataBuffer::DataBuffer() {
    creations++;
}

DataBuffer::DataBuffer(const char *buffer, int bufferLength, bool copy) {
    if(copy){
        this->buffer = (const char *) malloc(bufferLength);
        memcpy((void *) this->buffer, (const void *) buffer, bufferLength);
    }
    else
        this->buffer = buffer;
    this->writerindex = bufferLength;
    this->bufferLength = bufferLength;
    creations++;
}

DataBuffer::DataBuffer(int bufferLength) {
    this->buffer = (const char *) malloc(bufferLength);
    this->bufferLength = bufferLength;
    creations++;
}


DataBuffer::~DataBuffer() {
    if(buffer != NULL) {
        free((void*) buffer);
    }
    creations--;
}

int DataBuffer::readableBytes() {
    return bufferLength - readerindex;
}

int DataBuffer::read(const char *buffer, int length) {
    if(this->buffer == NULL){
        throw Exception("Buffer is null");
    }
    checkReadableBytes(length);
    memcpy((void *) buffer, (const void *) ((long int) (this->buffer) + readerindex), length);
    readerindex += length;
    return length;
}

char DataBuffer::read() {
    if(this->buffer == NULL){
        cout << "Null buffer" << endl;
        return 0;
    }
    checkReadableBytes(1);
    readerindex++;
    return this->buffer[readerindex - 1];
}

DataBuffer* DataBuffer::readBuffer(int length) {
    char* bbuffer = (char*) malloc(length);
    read(bbuffer,length);
    return new DataBuffer(bbuffer,length,false);
}

int DataBuffer::writeableBytes() {
    return fixedLength ? bufferLength - writerindex : -1;
}

void DataBuffer::write(const char *buffer, int length) {
    checkWriteableBytes(length);
    allocateBufferSize(length);
    void *vbuffer = (void *) ((long int) (this->buffer) + writerindex);
    memcpy((void *) vbuffer, buffer, length);
    writerindex = writerindex + length;
}

void DataBuffer::write(char byte) {
    write(&byte, 1);
}