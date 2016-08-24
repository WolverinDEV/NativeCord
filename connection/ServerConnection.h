//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SERVERCONNECTION_H
#define CBUNGEE_SERVERCONNECTION_H


#include "Socket.h"
#include "PlayerConnection.h"
#include "../protocoll/packet/ServerPacketHandler.h"

class ServerConnection : public Connection{
public:
    ServerConnection(Socket* socket) : Connection(socket){

    }

    virtual  ~ServerConnection(){
    }
private:
    //ServerPacketHandler* packetHandler = NULL;
};


#endif //CBUNGEE_SERVERCONNECTION_H
