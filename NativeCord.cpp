//
// Created by wolverindev on 28.08.16.
//

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "cpr/cpr.h"
#include "include/utils/SocketUtil.h"
#include "include/config/Configuration.h"
#include "include/connection/PlayerConnection.h"
#include "include/plugin/PluginManager.h"
#include "include/log/LogUtils.h"
#include "include/plugin/java/JavaPluginManagerImpl.h"
#include <vector>
using namespace std;

void error(const char* message){
    cerr << message << endl;
    exit(-1);
}

int ssockfd = 0;
sockaddr_in* cli_addr = nullptr;

void shutdownHook(void){
    cout << "Closing socket" << endl;
    close(ssockfd);
    if(cli_addr != nullptr)
        delete cli_addr;
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


        ssockfd = SocketUtil::createTCPServerSocket(Configuration::instance->config["network"]["port"].as<int>());
        if(ssockfd < 0){
            ssockfd = SocketUtil::createTCPServerSocket(Configuration::instance->config["network"]["port"].as<int>()+1); //TEST MODE!
            if(ssockfd < 0){
                error("Cant create socket.");
            }
        }
        while (1) {
            cli_addr = new sockaddr_in();
            socklen_t clilen = sizeof(*cli_addr);
            int newsockfd = accept(ssockfd, (struct sockaddr *) cli_addr, &clilen);
            if (newsockfd < 0)
                error("ERROR on accept");

            Socket *connection = new Socket(newsockfd);
            PlayerConnection *playerConnection = new PlayerConnection(cli_addr ,connection);
            playerConnection->start();
            //pthread_join((handler->getThreadHandle()),NULL);
        }
    }catch (Exception* e){
        cout << "Exception message: " << e->what() << endl;
    }
}

typedef std::function<int(void)> TestFunc;

pthread_t threadHandle;
void exitNativeCoord(){
    JavaPluginManagerImpl::instance->disable();
    PlayerConnection::connections.clear();
    PlayerConnection::activeConnections.clear();
    pthread_cancel(threadHandle);
    //pthread_join(threadHandle, NULL);
    ServerInfo::reset();

    cout << "Buffers: " << DataBuffer::creations << endl;
    cout << "Chat instances: " << ChatMessage::count << endl;
}

int main(int argc, char** argv) {
    JavaPluginManagerImpl* manager = new JavaPluginManagerImpl();
    if(!manager->enable()){
        logError("Cant enable plugin manager!");
        return 0;
    }

    /*
    DataStorage* handle = new DataStorage;
    handle->longs.push_back(2222);
    handle->ints.push_back(123);
    handle->bytes.push_back(125);
    handle->floats.push_back(1.23);
    handle->doubles.push_back(3.21);
    handle->strings.push_back("Hello world");
    jobject  jobj = manager->storageImpl->toJavaObject(*handle);

    DataStorage* out = manager->storageImpl->fromJavaObject(jobj);
    cout << out->_toString() << endl;
    */
    if(manager->getLoadedPlugins().size() > 0) {
        manager->enablePlugin(manager->getLoadedPlugins()[0]);
        DataStorage storage;
        manager->callEvent(EventType::PLAYER_HANDSCHAKE_EVENT, &storage);
    }
    //manager->disable();
    //delete manager;

    try {
        string filename = string("config.yml");
        Configuration::instance = new Configuration(filename);
        Configuration::instance->loadConfig();
        cout << "Loading configuration" << endl;
        if(!Configuration::instance->isValid()){
            logError("Configuration not valid!");
            vector<string> errors = Configuration::instance->getErrors();
            if(errors.size() == 1)
                logError("Error:");
            else
                logError("Errors (" + to_string(errors.size()) + "):");
            for(vector<string>::iterator it = errors.begin();it!=errors.end();it++){
                logError((errors.size() != 1 ? " - " : "") + *it);
            }
            return 0;
        }
        ServerInfo::loadServers();
        logMessage("Configuration successfull loaded.");
        pthread_create(&threadHandle,NULL,(void* (*)(void*)) &clientConnect,NULL);
        //pthread_join(threadHandle,NULL);


        string line;
        while (1){
            getline(cin, line);
            cout << "Having line: " << line << endl;
            if(strcmp(line.c_str(),"end") == 0){
                cout << "Stpping nativecord" << endl;
                vector<PlayerConnection*> ccopy(PlayerConnection::connections);
                for(vector<PlayerConnection*>::iterator it =ccopy.begin(); it != ccopy.end();it++)
                    (*it)->disconnect(new ChatMessage("§cNativecord is shuting down."));
                exitNativeCoord();
                return 0;
            }
        }
    }catch(Exception* ex){
        cout << "Exception: " << ex->what() << endl;
    }
    return 0;
}