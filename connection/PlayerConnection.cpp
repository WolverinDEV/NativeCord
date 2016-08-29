//
// Created by wolverindev on 23.08.16.
//

#include "PlayerConnection.h"

vector<PlayerConnection*> PlayerConnection::connections = vector<PlayerConnection*>(); //activeConnections
vector<PlayerConnection*> PlayerConnection::activeConnections = vector<PlayerConnection*>();

PlayerConnection::~PlayerConnection(){
    delete (this->getSocket());
    delete (this->handshake);
    delete (this->currentTargetConnection);
    delete (this->tabManager);
    delete (this->scoreManager);
    delete (this->packetHandler);
    delete (this->adress);
    for(std::vector<ServerConnection*>::iterator it = this->pendingConnections.begin(); it != this->pendingConnections.end(); ++it) {
        (*it)->disconnect(NULL);
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
    sendMessage(new ChatMessage(message));
}
void PlayerConnection::sendMessage(ChatMessage* message) {
    DataBuffer* buffer = new DataBuffer();
    buffer->writeVarInt(getClientVersion() == 46 ? 0x02 : 0x0F);
    buffer->writeString(message->toString());
    buffer->writeVarInt(0);
    writePacket(buffer);
    delete(buffer);
}

void PlayerConnection::connect(Socket *target) {
    ServerConnection* c = new ServerConnection(this,target);
    pendingConnections.push_back(c);
    cout << "Add: " << c << endl;
    c->startConnect();
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
