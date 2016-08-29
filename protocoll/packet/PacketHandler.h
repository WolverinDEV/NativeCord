//
// Created by wolverindev on 24.08.16.
//

#ifndef CBUNGEE_PACKETHANDLER_H
#define CBUNGEE_PACKETHANDLER_H

#include <pthread.h>
#include "../../connection/Socket.h"
#include "../Buffers/StreamedDataBuffer.h"
#include "zlib.h"
#include "../../connection/Connection.h"

class PacketHandler {
public:
    static void readerTask(void* handlerPtr){ //TODO dont decompress full packet. First only to the packetID
        PacketHandler *handler = (PacketHandler *) handlerPtr;
        handler->running = true;
        DataBuffer *buffer;
        while (1) {
            try {
                if(handler->connection == NULL){
                    cout << "Invelid ptr";
                    break;
                }
                if(handler->connection->getStream() == NULL){
                    cout << "Invalid stream" << endl;
                    break;
                }
                if(handler->connection->getState() == ConnectionState::CLOSED){
                    cout << "Closed channel" << endl;
                    break;
                }
                int packetLength = handler->connection->getStream()->readVarInt();
                buffer = handler->connection->getStream()->readBuffer(packetLength);
                if(handler->connection->getThreadshold() != -1){
                    ulong outlength = buffer->readVarInt();
                    if(outlength > 0) {
                        DataBuffer* out = new DataBuffer(outlength);
                        int state = uncompress((Bytef *) out->getBuffer(),
                                               (ulong *) &outlength,
                                               (Bytef *) buffer->getBuffer()+buffer->getReaderindex(),
                                               (ulong  ) buffer->getWriterindex());
                        switch (state) { //TODO error handling!
                            case Z_OK:
                                //cout << "Decompressed okey" << endl;
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
            } catch (Exception *ex) {
                if(nullptr != buffer)
                    delete buffer;
                if (dynamic_cast<StreamClosedException *>(ex) != NULL) {
                    cout << "Connection closed!" << endl;
                }
                cout << "Client reader exception: " << ex->what() << endl;
                handler->onException(ex);
                if(handler->connection->getState() != ConnectionState::CLOSED)
                    handler->connection->closeChannel();
                delete ex;
                break;
            }
        }
        handler->streamClosed();
        handler->running = false;
    }

    PacketHandler(Connection* connection) : connection(connection){
    }

    virtual ~PacketHandler(){
        pthread_cancel(threadHandle);
        if(running)
            pthread_join(threadHandle, NULL);
        else
            cout << "Thread stopped" << endl;
        //pthread_kill(threadHandle,SIGQUIT);
    }

    void startReader(){
        pthread_create(&threadHandle, NULL,(void* (*)(void*))&readerTask,(void*) this);
    }

    pthread_t getThreadHandle(){
        return threadHandle;
    }

protected:
    virtual void handlePacket(DataBuffer* buffer){
        throw new Exception("handlePacket() -> Not implemented methode!");
    }
    virtual void handlePacketHandschake(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketStatus(int packetId,DataBuffer* buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketLogin(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void handlePacketPlay(int packetId,DataBuffer *buffer){
        throw new Exception("Not implemented methode!");
    }
    virtual void streamClosed(){}
    virtual void onException(Exception* ex){}

        bool running;
    pthread_t threadHandle;
    Connection* connection;
};

#endif //CBUNGEE_PACKETHANDLER_H
