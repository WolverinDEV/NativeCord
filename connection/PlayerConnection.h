//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_PLAYERCONNECTION_H
#define CBUNGEE_PLAYERCONNECTION_H


#include "../protocoll/Buffers/StreamedDataBuffer.h"
#include "Socket.h"
#include "../protocoll/packet/Packets.h"
#include "../chat/ChatMessage.h"
#include "ConnectionState.h"
#include "Connection.h"

class PlayerConnection : public Connection{
public:
    PlayerConnection(Socket* socket) : Connection(socket){

    }
    ~PlayerConnection();

    void disconnect(ChatMessage* message);
    PacketHandshake *getHandshake() const {
        return handshake;
    }

    void setHandshake(PacketHandshake *handshake) {
        PlayerConnection::handshake = handshake;
    }

private:
    PacketHandshake* handshake;
};


#endif //CBUNGEE_PLAYERCONNECTION_H
