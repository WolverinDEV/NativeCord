//
// Created by wolverindev on 09.10.16.
//

#ifndef NATIVECORD_JAVAREFLECTMANAGER_H
#define NATIVECORD_JAVAREFLECTMANAGER_H

#include "jni.h"
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

        std::string classToString(jobject clazz);
        std::string methodeToString(jobject methode);
    private:
        JavaPluginManagerImpl& handle;
};


#endif //NATIVECORD_JAVAREFLECTMANAGER_H
