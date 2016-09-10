//
// Created by wolverindev on 24.08.16.
//

#include "ServerConnection.h"

#include "player/PlayerConnection.h"
#include <arpa/inet.h>

void ServerConnection::startConnect() {
    ((ServerPacketHandler*)phandler)->startReader();
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
        cout << "Passing: " << out.str() << endl;
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
