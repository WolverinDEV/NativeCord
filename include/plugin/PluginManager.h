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
        vector<Plugin*>& getLoadedPlugins();
        vector<Plugin*>& getEnabledPlugins();

        Plugin* enablePlugin(string name);
        bool disablePlugin(Plugin* plugin);
};


#endif //NATIVECORD_PLUGINMANAGER_H
