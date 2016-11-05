//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_PACKETHANDLER_H
#define CBUNGEE_PACKETHANDLER_H

#include <pthread.h>
#include "../connection/Socket.h"
#include "StreamedDataBuffer.h"
#include "zlib.h"
#include "../connection/Connection.h"

class PacketHandler {
public:
    friend class Connection;

    PacketHandler(Connection* connection) : connection(connection){
    }

    virtual ~PacketHandler(){
    }

protected:
    virtual void handlePacket(DataBuffer* buffer){
        throw new Exception("handlePacket() -> Not implemented methode!");
    }
    virtual void handlePacketHandschake(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketStatus(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketLogin(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketPlay(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }

    Connection* connection;
};

#endif //CBUNGEE_PACKETHANDLER_H
