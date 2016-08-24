//
// Created by wolverindev on 23.08.16.
//

#include "ClientPacketHandler.h"
#include "Packets.h"
#include "../../json/json.hpp"
#include "../../connection/ServerConnection.h"
#include "../../connection/Connection.h"
#include "../../utils/TimeUtils.h"
#include "../../utils/StringUtil.h"
#include "../../utils/EntityRewrite.h"

// for convenience
using json = nlohmann::json;

void ClientPacketHandler::forwardPacket(DataBuffer *buffer) {
    //connection->writePacket(buffer);
}

void ClientPacketHandler::handlePacketHandschake(const int packetId, DataBuffer *buffer) {
    PacketHandshake* handshake;
    switch (packetId) {
        case 0x00:
            cout << "Read packet" << endl;
            handshake = new PacketHandshake();
            handshake->read(-1, buffer);
            pconnection->setHandshake(handshake);
            switch (handshake->getState()) {
                case 0x01:
                    connection->setState(ConnectionState::STATUS);
                    break;
                case 0x02:
                    connection->setState(ConnectionState ::LOGIN);
                    break;
                default:
                    cout << "Cant find state: " << handshake->getState() << endl;
                    break;
            }
            cout << "Having handshake." << endl;
            cout << "Version: " << handshake->getClientVersion() << endl;
            cout << "State:   " << handshake->getState() << endl;
            break;
        default:
            std::cout << "Cant handle packet (" << packetId << ") at handshake! Disconnecting client!" << std::endl;
            connection->getSocket()->closeSocket();
            //TODO memory clear
            break;
    }
}


void ClientPacketHandler::handlePacketStatus(int packetId, DataBuffer *buffer) {
    DataBuffer* rbuffer = NULL;
    json* response = NULL;
    uint64_t time;
    switch (packetId) {
        case 0x00:
            cout << "Requesting status" << endl;
            rbuffer = new DataBuffer();
            rbuffer->writeVarInt(0x00);
            response = new json();
            (*response)["version"] = {{"name", "NativeCord [1.8.X-1.10.X] by WolverinDEV"},{"protocol",pconnection->getHandshake()->getClientVersion()}};
            (*response)["players"] = {{"max",100},{"online",1000}};
            //for(int i = 0;i<100;i++){
            //    (*response)["players"]["sample"][i] = {{"name",string("WolverinDEV [").append(to_string(i)).append("]")},{"id","456ee69f-c907-48ee-8d71-d7ba5aa00d20"}};
            //}
            (*response)["description"] = {"§aNativecord says hello\n§4Mady with love by §c§lWolverinDEV"};
            rbuffer->writeString(string(response->dump()));
            connection->writePacket(rbuffer);
            delete rbuffer;
            delete response;
            break;
        case 0x01: //Ping pong...
            time = buffer->readLong();
            rbuffer = new DataBuffer();
            rbuffer->writeVarInt(0x01);
            rbuffer->writeLong(TimeUtils::getCurrentTimeMillis());
            connection->writePacket(rbuffer);
            delete rbuffer;
            break;
        default:
            cout << "Cant handle packet (" << packetId << ") at status! Disconnecting client!" << endl;
            connection->getSocket()->closeSocket();
            //TODO memory clear
    }
}

void ClientPacketHandler::handlePacketLogin(int packetId, DataBuffer *buffer) {
    ChatMessage* message;
    Socket* target;
    switch (packetId) {
        case 0x00:
            pconnection->setName(buffer->readString());
            cout << "Player ["<<pconnection->getName()<<"] connecting" << endl;
            cout << "Start connecting to an server" << endl;
            target = SocketUtil::createTCPSocket("localhost", 25567);
            if(target == NULL){
                pconnection->disconnect(new ChatMessage(string("§cCant reach target server!")));
                return;
            }
            pconnection->setCurrentTargetConnection(new ServerConnection(pconnection,target));
            pconnection->getCurrentTargetConnection()->startConnect();
            //message = new ChatMessage(string("§cDu kannst leider momentan noch nicht §nconnecten§c!"));
            //pconnection->disconnect(message);
            break;
        default:
            std::cout << "Cant handle packet (" << packetId << ") at login! Disconnecting client!" << std::endl;
            connection->getSocket()->closeSocket();
            //TODO memory clear
            break;
    }
}

void ClientPacketHandler::handlePacketPlay(int packetId, DataBuffer *buffer) {
    if(pconnection->getClientVersion() >= 107 && packetId == 0x02 || pconnection->getClientVersion() == 47 && packetId == 0x01){
        string message = buffer->readString();
        vector<string> parts = StringUtils::split(message,' ');
        cout << "Having chat message: " << message << " ("<<parts[0].c_str()<< ")" << endl;
        if(strcmp(parts[0].c_str(),"/cbungee") == 0){
            pconnection->sendMessage("§aNativeCord by WolverinDEV version 0.2-ALPHA");
            return;
        }
        if(strcmp(parts[0].c_str(),"/server") == 0){
            if(parts.size() == 2){
                Socket* target = SocketUtil::createTCPSocket("localhost",25568);
                pconnection->connect(target);
                pconnection->sendMessage("§aConnected");
                return;
            }
            pconnection->sendMessage("§6Server list:");
            pconnection->sendMessage("§6No config = no servers //TODO add config"); //TODO
            return;
        }
    }
    if(pconnection->getCurrentTargetConnection() != NULL)
        EntityRewrite::entityRewide210Client(packetId,buffer,pconnection->getPlayerId(),pconnection->getCurrentTargetConnection()->getPlayerId());
    if(pconnection->getCurrentTargetConnection() != NULL){
        buffer->setReaderindex(buffer->getReaderindex()-DataBuffer::getVarIntSize(packetId)); //Packet id
        pconnection->getCurrentTargetConnection()->writePacket(buffer);
    }
} //TODO

void ClientPacketHandler::handlePacket(DataBuffer *buffer) {
    int packetId = buffer->readVarInt();
    switch (pconnection->getState()) {
        case ConnectionState::HANDSHAKING:
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