//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_PLAYERCONNECTION_H
#define CBUNGEE_PLAYERCONNECTION_H


#include "../protocoll/StreamedDataBuffer.h"
#include "Socket.h"
#include "../protocoll/Packets.h"
#include "../chat/ChatMessage.h"
#include "ConnectionState.h"
#include "Connection.h"
#include "../utils/SocketUtil.h"
#include "ServerConnection.h"
#include "../server/TabManager.h"
#include "../server/ScoreboardManager.h"
#include "../protocoll/ClientPacketHandler.h"
#include "GameProfile.h"
#include "../plugin/event/EventType.h"
#include "../plugin/event/EventHelper.h"
#include <vector>
#include <jni.h>

class PlayerConnection : public Connection {
    public:
        friend class ServerConnection;
        friend class ServerPacketHandler;

        static vector<PlayerConnection *> connections;
        static vector<PlayerConnection *> activeConnections;

        PlayerConnection(sockaddr_in* adress, Socket *socket);

        ~PlayerConnection();

        void disconnect(ChatMessage *message, bool deleteMessage = true);

        PacketHandshake *getHandshake() const {
            return handshake;
        }

        void setHandshake(PacketHandshake *handshake) {
            PlayerConnection::handshake = handshake;
        }

        virtual void setState(ConnectionState state) override;

        void sendMessage(string message);

        void sendMessage(ChatMessage *message);

        void connect(ServerInfo *target,bool sync = false);

        ServerConnection *getCurrentTargetConnection() const;

        int getClientVersion() {
            if (nullptr == handshake) {
                cout << "Handshake null?" << endl;
                return -1;
            }
            return handshake->getClientVersion();
        }

        string &getName();

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
        virtual void closeChannel(bool later);

        void start(){
            startReaderTask();
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
            writePacket(getClientVersion() , new PacketPlayRespawn(1,0,0,string("default")));
        }

        void setDimswitch(bool flag){
            dimswitch = flag;
        }

        string getSecret() {
            return secret;
        }

        void setSecret(const string &secret) {
            PlayerConnection::secret = secret;
        }

        string generateServerHash();

        GameProfile *getProfile() const {
            return profile;
        }

        void setProfile(GameProfile *profile) {
            PlayerConnection::profile = profile;
        }

        sockaddr_in *getAdress() const;

        int getJavaNativeAddress(){
            return nativeAddress;
        }

        jobject getJavaInstance(){
            return javaInstance;
        }

        ChatMessage *getLastDisconnectMessage() const {
            return lastDisconnectMessage;
        }

        void setLastDisconnectMessage(ChatMessage *lastDisconnectMessage) {
            PlayerConnection::lastDisconnectMessage = lastDisconnectMessage;
        }

    protected:
        virtual void handleConnectionClosed() override;

        virtual void handleException(Exception *data) override;

        virtual void handlePacket(DataBuffer *data) override;

    public:

        static jboolean NATIVE_sendPacket0(JNIEnv* env, jobject caller, jobject storage);
        static void NATIVE_disconnect0(JNIEnv* env, jobject caller, jstring message);
    private:
        void setCurrentTargetConnection(ServerConnection *currentTargetConnection);

        bool dimswitch = false;
        int playerId = -1;
        string name;
        sockaddr_in* adress;
        string secret;
        ClientPacketHandler* packetHandler = new ClientPacketHandler(this);
        PacketHandshake *handshake = nullptr;
        ServerConnection *currentTargetConnection = nullptr;
        vector<ServerConnection *> pendingConnections;
        TabManager *tabManager = new TabManager(this);
        ScoreboardManager *scoreManager = new ScoreboardManager(this);
        vector<ServerInfo*> fallbackServers = ServerInfo::buildDefaultServerQueue();
        GameProfile* profile = nullptr;

        ChatMessage* lastDisconnectMessage = nullptr;

        //Java instance
        jobject javaInstance = nullptr;
        uint64_t nativeAddress = -1;
};


#endif //CBUNGEE_PLAYERCONNECTION_H
