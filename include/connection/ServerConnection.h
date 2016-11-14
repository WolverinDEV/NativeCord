//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SERVERCONNECTION_H
#define CBUNGEE_SERVERCONNECTION_H


#include "Socket.h"
#include "Connection.h"
#include "../protocoll/PacketHandler.h"
#include "../chat/ChatMessage.h"
#include "../protocoll/ServerPacketHandler.h"
#include "../server/ServerInfo.h"

class PlayerConnection;
class ServerConnection : public Connection {
    public:
        ServerConnection(PlayerConnection *player, ServerInfo *target, bool sockCreate = true) : Connection(sockCreate ? target->createSocket() : nullptr), serverInfo(target), player(player) {
            phandler = new ServerPacketHandler(this);
        }

        bool isSocketValid() {
            int state = *((int *) getSocket());
            return state != -1 && state != -2 && state != -3;
        }

        virtual  ~ServerConnection() {
            delete phandler;
        }

        void startConnect();

        PlayerConnection *getPlayerConnection() const {
            return player;
        }

        int getPlayerId() const {
            return playerId;
        }

        void setPlayerId(int playerId) {
            ServerConnection::playerId = playerId;
        }

        ServerInfo *getServerInfo() const;

        ServerPacketHandler *getPacketHandler() {
            return (ServerPacketHandler*) phandler;
        }

        virtual void closeChannel() override;

    protected:
        virtual void handleConnectionClosed() override;

        virtual void handleException(Exception *data) override;

        virtual void handlePacket(DataBuffer *data) override;

    private:
        ServerInfo *serverInfo;
        int playerId = -1;
        ServerPacketHandler *phandler;
        PlayerConnection *player;
};

#endif //CBUNGEE_SERVERCONNECTION_H
