//
// Created by wolverindev on 23.08.16.
//

#include "PlayerConnection.h"

PlayerConnection::~PlayerConnection(){
    delete (this->getSocket());
}

void PlayerConnection::disconnect(ChatMessage* message) {
    if(message != NULL)
        if(getState() == LOGIN){
            DataBuffer* buffer = new DataBuffer();
            buffer->writeVarInt(0x00);
            buffer->writeString(message->toString());
            writePacket(buffer);
            delete(buffer);
        }else if(getState() == PLAYING){
            //TODO
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