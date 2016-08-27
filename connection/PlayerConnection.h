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
#include <vector>
class PlayerConnection : public Connection{
public:
    PlayerConnection(Socket* socket) : Connection(socket){
    }
    ~PlayerConnection();

    void disconnect(ChatMessage* message);
    PacketHandshake* getHandshake() const {
        return handshake;
    }

    void setHandshake(PacketHandshake *handshake) {
        PlayerConnection::handshake = handshake;
    }

    void sendMessage(string message);
    void sendMessage(ChatMessage* message);

    void connect(Socket* target);

    ServerConnection *getCurrentTargetConnection() const;

    void setCurrentTargetConnection(ServerConnection *currentTargetConnection);
    int getClientVersion(){
        if(nullptr == handshake){
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

    vector<ServerConnection*> getPendingConnection(){
        return pendingConnections;
    }

    TabManager* getTabManager(){
        return tabManager;
    }

    ScoreboardManager* getScoreboardManager(){
        return scoreManager;
    }

private:
    int playerId = -1;
    string name;
    PacketHandshake* handshake = nullptr;
    ServerConnection* currentTargetConnection = nullptr;
    vector<ServerConnection*> pendingConnections;
    TabManager* tabManager = new TabManager(this);
    ScoreboardManager* scoreManager = new ScoreboardManager(this);
};


#endif //CBUNGEE_PLAYERCONNECTION_H
