//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_SOCKET_H
#define CBUNGEE_SOCKET_H

#include "../protocoll/DataBuffer.h"
#include <stdio.h>
#include <signal.h>
#include <poll.h>

class Socket {
    public:
        Socket(int fd) : fd(fd) {
            connected = true;
            this->pollFd.events = POLL_IN;
            if(fd > 0)
                this->pollFd.fd = fd;
        }

        Socket(string host, int port) : host(host), port(port), fd(-1){
        }


        ~Socket();

        int writeBytes(char *buffer, int length);

        int writeBuffer(DataBuffer &buffer);

        int readBytes(char *buffer, int length, int timeout = 5000);

        virtual void closeSocket();

        virtual void connect();

        DataBuffer *readBuffer(int length);

        bool isConnected(){
            return connected && fd > 0;
        }

        bool isConnecting(){
            return false;
        }

        bool hasData(int timeout = 5000);

        int getFd() const;

    protected:
        struct pollfd pollFd;
        int32_t fd = -1;
    private:
        string host;
        int port = 0;
        bool connected = false;
};


#endif //CBUNGEE_SOCKET_H
