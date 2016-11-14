//
// Created by wolverindev on 24.08.16.
//

#include "../../include/connection/ServerConnection.h"
#include "../../include/connection/PlayerConnection.h"
#include <arpa/inet.h>

void ServerConnection::startConnect() {
    startReaderTask();
    if(Configuration::instance->config["network"]["ip_uuid_forward"].as<bool>()){
        //https://github.com/SpigotMC/BungeeCord/blob/master/proxy/src/main/java/net/md_5/bungee/ServerConnector.java#L91-L97
        PacketHandshake* handshake = new PacketHandshake();
        handshake->setState(getPlayerConnection()->getHandshake()->getState());
        handshake->setClientVersion(getPlayerConnection()->getHandshake()->getClientVersion());
        handshake->setPort(getPlayerConnection()->getHandshake()->getPort());

        stringstream out;
        out << handshake->getHost() << '\00';
        char* address;
        if(getPlayerConnection()->getAdress() != NULL)
            address = inet_ntoa(getPlayerConnection()->getAdress()->sin_addr);
        else
            address = "localhost";
        out << address << '\00';
        out << getPlayerConnection()->getProfile()->getUUIDRaw();
        if(getPlayerConnection()->getProfile()->hasTextures()){
            out << '\00';
            out << getPlayerConnection()->getProfile()->getProperties();
        }
        handshake->setHost(out.str());
        writePacket(getPlayerConnection()->getClientVersion(), handshake); //handshake
    }
    else
        writePacket(getPlayerConnection()->getClientVersion(), getPlayerConnection()->getHandshake(), false); //handshake


    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(0x00);
    buffer->writeString(getPlayerConnection()->getName());
    setState(ConnectionState::LOGIN);
    writePacket(buffer);
    delete(buffer);
}

ServerInfo *ServerConnection::getServerInfo() const {
    return serverInfo;
}

void ServerConnection::handleConnectionClosed() {
    getPlayerConnection()->setCurrentTargetConnection(nullptr);
    if(getPlayerConnection()->getState() == ConnectionState::LOGIN) {
        getPlayerConnection()->connectToNextFallback();
        return;
    }
    if(getPlayerConnection()->getCurrentTargetConnection() == nullptr || getPlayerConnection()->getCurrentTargetConnection()->getState() != ConnectionState::CLOSED){
        if(getPlayerConnection()->getFallbackServers().empty())
            getPlayerConnection()->disconnect(new ChatMessage("§fNative-Proxy:\n§cCant connect to target server."));
        else
        {
            getPlayerConnection()->connectToNextFallback();
        }
        return;
    }
}

void ServerConnection::handleException(Exception *ex) {
    if(getState() == ConnectionState::CLOSED || getState() == ConnectionState::CLOSED)
        return;
    if(getState() == ConnectionState::LOGIN){
        if(getPlayerConnection()->getFallbackServers().empty())
            getPlayerConnection()->disconnect(new ChatMessage(string("§fNative-Proxy:\n§cAn exception was thrown.\n§l» §7Message: §5")+ex->what()));
    } else {
        getPlayerConnection()->sendMessage(string("§c§l» §7An exception was thrown.\n§6§l» §7Message: §f")+ex->what());
    }
}

void ServerConnection::handlePacket(DataBuffer *data) {
    this->phandler->handlePacket(data);
}

void ServerConnection::closeChannel() {
    if(this->state == ConnectionState::CLOSED)
        return;
    Connection::closeChannel();
    handleConnectionClosed();
}
