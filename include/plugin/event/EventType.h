//
// Created by wolverindev on 09.10.16.
//

#ifndef NATIVECORD_EVENTTYPE_H
#define NATIVECORD_EVENTTYPE_H

#include "jni.h"
#include "../java/jni/DataStorage.h"
#include <functional>
#include <map>

enum EventType {
    PLAYER_HANDSCHAKE_EVENT,
    PLAYER_ENCRIPT_EVENT,
    PLAYER_CONNECTED_EVENT,
    PLAYER_DISCONNECT_EVENT,
    PLAYER_SERVER_CONNECT_EVENT,
    PLAYER_SERVER_CONNECTED_EVENT,
    PLAYER_CHAT_EVENT
};

#endif //NATIVECORD_EVENTTYPE_H
