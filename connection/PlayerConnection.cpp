//
// Created by wolverindev on 23.08.16.
//

#include "PlayerConnection.h"

vector<PlayerConnection*> PlayerConnection::connections = vector<PlayerConnection*>(); //activeConnections
vector<PlayerConnection*> PlayerConnection::activeConnections = vector<PlayerConnection*>();

PlayerConnection::~PlayerConnection(){
    delete (this->handshake);
    delete (this->currentTargetConnection);
    delete (this->tabManager);
    delete (this->scoreManager);
    delete (this->packetHandler);
    delete (this->adress);
    for(std::vector<ServerConnection*>::iterator it = this->pendingConnections.begin(); it != this->pendingConnections.end(); ++it) {
        if ((*it)->getState() == ConnectionState::CLOSED) {
            //delete *it;
            continue;
        }
        if(*it != NULL)
            (*it)->disconnect(nullptr);
        delete *it;
    }
    pendingConnections.clear();
    cout << "~PlayerConnection" << endl;
}

void PlayerConnection::disconnect(ChatMessage* message) {
    if(message != NULL) {
        if (getState() == LOGIN) {
            DataBuffer *buffer = new DataBuffer();
            buffer->writeVarInt(0x00);
            buffer->writeString(message->toString());
            writePacket(buffer);
            delete (buffer);
        } else if (getState() == PLAYING) {
            writePacket(getClientVersion(), new PacketPlayDisconnect(message));
        }
        delete message;
    }
    closeChannel();
}

ServerConnection *PlayerConnection::getCurrentTargetConnection() const {
    return currentTargetConnection;
}

void PlayerConnection::setCurrentTargetConnection(ServerConnection *currentTargetConnection) {
    PlayerConnection::currentTargetConnection = currentTargetConnection;
}

const string &PlayerConnection::getName() const {
    return name;
}

void PlayerConnection::setName(const string &name) {
    PlayerConnection::name = name;
}

void PlayerConnection::sendMessage(string message) {
    ChatMessage* message1 = new ChatMessage(message);
    sendMessage(message1);
    delete message1;
}
void PlayerConnection::sendMessage(ChatMessage* message) {
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(getClientVersion() == 46 ? 0x02 : 0x0F);
    buffer->writeString(message->toString());
    buffer->write(0);
    writePacket(buffer);
    delete buffer;
}

void* connectMethode(void* parm){
    ServerConnection* cparms = (ServerConnection*) parm;
    cparms->setSocket(cparms->getServerInfo()->createSocket()); //May by take some seconds to connect
    if(!cparms->isSocketValid()){
        if(cparms->getPlayerConnection()->getState() == ConnectionState::LOGIN){
            cparms->getPlayerConnection()->connectToNextFallback();
        }
        else {
            if(cparms->getPlayerConnection()->getCurrentTargetConnection() == NULL || cparms->getPlayerConnection()->getCurrentTargetConnection()->getState() == ConnectionState::CLOSED){
                if(cparms->getPlayerConnection()->getPendingConnection().empty())
                    cparms->getPlayerConnection()->disconnect(new ChatMessage("§fNative-Proxy:\n§7Cant connect to target server and your last server kicked you."));
                else
                    cparms->getPlayerConnection()->sendDimswitch();
                return nullptr;
            }
            else
                cparms->getPlayerConnection()->sendMessage("§c§l» §7Cant connect to target server.");
        }
        cparms->getPacketHandler()->removeFromPending();
        return nullptr;
    }
    cparms->startConnect();
    return nullptr;
}

void PlayerConnection::connect(ServerInfo *target, bool sync) {
    for(vector<ServerConnection*>::iterator it = pendingConnections.begin();it != pendingConnections.end();it++){
        if((*it)->getServerInfo()->getName().compare(target->getName()) == 0){
            sendMessage("§c§l» §7You alredy connecting to this server.");
            return;
        }
    }
    if(getCurrentTargetConnection() != NULL && getCurrentTargetConnection()->getState() != ConnectionState::CLOSED)
        if(target->getName().compare(getCurrentTargetConnection()->getServerInfo()->getName()) == 0){
            sendMessage("§c§l» §7You are alredy connected to the server.");
            return;
        }
    ServerConnection* c = new ServerConnection(this, target, false);
    getPendingConnection().push_back(c);
    pthread_t threadHandle;
    pthread_create(&threadHandle,NULL,connectMethode,(void*) c);
    if(sync)
        pthread_join(threadHandle, NULL); //Make sync
}

void* runlater(void* conn){
    usleep(100*1000);
    PlayerConnection* pconn = (PlayerConnection*) conn;
    delete pconn;
}

void PlayerConnection::closeChannel() {
    Connection::closeChannel();
    PlayerConnection::connections.erase(std::find(PlayerConnection::connections.begin(),PlayerConnection::connections.end(), this));
    vector<PlayerConnection*>::iterator rit = std::find(PlayerConnection::activeConnections.begin(),PlayerConnection::activeConnections.end(), this);
    if(rit != PlayerConnection::activeConnections.end())
        PlayerConnection::activeConnections.erase(rit);
    pthread_t  thread;
    pthread_create(&thread,NULL,&runlater,this);
}

const vector<ServerInfo *> PlayerConnection::getFallbackServers() const {
    return fallbackServers;
}
