//
// Created by wolverindev on 27.08.16.
//

#include "../../include/server/TabManager.h"
#include "../../include/connection/PlayerConnection.h"

void TabManager::resetTab() {
    if(uuids.empty())
        return;
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(connection->getClientVersion() == 46 ? 0x38 : 0x2D);
    buffer->writeVarInt(4); //Remove player
    buffer->writeVarInt(uuids.size());
    vector<uuid_t*> temp;
    for(std::vector<uuid_t*>::iterator it = this->uuids.begin(); it != this->uuids.end(); ++it) {
        uuid_t* playerId = *it;
        buffer->writeUUID(*playerId);
        temp.push_back(*it);
    }

    for(std::vector<uuid_t*>::iterator it = temp.begin(); it != temp.end(); ++it) {
        delete *it;
    }
    uuids.clear();
    temp.clear();
    connection->writePacket(buffer);
    delete buffer;
}