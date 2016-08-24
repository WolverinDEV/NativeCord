//
// Created by wolverindev on 24.08.16.
//

#include "ServerPacketHandler.h"
#include "../../utils/UUIDUtils.h"
#include "../../chat/ChatMessage.h"
#include "../../protocoll/packet/Packets.h"
#include "../../connection/PlayerConnection.h"

void ServerPacketHandler::handlePacket(DataBuffer *buffer) {
    cout << "Having server packet" << endl;
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
    string username;
    switch (packetId) {
        case 0x00:
            cout << "Login denided! Reason: " << buffer->readString().c_str() << endl;
            ((ServerConnection*)connection)->getPlayerConnection()->disconnect(new ChatMessage("§cTarget server denided login!")); //TODO fallback etc...
            break;
        case 0x01:
            throw  new Exception("Server is in online mode!");
        case 0x02:
            buffer->markReaderIndex();
            cout << "Target server accept login!" << endl;
            uuid_t playerUUID;
            uuid_parse(buffer->readString().c_str(),playerUUID);
            username = buffer->readString();
            cout << "Username: " << username << endl;
            cout << "UUID:     " << UUIDUtils::uuidToStrring(playerUUID) << endl;
            connection->setState(ConnectionState::PLAYING);

            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                buffer->resetReaderIndex();
                buffer->setReaderindex(buffer->getReaderindex()-1); //Packet id
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(buffer->readBuffer(buffer->readableBytes()));
                ((ServerConnection*)connection)->getPlayerConnection()->setState(ConnectionState::PLAYING);
            }
            break;
        case 0x03:
            int t = buffer->readVarInt();
            cout << "Setting threadshold to " << t << endl;
            connection->setThreadshold(t);
            break;
    }
}

void ServerPacketHandler::handlePacketPlay(int packetId, DataBuffer *buffer) {
    buffer->setReaderindex(buffer->getReaderindex()-1); //Packet id
    ((ServerConnection*)connection)->getPlayerConnection()->writePacket(buffer->readBuffer(buffer->readableBytes()));
} //TODO

void ServerPacketHandler::handlePacketStatus(int packetId, DataBuffer *buffer) {}