//
// Created by wolverindev on 10.10.16.
//

#ifndef NATIVECORD_EVENTHELPER_H
#define NATIVECORD_EVENTHELPER_H

#include <map>
#include <string>
#include "jni.h"
#include "EventType.h"
#include "../java/jni/DataStorage.h"

class JavaPluginManagerImpl;
using namespace std;
class EventHelper {
    public:
        static string EventTypeName[];
        static string JavaClassMapping[];

        static jobject createJavaInstance(JavaPluginManagerImpl *impl, EventType type, DataStorage *buffer);

    private:
        static std::map <string, jclass> mapping;
};
#endif //NATIVECORD_EVENTHELPER_H
