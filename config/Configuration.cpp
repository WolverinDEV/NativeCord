//
// Created by wolverindev on 28.08.16.
//

#include "Configuration.h"
#include <sys/stat.h>
#include <netdb.h>
#include <algorithm>
#include <unistd.h>

Configuration *Configuration::instance = NULL;

inline int existFile(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

bool checkNumberValid(YAML::Node number,int min,int max, bool minusOne = false){
    if(number.IsNull() || !number.IsDefined())
        return false;
    int num = number.as<int>();
    return num >= min && num <= max || num == -1 && minusOne;
}

void Configuration::loadConfig() {
    bool saveConfig = false;
    if (!existFile(fileName)) {
        config = YAML::Node();
    } else if(access(fileName.c_str(),R_OK) == 0){
        config = YAML::LoadFile(fileName.c_str());
    } else {
        errors.push_back(string("Cant read configuration. (Invalid permissions)"));
        return;
    }

    /**
     * Network
     */
    if (!config["network"]["ip_uuid_forward"].IsDefined()) {
        saveConfig = true;
        config["network"]["ip_uuid_forward"] = false;
    }
    if (!config["network"]["timeout"].IsDefined()) {
        saveConfig = true;
        config["network"]["timeout"] = 20000;
    }
    if (!config["network"]["player_limit"].IsDefined()) {
        saveConfig = true;
        config["network"]["player_limit"] = 350;
    }
    if (!config["network"]["connection_throttle"].IsDefined()) { //TODO
        saveConfig = true;
        config["network"]["connection_throttle"] = 2500;
    }
    if (!config["network"]["compression_threshold"].IsDefined()) {
        saveConfig = true;
        config["network"]["compression_threshold"] = 256;
    }
    if (!config["network"]["online_mode"].IsDefined()) {
        saveConfig = true;
        config["network"]["online_mode"] = true;
    }
    if (!config["network"]["host"].IsDefined()) { //TODO
        saveConfig = true;
        config["network"]["host"] = "localhost";
    }
    if (!config["network"]["port"].IsDefined()) {
        saveConfig = true;
        config["network"]["port"] = 25565;
    }

    /**
     * MOTD
     */
    if (!config["ping"]["visible_player_limit"].IsDefined()) {
        saveConfig = true;
        config["ping"]["visible_player_limit"] = 200;
    }
    if (!config["ping"]["motd"].IsDefined()) {
        saveConfig = true;
        config["ping"]["motd"] = string("&6It's not just another NativeCord! Its the NativeCord!\n&7Made by WolverinDEV and lots of coffee.");
    }
    //if (!config["ping"]["log"].IsDefined()) { //TODO log?
    //    saveConfig = true;
    //    config["ping"]["log"] = true;
    //}

    /**
     * Join
     */

    if (!config["join"]["default_server"].IsDefined()) {
        saveConfig = true;
        config["join"]["default_server"].push_back("lobby");
        config["join"]["default_server"].push_back("lobby2");
    }
    if (!config["join"]["forced_hosts"].IsDefined()) {
        saveConfig = true;
        config["join"]["forced_hosts"]["pvp.localhost"] = "pvp";
        config["join"]["forced_hosts"]["lobby.localhost"] = "lobby";
        config["join"]["forced_hosts"]["hub.localhost"] = "lobby";
    }
    if (!config["join"]["force_default_server"].IsDefined()) { //TODO save last locations
        saveConfig = true;
        config["join"]["force_default_server"] = true;
    }
    /**
     * Servers
     */
    if (!config["servers"].IsDefined()) {
        saveConfig = true;
        config["servers"]["lobby"]["host"] = "localhost";
        config["servers"]["lobby"]["port"] = 25570;
        config["servers"]["lobby"]["visible"] = true;

        config["servers"]["lobby2"]["host"] = "localhost";
        config["servers"]["lobby2"]["port"] = 25571;
        config["servers"]["lobby2"]["visible"] = true;

        config["servers"]["pvp"]["host"] = "localhost";
        config["servers"]["pvp"]["port"] = 25568;
        config["servers"]["pvp"]["visible"] = false;

        config["servers"]["sky"]["host"] = "localhost";
        config["servers"]["sky"]["port"] = 25569;
        config["servers"]["sky"]["visible"] = false;

        config["servers"]["disable_direct"] = true;
    }

    /**
     * Permissions
     */
    if (!config["permissions"]) {
        saveConfig = true;
        config["permissions"]["default"]["permissions"].push_back("nativecord.command.server.lobby");
        config["permissions"]["default"]["default"] = true;

        config["permissions"]["admin"]["permissions"].push_back("nativecord.command.server.connect_direct");
        config["permissions"]["admin"]["players"].push_back("WolverinDEV");
    }

    /**
     * Save config
     */

    if(saveConfig){
        ofstream confFile;
        confFile.open (fileName.c_str());
        if(confFile.bad()){
            errors.push_back(string("Cant open configuration file!"));
            return;
        }
        confFile << config;
        confFile.close();

        if(confFile.fail()){
            errors.push_back(string("Cant write to configuration file!"));
            return;
        }
    }

    /**
     * Checking for errors
     */

    vector<string> avariableServer;
    for (YAML::const_iterator it = config["servers"].begin(); it != config["servers"].end(); ++it) {
        if (it->first.as<string>().compare("disable_direct") == 0)
            continue;
        string name = it->first.as<string>();
        if (!config["servers"][name]["port"].IsDefined() || gethostbyname(config["servers"][name]["host"].as<string>().c_str()) == NULL) {
            errors.push_back(string("Invalid server configuration for server \"").append(name).append("\". (Invalid host)"));
            continue;
        }
        if (!config["servers"][name]["port"].IsDefined() || config["servers"][name]["port"].as<int>() > 65535 || config["servers"][name]["port"].as<int>() < 1) {
            errors.push_back(string("Invalid server configuration for server \"").append(name).append("\". (Invalid port)"));
            continue;
        }
        avariableServer.push_back(name);
    }


    if (!config["join"]["default_server"].IsSequence()) {
        errors.push_back("Invalid default server list!");
    } else {
        for (YAML::const_iterator it = config["join"]["default_server"].begin(); it != config["join"]["default_server"].end(); ++it) {
            if (std::find(avariableServer.begin(), avariableServer.end(), it->as<string>()) == avariableServer.end()) {
                errors.push_back(string("Missing server configuration for default server \"").append(it->as<string>()).append("\""));
            }
        }
    }

    for (YAML::const_iterator it = config["join"]["forced_hosts"].begin(); it != config["join"]["forced_hosts"].end(); ++it) {
        string name = it->first.as<string>();
        if (std::find(avariableServer.begin(), avariableServer.end(), config["join"]["forced_hosts"][name].as<string>()) == avariableServer.end()) {
            errors.push_back(string("Invalid forced host \"").append(name).append("\". Cant find target server \"").append(config["join"]["forced_hosts"][name].as<string>()).append("\""));
        }
    }

    if (gethostbyname(config["network"]["host"].as<string>().c_str()) == NULL)
        errors.push_back("Invalid network host");
    if(!checkNumberValid(config["network"]["port"],1,65535))
        errors.push_back(string("Invalid port. (Expression rule: 1 <= port(").append(config["network"]["port"].as<string>()).append(") <= 65535)"));
    if(!checkNumberValid(config["network"]["timeout"],50,30000))
        errors.push_back(string("Invalid timeout. (Expression rule: 50 <= timeout(").append(config["network"]["timeout"].as<string>()).append(") <= 30000)"));
    if(!checkNumberValid(config["network"]["player_limit"],1,10000, true))
        errors.push_back(string("Invalid network player limit. (Expression rule: 1 <= player_limit(").append(config["network"]["player_limit"].as<string>()).append(") <= 10000 OR timeout == -1)"));
    if(!checkNumberValid(config["network"]["connection_throttle"],50,60000, true))
        errors.push_back(string("Invalid connection throttle time. (Expression rule: 50 <= connection_throttle(").append(config["network"]["connection_throttle"].as<string>()).append(") <= 60000 OR connection_throttle == -1)"));
    if(!checkNumberValid(config["network"]["connection_throttle"],128,8192))
        errors.push_back(string("Invalid network compression threshold. (Expression rule: 128 <= compression_threshold(").append(config["network"]["compression_threshold"].as<string>()).append(") <= 8192)"));

    //TODO check permissions!
}