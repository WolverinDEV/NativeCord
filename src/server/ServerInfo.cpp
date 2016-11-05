//
// Created by wolverindev on 29.08.16.
//

#include "../../include/server/ServerInfo.h"
#include "../../include/plugin/java/JavaPluginManagerImpl.h"
vector<ServerInfo*> ServerInfo::servers;

ServerInfo::ServerInfo(const string &name, const string &host, int port, bool visible) : name(name), host(host), port(port), visible(visible) {
    createJavaInstance();
}

void ServerInfo::createJavaInstance() {
    if(JavaPluginManagerImpl::instance != nullptr) {
        JNIEnv *env = JavaPluginManagerImpl::instance->getEnv();
        this->javaObject = env->AllocObject(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_server);
        env->NewGlobalRef(javaObject);
        env->SetObjectField(javaObject, JavaPluginManagerImpl::instance->getRefelectManager()->f_server_name, env->NewStringUTF(name.c_str()));
        env->SetObjectField(javaObject, JavaPluginManagerImpl::instance->getRefelectManager()->f_server_host, env->NewStringUTF(host.c_str()));
        env->SetIntField(javaObject, JavaPluginManagerImpl::instance->getRefelectManager()->f_server_port, 33);
    }
}

jobject ServerInfo::NATIVE_registerServer(JNIEnv *env, jclass caller, jstring jname, jstring jhost, jint jport, jboolean addToConfig) {
    string name = string(env->GetStringUTFChars(jname, false));
    if(ServerInfo::getServerInfo(name) != nullptr){
        env->ThrowNew(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_illegalArgumentException, "Server with this name alredy registered by name.");
        return nullptr;
    }

    string host = string(env->GetStringUTFChars((jstring) jhost, false));
    if(ServerInfo::getServerInfo(host, jport) != nullptr){
        env->ThrowNew(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_illegalArgumentException, "Server with this name alredy registered by adress.");
        return nullptr;
    }
    ServerInfo* info;
    try{
        if(addToConfig)
            info = ServerInfo::createServerInfo(name,host, jport);
        else
            info = ServerInfo::createTempServerInfo(name,host,jport);
    }catch (exception e){
        env->ThrowNew(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_illegalArgumentException, ("Can register server. Message: "+string(e.what())).c_str());
        return nullptr;
    }
    return info->javaObject;
}

void ServerInfo::registerServerInList(ServerInfo *info) {
    if(JavaPluginManagerImpl::instance != nullptr) {
        JNIEnv *env = JavaPluginManagerImpl::instance->getEnv();
        jobject listObject = env->GetStaticObjectField(JavaPluginManagerImpl::instance->getRefelectManager()->clazz_list, JavaPluginManagerImpl::instance->getRefelectManager()->f_server_registeredServers);
        env->CallObjectMethod(listObject, JavaPluginManagerImpl::instance->getRefelectManager()->m_list_add, info->javaObject);
    }
    ServerInfo::servers.push_back(info);
}