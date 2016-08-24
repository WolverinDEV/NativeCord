//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SERVERCONNECTION_H
#define CBUNGEE_SERVERCONNECTION_H


#include "Socket.h"
#include "Connection.h"
#include "../protocoll/packet/PacketHandler.h"
#include "../chat/ChatMessage.h"
#include "../protocoll/packet/ServerPacketHandler.h"

class PlayerConnection;
class ServerConnection : public Connection{
public:
    ServerConnection(PlayerConnection* player,Socket* socket) : Connection(socket), player(player){
        phandler = new ServerPacketHandler(this);
    }

    virtual  ~ServerConnection(){
    }

    void startConnect();

    PlayerConnection *getPlayerConnection() const {
        return player;
    }

private:
    PacketHandler* phandler;
    PlayerConnection* player;
};

#endif //CBUNGEE_SERVERCONNECTION_H
