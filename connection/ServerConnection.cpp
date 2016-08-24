//
// Created by wolverindev on 24.08.16.
//

#include "ServerConnection.h"

#include "PlayerConnection.h"
#include "../protocoll/packet/ServerPacketHandler.h"

void ServerConnection::startConnect() {
    ((ServerPacketHandler*)phandler)->startReader();
    writePacket(getPlayerConnection()->getClientVersion(), getPlayerConnection()->getHandshake(), false); //handshake
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(0x00);
    buffer->writeString(getPlayerConnection()->getName());
    setState(ConnectionState::LOGIN);
    writePacket(buffer);
    delete(buffer);
}