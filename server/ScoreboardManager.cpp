//
// Created by wolverindev on 27.08.16.
//

#include "ScoreboardManager.h"
#include "../connection/PlayerConnection.h"

void ScoreboardManager::resetScoreboard() {
    if(objects.empty())
        return;
    for(std::vector<string>::iterator it = this->objects.begin(); it != this->objects.end(); ++it) {
        DataBuffer* buffer = new DataBuffer();
        buffer->writeVarInt(connection->getClientVersion() == 46 ? 0x3B : 0x3F);
        buffer->writeString(*it);
        buffer->writeByte(1); //remove
        connection->writePacket(buffer);
        delete buffer;
    }
    objects.clear();
}