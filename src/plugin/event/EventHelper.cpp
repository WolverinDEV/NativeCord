//
// Created by wolverindev on 10.10.16.
//

#include "../../../include/plugin/event/EventHelper.h"
#include "../../../include/plugin/java/JavaPluginManagerImpl.h"

std::string EventHelper::EventTypeName[] = {
        "PLAYER_HANDSCHAKE_EVENT",
        "PLAYER_ENCRIPT_EVENT",
        "PLAYER_CONNECTED_EVENT",
        "PLAYER_DISCONNECT_EVENT",
        "PLAYER_SERVER_CONNECT_EVENT",
        "PLAYER_SERVER_CONNECTED_EVENT",
        "PLAYER_CHAT_EVENT"
};

std::map<string, jclass> EventHelper:: mapping;

std::string EventHelper::JavaClassMapping[] = {
        "dev/wolveringer/nativecord/api/event/PlayerHandschakeEvent",
        "",
        "",
        "",
        "",
        "",
        ""
};

jobject EventHelper::createJavaInstance(JavaPluginManagerImpl *impl, EventType type, DataStorage *buffer) {
    string name = EventHelper::JavaClassMapping[type];
    if (name.empty())
        return nullptr;
    jclass cls;
    if (mapping.count(name) < 1)
        mapping[name] = (cls = impl->getEnv()->FindClass((name).c_str()));
    else
        cls = mapping[name];
    return impl->getEnv()->AllocObject(cls);
}