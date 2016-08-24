//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_STREAMEDDATABUFFER_H
#define CBUNGEE_STREAMEDDATABUFFER_H


#include "DataBuffer.h"
#include "../../connection/Socket.h"

class StreamClosedException : public Exception {
public:
    StreamClosedException() : Exception("Stream closed"){

    }
};
class OEFException : public Exception {
public:
    OEFException() : Exception("Readed zero bytes (OEF exception)"){

    }
};

class StreamedDataBuffer : public DataBuffer{
public:
    StreamedDataBuffer(Socket* socket){
        this->socket = socket;
    }
    void read(const char *buffer, int length) override ;
    char read() override ;
    int readableBytes() override;
    int writeableBytes() override;
    void write(const char *buffer, int length) override;
    void write(char byte) override;
    void checkReadableBytes(int bytes) override {}
    void checkWriteableBytes(int bytes) override {}
private:
    Socket* socket;
};


#endif //CBUNGEE_STREAMEDDATABUFFER_H
