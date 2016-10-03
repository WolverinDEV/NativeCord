//
// Created by wolverindev on 10.09.16.
//

#ifndef NATIVECORD_GAMEPROFILE_H
#define NATIVECORD_GAMEPROFILE_H

#include "../json/json.hpp"
#include "../utils/UUIDUtils.h"
#include "../utils/StringUtil.h"
#include <string>
#include <uuid/uuid.h>

using json = nlohmann::json;
using namespace std;
class GameProfile {
    public:
        GameProfile(string& name,uuid_t& uuid){
            handle["name"] = name;
            handle["id"] = StringUtils::replaceAll(UUIDUtils::uuidToString(uuid),"-",""); //no braces
        }
        GameProfile(json json){
            this->handle = json;
        }

        string getName(){
            return handle["name"];
        }
        uuid_t& getUUID(){
            cout << "Id: " << handle["id"] << endl;
            return UUIDUtils::getUUIDFromString(handle["id"], false);
        }

        string getUUIDRaw(){
            return UUIDUtils::getUUIDString36(handle["id"]); //TODO more performance friendly
        }

        bool hasTextures(){
            return handle.count("properties") != 0;
        }

        json& getProperties(){
            return handle["properties"];
        }
    private:
        //{"id":"57091d6f839f48b7a4b14474222d4ad1","name":"<name>","properties":[{"name":"textures","value":"XY","signature":"XY"}]}
        json handle;
};

#endif //NATIVECORD_GAMEPROFILE_H
