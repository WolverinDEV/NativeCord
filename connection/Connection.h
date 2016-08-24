//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_CONNECTION_H
#define CBUNGEE_CONNECTION_H

#include "Socket.h"
#include "../protocoll/Buffers/StreamedDataBuffer.h"
#include "../protocoll/packet/Packets.h"
#include "../chat/ChatMessage.h"
#include "ConnectionState.h"
#include <zlib.h>

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
        writePacket(clientVersion,packetData,true);
    }

    void writePacket(int clientVersion, Packet* packetData,bool del){
        DataBuffer* buffer = new DataBuffer();
        buffer->writeVarInt(packetData->getPacketId(clientVersion));
        packetData->write(clientVersion,buffer);
        writePacket(buffer);
        if(del)
            delete packetData;
        delete buffer;
    }

    void disconnect(ChatMessage* message) {
        closeChannel();
    };

    void writePacket(DataBuffer* packetData){
        if(threadshold != -1){
            if(packetData->getWriterindex() > threadshold){
                DataBuffer* target = new DataBuffer(compressBound(packetData->getWriterindex()));
                ulong  size = target->getWriterindex();
                int state = compress((Bytef *) target->getBuffer(),&size,(Bytef *) packetData->getBuffer(),packetData->getWriterindex());
                switch (state) {
                    case Z_OK:
                        cout << "Decompressed okey" << endl;
                        break;
                    case Z_BUF_ERROR:
                        cout << "Buffer error" << endl;
                        break;
                    case Z_DATA_ERROR:
                        cout << "Invalid data!" << endl;
                        break;
                    default:
                        cout << "Cant find state " << state << endl;
                        break;
                }
                stream->writeVarInt(DataBuffer::getVarIntSize(packetData->getWriterindex())+target->getBufferLength()); //Write data of full packet
                stream->writeVarInt(packetData->getWriterindex()); //Size of uncompressed packet
                stream->write(target->getBuffer(),target->getBufferLength());
                delete(target);
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
        socket->closeSocket();
    }

private:
    Socket* socket;
    StreamedDataBuffer* stream;
    ConnectionState state = ConnectionState ::HANDSHAKING;
    int threadshold = -1;
};

#endif //CBUNGEE_CONNECTION_H
