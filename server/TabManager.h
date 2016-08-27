//
// Created by wolverindev on 27.08.16.
//

#ifndef CBUNGEE_TABMANAGER_H
#define CBUNGEE_TABMANAGER_H


#include <uuid/uuid.h>
#include "../protocoll/Buffers/DataBuffer.h"
#include <vector>

class PlayerConnection;

using namespace std;

class TabManager {
public:
    TabManager(PlayerConnection* connection) : connection(connection){

    }

    ~TabManager(){
        for(std::vector<uuid_t*>::iterator it = uuids.begin(); it != uuids.end(); ++it) {
            delete *it;
        }
        uuids.clear();
    }

    void resetTab();

    void handleTabPacket(DataBuffer* buffer){
        int action = buffer->readVarInt();
        int length = buffer->readVarInt();
        int plength;
        for(int i = 0;i<length;i++){
            uuid_t* uuid = buffer->readUUID();
            switch (action){
                case 0:
                    buffer->readString(); //Name
                    plength = buffer->readVarInt(); //Sizeof props
                    for(int j = 0;j<plength;j++){
                        buffer->readString(); //Name
                        buffer->readString(); //Value
                        if(buffer->read() == 1)
                            buffer->readString(); //Sig
                    }
                    buffer->readVarInt(); //Gamemode
                    buffer->readVarInt(); //Ping
                    if(buffer->read() == 1)
                        buffer->readString(); //Name
                    uuids.push_back(uuid);
                    break;
                case 1:
                    buffer->readVarInt();
                    break;
                case 2:
                    buffer->readVarInt();
                    break;
                case 3:
                    if(buffer->read() == 1)
                        buffer->readString();
                    break;
                case 4:
                    for(std::vector<uuid_t*>::iterator it = this->uuids.begin(); it != this->uuids.end(); ++it) {
                        if(*it == uuid)
                            uuids.erase(it);
                        delete *it;
                    }
                    break;
            }
        }
    }
private:
    vector<uuid_t*> uuids;
    PlayerConnection* connection;
};


#endif //CBUNGEE_TABMANAGER_H
