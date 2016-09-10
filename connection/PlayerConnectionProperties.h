//
// Created by wolverindev on 03.09.16.
//

#ifndef NATIVECORD_PLAYERCONNECTIONPROPERTIES_H
#define NATIVECORD_PLAYERCONNECTIONPROPERTIES_H

#include "../json/json.hpp"
using json = nlohmann::json;
class PlayerProperties {
    public:
        json gameprofile;
};

#endif //NATIVECORD_PLAYERCONNECTIONPROPERTIES_H
