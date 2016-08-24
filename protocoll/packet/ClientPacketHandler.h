//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_CLIENTPACKETHANDLER_H
#define CBUNGEE_CLIENTPACKETHANDLER_H


#include "../../connection/PlayerConnection.h"
#include "PacketHandler.h"

class ClientPacketHandler : public PacketHandler {
public:
    ClientPacketHandler(PlayerConnection* connection) : PacketHandler(connection){
        this->pconnection = connection;
    }

    PlayerConnection* getPlayerConnection(){
        return pconnection;
    }

private:
    void forwardPacket(DataBuffer* buffer);

    void handlePacket(DataBuffer* buffer);
    void handlePacketHandschake(int packetId,DataBuffer* buffer);
    void handlePacketStatus(int packetId,DataBuffer* buffer);
    void handlePacketLogin(int packetId,DataBuffer *buffer);
    void handlePacketPlay(int packetId,DataBuffer *buffer);

protected:
    virtual void streamClosed() override;

private:
    PlayerConnection* pconnection;
};


#endif //CBUNGEE_CLIENTPACKETHANDLER_H
