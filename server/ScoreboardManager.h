//
// Created by wolverindev on 27.08.16.
//

#ifndef CBUNGEE_SCOREBOARDMANAGER_H
#define CBUNGEE_SCOREBOARDMANAGER_H

#include <string>
#include <algorithm>
#include <iostream>
#include "../protocoll/Buffers/DataBuffer.h"

class PlayerConnection;
using namespace std;

class ScoreboardManager {
public:
    ScoreboardManager(PlayerConnection* con) : connection(connection){

    }
    ~ScoreboardManager(){
        objects.clear();
    }
    void resetScoreboard();

    void handleObjectivePacket(DataBuffer* buffer){
        string obj = buffer->readString();
        int mode = buffer->read();
        if(mode == 0)
            objects.push_back(obj);
        else if(mode == 1)
            removeObjective(obj);
    }
private:
    vector<string> objects;
    PlayerConnection* connection;

    void removeObjective(string& obj){
        vector<string> temp;
        for (std::vector<string>::iterator it = objects.begin(); it != objects.end(); ++it) {
            if (strcmp((*it).c_str(), obj.c_str()) == 0)
                temp.push_back(*it);
        }
        for (std::vector<string>::iterator it = temp.begin(); it != temp.end(); ++it) {
            objects.erase(std::find(objects.begin(),objects.end(),*it));
        }
    }
};


#endif //CBUNGEE_SCOREBOARDMANAGER_H
