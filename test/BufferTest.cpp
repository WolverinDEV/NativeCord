/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BufferTest.cpp
 * Author: wolverindev
 *
 * Created on August 12, 2016, 12:07 AM
 */

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <execinfo.h>
#include <signal.h>
#include "../protocoll/Buffers/DataBuffer.h"
#include "../connection/Socket.h"
#include "../connection/PlayerConnection.h"
#include "../protocoll/packet/ClientPacketHandler.h"
#include "../chat/ChatColor.h"
#include "../chat/ChatMessage.h"
#include "../protocoll/packet/PacketHandler.h"
#include "../json/json.hpp"

using namespace std;

void reverse(char* arr, int count);
int readInt(int sockfd);

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void error(const char* message){
    cerr << message << endl;
    exit(-1);
}

void clientConnect(){
    try{
        /*
        int sockfd, n;
        struct sockaddr_in serv_addr;
        struct hostent *server;

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("ERROR opening socket");
            return;
        }
        server = gethostbyname("localhost");
        if (server == NULL) {
            perror("ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(25565);
        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            perror("ERROR connecting");
            exit(0);
        }
        */


        int sockfd;
        socklen_t clilen;
        struct sockaddr_in serv_addr, cli_addr;
        int n;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
            error("ERROR opening socket");
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(25566);
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0)
            error("ERROR on binding");
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        while (1){
            int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");

            Socket *connection = new Socket(newsockfd);
            PlayerConnection *playerConnection = new PlayerConnection(connection);
            ClientPacketHandler* handler = new ClientPacketHandler(playerConnection);
            handler->startReader();
            pthread_join((handler->getThreadHandle()),NULL);
        }
        //cout << "Packet length: " << playerConnection->getStream()->readVarInt() << endl;
        //cout << "Packet is: " <<  playerConnection->getStream()->readVarInt() << endl;
        //cout << playerConnection->getStream()->readString() << endl;
        /*
        int length = readInt(sockfd);
        cout << "Length: " << length << endl;
        char* buffer = new char[length];
        memset(buffer, 0, length);
        read(sockfd, buffer, length);
        DataBuffer* dbuffer = new DataBuffer(buffer, length);
        try {
            cout << "Int size: " << sizeof (long long) << endl;
            cout << "Double 1: " << (int) dbuffer->read() << endl;
            cout << "Double 2: " << (int) dbuffer->read() << endl;
            cout << "Double 3: " << (int) dbuffer->readVarInt() << endl;
            cout << "Double 4: " << dbuffer->readString() << endl;
            char* uuidBuffer = new char[32];
            uuid_unparse(*(dbuffer->readUUID()), uuidBuffer);
            cout << "Double 5: " << uuidBuffer << endl;

            dbuffer->clear();

            dbuffer->writeInt(11111);
            dbuffer->writeInt(22222);
            dbuffer->writeString(string("Hello back :D :X"));
        } catch (const char* message) {
            cout << "Exception message: " << message << endl;
        } catch (Exception* ex) {
            cout << "Exception: " << ex->what() << endl;
            exit(0);
        }
        cout << "Buffer writer index: " << dbuffer->getBufferLength() << "/" << dbuffer->getWriterindex() << endl;
        write(sockfd, dbuffer->getBuffer(), dbuffer->getBufferLength());
         */
    }catch (Exception* e){
        cout << "Exception message: " << e->what() << endl;
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    try {
        signal(SIGSEGV, handler);
        pthread_t threadHandle;
        pthread_create(&threadHandle,NULL,(void* (*)(void*)) &clientConnect,NULL);
        pthread_join(threadHandle,NULL);
    }catch(Exception* ex){
        cout << "Exception: " << ex->what() << endl;
    }
    return 0;
}

int readInt(int sockfd) {
    char* buffer = new char[4];
    memset(buffer, 0, 4);
    read(sockfd, buffer, 4);
    DataBuffer dbuffer(buffer, 4);
    return dbuffer.readInt();
}

void reverse(char* arr, int count) {
    char temp;
    for (int i = 0; i < count / 2; ++i) {
        temp = arr[i];
        arr[i] = arr[count - i - 1];
        arr[count - i - 1] = temp;
    }
}