//
// Created by wolverindev on 24.08.16.
//

#include "ServerPacketHandler.h"


void ServerPacketHandler::handlePacket(DataBuffer *buffer) {
    int packetId = buffer->readVarInt();
    switch (this->connection->getState()) {
        case ConnectionState ::HANDSHAKING:
            handlePacketHandschake(packetId, buffer);
            return;
        case ConnectionState::STATUS:
            handlePacketStatus(packetId, buffer);
            return;
        case ConnectionState::LOGIN:
            handlePacketLogin(packetId, buffer);
            return;
        case ConnectionState::PLAYING:
            handlePacketPlay(packetId, buffer);
            return;
        default:
            return;
    }
}

void ServerPacketHandler::handlePacketHandschake(int packetId, DataBuffer *buffer) {
    throw new Exception("Cant have a handshake packet on ServerConnection!");
}

void ServerPacketHandler::handlePacketLogin(int packetId, DataBuffer *buffer) {
    switch (packetId) {
        case 0x01:
            throw  new Exception("Server is in online mode!");
    }
}

void ServerPacketHandler::handlePacketPlay(int packetId, DataBuffer *buffer) {} //TODO

void ServerPacketHandler::handlePacketStatus(int packetId, DataBuffer *buffer) {}

void ServerPacketHandler::connect() {
    DataBuffer* buffer = new DataBuffer();
    new PacketHandshake(210,string("localhost"),25565,2);
}