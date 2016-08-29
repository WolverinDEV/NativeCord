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
#include "../utils/SocketUtil.h"
#include "ServerConnection.h"
#include "../server/TabManager.h"
#include "../server/ScoreboardManager.h"
#include "../protocoll/packet/ClientPacketHandler.h"
#include <vector>

class PlayerConnection : public Connection {
    public:
        static vector<PlayerConnection *> connections;
        static vector<PlayerConnection *> activeConnections;

        PlayerConnection(sockaddr_in* adress, Socket *socket) : Connection(socket), adress(adress) {
            PlayerConnection::connections.push_back(this);
        }

        ~PlayerConnection();

        void disconnect(ChatMessage *message);

        PacketHandshake *getHandshake() const {
            return handshake;
        }

        void setHandshake(PacketHandshake *handshake) {
            PlayerConnection::handshake = handshake;
        }

        void sendMessage(string message);

        void sendMessage(ChatMessage *message);

        void connect(ServerInfo *target,bool sync = false);

        ServerConnection *getCurrentTargetConnection() const;

        void setCurrentTargetConnection(ServerConnection *currentTargetConnection);

        int getClientVersion() {
            if (nullptr == handshake) {
                cout << "Handshake null?" << endl;
                return -1;
            }
            return handshake->getClientVersion();
        }

        const string &getName() const;

        void setName(const string &name);

        int getPlayerId() const {
            return playerId;
        }

        void setPlayerId(int playerId) {
            PlayerConnection::playerId = playerId;
        }

        vector<ServerConnection *> getPendingConnection() {
            return pendingConnections;
        }

        TabManager *getTabManager() {
            return tabManager;
        }

        ScoreboardManager *getScoreboardManager() {
            return scoreManager;
        }

        virtual void closeChannel() override;

        void start(){
            packetHandler->startReader();
        }

        const vector<ServerInfo *> getFallbackServers() const;

        void resetFallbackQueue(){
            fallbackServers.clear();
            fallbackServers = ServerInfo::buildDefaultServerQueue();
        }

        void removeFirstFallback(){
            fallbackServers.erase(fallbackServers.begin());
        }

        void connectToNextFallback(){
            if (getFallbackServers().empty()) {
                disconnect(new ChatMessage(string("§cNo fallback server found.")));
                return;
            }
            ServerInfo* target = getFallbackServers().front();
            removeFirstFallback();
            connect(target);
        }

        bool isInDimswitch(){
            return dimswitch;
        }

        void sendDimswitch(){
            if(dimswitch)
                return;
            dimswitch = true;
            writePacket(getClientVersion() ,new PacketPlayRespawn(1,0,0,string("default")));
        }

        void setDimswitch(bool flag){
            dimswitch = flag;
        }
    private:
        bool dimswitch = false;
        int playerId = -1;
        string name;
        sockaddr_in* adress;
        ClientPacketHandler* packetHandler = new ClientPacketHandler(this);
        PacketHandshake *handshake = nullptr;
        ServerConnection *currentTargetConnection = nullptr;
        vector<ServerConnection *> pendingConnections;
        TabManager *tabManager = new TabManager(this);
        ScoreboardManager *scoreManager = new ScoreboardManager(this);
        vector<ServerInfo*> fallbackServers = ServerInfo::buildDefaultServerQueue();
};


#endif //CBUNGEE_PLAYERCONNECTION_H
