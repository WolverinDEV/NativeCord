//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SOCKETUTIL_H
#define CBUNGEE_SOCKETUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../connection/Socket.h"
#include "../log/LogUtils.h"

using namespace std;

class SocketUtil {
public:
        static Socket* createTCPSocket(const char *host, uint16_t port) {
            int socket = createTCPFD(host,port);
            debugMessage("Created socket fd "+to_string(socket));
            if(socket > 0)
                return new Socket(socket);
            else
                return (Socket*) new int(socket);
        }

        static in_addr* getIPV4Bytes(const char *host){
            struct hostent *addr = gethostbyname(host);
            for(int i = 0; ; ++i)
            {
                char *temp = addr->h_addr_list[i];
                if(temp == NULL) //we reached the end of the list
                    break; //exit the loop.
                return reinterpret_cast<in_addr*>(temp);
            }
            return nullptr;
        }

        static int createTCPFD(const char *host, uint16_t port){
            int sockfd, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                debugMessage("Cant open socket: "+string(strerror(errno)));
                return -1;
            }
            server = gethostbyname(host);
            if (server == NULL) {
                debugMessage("Cant resolve host: "+string(strerror(errno)));
                return -2;
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
            serv_addr.sin_port = htons(port);
            if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                debugMessage("Error while connecting: "+string(strerror(errno)));
                close(sockfd);
                return -3;
            }
            return sockfd;
        }

        static int createTCPServerSocket(uint16_t port) {
            int sockfd;
            struct sockaddr_in serv_addr;
            int n;
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                perror("ERROR opening socket");
                return -1;
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(port);
            if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("ERROR on binding");
                return -2;
            }
            listen(sockfd, 5);
            return sockfd;
        }
};

#endif //CBUNGEE_SOCKETUTIL_H
