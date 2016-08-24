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

#include "DataBuffer.h"

using namespace std;

DataBuffer::DataBuffer() {
}

DataBuffer::DataBuffer(const char *buffer, int bufferLength, bool copy) {
    if(copy){
        this->buffer = (const char *) malloc(bufferLength);
        memcpy((void *) this->buffer, (const void *) buffer, bufferLength);
    }
    else
        this->buffer = buffer;
    this->bufferLength = bufferLength;
}

DataBuffer::DataBuffer(int bufferLength) {
    this->buffer = (const char *) malloc(bufferLength);
    this->bufferLength = bufferLength;
}


DataBuffer::~DataBuffer() {
    delete buffer;
}

int DataBuffer::readableBytes() {
    return bufferLength - readerindex;
}

void DataBuffer::read(const char *buffer, int length) {
    checkReadableBytes(length);
    memcpy((void *) buffer, (const void *) ((long int) (this->buffer) + readerindex), length);
    readerindex += length;
}

char DataBuffer::read() {
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
    char *buffer = new char[1];
    buffer[0] = byte;
    write(buffer, 1);
    delete buffer;
}