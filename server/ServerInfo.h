//
// Created by wolverindev on 29.08.16.
//

#ifndef CBUNGEE_SERVERINFO_H
#define CBUNGEE_SERVERINFO_H

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include <stdlib.h>
#include "../config/Configuration.h"
#include "../connection/Socket.h"
#include "../utils/SocketUtil.h"

using namespace std;
class ServerInfo {
    public:
        static vector<ServerInfo*> servers;
        static void loadServers(){
            for (YAML::const_iterator it = Configuration::instance->config["servers"].begin(); it != Configuration::instance->config["servers"].end(); ++it) {
                if (it->first.as<string>().compare("disable_direct") == 0)
                    continue;
                servers.push_back(new ServerInfo(it->first.as<string>()));
            }
        }

        static ServerInfo* getServerInfo(string name){
            for(vector<ServerInfo*>::iterator it = servers.begin(); it != servers.end(); it++){
                if(strcmp((*it)->getName().c_str(),name.c_str()) == 0)
                    return *it;
            }
            return NULL;
        }

        static ServerInfo* getServerInfo(string host,int port){
            for(vector<ServerInfo*>::iterator it = servers.begin(); it != servers.end(); it++){
                if(strcmp((*it)->getHost().c_str(),host.c_str()) == 0 && port == (*it)->port)
                    return *it;
            }
            return NULL;
        }

        static ServerInfo* addServerInfo(string name, string host,int port, bool visible){
            ServerInfo* out = getServerInfo(name);
            if(out == NULL){
                servers.push_back(out = new ServerInfo(name,host.c_str(),port,visible));
            }
            return out;
        }

        static ServerInfo* createTempServerInfo(string host,int port){
            return new ServerInfo(host.append(to_string(port)) ,host,port,false);
        }

        static vector<ServerInfo*> buildDefaultServerQueue(){
            YAML::Node default_server_node = Configuration::instance->config["join"]["default_server"];
            vector<ServerInfo*> out;
            for (YAML::const_iterator it = default_server_node.begin(); it != default_server_node.end(); ++it) {
                out.push_back(getServerInfo(it->as<string>()));
            }
            return out;
        }

        static void reset(){
            for(vector<ServerInfo*>::iterator it = servers.begin();it != servers.end();it++)
                delete (ServerInfo*) (*it);
            servers.clear();
        }

        ServerInfo(string name){
            this->name = name;
            this->host = Configuration::instance->config["servers"][name]["host"].as<string>();
            this->port = Configuration::instance->config["servers"][name]["port"].as<int>();
            this->visible = Configuration::instance->config["servers"][name]["visible"].as<bool>();
        }

        ServerInfo(const string &name, const string &host, int port, bool visible) : name(name), host(host), port(port), visible(visible) {}

        const string &getName() const {
            return name;
        }

        const string &getHost() const {
            return host;
        }

        int getPort() const {
            return port;
        }

        bool isVisible() const {
            return visible;
        }

        /**
         * @return See SocketUtil::createTCPSocket
         */
        Socket* createSocket(){
            return SocketUtil::createTCPSocket(host.c_str(), port);
        }

    private:
        string name;
        string host;
        int port;
        bool visible;
};

#endif //CBUNGEE_SERVERINFO_H
