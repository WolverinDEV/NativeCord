//
// Created by wolverindev on 09.10.16.
//

#include "../../../include/plugin/java/JavaReflectManager.h"
#include "../../../include/plugin/java/JavaPluginManagerImpl.h"

bool trueError(bool boolean, string message){
    if(boolean){
        logFatal(message);
    }
    return boolean;
}

bool JavaReflectManager::loadFields() {
    JNIEnv* env = handle.getEnv();

    clazz_pluginManagerImpl = env->FindClass("dev/wolveringer/nativecord/impl/PluginManagerImpl");
    if(clazz_pluginManagerImpl == NULL){
        logError("Cant find class dev/wolveringer/nativecord/impl/PluginManagerImpl!");
        return 0;
    }

    clazz_pluginManager = env->FindClass("dev/wolveringer/nativecord/plugin/PluginManager");
    if(trueError(clazz_pluginManager == NULL, "Cant find class dev/wolveringer/nativecord/plugin/PluginManager!")) return 0;

    f_pluginManagerImpl_handle = env->GetFieldID(clazz_pluginManagerImpl, "handle", "Ldev/wolveringer/nativecord/plugin/PluginManager;");
    if(f_pluginManagerImpl_handle == NULL){
        logError("Missing handle field in plugin manager impl!");
        return 0;
    }
    c_pluginManager = env->GetMethodID(clazz_pluginManager, "<init>", "(Ldev/wolveringer/nativecord/impl/PluginManagerImpl;)V");
    if(c_pluginManager == NULL){
        logError("Cant find plugin manager constructor!");
        return 0;
    }

    m_pluginManager_loadPlugins = env->GetMethodID(clazz_pluginManager, "loadPlugins", "()V");

    if(m_pluginManager_loadPlugins == NULL){
        logError("Cant find load plugins methode!");
        return 0;
    }

    clazz_plugin = env->FindClass("dev/wolveringer/nativecord/plugin/Plugin");
    if(clazz_plugin == NULL){
        logError("Cant find class dev/wolveringer/nativecord/plugin/Plugin!");
        return 0;
    }

    f_plugin_nPluginId = env->GetFieldID(clazz_plugin, "nativePluginAddress","J");
    if(f_plugin_nPluginId == NULL){
        logError("Cant find field nativePluginAddress in plugin class!");
        return 0;
    }

    m_plugin_load = env->GetMethodID(clazz_plugin,"load","()V");
    m_plugin_enable = env->GetMethodID(clazz_plugin,"enable","()V");
    m_plugin_disable = env->GetMethodID(clazz_plugin,"disable","()V");
    if(m_plugin_load == NULL || m_plugin_enable == NULL || m_plugin_disable == NULL){
        logError("Cant find some state methodes!");
        return 0;
    }


    clazz_datastorage = env->FindClass("dev/wolveringer/nativecord/impl/DataStorage");
    if(clazz_datastorage == NULL){
        logError("Cant find class dev/wolveringer/nativecord/impl/DataStorage!");
        return 0;
    }

    f_datastorage_stringLength = env->GetFieldID(clazz_datastorage, "stringLength","I");
    f_datastorage_longLength = env->GetFieldID(clazz_datastorage, "longLength","I");
    f_datastorage_intLength = env->GetFieldID(clazz_datastorage, "intLength","I");
    f_datastorage_byteLength = env->GetFieldID(clazz_datastorage, "byteLength","I");
    f_datastorage_doubleLength = env->GetFieldID(clazz_datastorage, "doubleLength","I");
    f_datastorage_floatLength = env->GetFieldID(clazz_datastorage, "floatLength","I");

    if(f_datastorage_stringLength == NULL || f_datastorage_longLength == NULL || f_datastorage_intLength == NULL || f_datastorage_byteLength == NULL || f_datastorage_doubleLength == NULL || f_datastorage_floatLength == NULL){
        logError("Cant find some value length fields!");
        return 0;
    }

    f_datastorage_strings = env->GetFieldID(clazz_datastorage,"strings","[Ljava/lang/String;");
    f_datastorage_longs = env->GetFieldID(clazz_datastorage,"longs","[J");
    f_datastorage_ints = env->GetFieldID(clazz_datastorage,"ints","[I");
    f_datastorage_bytes = env->GetFieldID(clazz_datastorage,"bytes","[B");
    f_datastorage_doubles = env->GetFieldID(clazz_datastorage,"doubles","[D");
    f_datastorage_floats = env->GetFieldID(clazz_datastorage,"floats","[F");

    if(f_datastorage_strings == NULL || f_datastorage_longs == NULL || f_datastorage_ints == NULL || f_datastorage_bytes == NULL || f_datastorage_doubles == NULL || f_datastorage_floats == NULL){
        logError("Cant find some value fields!");
        return 0;
    }

    clazz_illegalArgumentException = env->FindClass("java/lang/IllegalArgumentException");
    m_object_toString = handle.getEnv()->GetMethodID(handle.getEnv()->FindClass("java/lang/Object"),"toString","()Ljava/lang/String;");
    m_methode_getName = handle.getEnv()->GetMethodID(handle.getEnv()->FindClass("java/lang/reflect/Method"),"getName","()Ljava/lang/String;");

    clazz_playerConnection = env->FindClass("dev/wolveringer/nativecord/api/player/PlayerConnection");

    f_playerConnection_static_players = env->GetStaticFieldID(clazz_playerConnection, "connections","Ljava/util/ArrayList;");
    f_playerConnection_nativeAdress = env->GetFieldID(clazz_playerConnection, "nativeAdress", "J");
    f_playerConnection_clientVersion = env->GetFieldID(clazz_playerConnection, "connectionVersion","I");
    f_playerConnection_playerName = env->GetFieldID(clazz_playerConnection, "playerName","Ljava/lang/String;");
    f_playerConnection_uuid = env->GetFieldID(clazz_playerConnection, "uuid", "Ljava/util/UUID;");

    if(trueError(f_playerConnection_static_players == NULL ||
                         f_playerConnection_clientVersion == NULL ||
                         f_playerConnection_nativeAdress == NULL ||
                         f_playerConnection_playerName == NULL ||
                         f_playerConnection_uuid == NULL, "Cant find playerConnection fields! "+to_string(f_playerConnection_static_players == NULL)+"/"+to_string(f_playerConnection_clientVersion == NULL)+"/"+to_string(f_playerConnection_nativeAdress == NULL)+"/"+to_string(f_playerConnection_playerName == NULL)+"/"+to_string(f_playerConnection_uuid == NULL)+"/"+to_string(clazz_playerConnection == NULL))) return 0;

    clazz_server = env->FindClass("dev/wolveringer/nativecord/api/server/ServerInfo");
    f_server_registeredServers = env->GetStaticFieldID(clazz_server, "registeredServers", "Ljava/util/List;");
    f_server_name = env->GetFieldID(clazz_server, "name", "Ljava/lang/String;");
    f_server_host = env->GetFieldID(clazz_server, "host", "Ljava/lang/String;");
    f_server_port = env->GetFieldID(clazz_server, "port", "I");

    if(trueError(clazz_server == NULL || f_server_registeredServers == NULL ||
                         f_server_name == NULL || f_server_host == NULL || f_server_port == NULL, "Cant find server classes/methods "))
        return 0;

    clazz_list = env->FindClass("java/util/ArrayList");
    m_list_add = env->GetMethodID(clazz_list,"add","(Ljava/lang/Object;)Z");
    m_list_remove = env->GetMethodID(clazz_list,"remove","(Ljava/lang/Object;)Z");

    if(trueError(m_list_add == NULL || m_list_remove == NULL, "Some list fields are null!")) return 0;

    clazz_event = env->FindClass("dev/wolveringer/nativecord/api/event/Event");
    f_event_storage = env->GetFieldID(clazz_event, "storage","Ldev/wolveringer/nativecord/impl/DataStorage;");
    if(trueError(clazz_event == NULL || f_event_storage == NULL, "Some event fields are null!")) return 0;

    clazz_console = env->FindClass("dev/wolveringer/nativecord/terminal/Console");
    return 1;
}

