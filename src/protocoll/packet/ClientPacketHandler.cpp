//
// Created by wolverindev on 23.08.16.
//

#include "../../../include/protocoll/ClientPacketHandler.h"
#include "../../../include/protocoll/Packets.h"
#include "../../../include/json/json.hpp"
#include "../../../include/connection/ServerConnection.h"
#include "../../../include/connection/Connection.h"
#include "../../../include/utils/TimeUtils.h"
#include "../../../include/utils/StringUtil.h"
#include "../../../include/utils/EntityRewrite.h"
#include "../../../include/config/Configuration.h"
#include "../../../include/server/ServerInfo.h"
#include "../../../include/connection/PlayerConnection.h"
#include "../../../include/cryption/Cipper.h"
#include "../../../include/cryption/RSAUtil.h"
#include "../../../include/utils/Base64Utils.h"
#import "cpr/cpr.h"
#include "../../../include/utils/HTTPUtil.h"
#include "../../../include/plugin/event/EventHelper.h"
#include "../../../include/plugin/java/JavaPluginManagerImpl.h"
#include <typeinfo>

// for convenience
using json = nlohmann::json;

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

            if(JavaPluginManagerImpl::instance != nullptr){
                EventHelper::handleHandshake(this->pconnection, handshake);
            }
            debugMessage("Client Version: " + to_string(handshake->getClientVersion()));
            debugMessage("State:   " + to_string(handshake->getState()));
            break;
        default:
            debugMessage("Cant handle packet (" + to_string(packetId) + ") at handshake! Disconnecting client!");
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
            debugMessage("Requesting status");
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
            rbuffer->writeLong(time);
            connection->writePacket(rbuffer);
            delete rbuffer;
            break;
        default:
            debugMessage("Cant handle packet (" + to_string(packetId) + ") at status! Disconnecting client!");
            connection->getSocket()->closeSocket();
            //TODO memory clear
    }
}

void sendSuccessfullLoggedIn(PlayerConnection *pconnection){
    if(pconnection->getProfile() == nullptr){
        string offlineName = "OfflinePlayer:"+pconnection->getName();
        pconnection->setProfile(new GameProfile(pconnection->getName(), UUIDUtils::getOfflineUUID(offlineName)));
    }

    ServerInfo*  target;
    debugMessage("Player ["+pconnection->getName()+"] connecting");
    if(!isSupportedVersion(pconnection->getHandshake()->getClientVersion())){
        pconnection->disconnect(new ChatMessage(string("§fNative-Proxy:\n§cNativecord dosnt support your minecraft version.")));
        return;
    }

    pconnection->writePacket(pconnection->getHandshake()->getClientVersion(), new PacketThreadshold(Configuration::instance->config["network"]["compression_threshold"].as<int>()));
    pconnection->setThreadshold(Configuration::instance->config["network"]["compression_threshold"].as<int>());

    if(pconnection->getFallbackServers().empty()){
        pconnection->disconnect(new ChatMessage(string("§fNative-Proxy:\n§cNo fallback server found.")));
        return;
    }
    target = pconnection->getFallbackServers().front();
    pconnection->removeFirstFallback();
    pconnection->connect(target);
}

void sendLoginVerify(PlayerConnection *pconnection){
    string unencripted = RSAUtil::getPublicEncriptedKey(Cipper::publicKey)->getBase64Buffer();
    string data = base64_decode(unencripted);
    debugMessage("Send data length: " + data.length());
    debugMessage("Data: " + unencripted);
    PacketLoginEncryption* enc = new PacketLoginEncryption(Cipper::hash,(char*) data.data(),data.length(),(char*) Cipper::hash.data(),Cipper::hash.length());
    pconnection->writePacket(-1, enc);
}

