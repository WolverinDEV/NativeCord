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
        Connection(Socket *socket) : socket(socket), stream(socket != nullptr ? new StreamedDataBuffer(socket) : nullptr) {

        }

        virtual ~Connection() {
            delete (stream);
            delete (socket);
        }

        void setSocket(Socket* socket){
            if(nullptr != this->socket)
                throw new Exception("Cant init socket twice!");
            this->socket = socket;
            this->stream = new StreamedDataBuffer(socket);
        }

        virtual Socket *getSocket() {
            return socket;
        }

        StreamedDataBuffer *getStream() {
            return stream;
        }

        ConnectionState getState() const {
            return state;
        }

        void setState(ConnectionState state) {
            this->state = state;
        }

        int getThreadshold() const {
            return threadshold;
        }

        void setThreadshold(int threadshold) {
            Connection::threadshold = threadshold;
        }

        void writePacket(int clientVersion, Packet *packetData, bool del = true) {
            DataBuffer *buffer = new DataBuffer();
            buffer->writeVarInt(packetData->getPacketId(clientVersion));
            packetData->write(clientVersion, buffer);
            writePacket(buffer);
            if (del)
                delete packetData;
            delete buffer;
        }

        void disconnect(ChatMessage *message) {
            closeChannel();
            if(message)
                delete message;
        };

        void writePacket(DataBuffer *packetData) {
            pthread_mutex_lock(&mutex);
            if(getState() == ConnectionState::CLOSED)
                return;
            try{
                cout << "Write -> [threadshold=" << threadshold << ", packetData->getWriterindex()=" << packetData->getWriterindex() << endl;
                if (threadshold != -1) {
                    if (packetData->getWriterindex() > threadshold) {
                        uLong compSize = compressBound(packetData->getWriterindex());
                        DataBuffer *target = new DataBuffer(compSize);
                        int state = compress((Bytef *) target->getBuffer(), &compSize, (Bytef *) packetData->getBuffer(), packetData->getWriterindex());
                        switch (state) {
                            case Z_OK:
                                //cout << "Compressed okey" << endl;
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
                        stream->writeVarInt(DataBuffer::getVarIntSize(packetData->getWriterindex()) + target->getBufferLength()); //Write data of full packet
                        stream->writeVarInt(packetData->getWriterindex()); //Size of uncompressed packet
                        stream->write(target->getBuffer(), target->getBufferLength());
                        delete target;
                    } else {
                        stream->writeVarInt(packetData->getWriterindex() + 1);
                        stream->writeVarInt(0);
                        stream->write(packetData->getBuffer(), packetData->getWriterindex());
                    }
                } else {
                    stream->writeVarInt(packetData->getWriterindex());
                    stream->write(packetData->getBuffer(), packetData->getWriterindex());
                }
            }catch(Exception* ex){
                cout << "Having exception on write: " << ex->what() << endl;
                closeChannel();
                delete ex;
            }
            pthread_mutex_unlock(&mutex);
        }

        virtual void closeChannel() {
            if(socket != nullptr)
                socket->closeSocket();
            state = ConnectionState::CLOSED;
        }

    private:
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        Socket *socket = nullptr;
        StreamedDataBuffer *stream = nullptr;
        ConnectionState state = ConnectionState::HANDSHAKING;
        int threadshold = -1;
};

#endif //CBUNGEE_CONNECTION_H
