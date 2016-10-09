//
// Created by wolverindev on 09.10.16.
//

#include "../../../include/plugin/event/EventType.h"
#include "../../../include/utils/StringUtil.h"


/*
jobject EventHelper::createJavaInstance(JavaPluginManagerImpl* impl, EventType type, DataStorage *buffer) {
    string name = EventHelper::JavaClassMapping[type];
    if (name.empty())
        return nullptr;
    jclass cls;
    if (mapping.count(name) < 1)
        mapping[name] = (cls = impl->getEnv()->FindClass((name).c_str()));
    else
        cls = mapping[name];
    return impl->getEnv()->AllocObject(cls);
}
*/