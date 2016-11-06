//
// Created by wolverindev on 09.10.16.
//

#ifndef NATIVECORD_JAVAREFLECTMANAGER_H
#define NATIVECORD_JAVAREFLECTMANAGER_H

#include "jni.h"
#include "../../connection/PlayerConnection.h"
#include <string>

class JavaPluginManagerImpl;
class JavaReflectManager {
    public:
        JavaReflectManager(JavaPluginManagerImpl& impl) : handle(impl){

        }

        bool loadFields();
        void releaseFields();

        jclass clazz_pluginManager;
        jmethodID c_pluginManager;
        jmethodID m_pluginManager_loadPlugins;

        jclass clazz_pluginManagerImpl;
        jfieldID f_pluginManagerImpl_handle;

        jclass clazz_plugin;
        jfieldID f_plugin_nPluginId;
        jmethodID m_plugin_load;
        jmethodID m_plugin_enable;
        jmethodID m_plugin_disable;

        jclass clazz_datastorage;
        jfieldID f_datastorage_stringLength;
        jfieldID f_datastorage_strings;
        jfieldID f_datastorage_longLength;
        jfieldID f_datastorage_longs;
        jfieldID f_datastorage_intLength;
        jfieldID f_datastorage_ints;
        jfieldID f_datastorage_byteLength;
        jfieldID f_datastorage_bytes;
        jfieldID f_datastorage_doubleLength;
        jfieldID f_datastorage_doubles;
        jfieldID f_datastorage_floatLength;
        jfieldID f_datastorage_floats;

        jclass clazz_illegalArgumentException;

        jmethodID m_object_toString;

        jmethodID m_methode_getName;

        jclass clazz_playerConnection;
        jfieldID f_playerConnection_static_players;
        jfieldID f_playerConnection_nativeAdress;
        jfieldID f_playerConnection_connectionState;
        jfieldID f_playerConnection_clientVersion;
        jfieldID f_playerConnection_playerName;
        jfieldID f_playerConnection_uuid;
        jfieldID f_playerConnection_adress;
        //TODO server fields....

        jclass clazz_server;
        jfieldID f_server_registeredServers;
        jfieldID f_server_name;
        jfieldID f_server_host;
        jfieldID f_server_port;

        jclass clazz_list;
        jmethodID m_list_add;
        jmethodID m_list_remove;

        jclass clazz_event;
        jfieldID f_event_storage;

        jclass clazz_console;

        std::string classToString(jobject clazz);
        std::string methodeToString(jobject methode);

        jobject createPlayerInstance(PlayerConnection* handle);
        void registerPlayer(PlayerConnection* connection);
        void unregisterPlayer(PlayerConnection* connection);
    private:
        JavaPluginManagerImpl& handle;
};


#endif //NATIVECORD_JAVAREFLECTMANAGER_H
