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


        int sockfd = SocketUtil::createTCPServerSocket(25566);
        if(sockfd < 0){
            sockfd = SocketUtil::createTCPServerSocket(25565);
            if(sockfd < 0) {
                error("Cant create socket.");
            }
        }
        while (1){
            struct sockaddr_in* cli_addr = new sockaddr_in();
            socklen_t clilen = sizeof(*cli_addr);
            int newsockfd = accept(sockfd, (struct sockaddr *) cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");

            Socket *connection = new Socket(newsockfd);
            PlayerConnection *playerConnection = new PlayerConnection(connection);
            ClientPacketHandler* handler = new ClientPacketHandler(playerConnection);
            handler->startReader();
            //pthread_join((handler->getThreadHandle()),NULL);
        }
    }catch (Exception* e){
        cout << "Exception message: " << e->what() << endl;
    }
}


/*
 * 
 */
int main(int argc, char** argv) {
    try {
        pthread_t threadHandle;
        pthread_create(&threadHandle,NULL,(void* (*)(void*)) &clientConnect,NULL);
        pthread_join(threadHandle,NULL);
    }catch(Exception* ex){
        cout << "Exception: " << ex->what() << endl;
    }
    return 0;
}