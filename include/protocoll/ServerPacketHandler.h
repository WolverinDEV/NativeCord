//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_SERVERPACKETHANDLER_H
#define CBUNGEE_SERVERPACKETHANDLER_H


#include "PacketHandler.h"

class ServerConnection;
class PlayerConnection;
class ServerPacketHandler : public PacketHandler {
        friend class PlayerConnection;
    public:
        ServerPacketHandler(ServerConnection *connection) : PacketHandler((Connection *) connection), sconnection(connection) {
        }

        void removeFromPending();

    private:
    protected:
        virtual void onException(Exception *ex) override;

    private:
    protected:
        virtual void streamClosed() override;

    private:

        void forwardPacket(DataBuffer *buffer);

        void handlePacket(DataBuffer *buffer);

        void handlePacketHandschake(int packetId, DataBuffer *buffer) override;

        void handlePacketStatus(int packetId, DataBuffer *buffer) override;

        void handlePacketLogin(int packetId, DataBuffer *buffer) override;

        void handlePacketPlay(int packetId, DataBuffer *buffer) override;

        ServerConnection *sconnection;
};


#endif //CBUNGEE_SERVERPACKETHANDLER_H
