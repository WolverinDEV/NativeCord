//
// Created by wolverindev on 23.08.16.
//

#ifndef CBUNGEE_PACKETS_H
#define CBUNGEE_PACKETS_H

#include "DataBuffer.h"
#include "../chat/ChatMessage.h"
#include "../utils/StringUtil.h"
#include "../log/LogUtils.h"
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


class PacketLoginEncryption : public Packet {
    public:
        PacketLoginEncryption(bool request) : Packet(), request(request) {

        }

        PacketLoginEncryption(char *secret, int secretLength, char *verifyToken, int verifyTokenLength) : PacketLoginEncryption(false) {
            this->secretLength = secretLength;
            this->verifyTokenLength = verifyTokenLength;
            this->secret = (char*) malloc(secretLength);
            this->verifyToken = (char*) malloc(verifyTokenLength);

            memcpy(this->secret,secret,secretLength);
            memcpy(this->verifyToken,verifyToken,verifyTokenLength);
        }
        PacketLoginEncryption(string serverId,char *secret, int secretLength, char *verifyToken, int verifyTokenLength) : PacketLoginEncryption(true) {
            this->secretLength = secretLength;
            this->verifyTokenLength = verifyTokenLength;
            this->secret = (char*) malloc(secretLength);
            this->verifyToken = (char*) malloc(verifyTokenLength);
            this->serverId = serverId;
            memcpy(this->secret,secret,secretLength);
            memcpy(this->verifyToken,verifyToken,verifyTokenLength);
        }

        ~PacketLoginEncryption(){
            free(secret);
            free(verifyToken);
        }

        virtual void read(int clientVersion, DataBuffer *buffer) override {
            if(!request){
                secretLength = buffer->readVarInt(); //TODO length check!
                secret = (char*) malloc(secretLength);
                buffer->read(secret, secretLength);

                verifyTokenLength = buffer->readVarInt(); //TODO length check!
                verifyToken = (char*) malloc(secretLength);
                buffer->read(verifyToken, verifyTokenLength);
            }
        }

        virtual void write(int clientVersion, DataBuffer *buffer) override {
            if(request){
                buffer->writeString(serverId);
                buffer->writeVarInt(secretLength);
                buffer->write(secret,secretLength);
                buffer->writeVarInt(verifyTokenLength);
                buffer->write(verifyToken,verifyTokenLength);
            } else throw  new Exception("Cant send an excription response!");
        }

        virtual int getPacketId(int clientVersion) override {
            return 0x01;
        }

        bool isRequest() const {
            return request;
        }

        const string &getServerId() const {
            return serverId;
        }

        char *getSecret() const {
            return secret;
        }

        int getSecretLength() const {
            return secretLength;
        }

        char *getVerifyToken() const {
            return verifyToken;
        }

        int getVerifyTokenLength() const {
            return verifyTokenLength;
        }

    private:
        bool request;
        string serverId;
        char* secret = NULL;
        int secretLength;
        char* verifyToken = NULL;
        int verifyTokenLength;
};


class PacketPlayRespawn : public Packet {
public:
    PacketPlayRespawn() : Packet() {

    }

    PacketPlayRespawn(uint16_t dimension, char difficulty, char gamemode, string level) : dimension(dimension),
                                                                                             difficulty(difficulty),
                                                                                             gamemode(gamemode),
                                                                                             level(level) {
    }

    ~PacketPlayRespawn(){
        level.clear();
    }

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
        return clientVersion > 47 ? 0x33 : 0x07;
    }

private:
    uint16_t dimension;
    char difficulty;
    char gamemode;
    string level;
};

class PacketPlayDisconnect : public  Packet {
public:
    PacketPlayDisconnect(){

    }
    PacketPlayDisconnect(ChatMessage* message) : message(message){

    }
    ~PacketPlayDisconnect(){
        if(message != nullptr && message != NULL)
            delete(message);
    }
    virtual void read(int clientVersion, DataBuffer *buffer) override {
        string rmessage = buffer->readString();
        if(rmessage.empty()){
            debugMessage("Empty disconnect message. Message: '"+rmessage+"'");
            message = nullptr;
            return;
        }
        json json = json::parse(rmessage);
        if(json == NULL)
            cout << "Cant serelize " << rmessage << endl;
        else
            message = new ChatMessage(json);
    }

    virtual void write(int clientVersion, DataBuffer *buffer) override {
        buffer->writeString(message->toString());
    }

    virtual int getPacketId(int clientVersion) override {
        return clientVersion == 47 ? 0x40 : 0x1A;
    }
        ChatMessage* getMessage(){
            return message;
        }
private:
    ChatMessage* message = nullptr;
};

class PacketThreadshold : public  Packet {
public:
    PacketThreadshold(int toggle) : toggle(toggle){

    }
    virtual void read(int clientVersion, DataBuffer *buffer) override {
        toggle = buffer->readVarInt();
    }

    virtual void write(int clientVersion, DataBuffer *buffer) override {
        buffer->writeVarInt(toggle);
    }

    virtual int getPacketId(int clientVersion) override {
        return 0x03;
    }
private:
    int toggle;
};
#endif //CBUNGEE_PACKETS_H
