//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_JAVAPLUGINMANAGERIMPL_H
#define NATIVECORD_JAVAPLUGINMANAGERIMPL_H

#include "jni.h"
#include "../PluginManager.h"
#include "jni/DataStorageImpl.h"
#include "JavaReflectManager.h"

class JavaPluginManagerImpl : public PluginManager{
    public:
        static JavaPluginManagerImpl* instance;

        ~JavaPluginManagerImpl();
        JavaPluginManagerImpl();

        virtual vector<Plugin *> &getLoadedPlugins() override;

        virtual vector<Plugin *> &getEnabledPlugins() override;

        virtual bool enablePlugin(Plugin* plugin) override;

        virtual bool disablePlugin(Plugin *plugin) override;

        virtual bool enable() override;

        virtual bool disable() override;

        bool startJavaVM();
        bool registerNatives();
        bool stopJavaVM();

        const DataStorageImpl* getStorageImpl() const;

        DataStorageImpl *storageImpl;

        JNIEnv* getEnv(){
            return env;
        }

        bool flushException();

        static long NATIVE_registerPlugin(JNIEnv* env, jobject manager, jobject plugin);
        static void NATIVE_enablePlugin(JNIEnv* env, jobject manager, long plugin);
        static void NATIVE_disablePlugin(JNIEnv* env, jobject manager, long plugin);

        static void NATIVE_registerListener(JNIEnv* env, jobject manager, jlong plugin, jobject listenerClassInstance, jint eventId, jobject ref_methode);
        static void NATIVE_unregisterListener0(JNIEnv* env, jobject manager, jlong plugin);
        static void NATIVE_unregisterListener1(JNIEnv* env, jobject manager, jlong plugin, jobject listenerClassInstance);


        Plugin* getPlugin(long pluginId){
            for(vector<Plugin*>::iterator it = plugins.begin(); it != plugins.end();it++)
                if((*it)->getPluginId() == pluginId)
                    return *it;
            return nullptr;
        }

        JavaReflectManager* getRefelectManager(){
            return reflectManager;
        }
    private:
        vector<Plugin*> plugins;
        JNIEnv *env;
        JavaVM *jvm;
        JavaReflectManager* reflectManager;
        jobject pluginManager;
        jobject pluginManagerImpl;
};


#endif //NATIVECORD_JAVAPLUGINMANAGERIMPL_H
