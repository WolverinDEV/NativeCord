//
// Created by wolverindev on 10.10.16.
//

#include "../../../include/plugin/java/JavaPluginManagerImpl.h"
#include "../../../include/plugin/event/EventHelper.h"
#include "../../../include/connection/PlayerConnection.h"

#define EVENT_PACKAGE "dev/wolveringer/nativecord/api/event/"

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
        EVENT_PACKAGE"PlayerHandschakeEvent",
        "",
        "",
        "",
        EVENT_PACKAGE"PlayerServerConnectEvent",
        EVENT_PACKAGE"PlayerServerConnectedEvent",
        EVENT_PACKAGE"PlayerChatEvent"
};

jobject EventHelper::createJavaInstance(JavaPluginManagerImpl *impl, EventType type, DataStorage *buffer) {
    string name = EventHelper::JavaClassMapping[type];
    if (name.empty())
        return nullptr;
    jclass cls;

    jobject event = impl->getEnv()->AllocObject(impl->getEnv()->FindClass(name.c_str()));
    jobject obj = impl->getStorageImpl()->toJavaObject(*buffer);
    impl->getEnv()->SetObjectField(event, impl->getRefelectManager()->f_event_storage, obj);
    return event;
}

DataStorage& EventHelper::callEvent(EventType t, DataStorage &s) {
    //TODO cxx event
    DataStorage* ptr = &s;
    JavaPluginManagerImpl::instance->callEvent(t, ptr);
    return *ptr;
}

DataStorage EventHelper::createBaseEvent(){
    DataStorage storage = DataStorage();
    storage.bytes.push_back(0); //Canceled = false
    return storage;
}

DataStorage EventHelper::createPlayerEvent(PlayerConnection* conn){
    DataStorage storage = createBaseEvent();
    storage.longs.push_back((uint64_t)conn->getJavaNativeAddress());
    return storage;
}

void EventHelper::handleHandshake(PlayerConnection* conn, PacketHandshake * packet) {
    DataStorage storage = EventHelper::createPlayerEvent(conn);
    storage.strings.push_back(packet->getHost());
    storage.ints.push_back(packet->getPort());
    storage.ints.push_back(packet->getClientVersion());
    storage.ints.push_back(packet->getState());
    storage = callEvent(EventType::PLAYER_HANDSCHAKE_EVENT, storage);
    packet->setClientVersion(storage.ints[1]);
    packet->setPort(storage.ints[0]);
    packet->setHost(storage.strings[0]);
    packet->setState(storage.ints[2]);
}

ServerInfo* EventHelper::callServerConnectEvent(PlayerConnection *con, ServerInfo *ser) {
    DataStorage storage = EventHelper::createPlayerEvent(con);
    storage.strings.push_back(ser->getName());
    storage = callEvent(EventType::PLAYER_SERVER_CONNECT_EVENT, storage);
    ServerInfo* out = ser;
    if(ser->getName().compare(storage.strings[0]) > 0){
        debugMessage("Changing server con name: "+ser->getName()+" to "+storage.strings[0]);
        out = ServerInfo::getServerInfo(storage.strings[0]);
    }
    return out;
}

void EventHelper::callServerConnectedEvent(PlayerConnection *con) {
    debugMessage("Call server connected event");
    DataStorage storage = EventHelper::createPlayerEvent(con);
    storage.strings.push_back(con->getCurrentTargetConnection()->getServerInfo()->getName());
    storage = callEvent(EventType::PLAYER_SERVER_CONNECTED_EVENT, storage);
}


string EventHelper::callChatEvent(PlayerConnection *conn, std::string &message) {
    DataStorage storage = EventHelper::createPlayerEvent(conn);
    storage.strings.push_back(message);
    storage = callEvent(EventType::PLAYER_CHAT_EVENT, storage);
    debugMessage("After chat call event: "+storage._toString());
    if (storage.bytes[0] != 0)
        return "";
    return storage.strings[0];
}