void handleEncriptionResponse(PlayerConnection* connection, DataBuffer*& buffer){
    PacketLoginEncryption* pack = new PacketLoginEncryption(false);
    pack->read(connection->getClientVersion(),buffer);
    string key = Cipper::decodeMessagePrivateKey(pack->getSecret(),pack->getSecretLength(),Cipper::publicKey);
    connection->getStream()->setChupper((char*) key.data()); //Setup encription
    connection->setSecret(key);

    string hashDecripted = Cipper::decodeMessagePrivateKey(pack->getVerifyToken(),pack->getVerifyTokenLength(),Cipper::publicKey);

    if(hashDecripted.compare(Cipper::hash) != 0){
        connection->disconnect(new ChatMessage("§cInvalid verify token! §7(§bRecived token: §6"+hashDecripted+"§7)"));
        return;
    }

    auto r = cpr::GetCallback([connection](cpr::Response r) {
        debugMessage("2-Response: " + r.text);
        debugMessage("Code: " + r.status_code);
        if(r.status_code == 204){
            connection->disconnect(new ChatMessage("§cCant verify with mojang!"));
        } else{
            connection->setProfile(new GameProfile(json::parse(r.text)));
            debugMessage("Name: " + connection->getProfile()->getName());
            sendSuccessfullLoggedIn(connection);
        }
    }, cpr::Url{"https://sessionserver.mojang.com:443/session/minecraft/hasJoined"}, cpr::Parameters{{"username", connection->getName()}, {"serverId", connection->generateServerHash()}}, cpr::Header{{"User-Agent", "NativeCord"}}, cpr::Timeout{5000}, cpr::VerifySsl{false});
}

void ClientPacketHandler::handlePacketLogin(int packetId, DataBuffer *buffer) {
    switch (packetId) {
        case 0x00:
            pconnection->setName(buffer->readString());
            if(Configuration::instance->config["network"]["player_limit"].as<int>() != -1 && PlayerConnection::activeConnections.size() >= Configuration::instance->config["network"]["player_limit"].as<int>()){
                pconnection->disconnect(new ChatMessage("§cServer is full"));
                return;
            }
            if(Configuration::instance->config["network"]["online_mode"].as<bool>())
                sendLoginVerify(pconnection);
            else
                sendSuccessfullLoggedIn(pconnection);
            break;
        case 0x01:
            handleEncriptionResponse(pconnection,buffer);
            break;
        default:
            debugMessage("Cant handle packet (" + to_string(packetId) + ") at login! Disconnecting client!");
            pconnection->disconnect(new ChatMessage("Invalid packet."));
            //TODO memory clear
            //delete connection;
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
        vector<string> parts = StringUtils::split(message," ");
        debugMessage("Having chat message: " + message + " (" + parts[0] + ")");
        if(strcmp(parts[0].c_str(),"/ncord") == 0){
            pconnection->sendMessage("§5§l» §7NativeCord by WolverinDEV version 0.3-ALPHA");
            return;
        }
        if(strcmp(parts[0].c_str(),"/server") == 0){
            if(parts.size() == 3){
                if(strcmp(parts[1].c_str(),"direct") == 0){ //TODO
                    if(Configuration::instance->config["servers"]["disable_direct"]){
                        pconnection->sendMessage("§c§l> §7Direct server connecting is forbidden.");
                        return;
                    }
                    string targetAdress = parts[2];
                    vector<string> aparts = StringUtils::split(targetAdress,":");
                    if(aparts.size() != 2 && aparts.size() != 1){
                        pconnection->sendMessage("§c§l» §7Invalid target adress.");
                        return;
                    }
                    string host = aparts[0];
                    uint32_t port = aparts.size() == 2 ? atoi(aparts[1].c_str()) : 25565;
                    if(port < 0 || port > 65535){
                        pconnection->sendMessage("§c§l» §7Invalid target port.");
                        return;
                    }
                    ServerInfo* target = ServerInfo::getServerInfo(host.c_str(),port);
                    if(target == NULL){
                        target = ServerInfo::createTempServerInfo(host.append(to_string(port)), host,port);
                    }
                    pconnection->connect(target, false);

                    /*
                    Socket* target = SocketUtil::createTCPSocket(,);
                    if(*((int*) target) == -1){
                        pconnection->sendMessage(string("§c§l» §7An error happend while connecting. (Cant create socket)"));
                        delete(target);
                        return;
                    }
                    if(*((int*) target) == -2){
                        pconnection->sendMessage(string("§c§l» §7An error happend while connecting. (Cant resolve host)"));
                        delete(target);
                        return;
                    }
                    if(*((int*) target) == -3){
                        pconnection->sendMessage("§c§l» §7An error happend while connecting. (Cant connect to target adress)");
                        delete(target);
                        return;
                    }
                     */
                    //pconnection->connect(target);
                    pconnection->sendMessage("§a§l» §7Connecting to target server.");
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
                    pconnection->sendMessage("§c§l» §7Cant find target server configuration.");
                    return;
                }
                pconnection->sendMessage("§a§l» §7Connecting to target server.");
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

