//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_CONNECTION_H
#define CBUNGEE_CONNECTION_H

#include <zlib.h>
#include <openssl/evp.h>
#include "Socket.h"
#include "../protocoll/StreamedDataBuffer.h"
#include "../protocoll/Packets.h"
#include "../chat/ChatMessage.h"
#include "../cryption/Cipper.h"
#include "ConnectionState.h"

class Connection {
    public:
        static void readerTask(void* handlerPtr){ //TODO dont decompress full packet. First only to the packetID
            Connection *handler = (Connection *) handlerPtr;
            DataBuffer *buffer;
            while (1) {
                try {
                    if(handler->stream == nullptr){
                        cout << "Invalid stream" << endl;
                        break;
                    }
                    if(handler->state == ConnectionState::CLOSED){
                        cout << "Closed channel" << endl;
                        break;
                    }
                    int packetLength = handler->stream->readVarInt();
                    buffer = handler->stream->readBuffer(packetLength);
                    if(buffer == nullptr){
                        break;
                    }
                    if(handler->getThreadshold() != -1){
                        ulong outlength = buffer->readVarInt();
                        if(outlength > 0) {
                            DataBuffer* out = new DataBuffer(outlength);
                            int state = uncompress((Bytef *) out->getBuffer(),
                                                   (ulong *) &outlength,
                                                   (Bytef *) buffer->getBuffer()+buffer->getReaderindex(),
                                                   (ulong  ) buffer->getWriterindex());
                            switch (state) { //TODO error handling!
                                case Z_OK:
                                    break;
                                case Z_BUF_ERROR:
                                    debugMessage("Buffer error");
                                    handler->closeChannel();
                                    break;
                                case Z_DATA_ERROR:
                                    debugMessage("Invaliduffer error");
                                    handler->closeChannel();
                                    break;
                                default:
                                    debugMessage("Invaliduffer state error "+to_string(state));
                                    handler->closeChannel();
                                    break;
                            }
                            delete buffer;
                            buffer = out;
                        }
                        else{
                            buffer->push(-DataBuffer::getVarIntSize(outlength));
                            //buffer->setReaderindex(0);
                            //buffer->setWriterindex(packetLength-DataBuffer::getVarIntSize(outlength));
                            //cout << "Buffer: " << buffer->getWriterindex() << "/" << buffer->getBufferLength() << endl;
                        }
                    }
                    int rindex = buffer->getReaderindex();
                    buffer->setReaderindex(rindex);
                    handler->handlePacket(buffer);
                    delete  buffer; //Memory cleanup
                    buffer = nullptr;
                } catch (Exception ex) {
                    if(nullptr != buffer)
                        delete buffer;
                    if (dynamic_cast<StreamClosedException *>(&ex) != NULL) {
                        cout << "Connection closed!" << endl;
                    }
                    cout << "Client reader exception: " << ex.what() << endl;
                    handler->handleException(&ex);
                    if(handler->getState() != ConnectionState::CLOSED)
                        handler->closeChannel();
                    break;
                }
            }

            delete handler->readerThread;
            handler->readerThread = nullptr;
        }

        Connection(Socket *socket) : socket(socket), stream(socket != nullptr ? new StreamedDataBuffer(socket) : nullptr) {

        }

        virtual ~Connection() {
            stopReaderTask(true);
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

        virtual void setState(ConnectionState state) {
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

        void disconnect(ChatMessage *message,bool deleteObject = true) {
            closeChannel();
            if(deleteObject)
                delete message;
        }

        void writePacket(DataBuffer *packetData) {
            pthread_mutex_lock(&mutex);
            if(getState() == ConnectionState::CLOSED)
                return;
            DataBuffer *target = nullptr;
            try{
                if (threadshold != -1) {
                    if (packetData->getWriterindex() > threadshold) {
                        uLong compSize = compressBound(packetData->getWriterindex());
                        target = new DataBuffer(compSize);
                        int state = compress((Bytef *) target->getBuffer(), &compSize, (Bytef *) packetData->getBuffer(), packetData->getWriterindex());
                        switch (state) {
                            case Z_OK:
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
                        target = nullptr;
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
            if(target != nullptr)
                delete target;
            pthread_mutex_unlock(&mutex);
        }

        virtual void closeChannel() {
            if(socket != nullptr)
                socket->closeSocket();
            state = ConnectionState::CLOSED;
        }

        void stopReaderTask(bool join = false){
            if(readerThread != nullptr){
                pthread_cancel(*readerThread);
                if(join)
                    pthread_join(*readerThread, nullptr);
                delete(readerThread);
            }
        }

        void startReaderTask(){
            if(readerThread == nullptr){
                readerThread = new pthread_t;
                pthread_create(readerThread, nullptr, &readerTask, this);
            }
        }

    protected:
        bool open = false;

        virtual void handleConnectionClosed() = 0;
        virtual void handleException(Exception* data) = 0;
        virtual void handlePacket(DataBuffer* data) = 0;
    private:
        pthread_t* readerThread = nullptr;
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        Socket *socket = nullptr;
        StreamedDataBuffer *stream = nullptr;
        ConnectionState state = ConnectionState::HANDSHAKING;
        int threadshold = -1;
};

#endif //CBUNGEE_CONNECTION_H
