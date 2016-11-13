//
// Created by wolverindev on 28.08.16.
//

#include <cstdlib>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <CXXTerminal/Terminal.h>
#include "cpr/cpr.h"
#include "include/utils/SocketUtil.h"
#include "include/config/Configuration.h"
#include "include/connection/PlayerConnection.h"
#include "include/plugin/PluginManager.h"
#include "include/plugin/java/JavaPluginManagerImpl.h"
#include "include/NativeCord.h"

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

pthread_t NativeCord::clientAcceptThread;

void NativeCord::exitNativeCoord(){
    vector<PlayerConnection*> connections (PlayerConnection::activeConnections);

    ChatMessage disconnectMessage ("Nativecord is shutting down.");
    for(vector<PlayerConnection*>::iterator it = connections.begin(); it != connections.end(); it++){
        it.operator*()->disconnect(&disconnectMessage, false);
    }

    PlayerConnection::connections.clear();
    PlayerConnection::activeConnections.clear();
    pthread_cancel(NativeCord::clientAcceptThread);
    pthread_join(NativeCord::clientAcceptThread, NULL);
    ServerInfo::reset();

    debugMessage("Buffers: " + to_string(DataBuffer::creations));
    debugMessage("Chat instances: " + to_string(ChatMessage::count));

    JavaPluginManagerImpl::instance->disable(); // Close the process
}


int preloadV(){
    Terminal::setup();
    return 0;
}
int preload = preloadV();


int main(int argc, char** argv) {
    logMessage("Hello world");

    JavaPluginManagerImpl* manager = new JavaPluginManagerImpl();
    if(!manager->enable()){
        logError("Cant enable plugin manager!");
        return 0;
    }

    //TODO load plugin right
    vector<Plugin*> plugins (manager->getLoadedPlugins());
    for(vector<Plugin*>::iterator it = plugins.begin(); it != plugins.end(); it++) {
        Plugin* plugin = *it;
        stringstream ss;
        ss << "Enabling plugin " << plugin->getName() << " (Version: " << plugin->getVersion() << ") Authors: ";
        vector<string> v (plugin->getAuthors());
        if(!v.empty()){
            bool first = true;
            for(vector<string>::iterator ait = v.begin(); ait != v.end(); ait++){
                if(!first)
                    ss << ", ";
                ss  << *ait;
                first = false;
            }
        }else
            ss << "unknown";
        logMessage(ss.str());
        JavaPluginManagerImpl::instance->enablePlugin(*it);
    }
    //manager->disable();
    //delete manager;

    try {
        string filename = string("config.yml");
        Configuration::instance = new Configuration(filename);
        Configuration::instance->loadConfig();
        cout << "Loading configuration" << endl;
        if(!Configuration::instance->isValid()){
            logFatal("Configuration not valid!");
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
        pthread_create(&(NativeCord::clientAcceptThread),NULL,(void* (*)(void*)) &clientConnect,NULL);
        //pthread_join(clientAcceptThread,NULL);

        while (1){
            string command = Terminal::getInstance()->readLine(ANSI_GRAY ANSI_BOLD"> "ANSI_PURPLE);
            logMessage("§aHaving command "+command);

            if(command.compare("end") == 0){
                NativeCord::exitNativeCoord();
                return 0;
            }
        }
    }catch(Exception* ex){
        logFatal("Having error on main thread: "+string(ex->what()));
        logMessage("Exiting");
        return 1;
    }
    return 0;
}