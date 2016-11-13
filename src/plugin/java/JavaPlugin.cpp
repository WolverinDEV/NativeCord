//
// Created by wolverindev on 09.10.16.
//

#include "../../../include/plugin/java/JavaPlugin.h"

JavaPlugin::JavaPlugin(JavaPluginManagerImpl &manager, jobject instance)  : Plugin(), manager(manager) {
    this->pluginInstance = manager.getEnv()->NewGlobalRef(instance);
    manager.getEnv()->SetLongField(this->pluginInstance, manager.getRefelectManager()->f_plugin_nPluginId, this->pluginId);
}

JavaPlugin::~JavaPlugin() {
    manager.getEnv()->DeleteGlobalRef(this->pluginInstance);
}

string JavaPlugin::getName() {
    return string((char*) JavaPluginManagerImpl::instance->runOperation([&](JNIEnv* env){
        jobject description = env->GetObjectField(this->pluginInstance, JavaPluginManagerImpl::instance->getRefelectManager()->f_plugin_description);
        jstring field = (jstring) env->GetObjectField(description, JavaPluginManagerImpl::instance->getRefelectManager()->f_pluginDescription_name);
        jboolean _false = JNI_FALSE;
        return (void*) env->GetStringUTFChars(field, &_false);
    }));
}

string JavaPlugin::getVersion() {
    return string((char*) JavaPluginManagerImpl::instance->runOperation([&](JNIEnv* env){
        jobject description = env->GetObjectField(this->pluginInstance, JavaPluginManagerImpl::instance->getRefelectManager()->f_plugin_description);
        jstring field = (jstring) env->GetObjectField(description, JavaPluginManagerImpl::instance->getRefelectManager()->f_pluginDescription_version);
        jboolean _false = JNI_FALSE;
        return (void*) env->GetStringUTFChars(field, &_false);
    }));
}

vector<string> JavaPlugin::getAuthors() {
    JNIEnv* env = JavaPluginManagerImpl::instance->getEnv();
    jobject description = env->GetObjectField(this->pluginInstance, JavaPluginManagerImpl::instance->getRefelectManager()->f_plugin_description);
    jobject authorList = env->GetObjectField(description, JavaPluginManagerImpl::instance->getRefelectManager()->f_pluginDescription_authors);

    int size = env->CallIntMethod(authorList, JavaPluginManagerImpl::instance->getRefelectManager()->m_list_size);
    vector<string> authors;
    jboolean _false = JNI_FALSE;

    for(int i = 0;i<size;i++){
        jstring author = (jstring) env->CallObjectMethod(authorList, JavaPluginManagerImpl::instance->getRefelectManager()->m_list_get, (jint) i);
        authors.push_back(string(env->GetStringUTFChars(author, &_false)));
    }
    return authors;
}


void JavaPlugin::load() {
    debugMessage("Invoke load methods ("+to_string((long) this->pluginInstance)+")");
    JavaPluginManagerImpl::instance->getEnv()->CallVoidMethod(this->pluginInstance, JavaPluginManagerImpl::instance->getRefelectManager()->m_plugin_load);
    JavaPluginManagerImpl::instance->flushException();
}

void JavaPlugin::enable() {
    debugMessage("Invoke enable methods ("+to_string((long)this->pluginInstance)+")");
    JavaPluginManagerImpl::instance->getEnv()->CallVoidMethod(this->pluginInstance, JavaPluginManagerImpl::instance->getRefelectManager()->m_plugin_enable);
    if(JavaPluginManagerImpl::instance->flushException())
        return;
    debugMessage("Done!");
    this->enabled = true;
}