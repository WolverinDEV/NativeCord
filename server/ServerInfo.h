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
#include "../config/Configuration.h"
#include "../connection/PlayerConnection.h"

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

    private:
        string name;
        string host;
        int port;
        bool visible;
};

#endif //CBUNGEE_SERVERINFO_H