std::string JavaReflectManager::classToString(jobject clazz) {
    jboolean _true = 1;
    jstring out = (jstring) handle.getEnv()->CallObjectMethod(clazz,m_object_toString);
    return string(handle.getEnv()->GetStringUTFChars(out,&_true));
}

std::string JavaReflectManager::methodeToString(jobject methode) {
    jboolean _true = 1;
    jstring out = (jstring) handle.getEnv()->CallObjectMethod(methode,m_methode_getName);
    return string(handle.getEnv()->GetStringUTFChars(out,&_true));
}

jobject JavaReflectManager::createPlayerInstance(PlayerConnection *connection) {
    JNIEnv* env = handle.getEnv();
    jobject playerInstance = env->AllocObject(clazz_playerConnection);

    env->SetLongField(playerInstance, f_playerConnection_nativeAdress, (jlong) connection->getJavaNativeAddress());
    //TODO set adress field!
    return env->NewGlobalRef(playerInstance);
}

void JavaReflectManager::registerPlayer(PlayerConnection *connection) {
    JNIEnv* env = handle.getEnv();
    jobject list = env->GetStaticObjectField(clazz_playerConnection, f_playerConnection_static_players);
    if(!env->CallBooleanMethod(list, m_list_add, connection->getJavaInstance())){
        logError("Cant add player ("+to_string((uint64_t)connection)+") to existing player connections!");
    }
} //TODO
void JavaReflectManager::unregisterPlayer(PlayerConnection *connection) {
    JNIEnv* env = handle.getEnv();
    jobject  list = env->GetStaticObjectField(clazz_playerConnection, f_playerConnection_static_players);
    if(!env->CallBooleanMethod(list, m_list_remove, connection->getJavaInstance())){
        logError("Cant remove player ("+to_string((uint64_t)connection)+")["+connection->getName()+"] from existing player connections!");
    }
} //TODO