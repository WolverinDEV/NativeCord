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
    return "nundefined";
}

string JavaPlugin::getVersion() {
    return "nundefined";
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