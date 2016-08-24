//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_PACKETS_H
#define CBUNGEE_PACKETS_H

#include "../Buffers/DataBuffer.h"
#include <string>

class Packet {
public:
    Packet(){

    }
    virtual void read(int clientVersion, DataBuffer* buffer){

    }
    virtual void write(int clientVersion, DataBuffer* buffer){

    }
    virtual int getPacketId(int clientVersion){
        return -1;
    }
};



class PacketHandshake : public Packet{
public:
    PacketHandshake(){}
    PacketHandshake(int clientVersion, const string &host, uint16_t port, int state) : clientVersion(clientVersion), host(host), port(port), state(state) {}

    void read(int clientVersion,DataBuffer* buffer) override {
        this->clientVersion = buffer->readVarInt();
        host = buffer->readString();
        port = buffer->readShort();
        state = buffer->readVarInt();
    }
    void write(int clientVersion, DataBuffer* buffer) override {
        buffer->writeVarInt(this->clientVersion);
        buffer->writeString(host);
        buffer->writeShort(port);
        buffer->writeVarInt(state);
    }

    int getPacketId(int clientVersion) override {
        return 0x00;
    }

    int getClientVersion() const {
        return clientVersion;
    }

    void setClientVersion(int clientVersion) {
        PacketHandshake::clientVersion = clientVersion;
    }

    const string &getHost() const {
        return host;
    }

    void setHost(const string &host) {
        PacketHandshake::host = host;
    }

    uint16_t getPort() const {
        return port;
    }

    void setPort(uint16_t port) {
        PacketHandshake::port = port;
    }

    int getState() const {
        return state;
    }

    void setState(int state) {
        PacketHandshake::state = state;
    }


private:
    int clientVersion = 0;
    string host = string("");
    uint16_t port = 0;
    int state = 0;
};

class PacketRespawn : public Packet {
public:
    PacketRespawn() : Packet() {

    }

    PacketRespawn(uint16_t dimension, char difficulty, char gamemode, string level) : dimension(dimension),
                                                                                             difficulty(difficulty),
                                                                                             gamemode(gamemode),
                                                                                             level(level) {}

    virtual void read(int clientVersion, DataBuffer *buffer) override {
        dimension = buffer->readInt();
        difficulty = buffer->read();
        gamemode = buffer->read();
        level = buffer->readString();
    }

    virtual void write(int clientVersion, DataBuffer *buffer) override {
        buffer->writeInt(dimension);
        buffer->write(difficulty);
        buffer->write(gamemode);
        buffer->writeString(level);
    }

    virtual int getPacketId(int clientVersion) override {
        return clientVersion > 46 ? 0x33 : 0x07;
    }

private:
    uint16_t dimension;
    char difficulty;
    char gamemode;
    string level;
};

#endif //CBUNGEE_PACKETS_H
