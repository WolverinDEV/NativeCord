//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_JAVAPLUGINMANAGERIMPL_H
#define NATIVECORD_JAVAPLUGINMANAGERIMPL_H

#include "jni.h"
#include "../../../include/plugin/PluginManager.h"

class JavaPluginManagerImpl : public PluginManager{
    public:
        virtual vector<Plugin *> &getLoadedPlugins() override;

        virtual vector<Plugin *> &getEnabledPlugins() override;

        virtual Plugin *enablePlugin(string name) override;

        virtual bool disablePlugin(Plugin *plugin) override;

        virtual void enable() override;

        virtual void disable() override;

        void startJavaVM();
        void stopJavaVM();
    private:
        vector<Plugin*> plugins;
        JNIEnv *env;
        JavaVM *jvm;
};


#endif //NATIVECORD_JAVAPLUGINMANAGERIMPL_H
