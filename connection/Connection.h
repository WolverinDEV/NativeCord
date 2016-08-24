//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_CONNECTION_H
#define CBUNGEE_CONNECTION_H

#include "Socket.h"
#include "../protocoll/Buffers/StreamedDataBuffer.h"
#include "ConnectionState.h"
#include "../protocoll/packet/Packets.h"
class Connection {
public:
    Connection(Socket* socket) : socket(socket), stream(new StreamedDataBuffer(socket)){

    }
    virtual ~Connection(){
        delete(stream);
    }

    Socket* getSocket(){
        return socket;
    }
    StreamedDataBuffer* getStream(){
        return stream;
    }

    ConnectionState getState() const {
        return state;
    }

    void setState(ConnectionState state){
        this->state = state;
    }

    int getThreadshold() const {
        return threadshold;
    }

    void setThreadshold(int threadshold) {
        Connection::threadshold = threadshold;
    }

    void writePacket(int clientVersion, Packet* packetData){
        DataBuffer* buffer = new DataBuffer();
        buffer->writeVarInt(packetData->getPacketId(clientVersion));
        packetData->write(clientVersion,buffer);
        writePacket(buffer);
        delete buffer;
    }

    void writePacket(DataBuffer* packetData){
        if(threadshold != -1){
            if(packetData->getWriterindex() > threadshold){
                cout << "Cant write compressed!";
            } else {
                stream->writeVarInt(packetData->getWriterindex()+1);
                stream->writeVarInt(0);
                stream->write(packetData->getBuffer(),packetData->getWriterindex());
            }
        } else {
            stream->writeVarInt(packetData->getWriterindex());
            stream->write(packetData->getBuffer(),packetData->getWriterindex());
        }
    }
    void closeChannel(){

    }

private:
    Socket* socket;
    StreamedDataBuffer* stream;
    ConnectionState state = ConnectionState ::HANDSHAKING;
    int threadshold = -1;
};

#endif //CBUNGEE_CONNECTION_H
