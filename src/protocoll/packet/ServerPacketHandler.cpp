//
// Created by wolverindev on 24.08.16.
//

#include "../../../include/protocoll/ServerPacketHandler.h"
#include "../../../include/utils/UUIDUtils.h"
#include "../../../include/connection/PlayerConnection.h"
#include "../../../include/utils/EntityRewrite.h"

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
            message = new ChatMessage(string("§c§l» §7Cant connect to ").append(((ServerConnection*)connection)->getServerInfo()->getName()).append("\n§6§l» §7Reason: §f"));
            message->addSibling(new ChatMessage(json::parse(reason)));
            if(((ServerConnection*)connection)->getPlayerConnection()->getState() == LOGIN){
                removeFromPending();
                if(((ServerConnection*)connection)->getPlayerConnection()->getFallbackServers().empty()){
                    ((ServerConnection*)connection)->getPlayerConnection()->disconnect(new ChatMessage(string("§fNative-Proxy:\n§cNo avariable fallback server found.")));
                    return;
                }
                return;
            }
            removeFromPending();
            if(((ServerConnection*)connection)->getPlayerConnection()->getCurrentTargetConnection() == NULL || ((ServerConnection*)connection)->getPlayerConnection()->getCurrentTargetConnection()->getState() == ConnectionState::CLOSED){
                if(((ServerConnection*)connection)->getPlayerConnection()->getFallbackServers().empty())
                    ((ServerConnection*)connection)->getPlayerConnection()->disconnect(message);
                return;
            }
            ((ServerConnection*)connection)->getPlayerConnection()->sendMessage(message);
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
            cout << "UUID:     " << UUIDUtils::uuidToString(playerUUID) << endl;
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
                ((ServerConnection*)connection)->getPlayerConnection()->sendDimswitch();
            }
            ((ServerConnection*)connection)->getPlayerConnection()->getTabManager()->resetTab();
            ((ServerConnection*)connection)->getPlayerConnection()->getScoreboardManager()->resetScoreboard();
            ((ServerConnection *) connection)->getPlayerConnection()->resetFallbackQueue();
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
    if(packetId == 0x23 && clientVersion > 46 || packetId == 0x01 && clientVersion == 46) { //TODO 1.8 The packed sended twice or somethink like that lol? idk
        int playerId = buffer->readInt();
        bool del = true; //First time must send
        if(((ServerConnection*)connection)->getPlayerConnection()->getPlayerId() == -1) { //player version isnt defined
            ((ServerConnection *) connection)->getPlayerConnection()->setPlayerId(playerId);
            del = false;
        }
        ((ServerConnection*)connection)->setPlayerId(playerId);
        cout << "PacketID " << packetId << endl;
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
        ((ServerConnection*)connection)->getPlayerConnection()->setDimswitch(false);
        if(del)
          return;
    }
    if(packetId == 0x2D && clientVersion > 46 || packetId == 0x38 && clientVersion == 46) {
        ((ServerConnection *) connection)->getPlayerConnection()->getTabManager()->handleTabPacket(buffer);
    }
    if(packetId == 0x3F && clientVersion > 46 || packetId == 0x3B && clientVersion == 46) {
        ((ServerConnection *) connection)->getPlayerConnection()->getScoreboardManager()->handleObjectivePacket(buffer);
    }
    if(packetId == 0x1A && clientVersion > 46 || packetId == 0x40 && clientVersion == 46) {
        //Kick packet
        if(!((ServerConnection*)connection)->getPlayerConnection()->getFallbackServers().empty()){
            ((ServerConnection*)connection)->getPlayerConnection()->sendDimswitch();
            PacketPlayDisconnect* disconnect = new PacketPlayDisconnect();
            disconnect->read(((ServerConnection *) connection)->getPlayerConnection()->getClientVersion(), buffer);
            ChatMessage* message = new ChatMessage("§6§l» §7You have been kicked.\n§6§l» §7Reason: §f");
            if(disconnect->getMessage() != nullptr) {
                ((ServerConnection*)connection)->getPlayerConnection()->setLastDisconnectMessage(disconnect->getMessage()->clone());
                message->addSibling(disconnect->getMessage()->clone());
            }else
                message->addSibling(new ChatMessage("No reason!"));
            ((ServerConnection*)connection)->getPlayerConnection()->sendMessage(message);
            delete message;
            delete disconnect; //TODO Memory leak!
            ((ServerConnection*)connection)->closeChannel();
            return;
        }
    }
    entityRewriteServer(packetId, buffer, (ServerConnection *) connection);
    buffer->setReaderindex(rindex-1); //Packet id
    DataBuffer* data = buffer->readBuffer(buffer->readableBytes());
    try{
        ((ServerConnection*)connection)->getPlayerConnection()->writePacket(data);
        delete data;
    }catch (...){
        delete data;
    };
} //TODO

void ServerPacketHandler::handlePacketStatus(int packetId, DataBuffer *buffer) {}

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
