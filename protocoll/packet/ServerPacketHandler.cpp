//
// Created by wolverindev on 24.08.16.
//

#include "ServerPacketHandler.h"
#include "../../utils/UUIDUtils.h"
#include "../../chat/ChatMessage.h"
#include "../../protocoll/packet/Packets.h"
#include "../../connection/PlayerConnection.h"
#include "../../utils/EntityRewrite.h"

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
    string username;
    ChatMessage* message;
    string reason;
    Connection* old;
    switch (packetId) {
        case 0x00:
            reason = buffer->readString();
            cout << "Login denided! Reason: " << buffer->readString().c_str() << endl;
            message = new ChatMessage("§cTarget server denided login!\nReason: ");
            message->addSibling(new ChatMessage(reason));
            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                ((ServerConnection*)connection)->getPlayerConnection()->disconnect(message);
                return;
            }
            ((ServerConnection*)connection)->getPlayerConnection()->sendMessage(message);
            delete message;
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

            old = ((ServerConnection*)connection)->getPlayerConnection()->getCurrentTargetConnection();
            if(old != NULL && old != connection && old->getState() != ConnectionState::CLOSED){
                old->disconnect(NULL);
                old->setState(ConnectionState::CLOSED);
                delete old;
            }
            ((ServerConnection*)connection)->getPlayerConnection()->setCurrentTargetConnection((ServerConnection*) connection);

            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                buffer->resetReaderIndex();
                buffer->setReaderindex(buffer->getReaderindex()-1); //Packet id
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(buffer->readBuffer(buffer->readableBytes()));
                ((ServerConnection*)connection)->getPlayerConnection()->setState(ConnectionState::PLAYING);
            } else {
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(((ServerConnection*)connection)->getPlayerConnection()->getClientVersion(), new PacketPlayRespawn(1,0,0,string("default")));
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(((ServerConnection*)connection)->getPlayerConnection()->getClientVersion(), new PacketPlayRespawn(1,0,0,string("default"))); //Needed send only once :D @md_5
            }
            break;
        case 0x03:
            int t = buffer->readVarInt();
            cout << "Setting server packet threadshold to " << t << endl;
            connection->setThreadshold(t);
            break;
    }
}

void entityRewriteServer(int packetId, DataBuffer *buffer, ServerConnection *connection){ //TODO getRight rewrite
    int cversion = connection->getPlayerConnection()->getClientVersion();
    if(cversion == 210)
        EntityRewrite::entityRewrite210Server(packetId, buffer, connection->getPlayerId(),
                                              connection->getPlayerConnection()->getPlayerId());
    else if(cversion == 110 || cversion == 109 || cversion == 107)
        EntityRewrite::entityRewrite110Server(packetId, buffer, connection->getPlayerId(),
                                              connection->getPlayerConnection()->getPlayerId());
    else if(cversion == 47)
        EntityRewrite::entityRewrite47Server(packetId, buffer, connection->getPlayerId(),
                                             connection->getPlayerConnection()->getPlayerId());
}

void ServerPacketHandler::handlePacketPlay(int packetId, DataBuffer *buffer) {
    int rindex = buffer->getReaderindex();
    int clientVersion = ((ServerConnection*)connection)->getPlayerConnection()->getClientVersion();
    if(packetId == 0x23 && clientVersion > 46 || packetId == 0x01 && clientVersion == 46) {
        int playerId = buffer->readInt();
        bool del = true; //First time must send
        if(((ServerConnection*)connection)->getPlayerConnection()->getPlayerId() == -1) { //player version isnt defined
            ((ServerConnection *) connection)->getPlayerConnection()->setPlayerId(playerId);
            del = false;
        }
        ((ServerConnection*)connection)->setPlayerId(playerId);
        cout << "Your entity id: " << ((ServerConnection*)connection)->getPlayerConnection()->getPlayerId() << endl;
        cout << "Server entity id: " << playerId << endl;
        if(del) {
            //Dim Diff game type
            char gamemode = buffer->read();
            char dim = buffer->read();
            uint8_t diff = buffer->read();
            buffer->read(); //Tab list size
            string level = buffer->readString();
            ((ServerConnection *) connection)->getPlayerConnection()->writePacket( ((ServerConnection *) connection)->getPlayerConnection()->getClientVersion(), new PacketPlayRespawn(dim, diff, gamemode, level));
        }
        if(del)
          return;
    }
    entityRewriteServer(packetId, buffer, (ServerConnection *) connection);
    buffer->setReaderindex(rindex-1); //Packet id
    ((ServerConnection*)connection)->getPlayerConnection()->writePacket(buffer->readBuffer(buffer->readableBytes()));
} //TODO

void ServerPacketHandler::handlePacketStatus(int packetId, DataBuffer *buffer) {}

void ServerPacketHandler::onException(Exception* ex) {
    if(((ServerConnection *) connection)->getPlayerConnection()->getState() == ConnectionState::CLOSED || ((ServerConnection *) connection)->getState() == ConnectionState::CLOSED)
        return;
    if(((ServerConnection *) connection)->getPlayerConnection()->getState() == ConnectionState::LOGIN){
        ((ServerConnection *) connection)->getPlayerConnection()->disconnect(new ChatMessage(string("§cAn exception was thrown.\n§6Message: §5")+ex->what()));
    } else {
        ((ServerConnection *) connection)->getPlayerConnection()->sendMessage(string("§cAn exception was thrown.\n§6Message: §5")+ex->what());
    }
}
