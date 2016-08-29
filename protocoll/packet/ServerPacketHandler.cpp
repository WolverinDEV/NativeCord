//
// Created by wolverindev on 24.08.16.
//

#include "ServerPacketHandler.h"
#include "../../utils/UUIDUtils.h"
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


Packet* dim1 = new PacketPlayRespawn(1,0,0,string("default"));
void ServerPacketHandler::handlePacketLogin(int packetId, DataBuffer *buffer) {
    string username;
    ChatMessage* message;
    string reason;
    Connection* old;
    switch (packetId) {
        case 0x00:
            reason = buffer->readString();
            message = new ChatMessage(string("§cTarget server denided login!\nReason: "));
            message->addSibling(new ChatMessage(json::parse(reason)));
            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                removeFromPending();
                ((ServerConnection*)connection)->getPlayerConnection()->disconnect(message);
                return;
            }
            ((ServerConnection*)connection)->getPlayerConnection()->sendMessage(message);
            removeFromPending();
            delete message;
            break;
        case 0x01:
            removeFromPending();
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

            removeFromPending();

            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                buffer->resetReaderIndex();
                buffer->setReaderindex(buffer->getReaderindex()-1); //Packet id
                DataBuffer* data = buffer->readBuffer(buffer->readableBytes());
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(data);
                delete data;
                ((ServerConnection*)connection)->getPlayerConnection()->setState(ConnectionState::PLAYING);
                PlayerConnection::activeConnections.push_back(((ServerConnection*)connection)->getPlayerConnection());
            } else {
                ((ServerConnection*)connection)->getPlayerConnection()->writePacket(((ServerConnection*)connection)->getPlayerConnection()->getClientVersion(), dim1, false);
                //((ServerConnection*)connection)->getPlayerConnection()->writePacket(((ServerConnection*)connection)->getPlayerConnection()->getClientVersion(), new PacketPlayRespawn(-1,0,0,string("default"))); need only one send. the second will send when server spawn the entity. May by send if server slow?
            }
            ((ServerConnection*)connection)->getPlayerConnection()->getTabManager()->resetTab();
            ((ServerConnection*)connection)->getPlayerConnection()->getScoreboardManager()->resetScoreboard();
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
        EntityRewrite::entityRewrite210Server(packetId, buffer, connection->getPlayerId(), connection->getPlayerConnection()->getPlayerId());
    else if(cversion == 110 || cversion == 109 || cversion == 107)
        EntityRewrite::entityRewrite110Server(packetId, buffer, connection->getPlayerId(), connection->getPlayerConnection()->getPlayerId());
    else if(cversion == 47)
        EntityRewrite::entityRewrite47Server(packetId, buffer, connection->getPlayerId(),  connection->getPlayerConnection()->getPlayerId());
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
    if(packetId == 0x2D && clientVersion > 46 || packetId == 0x38 && clientVersion == 46) {
        ((ServerConnection *) connection)->getPlayerConnection()->getTabManager()->handleTabPacket(buffer);
    }
    if(packetId == 0x3F && clientVersion > 46 || packetId == 0x3B && clientVersion == 46) {
        ((ServerConnection *) connection)->getPlayerConnection()->getScoreboardManager()->handleObjectivePacket(buffer);
    }
    entityRewriteServer(packetId, buffer, (ServerConnection *) connection);
    buffer->setReaderindex(rindex-1); //Packet id
    DataBuffer* data = buffer->readBuffer(buffer->readableBytes());
    ((ServerConnection*)connection)->getPlayerConnection()->writePacket(data);
    delete data;
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

void ServerPacketHandler::removeFromPending() {
    vector<ServerConnection*> temp;
    vector<ServerConnection*> v = (((ServerConnection*)connection)->getPlayerConnection()->getPendingConnection());
    for (std::vector<ServerConnection *>::iterator it = v.begin(); it != v.end(); ++it) {
        if ((ServerConnection*) connection == *it)
            temp.push_back(*it);
    }
    for (std::vector<ServerConnection *>::iterator it = temp.begin(); it != temp.end(); ++it) {
        v.erase(std::find(v.begin(),v.end(),*it));
    }
}