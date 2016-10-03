//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_PLUGIN_H
#define NATIVECORD_PLUGIN_H

#include <string>

class PluginManager;
class Plugin {
    public:
        virtual std::string getName();
        virtual std::string getVersion();

        virtual void onEnable();
        virtual void onDisable();

        PluginManager* getPluginManager(){
            return manager;
        }
    private:
        PluginManager* manager;
};

#endif //NATIVECORD_PLUGIN_H
