//
// Created by wolverindev on 03.10.16.
//

#ifndef NATIVECORD_PLUGINMANAGER_H
#define NATIVECORD_PLUGINMANAGER_H

#include <vector>
#include <string>
#include "Plugin.h"

using namespace std;
class PluginManager {
    public:
        virtual bool enable() = 0;
        virtual bool disable() = 0;

        virtual vector<Plugin*>& getLoadedPlugins() = 0;
        virtual vector<Plugin*>& getEnabledPlugins() = 0;

        virtual bool enablePlugin(Plugin* plugin) = 0;
        virtual bool disablePlugin(Plugin* plugin) = 0;

        virtual void callEvent(EventType type,DataStorage* storage) = 0;
};


#endif //NATIVECORD_PLUGINMANAGER_H
