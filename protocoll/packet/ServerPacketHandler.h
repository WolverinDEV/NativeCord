//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SERVERPACKETHANDLER_H
#define CBUNGEE_SERVERPACKETHANDLER_H


#include "../../connection/ServerConnection.h"
#include "PacketHandler.h"

class ServerConnection;
class ServerPacketHandler : public PacketHandler{
public:
    ServerPacketHandler(ServerConnection* connection) : PacketHandler((Connection*) connection), sconnection(connection){

    }

    void connect();
private:
    void forwardPacket(DataBuffer* buffer);

    void handlePacket(DataBuffer* buffer);
    void handlePacketHandschake(int packetId,DataBuffer* buffer);
    void handlePacketStatus(int packetId,DataBuffer* buffer);
    void handlePacketLogin(int packetId,DataBuffer *buffer);
    void handlePacketPlay(int packetId,DataBuffer *buffer);
    ServerConnection* sconnection;
};


#endif //CBUNGEE_SERVERPACKETHANDLER_H
