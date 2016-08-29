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
#include "../../config/Configuration.h"
#include "../../server/ServerInfo.h"
#include "../../connection/PlayerConnection.h"

// for convenience
using json = nlohmann::json;

void ClientPacketHandler::forwardPacket(DataBuffer *buffer) {
    //connection->writePacket(buffer);
}

void ClientPacketHandler::streamClosed() {
    if (pconnection->getCurrentTargetConnection() != NULL)
        pconnection->getCurrentTargetConnection()->disconnect(NULL);
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

bool isSupportedVersion(int ver){
    return ver == 210 || ver == 110 || ver == 109 || ver == 108 || ver == 107 || ver == 47;
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
            (*response)["version"] = {{"name", "NativeCord [1.8.X-1.10.X] by WolverinDEV"},{"protocol",isSupportedVersion(pconnection->getHandshake()->getClientVersion()) ? pconnection->getHandshake()->getClientVersion() : -1}};
            (*response)["players"] = {{"max",Configuration::instance->config["ping"]["visible_player_limit"].as<int>()},{"online",PlayerConnection::activeConnections.size()}};
            //for(int i = 0;i<100;i++){
            //    (*response)["players"]["sample"][i] = {{"name",string("WolverinDEV [").append(to_string(i)).append("]")},{"id","456ee69f-c907-48ee-8d71-d7ba5aa00d20"}};
            //}
            (*response)["description"] = StringUtils::replaceAll(Configuration::instance->config["ping"]["motd"].as<string>(),string("&"),string("§"));
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
    ServerInfo*  target;
    switch (packetId) {
        case 0x00:
            pconnection->setName(buffer->readString());
            cout << "Player ["<<pconnection->getName()<<"] connecting" << endl;
            if(!isSupportedVersion(pconnection->getHandshake()->getClientVersion())){
                pconnection->disconnect(new ChatMessage(string("§cNativecord dosnt support your minecraft version.")));
                return;
            }

            pconnection->writePacket(pconnection->getHandshake()->getClientVersion(), new PacketThreadshold(Configuration::instance->config["network"]["compression_threshold"].as<int>()));
            pconnection->setThreadshold(Configuration::instance->config["network"]["compression_threshold"].as<int>());

            if(pconnection->getFallbackServers().empty()){
                pconnection->disconnect(new ChatMessage(string("§cNo fallback server found.")));
                return;
            }
            target = pconnection->getFallbackServers().front();
            pconnection->removeFirstFallback();
            pconnection->connect(target);
            break;
        default:
            std::cout << "Cant handle packet (" << packetId << ") at login! Disconnecting client!" << std::endl;
            connection->disconnect(new ChatMessage("Invalid packet."));
            //TODO memory clear
            delete connection;
            break;
    }
}

void entityRewrite(int pid,DataBuffer* buffer,ClientPacketHandler* handler){
    int cversion = handler->getPlayerConnection()->getClientVersion();
    if(cversion == 210)
        EntityRewrite::entityRewrite210Client(pid, buffer, handler->getPlayerConnection()->getPlayerId(), handler->getPlayerConnection()->getCurrentTargetConnection()->getPlayerId());
    else if(cversion == 110 || cversion == 109 || cversion == 108 || cversion == 107)
        EntityRewrite::entityRewrite110Client(pid, buffer, handler->getPlayerConnection()->getPlayerId(), handler->getPlayerConnection()->getCurrentTargetConnection()->getPlayerId());
    else if(cversion == 47)
        EntityRewrite::entityRewrite47Client(pid, buffer, handler->getPlayerConnection()->getPlayerId(), handler->getPlayerConnection()->getCurrentTargetConnection()->getPlayerId());
}

void ClientPacketHandler::handlePacketPlay(int packetId, DataBuffer *buffer) {
    if(pconnection->getClientVersion() >= 107 && packetId == 0x02 || pconnection->getClientVersion() == 47 && packetId == 0x01){
        string message = buffer->readString();
        vector<string> parts = StringUtils::split(message,' ');
        cout << "Having chat message: " << message << " ("<<parts[0].c_str()<< ")" << endl;
        if(strcmp(parts[0].c_str(),"/ncord") == 0){
            pconnection->sendMessage("§aNativeCord by WolverinDEV version 0.2-ALPHA");
            return;
        }
        if(strcmp(parts[0].c_str(),"/server") == 0){
            if(parts.size() == 3){
                if(strcmp(parts[1].c_str(),"direct") == 0){ //TODO
                    string targetAdress = parts[2];
                    vector<string> aparts = StringUtils::split(targetAdress,':');
                    if(aparts.size() != 2 && aparts.size() != 1){
                        pconnection->sendMessage("§cInvalid target adress.");
                        return;
                    }
                    string host = aparts[0];
                    uint32_t port = aparts.size() == 2 ? atoi(aparts[1].c_str()) : 25565;
                    if(port < 0 || port > 65535){
                        pconnection->sendMessage("§cInvalid target port.");
                        return;
                    }
                    Socket* target = SocketUtil::createTCPSocket(host.c_str(),port);
                    if(*((int*) target) == -1){
                        pconnection->sendMessage(string("§cAn error happend while connecting. (Cant create socket)"));
                        delete(target);
                        return;
                    }
                    if(*((int*) target) == -2){
                        pconnection->sendMessage(string("§cAn error happend while connecting. (Cant resolve host)"));
                        delete(target);
                        return;
                    }
                    if(*((int*) target) == -3){
                        pconnection->sendMessage("§cAn error happend while connecting. (Cant connect to target adress)");
                        delete(target);
                        return;
                    }
                    //pconnection->connect(target);
                    pconnection->sendMessage("§aConnecting to target server.");
                    return;
                }
            }
            else if(parts.size() == 2 && strcmp(parts[1].c_str(),"list") == 0){
                pconnection->sendMessage("§6Server list:");
                for(vector<ServerInfo*>::iterator it = ServerInfo::servers.begin();it != ServerInfo::servers.end();it++){
                    if((*it)->isVisible()){
                        ChatMessage* message = new ChatMessage();
                        message->setMessage(string(" §7- §a").append((*it)->getName()));
                        message->setHoverAction(new ActionEvent("show_text", (ChatMessage(string("§aClick to connect to server").append((*it)->getName()))).toString()));
                        message->setClickAction(new ActionEvent("run_command",string("/server ").append((*it)->getName())));
                        pconnection->sendMessage(message);
                        delete message;
                    }
                }
                return;
            }
            else if(parts.size() == 2){
                ServerInfo* info = ServerInfo::getServerInfo(parts[1]);
                if(info == NULL){
                    pconnection->sendMessage("§cCant find target server configuration.");
                    return;
                }
                pconnection->sendMessage("§aConnecting to target server.");
                pconnection->connect(info);
                return;
            }

            pconnection->sendMessage("§6/server list");
            pconnection->sendMessage("§6/server <servername>");
            pconnection->sendMessage("§6/server direct <host[:port]>");
            return;
        }
    }
    if(pconnection->getCurrentTargetConnection() != NULL){
        entityRewrite(packetId,buffer,this);
    }
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