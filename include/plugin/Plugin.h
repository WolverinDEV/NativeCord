//
// Created by wolverindev on 11.09.16.
//

#ifndef NATIVECORD_PLUGIN_H
#define NATIVECORD_PLUGIN_H

#include <string>
#include <map>
#include <functional>
#include "java/jni/DataStorage.h"
#include "event/EventType.h"
#include "../log/LogUtils.h"
#include "../protocoll/DataBuffer.h"
#include "../utils/TimeUtils.h"
#include "event/EventHelper.h"

class PluginManager;
class Plugin {
    public:
        Plugin(){
            this->pluginId = (uint64_t) this;
        }
        ~Plugin(){

        }

        virtual std::string getName() = 0;
        virtual std::string getVersion() = 0;

        virtual void onLoad(){};
        virtual void onEnable(){};
        virtual void onDisable(){};

        PluginManager* getPluginManager(){
            return manager;
        }

        uint64_t getPluginId(){
            return pluginId;
        }

        void registerEventListener(EventType event,string key,std::function<bool(DataStorage*)> function) throw(Exception){
            if(eventListeners[event].count(key) > 0){
                logError("Cant register "+key+" for event "+to_string(event)+". Alredy exist!");
                throw new Exception("Cant register "+key+" for event "+to_string(event)+". Alredy exist!");
            }
            eventListeners[event][key] = function;
        }

        void unregisterEventListener(EventType event, string key){
            eventListeners[event].erase(key);
        }

        bool isEventListenerRegistered(EventType type, string key){
            return eventListeners[type].count(key) > 0;
        }

        void unregisterAllEventListeners(){
            eventListeners.clear();
        }

        void callEvent(EventType type, DataStorage* storage){
            for(std::map<std::string, std::function<bool(DataStorage*)>>::iterator it = eventListeners[type].begin(); it != eventListeners[type].end(); it++){
                try{
                    uint64_t start = TimeUtils::getCurrentTimeMillis();
                    it->second(storage);
                    uint64_t end = TimeUtils::getCurrentTimeMillis();
                    if(end-start > 300){
                        logError("Needed more that 300ms to run event "+EventHelper::EventTypeName[type]+" ("+to_string(end-start)+"ms)");
                    }
                }catch (std::exception ex){
                    logError("An error ("+string(typeid(ex).name())+") happend while progressing event "+EventHelper::EventTypeName[type]+". Message: "+ex.what());
                }catch (std::exception* ex){
                    logError("An error ("+string(typeid(*ex).name())+") happend while progressing event "+EventHelper::EventTypeName[type]+". Message: "+ex->what());
                    delete ex;
                }catch (...){
                    logError("An error (unknown) happend while progressing event "+EventHelper::EventTypeName[type]);
                }
            }
        }
    protected:
        std::map<int, std::map<std::string ,std::function<bool(DataStorage*)>>> eventListeners;
        uint64_t pluginId;
        bool enabled;
    private:
        PluginManager* manager = nullptr;
};

#endif //NATIVECORD_PLUGIN_H
