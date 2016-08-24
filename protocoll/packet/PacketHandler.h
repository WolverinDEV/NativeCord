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
        while (1) {
            try {
                if(handler->connection == NULL){
                    cout << "Invelid ptr";
                    return;
                }
                if(handler->connection->getStream() == NULL){
                    cout << "Invalid stream" << endl;
                    return;
                }
                if(handler->connection->getState() == ConnectionState::CLOSED){
                    cout << "Closed channel" << endl;
                    return;
                }
                int packetLength = handler->connection->getStream()->readVarInt();
                //cout << "Having packetr with length " << packetLength << endl;
                DataBuffer *buffer = handler->connection->getStream()->readBuffer(packetLength);
                if(handler->connection->getThreadshold() != -1){
                    //cout << "Decompile packet!" << endl;
                    ulong outlength = buffer->readVarInt();
                    //cout << "Packet decompiled length: " << outlength << endl;
                    if(outlength > 0) {
                        DataBuffer* out = new DataBuffer(outlength);
                        int state = uncompress((Bytef *) out->getBuffer(),
                                               (ulong *) &outlength,
                                               (Bytef *) buffer->getBuffer()+buffer->getReaderindex(),
                                               (ulong  ) buffer->getWriterindex());
                        switch (state) {
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
                //cout << "Having packet: Packet length: " << packetLength << " PacketID: ";
                //cout << buffer->readVarInt() << endl;
                //buffer->setReaderindex(buffer->getReaderindex()-1);
                handler->handlePacket(buffer);
                delete  buffer; //Memory cleanup
            } catch (Exception *ex) {
                if (dynamic_cast<StreamClosedException *>(ex) != NULL) {
                    cout << "Connection closed!" << endl;
                }
                cout << "Client reader exception: " << ex->what() << endl;
                break;
            }
        }
    }

    PacketHandler(Connection* connection) : connection(connection){
    }

    virtual ~PacketHandler(){
        pthread_exit(&threadHandle);
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

    pthread_t threadHandle;
    Connection* connection;
};

#endif //CBUNGEE_PACKETHANDLER_H